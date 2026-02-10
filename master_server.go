package tnl

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

// MasterServer implements the TNL Master Server
type MasterServer struct {
	config          *MasterServerConfig
	socket          Socket
	connections     map[string]*MasterServerConnection
	servers         map[string]*ServerInfo
	gameTypes       []*MissionGameType
	missionTypes    []*MissionGameType
	connectRequests []*GameConnectRequest
	mutex           sync.RWMutex
	running         bool
	stopChan        chan bool
}

// NewMasterServer creates a new master server
func NewMasterServer(config *MasterServerConfig) *MasterServer {
	return &MasterServer{
		config:          config,
		connections:     make(map[string]*MasterServerConnection),
		servers:         make(map[string]*ServerInfo),
		gameTypes:       make([]*MissionGameType, 0),
		missionTypes:    make([]*MissionGameType, 0),
		connectRequests: make([]*GameConnectRequest, 0),
		stopChan:        make(chan bool),
	}
}

// Start starts the master server
func (ms *MasterServer) Start() error {
	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	if ms.running {
		return fmt.Errorf("master server is already running")
	}

	// Create and bind socket
	ms.socket = NewUDPSocket()
	bindAddr := NewAddress("0.0.0.0", uint16(ms.config.Port))
	if err := ms.socket.Bind(bindAddr); err != nil {
		return fmt.Errorf("failed to bind to port %d: %v", ms.config.Port, err)
	}

	ms.running = true
	LogInfo("Master Server started - listening on port %d", ms.config.Port)

	// Start the main processing loop
	go ms.processLoop()

	// Start the cleanup routine
	go ms.cleanupLoop()

	return nil
}

// Stop stops the master server
func (ms *MasterServer) Stop() {
	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	if !ms.running {
		return
	}

	ms.running = false
	close(ms.stopChan)

	if ms.socket != nil {
		ms.socket.Close()
	}

	LogInfo("Master Server stopped")
}

// processLoop is the main processing loop for the master server
func (ms *MasterServer) processLoop() {
	buffer := make([]byte, MaxPacketDataSize)

	for ms.running {
		// Check for incoming packets
		n, addr, err := ms.socket.RecvFrom(buffer)
		if err != nil {
			if ms.running {
				LogError("Error receiving packet: %v", err)
			}
			continue
		}

		// Process the packet
		go ms.processPacket(buffer[:n], addr)

		// Small delay to prevent CPU spinning
		time.Sleep(1 * time.Millisecond)
	}
}

// cleanupLoop periodically cleans up expired connections and requests
func (ms *MasterServer) cleanupLoop() {
	ticker := time.NewTicker(5 * time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			ms.cleanup()
		case <-ms.stopChan:
			return
		}
	}
}

// cleanup removes expired connections and requests
func (ms *MasterServer) cleanup() {
	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	currentTime := GetRealMilliseconds()

	// Clean up expired connect requests
	for i := len(ms.connectRequests) - 1; i >= 0; i-- {
		req := ms.connectRequests[i]
		if currentTime-req.RequestTime > ConnectRequestTimeout {
			LogInfo("Connect request from %s to %s timed out",
				req.Initiator.GetNetAddress().ToString(),
				req.Host.GetNetAddress().ToString())

			// Remove from slice
			ms.connectRequests = append(ms.connectRequests[:i], ms.connectRequests[i+1:]...)
		}
	}

	// Clean up expired server info
	for addr, server := range ms.servers {
		if server.IsExpired(60000) { // 60 second timeout
			delete(ms.servers, addr)
			LogInfo("Server %s expired and removed", addr)
		}
	}
}

// processPacket processes an incoming packet
func (ms *MasterServer) processPacket(data []byte, addr *Address) {
	// Create a BitStream from the packet data
	bs := NewBitStream(data)

	// Read packet type (simplified - in a full implementation this would be more complex)
	packetType := bs.ReadInt(8)

	switch packetType {
	case 1: // Connection request
		ms.handleConnectionRequest(bs, addr)
	case 2: // RPC call
		ms.handleRPCCall(bs, addr)
	default:
		LogWarning("Unknown packet type %d from %s", packetType, addr.ToString())
	}
}

// handleConnectionRequest handles a new connection request
func (ms *MasterServer) handleConnectionRequest(bs *BitStream, addr *Address) {
	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	// Check if connection already exists
	addrStr := addr.ToString()
	if _, exists := ms.connections[addrStr]; exists {
		LogWarning("Connection already exists for %s", addrStr)
		return
	}

	// Read connection parameters
	gameString := ""
	bs.ReadString(gameString)

	isGameServer := bs.ReadFlag()

	conn := NewMasterServerConnection(addr, ms)
	conn.gameString = gameString
	conn.isGameServer = isGameServer

	if isGameServer {
		// Read server-specific information
		conn.cpuSpeed = bs.ReadInt(32)
		conn.regionCode = bs.ReadInt(32)
		conn.numBots = bs.ReadInt(32)
		conn.playerCount = bs.ReadInt(32)
		conn.maxPlayers = bs.ReadInt(32)
		conn.infoFlags = bs.ReadInt(32)

		gameType := ""
		bs.ReadString(gameType)
		missionType := ""
		bs.ReadString(missionType)

		conn.currentGameType = ms.getOrCreateGameType(gameType)
		conn.currentMissionType = ms.getOrCreateMissionType(missionType)

		// Add to server list
		serverInfo := NewServerInfo(addr)
		serverInfo.GameString = gameString
		serverInfo.CPUSpeed = conn.cpuSpeed
		serverInfo.RegionCode = conn.regionCode
		serverInfo.NumBots = conn.numBots
		serverInfo.PlayerCount = conn.playerCount
		serverInfo.MaxPlayers = conn.maxPlayers
		serverInfo.InfoFlags = conn.infoFlags
		serverInfo.CurrentGameType = conn.currentGameType
		serverInfo.CurrentMissionType = conn.currentMissionType

		ms.servers[addrStr] = serverInfo
	}

	ms.connections[addrStr] = conn

	LogInfo("%s connected at %s", 
		map[bool]string{true: "Server", false: "Client"}[isGameServer], 
		addr.ToString())

	// Send MOTD if client supports it
	if !isGameServer {
		motd := ms.config.GetMOTD(gameString)
		conn.SendMOTD(motd)
	}
}

// handleRPCCall handles an RPC call from a client
func (ms *MasterServer) handleRPCCall(bs *BitStream, addr *Address) {
	ms.mutex.RLock()
	conn, exists := ms.connections[addr.ToString()]
	ms.mutex.RUnlock()

	if !exists {
		LogWarning("RPC call from unknown connection %s", addr.ToString())
		return
	}

	// Read RPC ID
	rpcID := bs.ReadInt(16)

	switch rpcID {
	case 1: // QueryGameTypes
		ms.handleQueryGameTypes(conn, bs)
	case 2: // QueryServers
		ms.handleQueryServers(conn, bs)
	case 3: // RequestArrangedConnection
		ms.handleRequestArrangedConnection(conn, bs)
	case 4: // AcceptArrangedConnection
		ms.handleAcceptArrangedConnection(conn, bs)
	case 5: // RejectArrangedConnection
		ms.handleRejectArrangedConnection(conn, bs)
	case 6: // UpdateServerStatus
		ms.handleUpdateServerStatus(conn, bs)
	default:
		LogWarning("Unknown RPC ID %d from %s", rpcID, addr.ToString())
	}
}

// handleQueryGameTypes handles a game types query
func (ms *MasterServer) handleQueryGameTypes(conn *MasterServerConnection, bs *BitStream) {
	queryID := bs.ReadInt(32)

	ms.mutex.RLock()
	defer ms.mutex.RUnlock()

	// Clean up null entries and build response
	gameTypes := make([]string, 0)
	missionTypes := make([]string, 0)

	for _, gt := range ms.gameTypes {
		if gt != nil {
			gameTypes = append(gameTypes, gt.Name)
		}
	}

	for _, mt := range ms.missionTypes {
		if mt != nil {
			missionTypes = append(missionTypes, mt.Name)
		}
	}

	// Send response in chunks
	ms.sendGameTypesResponse(conn, queryID, gameTypes, missionTypes)
}

// sendGameTypesResponse sends game types response in chunks
func (ms *MasterServer) sendGameTypesResponse(conn *MasterServerConnection, queryID U32, gameTypes, missionTypes []string) {
	gameIdx := 0
	missionIdx := 0

	for gameIdx < len(gameTypes) || missionIdx < len(missionTypes) {
		chunkGameTypes := make([]string, 0, GameMissionTypesPerPacket)
		chunkMissionTypes := make([]string, 0, GameMissionTypesPerPacket)
		count := 0

		// Add game types to chunk
		for gameIdx < len(gameTypes) && count < GameMissionTypesPerPacket {
			chunkGameTypes = append(chunkGameTypes, gameTypes[gameIdx])
			gameIdx++
			count++
		}

		// Add mission types to chunk
		for missionIdx < len(missionTypes) && count < GameMissionTypesPerPacket {
			chunkMissionTypes = append(chunkMissionTypes, missionTypes[missionIdx])
			missionIdx++
			count++
		}

		conn.SendGameTypesResponse(queryID, chunkGameTypes, chunkMissionTypes)
	}

	// Send final empty response to indicate completion
	conn.SendGameTypesResponse(queryID, []string{}, []string{})
}

// handleQueryServers handles a server list query
func (ms *MasterServer) handleQueryServers(conn *MasterServerConnection, bs *BitStream) {
	queryID := bs.ReadInt(32)
	regionMask := bs.ReadInt(32)
	minPlayers := bs.ReadInt(32)
	maxPlayers := bs.ReadInt(32)
	infoFlags := bs.ReadInt(32)
	maxBots := bs.ReadInt(32)
	minCPUSpeed := bs.ReadInt(32)

	gameType := ""
	bs.ReadString(gameType)
	missionType := ""
	bs.ReadString(missionType)

	ms.mutex.RLock()
	defer ms.mutex.RUnlock()

	// Find matching servers
	matchingServers := make([]*Address, 0)

	for _, server := range ms.servers {
		if server.MatchesFilter(conn.gameString, regionMask, minPlayers, maxPlayers, infoFlags, maxBots, minCPUSpeed, gameType, missionType) {
			matchingServers = append(matchingServers, server.Address)
		}
	}

	// Send response in chunks
	ms.sendServersResponse(conn, queryID, matchingServers)
}

// sendServersResponse sends server list response in chunks
func (ms *MasterServer) sendServersResponse(conn *MasterServerConnection, queryID U32, servers []*Address) {
	for i := 0; i < len(servers); i += IPMessageAddressCount {
		end := i + IPMessageAddressCount
		if end > len(servers) {
			end = len(servers)
		}

		chunk := servers[i:end]
		conn.SendServersResponse(queryID, chunk)
	}

	// Send final empty response to indicate completion
	if len(servers) > 0 {
		conn.SendServersResponse(queryID, []*Address{})
	}
}

// handleRequestArrangedConnection handles an arranged connection request
func (ms *MasterServer) handleRequestArrangedConnection(conn *MasterServerConnection, bs *BitStream) {
	requestID := bs.ReadInt(32)
	
	// Read remote address
	remoteIP := make([]byte, 4)
	bs.ReadBits(32, remoteIP)
	remotePort := bs.ReadInt(16)
	remoteAddr := NewAddress(string(remoteIP), uint16(remotePort))

	// Read internal address
	internalIP := make([]byte, 4)
	bs.ReadBits(32, internalIP)
	internalPort := bs.ReadInt(16)
	internalAddr := NewAddress(string(internalIP), uint16(internalPort))

	// Read connection parameters
	paramSize := bs.ReadInt(16)
	paramData := make([]byte, paramSize)
	bs.ReadBits(U32(paramSize)*8, paramData)
	connectionParams := NewByteBufferFromData(paramData)

	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	// Find the target server
	hostConn, exists := ms.connections[remoteAddr.ToString()]
	if !exists {
		// Server not found
		rejectData := NewByteBufferFromData([]byte(MasterNoSuchHost))
		conn.SendArrangedConnectionRejected(requestID, rejectData)
		return
	}

	// Create connect request
	req := NewGameConnectRequest()
	req.Initiator = conn
	req.Host = hostConn
	req.InitiatorQueryID = requestID
	req.HostQueryID = ms.getNextQueryID()

	ms.connectRequests = append(ms.connectRequests, req)

	LogInfo("Client %s requested connection to %s", 
		conn.GetNetAddress().ToString(), 
		hostConn.GetNetAddress().ToString())

	// Check for DOS attacks
	conn.checkActivityTime(2000)

	// Build possible addresses for the client
	possibleAddresses := make([]*Address, 0)
	
	// Add client address with port+1
	clientAddr := conn.GetNetAddress()
	altAddr := NewAddress(clientAddr.GetHost().String(), clientAddr.GetPort()+1)
	possibleAddresses = append(possibleAddresses, altAddr)
	
	// Add original client address
	possibleAddresses = append(possibleAddresses, clientAddr)
	
	// Add internal address if different
	if !internalAddr.Equals(clientAddr) {
		possibleAddresses = append(possibleAddresses, internalAddr)
	}

	// Notify the host server
	hostConn.SendClientRequestedArrangedConnection(req.HostQueryID, possibleAddresses, connectionParams)
}

// handleAcceptArrangedConnection handles acceptance of an arranged connection
func (ms *MasterServer) handleAcceptArrangedConnection(conn *MasterServerConnection, bs *BitStream) {
	requestID := bs.ReadInt(32)

	// Read internal address
	internalIP := make([]byte, 4)
	bs.ReadBits(32, internalIP)
	internalPort := bs.ReadInt(16)
	internalAddr := NewAddress(string(internalIP), uint16(internalPort))

	// Read connection data
	dataSize := bs.ReadInt(16)
	data := make([]byte, dataSize)
	bs.ReadBits(U32(dataSize)*8, data)
	connectionData := NewByteBufferFromData(data)

	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	// Find and remove the request
	req := ms.findAndRemoveRequest(requestID)
	if req == nil {
		return
	}

	LogInfo("Server %s accepted connection request from %s",
		conn.GetNetAddress().ToString(),
		req.Initiator.GetNetAddress().ToString())

	// Build possible addresses for the server
	possibleAddresses := make([]*Address, 0)
	
	serverAddr := conn.GetNetAddress()
	altAddr := NewAddress(serverAddr.GetHost().String(), serverAddr.GetPort()+1)
	possibleAddresses = append(possibleAddresses, altAddr)
	possibleAddresses = append(possibleAddresses, serverAddr)
	
	if !internalAddr.Equals(serverAddr) {
		possibleAddresses = append(possibleAddresses, internalAddr)
	}

	// Notify the initiator
	req.Initiator.SendArrangedConnectionAccepted(req.InitiatorQueryID, possibleAddresses, connectionData)
}

// handleRejectArrangedConnection handles rejection of an arranged connection
func (ms *MasterServer) handleRejectArrangedConnection(conn *MasterServerConnection, bs *BitStream) {
	requestID := bs.ReadInt(32)

	// Read reject data
	dataSize := bs.ReadInt(16)
	data := make([]byte, dataSize)
	bs.ReadBits(U32(dataSize)*8, data)
	rejectData := NewByteBufferFromData(data)

	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	// Find and remove the request
	req := ms.findAndRemoveRequest(requestID)
	if req == nil {
		return
	}

	LogInfo("Server %s rejected connection request from %s",
		conn.GetNetAddress().ToString(),
		req.Initiator.GetNetAddress().ToString())

	// Notify the initiator
	req.Initiator.SendArrangedConnectionRejected(req.InitiatorQueryID, rejectData)
}

// handleUpdateServerStatus handles a server status update
func (ms *MasterServer) handleUpdateServerStatus(conn *MasterServerConnection, bs *BitStream) {
	if !conn.isGameServer {
		return
	}

	gameType := ""
	bs.ReadString(gameType)
	missionType := ""
	bs.ReadString(missionType)
	
	botCount := bs.ReadInt(32)
	playerCount := bs.ReadInt(32)
	maxPlayers := bs.ReadInt(32)
	infoFlags := bs.ReadInt(32)

	ms.mutex.Lock()
	defer ms.mutex.Unlock()

	conn.currentGameType = ms.getOrCreateGameType(gameType)
	conn.currentMissionType = ms.getOrCreateMissionType(missionType)
	conn.numBots = botCount
	conn.playerCount = playerCount
	conn.maxPlayers = maxPlayers
	conn.infoFlags = infoFlags

	// Update server info
	addrStr := conn.GetNetAddress().ToString()
	if server, exists := ms.servers[addrStr]; exists {
		server.UpdateStatus(gameType, missionType, botCount, playerCount, maxPlayers, infoFlags)
	}

	conn.checkActivityTime(15000)

	LogInfo("Server %s updated status (%s, %s, %d, %d, %d)",
		conn.GetNetAddress().ToString(), gameType, missionType, botCount, playerCount, maxPlayers)
}

// Helper methods

func (ms *MasterServer) getOrCreateGameType(name string) *MissionGameType {
	for _, gt := range ms.gameTypes {
		if gt != nil && gt.Name == name {
			return gt
		}
	}
	
	newType := NewMissionGameType(name)
	ms.gameTypes = append(ms.gameTypes, newType)
	return newType
}

func (ms *MasterServer) getOrCreateMissionType(name string) *MissionGameType {
	for _, mt := range ms.missionTypes {
		if mt != nil && mt.Name == name {
			return mt
		}
	}
	
	newType := NewMissionGameType(name)
	ms.missionTypes = append(ms.missionTypes, newType)
	return newType
}

func (ms *MasterServer) findAndRemoveRequest(requestID U32) *GameConnectRequest {
	for i, req := range ms.connectRequests {
		if req.HostQueryID == requestID {
			// Remove from slice
			ms.connectRequests = append(ms.connectRequests[:i], ms.connectRequests[i+1:]...)
			return req
		}
	}
	return nil
}

var nextQueryID U32 = 1

func (ms *MasterServer) getNextQueryID() U32 {
	id := nextQueryID
	nextQueryID++
	return id
}

// LoadConfig loads configuration from a file
func LoadMasterServerConfig(filename string) (*MasterServerConfig, error) {
	config := NewMasterServerConfig()

	file, err := os.Open(filename)
	if err != nil {
		LogWarning("Unable to open config file %s: %v", filename, err)
		return config, nil // Return default config
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}

		parts := strings.Fields(line)
		if len(parts) < 2 {
			continue
		}

		switch strings.ToLower(parts[0]) {
		case "port":
			if port, err := strconv.ParseUint(parts[1], 10, 32); err == nil {
				config.Port = U32(port)
			}
		case "motd":
			if len(parts) >= 3 {
				gameType := parts[1]
				message := strings.Join(parts[2:], " ")
				// Handle quoted strings
				if strings.HasPrefix(message, "\"") && strings.HasSuffix(message, "\"") {
					message = message[1 : len(message)-1]
				}
				config.AddMOTD(gameType, message)
			}
		}
	}

	return config, scanner.Err()
}

// RunMasterServer runs the master server with the given configuration
func RunMasterServer(configFile string) error {
	// Initialize TNL
	if err := Init(); err != nil {
		return fmt.Errorf("failed to initialize TNL: %v", err)
	}
	defer Shutdown()

	// Load configuration
	config, err := LoadMasterServerConfig(configFile)
	if err != nil {
		return fmt.Errorf("failed to load config: %v", err)
	}

	// Create and start master server
	server := NewMasterServer(config)
	if err := server.Start(); err != nil {
		return fmt.Errorf("failed to start master server: %v", err)
	}
	defer server.Stop()

	// Wait for interrupt
	LogInfo("Master server running. Press Ctrl+C to stop.")
	select {} // Block forever
}