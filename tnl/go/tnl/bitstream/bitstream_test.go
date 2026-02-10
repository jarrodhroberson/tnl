package bitstream

import (
	"bytes"
	"testing"

	"github.com/go-gl/mathgl/mgl32"
)

func TestBitStream_Basic(t *testing.T) {
	buf := &bytes.Buffer{}
	writer := NewWriter(buf)

	writer.WriteFlag(true)
	writer.WriteUint8(0xAA)
	writer.WriteUint32(0x12345678)
	writer.WriteRangedUint32(50, 0, 100)
	writer.WriteString("Hello TNL")
	writer.Flush()

	if writer.Error() != nil {
		t.Fatalf("Writer error: %v", writer.Error())
	}

	reader := NewReader(buf)
	if reader.ReadFlag() != true {
		t.Errorf("Flag mismatch")
	}
	if val := reader.ReadUint8(); val != 0xAA {
		t.Errorf("Uint8 mismatch: expected 0xAA, got 0x%X", val)
	}
	if val := reader.ReadUint32(); val != 0x12345678 {
		t.Errorf("Uint32 mismatch: expected 0x12345678, got 0x%X", val)
	}
	if val := reader.ReadRangedUint32(0, 100); val != 50 {
		t.Errorf("Ranged mismatch: expected 50, got %d", val)
	}
	if val := reader.ReadString(); val != "Hello TNL" {
		t.Errorf("String mismatch: expected 'Hello TNL', got '%s'", val)
	}
}

func TestBitStream_Point(t *testing.T) {
	buf := &bytes.Buffer{}
	writer := NewWriter(buf)

	p := mgl32.Vec3{100, 200, 300}
	scale := float32(0.1)
	writer.WritePointCompressed(p, scale, false, mgl32.Vec3{})
	writer.Flush()

	reader := NewReader(buf)
	got := reader.ReadPointCompressed(scale, false, mgl32.Vec3{})

	if got.X() != p.X() || got.Y() != p.Y() || got.Z() != p.Z() {
		t.Errorf("Point mismatch: expected %v, got %v", p, got)
	}
}
