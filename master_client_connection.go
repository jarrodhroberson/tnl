package tnl

import (
	"fmt"
)

// MasterClientConnection represents a client's connection to the master server
type MasterClientConnection struct {
	masterAddress   *Address
	client          *MasterClient
	connectionState ConnectionState
}

// NewMasterClientConnection creates a new master client connection
func NewMasterClientConnection(masterAddress *Address, client *MasterClient) *MasterClientConnection {
	return &MasterClientConnection{
		masterAddress:   masterAddress,
		client:          client,
		connectionState: NotConnected,
	}
}

// Connect connects to the master server
func (mcc *MasterClientConnection) Connect() error {
	mcc.connectionState = AwaitingConnectResponse

	// Create connection request packet
	bs := NewResizableBitStream()
	
	// Write packet type
	bs.WriteInt(1, 8) // Connection request
	
	// Write game string
	bs.WriteString(mcc.client.gameString, 255)
	
	// Write server flag and server-specific data
	if bs.WriteFlag(mcc.client.isServer) {
		bs.WriteInt(mcc.client.cpuSpeed, 32)
		bs.WriteInt(mcc.client.regionCode, 32)
		bs.WriteInt(mcc.client.numBots, 32)
		bs.WriteInt(mcc.client.playerCount, 32)
		bs.WriteInt(mcc.client.maxPlayers, 32)
		bs.WriteInt(mcc.client.infoFlags, 32)
		bs.WriteString(mcc.client.currentGameType, 255)
		bs.WriteString(mcc.client.currentMissionType, 255)
	}

	// Send the packet
	return mcc.sendPacket(bs)
}

// GetConnectionState returns the current connection state
func (mcc *MasterClientConnection) GetConnectionState() ConnectionState {
	return mcc.connectionState
}

// SetConnectionState sets the connection state
func (mcc *MasterClientConnection) SetConnectionState(state ConnectionState) {
	mcc.connectionState = state
}

// RPC sending methods

func (mcc *MasterClientConnection) SendQueryGameTypes(queryID U32) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8)  // RPC packet
	bs.WriteInt(1, 16) // QueryGameTypes
	bs.WriteInt(queryID, 32)
	
	return mcc.sendPacket(bs)
}

func (mcc *MasterClientConnection) SendQueryServers(queryID, regionMask, minPlayers, maxPlayers, infoFlags, maxBots, minCPUSpeed U32, gameType, missionType string) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8)  // RPC packet
	bs.WriteInt(2, 16) // QueryServers
	bs.WriteInt(queryID, 32)
	bs.WriteInt(regionMask, 32)
	bs.WriteInt(minPlayers, 32)
	bs.WriteInt(maxPlayers, 32)
	bs.WriteInt(infoFlags, 32)
	bs.WriteInt(maxBots, 32)
	bs.WriteInt(minCPUSpeed, 32)
	bs.WriteString(gameType, 255)
	bs.WriteString(missionType, 255)
	
	return mcc.sendPacket(bs)
}

func (mcc *MasterClientConnection) SendRequestArrangedConnection(requestID U32, remoteAddress, internalAddress *Address, connectionParams *ByteBuffer) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8)  // RPC packet
	bs.WriteInt(3, 16) // RequestArrangedConnection
	bs.WriteInt(requestID, 32)
	
	// Write remote address
	remoteIP := remoteAddress.GetHost().To4()
	if remoteIP != nil {
		bs.WriteBits(32, remoteIP)
	} else {
		bs.WriteBits(32, []byte{0, 0, 0, 0})
	}
	bs.WriteInt(U32(remoteAddress.GetPort()), 16)
	
	// Write internal address
	internalIP := internalAddress.GetHost().To4()
	if internalIP != nil {
		bs.WriteBits(32, internalIP)
	} else {
		bs.WriteBits(32, []byte{0, 0, 0, 0})
	}
	bs.WriteInt(U32(internalAddress.GetPort()), 16)
	
	// Write connection parameters
	if connectionParams != nil {
		data := connectionParams.GetData()
		bs.WriteInt(U32(len(data)), 16)
		bs.WriteBits(U32(len(data))*8, data)
	} else {
		bs.WriteInt(0, 16)
	}
	
	return mcc.sendPacket(bs)
}

func (mcc *MasterClientConnection) SendAcceptArrangedConnection(requestID U32, internalAddress *Address, connectionData *ByteBuffer) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8)  // RPC packet
	bs.WriteInt(4, 16) // AcceptArrangedConnection
	bs.WriteInt(requestID, 32)
	
	// Write internal address
	internalIP := internalAddress.GetHost().To4()
	if internalIP != nil {
		bs.WriteBits(32, internalIP)
	} else {
		bs.WriteBits(32, []byte{0, 0, 0, 0})
	}
	bs.WriteInt(U32(internalAddress.GetPort()), 16)
	
	// Write connection data
	if connectionData != nil {
		data := connectionData.GetData()
		bs.WriteInt(U32(len(data)), 16)
		bs.WriteBits(U32(len(data))*8, data)
	} else {
		bs.WriteInt(0, 16)
	}
	
	return mcc.sendPacket(bs)
}

func (mcc *MasterClientConnection) SendRejectArrangedConnection(requestID U32, rejectData *ByteBuffer) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8)  // RPC packet
	bs.WriteInt(5, 16) // RejectArrangedConnection
	bs.WriteInt(requestID, 32)
	
	// Write reject data
	if rejectData != nil {
		data := rejectData.GetData()
		bs.WriteInt(U32(len(data)), 16)
		bs.WriteBits(U32(len(data))*8, data)
	} else {
		bs.WriteInt(0, 16)
	}
	
	return mcc.sendPacket(bs)
}

func (mcc *MasterClientConnection) SendUpdateServerStatus(gameType, missionType string, botCount, playerCount, maxPlayers, infoFlags U32) error {
	bs := NewResizableBitStream()
	
	bs.WriteInt(2, 8)  // RPC packet
	bs.WriteInt(6, 16) // UpdateServerStatus
	bs.WriteString(gameType, 255)
	bs.WriteString(missionType, 255)
	bs.WriteInt(botCount, 32)
	bs.WriteInt(playerCount, 32)
	bs.WriteInt(maxPlayers, 32)
	bs.WriteInt(infoFlags, 32)
	
	return mcc.sendPacket(bs)
}

// sendPacket sends a packet to the master server
func (mcc *MasterClientConnection) sendPacket(bs *BitStream) error {
	if mcc.client.socket == nil {
		return fmt.Errorf("no client socket available")
	}
	
	dataSize := bs.GetBytePosition()
	data := bs.GetBuffer()[:dataSize]
	
	return mcc.client.socket.SendTo(data, mcc.masterAddress)
}