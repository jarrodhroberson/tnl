package tnl

import (
	"fmt"
	"sync"
)

// MasterServerConnection represents a connection to/from the master server
type MasterServerConnection struct {
	address             *Address
	server              *MasterServer
	gameString          string
	isGameServer        bool
	strikeCount         U32
	lastActivityTime    U32
	lastQueryID         U32
	cpuSpeed            U32
	regionCode          U32
	playerCount         U32
	maxPlayers          U32
	numBots             U32
	infoFlags           U32
	currentGameType     *MissionGameType
	currentMissionType  *MissionGameType
	connectRequests     []*GameConnectRequest
	connectionState     ConnectionState
	mutex               sync.RWMutex
}

// NewMasterServerConnection creates a new master server connection
func NewMasterServerConnection(address *Address, server *MasterServer) *MasterServerConnection {
	return &MasterServerConnection{
		address:         address,
		server:          server,
		connectionState: Connected,
		lastActivityTime: GetRealMilliseconds(),
	}
}

// Implement MasterConnection interface

func (msc *MasterServerConnection) GetNetAddress() *Address {
	return msc.address
}

func (msc *MasterServerConnection) GetConnectionState() ConnectionState {
	msc.mutex.RLock()
	defer msc.mutex.RUnlock()
	return msc.connectionState
}

func (msc *MasterServerConnection) IsGameServer() bool {
	return msc.isGameServer
}

func (msc *MasterServerConnection) GetGameString() string {
	return msc.gameString
}

func (msc *MasterServerConnection) GetRegionCode() U32 {
	return msc.regionCode
}

func (msc *MasterServerConnection) GetCPUSpeed() U32 {
	return msc.cpuSpeed
}

func (msc *MasterServerConnection) GetInfoFlags() U32 {
	return msc.infoFlags
}

func (msc *MasterServerConnection) GetPlayerCount() U32 {
	return msc.playerCount
}

func (msc *MasterServerConnection) GetMaxPlayers() U32 {
	return msc.maxPlayers
}

func (msc *MasterServerConnection) GetNumBots() U32 {
	return msc.numBots
}

func (msc *MasterServerConnection) GetCurrentGameType() *MissionGameType {
	return msc.currentGameType
}

func (msc *MasterServerConnection) GetCurrentMissionType() *MissionGameType {
	return msc.currentMissionType
}

func (msc *MasterServerConnection) GetLastActivityTime() U32 {
	msc.mutex.RLock()
	defer msc.mutex.RUnlock()
	return msc.lastActivityTime
}

func (msc *MasterServerConnection) SetLastActivityTime(time U32) {
	msc.mutex.Lock()
	defer msc.mutex.Unlock()
	msc.lastActivityTime = time
}

func (msc *MasterServerConnection) GetStrikeCount() U32 {
	msc.mutex.RLock()
	defer msc.mutex.RUnlock()
	return msc.strikeCount
}

func (msc *MasterServerConnection) SetStrikeCount(count U32) {
	msc.mutex.Lock()
	defer msc.mutex.Unlock()
	msc.strikeCount = count
}

func (msc *MasterServerConnection) Disconnect(reason string) error {
	msc.mutex.Lock()
	defer msc.mutex.Unlock()
	
	msc.connectionState = Disconnected
	LogInfo("Connection %s disconnected: %s", msc.address.ToString(), reason)
	
	// Remove from server's connection list
	if msc.server != nil {
		msc.server.mutex.Lock()
		delete(msc.server.connections, msc.address.ToString())
		if msc.isGameServer {
			delete(msc.server.servers, msc.address.ToString())
		}
		msc.server.mutex.Unlock()
	}
	
	return nil
}

func (msc *MasterServerConnection) SendRPC(rpcName string, data interface{}) error {
	// In a full implementation, this would serialize and send the RPC
	// For now, we'll just log it
	LogDebug("Sending RPC %s to %s", rpcName, msc.address.ToString())
	return nil
}

// checkActivityTime validates that this connection is not issuing too many requests
func (msc *MasterServerConnection) checkActivityTime(timeDeltaMinimum U32) {
	currentTime := GetRealMilliseconds()
	
	msc.mutex.Lock()
	defer msc.mutex.Unlock()
	
	if currentTime-msc.lastActivityTime < timeDeltaMinimum {
		msc.strikeCount++
		if msc.strikeCount >= 3 {
			go msc.Disconnect("Too many requests - you're out!")
		}
	} else if msc.strikeCount > 0 {
		msc.strikeCount--
	}
	
	msc.lastActivityTime = currentTime
}

// RPC sending methods

func (msc *MasterServerConnection) SendGameTypesResponse(queryID U32, gameTypes, missionTypes []string) error {
	// Create response packet
	bs := NewResizableBitStream()
	
	// Write packet type
	bs.WriteInt(2, 8) // RPC packet
	
	// Write RPC ID
	bs.WriteInt(1, 16) // QueryGameTypesResponse
	
	// Write parameters
	bs.WriteInt(queryID, 32)
	
	// Write game types
	bs.WriteInt(U32(len(gameTypes)), 16)
	for _, gt := range gameTypes {
		bs.WriteString(gt, 255)
	}
	
	// Write mission types
	bs.WriteInt(U32(len(missionTypes)), 16)
	for _, mt := range missionTypes {
		bs.WriteString(mt, 255)
	}
	
	// Send packet
	return msc.sendPacket(bs)
}

func (msc *MasterServerConnection) SendServersResponse(queryID U32, servers []*Address) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8) // RPC packet
	bs.WriteInt(2, 16) // QueryServersResponse
	bs.WriteInt(queryID, 32)
	
	// Write server addresses
	bs.WriteInt(U32(len(servers)), 16)
	for _, addr := range servers {
		// Write IP address (4 bytes)
		ip := addr.GetHost().To4()
		if ip != nil {
			bs.WriteBits(32, ip)
		} else {
			bs.WriteBits(32, []byte{0, 0, 0, 0})
		}
		// Write port
		bs.WriteInt(U32(addr.GetPort()), 16)
	}
	
	return msc.sendPacket(bs)
}

func (msc *MasterServerConnection) SendClientRequestedArrangedConnection(requestID U32, possibleAddresses []*Address, connectionParams *ByteBuffer) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8) // RPC packet
	bs.WriteInt(3, 16) // ClientRequestedArrangedConnection
	bs.WriteInt(requestID, 32)
	
	// Write possible addresses
	bs.WriteInt(U32(len(possibleAddresses)), 16)
	for _, addr := range possibleAddresses {
		ip := addr.GetHost().To4()
		if ip != nil {
			bs.WriteBits(32, ip)
		} else {
			bs.WriteBits(32, []byte{0, 0, 0, 0})
		}
		bs.WriteInt(U32(addr.GetPort()), 16)
	}
	
	// Write connection parameters
	if connectionParams != nil {
		data := connectionParams.GetData()
		bs.WriteInt(U32(len(data)), 16)
		bs.WriteBits(U32(len(data))*8, data)
	} else {
		bs.WriteInt(0, 16)
	}
	
	return msc.sendPacket(bs)
}

func (msc *MasterServerConnection) SendArrangedConnectionAccepted(requestID U32, possibleAddresses []*Address, connectionData *ByteBuffer) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8) // RPC packet
	bs.WriteInt(4, 16) // ArrangedConnectionAccepted
	bs.WriteInt(requestID, 32)
	
	// Write possible addresses
	bs.WriteInt(U32(len(possibleAddresses)), 16)
	for _, addr := range possibleAddresses {
		ip := addr.GetHost().To4()
		if ip != nil {
			bs.WriteBits(32, ip)
		} else {
			bs.WriteBits(32, []byte{0, 0, 0, 0})
		}
		bs.WriteInt(U32(addr.GetPort()), 16)
	}
	
	// Write connection data
	if connectionData != nil {
		data := connectionData.GetData()
		bs.WriteInt(U32(len(data)), 16)
		bs.WriteBits(U32(len(data))*8, data)
	} else {
		bs.WriteInt(0, 16)
	}
	
	return msc.sendPacket(bs)
}

func (msc *MasterServerConnection) SendArrangedConnectionRejected(requestID U32, rejectData *ByteBuffer) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8) // RPC packet
	bs.WriteInt(5, 16) // ArrangedConnectionRejected
	bs.WriteInt(requestID, 32)
	
	// Write reject data
	if rejectData != nil {
		data := rejectData.GetData()
		bs.WriteInt(U32(len(data)), 16)
		bs.WriteBits(U32(len(data))*8, data)
	} else {
		bs.WriteInt(0, 16)
	}
	
	return msc.sendPacket(bs)
}

func (msc *MasterServerConnection) SendMOTD(motd string) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8) // RPC packet
	bs.WriteInt(6, 16) // SetMOTD
	bs.WriteString(motd, 255)
	
	return msc.sendPacket(bs)
}

// sendPacket sends a packet to the connection's address
func (msc *MasterServerConnection) sendPacket(bs *BitStream) error {
	if msc.server == nil || msc.server.socket == nil {
		return fmt.Errorf("no server socket available")
	}
	
	dataSize := bs.GetBytePosition()
	data := bs.GetBuffer()[:dataSize]
	
	return msc.server.socket.SendTo(data, msc.address)
}

// removeConnectRequest removes a connect request from this connection's list
func (msc *MasterServerConnection) removeConnectRequest(req *GameConnectRequest) {
	msc.mutex.Lock()
	defer msc.mutex.Unlock()
	
	for i, r := range msc.connectRequests {
		if r == req {
			msc.connectRequests = append(msc.connectRequests[:i], msc.connectRequests[i+1:]...)
			break
		}
	}
}