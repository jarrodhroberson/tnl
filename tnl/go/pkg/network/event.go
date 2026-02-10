package network

import (
	"fmt"
	"reflect"
	"sync"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
)

// GuaranteeType defines the delivery guarantee for a NetEvent.
type GuaranteeType int

const (
	Unguaranteed      GuaranteeType = 0
	Guaranteed        GuaranteeType = 1
	GuaranteedOrdered GuaranteeType = 2
)

// EventDirection defines the allowed direction for a NetEvent.
type EventDirection int

const (
	DirAny            EventDirection = 0
	DirServerToClient EventDirection = 1
	DirClientToServer EventDirection = 2
)

// Event is the interface for all network events.
type Event interface {
	Pack(conn *NetConnection, bs *bitstream.BitStream)
	Unpack(conn *NetConnection, bs *bitstream.BitStream)
	Process(conn *EventConnection)
	GetGuaranteeType() GuaranteeType
	GetDirection() EventDirection
}

// EventBase provides a default implementation for common Event methods.
type EventBase struct {
	GuaranteeType GuaranteeType
	Direction     EventDirection
}

func (e *EventBase) GetGuaranteeType() GuaranteeType { return e.GuaranteeType }
func (e *EventBase) GetDirection() EventDirection    { return e.Direction }

// EventRegistry manages the mapping of NetClass IDs to Event types.
type EventRegistry struct {
	mu        sync.RWMutex
	factories map[uint32]func() Event
}

var globalRegistry = &EventRegistry{
	factories: make(map[uint32]func() Event),
}

// RegisterEvent adds a new Event type to the registry.
func RegisterEvent(id uint32, factory func() Event) {
	globalRegistry.mu.Lock()
	defer globalRegistry.mu.Unlock()
	globalRegistry.factories[id] = factory
}

// CreateEvent instantiates a new Event based on its ID.
func CreateEvent(id uint32) (Event, error) {
	globalRegistry.mu.RLock()
	factory, ok := globalRegistry.factories[id]
	globalRegistry.mu.RUnlock()

	if !ok {
		return nil, fmt.Errorf("unknown event ID: %d", id)
	}

	return factory(), nil
}

// GetEventID should be handled by the Event itself or a fixed mapping
// For simplicity, we'll let the user provide it for now or use a simple map.
func GetEventID(ev Event) (uint32, bool) {
	// For now, we'll just check the registered factories IDs.
	// In a real implementation, we'd want a more efficient reverse map.
	globalRegistry.mu.RLock()
	defer globalRegistry.mu.RUnlock()

	for id, factory := range globalRegistry.factories {
		if reflect.TypeOf(factory()) == reflect.TypeOf(ev) {
			return id, true
		}
	}
	return 0, false
}
