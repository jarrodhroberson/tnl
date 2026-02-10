package main

import (
	"bytes"

	"github.com/jarrodhroberson/tnl-go/pkg/bitstream"
	"github.com/jarrodhroberson/tnl-go/pkg/handshake"
	"github.com/jarrodhroberson/tnl-go/pkg/netio"
	"github.com/jarrodhroberson/tnl-go/pkg/protocol"
	"github.com/rs/zerolog/log"
	"github.com/spf13/cobra"
)

var clientCmd = &cobra.Command{
	Use:   "client",
	Short: "Run the TNL demo client",
	Run: func(cmd *cobra.Command, args []string) {
		runClient()
	},
}

func runClient() {
	serverAddr, _ := netio.ParseAddress("127.0.0.1:28000")
	iface, _ := netio.NewInterface(":0") // Bind to random port

	// 1. Handshake
	nc := make([]byte, 16)

	buf := &bytes.Buffer{}
	bs := bitstream.NewWriter(buf)
	bs.WriteUint8(uint8(protocol.ConnectChallengeRequest))
	(&handshake.ChallengeRequest{ClientNonce: nc}).Write(bs)
	bs.Flush()

	log.Info().Msg("Client sending ChallengeRequest")
	iface.SendPacket(&netio.Packet{Addr: serverAddr, Data: buf.Bytes()})

	// Wait for response and solve puzzle (simplified for demo)
	// In a real app, this would be in the interface read loop.
	log.Info().Msg("Client demo waiting for server response... (Ctrl+C to stop)")
	select {}
}
