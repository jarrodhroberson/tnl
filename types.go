// Package tnl provides a Go implementation of the Torque Network Library
// Copyright (C) 2024 - Converted from original TNL C++ implementation
package tnl

import (
	"math"
	"unsafe"
)

// Basic type definitions equivalent to TNL C++ types
type (
	S8  int8
	U8  uint8
	S16 int16
	U16 uint16
	S32 int32
	U32 uint32
	S64 int64
	U64 uint64
	F32 float32
	F64 float64
)

// Constants
const (
	FloatOne        F32 = 1.0
	FloatHalf       F32 = 0.5
	FloatZero       F32 = 0.0
	FloatPi         F32 = math.Pi
	Float2Pi        F32 = 2.0 * math.Pi
	FloatInversePi  F32 = 1.0 / math.Pi
	FloatHalfPi     F32 = 0.5 * math.Pi
	Float2InversePi F32 = 2.0 / math.Pi
	FloatInverse2Pi F32 = 0.5 / math.Pi
	FloatSqrt2      F32 = 1.41421356237309504880
	FloatSqrtHalf   F32 = 0.7071067811865475244008443
)

// NetType serves as a base interface for all bit-compressed network types
type NetType interface {
	GetPrecisionBits() U32
}

// Int represents an unsigned integer with specified bit count
type Int struct {
	Value    U32
	BitCount U32
}

func NewInt(value U32, bitCount U32) Int {
	return Int{Value: value, BitCount: bitCount}
}

func (i Int) GetPrecisionBits() U32 {
	return i.BitCount
}

// SignedInt represents a signed integer with specified bit count
type SignedInt struct {
	Value    S32
	BitCount U32
}

func NewSignedInt(value S32, bitCount U32) SignedInt {
	return SignedInt{Value: value, BitCount: bitCount}
}

func (s SignedInt) GetPrecisionBits() U32 {
	return s.BitCount
}

// Float represents a 0...1 float with specified bit count
type Float struct {
	Value    F32
	BitCount U32
}

func NewFloat(value F32, bitCount U32) Float {
	return Float{Value: value, BitCount: bitCount}
}

func (f Float) GetPrecisionBits() U32 {
	return f.BitCount
}

// SignedFloat represents a -1...1 float with specified bit count
type SignedFloat struct {
	Value    F32
	BitCount U32
}

func NewSignedFloat(value F32, bitCount U32) SignedFloat {
	return SignedFloat{Value: value, BitCount: bitCount}
}

func (s SignedFloat) GetPrecisionBits() U32 {
	return s.BitCount
}

// RangedU32 represents an unsigned integer within a specific range
type RangedU32 struct {
	Value      U32
	RangeStart U32
	RangeEnd   U32
}

func NewRangedU32(value, rangeStart, rangeEnd U32) RangedU32 {
	return RangedU32{Value: value, RangeStart: rangeStart, RangeEnd: rangeEnd}
}

func (r RangedU32) GetPrecisionBits() U32 {
	rangeSize := r.RangeEnd - r.RangeStart + 1
	return GetNextBinLog2(rangeSize)
}

// Point3F represents a 3D point or vector
type Point3F struct {
	X, Y, Z F32
}

// Utility functions
func IsPow2(number U32) bool {
	return (number & (number - 1)) == 0
}

func GetBinLog2(value U32) U32 {
	floatValue := F32(value)
	return (*(*U32)(unsafe.Pointer(&floatValue)) >> 23) - 127
}

func GetNextBinLog2(number U32) U32 {
	if IsPow2(number) {
		return GetBinLog2(number)
	}
	return GetBinLog2(number) + 1
}

func GetNextPow2(value U32) U32 {
	if IsPow2(value) {
		return value
	}
	return 1 << (GetBinLog2(value) + 1)
}

// Min/Max functions
func GetMinU32(a, b U32) U32 {
	if a > b {
		return b
	}
	return a
}

func GetMaxU32(a, b U32) U32 {
	if a > b {
		return a
	}
	return b
}

func GetMinS32(a, b S32) S32 {
	if a > b {
		return b
	}
	return a
}

func GetMaxS32(a, b S32) S32 {
	if a > b {
		return a
	}
	return b
}

func GetMinF32(a, b F32) F32 {
	if a > b {
		return b
	}
	return a
}

func GetMaxF32(a, b F32) F32 {
	if a > b {
		return a
	}
	return b
}

// Buffer utility functions
func WriteU32ToBuffer(value U32, buffer []U8) {
	buffer[0] = U8(value >> 24)
	buffer[1] = U8(value >> 16)
	buffer[2] = U8(value >> 8)
	buffer[3] = U8(value)
}

func ReadU32FromBuffer(buffer []U8) U32 {
	return (U32(buffer[0]) << 24) |
		(U32(buffer[1]) << 16) |
		(U32(buffer[2]) << 8) |
		U32(buffer[3])
}

func WriteU16ToBuffer(value U16, buffer []U8) {
	buffer[0] = U8(value >> 8)
	buffer[1] = U8(value)
}

func ReadU16FromBuffer(buffer []U8) U16 {
	return (U16(buffer[0]) << 8) | U16(buffer[1])
}

func FourByteAlign(value U32) U32 {
	return (value + 3) & ^U32(3)
}

func BIT(x U32) U32 {
	return 1 << x
}