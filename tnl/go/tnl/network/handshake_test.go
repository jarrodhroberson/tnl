package network

import (
	"bytes"
	"testing"

	"github.com/jarrodhroberson/tnl-go/tnl/bitstream"
	"github.com/jarrodhroberson/tnl-go/tnl/crypto"
	"github.com/jarrodhroberson/tnl-go/tnl/handshake"
	"github.com/jarrodhroberson/tnl-go/tnl/netio"
	"github.com/jarrodhroberson/tnl-go/tnl/protocol"
)

// MockInterface captures sent packets for verification.
type MockInterface struct {
	*netio.Interface
	SentPackets []*netio.Packet
}

func (mi *MockInterface) SendPacket(p *netio.Packet) {
	mi.SentPackets = append(mi.SentPackets, p)
}

func TestHandshake_FullFlow(t *testing.T) {
	hm, _ := NewHandshakeManager()
	mi := &MockInterface{SentPackets: make([]*netio.Packet, 0)}
	clientAddr, _ := netio.ParseAddress("1.2.3.4:5678")

	// --- 1. Client sends ChallengeRequest ---
	clientNonce := []byte("client-nonce-16b")
	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	bs.WriteUint8(uint8(protocol.ConnectChallengeRequest))
	req := &handshake.ChallengeRequest{ClientNonce: clientNonce}
	req.Write(bs)
	bs.Flush()

	hm.HandlePacket(mi, &netio.Packet{Addr: clientAddr, Data: buf.Bytes()})

	if len(mi.SentPackets) != 1 {
		t.Fatalf("Expected 1 response packet, got %d", len(mi.SentPackets))
	}

	// --- 2. Client receives ChallengeResponse ---
	respData := mi.SentPackets[0].Data
	rbs := bitstream.NewReader(bytes.NewReader(respData))
	if rbs.ReadUint8() != uint8(protocol.ConnectChallengeResponse) {
		t.Fatal("Expected ChallengeResponse type")
	}
	resp := &handshake.ChallengeResponse{}
	resp.Read(rbs)

	// --- 3. Client solves puzzle and sends ConnectRequest ---
	puzzle := crypto.NewPuzzle()
	solution := puzzle.SolvePuzzle(resp.ClientIdentity, resp.ClientNonce, resp.ServerNonce, resp.Difficulty)

	clientKX, _ := crypto.NewKeyExchange()

	connReq := &handshake.ConnectRequest{
		ClientNonce:     resp.ClientNonce,
		ServerNonce:     resp.ServerNonce,
		PuzzleSolution:  solution,
		ClientPublicKey: clientKX.PublicKey(),
		ConnectionClass: "GameConnection",
		Data:            []byte("initial-data"),
	}

	buf2 := &bytes.Buffer{}
	bs2 := bitstream.NewWriter(buf2)
	bs2.WriteUint8(uint8(protocol.ConnectRequest))
	connReq.Write(bs2)
	bs2.Flush()

	mi.SentPackets = nil // Clear for next response
	hm.HandlePacket(mi, &netio.Packet{Addr: clientAddr, Data: buf2.Bytes()})

	if len(mi.SentPackets) != 1 {
		t.Fatalf("Expected ConnectAccept response, got %d", len(mi.SentPackets))
	}

	// --- 4. Client receives ConnectAccept ---
	acceptData := mi.SentPackets[0].Data
	abs := bitstream.NewReader(bytes.NewReader(acceptData))
	if abs.ReadUint8() != uint8(protocol.ConnectAccept) {
		t.Fatal("Expected ConnectAccept type")
	}
	accept := &handshake.ConnectAccept{}
	accept.Read(abs)

	if accept.InitialSendSeq != 1000 {
		t.Errorf("Expected initial seq 1000, got %d", accept.InitialSendSeq)
	}
}
