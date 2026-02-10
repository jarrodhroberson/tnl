package network

import (
	"bytes"
	"fmt"
	"sort"
	"sync"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
	"github.com/jarrodhroberson/tnl-go/pkg/protocol"
)

// GhostInfo tracks the state of a ghost on a specific connection.
type GhostInfo struct {
	obj       Replicatable
	ghostID   uint32
	mask      uint32
	priority  float32
	skips     int
	isNew     bool
	isDeleted bool
}

// ghostUpdateNote tracks a ghost update sent in a packet.
type ghostUpdateNote struct {
	ghostID uint32
	mask    uint32
}

// GhostConnection handles state replication and scoping.
type GhostConnection struct {
	*EventConnection

	// Server side: Objects currently in scope for this client
	ghostsToClient map[uint32]*GhostInfo // NetID -> GhostInfo

	// Client side: Ghosts received from the server
	ghostsFromRemote map[uint32]Replicatable // GhostID -> Replicatable

	// Mapping for GhostIDs (0-1023 typically)
	nextGhostID uint32

	// Reliability Tracking
	sentGhostUpdates map[uint32][]*ghostUpdateNote // packetSeq -> updates

	mu sync.Mutex
}

func NewGhostConnection(addr netio.Address, initialSendSeq uint32) *GhostConnection {
	gc := &GhostConnection{
		EventConnection:  NewEventConnection(addr, initialSendSeq),
		ghostsToClient:   make(map[uint32]*GhostInfo),
		ghostsFromRemote: make(map[uint32]Replicatable),
		nextGhostID:      1,
		sentGhostUpdates: make(map[uint32][]*ghostUpdateNote),
	}

	// Hook into EventConnection's callbacks (which were set in NewEventConnection)
	oldRecv := gc.OnPacketReceived
	oldDrop := gc.OnPacketDropped

	gc.OnPacketReceived = func(note *PacketNotify) {
		if oldRecv != nil {
			oldRecv(note)
		}
		gc.onGhostPacketReceived(note)
	}
	gc.OnPacketDropped = func(note *PacketNotify) {
		if oldDrop != nil {
			oldDrop(note)
		}
		gc.onGhostPacketDropped(note)
	}

	return gc
}

func (gc *GhostConnection) onGhostPacketReceived(note *PacketNotify) {
	gc.mu.Lock()
	defer gc.mu.Unlock()
	delete(gc.sentGhostUpdates, note.Sequence)
}

func (gc *GhostConnection) onGhostPacketDropped(note *PacketNotify) {
	gc.mu.Lock()
	defer gc.mu.Unlock()

	updates, ok := gc.sentGhostUpdates[note.Sequence]
	if !ok {
		return
	}

	for _, up := range updates {
		for _, gi := range gc.ghostsToClient {
			if gi.ghostID == up.ghostID {
				gi.mask |= up.mask
				break
			}
		}
	}
	delete(gc.sentGhostUpdates, note.Sequence)
}

// ObjectInScope adds an object to the connection's scope.
func (gc *GhostConnection) ObjectInScope(obj Replicatable) {
	gc.mu.Lock()
	defer gc.mu.Unlock()

	id := obj.GetNetID()
	if _, ok := gc.ghostsToClient[id]; !ok {
		gc.ghostsToClient[id] = &GhostInfo{
			obj:     obj,
			ghostID: gc.nextGhostID,
			mask:    0xFFFFFFFF, // Initial update
			isNew:   true,
		}
		gc.nextGhostID = (gc.nextGhostID % 1024) + 1
	}
}

// WriteGhosts writes prioritized ghost updates to the bitstream.
func (gc *GhostConnection) WriteGhosts(bs *bitstream.BitStream, packetSeq uint32) {
	gc.mu.Lock()
	defer gc.mu.Unlock()

	// 1. Prioritize ghosts
	var sorted []*GhostInfo
	for _, gi := range gc.ghostsToClient {
		// Update priority
		gi.priority = gi.obj.GetUpdatePriority(gi.mask, gi.skips)
		sorted = append(sorted, gi)
	}

	sort.Slice(sorted, func(i, j int) bool {
		return sorted[i].priority > sorted[j].priority
	})

	var sent []*ghostUpdateNote

	// 2. Write updates
	for _, gi := range sorted {
		if gi.mask == 0 {
			gi.skips++
			continue
		}

		if bs.WriteFlag(true) {
			bs.WriteRangedUint32(gi.ghostID, 0, 1024)

			if bs.WriteFlag(gi.isNew) {
				// Write type ID for instantiation
				if typeID, ok := GetObjectFactoryID(gi.obj); ok {
					bs.WriteRangedUint32(typeID, 0, 1024)
				}
				gi.isNew = false
			}

			// Pack the update and clear the bits that were successfully packed
			packedMask := gi.mask
			remainingMask := gi.obj.PackUpdate(gc, packedMask, bs)

			// The bits that WERE packed are (packedMask & ^remainingMask)
			sentMask := packedMask & ^remainingMask
			if sentMask != 0 {
				sent = append(sent, &ghostUpdateNote{ghostID: gi.ghostID, mask: sentMask})
			}

			gi.mask = remainingMask
			gi.skips = 0
		}
	}
	bs.WriteFlag(false) // End of ghosts

	if len(sent) > 0 {
		gc.sentGhostUpdates[packetSeq] = sent
	}
}

// ReadGhosts reads ghost updates from the bitstream.
func (gc *GhostConnection) ReadGhosts(bs *bitstream.BitStream) {
	gc.mu.Lock()
	defer gc.mu.Unlock()

	for bs.ReadFlag() {
		ghostID := bs.ReadRangedUint32(0, 1024)
		isNew := bs.ReadFlag()

		var obj Replicatable
		if isNew {
			typeID := bs.ReadRangedUint32(0, 1024)
			var err error
			obj, err = CreateObject(typeID)
			if err != nil {
				fmt.Printf("Error creating ghost: %v\n", err)
				continue
			}
			gc.ghostsFromRemote[ghostID] = obj
		} else {
			var ok bool
			obj, ok = gc.ghostsFromRemote[ghostID]
			if !ok {
				fmt.Printf("Error: ghost %d not found\n", ghostID)
				continue
			}
		}

		obj.UnpackUpdate(gc, bs)
		if isNew {
			obj.OnGhostAdd(gc)
		}
	}
}

func (gc *GhostConnection) ProcessPacket(data []byte) {
	buf := bytes.NewReader(data)
	bs := bitstream.NewReader(buf)

	pktType, _, ok := gc.ReadPacketHeader(bs)
	if !ok {
		return
	}

	if pktType == protocol.DataPacket {
		gc.ReadEvents(bs)
		gc.ReadGhosts(bs)
	}
}
