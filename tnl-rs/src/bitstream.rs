// tnlBitStream.h conversion
use crate::types::*;
use crate::bytebuffer::*;
use crate::assert::*;
use crate::platform::*; // for endian swap?
use crate::endian::*;

const RESIZE_PAD: u32 = 1500;

#[derive(Default)]
pub struct BitStream {
    buffer: ByteBuffer,
    bit_num: u32,
    max_read_bit_num: u32,
    max_write_bit_num: u32,
    error: bool,
    compress_relative: bool,
    compress_point: Point3F,
    string_buffer: Vec<u8>, // Using Vec instead of fixed array for flexibility
}

impl BitStream {
    pub fn new(size: u32) -> Self {
        let mut bs = Self {
            buffer: ByteBuffer::with_size(size),
            bit_num: 0,
            max_read_bit_num: size << 3,
            max_write_bit_num: size << 3,
            error: false,
            compress_relative: false,
            compress_point: Point3F::default(),
            string_buffer: Vec::with_capacity(256),
        };
        bs.reset();
        bs
    }

    pub fn reset(&mut self) {
        self.bit_num = 0;
        self.error = false;
        self.compress_relative = false;
        self.string_buffer.clear();
    }

    pub fn get_buffer(&self) -> &[u8] {
        self.buffer.get_buffer()
    }
    
    pub fn get_buffer_mut(&mut self) -> &mut [u8] {
        self.buffer.get_buffer_mut()
    }

    pub fn set_max_sizes(&mut self, max_read_size: u32, max_write_size: u32) {
        self.max_read_bit_num = max_read_size << 3;
        self.max_write_bit_num = max_write_size << 3;
    }

    pub fn get_byte_position(&self) -> u32 {
        (self.bit_num + 7) >> 3
    }
    
    pub fn set_byte_position(&mut self, pos: u32) {
        self.bit_num = pos << 3;
    }

    pub fn get_bit_position(&self) -> u32 {
        self.bit_num
    }
    
    pub fn set_bit_position(&mut self, pos: u32) {
        self.bit_num = pos;
    }
    
    pub fn advance_bit_position(&mut self, bits: i32) {
        self.bit_num = (self.bit_num as i32 + bits) as u32;
    }

    fn resize_bits(&mut self, new_bits: u32) -> bool {
        let new_size = ((self.max_write_bit_num + new_bits + 7) >> 3) + RESIZE_PAD;
        // In C++, resize logic calls buffer.resize().
        // Here we assume buffer handles memory.
        self.buffer.resize(new_size);
        self.max_read_bit_num = new_size << 3;
        self.max_write_bit_num = new_size << 3;
        true
    }

    pub fn write_bits(&mut self, bit_count: u32, bit_ptr: &[u8]) -> bool {
        if bit_count == 0 { return true; }
        if bit_count + self.bit_num > self.max_write_bit_num {
            if !self.resize_bits(bit_count + self.bit_num - self.max_write_bit_num) {
                return false;
            }
        }

        let up_shift = self.bit_num & 0x7;
        let down_shift = 8 - up_shift;
        
        // Safety: buffer resizing ensures we have space.
        // We use raw pointer logic or indexing. Rust slice indexing is safer.
        // But implementing arbitrary bit writes is tedious with safe Rust slices.
        // We'll use indexing carefully.

        let mut current_byte_idx = (self.bit_num >> 3) as usize;
        let mut source_idx = 0;
        let mut remaining_bits = bit_count;

        let buffer = self.buffer.get_buffer_mut();

        if down_shift >= remaining_bits {
             let mask = ((1 << remaining_bits) - 1) << up_shift;
             let source_byte = bit_ptr[source_idx];
             buffer[current_byte_idx] = (buffer[current_byte_idx] & !mask) | ((source_byte << up_shift) & mask);
             self.bit_num += remaining_bits;
             return true;
        }

        if up_shift == 0 {
             while remaining_bits >= 8 {
                 buffer[current_byte_idx] = bit_ptr[source_idx];
                 current_byte_idx += 1;
                 source_idx += 1;
                 remaining_bits -= 8;
             }
             if remaining_bits > 0 {
                 let mask = (1 << remaining_bits) - 1;
                 buffer[current_byte_idx] = (bit_ptr[source_idx] & mask) | (buffer[current_byte_idx] & !mask);
             }
             self.bit_num += bit_count;
             return true;
        }

        // Unaligned write
        let mut source_byte;
        let mut dest_byte = buffer[current_byte_idx] & (0xFF >> down_shift);
        // This dest_byte is the preserved lower bits of the destination byte.
        // Wait, C++ logic:
        // U8 destByte = *destPtr & (0xFF >> downShift);
        // This preserves the bits *below* the write start. Correct.

        let last_mask = 0xFF >> (7 - ((self.bit_num + bit_count - 1) & 0x7));
        // This mask is for the *last* byte written.

        self.bit_num += bit_count;

        while remaining_bits >= 8 {
            source_byte = bit_ptr[source_idx];
            source_idx += 1;
            
            buffer[current_byte_idx] = dest_byte | (source_byte << up_shift);
            current_byte_idx += 1;
            
            dest_byte = source_byte >> down_shift;
            remaining_bits -= 8;
        }

        if remaining_bits == 0 {
            // Write the spillover
            buffer[current_byte_idx] = (buffer[current_byte_idx] & !last_mask) | (dest_byte & last_mask);
            return true;
        }

        if remaining_bits <= down_shift {
             // We fit in the current byte (spillover + new bits)?
             // No, dest_byte contains spillover from previous.
             // We have remaining_bits from *new* source byte.
             // But we entered this block because remaining_bits < 8.
             // C++ logic:
             /*
               if(bitCount <= downShift) {
                  *destPtr = (*destPtr & ~lastMask) | ((destByte | (*sourcePtr << upShift)) & lastMask);
                  return true;
               }
             */
             let next_source = bit_ptr[source_idx];
             buffer[current_byte_idx] = (buffer[current_byte_idx] & !last_mask) | ((dest_byte | (next_source << up_shift)) & last_mask);
             return true;
        }
        
        // Spill over into next byte
        source_byte = bit_ptr[source_idx];
        buffer[current_byte_idx] = dest_byte | (source_byte << up_shift);
        current_byte_idx += 1;
        buffer[current_byte_idx] = (buffer[current_byte_idx] & !last_mask) | ((source_byte >> down_shift) & last_mask);
        
        true
    }

    pub fn read_bits(&mut self, bit_count: u32, bit_ptr: &mut [u8]) -> bool {
        if bit_count == 0 { return true; }
        if bit_count + self.bit_num > self.max_read_bit_num {
            self.error = true;
            return false;
        }

        let buffer = self.buffer.get_buffer();
        let mut source_idx = (self.bit_num >> 3) as usize;
        let mut dest_idx = 0;
        
        let down_shift = self.bit_num & 0x7;
        let up_shift = 8 - down_shift;

        let mut remaining_bits = bit_count;

        if down_shift == 0 {
            while remaining_bits >= 8 {
                bit_ptr[dest_idx] = buffer[source_idx];
                source_idx += 1;
                dest_idx += 1;
                remaining_bits -= 8;
            }
             // Handle last partial byte if any?
             // C++:
             /*
                if(!downShift) {
                   while(byteCount--) *destPtr++ = *sourcePtr++;
                   bitNum += bitCount;
                   return true;
                }
             */
             // Wait, byteCount is (bitCount + 7) >> 3.
             // It copies bytes.
             if remaining_bits > 0 {
                 // But wait, if remaining_bits < 8, we still copy the whole byte?
                 // The C++ code copies *bytes*. "byteCount = (bitCount + 7) >> 3".
                 // It copies more bits than requested into the buffer, but that's fine as long as dest has space.
                 // We should be careful not to overflow dest.
                 bit_ptr[dest_idx] = buffer[source_idx];
             }
             self.bit_num += bit_count;
             return true;
        }
        
        let mut source_byte = buffer[source_idx] >> down_shift;
        self.bit_num += bit_count;
        
        while remaining_bits >= 8 {
            source_idx += 1;
            let next_byte = buffer[source_idx];
            bit_ptr[dest_idx] = source_byte | (next_byte << up_shift);
            dest_idx += 1;
            source_byte = next_byte >> down_shift;
            remaining_bits -= 8;
        }
        
        if remaining_bits > 0 {
            if remaining_bits <= up_shift {
                bit_ptr[dest_idx] = source_byte;
            } else {
                source_idx += 1;
                bit_ptr[dest_idx] = source_byte | (buffer[source_idx] << up_shift);
            }
        }
        
        true
    }
    
    // ... Implement read_int, write_int etc.
    
    pub fn write_int(&mut self, val: u32, bit_count: u32) {
        // Rust is LE usually. C++ does convertHostToLEndian(val).
        let val_le = val.to_le();
        // We need to pass a slice of bytes representing val.
        // We can cast &val to &[u8] in unsafe, or use to_le_bytes().
        let bytes = val_le.to_le_bytes();
        self.write_bits(bit_count, &bytes);
    }
    
    pub fn read_int(&mut self, bit_count: u32) -> u32 {
        let mut ret: u32 = 0;
        // We need to read into ret's bytes.
        // Safety: We assume ret is 4 bytes.
        let mut bytes = [0u8; 4];
        self.read_bits(bit_count, &mut bytes);
        ret = u32::from_le_bytes(bytes);
        
        // Clear bits that we didn't read
        if bit_count == 32 {
            ret
        } else {
            ret & ((1 << bit_count) - 1)
        }
    }
    
    pub fn write_flag(&mut self, val: bool) -> bool {
        if self.bit_num + 1 > self.max_write_bit_num {
            if !self.resize_bits(1) {
                return false;
            }
        }
        let byte_idx = (self.bit_num >> 3) as usize;
        let bit_offset = self.bit_num & 0x7;
        let buffer = self.buffer.get_buffer_mut();
        
        if val {
            buffer[byte_idx] |= 1 << bit_offset;
        } else {
            buffer[byte_idx] &= !(1 << bit_offset);
        }
        self.bit_num += 1;
        val
    }
    
    pub fn read_flag(&mut self) -> bool {
        if self.bit_num >= self.max_read_bit_num {
            self.error = true;
            return false;
        }
        let byte_idx = (self.bit_num >> 3) as usize;
        let bit_offset = self.bit_num & 0x7;
        let val = (self.buffer.get_buffer()[byte_idx] & (1 << bit_offset)) != 0;
        self.bit_num += 1;
        val
    }

    pub fn read_string(&mut self) -> String {
        let mut bytes = Vec::new();
        if self.read_flag() {
            let offset = self.read_int(8);
            // This logic in C++ used mStringBuffer + offset.
            // Simplified: read into bytes.
            crate::huffman::read_huff_buffer(self, &mut bytes);
        } else {
            crate::huffman::read_huff_buffer(self, &mut bytes);
        }
        String::from_utf8_lossy(&bytes).into_owned()
    }

    pub fn write_string(&mut self, s: &str, max_len: u32) {
        // Simple implementation without substring compression for now
        self.write_flag(false);
        crate::huffman::write_huff_buffer(self, s.as_bytes(), max_len);
    }
}
