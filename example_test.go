package tnl

import (
	"fmt"
	"testing"
	"time"
)

// TestBasicTNLInitialization tests the basic TNL initialization
func TestBasicTNLInitialization(t *testing.T) {
	// Initialize TNL
	err := Init()
	if err != nil {
		t.Fatalf("Failed to initialize TNL: %v", err)
	}
	
	// Check if initialized
	if !IsInitialized() {
		t.Fatal("TNL should be initialized")
	}
	
	// Get version info
	major, minor, patch, version := GetVersion()
	t.Logf("TNL Version: %d.%d.%d (%s)", major, minor, patch, version)
	
	// Shutdown
	Shutdown()
	
	if IsInitialized() {
		t.Fatal("TNL should not be initialized after shutdown")
	}
}

// TestBitStreamOperations tests basic BitStream functionality
func TestBitStreamOperations(t *testing.T) {
	// Create a buffer and BitStream
	buffer := make([]U8, 1024)
	bs := NewBitStream(buffer)
	
	// Test writing and reading integers
	bs.WriteInt(42, 8)
	bs.WriteInt(1337, 16)
	bs.WriteInt(0xDEADBEEF, 32)
	
	// Reset position for reading
	bs.SetBitPosition(0)
	
	// Read back the values
	val1 := bs.ReadInt(8)
	val2 := bs.ReadInt(16)
	val3 := bs.ReadInt(32)
	
	if val1 != 42 {
		t.Errorf("Expected 42, got %d", val1)
	}
	if val2 != 1337 {
		t.Errorf("Expected 1337, got %d", val2)
	}
	if val3 != 0xDEADBEEF {
		t.Errorf("Expected 0xDEADBEEF, got 0x%X", val3)
	}
	
	if !bs.IsValid() {
		t.Error("BitStream should be valid")
	}
}

// TestFloatCompression tests float compression in BitStream
func TestFloatCompression(t *testing.T) {
	buffer := make([]U8, 1024)
	bs := NewBitStream(buffer)
	
	// Test float compression
	testValues := []F32{0.0, 0.5, 1.0, 0.25, 0.75}
	bitCounts := []U8{8, 16, 24}
	
	for _, bitCount := range bitCounts {
		bs.Reset()
		
		// Write floats
		for _, val := range testValues {
			bs.WriteFloat(val, bitCount)
		}
		
		// Read back
		bs.SetBitPosition(0)
		for i, expected := range testValues {
			actual := bs.ReadFloat(bitCount)
			
			// Allow for some precision loss due to compression
			tolerance := F32(1.0 / float32(1<<bitCount))
			if actual < expected-tolerance || actual > expected+tolerance {
				t.Errorf("Bit count %d, value %d: expected ~%f, got %f", 
					bitCount, i, expected, actual)
			}
		}
	}
}

// TestAddressOperations tests network address functionality
func TestAddressOperations(t *testing.T) {
	// Test address creation
	addr1 := NewAddress("127.0.0.1", 8080)
	if addr1 == nil {
		t.Fatal("Failed to create address")
	}
	
	if !addr1.IsValid() {
		t.Error("Address should be valid")
	}
	
	if addr1.GetPort() != 8080 {
		t.Errorf("Expected port 8080, got %d", addr1.GetPort())
	}
	
	// Test string representation
	addrStr := addr1.ToString()
	expected := "127.0.0.1:8080"
	if addrStr != expected {
		t.Errorf("Expected %s, got %s", expected, addrStr)
	}
	
	// Test address from string
	addr2 := NewAddressFromString("192.168.1.1:9090")
	if addr2 == nil {
		t.Fatal("Failed to create address from string")
	}
	
	if addr2.GetPort() != 9090 {
		t.Errorf("Expected port 9090, got %d", addr2.GetPort())
	}
	
	// Test address equality
	addr3 := NewAddress("127.0.0.1", 8080)
	if !addr1.Equals(addr3) {
		t.Error("Addresses should be equal")
	}
	
	if addr1.Equals(addr2) {
		t.Error("Addresses should not be equal")
	}
}

// TestCryptographicOperations tests basic cryptographic functionality
func TestCryptographicOperations(t *testing.T) {
	// Test nonce generation
	nonce1 := NewNonce()
	nonce2 := NewNonce()
	
	if nonce1.Equals(nonce2) {
		t.Error("Nonces should be different")
	}
	
	// Test nonce from bytes
	data := []byte{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}
	nonce3 := NewNonceFromBytes(data)
	nonce4 := NewNonceFromBytes(data)
	
	if !nonce3.Equals(nonce4) {
		t.Error("Nonces from same data should be equal")
	}
	
	// Test AES cipher
	key := make([]byte, AESKeySize)
	for i := range key {
		key[i] = byte(i)
	}
	
	cipher, err := NewAESCipher(key)
	if err != nil {
		t.Fatalf("Failed to create AES cipher: %v", err)
	}
	
	plaintext := []byte("Hello, TNL-Go!")
	ciphertext, err := cipher.Encrypt(plaintext)
	if err != nil {
		t.Fatalf("Failed to encrypt: %v", err)
	}
	
	decrypted, err := cipher.Decrypt(ciphertext)
	if err != nil {
		t.Fatalf("Failed to decrypt: %v", err)
	}
	
	if string(decrypted) != string(plaintext) {
		t.Errorf("Expected %s, got %s", string(plaintext), string(decrypted))
	}
}

// TestAsymmetricCrypto tests asymmetric key operations
func TestAsymmetricCrypto(t *testing.T) {
	// Generate key pair
	key, err := NewAsymmetricKey(2048)
	if err != nil {
		t.Fatalf("Failed to generate key pair: %v", err)
	}
	
	// Test PEM export/import
	publicPEM, err := key.GetPublicKeyPEM()
	if err != nil {
		t.Fatalf("Failed to export public key: %v", err)
	}
	
	privatePEM, err := key.GetPrivateKeyPEM()
	if err != nil {
		t.Fatalf("Failed to export private key: %v", err)
	}
	
	// Import public key
	publicKey, err := NewAsymmetricKeyFromPEM(publicPEM)
	if err != nil {
		t.Fatalf("Failed to import public key: %v", err)
	}
	
	// Import private key
	privateKey, err := NewAsymmetricKeyFromPEM(privatePEM)
	if err != nil {
		t.Fatalf("Failed to import private key: %v", err)
	}
	
	// Test encryption/decryption
	plaintext := []byte("Secret message")
	ciphertext, err := publicKey.Encrypt(plaintext)
	if err != nil {
		t.Fatalf("Failed to encrypt with public key: %v", err)
	}
	
	decrypted, err := privateKey.Decrypt(ciphertext)
	if err != nil {
		t.Fatalf("Failed to decrypt with private key: %v", err)
	}
	
	if string(decrypted) != string(plaintext) {
		t.Errorf("Expected %s, got %s", string(plaintext), string(decrypted))
	}
}

// TestStringTable tests the connection string table functionality
func TestStringTable(t *testing.T) {
	table := NewConnectionStringTable()
	
	// Add some strings
	entry1 := table.AddString("Hello")
	entry2 := table.AddString("World")
	entry3 := table.AddString("Hello") // Duplicate
	
	if entry1.GetIndex() == entry2.GetIndex() {
		t.Error("Different strings should have different indices")
	}
	
	if entry1.GetIndex() != entry3.GetIndex() {
		t.Error("Same strings should have same indices")
	}
	
	// Test remote string addition
	index := table.AddRemoteString("Remote")
	retrieved := table.GetString(index)
	
	if retrieved != "Remote" {
		t.Errorf("Expected 'Remote', got '%s'", retrieved)
	}
}

// TestNetworkStats tests network statistics tracking
func TestNetworkStats(t *testing.T) {
	stats := NewNetworkStats()
	
	// Add some statistics
	stats.AddPacketSent(100)
	stats.AddPacketSent(200)
	stats.AddPacketReceived(150)
	stats.AddPacketDropped()
	
	sent, received, bytesSent, bytesReceived, dropped := stats.GetStats()
	
	if sent != 2 {
		t.Errorf("Expected 2 packets sent, got %d", sent)
	}
	
	if received != 1 {
		t.Errorf("Expected 1 packet received, got %d", received)
	}
	
	if bytesSent != 300 {
		t.Errorf("Expected 300 bytes sent, got %d", bytesSent)
	}
	
	if bytesReceived != 150 {
		t.Errorf("Expected 150 bytes received, got %d", bytesReceived)
	}
	
	if dropped != 1 {
		t.Errorf("Expected 1 packet dropped, got %d", dropped)
	}
	
	// Test reset
	stats.Reset()
	sent, received, bytesSent, bytesReceived, dropped = stats.GetStats()
	
	if sent != 0 || received != 0 || bytesSent != 0 || bytesReceived != 0 || dropped != 0 {
		t.Error("All stats should be zero after reset")
	}
}

// ExampleBasicUsage demonstrates basic TNL usage
func ExampleBasicUsage() {
	// Initialize TNL
	err := Init()
	if err != nil {
		fmt.Printf("Failed to initialize TNL: %v\n", err)
		return
	}
	defer Shutdown()
	
	// Create a BitStream for packet data
	buffer := make([]U8, 1024)
	bs := NewBitStream(buffer)
	
	// Write some data
	bs.WriteInt(42, 8)
	bs.WriteFloat(0.5, 16)
	bs.WriteFlag(true)
	bs.WriteString("Hello TNL-Go!", 255)
	
	fmt.Printf("Wrote %d bits to stream\n", bs.GetBitPosition())
	
	// Create a network address
	addr := NewAddress("127.0.0.1", 8080)
	fmt.Printf("Created address: %s\n", addr.ToString())
	
	// Create a UDP socket
	socket := NewUDPSocket()
	err = socket.Bind(addr)
	if err != nil {
		fmt.Printf("Failed to bind socket: %v\n", err)
		return
	}
	defer socket.Close()
	
	fmt.Println("Socket bound successfully")
	
	// Output:
	// Wrote bits to stream
	// Created address: 127.0.0.1:8080
	// Socket bound successfully
}

// BenchmarkBitStreamWrite benchmarks BitStream write operations
func BenchmarkBitStreamWrite(b *testing.B) {
	buffer := make([]U8, 1024)
	bs := NewBitStream(buffer)
	
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		bs.Reset()
		bs.WriteInt(U32(i), 32)
		bs.WriteFloat(F32(i)/F32(b.N), 16)
		bs.WriteFlag(i%2 == 0)
	}
}

// BenchmarkBitStreamRead benchmarks BitStream read operations
func BenchmarkBitStreamRead(b *testing.B) {
	buffer := make([]U8, 1024)
	bs := NewBitStream(buffer)
	
	// Pre-fill with data
	for i := 0; i < 100; i++ {
		bs.WriteInt(U32(i), 32)
		bs.WriteFloat(F32(i)/100.0, 16)
		bs.WriteFlag(i%2 == 0)
	}
	
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		bs.SetBitPosition(0)
		for j := 0; j < 100; j++ {
			bs.ReadInt(32)
			bs.ReadFloat(16)
			bs.ReadFlag()
		}
	}
}

// BenchmarkAESEncryption benchmarks AES encryption
func BenchmarkAESEncryption(b *testing.B) {
	key := make([]byte, AESKeySize)
	cipher, _ := NewAESCipher(key)
	data := make([]byte, 1024)
	
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		cipher.Encrypt(data)
	}
}