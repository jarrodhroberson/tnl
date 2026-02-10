package network

import (
	"bytes"
	"testing"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
)

type testEvent struct {
	EventBase
	Data      string
	Processed bool
}

func (e *testEvent) Pack(conn *NetConnection, bs *bitstream.BitStream) {
	bs.WriteString(e.Data)
}

func (e *testEvent) Unpack(conn *NetConnection, bs *bitstream.BitStream) {
	e.Data = bs.ReadString()
}

func (e *testEvent) Process(conn *NetConnection) {
	e.Processed = true
}

func TestEventConnection_Ordering(t *testing.T) {
	RegisterEvent(100, func() Event {
		return &testEvent{EventBase: EventBase{GuaranteeType: GuaranteedOrdered}}
	})

	addr, _ := netio.ParseAddress("127.0.0.1:28000")
	conn := NewEventConnection(addr, 100)

	// Create and write ev1
	ev1 := &testEvent{EventBase: EventBase{GuaranteeType: GuaranteedOrdered}, Data: "First"}
	conn.PostEvent(ev1)
	buf1 := &bytes.Buffer{}
	bs1 := bitstream.NewWriter(buf1)
	conn.WriteEvents(bs1, 1)
	bs1.Flush()

	// Create and write ev2
	ev2 := &testEvent{EventBase: EventBase{GuaranteeType: GuaranteedOrdered}, Data: "Second"}
	conn.PostEvent(ev2)
	buf2 := &bytes.Buffer{}
	bs2 := bitstream.NewWriter(buf2)
	conn.WriteEvents(bs2, 2)
	bs2.Flush()

	// Receiver side
	recvConn := NewEventConnection(addr, 200)

	// 1. Receive packet 2 (out of order)
	recvConn.ReadEvents(bitstream.NewReader(buf2))
	if len(recvConn.receivedEvents) != 1 {
		t.Fatalf("Expected 1 event in wait queue, got %d", len(recvConn.receivedEvents))
	}

	// 2. Receive packet 1
	recvConn.ReadEvents(bitstream.NewReader(buf1))
	if len(recvConn.receivedEvents) != 0 {
		t.Fatalf("Expected 0 events in wait queue after processing, got %d", len(recvConn.receivedEvents))
	}

	if recvConn.nextRecvEventSeq != 2 {
		t.Errorf("Expected nextRecvEventSeq 2, got %d", recvConn.nextRecvEventSeq)
	}
}

func TestEventConnection_Retransmission(t *testing.T) {
	addr, _ := netio.ParseAddress("127.0.0.1:28000")
	conn := NewEventConnection(addr, 100)

	ev := &testEvent{EventBase: EventBase{GuaranteeType: Guaranteed}, Data: "Reliable"}
	conn.PostEvent(ev)

	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	conn.WriteEvents(bs, 50) // Packet Seq 50
	bs.Flush()

	if conn.unorderedSendQueue.Len() != 0 {
		t.Fatal("Expected event to be moved to sent map")
	}

	// Simulate packet drop
	note := &PacketNotify{Sequence: 50}
	conn.OnPacketDropped(note)

	if conn.unorderedSendQueue.Len() != 1 {
		t.Fatal("Expected event to be re-queued after drop")
	}
}
