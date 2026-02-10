// tnlNetBase.h conversion
use crate::types::*;
use std::any::Any;
use std::sync::{Arc, Weak, RwLock};
use std::collections::HashMap;
use std::sync::OnceLock;

// Enums
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum NetClassType {
    None = -1,
    Object = 0,
    DataBlock,
    Event,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum NetClassGroup {
    Game,
    Community,
    Master,
    Unused2,
}

pub const NET_CLASS_GROUP_COUNT: usize = 4;
pub const NET_CLASS_TYPE_COUNT: usize = 3; // Object, DataBlock, Event

// Masks
pub const NET_CLASS_GROUP_GAME_MASK: u32 = 1 << (NetClassGroup::Game as u32);
pub const NET_CLASS_GROUP_COMMUNITY_MASK: u32 = 1 << (NetClassGroup::Community as u32);
pub const NET_CLASS_GROUP_MASTER_MASK: u32 = 1 << (NetClassGroup::Master as u32);
pub const NET_CLASS_GROUP_ALL_MASK: u32 = (1 << NET_CLASS_GROUP_COUNT) - 1;

// Object Trait
// Corresponding to C++ Object class.
// Note: In C++, Object has ref counting. In Rust we use Arc.
pub trait Object: Any + Send + Sync {
    fn get_class_rep(&self) -> &'static NetClassRep;
    fn get_class_name(&self) -> &'static str {
        self.get_class_rep().name
    }
    fn get_class_id(&self, group: NetClassGroup) -> u32 {
        self.get_class_rep().class_id[group as usize]
    }
    
    // Support for downcasting
    fn as_any(&self) -> &dyn Any;
    fn as_any_mut(&mut self) -> &mut dyn Any;
}

// NetClassRep
// In C++, this is a class. In Rust, we'll make it a struct with a factory function.
pub struct NetClassRep {
    pub name: &'static str,
    pub class_type: NetClassType,
    pub group_mask: u32,
    pub class_version: i32,
    pub class_id: [u32; NET_CLASS_GROUP_COUNT],
    pub create_func: fn() -> Arc<RwLock<dyn Object>>, // Factory
    pub next_class: Option<&'static NetClassRep>, // Intrusive list not ideal in Rust, but we use a registry.
}

// Global Registry
pub struct NetClassRegistry {
    classes: Vec<&'static NetClassRep>,
    lookup: HashMap<String, &'static NetClassRep>,
    table: [[Vec<&'static NetClassRep>; NET_CLASS_TYPE_COUNT]; NET_CLASS_GROUP_COUNT],
}

impl NetClassRegistry {
    fn new() -> Self {
        // Init table
        let table = Default::default(); // recursive default needed?
        // [ [Vec::new(), ...], ... ]
        // Array initialization in Rust is tricky for non-Copy.
        // We'll init manually or use lazy_static logic in get().
        Self {
            classes: Vec::new(),
            lookup: HashMap::new(),
            table: Default::default(), // Only works if Vec implements Default, which it does. Arrays of arrays of Vec?
                                      // [ [Vec; 3]; 4]
        }
    }

    pub fn register(&mut self, rep: &'static NetClassRep) {
        self.classes.push(rep);
        self.lookup.insert(rep.name.to_string(), rep);
        // Table population logic from C++ initialize()
        // We can do it lazily or here.
        // C++ does sorting.
    }
    
    pub fn create(&self, name: &str) -> Option<Arc<RwLock<dyn Object>>> {
        if let Some(rep) = self.lookup.get(name) {
            Some((rep.create_func)())
        } else {
            None
        }
    }
}

static mut REGISTRY: OnceLock<RwLock<NetClassRegistry>> = OnceLock::new();

pub fn get_registry() -> &'static RwLock<NetClassRegistry> {
    unsafe {
        REGISTRY.get_or_init(|| RwLock::new(NetClassRegistry::new()))
    }
}

// Pointer types
pub type RefPtr<T> = Arc<RwLock<T>>;
pub type SafePtr<T> = Weak<RwLock<T>>;

// Macros for declaring class rep
// Rust doesn't have macros that run before main easily without external crates.
// We'll assume manual registration or use `inventory` crate if we could.
// For translation purposes, we'll assume a `register_classes()` function is called at startup.

#[macro_export]
macro_rules! tnl_declare_class {
    ($name:ident, $type_str:expr) => {
        // ...
    };
}
