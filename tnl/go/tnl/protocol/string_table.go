package protocol

import (
	"sync"
)

// StringTable manages the mapping of strings to IDs to minimize network traffic.
type StringTable struct {
	mu      sync.RWMutex
	strToID map[string]uint32
	idToStr map[uint32]string
	nextID  uint32
}

var GlobalStringTable = &StringTable{
	strToID: make(map[string]uint32),
	idToStr: make(map[uint32]string),
	nextID:  1,
}

func (st *StringTable) GetID(s string) uint32 {
	st.mu.RLock()
	id, ok := st.strToID[s]
	st.mu.RUnlock()

	if ok {
		return id
	}

	st.mu.Lock()
	defer st.mu.Unlock()

	// Double check
	if id, ok := st.strToID[s]; ok {
		return id
	}

	id = st.nextID
	st.strToID[s] = id
	st.idToStr[id] = s
	st.nextID++
	return id
}

func (st *StringTable) GetString(id uint32) (string, bool) {
	st.mu.RLock()
	defer st.mu.RUnlock()

	s, ok := st.idToStr[id]
	return s, ok
}
