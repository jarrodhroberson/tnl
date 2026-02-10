# TNL-Go: Torque Network Library for Go

TNL-Go is a Go implementation of the Torque Network Library (TNL), originally developed by GarageGames for real-time multiplayer games. This library provides comprehensive networking capabilities designed for applications that require reliable, efficient, and secure network communication.

## Features

- **Bit-level Packet Compression**: Efficient BitStream implementation for minimal bandwidth usage
- **Reliable and Unreliable Messaging**: Flexible packet delivery guarantees
- **Encryption Support**: Built-in AES encryption and RSA key exchange
- **Object Replication**: Network object system for synchronized game state
- **Adaptive Bandwidth Management**: Dynamic rate control for optimal performance
- **Connection Management**: Robust connection handling with timeout and retry logic
- **String Compression**: Network string tables for efficient text transmission
- **Cross-platform**: Pure Go implementation works on all Go-supported platforms

## Installation

```bash
go get github.com/yourusername/tnl-go
```

## Quick Start

```go
package main

import (
    "fmt"
    "log"
    "tnl-go"
)

func main() {
    // Initialize TNL
    if err := tnl.Init(); err != nil {
        log.Fatal("Failed to initialize TNL:", err)
    }
    defer tnl.Shutdown()

    // Create a BitStream for packet data
    buffer := make([]tnl.U8, 1024)
    bs := tnl.NewBitStream(buffer)

    // Write compressed data
    bs.WriteInt(42, 8)           // 8-bit integer
    bs.WriteFloat(0.5, 16)       // 16-bit compressed float (0-1 range)
    bs.WriteFlag(true)           // Single bit boolean
    bs.WriteString("Hello!", 255) // Compressed string

    fmt.Printf("Wrote %d bits to stream\n", bs.GetBitPosition())

    // Create a network address
    addr := tnl.NewAddress("127.0.0.1", 8080)
    fmt.Printf("Created address: %s\n", addr.ToString())

    // Create and bind a UDP socket
    socket := tnl.NewUDPSocket()
    if err := socket.Bind(addr); err != nil {
        log.Fatal("Failed to bind socket:", err)
    }
    defer socket.Close()

    fmt.Println("Socket bound successfully")
}
```

## Core Components

### BitStream

The BitStream class provides bit-level read/write operations for maximum bandwidth efficiency:

```go
// Create a BitStream
buffer := make([]tnl.U8, 1024)
bs := tnl.NewBitStream(buffer)

// Write various data types with bit-level precision
bs.WriteInt(255, 8)              // 8-bit unsigned integer
bs.WriteSignedInt(-100, 16)      // 16-bit signed integer
bs.WriteFloat(0.75, 12)          // 12-bit float (0-1 range)
bs.WriteRangedU32(50, 0, 100)    // Integer in range 0-100
bs.WriteFlag(true)               // Single bit

// Read data back
bs.SetBitPosition(0)
val1 := bs.ReadInt(8)
val2 := bs.ReadSignedInt(16)
val3 := bs.ReadFloat(12)
val4 := bs.ReadRangedU32(0, 100)
flag := bs.ReadFlag()
```

### Network Addresses

```go
// Create addresses
addr1 := tnl.NewAddress("192.168.1.100", 8080)
addr2 := tnl.NewAddressFromString("10.0.0.1:9090")

// Address operations
fmt.Println(addr1.ToString())     // "192.168.1.100:8080"
fmt.Println(addr1.IsValid())      // true
fmt.Println(addr1.Equals(addr2))  // false
```

### UDP Sockets

```go
// Create and configure socket
socket := tnl.NewUDPSocket()
socket.SetBlocking(false)
socket.SetBroadcast(true)

// Bind to address
addr := tnl.NewAddress("0.0.0.0", 8080)
if err := socket.Bind(addr); err != nil {
    log.Fatal(err)
}

// Send data
data := []byte("Hello, World!")
targetAddr := tnl.NewAddress("127.0.0.1", 8080)
socket.SendTo(data, targetAddr)

// Receive data
buffer := make([]byte, 1024)
n, fromAddr, err := socket.RecvFrom(buffer)
if err != nil {
    log.Printf("Receive error: %v", err)
} else {
    fmt.Printf("Received %d bytes from %s: %s\n", 
        n, fromAddr.ToString(), string(buffer[:n]))
}
```

### Encryption

```go
// AES Symmetric Encryption
key := make([]byte, tnl.AESKeySize)
// ... fill key with random data ...
cipher, err := tnl.NewAESCipher(key)
if err != nil {
    log.Fatal(err)
}

plaintext := []byte("Secret message")
ciphertext, err := cipher.Encrypt(plaintext)
if err != nil {
    log.Fatal(err)
}

decrypted, err := cipher.Decrypt(ciphertext)
if err != nil {
    log.Fatal(err)
}

// RSA Asymmetric Encryption
keyPair, err := tnl.NewAsymmetricKey(2048)
if err != nil {
    log.Fatal(err)
}

// Export/Import keys
publicPEM, _ := keyPair.GetPublicKeyPEM()
privatePEM, _ := keyPair.GetPrivateKeyPEM()

// Encrypt with public key, decrypt with private key
encrypted, _ := keyPair.Encrypt(plaintext)
decrypted, _ := keyPair.Decrypt(encrypted)
```

### String Tables

Network string tables provide efficient compression for frequently used strings:

```go
table := tnl.NewConnectionStringTable()

// Add strings to table
entry1 := table.AddString("PlayerName")
entry2 := table.AddString("GameMode")

// Strings are automatically assigned indices for network transmission
fmt.Printf("PlayerName index: %d\n", entry1.GetIndex())

// Retrieve strings by index
retrieved := table.GetString(entry1.GetIndex())
fmt.Printf("Retrieved: %s\n", retrieved)
```

## Architecture

TNL-Go maintains the core architecture of the original TNL while leveraging Go's strengths:

- **Goroutine-safe**: All components are designed for concurrent use
- **Interface-based**: Flexible design using Go interfaces
- **Memory-efficient**: Careful buffer management and object pooling
- **Error handling**: Proper Go error handling throughout
- **Testing**: Comprehensive test suite with benchmarks

## Differences from Original TNL

While maintaining compatibility with TNL concepts, this Go implementation includes some adaptations:

1. **Memory Management**: Uses Go's garbage collector instead of manual memory management
2. **Concurrency**: Leverages goroutines and channels instead of threads
3. **Error Handling**: Uses Go's error interface instead of exceptions
4. **Networking**: Built on Go's standard `net` package
5. **Type System**: Uses Go's type system with generics for type safety

## Performance

TNL-Go is designed for high performance with:

- Bit-level packing for minimal bandwidth usage
- Zero-copy operations where possible
- Efficient buffer reuse
- Optimized cryptographic operations
- Minimal allocations in hot paths

## Testing

Run the test suite:

```bash
go test -v
```

Run benchmarks:

```bash
go test -bench=.
```

## Contributing

Contributions are welcome! Please ensure:

1. All tests pass
2. Code follows Go conventions
3. New features include tests
4. Documentation is updated

## License

This project is licensed under the same terms as the original TNL - see the LICENSE file for details.

## Acknowledgments

- Original TNL developers at GarageGames
- The Go team for excellent networking and crypto libraries
- Contributors to the Go ecosystem

## Status

This is a work-in-progress implementation. Current status:

- ✅ Core types and utilities
- ✅ BitStream implementation
- ✅ Basic networking (UDP sockets)
- ✅ Cryptographic primitives
- ✅ String tables
- 🚧 Connection management (in progress)
- 🚧 Object replication system (planned)
- 🚧 Event system (planned)
- 🚧 Master server interface (planned)

## Examples

See the `examples/` directory for more comprehensive examples including:

- Simple client/server communication
- Encrypted connections
- Object replication
- Performance testing

For more information about the original TNL concepts and architecture, visit: http://www.opentnl.org