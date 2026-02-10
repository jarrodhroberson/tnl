package network

import (
	"fmt"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
)

// MessageEvent is a simple reliable ordered event for sending strings.
type MessageEvent struct {
	EventBase
	Message string
}

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

func (e *MessageEvent) Process(conn *NetConnection) {
	fmt.Printf("Message from %s: %s\n", conn.Addr.String(), e.Message)
}

func init() {
	RegisterEvent(1, func() Event { return NewMessageEvent("") })
}
