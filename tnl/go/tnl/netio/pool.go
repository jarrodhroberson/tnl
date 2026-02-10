package netio

import (
	"sync"

	"github.com/jarrodhroberson/tnl-go/tnl/protocol"
)

// PacketPool provides a pool of byte slices for packet data.
var PacketPool = sync.Pool{
	New: func() interface{} {
		return make([]byte, protocol.MaxPacketDataSize)
	},
}

// GetPacketBuffer retrieves a buffer from the pool.
func GetPacketBuffer() []byte {
	return PacketPool.Get().([]byte)
}

// PutPacketBuffer returns a buffer to the pool.
func PutPacketBuffer(b []byte) {
	if cap(b) >= protocol.MaxPacketDataSize {
		PacketPool.Put(b[:protocol.MaxPacketDataSize])
	}
}
