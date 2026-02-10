package network

import (
	"bytes"
	"testing"

	"github.com/jarrodhroberson/tnl-go/tnl/bitstream"
	"github.com/jarrodhroberson/tnl-go/tnl/netio"
)

type MovingObject struct {
	NetObject
	X, Y, Z float32
}

func (o *MovingObject) PackUpdate(conn *GhostConnection, mask uint32, bs *bitstream.BitStream) uint32 {
	if bs.WriteFlag((mask & 1) != 0) {
		bs.WriteFloat32(o.X)
		bs.WriteFloat32(o.Y)
		bs.WriteFloat32(o.Z)
	}
	return 0 // All bits packed
}

func (o *MovingObject) UnpackUpdate(conn *GhostConnection, bs *bitstream.BitStream) {
	if bs.ReadFlag() {
		o.X = bs.ReadFloat32()
		o.Y = bs.ReadFloat32()
		o.Z = bs.ReadFloat32()
	}
}

func (o *MovingObject) GetUpdatePriority(mask uint32, skips int) float32 {
	return 1.0 + float32(skips)*0.1
}

func (o *MovingObject) OnGhostAdd(conn *GhostConnection) bool { return true }
func (o *MovingObject) OnGhostRemove()                        {}

func TestGhostConnection_Replication(t *testing.T) {
	RegisterObject(200, func() Replicatable { return &MovingObject{} })

	addr, _ := netio.ParseAddress("127.0.0.1:28000")
	serverConn := NewGhostConnection(addr, 100)

	obj := &MovingObject{X: 1.0, Y: 2.0, Z: 3.0}
	obj.NetID = 1000 // Mock NetID

	// 1. Initial Ghosting
	serverConn.ObjectInScope(obj)

	buf1 := &bytes.Buffer{}
	bs1 := bitstream.NewWriter(buf1)
	serverConn.WriteGhosts(bs1, 1)
	bs1.Flush()

	clientConn := NewGhostConnection(addr, 200)
	clientConn.ReadGhosts(bitstream.NewReader(buf1))

	if len(clientConn.ghostsFromRemote) != 1 {
		t.Fatalf("Expected 1 ghost on client, got %d", len(clientConn.ghostsFromRemote))
	}

	// Find the ghost on the client
	var ghostObj *MovingObject
	for _, g := range clientConn.ghostsFromRemote {
		ghostObj = g.(*MovingObject)
	}

	if ghostObj.X != 1.0 || ghostObj.Y != 2.0 || ghostObj.Z != 3.0 {
		t.Errorf("Ghost state mismatch: %v", ghostObj)
	}

	// 2. State Update
	obj.X = 10.0
	gi := serverConn.ghostsToClient[obj.NetID]
	gi.mask |= 1

	buf2 := &bytes.Buffer{}
	bs2 := bitstream.NewWriter(buf2)
	serverConn.WriteGhosts(bs2, 2)
	bs2.Flush()

	clientConn.ReadGhosts(bitstream.NewReader(buf2))
	if ghostObj.X != 10.0 {
		t.Errorf("Expected ghost X=10.0, got %f", ghostObj.X)
	}
}

func TestGhostConnection_Reliability(t *testing.T) {
	addr, _ := netio.ParseAddress("127.0.0.1:28000")
	serverConn := NewGhostConnection(addr, 100)

	obj := &MovingObject{X: 1.0}
	obj.NetID = 1001

	serverConn.ObjectInScope(obj)
	gi := serverConn.ghostsToClient[obj.NetID]
	gi.mask = 1 // Need update

	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	serverConn.WriteGhosts(bs, 50) // Packet 50
	bs.Flush()

	if gi.mask != 0 {
		t.Fatal("Expected mask to be cleared after write")
	}

	// Simulate packet drop
	note := &PacketNotify{Sequence: 50}
	serverConn.OnPacketDropped(note)

	if gi.mask != 1 {
		t.Fatal("Expected mask to be restored after drop")
	}
}
