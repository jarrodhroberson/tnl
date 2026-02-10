package crypto

import (
	"crypto/sha256"
	"encoding/binary"
)

// Puzzle represents the TNL client puzzle logic.
type Puzzle struct{}

// NewPuzzle creates a new Puzzle instance.
func NewPuzzle() *Puzzle {
	return &Puzzle{}
}

// CheckSolution verifies if a puzzle solution is valid for the given parameters.
// difficulty is the number of leading zero bits required in the SHA-256 hash.
func (p *Puzzle) CheckSolution(solution uint32, clientIdentity uint32, clientNonce []byte, serverNonce []byte, difficulty uint32) bool {
	// Buffer: [solution (4b)] [clientIdentity (4b)] [clientNonce (16b)] [serverNonce (16b)]
	buf := make([]byte, 8+len(clientNonce)+len(serverNonce))
	binary.BigEndian.PutUint32(buf[0:4], solution)
	binary.BigEndian.PutUint32(buf[4:8], clientIdentity)
	copy(buf[8:], clientNonce)
	copy(buf[8+len(clientNonce):], serverNonce)

	h := sha256.Sum256(buf)

	// Check difficulty (leading zero bits)
	index := 0
	d := difficulty
	for d > 8 {
		if h[index] != 0 {
			return false
		}
		index++
		d -= 8
	}

	if d > 0 {
		mask := uint8(0xFF << (8 - d))
		if (h[index] & mask) != 0 {
			return false
		}
	}

	return true
}

// SolvePuzzle attempts to find a solution for the puzzle.
// This is a brute-force approach, mirroring TNL's solvePuzzle.
func (p *Puzzle) SolvePuzzle(clientIdentity uint32, clientNonce []byte, serverNonce []byte, difficulty uint32) uint32 {
	var solution uint32
	for {
		if p.CheckSolution(solution, clientIdentity, clientNonce, serverNonce, difficulty) {
			return solution
		}
		solution++
		// In a real implementation, we might want to check for context cancellation
	}
}
