// tnlGhostConnection.h conversion
use crate::types::*;
use crate::netbase::*;
use crate::bitstream::*;
use crate::netconnection::*;
use crate::eventconnection::*;
use crate::netobject::*;
use std::collections::HashMap;
use std::sync::{Arc, RwLock};

pub const GHOST_ID_BIT_SIZE: u32 = 10;
pub const MAX_GHOSTS: u32 = 1 << GHOST_ID_BIT_SIZE;

bitflags::bitflags! {
    pub struct GhostFlags: u32 {
        const IN_SCOPE = 1 << 0;
        const SCOPE_LOCAL_ALWAYS = 1 << 1;
        const NOT_YET_GHOSTED = 1 << 2;
        const GHOSTING = 1 << 3;
        const KILL_GHOST = 1 << 4;
        const KILLING_GHOST = 1 << 5;
        const NOT_AVAILABLE = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
    }
}

// GhostInfo
pub struct GhostInfo {
    pub object: Option<Arc<RwLock<dyn NetObject>>>,
    pub index: u32,
    pub update_mask: u32,
    pub flags: GhostFlags,
    pub priority: f32,
    pub array_index: i32,
}

pub struct GhostConnectionBase {
    pub base: EventConnectionBase,
    
    // Ghost lists
    pub ghost_array: Vec<GhostInfo>, // or similar
    // Using Vec<Option<GhostInfo>>?
    
    pub ghosts: HashMap<u32, GhostInfo>,
    
    pub ghosting: bool,
    pub scoping: bool,
    pub ghosting_sequence: u32,
    
    pub scope_object: Option<Arc<RwLock<dyn NetObject>>>,
}

impl GhostConnectionBase {
    pub fn new() -> Self {
        Self {
            base: EventConnectionBase::new(),
            ghost_array: Vec::new(),
            ghosts: HashMap::new(),
            ghosting: false,
            scoping: false,
            ghosting_sequence: 0,
            scope_object: None,
        }
    }
}

pub trait GhostConnectionTrait: EventConnectionTrait {
    fn get_ghost_base(&self) -> &GhostConnectionBase;
    fn get_ghost_base_mut(&mut self) -> &mut GhostConnectionBase;
    
    // Ghost logic
    fn set_scope_object(&mut self, object: Option<Arc<RwLock<dyn NetObject>>>) {
        self.get_ghost_base_mut().scope_object = object;
    }
    
    fn packet_received_ghost(&mut self, notify: &PacketNotify) {
        // Ghost processing
    }
    
    fn write_packet_ghosts(&mut self, stream: &mut BitStream, notify: &mut PacketNotify) {
        // Ghost writing
    }
    
    fn read_packet_ghosts(&mut self, stream: &mut BitStream) {
        // Ghost reading
    }
}

pub struct GhostConnectionImpl {
    pub base: GhostConnectionBase,
}

impl GhostConnectionImpl {
    pub fn new() -> Self {
        Self {
            base: GhostConnectionBase::new(),
        }
    }
}

impl Object for GhostConnectionImpl {
    fn get_class_rep(&self) -> &'static NetClassRep {
        todo!()
    }
    fn as_any(&self) -> &dyn std::any::Any { self }
    fn as_any_mut(&mut self) -> &mut dyn std::any::Any { self }
}

impl NetConnection for GhostConnectionImpl {
    fn get_base(&self) -> &NetConnectionBase { &self.base.base.base }
    fn get_base_mut(&mut self) -> &mut NetConnectionBase { &mut self.base.base.base }
    
    fn write_packet(&mut self, stream: &mut BitStream, notify: &mut PacketNotify) {
        // Logic for ghosts + events
        // self.write_packet_events(stream, notify);
        // self.write_packet_ghosts(stream, notify);
    }
}

impl EventConnectionTrait for GhostConnectionImpl {
    fn get_event_base(&self) -> &EventConnectionBase { &self.base.base }
    fn get_event_base_mut(&mut self) -> &mut EventConnectionBase { &mut self.base.base }
}

impl GhostConnectionTrait for GhostConnectionImpl {
    fn get_ghost_base(&self) -> &GhostConnectionBase { &self.base }
    fn get_ghost_base_mut(&mut self) -> &mut GhostConnectionBase { &mut self.base }
}
