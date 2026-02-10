// tnlHuffmanStringProcessor.h conversion
use crate::types::*;
use crate::bitstream::*;
use crate::assert::*;
use std::sync::Once;

// Frequency table from C++ source
static CHAR_FREQS: [u32; 256] = [
    0, 0, 0, 0, 0, 0, 0, 0, 0, 329, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2809, 68, 0, 27, 0, 58, 3, 62, 4, 7, 0, 0, 15, 65, 554, 3, 394, 404, 189, 117, 30, 51, 27, 15, 34, 32,
    80, 1, 142, 3, 142, 39, 0, 144, 125, 44, 122, 275, 70, 135, 61, 127, 8, 12, 113, 246, 122, 36, 185, 1,
    149, 309, 335, 12, 11, 14, 54, 151, 0, 0, 2, 0, 0, 211, 0, 2090, 344, 736, 993, 2872, 701, 605, 646, 1552,
    328, 305, 1240, 735, 1533, 1713, 562, 3, 1775, 1149, 1469, 979, 407, 553, 59, 279, 31, 0, 0, 0, 68, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
];

#[derive(Clone, Default)]
struct HuffNode {
    pop: u32,
    index0: i16,
    index1: i16,
}

#[derive(Clone, Default)]
struct HuffLeaf {
    pop: u32,
    num_bits: u8,
    symbol: u8,
    code: u32,
}

struct HuffmanTables {
    nodes: Vec<HuffNode>,
    leaves: Vec<HuffLeaf>,
}

static mut HUFFMAN_TABLES: Option<HuffmanTables> = None;
static INIT: Once = Once::new();

fn get_tables() -> &'static HuffmanTables {
    unsafe {
        INIT.call_once(|| {
            HUFFMAN_TABLES = Some(build_tables());
        });
        HUFFMAN_TABLES.as_ref().unwrap()
    }
}

fn build_tables() -> HuffmanTables {
    let mut leaves: Vec<HuffLeaf> = Vec::with_capacity(256);
    let mut nodes: Vec<HuffNode> = Vec::with_capacity(256);
    
    // Reserve space for root node
    nodes.push(HuffNode::default());

    for i in 0..256 {
        leaves.push(HuffLeaf {
            pop: CHAR_FREQS[i] + 1,
            num_bits: 0,
            symbol: i as u8,
            code: 0,
        });
    }

    struct HuffWrap {
        node_idx: Option<usize>,
        leaf_idx: Option<usize>,
    }

    impl HuffWrap {
        fn get_pop(&self, nodes: &[HuffNode], leaves: &[HuffLeaf]) -> u32 {
            if let Some(idx) = self.node_idx {
                nodes[idx].pop
            } else {
                leaves[self.leaf_idx.unwrap()].pop
            }
        }
    }

    let mut wraps: Vec<HuffWrap> = (0..256).map(|i| HuffWrap { node_idx: None, leaf_idx: Some(i) }).collect();

    while wraps.len() > 1 {
        let mut min1 = 0xfffffffe_u32;
        let mut min2 = 0xffffffff_u32;
        let mut index1 = usize::MAX;
        let mut index2 = usize::MAX;

        for (i, wrap) in wraps.iter().enumerate() {
            let pop = wrap.get_pop(&nodes, &leaves);
            if pop < min1 {
                min2 = min1;
                index2 = index1;
                min1 = pop;
                index1 = i;
            } else if pop < min2 {
                min2 = pop;
                index2 = i;
            }
        }

        // Create a node
        let pop1 = wraps[index1].get_pop(&nodes, &leaves);
        let pop2 = wraps[index2].get_pop(&nodes, &leaves);
        
        let idx0 = determine_index(&wraps[index1], &nodes, &leaves);
        let idx1 = determine_index(&wraps[index2], &nodes, &leaves);

        nodes.push(HuffNode {
            pop: pop1 + pop2,
            index0: idx0,
            index1: idx1,
        });
        let new_node_idx = nodes.len() - 1;

        // Merge
        let merge_idx = usize::max(index1, index2);
        let nuke_idx = usize::min(index1, index2); // Logic in C++: mergeIndex = index1 > index2 ? index2 : index1; Wait.
        // C++:
        // S32 mergeIndex = index1 > index2 ? index2 : index1;
        // S32 nukeIndex  = index1 > index2 ? index1 : index2;
        // pWrap[mergeIndex].set(&rNode);
        // if (index2 != (currWraps - 1)) pWrap[nukeIndex] = pWrap[currWraps - 1];
        
        let merge_idx = if index1 > index2 { index2 } else { index1 };
        let nuke_idx = if index1 > index2 { index1 } else { index2 };

        wraps[merge_idx] = HuffWrap { node_idx: Some(new_node_idx), leaf_idx: None };
        
        if nuke_idx != wraps.len() - 1 {
            wraps.swap(nuke_idx, wraps.len() - 1);
        }
        wraps.pop();
    }

    // Copy root
    nodes[0] = nodes[wraps[0].node_idx.unwrap()].clone();
    
    // Generate codes
    let mut temp_bs = BitStream::new(4); // 4 bytes (32 bits) enough for code
    generate_codes(&mut temp_bs, 0, 0, &nodes, &mut leaves);

    HuffmanTables { nodes, leaves }
}

fn determine_index(wrap: &HuffWrap, nodes: &[HuffNode], leaves: &[HuffLeaf]) -> i16 {
    if let Some(leaf_idx) = wrap.leaf_idx {
        -((leaf_idx as i16) + 1)
    } else {
        wrap.node_idx.unwrap() as i16
    }
}

fn generate_codes(bs: &mut BitStream, index: i16, depth: u8, nodes: &[HuffNode], leaves: &mut [HuffLeaf]) {
    if index < 0 {
        let leaf_idx = (-(index + 1)) as usize;
        let leaf = &mut leaves[leaf_idx];
        
        // Copy code from bitstream
        // In C++: memcpy(&rLeaf.code, rBS.getBuffer(), sizeof(rLeaf.code));
        // rLeaf.code is U32.
        // We need to read U32 from bs.buffer.
        // BitStream buffer is Little Endian?
        // Let's assume the bitstream contains the code bits.
        // The C++ generateCodes writes flags to the bitstream, essentially building the path.
        // Then it copies the bytes of the bitstream to `code`.
        
        let code_bytes = bs.get_buffer();
        // Assume code fits in u32 (max depth 32).
        if code_bytes.len() >= 4 {
             leaf.code = u32::from_le_bytes([code_bytes[0], code_bytes[1], code_bytes[2], code_bytes[3]]);
        } else {
             let mut arr = [0u8; 4];
             for (i, b) in code_bytes.iter().enumerate().take(4) {
                 arr[i] = *b;
             }
             leaf.code = u32::from_le_bytes(arr);
        }
        leaf.num_bits = depth;
    } else {
        let node = &nodes[index as usize];
        let pos = bs.get_bit_position();
        
        bs.write_flag(false);
        generate_codes(bs, node.index0, depth + 1, nodes, leaves);
        
        bs.set_bit_position(pos);
        bs.write_flag(true);
        generate_codes(bs, node.index1, depth + 1, nodes, leaves);
        
        bs.set_bit_position(pos);
    }
}

pub fn read_huff_buffer(stream: &mut BitStream, buffer: &mut Vec<u8>) {
    let tables = get_tables();
    
    if stream.read_flag() {
        let len = stream.read_int(8);
        buffer.reserve(len as usize); // Approx
        for _ in 0..len {
            let mut index: i16 = 0;
            loop {
                if index >= 0 {
                    if stream.read_flag() {
                        index = tables.nodes[index as usize].index1;
                    } else {
                        index = tables.nodes[index as usize].index0;
                    }
                } else {
                    buffer.push(tables.leaves[(-(index + 1)) as usize].symbol);
                    break;
                }
            }
        }
    } else {
        let len = stream.read_int(8);
        let start = buffer.len();
        buffer.resize(start + len as usize, 0);
        stream.read_bits(len * 8, &mut buffer[start..]);
    }
}

pub fn write_huff_buffer(stream: &mut BitStream, buffer: &[u8], max_len: u32) {
    let tables = get_tables();
    
    let len = buffer.len().min(max_len as usize);
    let len_u32 = len as u32;

    let mut num_bits = 0;
    for i in 0..len {
        num_bits += tables.leaves[buffer[i] as usize].num_bits as u32;
    }

    if num_bits >= (len_u32 * 8) {
        stream.write_flag(false);
        stream.write_int(len_u32, 8);
        stream.write_bits(len_u32 * 8, &buffer[0..len]);
    } else {
        stream.write_flag(true);
        stream.write_int(len_u32, 8);
        for i in 0..len {
            let leaf = &tables.leaves[buffer[i] as usize];
            // C++: pStream->writeBits(rLeaf.numBits, &rLeaf.code);
            // rLeaf.code is u32. We need to pass it as bytes? 
            // write_bits takes &[u8].
            let code_bytes = leaf.code.to_le_bytes(); // C++ memcpy logic implies memory order.
            stream.write_bits(leaf.num_bits as u32, &code_bytes);
        }
    }
}
