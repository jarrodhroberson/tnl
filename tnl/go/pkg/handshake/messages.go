package handshake

import (
	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
)

// ChallengeRequest (Client -> Server)
type ChallengeRequest struct {
	ClientNonce []byte // 16 bytes
}

func (m *ChallengeRequest) Write(bs *bitstream.BitStream) {
	for _, b := range m.ClientNonce {
		bs.WriteUint8(b)
	}
}

func (m *ChallengeRequest) Read(bs *bitstream.BitStream) {
	m.ClientNonce = make([]byte, 16)
	for i := 0; i < 16; i++ {
		m.ClientNonce[i] = bs.ReadUint8()
	}
}

// ChallengeResponse (Server -> Client)
type ChallengeResponse struct {
	ClientNonce    []byte // 16 bytes
	ServerNonce    []byte // 16 bytes
	ClientIdentity uint32
	Difficulty     uint32
	PublicKey      []byte // ECDH Public Key
}

func (m *ChallengeResponse) Write(bs *bitstream.BitStream) {
	for _, b := range m.ClientNonce {
		bs.WriteUint8(b)
	}
	for _, b := range m.ServerNonce {
		bs.WriteUint8(b)
	}
	bs.WriteUint32(m.ClientIdentity)
	bs.WriteUint32(m.Difficulty)

	bs.WriteRangedUint32(uint32(len(m.PublicKey)), 0, 1024)
	for _, b := range m.PublicKey {
		bs.WriteUint8(b)
	}
}

func (m *ChallengeResponse) Read(bs *bitstream.BitStream) {
	m.ClientNonce = make([]byte, 16)
	for i := 0; i < 16; i++ {
		m.ClientNonce[i] = bs.ReadUint8()
	}
	m.ServerNonce = make([]byte, 16)
	for i := 0; i < 16; i++ {
		m.ServerNonce[i] = bs.ReadUint8()
	}
	m.ClientIdentity = bs.ReadUint32()
	m.Difficulty = bs.ReadUint32()

	pkLen := bs.ReadRangedUint32(0, 1024)
	m.PublicKey = make([]byte, pkLen)
	for i := uint32(0); i < pkLen; i++ {
		m.PublicKey[i] = bs.ReadUint8()
	}
}

// ConnectRequest (Client -> Server)
type ConnectRequest struct {
	ClientNonce     []byte // 16 bytes
	ServerNonce     []byte // 16 bytes
	PuzzleSolution  uint32
	ClientPublicKey []byte
	ConnectionClass string
	Data            []byte // Encrypted connection-specific data
}

func (m *ConnectRequest) Write(bs *bitstream.BitStream) {
	for _, b := range m.ClientNonce {
		bs.WriteUint8(b)
	}
	for _, b := range m.ServerNonce {
		bs.WriteUint8(b)
	}
	bs.WriteUint32(m.PuzzleSolution)

	bs.WriteRangedUint32(uint32(len(m.ClientPublicKey)), 0, 1024)
	for _, b := range m.ClientPublicKey {
		bs.WriteUint8(b)
	}
	bs.WriteString(m.ConnectionClass)

	bs.WriteRangedUint32(uint32(len(m.Data)), 0, 1024)
	for _, b := range m.Data {
		bs.WriteUint8(b)
	}
}

func (m *ConnectRequest) Read(bs *bitstream.BitStream) {
	m.ClientNonce = make([]byte, 16)
	for i := 0; i < 16; i++ {
		m.ClientNonce[i] = bs.ReadUint8()
	}
	m.ServerNonce = make([]byte, 16)
	for i := 0; i < 16; i++ {
		m.ServerNonce[i] = bs.ReadUint8()
	}
	m.PuzzleSolution = bs.ReadUint32()

	pkLen := bs.ReadRangedUint32(0, 1024)
	m.ClientPublicKey = make([]byte, pkLen)
	for i := uint32(0); i < pkLen; i++ {
		m.ClientPublicKey[i] = bs.ReadUint8()
	}
	m.ConnectionClass = bs.ReadString()

	dataLen := bs.ReadRangedUint32(0, 1024)
	m.Data = make([]byte, dataLen)
	for i := uint32(0); i < dataLen; i++ {
		m.Data[i] = bs.ReadUint8()
	}
}
