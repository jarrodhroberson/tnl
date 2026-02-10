package main

import (
	"bytes"
	"fmt"
	"time"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/crypto"
	"github.com/jarrodhroberson/tnl-go/pkg/handshake"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
	"github.com/jarrodhroberson/tnl-go/pkg/network"
	"github.com/jarrodhroberson/tnl-go/pkg/protocol"
	"github.com/rs/zerolog/log"
	"github.com/spf13/cobra"
)

var clientCount int

func init() {
	clientCmd.Flags().IntVarP(&clientCount, "count", "c", 0, "Number of messages to send (0 for infinity)")
}

var clientCmd = &cobra.Command{
	Use:   "client",
	Short: "Run the TNL demo client",
	Run: func(cmd *cobra.Command, args []string) {
		runClient(clientCount)
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

func runClient(msgCount int) {
	serverAddr, _ := netio.ParseAddress("127.0.0.1:28000")
	iface, _ := netio.NewInterface(":0") // Bind to random port

	h := &clientHandshakeHandler{
		challengeRes: make(chan *handshake.ChallengeResponse, 1),
		connectAcc:   make(chan *handshake.ConnectAccept, 1),
	}
	iface.SetHandshakeHandler(h)
	iface.Start()

	// 1. Handshake: Send Challenge Request
	clientNonce := make([]byte, 16)
	// (Fill with random if needed, but 0 is fine for demo)

	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	bs.WriteUint8(uint8(protocol.ConnectChallengeRequest))
	(&handshake.ChallengeRequest{ClientNonce: clientNonce}).Write(bs)
	bs.Flush()

	log.Info().Msg("Client sending ConnectChallengeRequest")
	iface.SendPacket(&netio.Packet{Addr: serverAddr, Data: buf.Bytes()})

	// 2. Wait for Challenge Response
	var res *handshake.ChallengeResponse
	select {
	case res = <-h.challengeRes:
		log.Info().Msg("Client received ConnectChallengeResponse")
	case <-time.After(5 * time.Second):
		log.Fatal().Msg("Handshake timed out waiting for challenge response")
	}

	// 3. Solve Puzzle & Send Connect Request
	log.Info().Uint32("difficulty", res.Difficulty).Msg("Solving puzzle...")
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

	log.Info().Msg("Client sending ConnectRequest")
	iface.SendPacket(&netio.Packet{Addr: serverAddr, Data: buf.Bytes()})

	// 4. Wait for Accept
	var acc *handshake.ConnectAccept
	select {
	case acc = <-h.connectAcc:
		log.Info().Msg("Client received ConnectAccept. Handshake Complete!")
	case <-time.After(5 * time.Second):
		log.Fatal().Msg("Handshake timed out waiting for connect accept")
	}

	// 5. Establish GhostConnection
	// Sequence roles are reversed from the server's perspective:
	// acc.InitialSendSeq is what the server is sending (client's Recv)
	// acc.InitialRecvSeq is what the server is expecting (client's Send)
	gc := network.NewGhostConnection(serverAddr, acc.InitialRecvSeq)
	gc.SetInitialRecvSequence(acc.InitialSendSeq)
	iface.AddConnection(gc)

	// Implement client-side message logging
	network.OnMessageReceived = func(conn *network.EventConnection, msg string) {
		log.Info().Str("server", conn.Addr.String()).Msgf("Received: %s", msg)
	}

	done := make(chan bool)
	// Start message generation loop in a goroutine
	go func() {
		count := 0
		for {
			count++
			msg := fmt.Sprintf("Hello from client! Message #%d", count)
			log.Debug().Msgf("Posting event: %s", msg)
			gc.PostEvent(network.NewMessageEvent(msg))

			if msgCount > 0 && count >= msgCount {
				log.Info().Msgf("Sent %d messages, exiting...", msgCount)
				// Wait a bit for the last packet to be sent by the pulse loop
				time.Sleep(500 * time.Millisecond)
				done <- true
				return
			}
			time.Sleep(2 * time.Second)
		}
	}()

	if msgCount > 0 {
		log.Info().Msgf("Client running. Sending %d MessageEvents every 2s...", msgCount)
	} else {
		log.Info().Msg("Client running. Sending MessageEvents indefinitely every 2s...")
	}

	// Client pulse loop
	ticker := time.NewTicker(100 * time.Millisecond)
	for {
		select {
		case <-ticker.C:
			buf := &bytes.Buffer{}
			bs := bitstream.NewWriter(buf)

			gc.WritePacketHeader(bs, protocol.DataPacket)
			gc.WriteEvents(bs, gc.LastSendSeq())
			gc.WriteGhosts(bs, gc.LastSendSeq())
			bs.Flush()

			if buf.Len() > 0 {
				iface.SendPacket(&netio.Packet{Addr: gc.Addr, Data: buf.Bytes()})
			}
		case <-done:
			return
		}
	}
}
