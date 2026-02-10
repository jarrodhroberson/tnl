// tnlNetObject.h conversion
use crate::types::*;
use crate::netbase::*;
use crate::bitstream::*;
use crate::ghostconnection::*; // Cyclic dependency likely
use std::any::Any;

pub trait NetObject: Object {
    fn get_ghost_priority(&self, connection: &dyn Any) -> f32 { 0.0 } // connection is GhostConnection
    fn scope_query(&self, connection: &dyn Any) -> bool { true } // connection is GhostConnection
    
    fn pack_update(&mut self, connection: &dyn Any, stream: &mut BitStream, mask: u32, version: u32) -> u32 { 0 }
    fn unpack_update(&mut self, connection: &dyn Any, stream: &mut BitStream, version: u32) {}
    
    fn on_ghost_add(&mut self, connection: &dyn Any) {}
    fn on_ghost_remove(&mut self) {}
    fn on_ghost_available(&mut self, connection: &dyn Any) {}
    
    // Mask bits management
    fn get_dirty_mask(&self) -> u32 { 0 }
    fn set_dirty_mask(&mut self, mask: u32) {}
}

// NetObject implementation usually done via struct + trait impl
