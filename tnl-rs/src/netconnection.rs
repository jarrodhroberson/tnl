// tnlNetConnection.h conversion
use crate::types::*;
use crate::udp::*;
use crate::bitstream::*;
use crate::bytebuffer::*;
use crate::netbase::*;
use crate::assert::*;
use crate::platform::*;
use std::collections::VecDeque;
use std::sync::{Arc, RwLock};

// Constants
pub const MAX_PACKET_WINDOW_SIZE_SHIFT: u32 = 5;
pub const MAX_PACKET_WINDOW_SIZE: u32 = 1 << MAX_PACKET_WINDOW_SIZE_SHIFT;
pub const PACKET_WINDOW_MASK: u32 = MAX_PACKET_WINDOW_SIZE - 1;
pub const MAX_ACK_MASK_SIZE: u32 = 1 << (MAX_PACKET_WINDOW_SIZE_SHIFT - 5);
pub const MAX_ACK_BYTE_COUNT: u32 = MAX_ACK_MASK_SIZE << 2;
pub const SEQUENCE_NUMBER_BIT_SIZE: u32 = 11;
pub const SEQUENCE_NUMBER_WINDOW_SIZE: u32 = 1 << SEQUENCE_NUMBER_BIT_SIZE;
pub const SEQUENCE_NUMBER_MASK: u32 = !SEQUENCE_NUMBER_WINDOW_SIZE;
pub const ACK_SEQUENCE_NUMBER_BIT_SIZE: u32 = 10;
pub const ACK_SEQUENCE_NUMBER_WINDOW_SIZE: u32 = 1 << ACK_SEQUENCE_NUMBER_BIT_SIZE;
pub const ACK_SEQUENCE_NUMBER_MASK: u32 = !ACK_SEQUENCE_NUMBER_WINDOW_SIZE;
pub const PACKET_HEADER_BIT_SIZE: u32 = 3 + ACK_SEQUENCE_NUMBER_BIT_SIZE + SEQUENCE_NUMBER_BIT_SIZE;
pub const PACKET_HEADER_BYTE_SIZE: u32 = (PACKET_HEADER_BIT_SIZE + 7) >> 3;
pub const PACKET_HEADER_PAD_BITS: u32 = (PACKET_HEADER_BYTE_SIZE << 3) - PACKET_HEADER_BIT_SIZE;
pub const MESSAGE_SIGNATURE_BYTES: u32 = 5;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NetPacketType {
    DataPacket,
    PingPacket,
    AckPacket,
    InvalidPacketType,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ConnectionState {
    NotConnected = 0,
    AwaitingChallengeResponse,
    SendingPunchPackets,
    ComputingPuzzleSolution,
    AwaitingConnectResponse,
    ConnectTimedOut,
    ConnectRejected,
    Connected,
    Disconnected,
    TimedOut,
}

// PacketNotify struct (base)
pub struct PacketNotify {
    pub rate_changed: bool,
    pub send_time: u32,
    pub next_packet: Option<Box<PacketNotify>>, // Linked list
    // String list?
}

impl Default for PacketNotify {
    fn default() -> Self {
        Self {
            rate_changed: false,
            send_time: 0,
            next_packet: None,
        }
    }
}

pub struct NetConnectionBase {
    pub last_packet_recv_time: u32,
    pub last_seq_recvd_at_send: [u32; MAX_PACKET_WINDOW_SIZE as usize],
    pub last_seq_recvd: u32,
    pub highest_acked_seq: u32,
    pub last_send_seq: u32,
    pub ack_mask: [u32; MAX_ACK_MASK_SIZE as usize],
    pub last_recv_ack_ack: u32,
    pub initial_send_seq: u32,
    pub initial_recv_seq: u32,
    pub highest_acked_send_time: u32,
    
    pub ping_timeout: u32,
    pub ping_retry_count: u32,
    pub ping_send_count: u32,
    pub last_ping_send_time: u32,
    
    pub connection_state: ConnectionState,
    pub net_address: Address,
    
    // Notify Queue
    pub notify_queue: VecDeque<Box<PacketNotify>>, // Using VecDeque instead of intrusive list
}

impl NetConnectionBase {
    pub fn new() -> Self {
        Self {
            last_packet_recv_time: 0,
            last_seq_recvd_at_send: [0; MAX_PACKET_WINDOW_SIZE as usize],
            last_seq_recvd: 0,
            highest_acked_seq: 0,
            last_send_seq: 0,
            ack_mask: [0; MAX_ACK_MASK_SIZE as usize],
            last_recv_ack_ack: 0,
            initial_send_seq: 0,
            initial_recv_seq: 0,
            highest_acked_send_time: 0,
            ping_timeout: 5000,
            ping_retry_count: 10,
            ping_send_count: 0,
            last_ping_send_time: 0,
            connection_state: ConnectionState::NotConnected,
            net_address: Address::default(),
            notify_queue: VecDeque::new(),
        }
    }
    
    pub fn get_next_send_sequence(&self) -> u32 {
        self.last_send_seq + 1
    }
    
    pub fn window_full(&self) -> bool {
        self.last_send_seq - self.highest_acked_seq >= MAX_PACKET_WINDOW_SIZE
    }
    
    pub fn has_unacked_sent_packets(&self) -> bool {
        self.last_send_seq != self.highest_acked_seq
    }
}

// NetConnection Trait
pub trait NetConnection: Object {
    fn get_base(&self) -> &NetConnectionBase;
    fn get_base_mut(&mut self) -> &mut NetConnectionBase;

    // Virtual methods
    fn on_connect_terminated(&mut self, reason: u32, rejection_string: &str) {}
    fn on_connection_terminated(&mut self, reason: u32, error_string: &str) {}
    fn on_connection_established(&mut self) {}
    
    fn read_packet(&mut self, stream: &mut BitStream) {}
    fn write_packet(&mut self, stream: &mut BitStream, notify: &mut PacketNotify) {}
    fn packet_received(&mut self, notify: &PacketNotify) {}
    fn packet_dropped(&mut self, notify: &PacketNotify) {}
    
    fn prepare_write_packet(&mut self) {}
    fn is_data_to_transmit(&self) -> bool { false }
    
    // Packet processing
    fn process_raw_packet(&mut self, stream: &mut BitStream) {
        // Implementation of readRawPacket logic
    }
}

// Concrete generic NetConnection implementation can be done via a struct
// but usually users extend it.

pub struct NetConnectionImpl {
    pub base: NetConnectionBase,
}

impl NetConnectionImpl {
    pub fn new() -> Self {
        Self {
            base: NetConnectionBase::new(),
        }
    }
}

impl Object for NetConnectionImpl {
    fn get_class_rep(&self) -> &'static NetClassRep {
        // Placeholder
        todo!()
    }
    fn as_any(&self) -> &dyn std::any::Any { self }
    fn as_any_mut(&mut self) -> &mut dyn std::any::Any { self }
}

impl NetConnection for NetConnectionImpl {
    fn get_base(&self) -> &NetConnectionBase { &self.base }
    fn get_base_mut(&mut self) -> &mut NetConnectionBase { &mut self.base }
}
