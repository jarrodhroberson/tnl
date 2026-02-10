// tnlByteBuffer.h conversion
use crate::types::*;
use base64::prelude::*;
use md5::{Md5, Digest};
use sha2::Sha256;
use crc32fast::Hasher;

#[derive(Debug, Clone, Default)]
pub struct ByteBuffer {
    pub buffer: Vec<u8>,
}

impl ByteBuffer {
    pub fn new() -> Self {
        Self { buffer: Vec::new() }
    }

    pub fn with_size(size: u32) -> Self {
        Self { buffer: vec![0; size as usize] }
    }

    pub fn from_vec(vec: Vec<u8>) -> Self {
        Self { buffer: vec }
    }

    pub fn get_buffer(&self) -> &[u8] {
        &self.buffer
    }

    pub fn get_buffer_mut(&mut self) -> &mut [u8] {
        &mut self.buffer
    }

    pub fn get_buffer_size(&self) -> u32 {
        self.buffer.len() as u32
    }

    pub fn resize(&mut self, new_size: u32) {
        self.buffer.resize(new_size as usize, 0);
    }

    pub fn append_buffer(&mut self, other: &[u8]) {
        self.buffer.extend_from_slice(other);
    }

    pub fn clear(&mut self) {
        // C++ clear() memsets to 0 but keeps size? 
        // "memset(mDataPtr, 0, mBufSize);"
        self.buffer.fill(0);
    }

    pub fn encode_base64(&self) -> ByteBuffer {
        let encoded = BASE64_STANDARD.encode(&self.buffer);
        // C++ implementation includes a null terminator in the buffer size?
        // "ret->resize(outLen+1); ret->getBuffer()[outLen] = 0;"
        // We will just return the bytes of the string.
        ByteBuffer::from_vec(encoded.into_bytes())
    }

    pub fn decode_base64(&self) -> ByteBuffer {
        // Assume buffer contains ascii string
        // Strip null terminator if present?
        let slice = if self.buffer.last() == Some(&0) {
            &self.buffer[..self.buffer.len()-1]
        } else {
            &self.buffer
        };
        
        let decoded = BASE64_STANDARD.decode(slice).unwrap_or_default();
        ByteBuffer::from_vec(decoded)
    }

    pub fn encode_base16(&self) -> ByteBuffer {
        let encoded = hex::encode(&self.buffer);
        ByteBuffer::from_vec(encoded.into_bytes())
    }

    pub fn decode_base16(&self) -> ByteBuffer {
         let slice = if self.buffer.last() == Some(&0) {
            &self.buffer[..self.buffer.len()-1]
        } else {
            &self.buffer
        };
        let decoded = hex::decode(slice).unwrap_or_default();
        ByteBuffer::from_vec(decoded)
    }

    pub fn compute_md5_hash(&self) -> ByteBuffer {
        let mut hasher = Md5::new();
        hasher.update(&self.buffer);
        let result = hasher.finalize();
        ByteBuffer::from_vec(result.to_vec())
    }

    pub fn calculate_crc(&self, start: u32, end: u32, initial_val: u32) -> u32 {
        let start = start as usize;
        let mut end = end as usize;
        if start >= self.buffer.len() { return 0; }
        if end > self.buffer.len() { end = self.buffer.len(); }

        let mut hasher = Hasher::new_with_initial(initial_val);
        hasher.update(&self.buffer[start..end]);
        hasher.finalize()
    }
}
