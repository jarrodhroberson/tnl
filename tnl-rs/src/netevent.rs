// tnlNetEvent.h conversion
use crate::types::*;
use crate::netbase::*;
use crate::bitstream::*;
use crate::netconnection::*;
use std::any::Any;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NetEventGuaranteeType {
    GuaranteedOrdered = 0,
    Guaranteed = 1,
    Unguaranteed = 2,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NetEventDirection {
    DirAny,
    DirServerToClient,
    DirClientToServer,
}

pub trait NetEvent: Object {
    fn pack(&self, connection: &mut dyn Any, stream: &mut BitStream);
    fn unpack(&mut self, connection: &mut dyn Any, stream: &mut BitStream);
    fn process(&mut self, connection: &mut dyn Any);
    
    // Metadata methods (usually static in C++)
    fn get_guarantee_type(&self) -> NetEventGuaranteeType { NetEventGuaranteeType::GuaranteedOrdered }
    fn get_direction(&self) -> NetEventDirection { NetEventDirection::DirAny }
}

// Implement Object for NetEvent implementors? 
// Users implementing NetEvent must also implement Object.
