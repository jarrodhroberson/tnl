package network

import (
	"container/list"
	"fmt"
	"sync"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
)

// eventNote tracks an event and its sequence for ordering.
type eventNote struct {
	event    Event
	sequence uint32
}

// EventConnection extends NetConnection with event processing capabilities.
type EventConnection struct {
	*NetConnection

	// Send Queues
	unorderedSendQueue *list.List
	orderedSendQueue   *list.List

	// Receive Queues
	waitSeqQueue *list.List

	// Sequence Management
	nextSendEventSeq uint32
	nextRecvEventSeq uint32

	// Reliability Tracking
	sentEvents     map[uint32][]*eventNote // packetSeq -> events
	receivedEvents map[uint32]Event        // seq -> event (for ordering)

	mu sync.Mutex
}

func NewEventConnection(addr netio.Address, initialSendSeq uint32) *EventConnection {
	ec := &EventConnection{
		NetConnection:      NewNetConnection(addr, initialSendSeq),
		unorderedSendQueue: list.New(),
		orderedSendQueue:   list.New(),
		waitSeqQueue:       list.New(),
		sentEvents:         make(map[uint32][]*eventNote),
		receivedEvents:     make(map[uint32]Event),
	}
	ec.OnPacketReceived = ec.onPacketReceived
	ec.OnPacketDropped = ec.onPacketDropped
	return ec
}

func (ec *EventConnection) onPacketReceived(note *PacketNotify) {
	ec.mu.Lock()
	defer ec.mu.Unlock()
	delete(ec.sentEvents, note.Sequence)
}

func (ec *EventConnection) onPacketDropped(note *PacketNotify) {
	ec.mu.Lock()
	defer ec.mu.Unlock()

	events, ok := ec.sentEvents[note.Sequence]
	if !ok {
		return
	}

	// Re-queue guaranteed events
	for _, note := range events {
		if note.event.GetGuaranteeType() != Unguaranteed {
			if note.event.GetGuaranteeType() == GuaranteedOrdered {
				ec.orderedSendQueue.PushFront(note)
			} else {
				ec.unorderedSendQueue.PushFront(note)
			}
		}
	}
	delete(ec.sentEvents, note.Sequence)
}

// PostEvent queues an event to be sent to the remote host.
func (ec *EventConnection) PostEvent(ev Event) {
	ec.mu.Lock()
	defer ec.mu.Unlock()

	if ev.GetGuaranteeType() == GuaranteedOrdered {
		ec.orderedSendQueue.PushBack(&eventNote{
			event:    ev,
			sequence: ec.nextSendEventSeq,
		})
		ec.nextSendEventSeq++
	} else {
		ec.unorderedSendQueue.PushBack(&eventNote{
			event: ev,
		})
	}
}

// WriteEvents writes pending events into the bitstream.
func (ec *EventConnection) WriteEvents(bs *bitstream.BitStream, packetSeq uint32) {
	ec.mu.Lock()
	defer ec.mu.Unlock()

	var sent []*eventNote

	// 1. Write Unordered Events
	for e := ec.unorderedSendQueue.Front(); e != nil; {
		next := e.Next()
		note := e.Value.(*eventNote)

		if bs.WriteFlag(true) {
			ec.writeEvent(bs, note)
			if note.event.GetGuaranteeType() == Unguaranteed {
				ec.unorderedSendQueue.Remove(e)
			} else {
				sent = append(sent, note)
				ec.unorderedSendQueue.Remove(e)
			}
		}
		e = next
	}
	bs.WriteFlag(false)

	// 2. Write Ordered Events
	for e := ec.orderedSendQueue.Front(); e != nil; {
		next := e.Next()
		note := e.Value.(*eventNote)

		if bs.WriteFlag(true) {
			ec.writeEvent(bs, note)
			sent = append(sent, note)
			ec.orderedSendQueue.Remove(e)
		}
		e = next
	}
	bs.WriteFlag(false)

	if len(sent) > 0 {
		ec.sentEvents[packetSeq] = sent
	}
}

func (ec *EventConnection) writeEvent(bs *bitstream.BitStream, note *eventNote) {
	id, ok := GetEventID(note.event)
	if !ok {
		panic(fmt.Sprintf("unregistered event type: %T", note.event))
	}

	bs.WriteRangedUint32(id, 0, 1024)
	if note.event.GetGuaranteeType() == GuaranteedOrdered {
		bs.WriteUint32(note.sequence)
	}
	note.event.Pack(ec.NetConnection, bs)
}

// ReadEvents reads events from the bitstream and processes them.
func (ec *EventConnection) ReadEvents(bs *bitstream.BitStream) {
	ec.mu.Lock()
	defer ec.mu.Unlock()

	// Read Unordered
	for bs.ReadFlag() {
		ev := ec.readEvent(bs, nil)
		if ev != nil {
			ev.Process(ec.NetConnection)
		}
	}

	// Read Ordered
	for bs.ReadFlag() {
		var seq uint32
		ev := ec.readEvent(bs, &seq)
		if ev != nil {
			ec.receivedEvents[seq] = ev
		}
	}

	// Process any ready ordered events
	for {
		if ev, ok := ec.receivedEvents[ec.nextRecvEventSeq]; ok {
			ev.Process(ec.NetConnection)
			delete(ec.receivedEvents, ec.nextRecvEventSeq)
			ec.nextRecvEventSeq++
		} else {
			break
		}
	}
}

func (ec *EventConnection) readEvent(bs *bitstream.BitStream, outSeq *uint32) Event {
	id := bs.ReadRangedUint32(0, 1024)
	ev, err := CreateEvent(id)
	if err != nil {
		return nil
	}

	if ev.GetGuaranteeType() == GuaranteedOrdered {
		seq := bs.ReadUint32()
		if outSeq != nil {
			*outSeq = seq
		}
	}

	ev.Unpack(ec.NetConnection, bs)
	return ev
}
