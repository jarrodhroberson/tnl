package main

import (
	"bytes"
	"time"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
	"github.com/jarrodhroberson/tnl-go/pkg/network"
	"github.com/jarrodhroberson/tnl-go/pkg/protocol"
	"github.com/rs/zerolog/log"
	"github.com/spf13/cobra"
)

type logEntry struct {
	direction string
	addr      string
	message   string
}

var serverCmd = &cobra.Command{
	Use:   "server",
	Short: "Run the TNL demo server",
	Run: func(cmd *cobra.Command, args []string) {
		runServer()
	},
}

func runServer() {
	iface, err := netio.NewInterface("127.0.0.1:28000")
	if err != nil {
		log.Fatal().Err(err).Msg("failed to bind server")
	}

	hm, _ := network.NewHandshakeManager()
	iface.SetHandshakeHandler(hm)
	iface.Start()

	logChan := make(chan logEntry, 100)
	go func() {
		for entry := range logChan {
			log.Info().Str("dir", entry.direction).Str("addr", entry.addr).Msg(entry.message)
		}
	}()

	// Implement async message reversal
	network.OnMessageReceived = func(conn *network.EventConnection, msg string) {
		logChan <- logEntry{direction: "RECV", addr: conn.Addr.String(), message: msg}

		go func() {
			// Reverse the string
			runes := []rune(msg)
			for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
				runes[i], runes[j] = runes[j], runes[i]
			}
			reversed := string(runes)

			// Frame with square brackets
			response := "[" + reversed + "]"

			logChan <- logEntry{direction: "SEND", addr: conn.Addr.String(), message: response}
			conn.PostEvent(network.NewMessageEvent(response))
		}()
	}

	conns := make(map[string]*network.GhostConnection)

	hm.OnConnectionAccepted = func(addr netio.Address, secret []byte, iv []byte, sendSeq uint32, recvSeq uint32) {
		logChan <- logEntry{direction: "SYS", addr: addr.String(), message: "Handshake accepted, creating connection"}
		gc := network.NewGhostConnection(addr, sendSeq)
		gc.SetInitialRecvSequence(recvSeq)
		conns[addr.String()] = gc
		iface.AddConnection(gc)

		// Add some initial objects
		obj := &DemoObject{Name: "ServerObject", Value: 42}
		obj.NetID = 100
		gc.ObjectInScope(obj)
	}

	log.Info().Msg("Server running on 127.0.0.1:28000")

	// Server simulation loop
	ticker := time.NewTicker(100 * time.Millisecond)
	for range ticker.C {
		// Process received packets (simplified for demo)
		// ...

		// Replication pulse
		for _, gc := range conns {
			buf := &bytes.Buffer{}
			bs := bitstream.NewWriter(buf)
			gc.WritePacketHeader(bs, protocol.DataPacket)
			gc.WriteEvents(bs, gc.LastSendSeq())
			gc.WriteGhosts(bs, gc.LastSendSeq())
			bs.Flush()

			iface.SendPacket(&netio.Packet{Addr: gc.Addr, Data: buf.Bytes()})
		}
	}
}
