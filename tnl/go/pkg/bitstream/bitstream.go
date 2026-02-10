package bitstream

import (
	"io"
	"math"

	smaz "github.com/cespare/go-smaz"
	bstream "github.com/dgryski/go-bitstream"
	"github.com/go-gl/mathgl/mgl32"
)

type BitStream struct {
	reader *bstream.BitReader
	writer *bstream.BitWriter
	err    error
}

func NewReader(r io.Reader) *BitStream {
	return &BitStream{reader: bstream.NewReader(r)}
}

func NewWriter(w io.Writer) *BitStream {
	return &BitStream{writer: bstream.NewWriter(w)}
}

func (b *BitStream) Error() error {
	return b.err
}

// --- Basic Bit I/O ---

func (b *BitStream) WriteBit(bit bool) {
	if b.err != nil {
		return
	}
	b.err = b.writer.WriteBit(bstream.Bit(bit))
}

func (b *BitStream) ReadBit() bool {
	if b.err != nil {
		return false
	}
	var bit bstream.Bit
	bit, b.err = b.reader.ReadBit()
	return bool(bit)
}

func (b *BitStream) WriteBits(val uint64, nbits int) {
	if b.err != nil {
		return
	}
	// go-bitstream writes bits from the most significant bit of the uint64?
	// Actually, go-bitstream.WriteBits takes a uint64 and nbits.
	// TNL writes bits in a specific order. We'll need to ensure compatibility.
	// go-bitstream is big-endian bit order (MSB first).
	b.err = b.writer.WriteBits(val, nbits)
}

func (b *BitStream) ReadBits(nbits int) uint64 {
	if b.err != nil {
		return 0
	}
	var val uint64
	val, b.err = b.reader.ReadBits(nbits)
	return val
}

func (b *BitStream) Flush() error {
	if b.writer != nil {
		return b.writer.Flush(bstream.Zero)
	}
	return nil
}

// --- TNL Specific Primitives (Always Network Order) ---

func (b *BitStream) WriteUint8(val uint8) {
	b.WriteBits(uint64(val), 8)
}

func (b *BitStream) ReadUint8() uint8 {
	return uint8(b.ReadBits(8))
}

func (b *BitStream) WriteUint16(val uint16) {
	b.WriteBits(uint64(val), 16)
}

func (b *BitStream) ReadUint16() uint16 {
	return uint16(b.ReadBits(16))
}

func (b *BitStream) WriteUint32(val uint32) {
	b.WriteBits(uint64(val), 32)
}

func (b *BitStream) ReadUint32() uint32 {
	return uint32(b.ReadBits(32))
}

func (b *BitStream) WriteInt32(val int32) {
	b.WriteUint32(uint32(val))
}

func (b *BitStream) ReadInt32() int32 {
	return int32(b.ReadUint32())
}

func (b *BitStream) WriteFloat32(val float32) {
	b.WriteUint32(math.Float32bits(val))
}

func (b *BitStream) ReadFloat32() float32 {
	return math.Float32frombits(b.ReadUint32())
}

// --- Ranged I/O ---

func getBinLog2(n uint32) uint32 {
	if n == 0 {
		return 0
	}
	return uint32(math.Log2(float64(n)))
}

func isPow2(n uint32) bool {
	return n != 0 && (n&(n-1)) == 0
}

func getNextBinLog2(n uint32) uint32 {
	if n <= 1 {
		return 0
	}
	log2 := getBinLog2(n)
	if isPow2(n) {
		return log2
	}
	return log2 + 1
}

func (b *BitStream) WriteRangedUint32(val, min, max uint32) {
	if val < min || val > max {
		// Should we set an error? TNL asserts.
		return
	}
	rangeSize := max - min + 1
	rangeBits := getNextBinLog2(rangeSize)
	b.WriteBits(uint64(val-min), int(rangeBits))
}

func (b *BitStream) ReadRangedUint32(min, max uint32) uint32 {
	rangeSize := max - min + 1
	rangeBits := getNextBinLog2(rangeSize)
	return uint32(b.ReadBits(int(rangeBits))) + min
}

// --- Flags ---

func (b *BitStream) WriteFlag(val bool) bool {
	b.WriteBit(val)
	return val
}

func (b *BitStream) ReadFlag() bool {
	return b.ReadBit()
}

// --- 3D Point Compression (mgl32 integration) ---

var bitCounts = [4]int{16, 18, 20, 32}

func (b *BitStream) WritePointCompressed(p mgl32.Vec3, scale float32, relative bool, basePoint mgl32.Vec3) {
	if b.err != nil {
		return
	}

	invScale := 1.0 / scale
	var ptType uint32 = 3
	var vec mgl32.Vec3

	if relative {
		vec = p.Sub(basePoint)
		dist := vec.Len() * invScale
		if dist < float32(1<<15) {
			ptType = 0
		} else if dist < float32(1<<17) {
			ptType = 1
		} else if dist < float32(1<<19) {
			ptType = 2
		}
	}

	b.WriteBits(uint64(ptType), 2)

	if ptType != 3 {
		nbits := bitCounts[ptType]
		b.writeSignedInt(int32(vec.X()*invScale), nbits)
		b.writeSignedInt(int32(vec.Y()*invScale), nbits)
		b.writeSignedInt(int32(vec.Z()*invScale), nbits)
	} else {
		b.WriteFloat32(p.X())
		b.WriteFloat32(p.Y())
		b.WriteFloat32(p.Z())
	}
}

func (b *BitStream) ReadPointCompressed(scale float32, relative bool, basePoint mgl32.Vec3) mgl32.Vec3 {
	if b.err != nil {
		return mgl32.Vec3{}
	}

	ptType := uint32(b.ReadBits(2))
	var p mgl32.Vec3

	if ptType == 3 {
		p = mgl32.Vec3{b.ReadFloat32(), b.ReadFloat32(), b.ReadFloat32()}
	} else {
		nbits := bitCounts[ptType]
		p = mgl32.Vec3{
			float32(b.readSignedInt(nbits)),
			float32(b.readSignedInt(nbits)),
			float32(b.readSignedInt(nbits)),
		}
		if relative {
			p = basePoint.Add(p.Mul(scale))
		} else {
			p = p.Mul(scale)
		}
	}
	return p
}

func (b *BitStream) writeSignedInt(val int32, nbits int) {
	if b.WriteFlag(val < 0) {
		b.WriteBits(uint64(-val), nbits-1)
	} else {
		b.WriteBits(uint64(val), nbits-1)
	}
}

func (b *BitStream) readSignedInt(nbits int) int32 {
	if b.ReadFlag() {
		return -int32(b.ReadBits(nbits - 1))
	}
	return int32(b.ReadBits(nbits - 1))
}

// --- String Compression (Smaz) ---

func (b *BitStream) WriteString(s string) {
	if b.err != nil {
		return
	}
	compressed := smaz.Compress([]byte(s))
	length := len(compressed)
	// We'll write the length as a ranged U32 (0-1024 as per TNL byteBuffer write)
	b.WriteRangedUint32(uint32(length), 0, 1024)
	for _, v := range compressed {
		b.WriteUint8(v)
	}
}

func (b *BitStream) ReadString() string {
	if b.err != nil {
		return ""
	}
	length := b.ReadRangedUint32(0, 1024)
	if b.err != nil {
		return ""
	}
	compressed := make([]byte, length)
	for i := uint32(0); i < length; i++ {
		compressed[i] = b.ReadUint8()
	}
	decompressed, err := smaz.Decompress(compressed)
	if err != nil {
		b.err = err
		return ""
	}
	return string(decompressed)
}
