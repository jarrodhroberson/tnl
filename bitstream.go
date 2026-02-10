package tnl

import (
	"crypto/md5"
	"crypto/sha256"
	"encoding/binary"
	"errors"
	"math"
)

const (
	MaxPacketDataSize = 1500 // Maximum UDP packet size
	ResizePad         = 1500
)

// BitStream provides a bit-level stream interface to a data buffer
type BitStream struct {
	buffer              []U8
	bitNum              U32
	maxReadBitNum       U32
	maxWriteBitNum      U32
	error               bool
	compressRelative    bool
	compressPoint       Point3F
	stringTable         *ConnectionStringTable
	stringBuffer        [256]byte
	resizable           bool
}

// NewBitStream creates a new BitStream with the given buffer
func NewBitStream(buffer []U8) *BitStream {
	bs := &BitStream{
		buffer:    buffer,
		resizable: false,
	}
	bs.SetMaxSizes(U32(len(buffer)), U32(len(buffer)))
	bs.Reset()
	return bs
}

// NewBitStreamWithMaxWrite creates a new BitStream with specified max write size
func NewBitStreamWithMaxWrite(buffer []U8, maxWriteSize U32) *BitStream {
	bs := &BitStream{
		buffer:    buffer,
		resizable: false,
	}
	bs.SetMaxSizes(U32(len(buffer)), maxWriteSize)
	bs.Reset()
	return bs
}

// NewResizableBitStream creates a resizable BitStream
func NewResizableBitStream() *BitStream {
	bs := &BitStream{
		buffer:    make([]U8, ResizePad),
		resizable: true,
	}
	bs.SetMaxSizes(U32(len(bs.buffer)), U32(len(bs.buffer)))
	bs.Reset()
	return bs
}

// SetMaxSizes sets the maximum read and write sizes for the BitStream
func (bs *BitStream) SetMaxSizes(maxReadSize, maxWriteSize U32) {
	if maxWriteSize == 0 {
		maxWriteSize = maxReadSize
	}
	bs.SetMaxBitSizes(maxReadSize<<3, maxWriteSize<<3)
}

// SetMaxBitSizes sets the maximum read and write bit sizes for the BitStream
func (bs *BitStream) SetMaxBitSizes(maxReadBitSize, maxWriteBitSize U32) {
	if maxWriteBitSize == 0 {
		maxWriteBitSize = maxReadBitSize
	}
	bs.maxReadBitNum = maxReadBitSize
	bs.maxWriteBitNum = maxWriteBitSize
}

// Reset resets the read/write position to 0 and clears any error state
func (bs *BitStream) Reset() {
	bs.bitNum = 0
	bs.error = false
}

// ClearStringBuffer clears the string compression buffer
func (bs *BitStream) ClearStringBuffer() {
	bs.stringBuffer[0] = 0
}

// SetStringTable sets the ConnectionStringTable for compressing string table entries
func (bs *BitStream) SetStringTable(table *ConnectionStringTable) {
	bs.stringTable = table
}

// ClearError clears the error state from an attempted read or write overrun
func (bs *BitStream) ClearError() {
	bs.error = false
}

// GetBytePtr returns a pointer to the next byte in the BitStream
func (bs *BitStream) GetBytePtr() []U8 {
	bytePos := (bs.bitNum + 7) >> 3
	if int(bytePos) < len(bs.buffer) {
		return bs.buffer[bytePos:]
	}
	return nil
}

// GetBytePosition returns the current position in the stream rounded up to the next byte
func (bs *BitStream) GetBytePosition() U32 {
	return (bs.bitNum + 7) >> 3
}

// GetBitPosition returns the current bit position in the stream
func (bs *BitStream) GetBitPosition() U32 {
	return bs.bitNum
}

// SetBytePosition sets the position in the stream to the first bit of byte newPosition
func (bs *BitStream) SetBytePosition(newPosition U32) {
	bs.bitNum = newPosition << 3
}

// SetBitPosition sets the position in the stream to newBitPosition
func (bs *BitStream) SetBitPosition(newBitPosition U32) {
	bs.bitNum = newBitPosition
}

// AdvanceBitPosition advances the position in the stream by numBits
func (bs *BitStream) AdvanceBitPosition(numBits S32) {
	bs.SetBitPosition(bs.GetBitPosition() + U32(numBits))
}

// GetMaxReadBitPosition returns the maximum readable bit position
func (bs *BitStream) GetMaxReadBitPosition() U32 {
	return bs.maxReadBitNum
}

// GetBitSpaceAvailable returns the number of bits that can be written without resizing
func (bs *BitStream) GetBitSpaceAvailable() U32 {
	return bs.maxWriteBitNum - bs.bitNum
}

// ZeroToByteBoundary pads the bits up to the next byte boundary with 0's
func (bs *BitStream) ZeroToByteBoundary() {
	if bs.bitNum&0x7 != 0 {
		bs.WriteInt(0, U8(8-(bs.bitNum&0x7)))
	}
}

// resizeBits resizes the buffer if needed and possible
func (bs *BitStream) resizeBits(numBitsNeeded U32) bool {
	if !bs.resizable {
		return false
	}
	
	bitsNeeded := bs.bitNum + numBitsNeeded
	bytesNeeded := (bitsNeeded + 7) >> 3
	
	if U32(len(bs.buffer)) < bytesNeeded {
		newSize := bytesNeeded + ResizePad
		newBuffer := make([]U8, newSize)
		copy(newBuffer, bs.buffer)
		bs.buffer = newBuffer
		bs.SetMaxSizes(newSize, newSize)
	}
	return true
}

// WriteInt writes an unsigned integer value between 0 and 2^(bitCount-1) into the stream
func (bs *BitStream) WriteInt(value U32, bitCount U8) {
	if bs.bitNum+U32(bitCount) > bs.maxWriteBitNum {
		if !bs.resizeBits(U32(bitCount)) {
			bs.error = true
			return
		}
	}
	
	if bitCount == 0 {
		return
	}
	
	// Mask the value to ensure it fits in bitCount bits
	value &= (1 << bitCount) - 1
	
	for bitCount > 0 {
		bytePos := bs.bitNum >> 3
		bitPos := bs.bitNum & 7
		bitsInByte := 8 - bitPos
		
		if U32(bitCount) <= bitsInByte {
			// All remaining bits fit in current byte
			mask := U8((1 << bitCount) - 1)
			bs.buffer[bytePos] |= U8(value&U32(mask)) << bitPos
			bs.bitNum += U32(bitCount)
			return
		} else {
			// Fill current byte and continue
			mask := U8((1 << bitsInByte) - 1)
			bs.buffer[bytePos] |= U8(value&U32(mask)) << bitPos
			value >>= bitsInByte
			bitCount -= U8(bitsInByte)
			bs.bitNum += bitsInByte
		}
	}
}

// ReadInt reads an unsigned integer value between 0 and 2^(bitCount-1) from the stream
func (bs *BitStream) ReadInt(bitCount U8) U32 {
	if bs.bitNum+U32(bitCount) > bs.maxReadBitNum {
		bs.error = true
		return 0
	}
	
	if bitCount == 0 {
		return 0
	}
	
	var value U32
	var bitsRead U8
	
	for bitCount > 0 {
		bytePos := bs.bitNum >> 3
		bitPos := bs.bitNum & 7
		bitsInByte := 8 - bitPos
		
		if U32(bitCount) <= bitsInByte {
			// All remaining bits are in current byte
			mask := U8((1 << bitCount) - 1)
			bits := (bs.buffer[bytePos] >> bitPos) & mask
			value |= U32(bits) << bitsRead
			bs.bitNum += U32(bitCount)
			return value
		} else {
			// Read from current byte and continue
			mask := U8((1 << bitsInByte) - 1)
			bits := (bs.buffer[bytePos] >> bitPos) & mask
			value |= U32(bits) << bitsRead
			bitCount -= U8(bitsInByte)
			bitsRead += U8(bitsInByte)
			bs.bitNum += bitsInByte
		}
	}
	
	return value
}

// WriteIntAt writes an unsigned integer at a specific bit position without changing current position
func (bs *BitStream) WriteIntAt(value U32, bitCount U8, bitPosition U32) {
	curPos := bs.GetBitPosition()
	bs.SetBitPosition(bitPosition)
	bs.WriteInt(value, bitCount)
	bs.SetBitPosition(curPos)
}

// WriteSignedInt writes a signed integer value
func (bs *BitStream) WriteSignedInt(value S32, bitCount U8) {
	if bitCount == 0 {
		return
	}
	
	// Convert to unsigned representation
	var uValue U32
	if value < 0 {
		uValue = U32(value + (1 << (bitCount - 1)))
	} else {
		uValue = U32(value)
	}
	
	bs.WriteInt(uValue, bitCount)
}

// ReadSignedInt reads a signed integer value
func (bs *BitStream) ReadSignedInt(bitCount U8) S32 {
	if bitCount == 0 {
		return 0
	}
	
	uValue := bs.ReadInt(bitCount)
	
	// Convert from unsigned representation
	maxVal := S32(1 << (bitCount - 1))
	if S32(uValue) >= maxVal {
		return S32(uValue) - (maxVal << 1)
	}
	return S32(uValue)
}

// WriteRangedU32 writes an unsigned integer value in the range rangeStart to rangeEnd inclusive
func (bs *BitStream) WriteRangedU32(value, rangeStart, rangeEnd U32) {
	if value < rangeStart || value > rangeEnd {
		bs.error = true
		return
	}
	
	rangeSize := rangeEnd - rangeStart + 1
	rangeBits := GetNextBinLog2(rangeSize)
	bs.WriteInt(value-rangeStart, U8(rangeBits))
}

// ReadRangedU32 reads an unsigned integer value in the range rangeStart to rangeEnd inclusive
func (bs *BitStream) ReadRangedU32(rangeStart, rangeEnd U32) U32 {
	if rangeEnd < rangeStart {
		bs.error = true
		return rangeStart
	}
	
	rangeSize := rangeEnd - rangeStart + 1
	rangeBits := GetNextBinLog2(rangeSize)
	val := bs.ReadInt(U8(rangeBits))
	return val + rangeStart
}

// WriteEnum writes an enumeration value in the range 0...enumRange-1
func (bs *BitStream) WriteEnum(enumValue, enumRange U32) {
	bs.WriteInt(enumValue, U8(GetNextBinLog2(enumRange)))
}

// ReadEnum reads an enumeration value in the range 0...enumRange-1
func (bs *BitStream) ReadEnum(enumRange U32) U32 {
	return bs.ReadInt(U8(GetNextBinLog2(enumRange)))
}

// WriteFloat writes a float from 0 to 1 inclusive, using bitCount bits of precision
func (bs *BitStream) WriteFloat(f F32, bitCount U8) {
	if f < 0 {
		f = 0
	} else if f > 1 {
		f = 1
	}
	
	maxVal := U32((1 << bitCount) - 1)
	intVal := U32(f * F32(maxVal) + 0.5)
	bs.WriteInt(intVal, bitCount)
}

// ReadFloat reads a float from 0 to 1 inclusive, using bitCount bits of precision
func (bs *BitStream) ReadFloat(bitCount U8) F32 {
	maxVal := U32((1 << bitCount) - 1)
	intVal := bs.ReadInt(bitCount)
	return F32(intVal) / F32(maxVal)
}

// WriteSignedFloat writes a signed float from -1 to 1 inclusive
func (bs *BitStream) WriteSignedFloat(f F32, bitCount U8) {
	if f < -1 {
		f = -1
	} else if f > 1 {
		f = 1
	}
	
	// Convert to 0-1 range
	normalized := (f + 1) * 0.5
	bs.WriteFloat(normalized, bitCount)
}

// ReadSignedFloat reads a signed float from -1 to 1 inclusive
func (bs *BitStream) ReadSignedFloat(bitCount U8) F32 {
	normalized := bs.ReadFloat(bitCount)
	return (normalized * 2) - 1
}

// WriteClassID writes an object's class ID
func (bs *BitStream) WriteClassID(classID, classType, classGroup U32) {
	bitSize := GetNetClassBitSize(classGroup, classType)
	bs.WriteInt(classID, U8(bitSize))
}

// ReadClassID reads a class ID for an object
func (bs *BitStream) ReadClassID(classType, classGroup U32) U32 {
	bitSize := GetNetClassBitSize(classGroup, classType)
	if bitSize == 0 {
		return 0xFFFFFFFF // Invalid class ID
	}
	return bs.ReadInt(U8(bitSize))
}

// WriteFlag writes a single boolean flag and returns the boolean that was written
func (bs *BitStream) WriteFlag(val bool) bool {
	if val {
		bs.WriteInt(1, 1)
	} else {
		bs.WriteInt(0, 1)
	}
	return val
}

// ReadFlag reads a single bit from the stream
func (bs *BitStream) ReadFlag() bool {
	if bs.bitNum >= bs.maxReadBitNum {
		bs.error = true
		return false
	}
	
	bytePos := bs.bitNum >> 3
	bitPos := bs.bitNum & 7
	mask := U8(1 << bitPos)
	result := (bs.buffer[bytePos] & mask) != 0
	bs.bitNum++
	return result
}

// WriteBits writes bitCount bits into the stream from bitPtr
func (bs *BitStream) WriteBits(bitCount U32, data []U8) bool {
	if bs.bitNum+bitCount > bs.maxWriteBitNum {
		if !bs.resizeBits(bitCount) {
			bs.error = true
			return false
		}
	}
	
	byteCount := (bitCount + 7) >> 3
	if U32(len(data)) < byteCount {
		bs.error = true
		return false
	}
	
	// Simple byte-aligned copy for now
	if bs.bitNum&7 == 0 {
		copy(bs.buffer[bs.bitNum>>3:], data[:byteCount])
		bs.bitNum += bitCount
		return true
	}
	
	// Bit-by-bit copy for non-aligned data
	for i := U32(0); i < bitCount; i++ {
		byteIdx := i >> 3
		bitIdx := i & 7
		bit := (data[byteIdx] >> bitIdx) & 1
		bs.WriteInt(U32(bit), 1)
	}
	
	return !bs.error
}

// ReadBits reads bitCount bits from the stream into data
func (bs *BitStream) ReadBits(bitCount U32, data []U8) bool {
	if bs.bitNum+bitCount > bs.maxReadBitNum {
		bs.error = true
		return false
	}
	
	byteCount := (bitCount + 7) >> 3
	if U32(len(data)) < byteCount {
		bs.error = true
		return false
	}
	
	// Simple byte-aligned copy for now
	if bs.bitNum&7 == 0 {
		copy(data[:byteCount], bs.buffer[bs.bitNum>>3:])
		bs.bitNum += bitCount
		return true
	}
	
	// Bit-by-bit copy for non-aligned data
	for i := U32(0); i < bitCount; i++ {
		bit := bs.ReadInt(1)
		byteIdx := i >> 3
		bitIdx := i & 7
		if bit != 0 {
			data[byteIdx] |= 1 << bitIdx
		} else {
			data[byteIdx] &^= 1 << bitIdx
		}
	}
	
	return !bs.error
}

// SetBit sets the bit at position bitCount to the value of set
func (bs *BitStream) SetBit(bitCount U32, set bool) bool {
	if bitCount >= U32(len(bs.buffer))<<3 {
		return false
	}
	
	bytePos := bitCount >> 3
	bitPos := bitCount & 7
	mask := U8(1 << bitPos)
	
	if set {
		bs.buffer[bytePos] |= mask
	} else {
		bs.buffer[bytePos] &^= mask
	}
	
	return true
}

// TestBit tests the value of the bit at position bitCount
func (bs *BitStream) TestBit(bitCount U32) bool {
	if bitCount >= U32(len(bs.buffer))<<3 {
		return false
	}
	
	bytePos := bitCount >> 3
	bitPos := bitCount & 7
	mask := U8(1 << bitPos)
	
	return (bs.buffer[bytePos] & mask) != 0
}

// IsFull returns whether the BitStream writing has exceeded the write target size
func (bs *BitStream) IsFull() bool {
	return bs.bitNum > (U32(len(bs.buffer)) << 3)
}

// IsValid returns whether the stream has generated an error condition
func (bs *BitStream) IsValid() bool {
	return !bs.error
}

// GetBuffer returns the underlying buffer
func (bs *BitStream) GetBuffer() []U8 {
	return bs.buffer
}

// GetBufferSize returns the size of the underlying buffer
func (bs *BitStream) GetBufferSize() U32 {
	return U32(len(bs.buffer))
}

// HashAndEncrypt hashes the BitStream and encrypts with the given cipher
func (bs *BitStream) HashAndEncrypt(hashDigestSize, encryptStartOffset U32, cipher SymmetricCipher) error {
	if cipher == nil {
		return errors.New("cipher is nil")
	}
	
	// Calculate hash
	dataSize := bs.GetBytePosition()
	var hash []byte
	
	switch hashDigestSize {
	case 16:
		h := md5.Sum(bs.buffer[:dataSize])
		hash = h[:]
	case 32:
		h := sha256.Sum256(bs.buffer[:dataSize])
		hash = h[:]
	default:
		return errors.New("unsupported hash digest size")
	}
	
	// Append hash to buffer
	copy(bs.buffer[dataSize:], hash)
	
	// Encrypt from offset
	if encryptStartOffset < dataSize+hashDigestSize {
		encryptSize := dataSize + hashDigestSize - encryptStartOffset
		encrypted, err := cipher.Encrypt(bs.buffer[encryptStartOffset:encryptStartOffset+encryptSize])
		if err != nil {
			return err
		}
		copy(bs.buffer[encryptStartOffset:], encrypted)
	}
	
	return nil
}

// DecryptAndCheckHash decrypts the BitStream and validates the hash
func (bs *BitStream) DecryptAndCheckHash(hashDigestSize, decryptStartOffset U32, cipher SymmetricCipher) bool {
	if cipher == nil {
		return false
	}
	
	dataSize := bs.GetBytePosition()
	if dataSize < hashDigestSize {
		return false
	}
	
	// Decrypt from offset
	if decryptStartOffset < dataSize {
		decryptSize := dataSize - decryptStartOffset
		decrypted, err := cipher.Decrypt(bs.buffer[decryptStartOffset:decryptStartOffset+decryptSize])
		if err != nil {
			return false
		}
		copy(bs.buffer[decryptStartOffset:], decrypted)
	}
	
	// Verify hash
	actualDataSize := dataSize - hashDigestSize
	var expectedHash []byte
	
	switch hashDigestSize {
	case 16:
		h := md5.Sum(bs.buffer[:actualDataSize])
		expectedHash = h[:]
	case 32:
		h := sha256.Sum256(bs.buffer[:actualDataSize])
		expectedHash = h[:]
	default:
		return false
	}
	
	// Compare hashes
	storedHash := bs.buffer[actualDataSize:dataSize]
	for i := range expectedHash {
		if expectedHash[i] != storedHash[i] {
			return false
		}
	}
	
	return true
}

// PacketStream provides a network interface to the BitStream for easy construction of data packets
type PacketStream struct {
	*BitStream
	buffer [MaxPacketDataSize]U8
}

// NewPacketStream creates a new PacketStream
func NewPacketStream(targetPacketSize U32) *PacketStream {
	if targetPacketSize == 0 || targetPacketSize > MaxPacketDataSize {
		targetPacketSize = MaxPacketDataSize
	}
	
	ps := &PacketStream{}
	ps.BitStream = NewBitStreamWithMaxWrite(ps.buffer[:], targetPacketSize)
	return ps
}

// SendTo sends this packet to the specified address through the specified socket
func (ps *PacketStream) SendTo(socket Socket, address *Address) error {
	dataSize := ps.GetBytePosition()
	return socket.SendTo(ps.buffer[:dataSize], address)
}

// RecvFrom reads a packet into the stream from the specified socket
func (ps *PacketStream) RecvFrom(socket Socket) (*Address, error) {
	n, addr, err := socket.RecvFrom(ps.buffer[:])
	if err != nil {
		return nil, err
	}
	
	ps.Reset()
	ps.SetMaxSizes(U32(n), U32(n))
	return addr, nil
}