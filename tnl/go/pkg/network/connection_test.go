package network

import (
	"bytes"
	"testing"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
	"github.com/jarrodhroberson/tnl-go/pkg/protocol"
)

func TestNetConnection_Notify(t *testing.T) {
	addr, _ := netio.ParseAddress("127.0.0.1:28000")
	conn := NewNetConnection(addr, 100)
	// lastRecvSeq is 50. pkSequenceNumber must be within [50, 50+31]
	conn.SetInitialRecvSequence(50)

	// --- 1. Client Sends a Data Packet ---
	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	conn.WritePacketHeader(bs, protocol.DataPacket)
	bs.Flush()

	if conn.lastSendSeq != 101 {
		t.Errorf("Expected lastSendSeq 101, got %d", conn.lastSendSeq)
	}

	// --- 2. Client Receives an Ack Packet from Server ---
	// Server's packet header: Seq 60, acking client's 101
	serverBuf := &bytes.Buffer{}
	sbs := bitstream.NewWriter(serverBuf)
	sbs.WriteBits(uint64(protocol.AckPacket), 2)
	sbs.WriteBits(uint64(60&0x1F), 5)
	sbs.WriteBit(true)
	sbs.WriteBits(uint64(60>>5), protocol.SequenceNumberBitSize-5)
	sbs.WriteBits(uint64(101&(protocol.AckSequenceNumberWindowSize-1)), protocol.AckSequenceNumberBitSize)
	sbs.WriteBits(0, protocol.PacketHeaderPadBits)

	sbs.WriteRangedUint32(1, 0, protocol.MaxAckByteCount)
	sbs.WriteBits(1, 8)
	sbs.WriteBits(0, 8) // Delay
	sbs.Flush()

	reader := bitstream.NewReader(serverBuf)
	pktType, seq, ok := conn.ReadPacketHeader(reader)

	if !ok || pktType != protocol.AckPacket || seq != 60 {
		t.Errorf("Failed to read header: ok=%v, type=%v, seq=%d", ok, pktType, seq)
	}

	if conn.highestAckedSeq != 101 {
		t.Errorf("Expected highestAckedSeq 101, got %d", conn.highestAckedSeq)
	}

	if len(conn.notifyQueue) != 0 {
		t.Errorf("Expected notifyQueue to be empty after ack, got %d", len(conn.notifyQueue))
	}
}

func TestNetConnection_Wrapping(t *testing.T) {
	addr, _ := netio.ParseAddress("127.0.0.1:28000")
	initial := uint32(2040)
	conn := NewNetConnection(addr, initial)

	for i := 0; i < 20; i++ {
		buf := &bytes.Buffer{}
		bs := bitstream.NewWriter(buf)
		conn.WritePacketHeader(bs, protocol.DataPacket)
		bs.Flush()
	}

	if conn.lastSendSeq != initial+20 {
		t.Errorf("Sequence mismatch: %d", conn.lastSendSeq)
	}

	// The NEXT packet should be 2061, which wraps to 13
	expectedWrapped := (initial + 21) & (protocol.SequenceNumberWindowSize - 1)

	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	conn.WritePacketHeader(bs, protocol.DataPacket)
	bs.Flush()

	reader := bitstream.NewReader(buf)
	reader.ReadBits(2) // skip type
	low := reader.ReadBits(5)
	reader.ReadBit() // skip game bit
	high := reader.ReadBits(protocol.SequenceNumberBitSize - 5)

	readSeq := low | (high << 5)
	if uint32(readSeq) != expectedWrapped {
		t.Errorf("Read wrapped sequence mismatch: expected %d, got %d", expectedWrapped, readSeq)
	}
}
