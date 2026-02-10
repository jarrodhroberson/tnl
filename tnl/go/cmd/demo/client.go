package main

import (
	"bytes"
	"fmt"
	"math"
	"math/rand"
	"sync"
	"time"

	"github.com/jarrodhroberson/tnl-go/tnl/bitstream"
	"github.com/jarrodhroberson/tnl-go/tnl/crypto"
	"github.com/jarrodhroberson/tnl-go/tnl/handshake"
	"github.com/jarrodhroberson/tnl-go/tnl/netio"
	"github.com/jarrodhroberson/tnl-go/tnl/network"
	"github.com/jarrodhroberson/tnl-go/tnl/protocol"
	"github.com/rs/zerolog/log"
	"github.com/spf13/cobra"
)

var clientCount int
var clientSleep int
var clientNum int
var clientSigma int

func init() {
	clientCmd.Flags().IntVarP(&clientCount, "count", "c", 0, "Number of messages to send (0 for infinity)")
	clientCmd.Flags().IntVarP(&clientSleep, "sleep", "s", 2000, "Sleep time between messages in milliseconds")
	clientCmd.Flags().IntVarP(&clientNum, "clients", "n", 1, "Number of simultaneous clients to run")
	clientCmd.Flags().IntVarP(&clientSigma, "sigma", "z", 0, "Standard deviation for sleep time in milliseconds")
}

var clientCmd = &cobra.Command{
	Use:   "client",
	Short: "Run the TNL demo client",
	Run: func(cmd *cobra.Command, args []string) {
		runClient(clientCount, clientSleep, clientNum, clientSigma)
	},
}

type clientHandshakeHandler struct {
	challengeRes chan *handshake.ChallengeResponse
	connectAcc   chan *handshake.ConnectAccept
}

func (h *clientHandshakeHandler) HandlePacket(s netio.PacketSender, p *netio.Packet) {
	bs := bitstream.NewReader(bytes.NewReader(p.Data))
	pktType := protocol.PacketType(bs.ReadUint8())
	switch pktType {
	case protocol.ConnectChallengeResponse:
		res := &handshake.ChallengeResponse{}
		res.Read(bs)
		h.challengeRes <- res
	case protocol.ConnectAccept:
		acc := &handshake.ConnectAccept{}
		acc.Read(bs)
		h.connectAcc <- acc
	}
}

type demoClient struct {
	id       int
	msgCount int
	sleepMs  int
	sigmaMs  int
	sendChan chan string
	recvChan chan string
	done     chan bool
}

func runClient(msgCount int, sleepMs int, numClients int, sigmaMs int) {
	var wg sync.WaitGroup
	clients := make([]*demoClient, numClients)

	// Global hook to route messages to per-client channels
	network.OnMessageReceived = func(conn *network.EventConnection, msg string) {
		if c, ok := conn.Tag.(*demoClient); ok {
			c.recvChan <- msg
		}
	}

	for i := 0; i < numClients; i++ {
		wg.Add(1)
		clients[i] = &demoClient{
			id:       i + 1,
			msgCount: msgCount,
			sleepMs:  sleepMs,
			sigmaMs:  sigmaMs,
			sendChan: make(chan string, 10),
			recvChan: make(chan string, 10),
			done:     make(chan bool),
		}
		go clients[i].start(&wg)
	}

	wg.Wait()
	log.Info().Msg("All clients finished execution.")
}

func (c *demoClient) start(wg *sync.WaitGroup) {
	defer wg.Done()
	prefix := fmt.Sprintf("[Client #%d]", c.id)

	serverAddr, _ := netio.ParseAddress("127.0.0.1:28000")
	iface, _ := netio.NewInterface(":0") // Bind to random port

	h := &clientHandshakeHandler{
		challengeRes: make(chan *handshake.ChallengeResponse, 1),
		connectAcc:   make(chan *handshake.ConnectAccept, 1),
	}
	iface.SetHandshakeHandler(h)
	iface.Start()
	defer iface.Stop()

	// 1. Handshake: Send Challenge Request
	clientNonce := make([]byte, 16)
	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	bs.WriteUint8(uint8(protocol.ConnectChallengeRequest))
	(&handshake.ChallengeRequest{ClientNonce: clientNonce}).Write(bs)
	bs.Flush()

	log.Info().Msgf("%s sending ConnectChallengeRequest", prefix)
	iface.SendPacket(&netio.Packet{Addr: serverAddr, Data: buf.Bytes()})

	// 2. Wait for Challenge Response
	var res *handshake.ChallengeResponse
	select {
	case res = <-h.challengeRes:
		log.Info().Msgf("%s received ConnectChallengeResponse", prefix)
	case <-time.After(5 * time.Second):
		log.Error().Msgf("%s handshake timed out waiting for challenge response", prefix)
		return
	}

	// 3. Solve Puzzle & Send Connect Request
	log.Info().Uint32("difficulty", res.Difficulty).Msgf("%s solving puzzle...", prefix)
	puzzle := crypto.NewPuzzle()
	solution := puzzle.SolvePuzzle(res.ClientIdentity, clientNonce, res.ServerNonce, res.Difficulty)

	kx, _ := crypto.NewKeyExchange()
	req := &handshake.ConnectRequest{
		ClientNonce:     clientNonce,
		ServerNonce:     res.ServerNonce,
		PuzzleSolution:  solution,
		ClientPublicKey: kx.PublicKey(),
	}

	buf = &bytes.Buffer{}
	bs = bitstream.NewWriter(buf)
	bs.WriteUint8(uint8(protocol.ConnectRequest))
	req.Write(bs)
	bs.Flush()

	log.Info().Msgf("%s sending ConnectRequest", prefix)
	iface.SendPacket(&netio.Packet{Addr: serverAddr, Data: buf.Bytes()})

	// 4. Wait for Accept
	var acc *handshake.ConnectAccept
	select {
	case acc = <-h.connectAcc:
		log.Info().Msgf("%s received ConnectAccept. Handshake Complete!", prefix)
	case <-time.After(5 * time.Second):
		log.Error().Msgf("%s handshake timed out waiting for connect accept", prefix)
		return
	}

	// 5. Establish GhostConnection
	gc := network.NewGhostConnection(serverAddr, acc.InitialRecvSeq)
	gc.SetInitialRecvSequence(acc.InitialSendSeq)
	gc.Tag = c // Associate this client struct with the connection
	iface.AddConnection(gc)

	// Message generation loop
	go func() {
		count := 0
		for {
			count++
			msg := fmt.Sprintf("Hello from client #%d! Message #%d", c.id, count)
			c.sendChan <- msg

			if c.msgCount > 0 && count >= c.msgCount {
				// Wait a bit for the last packet to be sent by the pulse loop
				time.Sleep(500 * time.Millisecond)
				c.done <- true
				return
			}

			// Calculate sleep duration with sigma
			durationMs := float64(c.sleepMs)
			if c.sigmaMs > 0 {
				durationMs += rand.NormFloat64() * float64(c.sigmaMs)
			}
			// Ensure non-negative duration
			duration := time.Duration(math.Max(0, durationMs)) * time.Millisecond
			time.Sleep(duration)
		}
	}()

	// Message receiver loop
	go func() {
		for msg := range c.recvChan {
			log.Info().Str("server", serverAddr.String()).Msgf("%s Received: %s", prefix, msg)
		}
	}()

	if c.msgCount > 0 {
		log.Info().Msgf("%s running. Sending %d messages every %dms...", prefix, c.msgCount, c.sleepMs)
	} else {
		log.Info().Msgf("%s running. Sending messages indefinitely every %dms...", prefix, c.sleepMs)
	}

	// Client pulse loop
	ticker := time.NewTicker(100 * time.Millisecond)
	defer ticker.Stop()
	for {
		select {
		case <-ticker.C:
			// Drain send channel and post to GhostConnection
			select {
			case msg := <-c.sendChan:
				log.Debug().Msgf("%s posting event: %s", prefix, msg)
				gc.PostEvent(network.NewMessageEvent(msg))
			default:
			}

			buf := &bytes.Buffer{}
			bs := bitstream.NewWriter(buf)

			gc.WritePacketHeader(bs, protocol.DataPacket)
			gc.WriteEvents(bs, gc.LastSendSeq())
			gc.WriteGhosts(bs, gc.LastSendSeq())
			bs.Flush()

			if buf.Len() > 0 {
				iface.SendPacket(&netio.Packet{Addr: gc.Addr, Data: buf.Bytes()})
			}
		case <-c.done:
			return
		}
	}
}
