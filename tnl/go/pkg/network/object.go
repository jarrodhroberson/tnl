package network

import (
	"fmt"
	"reflect"
	"sync"
	"sync/atomic"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
)

// Replicatable is the interface for objects that can be ghosted across connections.
type Replicatable interface {
	PackUpdate(conn *GhostConnection, mask uint32, bs *bitstream.BitStream) uint32
	UnpackUpdate(conn *GhostConnection, bs *bitstream.BitStream)
	GetUpdatePriority(mask uint32, skips int) float32
	OnGhostAdd(conn *GhostConnection) bool
	OnGhostRemove()
	GetNetID() uint32
}

var nextNetID uint32 = 1

// NetObject is the base implementation of Replicatable.
type NetObject struct {
	NetID     uint32
	dirtyMask uint32
	mu        sync.Mutex
}

func NewNetObject() *NetObject {
	return &NetObject{
		NetID: atomic.AddUint32(&nextNetID, 1),
	}
}

func (n *NetObject) GetNetID() uint32 {
	return n.NetID
}

func (n *NetObject) SetMaskBits(bits uint32) {
	n.mu.Lock()
	n.dirtyMask |= bits
	n.mu.Unlock()
	// In a real implementation, we'd also add this to a global "dirty list"
	// but for now, the GhostConnection will scan objects it knows about.
}

func (n *NetObject) ClearMaskBits(bits uint32) {
	n.mu.Lock()
	n.dirtyMask &= ^bits
	n.mu.Unlock()
}

func (n *NetObject) GetMask() uint32 {
	n.mu.Lock()
	defer n.mu.Unlock()
	return n.dirtyMask
}

// GlobalObjectRegistry for ghost instantiation.
type ObjectRegistry struct {
	mu        sync.RWMutex
	factories map[uint32]func() Replicatable
}

var globalObjRegistry = &ObjectRegistry{
	factories: make(map[uint32]func() Replicatable),
}

func RegisterObject(id uint32, factory func() Replicatable) {
	globalObjRegistry.mu.Lock()
	defer globalObjRegistry.mu.Unlock()
	globalObjRegistry.factories[id] = factory
}

func CreateObject(id uint32) (Replicatable, error) {
	globalObjRegistry.mu.RLock()
	factory, ok := globalObjRegistry.factories[id]
	globalObjRegistry.mu.RUnlock()

	if !ok {
		return nil, fmt.Errorf("unknown object type ID: %d", id)
	}

	return factory(), nil
}

func GetObjectFactoryID(obj Replicatable) (uint32, bool) {
	globalObjRegistry.mu.RLock()
	defer globalObjRegistry.mu.RUnlock()

	for id, factory := range globalObjRegistry.factories {
		if reflect.TypeOf(factory()) == reflect.TypeOf(obj) {
			return id, true
		}
	}
	return 0, false
}
