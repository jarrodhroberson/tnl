package netio

import (
	"context"
	"net"
	"sync"

	"github.com/rs/zerolog/log"
)

// Packet represents a received or to-be-sent UDP packet.
type Packet struct {
	Addr Address
	Data []byte
}

// Interface manages a UDP socket and handles packet routing.
type Interface struct {
	conn      *net.UDPConn
	readChan  chan *Packet
	writeChan chan *Packet

	mu          sync.RWMutex
	connections map[string]Connection
	handshaker  HandshakeHandler

	ctx    context.Context
	cancel context.CancelFunc
}

// PacketSender is an interface for types that can send UDP packets.
type PacketSender interface {
	SendPacket(p *Packet)
}

// HandshakeHandler processes packets from unknown addresses.
type HandshakeHandler interface {
	HandlePacket(s PacketSender, p *Packet)
}

// Connection is an interface for types that can handle received packets.
type Connection interface {
	ProcessPacket(data []byte)
	GetAddress() Address
}

func NewInterface(bindAddr string) (*Interface, error) {
	addr, err := net.ResolveUDPAddr("udp", bindAddr)
	if err != nil {
		return nil, err
	}

	conn, err := net.ListenUDP("udp", addr)
	if err != nil {
		return nil, err
	}

	ctx, cancel := context.WithCancel(context.Background())
	return &Interface{
		conn:        conn,
		readChan:    make(chan *Packet, 100),
		writeChan:   make(chan *Packet, 100),
		connections: make(map[string]Connection),
		ctx:         ctx,
		cancel:      cancel,
	}, nil
}

func (i *Interface) Start() {
	go i.readLoop()
	go i.writeLoop()
}

func (i *Interface) Stop() {
	i.cancel()
	i.conn.Close()
}

func (i *Interface) readLoop() {
	for {
		select {
		case <-i.ctx.Done():
			return
		default:
			buf := GetPacketBuffer()
			n, addr, err := i.conn.ReadFromUDP(buf)
			if err != nil {
				// Check if the error is due to the connection being closed
				select {
				case <-i.ctx.Done():
					// Context cancelled, connection closed intentionally
					return
				default:
				}

				if opErr, ok := err.(*net.OpError); ok && opErr.Err.Error() == "use of closed network connection" {
					return
				}

				log.Error().Err(err).Msg("read error")
				PutPacketBuffer(buf)
				continue
			}

			data := make([]byte, n)
			copy(data, buf[:n])
			PutPacketBuffer(buf)

			p := &Packet{
				Addr: NewAddress(addr),
				Data: data,
			}

			i.dispatchPacket(p)
		}
	}
}

func (i *Interface) dispatchPacket(p *Packet) {
	i.mu.RLock()
	conn, ok := i.connections[p.Addr.String()]
	i.mu.RUnlock()

	if ok {
		conn.ProcessPacket(p.Data)
	} else {
		// Potential new connection or info packet
		i.handleUnknownPacket(p)
	}
}

func (i *Interface) handleUnknownPacket(p *Packet) {
	i.mu.RLock()
	h := i.handshaker
	i.mu.RUnlock()

	if h != nil {
		h.HandlePacket(i, p)
	} else {
		log.Debug().Str("addr", p.Addr.String()).Msg("received packet from unknown address (no handshaker)")
	}
}

func (i *Interface) SetHandshakeHandler(h HandshakeHandler) {
	i.mu.Lock()
	defer i.mu.Unlock()
	i.handshaker = h
}

func (i *Interface) writeLoop() {
	for {
		select {
		case <-i.ctx.Done():
			return
		case p := <-i.writeChan:
			_, err := i.conn.WriteToUDP(p.Data, p.Addr.UDPAddr)
			if err != nil {
				log.Error().Err(err).Str("to", p.Addr.String()).Msg("write error")
			}
		}
	}
}

func (i *Interface) SendPacket(p *Packet) {
	i.writeChan <- p
}

func (i *Interface) AddConnection(conn Connection) {
	i.mu.Lock()
	defer i.mu.Unlock()
	i.connections[conn.GetAddress().String()] = conn
}
