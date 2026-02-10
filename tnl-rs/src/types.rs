// tnlTypes.h conversion

// Basic types mapping (Rust primitives)
pub type U8 = u8;
pub type S8 = i8;
pub type U16 = u16;
pub type S16 = i16;
pub type U32 = u32;
pub type S32 = i32;
pub type U64 = u64;
pub type S64 = i64;
pub type F32 = f32;
pub type F64 = f64;

// Constants
pub const FLOAT_ONE: f32 = 1.0;
pub const FLOAT_HALF: f32 = 0.5;
pub const FLOAT_ZERO: f32 = 0.0;

pub const FLOAT_PI: f32 = std::f32::consts::PI;
pub const FLOAT_2PI: f32 = std::f32::consts::TAU; // 2 * PI
pub const FLOAT_INVERSE_PI: f32 = 1.0 / std::f32::consts::PI;
pub const FLOAT_HALF_PI: f32 = std::f32::consts::FRAC_PI_2;
pub const FLOAT_2INVERSE_PI: f32 = 2.0 / std::f32::consts::PI;
pub const FLOAT_INVERSE_2PI: f32 = 0.5 / std::f32::consts::PI;

pub const FLOAT_SQRT2: f32 = std::f32::consts::SQRT_2;
pub const FLOAT_SQRT_HALF: f32 = std::f32::consts::FRAC_1_SQRT_2;

// Limits are available in std via type::MIN/MAX, but providing aliases if needed.
// C++ defines S8_MIN, etc.

// NetType wrappers for RPC
// Using const generics for bit counts.

pub trait NetType {}

#[derive(Debug, Clone, Copy)]
pub struct Int<const BIT_COUNT: u32> {
    pub value: u32,
}

impl<const BIT_COUNT: u32> NetType for Int<BIT_COUNT> {}

impl<const BIT_COUNT: u32> From<u32> for Int<BIT_COUNT> {
    fn from(val: u32) -> Self {
        Self { value: val }
    }
}

impl<const BIT_COUNT: u32> From<Int<BIT_COUNT>> for u32 {
    fn from(val: Int<BIT_COUNT>) -> Self {
        val.value
    }
}

impl<const BIT_COUNT: u32> Int<BIT_COUNT> {
    pub fn get_precision_bits(&self) -> u32 {
        BIT_COUNT
    }
}

#[derive(Debug, Clone, Copy)]
pub struct SignedInt<const BIT_COUNT: u32> {
    pub value: i32,
}

impl<const BIT_COUNT: u32> NetType for SignedInt<BIT_COUNT> {}

impl<const BIT_COUNT: u32> From<i32> for SignedInt<BIT_COUNT> {
    fn from(val: i32) -> Self {
        Self { value: val }
    }
}

impl<const BIT_COUNT: u32> From<SignedInt<BIT_COUNT>> for i32 {
    fn from(val: SignedInt<BIT_COUNT>) -> Self {
        val.value
    }
}

impl<const BIT_COUNT: u32> SignedInt<BIT_COUNT> {
    pub fn get_precision_bits(&self) -> u32 {
        BIT_COUNT
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Float<const BIT_COUNT: u32> {
    pub value: f32,
}

impl<const BIT_COUNT: u32> NetType for Float<BIT_COUNT> {}

impl<const BIT_COUNT: u32> From<f32> for Float<BIT_COUNT> {
    fn from(val: f32) -> Self {
        Self { value: val }
    }
}

impl<const BIT_COUNT: u32> From<Float<BIT_COUNT>> for f32 {
    fn from(val: Float<BIT_COUNT>) -> Self {
        val.value
    }
}

#[derive(Debug, Clone, Copy)]
pub struct SignedFloat<const BIT_COUNT: u32> {
    pub value: f32,
}

impl<const BIT_COUNT: u32> NetType for SignedFloat<BIT_COUNT> {}

impl<const BIT_COUNT: u32> From<f32> for SignedFloat<BIT_COUNT> {
    fn from(val: f32) -> Self {
        Self { value: val }
    }
}

impl<const BIT_COUNT: u32> From<SignedFloat<BIT_COUNT>> for f32 {
    fn from(val: SignedFloat<BIT_COUNT>) -> Self {
        val.value
    }
}

#[derive(Debug, Clone, Copy)]
pub struct RangedU32<const RANGE_START: u32, const RANGE_END: u32> {
    pub value: u32,
}

impl<const RANGE_START: u32, const RANGE_END: u32> NetType for RangedU32<RANGE_START, RANGE_END> {}

impl<const RANGE_START: u32, const RANGE_END: u32> From<u32> for RangedU32<RANGE_START, RANGE_END> {
    fn from(val: u32) -> Self {
        Self { value: val }
    }
}

impl<const RANGE_START: u32, const RANGE_END: u32> From<RangedU32<RANGE_START, RANGE_END>> for u32 {
    fn from(val: RangedU32<RANGE_START, RANGE_END>) -> Self {
        val.value
    }
}

// Math Helpers

pub fn is_pow2(number: u32) -> bool {
    (number != 0) && (number & (number - 1)) == 0
}

pub fn get_bin_log2(value: u32) -> u32 {
    // Equivalent to (float bit hack in C++ source)
    // C++ source: (*((U32 *) &floatValue) >> 23) - 127;
    // This is an approximation/fast log2 for powers of 2 (or flooring).
    // Rust has u32::ilog2()
    if value == 0 { return 0; } // Handle 0 case if needed, though ilog2 panics or returns 0? 0->panic.
    value.ilog2()
}

pub fn get_next_bin_log2(number: u32) -> u32 {
    if number == 0 { return 0; }
    let log = get_bin_log2(number);
    if is_pow2(number) {
        log
    } else {
        log + 1
    }
}

pub fn get_next_pow2(value: u32) -> u32 {
    if is_pow2(value) {
        value
    } else {
        1 << (get_bin_log2(value) + 1)
    }
}

// Helper to write U32 to buffer (Big Endian in C++ code? No, C++ implementation was manually shifting)
// C++:
// buffer[0] = value >> 24;
// buffer[1] = value >> 16;
// buffer[2] = value >> 8;
// buffer[3] = value;
// This is Big Endian writing.

pub fn write_u32_to_buffer(value: u32, buffer: &mut [u8]) {
    buffer[0] = (value >> 24) as u8;
    buffer[1] = (value >> 16) as u8;
    buffer[2] = (value >> 8) as u8;
    buffer[3] = value as u8;
}

pub fn read_u32_from_buffer(buffer: &[u8]) -> u32 {
    ((buffer[0] as u32) << 24) |
    ((buffer[1] as u32) << 16) |
    ((buffer[2] as u32) << 8 ) |
     (buffer[3] as u32)
}

pub fn write_u16_to_buffer(value: u16, buffer: &mut [u8]) {
    buffer[0] = (value >> 8) as u8;
    buffer[1] = value as u8;
}

pub fn read_u16_from_buffer(buffer: &[u8]) -> u16 {
    ((buffer[0] as u16) << 8) |
     (buffer[1] as u16)
}

pub fn four_byte_align(value: u32) -> u32 {
    (value + 3) & !3
}

pub fn bit(x: u32) -> u32 {
    1 << x
}

#[derive(Debug, Clone, Copy, Default, PartialEq)]
pub struct Point3F {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Point3F {
    pub fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }
}
