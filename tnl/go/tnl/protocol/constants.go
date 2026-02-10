package protocol

// Packet Header Constants
const (
	MaxPacketDataSize           = 1500
	MaxPacketWindowSizeShift    = 5
	MaxPacketWindowSize         = 1 << MaxPacketWindowSizeShift
	PacketWindowMask            = MaxPacketWindowSize - 1
	MaxAckMaskSize              = 1 << (MaxPacketWindowSizeShift - 5)
	MaxAckByteCount             = MaxAckMaskSize << 2
	SequenceNumberBitSize       = 11
	SequenceNumberWindowSize    = 1 << SequenceNumberBitSize
	SequenceNumberMask          = uint32(0xFFFFFFFF) ^ uint32(SequenceNumberWindowSize-1)
	AckSequenceNumberBitSize    = 10
	AckSequenceNumberWindowSize = 1 << AckSequenceNumberBitSize
	AckSequenceNumberMask       = uint32(0xFFFFFFFF) ^ uint32(AckSequenceNumberWindowSize-1)

	PacketHeaderBitSize  = 3 + AckSequenceNumberBitSize + SequenceNumberBitSize
	PacketHeaderByteSize = (PacketHeaderBitSize + 7) >> 3
	PacketHeaderPadBits  = (PacketHeaderByteSize << 3) - PacketHeaderBitSize

	MessageSignatureBytes = 5
	MinimumPaddingBits    = 128
)

// NetClassType defines the ranges of individual class identifiers
type NetClassType int32

const (
	NetClassTypeNone      NetClassType = -1
	NetClassTypeObject    NetClassType = 0
	NetClassTypeDataBlock NetClassType = 1
	NetClassTypeEvent     NetClassType = 2
	NetClassTypeCount     NetClassType = 3
)

// NetClassGroup defines different service types
type NetClassGroup uint32

const (
	NetClassGroupGame      NetClassGroup = 0
	NetClassGroupCommunity NetClassGroup = 1
	NetClassGroupMaster    NetClassGroup = 2
	NetClassGroupCount     NetClassGroup = 3
	NetClassGroupInvalid   NetClassGroup = NetClassGroupCount
)

// NetClassMask values
const (
	NetClassGroupGameMask      = 1 << NetClassGroupGame
	NetClassGroupCommunityMask = 1 << NetClassGroupCommunity
	NetClassGroupMasterMask    = 1 << NetClassGroupMaster
	NetClassGroupAllMask       = (1 << NetClassGroupCount) - 1
)

// PacketType for non-connected data
type PacketType uint8

const (
	ConnectChallengeRequest  PacketType = 0
	ConnectChallengeResponse PacketType = 1
	ConnectRequest           PacketType = 2
	ConnectReject            PacketType = 3
	ConnectAccept            PacketType = 4
	Disconnect               PacketType = 5
	Punch                    PacketType = 6
	ArrangedConnectRequest   PacketType = 7
	FirstValidInfoPacketId   PacketType = 8
)

// NetPacketType for connected protocol
type NetPacketType uint8

const (
	DataPacket        NetPacketType = 0
	PingPacket        NetPacketType = 1
	AckPacket         NetPacketType = 2
	InvalidPacketType NetPacketType = 3
)

// TerminationReason for connection end
type TerminationReason int

const (
	ReasonTimedOut TerminationReason = iota
	ReasonFailedConnectHandshake
	ReasonRemoteHostRejectedConnection
	ReasonRemoteDisconnectPacket
	ReasonDuplicateConnectionAttempt
	ReasonSelfDisconnect
	ReasonError
)

// NetConnectionState for handshaking
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
)
