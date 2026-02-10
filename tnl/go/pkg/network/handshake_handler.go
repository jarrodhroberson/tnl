package network

import (
	"bytes"
	"crypto/rand"
	"io"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/crypto"
	"github.com/jarrodhroberson/tnl-go/pkg/handshake"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
	"github.com/jarrodhroberson/tnl-go/pkg/protocol"
	"github.com/rs/zerolog/log"
)

// HandshakeManager manages the server-side handshake state.
type HandshakeManager struct {
	serverSecret     []byte
	currentNonce     []byte
	puzzleDifficulty uint32
	kx               *crypto.KeyExchange

	// Callbacks
	OnConnectionAccepted func(addr netio.Address, sharedSecret []byte, iv []byte)
}

func NewHandshakeManager() (*HandshakeManager, error) {
	secret := make([]byte, 32)
	io.ReadFull(rand.Reader, secret)
	nonce := make([]byte, 16)
	io.ReadFull(rand.Reader, nonce)

	kx, err := crypto.NewKeyExchange()
	if err != nil {
		return nil, err
	}

	return &HandshakeManager{
		serverSecret:     secret,
		currentNonce:     nonce,
		puzzleDifficulty: 17, // Default difficulty
		kx:               kx,
	}, nil
}

func (hm *HandshakeManager) HandlePacket(s netio.PacketSender, p *netio.Packet) {
	bs := bitstream.NewReader(bytes.NewReader(p.Data))
	pktType := protocol.PacketType(bs.ReadUint8())

	switch pktType {
	case protocol.ConnectChallengeRequest:
		hm.handleChallengeRequest(s, p, bs)
	case protocol.ConnectRequest:
		hm.handleConnectRequest(s, p, bs)
	default:
		log.Debug().Uint8("type", uint8(pktType)).Str("addr", p.Addr.String()).Msg("unknown handshake packet type")
	}
}

func (hm *HandshakeManager) handleChallengeRequest(s netio.PacketSender, p *netio.Packet, bs *bitstream.BitStream) {
	req := &handshake.ChallengeRequest{}
	req.Read(bs)

	resp := &handshake.ChallengeResponse{
		ClientNonce:    req.ClientNonce,
		ServerNonce:    hm.currentNonce,
		ClientIdentity: handshake.ComputeIdentity(p.Addr.String(), hm.serverSecret, hm.currentNonce),
		Difficulty:     hm.puzzleDifficulty,
		PublicKey:      hm.kx.PublicKey(),
	}

	buf := &bytes.Buffer{}
	out := bitstream.NewWriter(buf)
	out.WriteUint8(uint8(protocol.ConnectChallengeResponse))
	resp.Write(out)
	out.Flush()

	s.SendPacket(&netio.Packet{Addr: p.Addr, Data: buf.Bytes()})
}

func (hm *HandshakeManager) handleConnectRequest(s netio.PacketSender, p *netio.Packet, bs *bitstream.BitStream) {
	req := &handshake.ConnectRequest{}
	req.Read(bs)

	// 1. Verify Identity
	expectedIc := handshake.ComputeIdentity(p.Addr.String(), hm.serverSecret, hm.currentNonce)
	if req.ClientNonce == nil || req.ServerNonce == nil {
		return
	}

	// 2. Verify Puzzle
	puzzle := crypto.NewPuzzle()
	if !puzzle.CheckSolution(req.PuzzleSolution, expectedIc, req.ClientNonce, req.ServerNonce, hm.puzzleDifficulty) {
		log.Warn().Str("addr", p.Addr.String()).Msg("invalid puzzle solution")
		return
	}

	// 3. Compute Shared Secret
	sharedSecret, err := hm.kx.ComputeSharedSecret(req.ClientPublicKey)
	if err != nil {
		log.Error().Err(err).Msg("failed to compute shared secret")
		return
	}

	// 4. Create Connection (Phase 4/5 integration will happen here)
	// For now, send Accept
	_ = sharedSecret // Silence lint until integrated with NetConnection session keys
	accept := &handshake.ConnectAccept{
		InitialSendSeq: 1000,
		InitialRecvSeq: 2000,
		SessionIV:      make([]byte, 12), // AES-GCM nonce size
		Data:           []byte("welcome"),
	}
	io.ReadFull(rand.Reader, accept.SessionIV)

	buf := &bytes.Buffer{}
	out := bitstream.NewWriter(buf)
	out.WriteUint8(uint8(protocol.ConnectAccept))
	accept.Write(out)
	out.Flush()

	s.SendPacket(&netio.Packet{Addr: p.Addr, Data: buf.Bytes()})

	if hm.OnConnectionAccepted != nil {
		hm.OnConnectionAccepted(p.Addr, sharedSecret, accept.SessionIV)
	}

	log.Info().Str("addr", p.Addr.String()).Msg("connection accepted")
}
