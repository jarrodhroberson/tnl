// Package tnl provides a Go implementation of the Torque Network Library
// 
// TNL is a comprehensive networking library designed for real-time multiplayer games
// and applications. It provides features like:
// - Reliable and unreliable packet transmission
// - Object replication and ghosting
// - Encryption and security
// - Adaptive bandwidth management
// - Connection management
//
// This Go implementation maintains the core architecture and concepts of the original
// C++ TNL while leveraging Go's strengths like goroutines, channels, and built-in
// networking support.
package tnl

import (
	"fmt"
	"log"
	"sync"
)

// Version information
const (
	TNLVersionMajor = 1
	TNLVersionMinor = 0
	TNLVersionPatch = 0
	TNLVersionString = "1.0.0-go"
)

// Global initialization state
var (
	tnlInitialized bool
	initMutex      sync.Mutex
)

// Init initializes the TNL library
// This must be called before using any TNL functionality
func Init() error {
	initMutex.Lock()
	defer initMutex.Unlock()
	
	if tnlInitialized {
		return nil
	}
	
	log.Printf("Initializing TNL-Go version %s", TNLVersionString)
	
	// Initialize the class system
	Initialize()
	
	// Register built-in classes
	registerBuiltinClasses()
	
	tnlInitialized = true
	log.Println("TNL-Go initialization complete")
	
	return nil
}

// Shutdown shuts down the TNL library
func Shutdown() {
	initMutex.Lock()
	defer initMutex.Unlock()
	
	if !tnlInitialized {
		return
	}
	
	log.Println("Shutting down TNL-Go")
	
	// Clean up resources
	tnlInitialized = false
	
	log.Println("TNL-Go shutdown complete")
}

// IsInitialized returns true if TNL has been initialized
func IsInitialized() bool {
	initMutex.Lock()
	defer initMutex.Unlock()
	return tnlInitialized
}

// GetVersion returns the TNL version information
func GetVersion() (major, minor, patch int, versionString string) {
	return TNLVersionMajor, TNLVersionMinor, TNLVersionPatch, TNLVersionString
}

// registerBuiltinClasses registers the built-in TNL classes
func registerBuiltinClasses() {
	// Register base object class
	RegisterClass("Object", NetClassGroupAllMask, NetClassTypeObject, 1, func() *BaseObject {
		return NewBaseObject()
	})
	
	// Additional built-in classes would be registered here
	// For example: NetConnection, NetEvent, etc.
}

// LogLevel represents different logging levels
type LogLevel int

const (
	LogLevelError LogLevel = iota
	LogLevelWarning
	LogLevelInfo
	LogLevelDebug
)

// Logger interface for TNL logging
type Logger interface {
	Log(level LogLevel, format string, args ...interface{})
}

// DefaultLogger provides a simple console logger
type DefaultLogger struct{}

// Log implements the Logger interface
func (l *DefaultLogger) Log(level LogLevel, format string, args ...interface{}) {
	var prefix string
	switch level {
	case LogLevelError:
		prefix = "[ERROR]"
	case LogLevelWarning:
		prefix = "[WARN] "
	case LogLevelInfo:
		prefix = "[INFO] "
	case LogLevelDebug:
		prefix = "[DEBUG]"
	}
	
	message := fmt.Sprintf(format, args...)
	log.Printf("%s %s", prefix, message)
}

// Global logger instance
var globalLogger Logger = &DefaultLogger{}

// SetLogger sets the global logger
func SetLogger(logger Logger) {
	if logger != nil {
		globalLogger = logger
	}
}

// Log functions for different levels
func LogError(format string, args ...interface{}) {
	globalLogger.Log(LogLevelError, format, args...)
}

func LogWarning(format string, args ...interface{}) {
	globalLogger.Log(LogLevelWarning, format, args...)
}

func LogInfo(format string, args ...interface{}) {
	globalLogger.Log(LogLevelInfo, format, args...)
}

func LogDebug(format string, args ...interface{}) {
	globalLogger.Log(LogLevelDebug, format, args...)
}

// Assert provides assertion functionality similar to TNL's assert system
func Assert(condition bool, message string) {
	if !condition {
		LogError("Assertion failed: %s", message)
		panic(fmt.Sprintf("TNL Assertion failed: %s", message))
	}
}

// AssertFatal provides fatal assertion functionality
func AssertFatal(condition bool, message string) {
	if !condition {
		LogError("Fatal assertion failed: %s", message)
		panic(fmt.Sprintf("TNL Fatal assertion failed: %s", message))
	}
}

// Platform provides platform-specific functionality
type Platform struct{}

// GetRealMilliseconds returns the current time in milliseconds since epoch
func (p *Platform) GetRealMilliseconds() U32 {
	return GetRealMilliseconds()
}

// Sleep pauses execution for the specified number of milliseconds
func (p *Platform) Sleep(milliseconds U32) {
	Sleep(milliseconds)
}

// Global platform instance
var GlobalPlatform = &Platform{}

// Endian conversion functions (Go handles this automatically, but we provide
// the interface for compatibility)

// ConvertHostToLEndian converts host byte order to little endian
func ConvertHostToLEndian[T any](value T) T {
	// Go's binary package handles endianness automatically
	// This is a no-op for compatibility
	return value
}

// ConvertLEndianToHost converts little endian to host byte order
func ConvertLEndianToHost[T any](value T) T {
	// Go's binary package handles endianness automatically
	// This is a no-op for compatibility
	return value
}

// Error handling utilities

// TNLError represents a TNL-specific error
type TNLError struct {
	Code    int
	Message string
}

// Error implements the error interface
func (e *TNLError) Error() string {
	return fmt.Sprintf("TNL Error %d: %s", e.Code, e.Message)
}

// NewTNLError creates a new TNL error
func NewTNLError(code int, message string) *TNLError {
	return &TNLError{
		Code:    code,
		Message: message,
	}
}

// Common TNL error codes
const (
	ErrorNone = iota
	ErrorInvalidParameter
	ErrorNotInitialized
	ErrorAlreadyInitialized
	ErrorNetworkError
	ErrorEncryptionError
	ErrorInvalidPacket
	ErrorConnectionFailed
	ErrorTimeout
)

// Utility functions for common operations

// Min returns the minimum of two values
func Min[T comparable](a, b T) T {
	if a < b {
		return a
	}
	return b
}

// Max returns the maximum of two values
func Max[T comparable](a, b T) T {
	if a > b {
		return a
	}
	return b
}

// Clamp clamps a value between min and max
func Clamp[T comparable](value, min, max T) T {
	if value < min {
		return min
	}
	if value > max {
		return max
	}
	return value
}

// Statistics tracking for network performance
type NetworkStats struct {
	PacketsSent     U64
	PacketsReceived U64
	BytesSent       U64
	BytesReceived   U64
	PacketsDropped  U64
	mutex           sync.RWMutex
}

// NewNetworkStats creates a new NetworkStats instance
func NewNetworkStats() *NetworkStats {
	return &NetworkStats{}
}

// AddPacketSent increments the packets sent counter
func (ns *NetworkStats) AddPacketSent(bytes U32) {
	ns.mutex.Lock()
	defer ns.mutex.Unlock()
	ns.PacketsSent++
	ns.BytesSent += U64(bytes)
}

// AddPacketReceived increments the packets received counter
func (ns *NetworkStats) AddPacketReceived(bytes U32) {
	ns.mutex.Lock()
	defer ns.mutex.Unlock()
	ns.PacketsReceived++
	ns.BytesReceived += U64(bytes)
}

// AddPacketDropped increments the packets dropped counter
func (ns *NetworkStats) AddPacketDropped() {
	ns.mutex.Lock()
	defer ns.mutex.Unlock()
	ns.PacketsDropped++
}

// GetStats returns a copy of the current statistics
func (ns *NetworkStats) GetStats() (sent, received, bytesSent, bytesReceived, dropped U64) {
	ns.mutex.RLock()
	defer ns.mutex.RUnlock()
	return ns.PacketsSent, ns.PacketsReceived, ns.BytesSent, ns.BytesReceived, ns.PacketsDropped
}

// Reset resets all statistics to zero
func (ns *NetworkStats) Reset() {
	ns.mutex.Lock()
	defer ns.mutex.Unlock()
	ns.PacketsSent = 0
	ns.PacketsReceived = 0
	ns.BytesSent = 0
	ns.BytesReceived = 0
	ns.PacketsDropped = 0
}

// Global network statistics
var GlobalNetworkStats = NewNetworkStats()