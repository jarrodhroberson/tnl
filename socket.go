package tnl

import (
	"fmt"
	"net"
	"strconv"
	"strings"
	"syscall"
	"time"
)

// NetError represents network error types
type NetError int

const (
	NoError NetError = iota
	WouldBlock
	NotConnected
	ConnectionPending
	ConnectionRefused
	TimedOut
	NameLookupRequired
	InvalidPacketProtocol
	UnknownError
)

func (e NetError) Error() string {
	switch e {
	case NoError:
		return "no error"
	case WouldBlock:
		return "operation would block"
	case NotConnected:
		return "not connected"
	case ConnectionPending:
		return "connection pending"
	case ConnectionRefused:
		return "connection refused"
	case TimedOut:
		return "timed out"
	case NameLookupRequired:
		return "name lookup required"
	case InvalidPacketProtocol:
		return "invalid packet protocol"
	default:
		return "unknown error"
	}
}

// Address represents a network address
type Address struct {
	host net.IP
	port uint16
}

// NewAddress creates a new Address
func NewAddress(host string, port uint16) *Address {
	ip := net.ParseIP(host)
	if ip == nil {
		// Try to resolve hostname
		ips, err := net.LookupIP(host)
		if err != nil || len(ips) == 0 {
			return nil
		}
		ip = ips[0]
	}
	return &Address{host: ip, port: port}
}

// NewAddressFromString creates an Address from a string like "192.168.1.1:8080"
func NewAddressFromString(addrStr string) *Address {
	parts := strings.Split(addrStr, ":")
	if len(parts) != 2 {
		return nil
	}
	
	port, err := strconv.ParseUint(parts[1], 10, 16)
	if err != nil {
		return nil
	}
	
	return NewAddress(parts[0], uint16(port))
}

// GetHost returns the host IP
func (a *Address) GetHost() net.IP {
	return a.host
}

// GetPort returns the port
func (a *Address) GetPort() uint16 {
	return a.port
}

// ToString returns the string representation of the address
func (a *Address) ToString() string {
	return fmt.Sprintf("%s:%d", a.host.String(), a.port)
}

// String implements the Stringer interface
func (a *Address) String() string {
	return a.ToString()
}

// IsValid returns true if the address is valid
func (a *Address) IsValid() bool {
	return a.host != nil && a.port > 0
}

// Equals compares two addresses
func (a *Address) Equals(other *Address) bool {
	if other == nil {
		return false
	}
	return a.host.Equal(other.host) && a.port == other.port
}

// ToUDPAddr converts to net.UDPAddr
func (a *Address) ToUDPAddr() *net.UDPAddr {
	return &net.UDPAddr{
		IP:   a.host,
		Port: int(a.port),
	}
}

// Socket represents a network socket
type Socket interface {
	Bind(address *Address) error
	Connect(address *Address) error
	SendTo(data []byte, address *Address) error
	RecvFrom(buffer []byte) (int, *Address, error)
	SetBlocking(blocking bool) error
	SetBroadcast(broadcast bool) error
	SetReuseAddress(reuse bool) error
	Close() error
	IsValid() bool
	GetLocalAddress() *Address
}

// UDPSocket implements Socket for UDP connections
type UDPSocket struct {
	conn        *net.UDPConn
	localAddr   *Address
	isBlocking  bool
	isBroadcast bool
}

// NewUDPSocket creates a new UDP socket
func NewUDPSocket() *UDPSocket {
	return &UDPSocket{
		isBlocking: true,
	}
}

// Bind binds the socket to the specified address
func (s *UDPSocket) Bind(address *Address) error {
	if address == nil {
		return fmt.Errorf("address is nil")
	}
	
	udpAddr := address.ToUDPAddr()
	conn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		return err
	}
	
	s.conn = conn
	s.localAddr = address
	
	// Set initial socket options
	if !s.isBlocking {
		s.SetBlocking(false)
	}
	if s.isBroadcast {
		s.SetBroadcast(true)
	}
	
	return nil
}

// Connect connects the socket to the specified address (for UDP, this just sets the default destination)
func (s *UDPSocket) Connect(address *Address) error {
	if address == nil {
		return fmt.Errorf("address is nil")
	}
	
	udpAddr := address.ToUDPAddr()
	conn, err := net.DialUDP("udp", nil, udpAddr)
	if err != nil {
		return err
	}
	
	s.conn = conn
	return nil
}

// SendTo sends data to the specified address
func (s *UDPSocket) SendTo(data []byte, address *Address) error {
	if s.conn == nil {
		return fmt.Errorf("socket not initialized")
	}
	
	if address == nil {
		// Use connected address if available
		_, err := s.conn.Write(data)
		return err
	}
	
	udpAddr := address.ToUDPAddr()
	_, err := s.conn.WriteToUDP(data, udpAddr)
	return err
}

// RecvFrom receives data from the socket
func (s *UDPSocket) RecvFrom(buffer []byte) (int, *Address, error) {
	if s.conn == nil {
		return 0, nil, fmt.Errorf("socket not initialized")
	}
	
	n, addr, err := s.conn.ReadFromUDP(buffer)
	if err != nil {
		// Convert Go errors to TNL NetError types
		if netErr, ok := err.(net.Error); ok {
			if netErr.Timeout() {
				return 0, nil, TimedOut
			}
			if netErr.Temporary() {
				return 0, nil, WouldBlock
			}
		}
		return 0, nil, err
	}
	
	address := &Address{
		host: addr.IP,
		port: uint16(addr.Port),
	}
	
	return n, address, nil
}

// SetBlocking sets the blocking mode of the socket
func (s *UDPSocket) SetBlocking(blocking bool) error {
	s.isBlocking = blocking
	
	if s.conn == nil {
		return nil // Will be applied when socket is created
	}
	
	// Get the underlying file descriptor
	if udpConn, ok := s.conn.(*net.UDPConn); ok {
		file, err := udpConn.File()
		if err != nil {
			return err
		}
		defer file.Close()
		
		fd := int(file.Fd())
		if blocking {
			return syscall.SetNonblock(fd, false)
		} else {
			return syscall.SetNonblock(fd, true)
		}
	}
	
	return fmt.Errorf("unable to set blocking mode")
}

// SetBroadcast enables or disables broadcast on the socket
func (s *UDPSocket) SetBroadcast(broadcast bool) error {
	s.isBroadcast = broadcast
	
	if s.conn == nil {
		return nil // Will be applied when socket is created
	}
	
	// Get the underlying file descriptor
	if udpConn, ok := s.conn.(*net.UDPConn); ok {
		file, err := udpConn.File()
		if err != nil {
			return err
		}
		defer file.Close()
		
		fd := int(file.Fd())
		if broadcast {
			return syscall.SetsockoptInt(fd, syscall.SOL_SOCKET, syscall.SO_BROADCAST, 1)
		} else {
			return syscall.SetsockoptInt(fd, syscall.SOL_SOCKET, syscall.SO_BROADCAST, 0)
		}
	}
	
	return fmt.Errorf("unable to set broadcast mode")
}

// SetReuseAddress enables or disables address reuse on the socket
func (s *UDPSocket) SetReuseAddress(reuse bool) error {
	if s.conn == nil {
		return fmt.Errorf("socket not initialized")
	}
	
	// Get the underlying file descriptor
	if udpConn, ok := s.conn.(*net.UDPConn); ok {
		file, err := udpConn.File()
		if err != nil {
			return err
		}
		defer file.Close()
		
		fd := int(file.Fd())
		if reuse {
			return syscall.SetsockoptInt(fd, syscall.SOL_SOCKET, syscall.SO_REUSEADDR, 1)
		} else {
			return syscall.SetsockoptInt(fd, syscall.SOL_SOCKET, syscall.SO_REUSEADDR, 0)
		}
	}
	
	return fmt.Errorf("unable to set reuse address")
}

// Close closes the socket
func (s *UDPSocket) Close() error {
	if s.conn != nil {
		err := s.conn.Close()
		s.conn = nil
		return err
	}
	return nil
}

// IsValid returns true if the socket is valid and connected
func (s *UDPSocket) IsValid() bool {
	return s.conn != nil
}

// GetLocalAddress returns the local address of the socket
func (s *UDPSocket) GetLocalAddress() *Address {
	if s.conn == nil {
		return s.localAddr
	}
	
	addr := s.conn.LocalAddr().(*net.UDPAddr)
	return &Address{
		host: addr.IP,
		port: uint16(addr.Port),
	}
}

// Platform-specific socket utilities

// GetRealMilliseconds returns the current time in milliseconds
func GetRealMilliseconds() U32 {
	return U32(time.Now().UnixNano() / int64(time.Millisecond))
}

// Sleep pauses execution for the specified number of milliseconds
func Sleep(milliseconds U32) {
	time.Sleep(time.Duration(milliseconds) * time.Millisecond)
}

// GetLocalAddresses returns a list of local network addresses
func GetLocalAddresses() []*Address {
	var addresses []*Address
	
	interfaces, err := net.Interfaces()
	if err != nil {
		return addresses
	}
	
	for _, iface := range interfaces {
		if iface.Flags&net.FlagUp == 0 || iface.Flags&net.FlagLoopback != 0 {
			continue
		}
		
		addrs, err := iface.Addrs()
		if err != nil {
			continue
		}
		
		for _, addr := range addrs {
			if ipnet, ok := addr.(*net.IPNet); ok {
				if ipnet.IP.To4() != nil {
					address := &Address{
						host: ipnet.IP,
						port: 0, // Port will be set when binding
					}
					addresses = append(addresses, address)
				}
			}
		}
	}
	
	return addresses
}

// IsAddressLocal returns true if the address is on the local machine
func IsAddressLocal(address *Address) bool {
	if address == nil {
		return false
	}
	
	// Check if it's localhost
	if address.host.IsLoopback() {
		return true
	}
	
	// Check against local interfaces
	localAddresses := GetLocalAddresses()
	for _, localAddr := range localAddresses {
		if address.host.Equal(localAddr.host) {
			return true
		}
	}
	
	return false
}

// ResolveAddress resolves a hostname to an IP address
func ResolveAddress(hostname string) (*Address, error) {
	ips, err := net.LookupIP(hostname)
	if err != nil {
		return nil, err
	}
	
	if len(ips) == 0 {
		return nil, fmt.Errorf("no IP addresses found for hostname: %s", hostname)
	}
	
	// Prefer IPv4 addresses
	for _, ip := range ips {
		if ip.To4() != nil {
			return &Address{host: ip, port: 0}, nil
		}
	}
	
	// Fall back to first IP if no IPv4 found
	return &Address{host: ips[0], port: 0}, nil
}