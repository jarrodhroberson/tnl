package tnl

import (
	"fmt"
	"reflect"
	"sync"
)

// NetClassType defines the ranges of individual class identifiers
type NetClassType int

const (
	NetClassTypeNone NetClassType = iota - 1
	NetClassTypeObject
	NetClassTypeDataBlock
	NetClassTypeEvent
	NetClassTypeCount
)

// NetClassGroup defines different service types for an application
type NetClassGroup int

const (
	NetClassGroupGame NetClassGroup = iota
	NetClassGroupCommunity
	NetClassGroupMaster
	NetClassGroupUnused2
	NetClassGroupCount
	NetClassGroupInvalid = NetClassGroupCount
)

// NetClassMask values indicate which NetClassGroup(s) an object can be transmitted through
type NetClassMask uint32

const (
	NetClassGroupGameMask      NetClassMask = 1 << NetClassGroupGame
	NetClassGroupCommunityMask NetClassMask = 1 << NetClassGroupCommunity
	NetClassGroupMasterMask    NetClassMask = 1 << NetClassGroupMaster
	NetClassGroupAllMask       NetClassMask = (1 << NetClassGroupCount) - 1
)

// NetClassRep represents each declared NetClass and is used to construct object instances
type NetClassRep struct {
	className           string
	classType           NetClassType
	classGroupMask      U32
	classVersion        S32
	classID             [NetClassGroupCount]U32
	initialUpdateBits   U32
	partialUpdateBits   U32
	initialUpdateCount  U32
	partialUpdateCount  U32
	createFunc          func() Object
}

// Global class management
var (
	classLinkList                                                      []*NetClassRep
	classTable    [NetClassGroupCount][NetClassTypeCount][]*NetClassRep
	classCRC      [NetClassGroupCount]U32
	netClassBits  [NetClassGroupCount][NetClassTypeCount]U32
	initialized   bool
	classMutex    sync.RWMutex
)

// NewNetClassRep creates a new NetClassRep
func NewNetClassRep(className string, groupMask U32, classType NetClassType, classVersion S32, createFunc func() Object) *NetClassRep {
	rep := &NetClassRep{
		className:      className,
		classType:      classType,
		classGroupMask: groupMask,
		classVersion:   classVersion,
		createFunc:     createFunc,
	}
	
	classMutex.Lock()
	classLinkList = append(classLinkList, rep)
	classMutex.Unlock()
	
	return rep
}

// GetClassID returns the class ID within its type for the particular group
func (r *NetClassRep) GetClassID(classGroup NetClassGroup) U32 {
	return r.classID[classGroup]
}

// GetClassType returns the NetClassType of this class
func (r *NetClassRep) GetClassType() NetClassType {
	return r.classType
}

// GetClassVersion returns the version of this class
func (r *NetClassRep) GetClassVersion() S32 {
	return r.classVersion
}

// GetClassName returns the string class name
func (r *NetClassRep) GetClassName() string {
	return r.className
}

// AddInitialUpdate records bits used in the initial update
func (r *NetClassRep) AddInitialUpdate(bitCount U32) {
	r.initialUpdateCount++
	r.initialUpdateBits += bitCount
}

// AddPartialUpdate records bits used in a partial update
func (r *NetClassRep) AddPartialUpdate(bitCount U32) {
	r.partialUpdateCount++
	r.partialUpdateBits += bitCount
}

// Create creates an instance of the class this represents
func (r *NetClassRep) Create() Object {
	if r.createFunc != nil {
		return r.createFunc()
	}
	return nil
}

// Global functions for class management

// GetNetClassCount returns the number of classes registered under classGroup and classType
func GetNetClassCount(classGroup U32, classType U32) U32 {
	classMutex.RLock()
	defer classMutex.RUnlock()
	return U32(len(classTable[classGroup][classType]))
}

// GetNetClassBitSize returns the number of bits necessary to transmit class IDs
func GetNetClassBitSize(classGroup U32, classType U32) U32 {
	classMutex.RLock()
	defer classMutex.RUnlock()
	return netClassBits[classGroup][classType]
}

// IsVersionBorderCount returns true if the given class count is on a version boundary
func IsVersionBorderCount(classGroup U32, classType U32, count U32) bool {
	classMutex.RLock()
	defer classMutex.RUnlock()
	
	table := classTable[classGroup][classType]
	if count == U32(len(table)) {
		return true
	}
	if count > 0 && int(count) < len(table) && int(count-1) < len(table) {
		return table[count].GetClassVersion() != table[count-1].GetClassVersion()
	}
	return false
}

// GetClass returns the NetClassRep for the specified index
func GetClass(classGroup U32, classType U32, index U32) *NetClassRep {
	classMutex.RLock()
	defer classMutex.RUnlock()
	
	table := classTable[classGroup][classType]
	if int(index) < len(table) {
		return table[index]
	}
	return nil
}

// GetClassGroupCRC returns a CRC of class data for checking on connection
func GetClassGroupCRC(classGroup NetClassGroup) U32 {
	classMutex.RLock()
	defer classMutex.RUnlock()
	return classCRC[classGroup]
}

// Initialize initializes the class table and associated data
func Initialize() {
	classMutex.Lock()
	defer classMutex.Unlock()
	
	if initialized {
		return
	}
	
	// Clear tables
	for i := range classTable {
		for j := range classTable[i] {
			classTable[i][j] = nil
		}
	}
	
	// Process all registered classes
	for _, rep := range classLinkList {
		for group := 0; group < int(NetClassGroupCount); group++ {
			if rep.classGroupMask&(1<<uint(group)) != 0 {
				classType := int(rep.classType)
				if classType >= 0 && classType < int(NetClassTypeCount) {
					rep.classID[group] = U32(len(classTable[group][classType]))
					classTable[group][classType] = append(classTable[group][classType], rep)
				}
			}
		}
	}
	
	// Calculate bit sizes needed for each class group/type combination
	for group := 0; group < int(NetClassGroupCount); group++ {
		for classType := 0; classType < int(NetClassTypeCount); classType++ {
			count := len(classTable[group][classType])
			if count > 0 {
				netClassBits[group][classType] = GetNextBinLog2(U32(count))
			}
		}
	}
	
	initialized = true
}

// CreateObject creates an instance of a class by name
func CreateObject(className string) Object {
	classMutex.RLock()
	defer classMutex.RUnlock()
	
	for _, rep := range classLinkList {
		if rep.className == className {
			return rep.Create()
		}
	}
	return nil
}

// CreateObjectByID creates an instance of a class by group, type, and class ID
func CreateObjectByID(groupID, typeID, classID U32) Object {
	classMutex.RLock()
	defer classMutex.RUnlock()
	
	if groupID < U32(NetClassGroupCount) && typeID < U32(NetClassTypeCount) {
		table := classTable[groupID][typeID]
		if int(classID) < len(table) {
			return table[classID].Create()
		}
	}
	return nil
}

// Object is the base interface for all networked objects
type Object interface {
	GetClassRep() *NetClassRep
	GetClassID(classGroup NetClassGroup) U32
	GetClassName() string
	DestroySelf()
}

// BaseObject provides a basic implementation of Object
type BaseObject struct {
	classRep *NetClassRep
	refCount int32
}

// NewBaseObject creates a new BaseObject
func NewBaseObject() *BaseObject {
	return &BaseObject{refCount: 1}
}

// GetClassRep returns the NetClassRep associated with this object
func (o *BaseObject) GetClassRep() *NetClassRep {
	return o.classRep
}

// SetClassRep sets the NetClassRep for this object
func (o *BaseObject) SetClassRep(rep *NetClassRep) {
	o.classRep = rep
}

// GetClassID returns the class ID within the specified NetClassGroup
func (o *BaseObject) GetClassID(classGroup NetClassGroup) U32 {
	if o.classRep != nil {
		return o.classRep.GetClassID(classGroup)
	}
	return 0
}

// GetClassName returns the unmangled class name
func (o *BaseObject) GetClassName() string {
	if o.classRep != nil {
		return o.classRep.GetClassName()
	}
	return ""
}

// DestroySelf destroys the object (default implementation)
func (o *BaseObject) DestroySelf() {
	// Default implementation - can be overridden
}

// RefPtr provides reference counting for objects
type RefPtr[T Object] struct {
	object T
}

// NewRefPtr creates a new RefPtr
func NewRefPtr[T Object](obj T) *RefPtr[T] {
	return &RefPtr[T]{object: obj}
}

// Get returns the referenced object
func (r *RefPtr[T]) Get() T {
	return r.object
}

// Set sets the referenced object
func (r *RefPtr[T]) Set(obj T) {
	r.object = obj
}

// IsNull returns true if the pointer is null
func (r *RefPtr[T]) IsNull() bool {
	return r.object == nil || reflect.ValueOf(r.object).IsNil()
}

// IsValid returns true if the pointer is valid
func (r *RefPtr[T]) IsValid() bool {
	return !r.IsNull()
}

// SafePtr provides safe pointers that are automatically set to nil when the object is deleted
type SafePtr[T Object] struct {
	object T
}

// NewSafePtr creates a new SafePtr
func NewSafePtr[T Object](obj T) *SafePtr[T] {
	return &SafePtr[T]{object: obj}
}

// Get returns the referenced object
func (s *SafePtr[T]) Get() T {
	return s.object
}

// Set sets the referenced object
func (s *SafePtr[T]) Set(obj T) {
	s.object = obj
}

// IsNull returns true if the pointer is null
func (s *SafePtr[T]) IsNull() bool {
	return s.object == nil || reflect.ValueOf(s.object).IsNil()
}

// IsValid returns true if the pointer is valid
func (s *SafePtr[T]) IsValid() bool {
	return !s.IsNull()
}

// RegisterClass is a helper function to register a class with the TNL system
func RegisterClass[T Object](className string, groupMask U32, classType NetClassType, classVersion S32, createFunc func() T) *NetClassRep {
	return NewNetClassRep(className, groupMask, classType, classVersion, func() Object {
		return createFunc()
	})
}

// LogBitUsage logs the bit usage information of all NetClassReps
func LogBitUsage() {
	classMutex.RLock()
	defer classMutex.RUnlock()
	
	fmt.Println("TNL Class Bit Usage Report:")
	fmt.Println("===========================")
	
	for _, rep := range classLinkList {
		if rep.initialUpdateCount > 0 || rep.partialUpdateCount > 0 {
			fmt.Printf("Class: %s\n", rep.className)
			if rep.initialUpdateCount > 0 {
				avgInitial := float64(rep.initialUpdateBits) / float64(rep.initialUpdateCount)
				fmt.Printf("  Initial Updates: %d packets, %d total bits, %.2f avg bits/packet\n",
					rep.initialUpdateCount, rep.initialUpdateBits, avgInitial)
			}
			if rep.partialUpdateCount > 0 {
				avgPartial := float64(rep.partialUpdateBits) / float64(rep.partialUpdateCount)
				fmt.Printf("  Partial Updates: %d packets, %d total bits, %.2f avg bits/packet\n",
					rep.partialUpdateCount, rep.partialUpdateBits, avgPartial)
			}
		}
	}
}