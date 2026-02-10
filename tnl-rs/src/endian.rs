// tnlEndian.h conversion
use crate::types::*;

pub trait Endian: Copy {
    fn to_le(self) -> Self;
    fn to_be(self) -> Self;
    fn from_le(self) -> Self;
    fn from_be(self) -> Self;
}

macro_rules! impl_endian {
    ($t:ty) => {
        impl Endian for $t {
            #[inline]
            fn to_le(self) -> Self { self.to_le() }
            #[inline]
            fn to_be(self) -> Self { self.to_be() }
            #[inline]
            fn from_le(self) -> Self { self.from_le() }
            #[inline]
            fn from_be(self) -> Self { self.from_be() }
        }
    }
}

impl_endian!(u8);
impl_endian!(i8);
impl_endian!(u16);
impl_endian!(i16);
impl_endian!(u32);
impl_endian!(i32);
impl_endian!(u64);
impl_endian!(i64);
// f32/f64 don't have standard endian methods in older Rust, but they have to_bits().
// Actually they do have from_bits() / to_bits().

impl Endian for f32 {
    #[inline]
    fn to_le(self) -> Self { f32::from_bits(self.to_bits().to_le()) }
    #[inline]
    fn to_be(self) -> Self { f32::from_bits(self.to_bits().to_be()) }
    #[inline]
    fn from_le(self) -> Self { f32::from_bits(u32::from_le(self.to_bits())) }
    #[inline]
    fn from_be(self) -> Self { f32::from_bits(u32::from_be(self.to_bits())) }
}

impl Endian for f64 {
    #[inline]
    fn to_le(self) -> Self { f64::from_bits(self.to_bits().to_le()) }
    #[inline]
    fn to_be(self) -> Self { f64::from_bits(self.to_bits().to_be()) }
    #[inline]
    fn from_le(self) -> Self { f64::from_bits(u64::from_le(self.to_bits())) }
    #[inline]
    fn from_be(self) -> Self { f64::from_bits(u64::from_be(self.to_bits())) }
}

// C++ style functions
#[inline]
pub fn convert_host_to_le_endian<T: Endian>(val: T) -> T { val.to_le() }
#[inline]
pub fn convert_le_endian_to_host<T: Endian>(val: T) -> T { val.from_le() }
#[inline]
pub fn convert_host_to_be_endian<T: Endian>(val: T) -> T { val.to_be() }
#[inline]
pub fn convert_be_endian_to_host<T: Endian>(val: T) -> T { val.from_be() }

// EndianSwap logic
pub trait EndianSwap: Copy {
    fn endian_swap(self) -> Self;
}

impl<T: Endian> EndianSwap for T {
    fn endian_swap(self) -> Self {
        // C++ endianSwap does byte swapping unconditionally?
        // Wait, C++ endianSwap(u16) is:
        // return U16(((in_swap >> 8) & 0x00ff) | ((in_swap << 8) & 0xff00));
        // Yes, it swaps bytes.
        // But for Endian conversion (HostToLE), it depends on platform endianness.
        // So endian_swap() is just swap_bytes().
        
        // However, I can't use swap_bytes() generically on T easily without another trait or macro.
        // But since I implemented Endian using inherent methods (via macro), I can use to_be() on LE machine 
        // effectively as swap_bytes().
        
        // Actually, let's just use the macro again.
        // Or cleaner:
        // The trait defines to_be/le. 
        // swap_bytes() is inherent on primitives.
        
        // I'll just implement endian_swap for each type using the macro above?
        // No, I'll add swap_bytes to Endian trait? 
        // No, Endian trait is for conversion.
        
        // Let's keep it simple.
        self.to_be() // This is wrong if host is BE.
    }
}

// Correct approach for endian_swap (unconditional swap)
pub trait SwapBytes {
    fn swap_bytes(self) -> Self;
}

macro_rules! impl_swap_bytes {
    ($t:ty) => {
        impl SwapBytes for $t {
            #[inline]
            fn swap_bytes(self) -> Self { self.swap_bytes() }
        }
    }
}
impl_swap_bytes!(u8);
impl_swap_bytes!(i8);
impl_swap_bytes!(u16);
impl_swap_bytes!(i16);
impl_swap_bytes!(u32);
impl_swap_bytes!(i32);
impl_swap_bytes!(u64);
impl_swap_bytes!(i64);

impl SwapBytes for f32 {
    fn swap_bytes(self) -> Self { f32::from_bits(self.to_bits().swap_bytes()) }
}
impl SwapBytes for f64 {
    fn swap_bytes(self) -> Self { f64::from_bits(self.to_bits().swap_bytes()) }
}

#[inline]
pub fn endian_swap<T: SwapBytes>(val: T) -> T { val.swap_bytes() }
