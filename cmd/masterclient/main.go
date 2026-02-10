package main

import (
	"flag"
	"fmt"
	"log"
	"math/rand"
	"os"
	"os/signal"
	"strconv"
	"syscall"
	"time"
	"tnl-go"
)

func main() {
	// Parse command line arguments
	var (
		masterAddr = flag.String("master", "", "Master server address (required)")
		isServer   = flag.Bool("server", false, "Run as game server instead of client")
		port       = flag.Uint("port", 0, "Local port to bind to (0 for automatic)")
		gameString = flag.String("game", "MasterServerTestGame", "Game identifier string")
		help       = flag.Bool("help", false, "Show help message")
	)
	flag.Parse()

	if *help || *masterAddr == "" {
		fmt.Println("TNL-Go Master Client")
		fmt.Println("Usage: masterclient -master <address> [options]")
		fmt.Println()
		fmt.Println("Options:")
		flag.PrintDefaults()
		fmt.Println()
		fmt.Println("Examples:")
		fmt.Println("  # Run as client connecting to master at localhost:29005")
		fmt.Println("  masterclient -master localhost:29005")
		fmt.Println()
		fmt.Println("  # Run as server on port 28000")
		fmt.Println("  masterclient -master localhost:29005 -server -port 28000")
		fmt.Println()
		fmt.Println("Description:")
		fmt.Println("  In client mode, connects to master server, queries for game servers,")
		fmt.Println("  and attempts to connect to them via arranged connections.")
		fmt.Println()
		fmt.Println("  In server mode, registers with master server and accepts arranged")
		fmt.Println("  connections from clients (75% acceptance rate for testing).")
		return
	}

	// Initialize TNL
	if err := tnl.Init(); err != nil {
		log.Fatalf("Failed to initialize TNL: %v", err)
	}
	defer tnl.Shutdown()

	// Parse master server address
	masterAddress := tnl.NewAddressFromString(*masterAddr)
	if masterAddress == nil {
		log.Fatalf("Invalid master server address: %s", *masterAddr)
	}

	// Initialize random number generator
	rand.Seed(time.Now().UnixNano())

	// Create master client
	client := tnl.NewMasterClient(masterAddress, *isServer, *gameString)

	// Start the client
	if err := client.Start(uint16(*port)); err != nil {
		log.Fatalf("Failed to start master client: %v", err)
	}
	defer client.Stop()

	// If we're a server, start periodic status updates
	if *isServer {
		client.StartPeriodicUpdates()
	}

	// Set up signal handling for graceful shutdown
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

	clientType := "Client"
	if *isServer {
		clientType = "Server"
	}

	fmt.Printf("TNL-Go Master %s started\n", clientType)
	fmt.Printf("Master server: %s\n", masterAddress.ToString())
	fmt.Printf("Game string: %s\n", *gameString)
	if *port != 0 {
		fmt.Printf("Local port: %d\n", *port)
	}
	fmt.Println("Press Ctrl+C to stop...")

	// Wait for shutdown signal
	<-sigChan
	fmt.Printf("\nShutting down master %s...\n", toLower(clientType))
}

// Helper function since strings package isn't imported
func toLower(s string) string {
	result := make([]byte, len(s))
	for i := 0; i < len(s); i++ {
		if s[i] >= 'A' && s[i] <= 'Z' {
			result[i] = s[i] + 32
		} else {
			result[i] = s[i]
		}
	}
	return string(result)
}