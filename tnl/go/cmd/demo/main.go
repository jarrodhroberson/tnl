package main

import (
	"fmt"
	"os"
	"time"

	"github.com/jarrodhroberson/tnl-go/tnl/bitstream"
	"github.com/jarrodhroberson/tnl-go/tnl/network"
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
	"github.com/spf13/cobra"
)

// DemoObject is a simple replicated object for the demo.
type DemoObject struct {
	network.NetObject
	Name  string
	Value int32
}

func (o *DemoObject) PackUpdate(conn *network.GhostConnection, mask uint32, bs *bitstream.BitStream) uint32 {
	if bs.WriteFlag((mask & 1) != 0) {
		bs.WriteString(o.Name)
	}
	if bs.WriteFlag((mask & 2) != 0) {
		bs.WriteInt32(o.Value)
	}
	return 0
}

func (o *DemoObject) UnpackUpdate(conn *network.GhostConnection, bs *bitstream.BitStream) {
	if bs.ReadFlag() {
		o.Name = bs.ReadString()
	}
	if bs.ReadFlag() {
		o.Value = bs.ReadInt32()
	}
}

func (o *DemoObject) GetUpdatePriority(mask uint32, skips int) float32 { return 1.0 }
func (o *DemoObject) OnGhostAdd(conn *network.GhostConnection) bool {
	fmt.Printf("[Client] Ghost Added: %s = %d\n", o.Name, o.Value)
	return true
}
func (o *DemoObject) OnGhostRemove() {}

func init() {
	network.RegisterObject(1, func() network.Replicatable { return &DemoObject{} })
}

var rootCmd = &cobra.Command{
	Use:   "demo",
	Short: "TNL Demo Application",
}

func main() {
	log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr, TimeFormat: time.RFC3339})

	rootCmd.AddCommand(serverCmd)
	rootCmd.AddCommand(clientCmd)

	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
