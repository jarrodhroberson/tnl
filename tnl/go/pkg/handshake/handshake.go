package handshake

import (
	"crypto/hmac"
	"crypto/sha256"
	"encoding/binary"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
)

// ConnectAccept (Server -> Client)
type ConnectAccept struct {
	InitialSendSeq uint32
	InitialRecvSeq uint32
	SessionIV      []byte // For AES
	Data           []byte // Encrypted class data
}

func (m *ConnectAccept) Write(bs *bitstream.BitStream) {
	bs.WriteUint32(m.InitialSendSeq)
	bs.WriteUint32(m.InitialRecvSeq)

	bs.WriteRangedUint32(uint32(len(m.SessionIV)), 0, 1024)
	for _, b := range m.SessionIV {
		bs.WriteUint8(b)
	}

	bs.WriteRangedUint32(uint32(len(m.Data)), 0, 1024)
	for _, b := range m.Data {
		bs.WriteUint8(b)
	}
}

func (m *ConnectAccept) Read(bs *bitstream.BitStream) {
	m.InitialSendSeq = bs.ReadUint32()
	m.InitialRecvSeq = bs.ReadUint32()

	ivLen := bs.ReadRangedUint32(0, 1024)
	m.SessionIV = make([]byte, ivLen)
	for i := uint32(0); i < ivLen; i++ {
		m.SessionIV[i] = bs.ReadUint8()
	}

	dataLen := bs.ReadRangedUint32(0, 1024)
	m.Data = make([]byte, dataLen)
	for i := uint32(0); i < dataLen; i++ {
		m.Data[i] = bs.ReadUint8()
	}
}

// ConnectReject (Server -> Client)
type ConnectReject struct {
	Reason string
}

func (m *ConnectReject) Write(bs *bitstream.BitStream) {
	bs.WriteString(m.Reason)
}

func (m *ConnectReject) Read(bs *bitstream.BitStream) {
	m.Reason = bs.ReadString()
}

// IdentityCookie generates a stateless identity for a client.
func ComputeIdentity(addr string, serverSecret []byte, serverNonce []byte) uint32 {
	h := hmac.New(sha256.New, serverSecret)
	h.Write([]byte(addr))
	h.Write(serverNonce)
	sum := h.Sum(nil)
	return binary.BigEndian.Uint32(sum[:4])
}
