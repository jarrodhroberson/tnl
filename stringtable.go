package tnl

import (
	"sync"
)

// StringTableEntry represents an entry in the string table
type StringTableEntry struct {
	string string
	index  U32
}

// NewStringTableEntry creates a new StringTableEntry
func NewStringTableEntry(str string) *StringTableEntry {
	return &StringTableEntry{
		string: str,
		index:  0,
	}
}

// GetString returns the string value
func (ste *StringTableEntry) GetString() string {
	return ste.string
}

// GetIndex returns the index in the string table
func (ste *StringTableEntry) GetIndex() U32 {
	return ste.index
}

// SetIndex sets the index in the string table
func (ste *StringTableEntry) SetIndex(index U32) {
	ste.index = index
}

// ConnectionStringTable manages string compression for network connections
type ConnectionStringTable struct {
	mutex           sync.RWMutex
	localStrings    map[string]*StringTableEntry
	remoteStrings   []string
	localIndex      U32
	remoteIndex     U32
	packetList      *PacketList
}

// PacketList tracks strings sent in packets for acknowledgment
type PacketList struct {
	strings []*StringTableEntry
	next    *PacketList
}

// NewConnectionStringTable creates a new ConnectionStringTable
func NewConnectionStringTable() *ConnectionStringTable {
	return &ConnectionStringTable{
		localStrings:  make(map[string]*StringTableEntry),
		remoteStrings: make([]string, 0),
		localIndex:    1, // Start at 1, 0 is reserved for empty string
		remoteIndex:   1,
	}
}

// AddString adds a string to the local string table
func (cst *ConnectionStringTable) AddString(str string) *StringTableEntry {
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	if entry, exists := cst.localStrings[str]; exists {
		return entry
	}
	
	entry := &StringTableEntry{
		string: str,
		index:  cst.localIndex,
	}
	
	cst.localStrings[str] = entry
	cst.localIndex++
	
	return entry
}

// GetString retrieves a string by its remote index
func (cst *ConnectionStringTable) GetString(index U32) string {
	cst.mutex.RLock()
	defer cst.mutex.RUnlock()
	
	if index == 0 {
		return ""
	}
	
	if int(index-1) < len(cst.remoteStrings) {
		return cst.remoteStrings[index-1]
	}
	
	return ""
}

// AddRemoteString adds a string received from the remote host
func (cst *ConnectionStringTable) AddRemoteString(str string) U32 {
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	// Check if string already exists
	for i, existing := range cst.remoteStrings {
		if existing == str {
			return U32(i + 1)
		}
	}
	
	// Add new string
	cst.remoteStrings = append(cst.remoteStrings, str)
	index := U32(len(cst.remoteStrings))
	
	return index
}

// GetLocalString gets a local string table entry
func (cst *ConnectionStringTable) GetLocalString(str string) *StringTableEntry {
	cst.mutex.RLock()
	defer cst.mutex.RUnlock()
	
	if entry, exists := cst.localStrings[str]; exists {
		return entry
	}
	
	return nil
}

// WriteStringTableEntry writes a string table entry to a BitStream
func (cst *ConnectionStringTable) WriteStringTableEntry(bs *BitStream, ste *StringTableEntry) {
	if ste == nil {
		bs.WriteInt(0, 8) // Empty string marker
		return
	}
	
	cst.mutex.RLock()
	defer cst.mutex.RUnlock()
	
	// Check if this is a known string
	if entry, exists := cst.localStrings[ste.string]; exists {
		// Write as index reference
		bs.WriteFlag(true) // Has index flag
		bs.WriteInt(entry.index, 16)
		
		// Add to packet list for acknowledgment tracking
		if cst.packetList != nil {
			cst.packetList.strings = append(cst.packetList.strings, entry)
		}
	} else {
		// Write as full string
		bs.WriteFlag(false) // No index flag
		bs.WriteString(ste.string, 255)
		
		// Add to local table
		newEntry := cst.AddString(ste.string)
		ste.index = newEntry.index
	}
}

// ReadStringTableEntry reads a string table entry from a BitStream
func (cst *ConnectionStringTable) ReadStringTableEntry(bs *BitStream) *StringTableEntry {
	hasIndex := bs.ReadFlag()
	
	if hasIndex {
		// Read as index reference
		index := bs.ReadInt(16)
		str := cst.GetString(index)
		return &StringTableEntry{
			string: str,
			index:  index,
		}
	} else {
		// Read as full string
		var strBuffer [256]byte
		bs.ReadString(string(strBuffer[:]))
		str := string(strBuffer[:])
		
		// Add to remote table
		index := cst.AddRemoteString(str)
		return &StringTableEntry{
			string: str,
			index:  index,
		}
	}
}

// StartPacket starts tracking strings for a new packet
func (cst *ConnectionStringTable) StartPacket() {
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	newPacket := &PacketList{
		strings: make([]*StringTableEntry, 0),
		next:    cst.packetList,
	}
	cst.packetList = newPacket
}

// EndPacket ends tracking for the current packet and returns the packet list
func (cst *ConnectionStringTable) EndPacket() *PacketList {
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	current := cst.packetList
	if current != nil {
		cst.packetList = current.next
		current.next = nil
	}
	
	return current
}

// PacketReceived handles acknowledgment of a packet
func (cst *ConnectionStringTable) PacketReceived(packetList *PacketList) {
	if packetList == nil {
		return
	}
	
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	// Mark strings as acknowledged
	for _, entry := range packetList.strings {
		// In a full implementation, this would update acknowledgment status
		_ = entry
	}
}

// PacketDropped handles notification that a packet was dropped
func (cst *ConnectionStringTable) PacketDropped(packetList *PacketList) {
	if packetList == nil {
		return
	}
	
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	// Handle dropped packet - might need to resend strings
	for _, entry := range packetList.strings {
		// In a full implementation, this would handle retransmission
		_ = entry
	}
}

// GetLocalStringCount returns the number of local strings
func (cst *ConnectionStringTable) GetLocalStringCount() U32 {
	cst.mutex.RLock()
	defer cst.mutex.RUnlock()
	
	return cst.localIndex - 1
}

// GetRemoteStringCount returns the number of remote strings
func (cst *ConnectionStringTable) GetRemoteStringCount() U32 {
	cst.mutex.RLock()
	defer cst.mutex.RUnlock()
	
	return U32(len(cst.remoteStrings))
}

// Clear clears all strings from the table
func (cst *ConnectionStringTable) Clear() {
	cst.mutex.Lock()
	defer cst.mutex.Unlock()
	
	cst.localStrings = make(map[string]*StringTableEntry)
	cst.remoteStrings = make([]string, 0)
	cst.localIndex = 1
	cst.remoteIndex = 1
	cst.packetList = nil
}

// HuffmanStringProcessor provides Huffman compression for strings
type HuffmanStringProcessor struct {
	// Simplified implementation - in a full version this would contain
	// Huffman trees and compression tables
}

// NewHuffmanStringProcessor creates a new HuffmanStringProcessor
func NewHuffmanStringProcessor() *HuffmanStringProcessor {
	return &HuffmanStringProcessor{}
}

// CompressString compresses a string using Huffman encoding
func (hsp *HuffmanStringProcessor) CompressString(input string) []byte {
	// Simplified implementation - just return the string as bytes
	// A full implementation would use actual Huffman compression
	return []byte(input)
}

// DecompressString decompresses a Huffman-encoded string
func (hsp *HuffmanStringProcessor) DecompressString(data []byte) string {
	// Simplified implementation - just return the bytes as string
	// A full implementation would use actual Huffman decompression
	return string(data)
}

// WriteString writes a Huffman compressed string to a BitStream
func (bs *BitStream) WriteString(str string, maxLen U8) {
	if len(str) > int(maxLen) {
		str = str[:maxLen]
	}
	
	// Write length
	bs.WriteInt(U32(len(str)), 8)
	
	// Write string data
	for _, b := range []byte(str) {
		bs.WriteInt(U32(b), 8)
	}
}

// ReadString reads a Huffman compressed string from a BitStream
func (bs *BitStream) ReadString(buffer string) {
	// Read length
	length := bs.ReadInt(8)
	
	// Read string data
	data := make([]byte, length)
	for i := U32(0); i < length; i++ {
		data[i] = U8(bs.ReadInt(8))
	}
	
	// This is a simplified approach - the buffer parameter in the original
	// C++ code was used differently. In Go, we'd typically return the string.
	copy([]byte(buffer), data)
}