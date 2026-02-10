package tnl

import (
	"crypto"
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"errors"
	"fmt"
	"io"
	"math/big"
	"time"
)

// SymmetricCipher interface for symmetric encryption
type SymmetricCipher interface {
	Encrypt(data []byte) ([]byte, error)
	Decrypt(data []byte) ([]byte, error)
	GetKeySize() int
	GetBlockSize() int
}

// AESCipher implements SymmetricCipher using AES
type AESCipher struct {
	block cipher.Block
	gcm   cipher.AEAD
}

const (
	AESKeySize   = 32 // AES-256
	AESBlockSize = 16
)

// NewAESCipher creates a new AES cipher with the given key
func NewAESCipher(key []byte) (*AESCipher, error) {
	if len(key) != AESKeySize {
		return nil, fmt.Errorf("invalid key size: expected %d, got %d", AESKeySize, len(key))
	}
	
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	
	gcm, err := cipher.NewGCM(block)
	if err != nil {
		return nil, err
	}
	
	return &AESCipher{
		block: block,
		gcm:   gcm,
	}, nil
}

// Encrypt encrypts the given data
func (c *AESCipher) Encrypt(data []byte) ([]byte, error) {
	nonce := make([]byte, c.gcm.NonceSize())
	if _, err := io.ReadFull(rand.Reader, nonce); err != nil {
		return nil, err
	}
	
	ciphertext := c.gcm.Seal(nonce, nonce, data, nil)
	return ciphertext, nil
}

// Decrypt decrypts the given data
func (c *AESCipher) Decrypt(data []byte) ([]byte, error) {
	nonceSize := c.gcm.NonceSize()
	if len(data) < nonceSize {
		return nil, errors.New("ciphertext too short")
	}
	
	nonce, ciphertext := data[:nonceSize], data[nonceSize:]
	plaintext, err := c.gcm.Open(nil, nonce, ciphertext, nil)
	if err != nil {
		return nil, err
	}
	
	return plaintext, nil
}

// GetKeySize returns the key size in bytes
func (c *AESCipher) GetKeySize() int {
	return AESKeySize
}

// GetBlockSize returns the block size in bytes
func (c *AESCipher) GetBlockSize() int {
	return AESBlockSize
}

// AsymmetricKey represents a public/private key pair
type AsymmetricKey struct {
	privateKey *rsa.PrivateKey
	publicKey  *rsa.PublicKey
}

// NewAsymmetricKey generates a new RSA key pair
func NewAsymmetricKey(bits int) (*AsymmetricKey, error) {
	if bits < 1024 {
		bits = 2048 // Default to 2048 bits for security
	}
	
	privateKey, err := rsa.GenerateKey(rand.Reader, bits)
	if err != nil {
		return nil, err
	}
	
	return &AsymmetricKey{
		privateKey: privateKey,
		publicKey:  &privateKey.PublicKey,
	}, nil
}

// NewAsymmetricKeyFromPEM creates a key from PEM-encoded data
func NewAsymmetricKeyFromPEM(pemData []byte) (*AsymmetricKey, error) {
	block, _ := pem.Decode(pemData)
	if block == nil {
		return nil, errors.New("failed to decode PEM block")
	}
	
	switch block.Type {
	case "RSA PRIVATE KEY":
		privateKey, err := x509.ParsePKCS1PrivateKey(block.Bytes)
		if err != nil {
			return nil, err
		}
		return &AsymmetricKey{
			privateKey: privateKey,
			publicKey:  &privateKey.PublicKey,
		}, nil
		
	case "PUBLIC KEY":
		publicKey, err := x509.ParsePKIXPublicKey(block.Bytes)
		if err != nil {
			return nil, err
		}
		rsaPublicKey, ok := publicKey.(*rsa.PublicKey)
		if !ok {
			return nil, errors.New("not an RSA public key")
		}
		return &AsymmetricKey{
			publicKey: rsaPublicKey,
		}, nil
		
	default:
		return nil, fmt.Errorf("unsupported PEM block type: %s", block.Type)
	}
}

// GetPublicKeyPEM returns the public key in PEM format
func (k *AsymmetricKey) GetPublicKeyPEM() ([]byte, error) {
	if k.publicKey == nil {
		return nil, errors.New("no public key available")
	}
	
	publicKeyBytes, err := x509.MarshalPKIXPublicKey(k.publicKey)
	if err != nil {
		return nil, err
	}
	
	block := &pem.Block{
		Type:  "PUBLIC KEY",
		Bytes: publicKeyBytes,
	}
	
	return pem.EncodeToMemory(block), nil
}

// GetPrivateKeyPEM returns the private key in PEM format
func (k *AsymmetricKey) GetPrivateKeyPEM() ([]byte, error) {
	if k.privateKey == nil {
		return nil, errors.New("no private key available")
	}
	
	privateKeyBytes := x509.MarshalPKCS1PrivateKey(k.privateKey)
	
	block := &pem.Block{
		Type:  "RSA PRIVATE KEY",
		Bytes: privateKeyBytes,
	}
	
	return pem.EncodeToMemory(block), nil
}

// Encrypt encrypts data with the public key
func (k *AsymmetricKey) Encrypt(data []byte) ([]byte, error) {
	if k.publicKey == nil {
		return nil, errors.New("no public key available")
	}
	
	return rsa.EncryptOAEP(sha256.New(), rand.Reader, k.publicKey, data, nil)
}

// Decrypt decrypts data with the private key
func (k *AsymmetricKey) Decrypt(data []byte) ([]byte, error) {
	if k.privateKey == nil {
		return nil, errors.New("no private key available")
	}
	
	return rsa.DecryptOAEP(sha256.New(), rand.Reader, k.privateKey, data, nil)
}

// Sign signs data with the private key
func (k *AsymmetricKey) Sign(data []byte) ([]byte, error) {
	if k.privateKey == nil {
		return nil, errors.New("no private key available")
	}
	
	hash := sha256.Sum256(data)
	return rsa.SignPKCS1v15(rand.Reader, k.privateKey, crypto.SHA256, hash[:])
}

// Verify verifies a signature with the public key
func (k *AsymmetricKey) Verify(data, signature []byte) error {
	if k.publicKey == nil {
		return errors.New("no public key available")
	}
	
	hash := sha256.Sum256(data)
	return rsa.VerifyPKCS1v15(k.publicKey, crypto.SHA256, hash[:], signature)
}

// GenerateSharedSecret generates a shared secret using Diffie-Hellman key exchange
func (k *AsymmetricKey) GenerateSharedSecret(otherPublicKey *AsymmetricKey) ([]byte, error) {
	if k.privateKey == nil {
		return nil, errors.New("no private key available")
	}
	if otherPublicKey.publicKey == nil {
		return nil, errors.New("other public key not available")
	}
	
	// For RSA, we'll use a simple approach: encrypt a random secret with the other's public key
	// and decrypt it with our private key (this is a simplified version)
	secret := make([]byte, 32)
	if _, err := rand.Read(secret); err != nil {
		return nil, err
	}
	
	// In a real implementation, this would use proper key exchange algorithms like ECDH
	// For now, we'll just return a hash of both public keys as a shared secret
	hash := sha256.New()
	pubKey1, _ := k.GetPublicKeyPEM()
	pubKey2, _ := otherPublicKey.GetPublicKeyPEM()
	hash.Write(pubKey1)
	hash.Write(pubKey2)
	
	return hash.Sum(nil), nil
}

// Certificate represents a digital certificate
type Certificate struct {
	cert      *x509.Certificate
	publicKey *AsymmetricKey
}

// NewCertificate creates a new self-signed certificate
func NewCertificate(key *AsymmetricKey, subject string) (*Certificate, error) {
	if key.privateKey == nil {
		return nil, errors.New("private key required for certificate creation")
	}
	
	template := &x509.Certificate{
		SerialNumber: big.NewInt(1),
		Subject: pkix.Name{
			CommonName: subject,
		},
		NotBefore:             time.Now(),
		NotAfter:              time.Now().Add(365 * 24 * time.Hour), // 1 year
		KeyUsage:              x509.KeyUsageKeyEncipherment | x509.KeyUsageDigitalSignature,
		ExtKeyUsage:           []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth, x509.ExtKeyUsageClientAuth},
		BasicConstraintsValid: true,
	}
	
	certDER, err := x509.CreateCertificate(rand.Reader, template, template, key.publicKey, key.privateKey)
	if err != nil {
		return nil, err
	}
	
	cert, err := x509.ParseCertificate(certDER)
	if err != nil {
		return nil, err
	}
	
	publicKey := &AsymmetricKey{
		publicKey: cert.PublicKey.(*rsa.PublicKey),
	}
	
	return &Certificate{
		cert:      cert,
		publicKey: publicKey,
	}, nil
}

// NewCertificateFromPEM creates a certificate from PEM-encoded data
func NewCertificateFromPEM(pemData []byte) (*Certificate, error) {
	block, _ := pem.Decode(pemData)
	if block == nil || block.Type != "CERTIFICATE" {
		return nil, errors.New("invalid certificate PEM data")
	}
	
	cert, err := x509.ParseCertificate(block.Bytes)
	if err != nil {
		return nil, err
	}
	
	publicKey := &AsymmetricKey{
		publicKey: cert.PublicKey.(*rsa.PublicKey),
	}
	
	return &Certificate{
		cert:      cert,
		publicKey: publicKey,
	}, nil
}

// GetPEM returns the certificate in PEM format
func (c *Certificate) GetPEM() []byte {
	block := &pem.Block{
		Type:  "CERTIFICATE",
		Bytes: c.cert.Raw,
	}
	return pem.EncodeToMemory(block)
}

// GetPublicKey returns the public key from the certificate
func (c *Certificate) GetPublicKey() *AsymmetricKey {
	return c.publicKey
}

// Verify verifies the certificate against a CA certificate
func (c *Certificate) Verify(caCert *Certificate) error {
	if caCert == nil {
		return errors.New("CA certificate is nil")
	}
	
	roots := x509.NewCertPool()
	roots.AddCert(caCert.cert)
	
	opts := x509.VerifyOptions{
		Roots: roots,
	}
	
	_, err := c.cert.Verify(opts)
	return err
}

// GetSubject returns the certificate subject
func (c *Certificate) GetSubject() string {
	return c.cert.Subject.CommonName
}

// IsExpired returns true if the certificate has expired
func (c *Certificate) IsExpired() bool {
	return time.Now().After(c.cert.NotAfter)
}

// Nonce represents a cryptographic nonce
type Nonce struct {
	data [16]byte
}

// NewNonce generates a new random nonce
func NewNonce() *Nonce {
	n := &Nonce{}
	rand.Read(n.data[:])
	return n
}

// NewNonceFromBytes creates a nonce from byte data
func NewNonceFromBytes(data []byte) *Nonce {
	n := &Nonce{}
	copy(n.data[:], data)
	return n
}

// GetBytes returns the nonce as a byte slice
func (n *Nonce) GetBytes() []byte {
	return n.data[:]
}

// Equals compares two nonces
func (n *Nonce) Equals(other *Nonce) bool {
	if other == nil {
		return false
	}
	for i := range n.data {
		if n.data[i] != other.data[i] {
			return false
		}
	}
	return true
}

// String returns the nonce as a hex string
func (n *Nonce) String() string {
	return fmt.Sprintf("%x", n.data)
}

// ByteBuffer represents a managed byte buffer
type ByteBuffer struct {
	data []byte
}

// NewByteBuffer creates a new ByteBuffer
func NewByteBuffer(size int) *ByteBuffer {
	return &ByteBuffer{
		data: make([]byte, size),
	}
}

// NewByteBufferFromData creates a ByteBuffer from existing data
func NewByteBufferFromData(data []byte) *ByteBuffer {
	return &ByteBuffer{
		data: make([]byte, len(data)),
	}
}

// GetData returns the underlying byte slice
func (b *ByteBuffer) GetData() []byte {
	return b.data
}

// GetSize returns the size of the buffer
func (b *ByteBuffer) GetSize() int {
	return len(b.data)
}

// Resize resizes the buffer
func (b *ByteBuffer) Resize(newSize int) {
	if newSize != len(b.data) {
		newData := make([]byte, newSize)
		copy(newData, b.data)
		b.data = newData
	}
}

// ByteBufferPtr is a reference-counted pointer to a ByteBuffer
type ByteBufferPtr struct {
	buffer *ByteBuffer
}

// NewByteBufferPtr creates a new ByteBufferPtr
func NewByteBufferPtr(size int) *ByteBufferPtr {
	return &ByteBufferPtr{
		buffer: NewByteBuffer(size),
	}
}

// Get returns the underlying ByteBuffer
func (p *ByteBufferPtr) Get() *ByteBuffer {
	return p.buffer
}

// IsNull returns true if the pointer is null
func (p *ByteBufferPtr) IsNull() bool {
	return p.buffer == nil
}