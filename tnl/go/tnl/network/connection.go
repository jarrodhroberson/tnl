package network

import (
	"sync"
	"time"

	"github.com/jarrodhroberson/tnl-go/tnl/bitstream"
	"github.com/jarrodhroberson/tnl-go/tnl/netio"
	"github.com/jarrodhroberson/tnl-go/tnl/protocol"
	"github.com/rs/zerolog/log"
)

// PacketNotify tracks info about a sent packet to handle acks/nacks.
type PacketNotify struct {
	Sequence uint32
	SendTime time.Time
}

// NetConnection handles the Notify protocol over an unreliable UDP transport.
type NetConnection struct {
	Addr netio.Address

	// Notify Protocol State
	initialSendSeq  uint32
	initialRecvSeq  uint32
	lastSendSeq     uint32
	lastRecvSeq     uint32
	highestAckedSeq uint32
	ackMask         uint32 // 32 bits for MaxPacketWindowSize = 32
	lastRecvAckAck  uint32 // The highest seq we know the other side has acked

	// Slotted window for notify objects
	notifyQueue        map[uint32]*PacketNotify
	lastSeqRecvdAtSend [protocol.MaxPacketWindowSize]uint32

	mu sync.Mutex

	// Connection Stats
	roundTripTime      time.Duration
	lastPacketRecvTime time.Time

	// Adaptive Protocol (TCP-like)
	cwnd     float32
	ssthresh float32

	// Callbacks
	OnPacketReceived func(note *PacketNotify)
	OnPacketDropped  func(note *PacketNotify)

	// Termination
	state protocol.ConnectionState

	// Tag is an application-defined value associated with the connection.
	Tag interface{}
}

func NewNetConnection(addr netio.Address, initialSendSeq uint32) *NetConnection {
	return &NetConnection{
		Addr:               addr,
		initialSendSeq:     initialSendSeq,
		lastSendSeq:        initialSendSeq,
		highestAckedSeq:    initialSendSeq,
		notifyQueue:        make(map[uint32]*PacketNotify),
		lastPacketRecvTime: time.Now(),
		state:              protocol.NotConnected,
		cwnd:               2,
		ssthresh:           30,
	}
}

func (nc *NetConnection) SetInitialRecvSequence(seq uint32) {
	nc.mu.Lock()
	defer nc.mu.Unlock()
	nc.initialRecvSeq = seq
	nc.lastRecvSeq = seq
	nc.lastRecvAckAck = seq
}

// ReadPacketHeader parses the TNL notify header and processes acks.
func (nc *NetConnection) ReadPacketHeader(bs *bitstream.BitStream) (protocol.NetPacketType, uint32, bool) {
	nc.mu.Lock()
	defer nc.mu.Unlock()

	pktType := protocol.NetPacketType(bs.ReadBits(2))
	seqLow := uint32(bs.ReadBits(5))
	isGameData := bs.ReadFlag()
	seqHigh := uint32(bs.ReadBits(protocol.SequenceNumberBitSize - 5))

	pkSequenceNumber := seqLow | (seqHigh << 5)
	pkHighestAck := uint32(bs.ReadBits(protocol.AckSequenceNumberBitSize))

	// Pad bits check (TNL expects 0)
	if bs.ReadBits(protocol.PacketHeaderPadBits) != 0 {
		return protocol.InvalidPacketType, 0, false
	}

	if !isGameData {
		return protocol.InvalidPacketType, 0, false
	}

	// Reconstruct sequence number (handle wrap)
	pkSequenceNumber |= (nc.lastRecvSeq & protocol.SequenceNumberMask)
	if pkSequenceNumber < nc.lastRecvSeq {
		pkSequenceNumber += protocol.SequenceNumberWindowSize
	}

	if pkSequenceNumber-nc.lastRecvSeq > (protocol.MaxPacketWindowSize - 1) {
		log.Debug().Str("addr", nc.Addr.String()).
			Uint32("seq", pkSequenceNumber).
			Uint32("lastRecv", nc.lastRecvSeq).
			Msg("packet rejected: outside receive window")
		return protocol.InvalidPacketType, 0, false // Outside window
	}

	// Reconstruct highest ack (handle wrap)
	pkHighestAck |= (nc.highestAckedSeq & protocol.AckSequenceNumberMask)
	if pkHighestAck < nc.highestAckedSeq {
		pkHighestAck += protocol.AckSequenceNumberWindowSize
	}

	if pkHighestAck > nc.lastSendSeq {
		return protocol.InvalidPacketType, 0, false // Outside window
	}

	// Ack Mask Reading
	ackByteCount := uint32(bs.ReadRangedUint32(0, protocol.MaxAckByteCount))
	// For simplicity, we assume MaxAckByteCount is 4 here as per TNL default MaxPacketWindowSize=32
	pkAckMask := uint32(bs.ReadBits(int(ackByteCount * 8)))

	// Send Delay
	sendDelay := (time.Duration(bs.ReadBits(8)) << 3) * time.Millisecond

	// Process notifies (Acks)
	nc.processAcks(pkHighestAck, pkAckMask, sendDelay)

	// Update local ack mask (for packets received from remote)
	shift := pkSequenceNumber - nc.lastRecvSeq
	if shift > 0 {
		if shift >= 32 {
			nc.ackMask = 0
		} else {
			nc.ackMask <<= shift
		}
		if pktType == protocol.DataPacket {
			nc.ackMask |= 1
		}
	}

	nc.lastRecvSeq = pkSequenceNumber
	nc.highestAckedSeq = pkHighestAck
	nc.lastPacketRecvTime = time.Now()

	return pktType, pkSequenceNumber, true
}

func (nc *NetConnection) processAcks(pkHighestAck uint32, pkAckMask uint32, sendDelay time.Duration) {
	notifyCount := pkHighestAck - nc.highestAckedSeq
	for i := uint32(0); i < notifyCount; i++ {
		notifyIndex := nc.highestAckedSeq + i + 1
		bit := (pkHighestAck - notifyIndex) & 0x1F
		success := (pkAckMask & (1 << bit)) != 0

		if note, ok := nc.notifyQueue[notifyIndex&protocol.PacketWindowMask]; ok {
			if success {
				// Estimate RTT
				rtt := time.Since(note.SendTime) - sendDelay
				if rtt < 0 {
					rtt = 0
				}
				if nc.roundTripTime == 0 {
					nc.roundTripTime = rtt
				} else {
					nc.roundTripTime = (nc.roundTripTime*9 + rtt) / 10
				}

				// Adaptive logic (slow start)
				if nc.cwnd < nc.ssthresh {
					nc.cwnd++
				} else {
					nc.cwnd += 1.0 / nc.cwnd
				}

				nc.lastRecvAckAck = nc.lastSeqRecvdAtSend[notifyIndex&protocol.PacketWindowMask]
				nc.packetReceived(note)
			} else {
				// Packet dropped logic
				nc.ssthresh = nc.cwnd / 2
				if nc.ssthresh < 2 {
					nc.ssthresh = 2
				}
				nc.cwnd = nc.ssthresh
				nc.packetDropped(note)
			}

			delete(nc.notifyQueue, notifyIndex&protocol.PacketWindowMask)
			PutPacketNotify(note)
		}
	}
}

func (nc *NetConnection) packetReceived(note *PacketNotify) {
	if nc.OnPacketReceived != nil {
		nc.OnPacketReceived(note)
	}
}

func (nc *NetConnection) packetDropped(note *PacketNotify) {
	if nc.OnPacketDropped != nil {
		nc.OnPacketDropped(note)
	}
}

// WritePacketHeader writes the TNL notify header into the stream.
func (nc *NetConnection) WritePacketHeader(bs *bitstream.BitStream, packetType protocol.NetPacketType) {
	nc.mu.Lock()
	defer nc.mu.Unlock()

	if packetType == protocol.DataPacket {
		nc.lastSendSeq++
	}

	seq := nc.lastSendSeq & (protocol.SequenceNumberWindowSize - 1)

	bs.WriteBits(uint64(packetType), 2)
	bs.WriteBits(uint64(seq&0x1F), 5)                              // Low 5 bits
	bs.WriteBit(true)                                              // Game data packet
	bs.WriteBits(uint64(seq>>5), protocol.SequenceNumberBitSize-5) // High bits

	ackSeq := nc.lastRecvSeq & (protocol.AckSequenceNumberWindowSize - 1)
	bs.WriteBits(uint64(ackSeq), protocol.AckSequenceNumberBitSize)
	bs.WriteBits(0, protocol.PacketHeaderPadBits)

	// Ack Mask Info
	ackByteCount := (nc.lastRecvSeq - nc.lastRecvAckAck + 7) >> 3
	if ackByteCount > protocol.MaxAckByteCount {
		ackByteCount = protocol.MaxAckByteCount
	}
	bs.WriteRangedUint32(ackByteCount, 0, protocol.MaxAckByteCount)
	bs.WriteBits(uint64(nc.ackMask), int(ackByteCount*8))

	// Send Delay (time since last received packet)
	sendDelay := time.Since(nc.lastPacketRecvTime) / (8 * time.Millisecond)
	if sendDelay > 255 {
		sendDelay = 255
	}
	bs.WriteBits(uint64(sendDelay), 8)

	if packetType == protocol.DataPacket {
		idx := nc.lastSendSeq & protocol.PacketWindowMask
		note := GetPacketNotify()
		note.Sequence = nc.lastSendSeq
		note.SendTime = time.Now()
		nc.notifyQueue[idx] = note
		nc.lastSeqRecvdAtSend[idx] = nc.lastRecvSeq
	}
}

func (nc *NetConnection) WindowFull() bool {
	nc.mu.Lock()
	defer nc.mu.Unlock()

	diff := nc.lastSendSeq - nc.highestAckedSeq
	if diff >= (protocol.MaxPacketWindowSize - 2) {
		return true
	}
	return diff >= uint32(nc.cwnd)
}

func (nc *NetConnection) LastSendSeq() uint32 {
	nc.mu.Lock()
	defer nc.mu.Unlock()
	return nc.lastSendSeq
}

func (nc *NetConnection) LastRecvSeq() uint32 {
	nc.mu.Lock()
	defer nc.mu.Unlock()
	return nc.lastRecvSeq
}

func (nc *NetConnection) GetAddress() netio.Address {
	return nc.Addr
}
