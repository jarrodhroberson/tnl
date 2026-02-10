package crypto

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/ecdh"
	"crypto/rand"
	"io"
)

// KeyExchange handles ECDH key exchange using P-256.
type KeyExchange struct {
	privateKey *ecdh.PrivateKey
	publicKey  *ecdh.PublicKey
}

func NewKeyExchange() (*KeyExchange, error) {
	priv, err := ecdh.P256().GenerateKey(rand.Reader)
	if err != nil {
		return nil, err
	}
	return &KeyExchange{
		privateKey: priv,
		publicKey:  priv.PublicKey(),
	}, nil
}

func (k *KeyExchange) PublicKey() []byte {
	return k.publicKey.Bytes()
}

func (k *KeyExchange) ComputeSharedSecret(remotePubKey []byte) ([]byte, error) {
	remote, err := ecdh.P256().NewPublicKey(remotePubKey)
	if err != nil {
		return nil, err
	}
	return k.privateKey.ECDH(remote)
}

// Cipher wraps AES-GCM for packet encryption.
type Cipher struct {
	aead cipher.AEAD
}

func NewCipher(key []byte) (*Cipher, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	aead, err := cipher.NewGCM(block)
	if err != nil {
		return nil, err
	}
	return &Cipher{aead: aead}, nil
}

func (c *Cipher) Encrypt(nonce, plaintext, additionalData []byte) []byte {
	return c.aead.Seal(nil, nonce, plaintext, additionalData)
}

func (c *Cipher) Decrypt(nonce, ciphertext, additionalData []byte) ([]byte, error) {
	return c.aead.Open(nil, nonce, ciphertext, additionalData)
}

// GenerateNonce generates a random nonce of the specified size.
func GenerateNonce(size int) ([]byte, error) {
	nonce := make([]byte, size)
	if _, err := io.ReadFull(rand.Reader, nonce); err != nil {
		return nil, err
	}
	return nonce, nil
}
