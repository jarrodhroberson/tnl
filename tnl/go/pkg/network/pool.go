package network

import (
	"sync"
)

// NotifyPool provides a pool of PacketNotify objects.
var NotifyPool = sync.Pool{
	New: func() interface{} {
		return &PacketNotify{}
	},
}

// GetPacketNotify retrieves a notify object from the pool.
func GetPacketNotify() *PacketNotify {
	return NotifyPool.Get().(*PacketNotify)
}

// PutPacketNotify returns a notify object to the pool.
func PutPacketNotify(n *PacketNotify) {
	*n = PacketNotify{} // Clear for reuse
	NotifyPool.Put(n)
}
