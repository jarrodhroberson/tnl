package tnl

import (
	"net"
)

// Master server constants
const (
	ConnectRequestTimeout     = 30000 // 30 seconds
	IPMessageAddressCount     = 30
	GameMissionTypesPerPacket = 20
	DefaultMasterPort         = 29005
)

// Master server error messages
const (
	MasterNoSuchHost      = "No Such Host"
	MasterRequestTimedOut = "Timed Out"
)

// MasterServerInterface defines the RPC interface for the TNL Master Server
type MasterServerInterface interface {
	// Client to Master RPCs
	C2MQueryGameTypes(queryID U32) error
	C2MQueryServers(queryID, regionMask, minPlayers, maxPlayers, infoFlags, maxBots, minCPUSpeed U32, gameType, missionType string) error
	C2MRequestArrangedConnection(requestID U32, remoteAddress, internalAddress *Address, connectionParameters *ByteBuffer) error
	C2MAcceptArrangedConnection(requestID U32, internalAddress *Address, connectionData *ByteBuffer) error
	C2MRejectArrangedConnection(requestID U32, rejectData *ByteBuffer) error
	C2MUpdateServerStatus(gameType, missionType string, botCount, playerCount, maxPlayers, infoFlags U32) error

	// Master to Client RPCs
	M2CQueryGameTypesResponse(queryID U32, gameTypes, missionTypes []string) error
	M2CQueryServersResponse(queryID U32, ipList []*Address) error
	M2CClientRequestedArrangedConnection(requestID U32, possibleAddresses []*Address, connectionParameters *ByteBuffer) error
	M2CArrangedConnectionAccepted(requestID U32, possibleAddresses []*Address, connectionData *ByteBuffer) error
	M2CArrangedConnectionRejected(requestID U32, rejectData *ByteBuffer) error
	M2CSetMOTD(motdString string) error
}

// IPAddress represents an IP address for network communication
type IPAddress struct {
	IP   net.IP
	Port uint16
}

// NewIPAddress creates a new IPAddress
func NewIPAddress(ip net.IP, port uint16) *IPAddress {
	return &IPAddress{
		IP:   ip,
		Port: port,
	}
}

// ToAddress converts IPAddress to Address
func (ip *IPAddress) ToAddress() *Address {
	return &Address{
		host: ip.IP,
		port: ip.Port,
	}
}

// String returns string representation of IPAddress
func (ip *IPAddress) String() string {
	return ip.ToAddress().ToString()
}

// MissionGameType represents a game or mission type
type MissionGameType struct {
	Name string
}

// NewMissionGameType creates a new MissionGameType
func NewMissionGameType(name string) *MissionGameType {
	return &MissionGameType{Name: name}
}

// GameConnectRequest represents a connection request between client and server
type GameConnectRequest struct {
	Initiator        MasterConnection
	Host             MasterConnection
	InitiatorQueryID U32
	HostQueryID      U32
	RequestTime      U32
}

// NewGameConnectRequest creates a new GameConnectRequest
func NewGameConnectRequest() *GameConnectRequest {
	return &GameConnectRequest{
		RequestTime: GetRealMilliseconds(),
	}
}

// MasterConnection interface for master server connections
type MasterConnection interface {
	GetNetAddress() *Address
	GetConnectionState() ConnectionState
	IsGameServer() bool
	GetGameString() string
	GetRegionCode() U32
	GetCPUSpeed() U32
	GetInfoFlags() U32
	GetPlayerCount() U32
	GetMaxPlayers() U32
	GetNumBots() U32
	GetCurrentGameType() *MissionGameType
	GetCurrentMissionType() *MissionGameType
	GetLastActivityTime() U32
	SetLastActivityTime(time U32)
	GetStrikeCount() U32
	SetStrikeCount(count U32)
	Disconnect(reason string) error
	SendRPC(rpcName string, data interface{}) error
}

// ConnectionState represents the state of a network connection
type ConnectionState int

const (
	NotConnected ConnectionState = iota
	AwaitingChallengeResponse
	SendingPunchPackets
	ComputingPuzzleSolution
	AwaitingConnectResponse
	ConnectTimedOut
	ConnectRejected
	Connected
	Disconnected
	TimedOut
	StateCount
)

// String returns string representation of ConnectionState
func (cs ConnectionState) String() string {
	switch cs {
	case NotConnected:
		return "NotConnected"
	case AwaitingChallengeResponse:
		return "AwaitingChallengeResponse"
	case SendingPunchPackets:
		return "SendingPunchPackets"
	case ComputingPuzzleSolution:
		return "ComputingPuzzleSolution"
	case AwaitingConnectResponse:
		return "AwaitingConnectResponse"
	case ConnectTimedOut:
		return "ConnectTimedOut"
	case ConnectRejected:
		return "ConnectRejected"
	case Connected:
		return "Connected"
	case Disconnected:
		return "Disconnected"
	case TimedOut:
		return "TimedOut"
	default:
		return "Unknown"
	}
}

// MasterServerConfig holds configuration for the master server
type MasterServerConfig struct {
	Port     U32
	MOTDList []MOTDEntry
}

// MOTDEntry represents a Message of the Day entry
type MOTDEntry struct {
	GameType string
	Message  string
}

// NewMasterServerConfig creates a new master server configuration
func NewMasterServerConfig() *MasterServerConfig {
	return &MasterServerConfig{
		Port:     DefaultMasterPort,
		MOTDList: make([]MOTDEntry, 0),
	}
}

// AddMOTD adds a Message of the Day entry
func (config *MasterServerConfig) AddMOTD(gameType, message string) {
	config.MOTDList = append(config.MOTDList, MOTDEntry{
		GameType: gameType,
		Message:  message,
	})
}

// GetMOTD returns the best matching MOTD for a game type
func (config *MasterServerConfig) GetMOTD(gameType string) string {
	bestMatch := "Welcome to TNL-Go. Have a nice day."
	maxMatchLen := 0

	for _, entry := range config.MOTDList {
		matchLen := 0
		for i := 0; i < len(entry.GameType) && i < len(gameType) && entry.GameType[i] == gameType[i]; i++ {
			matchLen++
		}
		if matchLen > maxMatchLen {
			maxMatchLen = matchLen
			bestMatch = entry.Message
		}
	}

	return bestMatch
}

// ServerInfo holds information about a game server
type ServerInfo struct {
	Address           *Address
	GameString        string
	RegionCode        U32
	CPUSpeed          U32
	InfoFlags         U32
	PlayerCount       U32
	MaxPlayers        U32
	NumBots           U32
	CurrentGameType   *MissionGameType
	CurrentMissionType *MissionGameType
	LastUpdate        U32
}

// NewServerInfo creates a new ServerInfo
func NewServerInfo(address *Address) *ServerInfo {
	return &ServerInfo{
		Address:    address,
		LastUpdate: GetRealMilliseconds(),
	}
}

// MatchesFilter checks if the server matches the given filter criteria
func (si *ServerInfo) MatchesFilter(gameString string, regionMask, minPlayers, maxPlayers, infoFlags, maxBots, minCPUSpeed U32, gameType, missionType string) bool {
	// Check game string
	if si.GameString != gameString {
		return false
	}

	// Check region
	if (si.RegionCode & regionMask) == 0 {
		return false
	}

	// Check player count
	if si.PlayerCount > maxPlayers || si.PlayerCount < minPlayers {
		return false
	}

	// Check info flags
	if (infoFlags & ^si.InfoFlags) != 0 {
		return false
	}

	// Check bot count
	if maxBots < si.NumBots {
		return false
	}

	// Check CPU speed
	if minCPUSpeed > si.CPUSpeed {
		return false
	}

	// Check game type
	if gameType != "" && si.CurrentGameType != nil && gameType != si.CurrentGameType.Name {
		return false
	}

	// Check mission type
	if missionType != "" && si.CurrentMissionType != nil && missionType != si.CurrentMissionType.Name {
		return false
	}

	return true
}

// UpdateStatus updates the server status information
func (si *ServerInfo) UpdateStatus(gameType, missionType string, botCount, playerCount, maxPlayers, infoFlags U32) {
	si.CurrentGameType = NewMissionGameType(gameType)
	si.CurrentMissionType = NewMissionGameType(missionType)
	si.NumBots = botCount
	si.PlayerCount = playerCount
	si.MaxPlayers = maxPlayers
	si.InfoFlags = infoFlags
	si.LastUpdate = GetRealMilliseconds()
}

// IsExpired checks if the server info has expired
func (si *ServerInfo) IsExpired(timeout U32) bool {
	return GetRealMilliseconds()-si.LastUpdate > timeout
}