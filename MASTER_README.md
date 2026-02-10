# TNL-Go Master Server

This directory contains the Go implementation of the TNL Master Server and Master Client, converted from the original C++ implementation.

## Overview

The TNL Master Server provides matchmaking services for multiplayer games. It maintains a list of active game servers and allows clients to:

- Query for available servers based on filter criteria
- Request arranged connections to servers behind firewalls/NAT
- Receive server lists and game type information

## Components

### Master Server (`master_server.go`)
The main master server implementation that:
- Listens for connections from game servers and clients
- Maintains a registry of active game servers
- Handles server queries and filtering
- Facilitates arranged connections between clients and servers
- Manages Message of the Day (MOTD) delivery

### Master Client (`master_client.go`)
A client library that can operate in two modes:
- **Client Mode**: Queries master server for game servers and requests connections
- **Server Mode**: Registers with master server and accepts arranged connections

### Master Interface (`master_interface.go`)
Defines the RPC interface and data structures used for master server communication.

## Building

### Master Server Executable
```bash
go build -o master ./cmd/master
```

### Master Client Executable  
```bash
go build -o masterclient ./cmd/masterclient
```

## Usage

### Running the Master Server

```bash
# Use default configuration
./master

# Specify custom config file
./master -config /path/to/master.cfg

# Override port
./master -port 29006

# Show help
./master -help
```

### Configuration File

The master server uses a simple text configuration format:

```
# master.cfg
port 29005
motd * "Welcome to TNL-Go Master Server!"
motd TestGame "Welcome to the test game!"
```

Configuration options:
- `port <number>` - Set the listening port (default: 29005)
- `motd <game_type> "<message>"` - Set Message of the Day for specific game types

### Running the Master Client

#### As a Game Client
```bash
# Connect to master server and query for game servers
./masterclient -master localhost:29005

# Use custom game identifier
./masterclient -master localhost:29005 -game "MyAwesomeGame"
```

#### As a Game Server
```bash
# Register as a game server
./masterclient -master localhost:29005 -server -port 28000

# Register with custom game type
./masterclient -master localhost:29005 -server -port 28000 -game "MyAwesomeGame"
```

## Protocol

The master server uses a custom UDP-based protocol with the following RPC methods:

### Client to Master (C2M)
- `QueryGameTypes` - Request list of available game and mission types
- `QueryServers` - Request list of servers matching filter criteria  
- `RequestArrangedConnection` - Request connection to a specific server
- `AcceptArrangedConnection` - Accept an arranged connection request (server)
- `RejectArrangedConnection` - Reject an arranged connection request (server)
- `UpdateServerStatus` - Update server status information (server)

### Master to Client (M2C)
- `QueryGameTypesResponse` - Response with game and mission types
- `QueryServersResponse` - Response with matching server list
- `ClientRequestedArrangedConnection` - Notify server of connection request
- `ArrangedConnectionAccepted` - Notify client that connection was accepted
- `ArrangedConnectionRejected` - Notify client that connection was rejected
- `SetMOTD` - Send Message of the Day to client

## Features

### Server Registration
Game servers register with the master by connecting and providing:
- Game identifier string
- CPU speed and region code
- Current player/bot counts and maximum players
- Game type and mission type
- Server capability flags

### Server Filtering
Clients can filter servers by:
- Region mask (geographical regions)
- Player count ranges (min/max players)
- Bot count limits
- CPU speed requirements
- Game and mission types
- Server capability flags

### Arranged Connections
The master server facilitates connections between clients and servers behind firewalls/NAT by:
1. Client requests connection to specific server
2. Master forwards request to server with client's possible addresses
3. Server accepts/rejects and provides its possible addresses
4. Master coordinates the connection attempt between both parties

### DoS Protection
The master server includes protection against Denial of Service attacks:
- Rate limiting on requests (strike system)
- Connection request timeouts
- Activity time validation

## Example Usage

### Simple Test Scenario

1. Start the master server:
```bash
./master
```

2. Start a game server:
```bash
./masterclient -master localhost:29005 -server -port 28000
```

3. Start a client:
```bash
./masterclient -master localhost:29005
```

The client will automatically:
- Connect to the master server
- Query for available game types
- Query for available servers
- Attempt to connect to a random server
- Handle connection acceptance/rejection

### Integration with Games

To integrate the master server with your game:

1. **For Game Servers:**
```go
// Create master client in server mode
masterAddr := tnl.NewAddress("master.yourgame.com", 29005)
client := tnl.NewMasterClient(masterAddr, true, "YourGameName")

// Start and register with master
client.Start(28000)
client.StartPeriodicUpdates()

// Update server status as needed
client.UpdateServerStatus()
```

2. **For Game Clients:**
```go
// Create master client in client mode  
masterAddr := tnl.NewAddress("master.yourgame.com", 29005)
client := tnl.NewMasterClient(masterAddr, false, "YourGameName")

// Start and query for servers
client.Start(0)

// The client will automatically handle server discovery
// and arranged connection requests
```

## Testing

Run the master server tests:
```bash
go test -v -run TestMaster
```

Run integration tests:
```bash
go test -v -run TestMasterServerIntegration
```

Run benchmarks:
```bash
go test -bench=BenchmarkMaster
```

## Differences from Original TNL

While maintaining protocol compatibility, the Go implementation includes some improvements:

1. **Concurrency**: Uses goroutines instead of threads for better performance
2. **Memory Management**: Automatic garbage collection instead of manual memory management  
3. **Error Handling**: Proper Go error handling instead of exceptions
4. **Configuration**: Simplified configuration file parsing
5. **Logging**: Structured logging with configurable levels
6. **Testing**: Comprehensive test suite with benchmarks

## Performance

The Go implementation is designed for high performance:
- Concurrent packet processing using goroutines
- Efficient server filtering with optimized data structures
- Minimal memory allocations in hot paths
- Built-in connection pooling and reuse

Typical performance characteristics:
- Handles 1000+ concurrent connections
- Sub-millisecond query response times
- Minimal CPU usage during idle periods
- Automatic cleanup of expired connections and requests

## Security Considerations

The master server includes several security features:
- DoS protection with rate limiting
- Input validation on all RPC parameters
- Connection timeout management
- No sensitive data storage (stateless design)

For production deployment, consider:
- Running behind a firewall with only necessary ports open
- Implementing additional rate limiting at the network level
- Regular monitoring of connection patterns
- Keeping the master server software updated

## Troubleshooting

### Common Issues

1. **Port already in use**
   - Change the port in configuration or command line
   - Check for other processes using the port

2. **Connection refused**
   - Verify master server is running
   - Check firewall settings
   - Ensure correct address and port

3. **No servers found**
   - Verify game servers are registered with master
   - Check game string matches between client and server
   - Review server filter criteria

### Debug Logging

Enable debug logging to troubleshoot issues:
```go
tnl.SetLogger(&tnl.DefaultLogger{})
```

This will provide detailed information about:
- Connection attempts and results
- RPC message processing
- Server registration and updates
- Arranged connection negotiations

## Contributing

When contributing to the master server implementation:

1. Maintain protocol compatibility with original TNL
2. Add comprehensive tests for new features
3. Update documentation for any API changes
4. Follow Go coding conventions and best practices
5. Ensure thread safety for all concurrent operations