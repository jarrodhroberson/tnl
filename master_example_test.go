package tnl

import (
	"fmt"
	"math/rand"
	"testing"
	"time"
)

// TestMasterServerBasic tests basic master server functionality
func TestMasterServerBasic(t *testing.T) {
	// Initialize TNL
	if err := Init(); err != nil {
		t.Fatalf("Failed to initialize TNL: %v", err)
	}
	defer Shutdown()

	// Create configuration
	config := NewMasterServerConfig()
	config.Port = 29006 // Use different port for testing
	config.AddMOTD("TestGame", "Welcome to the test!")

	// Create and start master server
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		t.Fatalf("Failed to start master server: %v", err)
	}
	defer server.Stop()

	// Give server time to start
	time.Sleep(100 * time.Millisecond)

	t.Logf("Master server started successfully on port %d", config.Port)
}

// TestMasterClientConnection tests client connection to master server
func TestMasterClientConnection(t *testing.T) {
	// Initialize TNL
	if err := Init(); err != nil {
		t.Fatalf("Failed to initialize TNL: %v", err)
	}
	defer Shutdown()

	// Start master server
	config := NewMasterServerConfig()
	config.Port = 29007
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		t.Fatalf("Failed to start master server: %v", err)
	}
	defer server.Stop()

	time.Sleep(100 * time.Millisecond)

	// Create client
	masterAddr := NewAddress("127.0.0.1", uint16(config.Port))
	client := NewMasterClient(masterAddr, false, "TestGame")

	if err := client.Start(0); err != nil {
		t.Fatalf("Failed to start master client: %v", err)
	}
	defer client.Stop()

	// Give client time to connect
	time.Sleep(500 * time.Millisecond)

	t.Log("Master client connected successfully")
}

// TestMasterServerConfig tests configuration loading
func TestMasterServerConfig(t *testing.T) {
	config := NewMasterServerConfig()
	
	// Test default values
	if config.Port != DefaultMasterPort {
		t.Errorf("Expected default port %d, got %d", DefaultMasterPort, config.Port)
	}

	// Test MOTD functionality
	config.AddMOTD("TestGame", "Test message")
	config.AddMOTD("*", "Default message")

	motd := config.GetMOTD("TestGame")
	if motd != "Test message" {
		t.Errorf("Expected 'Test message', got '%s'", motd)
	}

	motd = config.GetMOTD("UnknownGame")
	if motd != "Default message" {
		t.Errorf("Expected 'Default message', got '%s'", motd)
	}
}

// TestServerInfo tests server information management
func TestServerInfo(t *testing.T) {
	addr := NewAddress("192.168.1.100", 28000)
	server := NewServerInfo(addr)

	// Test initial state
	if server.Address != addr {
		t.Error("Server address not set correctly")
	}

	// Test status update
	server.UpdateStatus("TestGame", "TestMission", 5, 10, 32, 0)
	
	if server.CurrentGameType.Name != "TestGame" {
		t.Errorf("Expected game type 'TestGame', got '%s'", server.CurrentGameType.Name)
	}

	if server.PlayerCount != 10 {
		t.Errorf("Expected player count 10, got %d", server.PlayerCount)
	}

	// Test filter matching
	matches := server.MatchesFilter("TestGame", 0xFFFFFFFF, 0, 32, 0, 10, 0, "TestGame", "TestMission")
	if !matches {
		t.Error("Server should match the filter")
	}

	matches = server.MatchesFilter("OtherGame", 0xFFFFFFFF, 0, 32, 0, 10, 0, "", "")
	if matches {
		t.Error("Server should not match different game string")
	}
}

// TestGameConnectRequest tests connection request management
func TestGameConnectRequest(t *testing.T) {
	req := NewGameConnectRequest()
	
	if req.RequestTime == 0 {
		t.Error("Request time should be set")
	}

	// Test that request time is recent
	currentTime := GetRealMilliseconds()
	if currentTime-req.RequestTime > 1000 { // Within 1 second
		t.Error("Request time should be recent")
	}
}

// ExampleMasterServer demonstrates basic master server usage
func ExampleMasterServer() {
	// Initialize TNL
	if err := Init(); err != nil {
		fmt.Printf("Failed to initialize TNL: %v\n", err)
		return
	}
	defer Shutdown()

	// Create configuration
	config := NewMasterServerConfig()
	config.Port = 29008
	config.AddMOTD("*", "Welcome to the example server!")

	// Create and start master server
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		fmt.Printf("Failed to start server: %v\n", err)
		return
	}
	defer server.Stop()

	fmt.Printf("Master server running on port %d\n", config.Port)

	// In a real application, you would keep the server running
	// Here we'll just demonstrate that it started successfully
	time.Sleep(100 * time.Millisecond)

	// Output:
	// Master server running on port 29008
}

// ExampleMasterClient demonstrates basic master client usage
func ExampleMasterClient() {
	// Initialize TNL
	if err := Init(); err != nil {
		fmt.Printf("Failed to initialize TNL: %v\n", err)
		return
	}
	defer Shutdown()

	// Start a master server for the example
	config := NewMasterServerConfig()
	config.Port = 29009
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		fmt.Printf("Failed to start server: %v\n", err)
		return
	}
	defer server.Stop()

	time.Sleep(100 * time.Millisecond)

	// Create and start client
	masterAddr := NewAddress("127.0.0.1", uint16(config.Port))
	client := NewMasterClient(masterAddr, false, "ExampleGame")

	if err := client.Start(0); err != nil {
		fmt.Printf("Failed to start client: %v\n", err)
		return
	}
	defer client.Stop()

	fmt.Printf("Master client connected to %s\n", masterAddr.ToString())

	// Output:
	// Master client connected to 127.0.0.1:29009
}

// BenchmarkMasterServerConnections benchmarks master server connection handling
func BenchmarkMasterServerConnections(b *testing.B) {
	// Initialize TNL
	if err := Init(); err != nil {
		b.Fatalf("Failed to initialize TNL: %v", err)
	}
	defer Shutdown()

	// Start master server
	config := NewMasterServerConfig()
	config.Port = 29010
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		b.Fatalf("Failed to start master server: %v", err)
	}
	defer server.Stop()

	time.Sleep(100 * time.Millisecond)

	b.ResetTimer()

	// Benchmark creating multiple client connections
	for i := 0; i < b.N; i++ {
		masterAddr := NewAddress("127.0.0.1", uint16(config.Port))
		client := NewMasterClient(masterAddr, false, fmt.Sprintf("TestGame%d", i))
		
		if err := client.Start(0); err != nil {
			b.Fatalf("Failed to start client: %v", err)
		}
		
		// Small delay to allow connection
		time.Sleep(1 * time.Millisecond)
		
		client.Stop()
	}
}

// BenchmarkServerInfoFiltering benchmarks server filtering performance
func BenchmarkServerInfoFiltering(b *testing.B) {
	// Create a bunch of test servers
	servers := make([]*ServerInfo, 1000)
	for i := 0; i < 1000; i++ {
		addr := NewAddress("192.168.1.1", uint16(28000+i))
		server := NewServerInfo(addr)
		server.GameString = fmt.Sprintf("Game%d", i%10)
		server.RegionCode = U32(i % 4)
		server.PlayerCount = U32(rand.Intn(32))
		server.MaxPlayers = 32
		server.NumBots = U32(rand.Intn(10))
		server.CPUSpeed = U32(1000 + rand.Intn(2000))
		server.InfoFlags = U32(rand.Intn(16))
		server.CurrentGameType = NewMissionGameType(fmt.Sprintf("GameType%d", i%5))
		server.CurrentMissionType = NewMissionGameType(fmt.Sprintf("Mission%d", i%3))
		servers[i] = server
	}

	b.ResetTimer()

	// Benchmark filtering
	for i := 0; i < b.N; i++ {
		matchCount := 0
		for _, server := range servers {
			if server.MatchesFilter("Game1", 0xFFFFFFFF, 0, 32, 0, 10, 500, "", "") {
				matchCount++
			}
		}
		_ = matchCount
	}
}

// TestMasterServerIntegration tests a complete master server scenario
func TestMasterServerIntegration(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping integration test in short mode")
	}

	// Initialize TNL
	if err := Init(); err != nil {
		t.Fatalf("Failed to initialize TNL: %v", err)
	}
	defer Shutdown()

	// Start master server
	config := NewMasterServerConfig()
	config.Port = 29011
	config.AddMOTD("IntegrationTest", "Welcome to integration test!")
	
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		t.Fatalf("Failed to start master server: %v", err)
	}
	defer server.Stop()

	time.Sleep(100 * time.Millisecond)

	// Start a game server
	masterAddr := NewAddress("127.0.0.1", uint16(config.Port))
	gameServer := NewMasterClient(masterAddr, true, "IntegrationTest")
	
	if err := gameServer.Start(28001); err != nil {
		t.Fatalf("Failed to start game server: %v", err)
	}
	defer gameServer.Stop()

	gameServer.StartPeriodicUpdates()
	time.Sleep(200 * time.Millisecond)

	// Start a client
	client := NewMasterClient(masterAddr, false, "IntegrationTest")
	
	if err := client.Start(0); err != nil {
		t.Fatalf("Failed to start client: %v", err)
	}
	defer client.Stop()

	// Give time for client to query and potentially connect
	time.Sleep(2 * time.Second)

	t.Log("Integration test completed successfully")
}