// tnlEventConnection.h conversion
use crate::types::*;
use crate::netbase::*;
use crate::netconnection::*;
use crate::netevent::*;
use crate::bitstream::*;
use std::collections::VecDeque;
use std::sync::{Arc, RwLock};

pub struct EventNote {
    pub event: Arc<RwLock<dyn NetEvent>>,
    pub seq_count: i32,
    // next_event handled by collection
}

pub struct EventConnectionBase {
    pub base: NetConnectionBase,
    
    // Queues
    pub send_event_queue: VecDeque<EventNote>,
    pub unordered_send_event_queue: VecDeque<EventNote>,
    pub wait_seq_events: VecDeque<EventNote>,
    pub notify_event_list: VecDeque<EventNote>,
    
    pub next_send_event_seq: i32,
    pub next_recv_event_seq: i32,
    pub last_acked_event_seq: i32,
    
    pub event_class_count: u32,
    pub event_class_bit_size: u32,
    pub event_class_version: u32,
}

impl EventConnectionBase {
    pub fn new() -> Self {
        Self {
            base: NetConnectionBase::new(),
            send_event_queue: VecDeque::new(),
            unordered_send_event_queue: VecDeque::new(),
            wait_seq_events: VecDeque::new(),
            notify_event_list: VecDeque::new(),
            next_send_event_seq: 0,
            next_recv_event_seq: 0,
            last_acked_event_seq: -1,
            event_class_count: 0, // Should be initialized from registry
            event_class_bit_size: 0,
            event_class_version: 0,
        }
    }
    
    pub fn post_net_event(&mut self, event: Arc<RwLock<dyn NetEvent>>) {
        let guarantee_type = {
            let e = event.read().unwrap();
            e.get_guarantee_type()
        };
        
        let note = EventNote {
            event,
            seq_count: if guarantee_type == NetEventGuaranteeType::GuaranteedOrdered {
                let seq = self.next_send_event_seq;
                self.next_send_event_seq += 1;
                seq
            } else {
                -1
            },
        };
        
        if guarantee_type == NetEventGuaranteeType::Unguaranteed {
            self.unordered_send_event_queue.push_back(note);
        } else {
            self.send_event_queue.push_back(note);
        }
    }
}

// EventConnection trait extension
pub trait EventConnectionTrait: NetConnection {
    fn get_event_base(&self) -> &EventConnectionBase;
    fn get_event_base_mut(&mut self) -> &mut EventConnectionBase;
    
    fn post_net_event(&mut self, event: Arc<RwLock<dyn NetEvent>>) {
        self.get_event_base_mut().post_net_event(event);
    }
    
    // Override packet methods
    fn write_packet_events(&mut self, stream: &mut BitStream, notify: &mut PacketNotify) {
        // Implementation of writePacket logic for events
    }
}

pub struct EventConnectionImpl {
    pub base: EventConnectionBase,
}

impl EventConnectionImpl {
    pub fn new() -> Self {
        Self {
            base: EventConnectionBase::new(),
        }
    }
}

// Implement Object
impl Object for EventConnectionImpl {
    fn get_class_rep(&self) -> &'static NetClassRep {
        todo!()
    }
    fn as_any(&self) -> &dyn std::any::Any { self }
    fn as_any_mut(&mut self) -> &mut dyn std::any::Any { self }
}

// Implement NetConnection
impl NetConnection for EventConnectionImpl {
    fn get_base(&self) -> &NetConnectionBase { &self.base.base }
    fn get_base_mut(&mut self) -> &mut NetConnectionBase { &mut self.base.base }
    
    fn write_packet(&mut self, stream: &mut BitStream, notify: &mut PacketNotify) {
        // Call event write logic
        // self.write_packet_events(stream, notify);
    }
}

impl EventConnectionTrait for EventConnectionImpl {
    fn get_event_base(&self) -> &EventConnectionBase { &self.base }
    fn get_event_base_mut(&mut self) -> &mut EventConnectionBase { &mut self.base }
}
