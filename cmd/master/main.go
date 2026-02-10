package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"syscall"
	"tnl-go"
)

func main() {
	// Parse command line flags
	var (
		configFile = flag.String("config", "master.cfg", "Configuration file path")
		port       = flag.Uint("port", tnl.DefaultMasterPort, "Port to listen on")
		help       = flag.Bool("help", false, "Show help message")
	)
	flag.Parse()

	if *help {
		fmt.Println("TNL-Go Master Server")
		fmt.Println("Usage: master [options]")
		fmt.Println()
		fmt.Println("Options:")
		flag.PrintDefaults()
		fmt.Println()
		fmt.Println("Configuration file format:")
		fmt.Println("  port <number>                    # Set listening port")
		fmt.Println("  motd <game_type> \"<message>\"     # Set MOTD for game type")
		fmt.Println()
		fmt.Println("Example configuration:")
		fmt.Println("  port 29005")
		fmt.Println("  motd * \"Welcome to TNL-Go Master Server!\"")
		fmt.Println("  motd TestGame \"Welcome to the Test Game server!\"")
		return
	}

	// Initialize TNL
	if err := tnl.Init(); err != nil {
		log.Fatalf("Failed to initialize TNL: %v", err)
	}
	defer tnl.Shutdown()

	// Load configuration
	config, err := tnl.LoadMasterServerConfig(*configFile)
	if err != nil {
		log.Fatalf("Failed to load config: %v", err)
	}

	// Override port if specified on command line
	if *port != tnl.DefaultMasterPort {
		config.Port = tnl.U32(*port)
	}

	// Create and start master server
	server := tnl.NewMasterServer(config)
	if err := server.Start(); err != nil {
		log.Fatalf("Failed to start master server: %v", err)
	}
	defer server.Stop()

	// Set up signal handling for graceful shutdown
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

	fmt.Printf("TNL-Go Master Server running on port %d\n", config.Port)
	fmt.Println("Press Ctrl+C to stop...")

	// Wait for shutdown signal
	<-sigChan
	fmt.Println("\nShutting down master server...")
}