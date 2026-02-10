package netio

import (
	"net"
	"strings"
)

// Address wraps net.UDPAddr to provide TNL-compatible address handling.
type Address struct {
	UDPAddr *net.UDPAddr
}

// NewAddress creates an Address from a net.UDPAddr.
func NewAddress(addr *net.UDPAddr) Address {
	return Address{UDPAddr: addr}
}

// ParseAddress parses a string into an Address.
// Supports formats: "host:port", "ip:port", and TNL format "ip:port" (no transport prefix needed for Go).
func ParseAddress(s string) (Address, error) {
	// TNL often uses "transport:host:port" (e.g., "ip:127.0.0.1:28000")
	// We'll strip the "ip:" or "udp:" prefix if present.
	if strings.HasPrefix(s, "ip:") {
		s = s[3:]
	} else if strings.HasPrefix(s, "udp:") {
		s = s[4:]
	}

	addr, err := net.ResolveUDPAddr("udp", s)
	if err != nil {
		return Address{}, err
	}
	return Address{UDPAddr: addr}, nil
}

func (a Address) String() string {
	if a.UDPAddr == nil {
		return "none"
	}
	return a.UDPAddr.String()
}

func (a Address) IsEqual(other Address) bool {
	if a.UDPAddr == nil || other.UDPAddr == nil {
		return a.UDPAddr == other.UDPAddr
	}
	return a.UDPAddr.IP.Equal(other.UDPAddr.IP) && a.UDPAddr.Port == other.UDPAddr.Port
}

// IsEqualAddress returns true if the IPs match, ignoring the port.
func (a Address) IsEqualAddress(other Address) bool {
	if a.UDPAddr == nil || other.UDPAddr == nil {
		return a.UDPAddr == other.UDPAddr
	}
	return a.UDPAddr.IP.Equal(other.UDPAddr.IP)
}

func (a Address) Hash() uint32 {
	if a.UDPAddr == nil || len(a.UDPAddr.IP) == 0 {
		return 0
	}
	// Simple hash similar to TNL's
	var h uint32
	for _, b := range a.UDPAddr.IP {
		h = h*31 + uint32(b)
	}
	h = h ^ (uint32(a.UDPAddr.Port) << 8)
	return h
}
