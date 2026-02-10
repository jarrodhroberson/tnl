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

	conns := make(map[string]*network.GhostConnection)

	hm.OnConnectionAccepted = func(addr netio.Address, secret []byte, iv []byte) {
		log.Info().Str("addr", addr.String()).Msg("Server creating GhostConnection")
		gc := network.NewGhostConnection(addr, 100)
		conns[addr.String()] = gc

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
