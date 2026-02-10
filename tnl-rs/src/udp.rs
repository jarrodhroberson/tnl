// tnlUDP.h conversion
use crate::types::*;
use std::net::{SocketAddr, UdpSocket, ToSocketAddrs, Ipv4Addr, IpAddr};
use std::str::FromStr;
use std::io;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum TransportProtocol {
    IPProtocol,
    TCPProtocol,
    IPXProtocol, // Not supported, placeholder
    IPv6Protocol, // Supported by Rust, but distinct here for compatibility
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum NamedAddress {
    None,
    Localhost,
    Broadcast,
    Any,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Address {
    pub transport: TransportProtocol,
    pub addr: SocketAddr,
}

impl Default for Address {
    fn default() -> Self {
        Self {
            transport: TransportProtocol::IPProtocol,
            addr: SocketAddr::new(IpAddr::V4(Ipv4Addr::UNSPECIFIED), 0),
        }
    }
}

impl Address {
    pub fn new(transport: TransportProtocol, name: NamedAddress, port: u16) -> Self {
        let ip = match name {
            NamedAddress::None | NamedAddress::Any => Ipv4Addr::UNSPECIFIED,
            NamedAddress::Localhost => Ipv4Addr::LOCALHOST,
            NamedAddress::Broadcast => Ipv4Addr::BROADCAST,
        };
        Self {
            transport,
            addr: SocketAddr::new(IpAddr::V4(ip), port),
        }
    }

    pub fn from_socket_addr(addr: SocketAddr) -> Self {
        Self {
            transport: TransportProtocol::IPProtocol, // Assume IP
            addr,
        }
    }

    pub fn set_from_string(&mut self, s: &str) -> bool {
        // Simple parsing logic matching C++ somewhat
        // "ip:127.0.0.1:8000"
        // "broadcast:8000"
        
        let s = s.trim();
        let (protocol, rest) = if s.starts_with("ip:") {
            (TransportProtocol::IPProtocol, &s[3..])
        } else if s.starts_with("tcp:") {
            (TransportProtocol::TCPProtocol, &s[4..])
        } else if s.starts_with("ipx:") {
            return false; // Not supported
        } else {
            (TransportProtocol::IPProtocol, s)
        };

        // Handle "broadcast"
        // Rust ToSocketAddrs might not handle "broadcast" keyword directly in all cases.
        if rest.starts_with("broadcast") {
             // Extract port if any
             let port_part = rest.strip_prefix("broadcast").unwrap();
             let port = if port_part.starts_with(':') {
                 port_part[1..].parse::<u16>().unwrap_or(0)
             } else {
                 0
             };
             self.transport = protocol;
             self.addr = SocketAddr::new(IpAddr::V4(Ipv4Addr::BROADCAST), port);
             return true;
        }

        if rest.starts_with("any") {
             let port_part = rest.strip_prefix("any").unwrap();
             let port = if port_part.starts_with(':') {
                 port_part[1..].parse::<u16>().unwrap_or(0)
             } else {
                 0
             };
             self.transport = protocol;
             self.addr = SocketAddr::new(IpAddr::V4(Ipv4Addr::UNSPECIFIED), port);
             return true;
        }

        if rest.starts_with("localhost") {
             let port_part = rest.strip_prefix("localhost").unwrap();
             let port = if port_part.starts_with(':') {
                 port_part[1..].parse::<u16>().unwrap_or(0)
             } else {
                 0
             };
             self.transport = protocol;
             self.addr = SocketAddr::new(IpAddr::V4(Ipv4Addr::LOCALHOST), port);
             return true;
        }

        // Standard parsing
        if let Ok(addr) = rest.parse::<SocketAddr>() {
            self.transport = protocol;
            self.addr = addr;
            return true;
        }
        
        // DNS resolution
        if let Ok(mut iter) = rest.to_socket_addrs() {
            if let Some(addr) = iter.next() {
                self.transport = protocol;
                self.addr = addr;
                return true;
            }
        }

        false
    }

    pub fn to_string(&self) -> String {
        format!("{}", self.addr)
    }
    
    pub fn is_equal_address(&self, other: &Address) -> bool {
        self.addr.ip() == other.addr.ip()
    }
    
    pub fn hash(&self) -> u32 {
        // Simple hash
        use std::hash::{Hash, Hasher};
        let mut hasher = std::collections::hash_map::DefaultHasher::new();
        self.addr.hash(&mut hasher);
        hasher.finish() as u32
    }
}

#[derive(Debug, PartialEq, Copy, Clone)]
pub enum NetError {
    NoError,
    InvalidPacketProtocol,
    WouldBlock,
    UnknownError,
    // Add Rust specific
    IoError,
}

pub struct Socket {
    socket: Option<UdpSocket>,
    transport_protocol: TransportProtocol,
}

impl Socket {
    pub fn new(bind_address: &Address, _send_buffer_size: u32, _recv_buffer_size: u32, broadcast: bool, non_blocking: bool) -> Self {
        let socket = if bind_address.transport == TransportProtocol::IPProtocol {
            // Bind UDP
            match UdpSocket::bind(bind_address.addr) {
                Ok(s) => {
                    if broadcast {
                        let _ = s.set_broadcast(true);
                    }
                    if non_blocking {
                        let _ = s.set_nonblocking(true);
                    }
                    // Buffer sizes set via socket opts if needed, Rust std doesn't expose easily, 
                    // need socket2 crate for that. We'll skip for now or use defaults.
                    Some(s)
                },
                Err(e) => {
                    crate::platform::output_debug_string(&format!("Socket bind failed: {}", e));
                    None
                }
            }
        } else {
            // TCP not implemented in this struct for now, or use socket2
            None
        };

        Self {
            socket,
            transport_protocol: bind_address.transport,
        }
    }

    pub fn is_valid(&self) -> bool {
        self.socket.is_some()
    }

    pub fn send_to(&self, address: &Address, buffer: &[u8]) -> NetError {
        if let Some(ref s) = self.socket {
            if address.transport != self.transport_protocol {
                return NetError::InvalidPacketProtocol;
            }
            match s.send_to(buffer, address.addr) {
                Ok(_) => NetError::NoError,
                Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => NetError::WouldBlock,
                Err(_) => NetError::UnknownError,
            }
        } else {
            NetError::UnknownError
        }
    }

    pub fn recv_from(&self, buffer: &mut [u8]) -> (NetError, Option<Address>, usize) {
        if let Some(ref s) = self.socket {
            match s.recv_from(buffer) {
                Ok((size, src)) => (NetError::NoError, Some(Address::from_socket_addr(src)), size),
                Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => (NetError::WouldBlock, None, 0),
                Err(_) => (NetError::UnknownError, None, 0),
            }
        } else {
            (NetError::UnknownError, None, 0)
        }
    }

    pub fn get_bound_address(&self) -> Address {
        if let Some(ref s) = self.socket {
            if let Ok(addr) = s.local_addr() {
                return Address::from_socket_addr(addr);
            }
        }
        Address::default()
    }
    
    // Static helper
    pub fn get_interface_addresses() -> Vec<Address> {
        // Listing interfaces in Rust requires external crates (like `pnet` or `if_addrs`).
        // We will just return localhost for now.
        vec![Address::from_socket_addr(SocketAddr::new(IpAddr::V4(Ipv4Addr::LOCALHOST), 0))]
    }
}
