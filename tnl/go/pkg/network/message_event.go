package network

import (
	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
)

// MessageEvent is a simple reliable ordered event for sending strings.
type MessageEvent struct {
	EventBase
	Message string
}

var OnMessageReceived func(conn *EventConnection, msg string)

func NewMessageEvent(msg string) *MessageEvent {
	return &MessageEvent{
		EventBase: EventBase{
			GuaranteeType: GuaranteedOrdered,
			Direction:     DirAny,
		},
		Message: msg,
	}
}

func (e *MessageEvent) Pack(conn *NetConnection, bs *bitstream.BitStream) {
	bs.WriteString(e.Message)
}

func (e *MessageEvent) Unpack(conn *NetConnection, bs *bitstream.BitStream) {
	e.Message = bs.ReadString()
}

func (e *MessageEvent) Process(conn *EventConnection) {
	if OnMessageReceived != nil {
		OnMessageReceived(conn, e.Message)
	}
}

func init() {
	RegisterEvent(1, func() Event { return NewMessageEvent("") })
}
