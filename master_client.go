package tnl

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

// MasterClient represents a client that connects to the master server
type MasterClient struct {
	masterAddress    *Address
	socket           Socket
	connection       *MasterClientConnection
	isServer         bool
	gameString       string
	currentQueryID   U32
	serverList       []*Address
	gameTypes        []string
	missionTypes     []string
	running          bool
	stopChan         chan bool
	mutex            sync.RWMutex
	
	// Server-specific fields
	cpuSpeed         U32
	regionCode       U32
	playerCount      U32
	maxPlayers       U32
	numBots          U32
	infoFlags        U32
	currentGameType  string
	currentMissionType string
}

// NewMasterClient creates a new master client
func NewMasterClient(masterAddress *Address, isServer bool, gameString string) *MasterClient {
	return &MasterClient{
		masterAddress:      masterAddress,
		isServer:          isServer,
		gameString:        gameString,
		serverList:        make([]*Address, 0),
		gameTypes:         make([]string, 0),
		missionTypes:      make([]string, 0),
		stopChan:          make(chan bool),
		currentGameType:   "SomeGameType",
		currentMissionType: "SomeMissionType",
		cpuSpeed:          1000,
		regionCode:        0xFFFFFFFF,
		maxPlayers:        32,
		playerCount:       10,
		numBots:           5,
		infoFlags:         0,
	}
}

// Start starts the master client
func (mc *MasterClient) Start(port uint16) error {
	mc.mutex.Lock()
	defer mc.mutex.Unlock()

	if mc.running {
		return fmt.Errorf("master client is already running")
	}

	// Create socket
	mc.socket = NewUDPSocket()
	bindAddr := NewAddress("0.0.0.0", port)
	if err := mc.socket.Bind(bindAddr); err != nil {
		return fmt.Errorf("failed to bind to port %d: %v", port, err)
	}

	mc.running = true
	LogInfo("%s started - master is at %s", 
		map[bool]string{true: "Server", false: "Client"}[mc.isServer], 
		mc.masterAddress.ToString())

	// Start processing loop
	go mc.processLoop()

	// Connect to master server
	go mc.connectToMaster()

	return nil
}

// Stop stops the master client
func (mc *MasterClient) Stop() {
	mc.mutex.Lock()
	defer mc.mutex.Unlock()

	if !mc.running {
		return
	}

	mc.running = false
	close(mc.stopChan)

	if mc.socket != nil {
		mc.socket.Close()
	}

	LogInfo("Master client stopped")
}

// processLoop is the main processing loop
func (mc *MasterClient) processLoop() {
	buffer := make([]byte, MaxPacketDataSize)

	for mc.running {
		// Check for incoming packets
		n, addr, err := mc.socket.RecvFrom(buffer)
		if err != nil {
			if mc.running {
				LogError("Error receiving packet: %v", err)
			}
			continue
		}

		// Process the packet
		go mc.processPacket(buffer[:n], addr)

		// Small delay
		time.Sleep(1 * time.Millisecond)
	}
}

// connectToMaster connects to the master server
func (mc *MasterClient) connectToMaster() {
	for mc.running {
		mc.mutex.RLock()
		needsConnection := mc.connection == nil || 
			mc.connection.GetConnectionState() == Disconnected || 
			mc.connection.GetConnectionState() == TimedOut
		mc.mutex.RUnlock()

		if needsConnection {
			LogInfo("Connecting to master server at %s", mc.masterAddress.ToString())
			
			conn := NewMasterClientConnection(mc.masterAddress, mc)
			mc.mutex.Lock()
			mc.connection = conn
			mc.mutex.Unlock()
			
			if err := conn.Connect(); err != nil {
				LogError("Failed to connect to master: %v", err)
				time.Sleep(5 * time.Second) // Wait before retry
				continue
			}
		}

		time.Sleep(1 * time.Second)
	}
}

// processPacket processes an incoming packet
func (mc *MasterClient) processPacket(data []byte, addr *Address) {
	// Only process packets from the master server
	if !addr.Equals(mc.masterAddress) {
		return
	}

	bs := NewBitStream(data)
	packetType := bs.ReadInt(8)

	switch packetType {
	case 1: // Connection response
		mc.handleConnectionResponse(bs)
	case 2: // RPC response
		mc.handleRPCResponse(bs)
	default:
		LogWarning("Unknown packet type %d from master", packetType)
	}
}

// handleConnectionResponse handles a connection response from the master
func (mc *MasterClient) handleConnectionResponse(bs *BitStream) {
	success := bs.ReadFlag()
	
	mc.mutex.Lock()
	defer mc.mutex.Unlock()
	
	if success {
		if mc.connection != nil {
			mc.connection.SetConnectionState(Connected)
			LogInfo("Connected to master server")
			
			// If we're a client, start querying for game types
			if !mc.isServer {
				go mc.startGameTypesQuery()
			}
		}
	} else {
		LogError("Master server rejected connection")
		if mc.connection != nil {
			mc.connection.SetConnectionState(ConnectRejected)
		}
	}
}

// handleRPCResponse handles RPC responses from the master
func (mc *MasterClient) handleRPCResponse(bs *BitStream) {
	rpcID := bs.ReadInt(16)

	switch rpcID {
	case 1: // QueryGameTypesResponse
		mc.handleGameTypesResponse(bs)
	case 2: // QueryServersResponse
		mc.handleServersResponse(bs)
	case 3: // ClientRequestedArrangedConnection
		mc.handleClientRequestedArrangedConnection(bs)
	case 4: // ArrangedConnectionAccepted
		mc.handleArrangedConnectionAccepted(bs)
	case 5: // ArrangedConnectionRejected
		mc.handleArrangedConnectionRejected(bs)
	case 6: // SetMOTD
		mc.handleSetMOTD(bs)
	default:
		LogWarning("Unknown RPC ID %d from master", rpcID)
	}
}

// startGameTypesQuery starts querying for game types
func (mc *MasterClient) startGameTypesQuery() {
	mc.mutex.Lock()
	mc.currentQueryID++
	queryID := mc.currentQueryID
	mc.mutex.Unlock()

	if mc.connection != nil {
		mc.connection.SendQueryGameTypes(queryID)
	}
}

// handleGameTypesResponse handles game types response
func (mc *MasterClient) handleGameTypesResponse(bs *BitStream) {
	queryID := bs.ReadInt(32)
	
	mc.mutex.RLock()
	if queryID != mc.currentQueryID {
		mc.mutex.RUnlock()
		return // Ignore old queries
	}
	mc.mutex.RUnlock()

	// Read game types
	gameTypeCount := bs.ReadInt(16)
	gameTypes := make([]string, gameTypeCount)
	for i := U32(0); i < gameTypeCount; i++ {
		gameTypes[i] = ""
		bs.ReadString(gameTypes[i])
	}

	// Read mission types
	missionTypeCount := bs.ReadInt(16)
	missionTypes := make([]string, missionTypeCount)
	for i := U32(0); i < missionTypeCount; i++ {
		missionTypes[i] = ""
		bs.ReadString(missionTypes[i])
	}

	mc.mutex.Lock()
	mc.gameTypes = append(mc.gameTypes, gameTypes...)
	mc.missionTypes = append(mc.missionTypes, missionTypes...)
	mc.mutex.Unlock()

	LogInfo("Got game types response - %d game types, %d mission types", len(gameTypes), len(missionTypes))

	// If this is the final response (empty lists), query for servers
	if len(gameTypes) == 0 && len(missionTypes) == 0 {
		mc.queryServers()
	}
}

// queryServers queries for available servers
func (mc *MasterClient) queryServers() {
	mc.mutex.Lock()
	mc.currentQueryID++
	queryID := mc.currentQueryID
	mc.serverList = mc.serverList[:0] // Clear server list
	mc.mutex.Unlock()

	if mc.connection != nil {
		// Query for all servers
		mc.connection.SendQueryServers(queryID, 0xFFFFFFFF, 0, 128, 0, 128, 0, "", "")
	}
}

// handleServersResponse handles server list response
func (mc *MasterClient) handleServersResponse(bs *BitStream) {
	queryID := bs.ReadInt(32)
	
	mc.mutex.RLock()
	if queryID != mc.currentQueryID {
		mc.mutex.RUnlock()
		return
	}
	mc.mutex.RUnlock()

	// Read server addresses
	serverCount := bs.ReadInt(16)
	servers := make([]*Address, serverCount)
	
	for i := U32(0); i < serverCount; i++ {
		ip := make([]byte, 4)
		bs.ReadBits(32, ip)
		port := bs.ReadInt(16)
		servers[i] = NewAddress(fmt.Sprintf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]), uint16(port))
	}

	mc.mutex.Lock()
	mc.serverList = append(mc.serverList, servers...)
	mc.mutex.Unlock()

	LogInfo("Got server list response - %d servers", len(servers))
	for _, server := range servers {
		LogInfo("  %s", server.ToString())
	}

	// If this is the final response (empty list), try to connect to a server
	if len(servers) == 0 && !mc.isServer {
		mc.tryConnectToServer()
	}
}

// tryConnectToServer attempts to connect to a random server
func (mc *MasterClient) tryConnectToServer() {
	mc.mutex.RLock()
	serverCount := len(mc.serverList)
	mc.mutex.RUnlock()

	if serverCount == 0 {
		LogInfo("No game servers available")
		return
	}

	// Pick a random server
	mc.mutex.RLock()
	index := rand.Intn(len(mc.serverList))
	targetServer := mc.serverList[index]
	mc.mutex.RUnlock()

	mc.mutex.Lock()
	mc.currentQueryID++
	requestID := mc.currentQueryID
	mc.mutex.Unlock()

	LogInfo("Requesting arranged connection with %s", targetServer.ToString())

	// Create connection parameters
	connectionParams := NewByteBufferFromData([]byte("Hello World!"))

	// Get our internal address
	internalAddr := mc.socket.GetLocalAddress()

	if mc.connection != nil {
		mc.connection.SendRequestArrangedConnection(requestID, targetServer, internalAddr, connectionParams)
	}
}

// handleClientRequestedArrangedConnection handles a client connection request (server side)
func (mc *MasterClient) handleClientRequestedArrangedConnection(bs *BitStream) {
	if !mc.isServer {
		return
	}

	requestID := bs.ReadInt(32)

	// Read possible addresses
	addressCount := bs.ReadInt(16)
	possibleAddresses := make([]*Address, addressCount)
	for i := U32(0); i < addressCount; i++ {
		ip := make([]byte, 4)
		bs.ReadBits(32, ip)
		port := bs.ReadInt(16)
		possibleAddresses[i] = NewAddress(fmt.Sprintf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]), uint16(port))
	}

	// Read connection parameters
	paramSize := bs.ReadInt(16)
	paramData := make([]byte, paramSize)
	if paramSize > 0 {
		bs.ReadBits(U32(paramSize)*8, paramData)
	}
	connectionParams := NewByteBufferFromData(paramData)

	LogInfo("Client requested arranged connection from %s", possibleAddresses[0].ToString())

	// Accept connection 75% of the time
	if rand.Float32() > 0.75 {
		LogInfo("Rejecting arranged connection")
		if mc.connection != nil {
			mc.connection.SendRejectArrangedConnection(requestID, connectionParams)
		}
	} else {
		LogInfo("Accepting arranged connection")
		
		// Generate shared secret data
		data := make([]byte, 64) // Nonce + ServerNonce + SharedSecret
		rand.Read(data)
		
		connectionData := NewByteBufferFromData(data)
		internalAddr := mc.socket.GetLocalAddress()
		
		if mc.connection != nil {
			mc.connection.SendAcceptArrangedConnection(requestID, internalAddr, connectionData)
		}
		
		// In a real implementation, we would create a GameConnection here
		LogInfo("Generated shared secret data for arranged connection")
	}
}

// handleArrangedConnectionAccepted handles acceptance of an arranged connection (client side)
func (mc *MasterClient) handleArrangedConnectionAccepted(bs *BitStream) {
	if mc.isServer {
		return
	}

	requestID := bs.ReadInt(32)
	
	mc.mutex.RLock()
	if requestID != mc.currentQueryID {
		mc.mutex.RUnlock()
		return
	}
	mc.mutex.RUnlock()

	// Read possible addresses
	addressCount := bs.ReadInt(16)
	possibleAddresses := make([]*Address, addressCount)
	for i := U32(0); i < addressCount; i++ {
		ip := make([]byte, 4)
		bs.ReadBits(32, ip)
		port := bs.ReadInt(16)
		possibleAddresses[i] = NewAddress(fmt.Sprintf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]), uint16(port))
	}

	// Read connection data
	dataSize := bs.ReadInt(16)
	connectionData := make([]byte, dataSize)
	if dataSize > 0 {
		bs.ReadBits(U32(dataSize)*8, connectionData)
	}

	LogInfo("Remote host accepted arranged connection")
	LogInfo("Shared secret data received (%d bytes)", len(connectionData))

	// In a real implementation, we would create a GameConnection here
	// For now, we'll just wait a bit and then query for servers again
	go func() {
		time.Sleep(15 * time.Second)
		LogInfo("Simulated game connection ended, querying for new servers")
		mc.startGameTypesQuery()
	}()
}

// handleArrangedConnectionRejected handles rejection of an arranged connection (client side)
func (mc *MasterClient) handleArrangedConnectionRejected(bs *BitStream) {
	if mc.isServer {
		return
	}

	requestID := bs.ReadInt(32)
	
	mc.mutex.RLock()
	if requestID != mc.currentQueryID {
		mc.mutex.RUnlock()
		return
	}
	mc.mutex.RUnlock()

	// Read reject data
	dataSize := bs.ReadInt(16)
	rejectData := make([]byte, dataSize)
	if dataSize > 0 {
		bs.ReadBits(U32(dataSize)*8, rejectData)
	}

	LogInfo("Remote host rejected arranged connection: %s", string(rejectData))
	LogInfo("Requesting new game types list")
	
	// Try again
	go func() {
		time.Sleep(2 * time.Second)
		mc.startGameTypesQuery()
	}()
}

// handleSetMOTD handles MOTD from the master server
func (mc *MasterClient) handleSetMOTD(bs *BitStream) {
	motd := ""
	bs.ReadString(motd)
	LogInfo("MOTD: %s", motd)
}

// UpdateServerStatus updates the server status (server side only)
func (mc *MasterClient) UpdateServerStatus() {
	if !mc.isServer || mc.connection == nil {
		return
	}

	// Randomize some values to simulate activity
	mc.mutex.Lock()
	mc.playerCount = U32(rand.Intn(int(mc.maxPlayers)))
	mc.numBots = U32(rand.Intn(10))
	mc.mutex.Unlock()

	mc.connection.SendUpdateServerStatus(
		mc.currentGameType, 
		mc.currentMissionType,
		mc.numBots,
		mc.playerCount,
		mc.maxPlayers,
		mc.infoFlags,
	)
}

// StartPeriodicUpdates starts periodic server status updates (server side only)
func (mc *MasterClient) StartPeriodicUpdates() {
	if !mc.isServer {
		return
	}

	go func() {
		ticker := time.NewTicker(30 * time.Second)
		defer ticker.Stop()

		for {
			select {
			case <-ticker.C:
				mc.UpdateServerStatus()
			case <-mc.stopChan:
				return
			}
		}
	}()
}