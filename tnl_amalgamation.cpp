// TNL Amalgamated Source File
// Generated from directory: tnl

// --- System Includes ---
// --- Headers ---
// --- START OF FILE: tnl/tnl.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_H_
#define _TNL_H_

/// Global namespace for all TNL classes
namespace TNL {
};

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

#ifndef _TNL_ASSERT_H_
#include "tnlAssert.h"
#endif

#ifndef _TNL_ENDIAN_H_
#include "tnlEndian.h"
#endif

#ifndef _TNL_PLATFORM_H_
#include "tnlPlatform.h"
#endif

#ifndef _TNL_UDP_H_
#include "tnlUDP.h"
#endif

#endif

// --- END OF FILE: tnl/tnl.h ---
// --- START OF FILE: tnl/tnlAssert.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_ASSERT_H_
#define _TNL_ASSERT_H_

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

namespace TNL
{

/**
   sprintf style string formating into a fixed temporary buffer.
   @param in_msg sprintf style format string
   @returns pointer to fixed buffer containing formatted string

   <b>Example:</b>
   @code
   U8 a = 5;
   S16 b = -10;
   char *output = avar("hello %s! a=%u, b=%d", "world");
   ouput = "hello world! a=5, b=-10"
   @endcode
   @warning avar uses a static fixed buffer.  Treat the buffer as volatile data
   and use it immediately.  Other functions my use avar too and clobber the buffer.
 */
const char* avar(const char *in_msg, ...);

#ifdef TNL_ENABLE_ASSERTS

/// Assert functions for validating proper program state.
class Assert
{
public:
   static void processAssert(const char*  filename, U32 lineNumber, const char*  message);
   static bool processingAssert();
};


   /**
      Assert that the statement x is true, otherwise halt.
      If the statement x is true, continue processing.
      If the statement x is false, log the file and line where the assert occured,
      the message y and displaying a dialog containing the message y. The user then
      has the option to halt or continue causing the debugger to break.
      These asserts are only present when TNL_ENABLE_ASSERTS is defined.
      This assert is very useful for verifying data as well as function entry and
      exit conditions.
    */
   #define TNLAssert(x, y) { if (!bool(x)) { TNL::Assert::processAssert( __FILE__, __LINE__,  y); TNL_DEBUGBREAK(); } }

   /**
      TNLAssertV - same as TNLAssert, except that the message y must be a
      parenthesized printf-style variable argument list.
      These asserts are only present in DEBUG builds.
    */
   #define TNLAssertV(x, y) { if (!bool(x)) { TNL::Assert::processAssert(__FILE__, __LINE__,  avar y); TNL_DEBUGBREAK(); } }

#else
   #define TNLAssert(x, y)    { }
   #define TNLAssertV(x, y)   { }
#endif

};

#endif // _TNL_ASSERT_H_


// --- END OF FILE: tnl/tnlAssert.h ---
// --- START OF FILE: tnl/tnlAsymmetricKey.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_ASYMMETRICKEY_H_
#define _TNL_ASYMMETRICKEY_H_

#ifndef _TNL_BITSTREAM_H_
#include "tnlBitStream.h"
#endif


namespace TNL {

class AsymmetricKey : public Object
{
   /// Raw key data.
   ///
   /// The specific format of this buffer varies by cryptosystem, so look
   /// at the subclass to see how it's laid out. In general this is an
   /// opaque buffer.
   void *mKeyData;

   /// Size of the key at construct time.
   U32 mKeySize;

   /// Do we have the private key for this?
   ///
   /// We may only have access to the public key (for instance, when validating
   /// a message signed by someone else).
   bool mHasPrivateKey;

   /// Buffer containing the public half of this keypair.
   ByteBufferPtr mPublicKey;

   /// Buffer containing the private half of this keypair.
   ByteBufferPtr mPrivateKey;
   bool mIsValid;

   /// Load keypair from a buffer.
   void load(const ByteBuffer &theBuffer);

   /// Enum used to indicate the portion of the key we are working with.
   enum KeyType {
      KeyTypePrivate,
      KeyTypePublic,
   };
public:

   /// Constructs an AsymmetricKey from the specified data pointer.
   AsymmetricKey(U8 *dataPtr, U32 bufferSize) : mKeyData(NULL)
   {
      load(ByteBuffer(dataPtr, bufferSize));
   }

   /// Constructs an AsymmetricKey from a ByteBuffer.
   AsymmetricKey(const ByteBuffer &theBuffer) : mKeyData(NULL)
   {
      load(theBuffer);
   }

   /// Constructs an AsymmetricKey by reading it from a BitStream.
   AsymmetricKey(BitStream *theStream)
   {
      ByteBuffer theBuffer;
      theStream->read(&theBuffer);
      load(theBuffer);
   }

   /// Generates a new asymmetric key of keySize bytes
   AsymmetricKey(U32 keySize);
   
   /// Destructor for the AsymmetricKey.
   ~AsymmetricKey();

   /// Returns a ByteBuffer containing an encoding of the public key.
   ByteBufferPtr getPublicKey() { return mPublicKey; }

   /// Returns a ByteBuffer containing an encoding of the private key.
   ByteBufferPtr getPrivateKey() { return mPrivateKey; }

   /// Returns true if this AsymmetricKey is a key pair.
   bool hasPrivateKey() { return mHasPrivateKey; }

   /// Returns true if this is a valid key.
   bool isValid() { return mIsValid; }
   /// Compute a key we can share with the specified AsymmetricKey
   /// for a symmetric crypto.
   ByteBufferPtr computeSharedSecretKey(AsymmetricKey *publicKey);

   /// Returns the strength of the AsymmetricKey in byte size.
   U32 getKeySize() { return mKeySize; }

   /// Constructs a digital signature for the specified buffer of bits.  This
   /// method only works for private keys.  A public key only Asymmetric key
   /// will generate a signature of 0 bytes in length.
   ByteBufferPtr hashAndSign(const ByteBuffer &theByteBuffer);

   /// Returns true if the private key associated with this AsymmetricKey 
   /// signed theByteBuffer with theSignature.
   bool verifySignature(const ByteBuffer &theByteBuffer, const ByteBuffer &theSignature);
};

typedef RefPtr<AsymmetricKey> AsymmetricKeyPtr;

};

#endif

// --- END OF FILE: tnl/tnlAsymmetricKey.h ---
// --- START OF FILE: tnl/tnlBitSet.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_BITSET_H_
#define _TNL_BITSET_H_

//Includes
#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

namespace TNL {

/// Represents a word of bits.
///
/// A helper class to perform operations on a set of 32 bits.
class BitSet32
{
private:
   U32 mBits;

public:
   /// Default constructor initializes this bit set to all zeros.
   BitSet32()                         { mBits = 0; }

   /// Copy constructor.
   BitSet32(const BitSet32& in_rCopy) { mBits = in_rCopy.mBits; }

   /// Construct from an input U32.
   BitSet32(const U32 in_mask)        { mBits = in_mask; }

   /// @name Accessors
   /// @{

   /// Returns the U32 representation of the bit set.
   operator U32() const               { return mBits; }

   /// Returns the U32 representation of the bit set.
   U32 getMask() const                { return mBits; }

   /// @}

   /// @name Mutators
   ///
   /// Most of these methods take a word (ie, a BitSet32) of bits
   /// to operate with.
   /// @{

   /// Sets all the bits in the bit set to 1
   void set()                         { mBits  = 0xFFFFFFFFUL; }

   /// Sets all the bits in the bit set that are set in m.
   void set(const U32 m)              { mBits |= m; }

   /// For each bit set in s, sets or clears that bit in this, depending on whether b is true or false.
   void set(BitSet32 s, bool b)       { mBits = (mBits&~(s.mBits))|(b?s.mBits:0); }

   /// Clears all the bits in the bit set to 0.
   void clear()                       { mBits  = 0; }

   /// Clears all the bits in the bit set that are set in m.
   void clear(const U32 m)            { mBits &= ~m; }

   /// Flips all the bits in the bit set that are set in m.
   void toggle(const U32 m)           { mBits ^= m; }

   /// Test if the passed bits are set.
   bool test(const U32 m) const       { return (mBits & m) != 0; }

   /// Test if the passed bits and only the passed bits are set.
   bool testStrict(const U32 m) const { return (mBits & m) == m; }

   BitSet32& operator =(const U32 m)  { mBits  = m;  return *this; }
   BitSet32& operator|=(const U32 m)  { mBits |= m; return *this; }
   BitSet32& operator&=(const U32 m)  { mBits &= m; return *this; }
   BitSet32& operator^=(const U32 m)  { mBits ^= m; return *this; }

   BitSet32 operator|(const U32 m) const { return BitSet32(mBits | m); }
   BitSet32 operator&(const U32 m) const { return BitSet32(mBits & m); }
   BitSet32 operator^(const U32 m) const { return BitSet32(mBits ^ m); }

   /// @}
};

};

#endif //_TNL_BITSET_H_

// --- END OF FILE: tnl/tnlBitSet.h ---
// --- START OF FILE: tnl/tnlBitStream.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_BITSTREAM_H_
#define _TNL_BITSTREAM_H_

#ifndef _TNL_BYTEBUFFER_H_
#include "tnlByteBuffer.h"
#endif

#ifndef _TNL_CONNECTIONSTRINGTABLE_H_
#include "tnlConnectionStringTable.h"
#endif

namespace TNL {

class SymmetricCipher;

/// Point3F is used by BitStream for transmitting 3D points and vectors.
///
/// Users of the TNL should provide conversion operators to and from
/// this TNL-native type.
struct Point3F
{
   F32 x; ///< the X coordinate
   F32 y; ///< the Y coordinate
   F32 z; ///< the Z coordinate
};

/// Helper macro used in BitStream declaration.
///
/// @note DeclareTemplatizedReadWrite macro declares a read and write function
///       for the specified type. This is a macro because MSVC6 has seriously
///       broken template functionality.  Thanks MS!
#define DeclareTemplatizedReadWrite(T) \
inline bool write(T value) { T temp = convertHostToLEndian(value); return write(sizeof(T), &temp); } \
inline bool read(T *value) { T temp; bool success = read(sizeof(T), &temp); *value = convertLEndianToHost(temp); return success;}

/// BitStream provides a bit-level stream interface to a data buffer.
class BitStream : public ByteBuffer
{
protected:
   enum {
      ResizePad = 1500,
   };
   U32  bitNum;               ///< The current bit position for reading/writing in the bit stream.
   bool error;                ///< Flag set if a user operation attempts to read or write past the max read/write sizes.
   bool mCompressRelative;    ///< Flag set if the bit stream should compress points relative to a compression point.
   Point3F mCompressPoint;    ///< Reference point for relative point compression.
   U32  maxReadBitNum;        ///< Last valid read bit position.
   U32  maxWriteBitNum;       ///< Last valid write bit position.
   ConnectionStringTable *mStringTable; ///< String table used to compress StringTableEntries over the network.
   /// String buffer holds the last string written into the stream for substring compression.
   char mStringBuffer[256];

   bool resizeBits(U32 numBitsNeeded);
public:

   /// @name Constructors
   ///
   /// Note that the BitStream essentially wraps an existing buffer, so to use a bitstream you must
   /// have an existing buffer for it to work with!
   ///
   /// @{

   /// Default to maximum write size being the size of the buffer.
   BitStream(U8 *bufPtr, U32 bufSize) : ByteBuffer(bufPtr, bufSize) { setMaxSizes(bufSize, bufSize); reset(); }

   /// Optionally, specify a maximum write size.
   BitStream(U8 *bufPtr, U32 bufSize, U32 maxWriteSize) 
      : ByteBuffer(bufPtr, bufSize) { setMaxSizes(bufSize, maxWriteSize); reset(); }

   /// Creates a resizable BitStream
   BitStream() : ByteBuffer() { setMaxSizes( getBufferSize(), getBufferSize() ); reset(); }
   /// @}

   /// Sets the maximum read and write sizes for the BitStream.
   void setMaxSizes(U32 maxReadSize, U32 maxWriteSize = 0);

   /// Sets the maximum read and write bit sizes for the BitStream.
   void setMaxBitSizes(U32 maxReadBitSize, U32 maxWriteBitSize = 0);

   /// resets the read/write position to 0 and clears any error state.
   void reset();

   /// clears the string compression buffer.
   void clearStringBuffer() { mStringBuffer[0] = 0; }

   /// sets the ConnectionStringTable for compressing string table entries across the network
   void setStringTable(ConnectionStringTable *table) { mStringTable = table; }

   /// clears the error state from an attempted read or write overrun
   void clearError() { error = false; }

   /// Returns a pointer to the next byte in the BitStream from the current bit position
   U8*  getBytePtr();

   /// Returns the current position in the stream rounded up to the next byte.
   U32 getBytePosition() const;
   /// Returns the current bit position in the stream
   U32 getBitPosition() const;
   /// Sets the position in the stream to the first bit of byte newPosition.
   void setBytePosition(const U32 newPosition);
   /// Sets the position in the stream to newBitPosition.
   void setBitPosition(const U32 newBitPosition);
   /// Advances the position in the stream by numBits.
   void advanceBitPosition(const S32 numBits);

   /// Returns the maximum readable bit position
   U32 getMaxReadBitPosition() const { return maxReadBitNum; }

   /// Returns the number of bits that can be written into the BitStream without resizing
   U32 getBitSpaceAvailable() const { return maxWriteBitNum - bitNum; }

   /// Pads the bits up to the next byte boundary with 0's.
   void zeroToByteBoundary();

   /// Writes an unsigned integer value between 0 and 2^(bitCount - 1) into the stream.
   void writeInt(U32 value, U8 bitCount);
   /// Reads an unsigned integer value between 0 and 2^(bitCount - 1) from the stream.
   U32  readInt(U8 bitCount);

   /// Writes an unsigned integer value between 0 and 2^(bitCount -1) into the stream at the specified position, without changing the current write position.
   void writeIntAt(U32 value, U8 bitCount, U32 bitPosition);

   /// Writes a signed integer value between -2^(bitCount-1) and 2^(bitCount-1) - 1.
   void writeSignedInt(S32 value, U8 bitCount);
   /// Reads a signed integer value between -2^(bitCount-1) and 2^(bitCount-1) - 1.
   S32  readSignedInt(U8 bitCount);

   /// Writes an unsigned integer value in the range rangeStart to rangeEnd inclusive.
   void writeRangedU32(U32 value, U32 rangeStart, U32 rangeEnd);
   /// Reads an unsigned integer value in the range rangeStart to rangeEnd inclusive.
   U32  readRangedU32(U32 rangeStart, U32 rangeEnd);

   /// Writes an enumeration value in the range of 0 ... enumRange - 1.
   void writeEnum(U32 enumValue, U32 enumRange);
   /// Reads an enumeration value in the range 0 ... enumRange - 1.
   U32 readEnum(U32 enumRange);

   /// Writes a float from 0 to 1 inclusive, using bitCount bits of precision.
   void writeFloat(F32 f, U8 bitCount);
   /// Reads a float from 0 to 1 inclusive, using bitCount bits of precision.
   F32  readFloat(U8 bitCount);

   /// Writes a signed float from -1 to 1 inclusive, using bitCount bits of precision.
   void writeSignedFloat(F32 f, U8 bitCount);
   /// Reads a signed float from -1 to 1 inclusive, using bitCount bits of precision.
   F32  readSignedFloat(U8 bitCount);

   /// Writes an object's class ID, given its class type and class group.
   void writeClassId(U32 classId, U32 classType, U32 classGroup);
   /// Reads a class ID for an object, given a class type and class group.  Returns -1 if the class type is out of range
   U32 readClassId(U32 classType, U32 classGroup);

   /// Writes a normalized vector into the stream, using bitCount bits for the precision of angles phi and theta.
   void writeNormalVector(const Point3F &vec, U8 bitCount);
   /// Reads a normalized vector from the stream, using bitCount bits for the precision of angles phi and theta.
   void readNormalVector(Point3F *vec, U8 bitCount);

   /// Uses the same method as in writeNormalVector to reduce the precision of a normal vector
   /// to determine what will be read from the stream.
   static Point3F dumbDownNormal(const Point3F& vec, U8 bitCount);

   /// Writes a normalized vector by writing a z value and theta angle.
   void writeNormalVector(const Point3F& vec, U8 angleBitCount, U8 zBitCount);
   /// Reads a normalized vector by reading a z value and theta angle.
   void readNormalVector(Point3F *vec, U8 angleBitCount, U8 zBitCount);

   /// Sets a reference point for subsequent compressed point writing.
   void setPointCompression(const Point3F &p);
   /// Disables compression of point.
   void clearPointCompression();
   /// Writes a point into the stream, to a precision denoted by scale.
   void writePointCompressed(const Point3F &p, F32 scale);
   /// Reads a compressed point from the stream, to a precision denoted by scale.
   void readPointCompressed(Point3F *p, F32 scale);

   /// Writes bitCount bits into the stream from bitPtr.
   bool writeBits(U32 bitCount, const void *bitPtr);
   /// Reads bitCount bits from the stream into bitPtr.
   bool readBits(U32 bitCount, void *bitPtr);

   /// Writes a ByteBuffer into the stream.  The ByteBuffer can be no larger than 1024 bytes in size.
   bool write(const ByteBuffer *theBuffer);

   /// Reads a ByteBuffer in from the stream.
   bool read(ByteBuffer *theBuffer);

   /// Writes a single boolean flag (bit) into the stream, and returns the boolean that was written.
   ///
   /// This is set up so you can do...
   ///
   /// @code
   ///   if(stream->writeFlag(foo == bar))
   ///   {
   ///     ... write other stuff ...
   ///   }
   /// @endcode
   bool writeFlag(bool val);

   /// Reads a single bit from the stream.
   ///
   /// This is set up so you can do...
   ///
   /// @code
   ///   if(stream->readFlag())
   ///   {
   ///     ... read other stuff ...
   ///   }
   /// @endcode
   bool readFlag();

   bool write(bool value) { writeFlag(value); return !error; }
   bool read(bool *value) { *value = readFlag(); return !error; }

   /// Writes a huffman compressed string into the stream.
   void writeString(const char *stringBuf, U8 maxLen=255);
   /// Reads a huffman compressed string from the stream.
   void readString(char stringBuf[256]);

   /// Writes a string table entry into the stream
   void writeStringTableEntry(const StringTableEntry &ste);

   /// Reads a string table entry from the stream
   void readStringTableEntry(StringTableEntry *ste);

   /// Writes byte data into the stream.
   bool write(const U32 in_numBytes, const void* in_pBuffer);
   /// Reads byte data from the stream.
   bool read(const U32 in_numBytes,  void* out_pBuffer);

   /// @name Various types that the BitStream can read and write...
   /// @{

   ///
   DeclareTemplatizedReadWrite(U8);
   DeclareTemplatizedReadWrite(S8);
   DeclareTemplatizedReadWrite(U16);
   DeclareTemplatizedReadWrite(S16);
   DeclareTemplatizedReadWrite(U32);
   DeclareTemplatizedReadWrite(S32);
   DeclareTemplatizedReadWrite(S64);
   DeclareTemplatizedReadWrite(U64);
   DeclareTemplatizedReadWrite(F32);
   DeclareTemplatizedReadWrite(F64);

   /// @}

   /// Sets the bit at position bitCount to the value of set
   bool setBit(U32 bitCount, bool set);
   /// Tests the value of the bit at position bitCount.
   bool testBit(U32 bitCount);

   /// Returns whether the BitStream writing has exceeded the write target size.
   bool isFull() { return bitNum > (getBufferSize() << 3); }
   /// Returns whether the stream has generated an error condition due to reading or writing past the end of the buffer.
   bool isValid() { return !error; }

   /// Hashes the BitStream, writing the hash digest into the end of the buffer, and then encrypts with the given cipher
   void hashAndEncrypt(U32 hashDigestSize, U32 encryptStartOffset, SymmetricCipher *theCipher);

   /// Decrypts the BitStream, then checks the hash digest at the end of the buffer to validate the contents
   bool decryptAndCheckHash(U32 hashDigestSize, U32 decryptStartOffset, SymmetricCipher *theCipher);
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


inline U32 BitStream::getBytePosition() const
{
   return (bitNum + 7) >> 3;
}

inline U32 BitStream::getBitPosition() const
{
   return bitNum;
}

inline void BitStream::setBytePosition(const U32 newPosition)
{
   bitNum = newPosition << 3;
}

inline void BitStream::setBitPosition(const U32 newBitPosition)
{
   bitNum = newBitPosition;
}

inline void BitStream::advanceBitPosition(const S32 numBits)
{
   setBitPosition(getBitPosition() + numBits);
}

inline void BitStream::zeroToByteBoundary()
{
   if(bitNum & 0x7)
      writeInt(0, 8 - (bitNum & 0x7));
}

inline bool BitStream::write(const U32 in_numBytes, const void* in_pBuffer)
{
   return writeBits(in_numBytes << 3, in_pBuffer);
}

inline bool BitStream::read(const U32 in_numBytes,  void* out_pBuffer)
{
   return readBits(in_numBytes << 3, out_pBuffer);
}

inline bool BitStream::readFlag()
{
   if(bitNum > maxReadBitNum)
   {
      error = true;
      TNLAssert(false, "Out of range read");
      return false;
   }
   S32 mask = 1 << (bitNum & 0x7);
   bool ret = (*(getBuffer() + (bitNum >> 3)) & mask) != 0;
   bitNum++;
   return ret;
}

inline void BitStream::writeIntAt(U32 value, U8 bitCount, U32 bitPosition)
{
   U32 curPos = getBitPosition();
   setBitPosition(bitPosition);
   writeInt(value, bitCount);
   setBitPosition(curPos);
}

inline void BitStream::writeRangedU32(U32 value, U32 rangeStart, U32 rangeEnd)
{
   TNLAssert(value >= rangeStart && value <= rangeEnd, "Out of bounds value!");
   TNLAssert(rangeEnd >= rangeStart, "error, end of range less than start");

   U32 rangeSize = rangeEnd - rangeStart + 1;
   U32 rangeBits = getNextBinLog2(rangeSize);

   writeInt(S32(value - rangeStart), S32(rangeBits));
}

inline U32 BitStream::readRangedU32(U32 rangeStart, U32 rangeEnd)
{
   TNLAssert(rangeEnd >= rangeStart, "error, end of range less than start");

   U32 rangeSize = rangeEnd - rangeStart + 1;
   U32 rangeBits = getNextBinLog2(rangeSize);

   U32 val = U32(readInt(S32(rangeBits)));
   return val + rangeStart;
}

inline void BitStream::writeEnum(U32 enumValue, U32 enumRange)
{
   writeInt(enumValue, getNextBinLog2(enumRange));
}

inline U32 BitStream::readEnum(U32 enumRange)
{
   return U32(readInt(getNextBinLog2(enumRange)));
}

/// PacketStream provides a network interface to the BitStream for easy construction of data packets.
class PacketStream : public BitStream
{
   typedef BitStream Parent;
   U8 buffer[MaxPacketDataSize]; ///< internal buffer for packet data, sized to the maximum UDP packet size.
public:
   /// Constructor assigns the internal buffer to the BitStream.
   PacketStream(U32 targetPacketSize = MaxPacketDataSize) : BitStream(buffer, targetPacketSize, MaxPacketDataSize) {}
   /// Sends this packet to the specified address through the specified socket.
   NetError sendto(Socket &outgoingSocket, const Address &theAddress);
   /// Reads a packet into the stream from the specified socket.
   NetError recvfrom(Socket &incomingSocket, Address *recvAddress);
};


};
#endif //_TNL_BITSTREAM_H_

// --- END OF FILE: tnl/tnlBitStream.h ---
// --- START OF FILE: tnl/tnlByteBuffer.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_BYTEBUFFER_H_
#define _TNL_BYTEBUFFER_H_

#ifndef _TNL_H_
#include "tnl.h"
#endif

#ifndef _TNL_NETBASE_H_
#include "tnlNetBase.h"
#endif

namespace TNL {

class ByteBuffer : public Object
{
   friend struct MethodArgList;
protected:
   /// Pointer to our data buffer.
   U8 *mDataPtr;

   /// Length of buffer.
   U32  mBufSize;

   /// Do we own the memory we are using? (If so, we should free it.)
   bool mOwnsMemory;
public:
   // Act 1, Scene 1
   // Enter: enums, constructors, with divers alarums

   // Prologue:
   enum {
      DefaultBufferSize = 1500, ///< Starting size of the buffer, varlot.
   };

   // Chorus:

   /// Create a ByteBuffer from a chunk of memory.
   ByteBuffer(U8 *dataPtr, U32 bufferSize)
   {
      mBufSize = bufferSize;
      mDataPtr = dataPtr;
      mOwnsMemory = false;
   }

   /// Create a ByteBuffer of the specified size.
   ByteBuffer(U32 bufferSize = DefaultBufferSize)
   {
      mBufSize = bufferSize;
      mDataPtr = (U8 *) malloc(bufferSize);
      mOwnsMemory = true;
   }

   /// Copy constructor!
   ByteBuffer(const ByteBuffer &theBuffer)
   {
      mBufSize = theBuffer.mBufSize;
      mDataPtr = theBuffer.mDataPtr;
      mOwnsMemory = false;
   }

   ~ByteBuffer()
   {
      if(mOwnsMemory)
         free(mDataPtr);
   }

   /// Set the ByteBuffer to point to a new chunk of memory.
   void setBuffer(U8 *dataPtr, U32 bufferSize)
   {
      if(mOwnsMemory)
      {
         mOwnsMemory = false;
         free(mDataPtr);
      }
      mDataPtr = dataPtr;
      mBufSize = bufferSize;
   }

   /// Set the ByteBuffer to point to a new chunk of memory, indicating whether
   /// it should own the chunk or not.
   void setBuffer(U8 *dataPtr, U32 bufferSize, bool newOwnsMemory)
   {
      setBuffer(dataPtr, bufferSize);
      mOwnsMemory = newOwnsMemory;
   }

   /// Attempts to resize the buffer.
   ///
   /// @returns True if it owns its own memory, false otherwise.
   bool resize(U32 newBufferSize)
   {
      if(mBufSize >= newBufferSize)
         mBufSize = newBufferSize;
      else if(mOwnsMemory)
      {
         mBufSize = newBufferSize;
         mDataPtr = (U8 *) realloc(mDataPtr, newBufferSize);
         return true;
      }
      return false;
   }

   /// Appends the specified buffer to the end of the byte buffer.
   /// returns false if it does not own its own memory.
   bool appendBuffer(const U8 *dataBuffer, U32 bufferSize)
   {
      U32 start = mBufSize;
      if(!resize(mBufSize + bufferSize))
         return false;
      memcpy(mDataPtr + start, dataBuffer, bufferSize);
      return true;
   }

   /// Appends the specified ByteBuffer to the end of this byte buffer.
   /// returns fals if it does not own its own memory.
   bool appendBuffer(const ByteBuffer &theBuffer)
   {
      return appendBuffer(theBuffer.getBuffer(), theBuffer.getBufferSize());
   }

   /// Copies the current buffer into a newly allocated buffer that the ByteBuffer owns.
   void takeOwnership()
   {
      if(!mOwnsMemory)
      {
         U8 *memPtr = (U8 *) malloc(mBufSize);
         memcpy(memPtr, mDataPtr, mBufSize);
         mDataPtr = memPtr;
         mOwnsMemory = true;
      }
   }

   /// Does this ByteBuffer own its own memory?
   bool ownsMemory()
   {
      return mOwnsMemory;
   }

   U32 getBufferSize() const
   {
      return mBufSize;
   }

   U8 *getBuffer()
   {
      return mDataPtr;
   }

   const U8 *getBuffer() const
   {
      return mDataPtr;
   }

   /// Clear the buffer.
   void clear()
   {
      memset(mDataPtr, 0, mBufSize);
   }

   /// Encode the buffer to base 64, returning the encoded buffer.
   RefPtr<ByteBuffer> encodeBase64() const;
   /// Decode the buffer from base 64, returning the decoded buffer.
   RefPtr<ByteBuffer> decodeBase64() const;

   /// Computes an MD5 hash and returns it in a ByteBuffer
   RefPtr<ByteBuffer> computeMD5Hash(U32 len = 0) const;

   /// Converts to ascii-hex, returning the encoded buffer.
   RefPtr<ByteBuffer> encodeBase16() const;
   /// Decodes the buffer from base 16, returning the decoded buffer.
   RefPtr<ByteBuffer> decodeBase16() const;

   /// Returns a 32 bit CRC for the buffer.
   U32 calculateCRC(U32 start = 0, U32 end = 0xFFFFFFFF, U32 crcVal = 0xFFFFFFFF) const;
};

typedef RefPtr<ByteBuffer> ByteBufferPtr;
typedef const ByteBuffer &ByteBufferRef;

};

#endif

// --- END OF FILE: tnl/tnlByteBuffer.h ---
// --- START OF FILE: tnl/tnlCertificate.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_CERTIFICATE_H_
#define _TNL_CERTIFICATE_H_

#ifndef _TNL_BYTEBUFFER_H_
#include "tnlByteBuffer.h"
#endif

#ifndef _TNL_ASYMMETRICKEY_H_
#include "tnlAsymmetricKey.h"
#endif

namespace TNL
{

/// The Certificate class manages a digitally signed certificate.
/// Certificates consist of an application-defined payload, a public
/// key, and a signature.  It is up to the application to determine
/// from the payload what, if any, certificate authority (CA) signed
/// the certificate.  The validate() method can be used to check the
/// certificate's authenticity against the known public key of the
/// signing Certificate Authority.
///
/// The payload could include such items as:
///  - The valid date range for the certificate
///  - The domain of the certificate holder
///  - The identifying name of the Certificate Authority
///  - A player's name and user id for a multiplayer game
///
/// 
class Certificate : public ByteBuffer
{
protected:
   RefPtr<AsymmetricKey> mPublicKey;      ///< The public key for the holder of this certificate
   ByteBufferPtr mPayload;                ///< The certificate payload, including the identity of the holder and the Certificate Authority
   ByteBufferPtr mSignature;              ///< The digital signature of this certificate by the signatory
   bool mIsValid;                         ///< flag to signify whether this certificate has a valid form
   U32 mSignatureByteSize;                ///< Number of bytes of the ByteBuffer signed by the CA
public:
   enum {
      MaxPayloadSize = 512,
   };
   /// Certificate constructor
   Certificate(U8 *dataPtr, U32 dataSize) : ByteBuffer(dataPtr, dataSize)
   {
      mSignatureByteSize = 0;
      mIsValid = false;
      takeOwnership();
      parse();
   }

   Certificate(const ByteBuffer &buffer) : ByteBuffer(buffer)
   {
      mSignatureByteSize = 0;
      mIsValid = false;
      takeOwnership();
      parse();
   }
   Certificate(BitStream *stream)
   {
      mSignatureByteSize = 0;
      mIsValid = false;
      stream->read(this);
      parse();
   }
   Certificate(const ByteBuffer &payload, RefPtr<AsymmetricKey> publicKey, RefPtr<AsymmetricKey> theCAPrivateKey);

   void parse(); ///< Parses this certificate into the payload, public key, identiy, certificate authority and signature

   /// returns the validity of the certificate's formation
   bool isValid()
   {
      return mIsValid;
   }
   /// returns true if this certificate was signed by the private key corresponding to the passed public key.
   bool validate(RefPtr<AsymmetricKey> signatoryPublicKey);

   /// Returns the public key from the certificate
   RefPtr<AsymmetricKey> getPublicKey() { return mPublicKey; }

   /// Returns the certificate payload
   ByteBufferPtr getPayload() { return mPayload; }
};

};

#endif

// --- END OF FILE: tnl/tnlCertificate.h ---
// --- START OF FILE: tnl/tnlClientPuzzle.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU
//   General Public License, alternative licensing options are available
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_CLIENTPUZZLE_H_
#define _TNL_CLIENTPUZZLE_H_

#include "tnlDataChunker.h"
#include "tnlNonce.h"

// JMQ: work around X.h header file
#if defined(TNL_OS_LINUX) && defined(Success)
#undef Success
#endif

namespace TNL {

/// The ClientPuzzleManager class issues, solves and validates client
/// puzzles for connection authentication.
class ClientPuzzleManager
{
public:
private:
   /// NonceTable manages the list of client nonces for which clients
   /// have constructed valid puzzle solutions for the current server
   /// nonce.  There are 2 nonce tables in the ClientPuzzleManager -
   /// one for the current nonce and one for the previous nonce.

   class NonceTable {
    private:
      struct Entry {
         Nonce mNonce;
         Entry *mHashNext;
      };
      enum {
         MinHashTableSize = 127,
         MaxHashTableSize = 387,
      };

      Entry **mHashTable;
      U32 mHashTableSize;
      DataChunker mChunker;

    public:
      /// NonceTable constructor
      NonceTable() { reset(); }

      /// Resets and clears the nonce table
      void reset();

      /// checks if the given nonce is already in the table and adds it
      /// if it is not.  Returns true if the nonce was not in the table
      /// when the function was called.
      bool checkAdd(Nonce &theNonce);
   };

   U32 mCurrentDifficulty;
   U32 mLastUpdateTime;
   U32 mLastTickTime;

   Nonce mCurrentNonce;
   Nonce mLastNonce;

   NonceTable *mCurrentNonceTable;
   NonceTable *mLastNonceTable;
   static bool checkOneSolution(U32 solution, Nonce &clientNonce, Nonce &serverNonce, U32 puzzleDifficulty, U32 clientIdentity);
public:
   ClientPuzzleManager();
   ~ClientPuzzleManager();

   /// Checks to see if a new nonce needs to be created, and if so
   /// generates one and tosses out the current list of accepted nonces
   void tick(U32 currentTime);

   /// Error codes that can be returned by checkSolution
   enum ErrorCode {
      Success,
      InvalidSolution,
      InvalidServerNonce,
      InvalidClientNonce,
      InvalidPuzzleDifficulty,
      ErrorCodeCount,
   };

   /// Difficulty levels of puzzles
   enum {
      PuzzleRefreshTime          = 30000, ///< Refresh the server puzzle every 30 seconds
      InitialPuzzleDifficulty    = 17, ///< Initial puzzle difficulty is set so clients do approx 2-3x the shared secret
                                       ///  generation of the server
      MaxPuzzleDifficulty        = 26, ///< Maximum puzzle difficulty is approx 1 minute to solve on ~2004 hardware.
      MaxSolutionComputeFragment = 30, ///< Number of milliseconds spent computing solution per call to solvePuzzle.
      SolutionFragmentIterations = 50000, ///< Number of attempts to spend on the client puzzle per call to solvePuzzle.
   };

   /// Checks a puzzle solution submitted by a client to see if it is a valid solution for the current or previous puzzle nonces
   ErrorCode checkSolution(U32 solution, Nonce &clientNonce, Nonce &serverNonce, U32 puzzleDifficulty, U32 clientIdentity);

   /// Computes a puzzle solution value for the given puzzle difficulty and server nonce.  If the execution time of this function
   /// exceeds MaxSolutionComputeFragment milliseconds, it will return the current trail solution in the solution variable and a
   /// return value of false.
   ///
   /// @note Although the behavior of this function can be tweaked using MaxSolutionComputeFragment and
   ///       SolutionFragmentIterations, it's important to bias these settings in favor of rapid puzzle
   ///       completion. A client puzzle is only valid for two times PuzzleRefreshTime, so for about a
   ///       minute, maximum. Most of the time the puzzle can be solved in only a few hundred
   ///       milliseconds. It's better to solve the puzzle fast than to let it drag out, (ie, it's better to
   ///       let your application hitch for a moment whilst calculating than to make the user endure many
   ///       seconds of lag) so reducing the timeout or iterations should be done only if you know what
   ///       you're doing.
   ///
   static bool solvePuzzle(U32 *solution, Nonce &clientNonce, Nonce &serverNonce, U32 puzzleDifficulty, U32 clientIdentity);

   /// Returns the current server nonce
   Nonce getCurrentNonce() { return mCurrentNonce; }

   /// Returns the current client puzzle difficulty
   U32 getCurrentDifficulty() { return mCurrentDifficulty; }
};

};

#endif

// --- END OF FILE: tnl/tnlClientPuzzle.h ---
// --- START OF FILE: tnl/tnlConnectionStringTable.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_CONNECTIONSTRINGTABLE_H_
#define _TNL_CONNECTIONSTRINGTABLE_H_

#ifndef _TNL_NETSTRINGTABLE_H_
#include "tnlNetStringTable.h"
#endif

namespace TNL {

class NetConnection;
class BitStream;

/// ConnectionStringTable is a helper class to EventConnection for reducing duplicated string data sends
class ConnectionStringTable
{
public:
   enum StringTableConstants{
      EntryCount = 1024,
      EntryBitSize = 10,
   };

   struct Entry; 

   struct PacketEntry {
      PacketEntry *nextInPacket; ///< The next string table entry updated in the packet this is linked in.
      Entry *stringTableEntry; ///< The ConnectionStringTable::Entry this refers to
      StringTableEntry string; ///< The StringTableEntry that was set in that string
   };

public:
   struct PacketList {
      PacketEntry *stringHead;  ///< The head of the linked list of strings sent in this packet.
      PacketEntry *stringTail; ///< The tail of the linked list of strings sent in this packet.

      PacketList() { stringHead = stringTail = NULL; }
   };

   /// An entry in the EventConnection's string table
   struct Entry {
      StringTableEntry string; ///< Global string table entry of this string
                           ///< will be 0 if this string is unused.
      U32 index;           ///< Index of this entry.
      Entry *nextHash;     ///< The next hash entry for this id.
      Entry *nextLink;     ///< The next entry in the LRU list.
      Entry *prevLink;     ///< The prev entry in the LRU list.

      /// Does the other side now have this string?
      bool receiveConfirmed;
   };

private:
   Entry mEntryTable[EntryCount];
   Entry *mHashTable[EntryCount];
   StringTableEntry mRemoteStringTable[EntryCount];
   Entry mLRUHead, mLRUTail;

   NetConnection *mParent;

   /// Pushes an entry to the back of the LRU list.
   inline void pushBack(Entry *entry)
   {
      entry->prevLink->nextLink = entry->nextLink;
      entry->nextLink->prevLink = entry->prevLink;
      entry->nextLink = &mLRUTail;
      entry->prevLink = mLRUTail.prevLink;
      entry->nextLink->prevLink = entry;
      entry->prevLink->nextLink = entry;
   }
public:
   ConnectionStringTable(NetConnection *parent);

   void writeStringTableEntry(BitStream *stream, StringTableEntryRef string);
   StringTableEntry readStringTableEntry(BitStream *stream);

   void packetReceived(PacketList *note);
   void packetDropped(PacketList *note);
};

};

#endif


// --- END OF FILE: tnl/tnlConnectionStringTable.h ---
// --- START OF FILE: tnl/tnlDataChunker.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_DATACHUNKER_H_
#define _TNL_DATACHUNKER_H_


//----------------------------------------------------------------------------

namespace TNL {

//----------------------------------------------------------------------------
/// Implements a chunked data allocater.
///
/// Calling new/malloc all the time is a time consuming operation. Therefore,
/// we provide the DataChunker, which allocates memory in blocks of
/// chunkSize (by default 16k, see ChunkSize, though it can be set in
/// the constructor), then doles it out as requested, in chunks of up to
/// chunkSize in size.
///
/// It will assert if you try to get more than ChunkSize bytes at a time,
/// and it deals with the logic of allocating new blocks and giving out
/// word-aligned chunks.
///
/// Note that new/free/realloc WILL NOT WORK on memory gotten from the
/// DataChunker. This also only grows (you can call freeBlocks to deallocate
/// and reset things).
class DataChunker
{
  public:
   enum {
      ChunkSize = 16376 ///< Default size of each DataBlock page in the DataChunker
   };

  private:
   /// DataBlock representation for each page of the DataChunker
   struct DataBlock
   {
      DataBlock *next;        ///< linked list pointer to the next DataBlock for this chunker
      U8 *data;               ///< allocated pointer for the base of this page
      S32 curIndex;           ///< current allocation point within this DataBlock
      DataBlock(S32 size);
      ~DataBlock();
   };
   DataBlock *curBlock;       ///< current page we're allocating data from.  If the
                              ///< data size request is greater than the memory space currently
                              ///< available in the current page, a new page will be allocated.
   S32 chunkSize;             ///< The size allocated for each page in the DataChunker
  public:
   void *alloc(S32 size);     ///< allocate a pointer to memory of size bytes from the DataChunker
   void freeBlocks();         ///< free all pages currently allocated in the DataChunker

   DataChunker(S32 size=ChunkSize); ///< Construct a DataChunker with a page size of size bytes.
   ~DataChunker();

   /// Swaps the memory allocated in one data chunker for another.  This can be used to implement
   /// packing of memory stored in a DataChunker.
   void swap(DataChunker &d)
   {
      DataBlock *temp = d.curBlock;
      d.curBlock = curBlock;
      curBlock = temp;
   }
};

//----------------------------------------------------------------------------

/// Templatized data chunker class with proper construction and destruction of its elements.
///
/// DataChunker just allocates space. This subclass actually constructs/destructs the
/// elements. This class is appropriate for more complex classes.
template<class T>
class ClassChunker: private DataChunker
{
   S32 numAllocated; ///< number of elements currently allocated through this ClassChunker
   S32 elementSize; ///< the size of each element, or the size of a pointer, whichever is greater
   T *freeListHead; ///< a pointer to a linked list of freed elements for reuse
public:
   ClassChunker(S32 size = DataChunker::ChunkSize) : DataChunker(size)
   {
      numAllocated = 0;
      elementSize = getMax(U32(sizeof(T)), U32(sizeof(T *)));
      freeListHead = NULL;
   }
   /// Allocates and properly constructs in place a new element.
   T *alloc()
   {
      numAllocated++;
      if(freeListHead == NULL)
         return constructInPlace(reinterpret_cast<T*>(DataChunker::alloc(elementSize)));
      T* ret = freeListHead;
      freeListHead = *(reinterpret_cast<T**>(freeListHead));
      return constructInPlace(ret);
   }

   /// Properly destructs and frees an element allocated with the alloc method.
   void free(T* elem)
   {
      destructInPlace(elem);
      numAllocated--;
      *(reinterpret_cast<T**>(elem)) = freeListHead;
      freeListHead = elem;
   }

   void freeBlocks()
   {
	   DataChunker::freeBlocks();
   }
};

};

#endif

// --- END OF FILE: tnl/tnlDataChunker.h ---
// --- START OF FILE: tnl/tnlEndian.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_ENDIAN_H_
#define _TNL_ENDIAN_H_

namespace TNL {

inline U8 endianSwap(const U8 in_swap)
{
   return in_swap;
}

inline S8 endianSwap(const S8 in_swap)
{
   return in_swap;
}

/**
   Convert the byte ordering on the U16 to and from big/little endian format.
   @param in_swap Any U16
   @returns swapped U16.
 */

inline U16 endianSwap(const U16 in_swap)
{
   return U16(((in_swap >> 8) & 0x00ff) |
              ((in_swap << 8) & 0xff00));
}

inline S16 endianSwap(const S16 in_swap)
{
   return S16(endianSwap(U16(in_swap)));
}

/**
   Convert the byte ordering on the U32 to and from big/little endian format.
   @param in_swap Any U32
   @returns swapped U32.
 */
inline U32 endianSwap(const U32 in_swap)
{
   return U32(((in_swap >> 24) & 0x000000ff) |
              ((in_swap >>  8) & 0x0000ff00) |
              ((in_swap <<  8) & 0x00ff0000) |
              ((in_swap << 24) & 0xff000000));
}

inline S32 endianSwap(const S32 in_swap)
{
   return S32(endianSwap(U32(in_swap)));
}

inline U64 endianSwap(const U64 in_swap)
{
   U32 *inp = (U32 *) &in_swap;
   U64 ret;
   U32 *outp = (U32 *) &ret;
   outp[0] = endianSwap(inp[1]);
   outp[1] = endianSwap(inp[0]);
   return ret;
}

inline S64 endianSwap(const S64 in_swap)
{
   return S64(endianSwap(U64(in_swap)));
}

inline F32 endianSwap(const F32 in_swap)
{
   U32 result = endianSwap(* ((U32 *) &in_swap) );
   return * ((F32 *) &result);
}

inline F64 endianSwap(const F64 in_swap)
{
   U64 result = endianSwap(* ((U64 *) &in_swap) );
   return * ((F64 *) &result);
}

//------------------------------------------------------------------------------
// Endian conversions
#ifdef TNL_LITTLE_ENDIAN

#define TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(type) \
   inline type convertHostToLEndian(type i) { return i; } \
   inline type convertLEndianToHost(type i) { return i; } \
   inline type convertHostToBEndian(type i) { return endianSwap(i); } \
   inline type convertBEndianToHost(type i) { return endianSwap(i); }

#elif defined(TNL_BIG_ENDIAN)

#define TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(type) \
   inline type convertHostToLEndian(type i) { return endianSwap(i); } \
   inline type convertLEndianToHost(type i) { return endianSwap(i); } \
   inline type convertHostToBEndian(type i) { return i; } \
   inline type convertBEndianToHost(type i) { return i; }

#else
#error "Endian define not set!"
#endif


TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(U8)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(S8)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(U16)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(S16)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(U32)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(S32)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(U64)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(S64)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(F32)
TNL_DECLARE_TEMPLATIZED_ENDIAN_CONV(F64)

};

#endif

// --- END OF FILE: tnl/tnlEndian.h ---
// --- START OF FILE: tnl/tnlEventConnection.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_EVENTCONNECTION_H_
#define _TNL_EVENTCONNECTION_H_

#ifndef _TNL_NETCONNECTION_H_
#include "tnlNetConnection.h"
#endif

#ifndef _TNL_NETEVENT_H_
#include "tnlNetEvent.h"
#endif

#ifndef _TNL_DATACHUNKER_H_
#include "tnlDataChunker.h"
#endif

namespace TNL {

/// EventConnection is a NetConnection subclass used for sending guaranteed and unguaranteed
/// event packages across a connection.
///
/// The EventConnection is responsible for transmitting NetEvents over the wire.
/// It deals with ensuring that the various types of NetEvents are delivered appropriately,
/// and with notifying the event of its delivery status.
///
/// The EventConnection is mainly accessed via postNetEvent(), which accepts NetEvents.
///
/// @see NetEvent for a more thorough explanation of how to use events.

class EventConnection : public NetConnection
{
   typedef NetConnection Parent;

   /// EventNote associates a single event posted to a connection with a sequence number for ordered processing
   struct EventNote
   {
      RefPtr<NetEvent> mEvent; ///< A safe reference to the event
      S32 mSeqCount; ///< the sequence number of this event for ordering
      EventNote *mNextEvent; ///< The next event either on the connection or on the PacketNotify
   };
public:
   /// EventPacketNotify tracks all the events sent with a single packet
   struct EventPacketNotify : public NetConnection::PacketNotify
   {
      EventNote *eventList; ///< linked list of events sent with this packet
      EventPacketNotify() { eventList = NULL; }
   };

   EventConnection();
   ~EventConnection();
protected:
   enum DebugConstants
   {
      DebugChecksum = 0xF00DBAAD,
      BitStreamPosBitSize = 16,
   };

   /// Allocates a PacketNotify for this connection
   PacketNotify *allocNotify() { return new EventPacketNotify; }

   /// Override processing to requeue any guaranteed events in the packet that was dropped
   void packetDropped(PacketNotify *notify);

   /// Override processing to notify for delivery and dereference any events sent in the packet
   void packetReceived(PacketNotify *notify);

   /// Writes pending events into the packet, and attaches them to the PacketNotify
   void writePacket(BitStream *bstream, PacketNotify *notify);

   /// Reads events from the stream, and queues them for processing
   void readPacket(BitStream *bstream);

   /// Returns true if there are events pending that should be sent across the wire
   virtual bool isDataToTransmit();

   /// Dispatches an event
   void processEvent(NetEvent *theEvent);


//----------------------------------------------------------------
// event manager functions/code:
//----------------------------------------------------------------

private:
   static ClassChunker<EventNote> mEventNoteChunker; ///< Quick memory allocator for net event notes

   EventNote *mSendEventQueueHead;          ///< Head of the list of events to be sent to the remote host
   EventNote *mSendEventQueueTail;          ///< Tail of the list of events to be sent to the remote host.  New events are tagged on to the end of this list
   EventNote *mUnorderedSendEventQueueHead; ///< Head of the list of events sent without ordering information
   EventNote *mUnorderedSendEventQueueTail; ///< Tail of the list of events sent without ordering information
   EventNote *mWaitSeqEvents;   ///< List of ordered events on the receiving host that are waiting on previous sequenced events to arrive.
   EventNote *mNotifyEventList; ///< Ordered list of events on the sending host that are waiting for receipt of processing on the client.

   S32 mNextSendEventSeq;  ///< The next sequence number for an ordered event sent through this connection
   S32 mNextRecvEventSeq;  ///< The next receive event sequence to process
   S32 mLastAckedEventSeq; ///< The last event the remote host is known to have processed

   enum {
      InvalidSendEventSeq = -1,
      FirstValidSendEventSeq = 0
   };

protected:
   U32 mEventClassCount;      ///< Number of NetEvent classes supported by this connection
   U32 mEventClassBitSize;    ///< Bit field width of NetEvent class count.
   U32 mEventClassVersion;    ///< The highest version number of events on this connection.

   /// Writes the NetEvent class count into the stream, so that the remote
   /// host can negotiate a class count for the connection
   void writeConnectRequest(BitStream *stream);

   /// Reads the NetEvent class count max that the remote host is requesting.
   /// If this host has MORE NetEvent classes declared, the mEventClassCount
   /// is set to the requested count, and is verified to lie on a boundary between versions.
   bool readConnectRequest(BitStream *stream, const char **errorString);

   /// Writes the negotiated NetEvent class count into the stream.   
   void writeConnectAccept(BitStream *stream);

   /// Reads the negotiated NetEvent class count from the stream and validates that it is on
   /// a boundary between versions.
   bool readConnectAccept(BitStream *stream, const char **errorString);
public:
   /// returns the highest event version number supported on this connection.
   U32 getEventClassVersion() { return mEventClassVersion; }

   /// Posts a NetEvent for processing on the remote host
   bool postNetEvent(NetEvent *event);
};

};

#endif

// --- END OF FILE: tnl/tnlEventConnection.h ---
// --- START OF FILE: tnl/tnlGhostConnection.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_GHOSTCONNECTION_H_
#define _TNL_GHOSTCONNECTION_H_

#ifndef _TNL_EVENTCONNECTION_H_
#include "tnlEventConnection.h"
#endif

#ifndef _TNL_RPC_H_
#include "tnlRPC.h"
#endif

namespace TNL {

struct GhostInfo;

/// GhostConnection is a subclass of EventConnection that manages the transmission
/// (ghosting) and updating of NetObjects over a connection.
///
/// The GhostConnection is responsible for doing scoping calculations (on the server side)
/// and transmitting most-recent ghost information to the client.
///
/// Ghosting is the most complex, and most powerful, part of TNL's capabilities. It
/// allows the information sent to clients to be very precisely matched to what they need, so that
/// no excess bandwidth is wasted.  Each GhostConnection has a <b>scope object</b> that is responsible
/// for determining what other NetObject instances are relevant to that connection's client.  Each time
/// GhostConnection sends a packet, NetObject::performScopeQuery() is called on the scope object, which
/// calls GhostConnection::objectInScope() for each relevant object.
///
/// Each object that is in scope, and in need of update (based on its maskbits) is given a priority
/// ranking by calling that object's getUpdatePriority() method.  The packet is then filled with
/// updates, ordered by priority. This way the most important updates get through first, with less
/// important updates being sent as space is available.
///
/// There is a cap on the maximum number of ghosts that can be active through a GhostConnection at once.
/// The enum GhostIdBitSize (defaults to 10) determines how many bits will be used to transmit the ID for
/// each ghost, so the maximum number is 2^GhostIdBitSize or 1024.  This can be easily raised; see the
/// GhostConstants enum.
///
/// Each object ghosted is assigned a ghost ID; the client is <b>only</b> aware of the ghost ID. This acts
/// to enhance simulation security, as it becomes difficult to map objects from one connection to another,
/// or to reliably identify objects from ID alone. IDs are also reassigned based on need, making it hard
/// to track objects that have fallen out of scope (as any object which the player shouldn't see would).
///
/// resolveGhost() is used on the client side, and resolveObjectFromGhostIndex() on the server side, to
/// convert ghost IDs to object references.
///
/// @see NetObject for more information on network object functionality.
class GhostConnection : public EventConnection
{
   typedef EventConnection Parent;
   friend class ConnectionMessageEvent;
public:
   /// GhostRef tracks an update sent in one packet for the ghost of one NetObject.
   ///
   /// When we are notified that a pack is sent/lost, this is used to determine what
   /// updates need to be resent and so forth.
   struct GhostRef
   {
      U32 mask;              ///< The mask of bits that were updated in this packet
      U32 ghostInfoFlags;    ///< GhostInfo::Flags bitset, determes if the ghost is in a
                             ///  special processing mode (created/deleted)
      GhostInfo *ghost;      ///< The ghost information for the object on the connection that sent
                             ///  the packet this GhostRef is attached to
      GhostRef *nextRef;     ///< The next ghost updated in this packet
      GhostRef *updateChain; ///< A pointer to the GhostRef on the least previous packet that
                             ///  updated this ghost, or NULL, if no prior packet updated this ghost
   };

   /// Notify structure attached to each packet with information about the ghost updates in the packet
   struct GhostPacketNotify : public EventConnection::EventPacketNotify
   {
      GhostRef *ghostList; ///< list of ghosts updated in this packet
      GhostPacketNotify() { ghostList = NULL; }
   };

protected:

   /// Override of EventConnection's allocNotify, to use the GhostPacketNotify structure.
   PacketNotify *allocNotify() { return new GhostPacketNotify; }

   /// Override to properly update the GhostInfo's for all ghosts that had upates in the dropped packet.
   void packetDropped(PacketNotify *notify);

   /// Override to update flags associated with the ghosts updated in this packet.
   void packetReceived(PacketNotify *notify);

   /// Performs the scoping query in order to determine if there is data to send from this GhostConnection.
   void prepareWritePacket();

   /// Override to write ghost updates into each packet.
   void writePacket(BitStream *bstream, PacketNotify *notify);

   /// Override to read updated ghost information from the packet stream.
   void readPacket(BitStream *bstream);

   /// Override to check if there is data pending on this GhostConnection.
   bool isDataToTransmit();

//----------------------------------------------------------------
// ghost manager functions/code:
//----------------------------------------------------------------

protected:
   GhostInfo **mGhostArray;   ///< Array of GhostInfo structures used to track all the objects ghosted by this side of the connection.
                              ///
                              ///  For efficiency, ghosts are stored in three segments - the first segment contains GhostInfos
                              ///  that have pending updates, the second ghostrefs that need no updating, and last, free
                              ///  GhostInfos that may be reused.

   S32 mGhostZeroUpdateIndex; ///< Index in mGhostArray of first ghost with 0 update mask (ie, with no updates).
   S32 mGhostFreeIndex;       ///< index in mGhostArray of first free ghost.

   bool mGhosting;         ///< Am I currently ghosting objects over?
   bool mScoping;          ///< Am I currently allowing objects to be scoped?
   U32  mGhostingSequence; ///< Sequence number describing this ghosting session.

   NetObject **mLocalGhosts;        ///< Local ghost array for remote objects, or NULL if mGhostTo is false.

   GhostInfo *mGhostRefs;           ///< Allocated array of ghostInfos, or NULL if mGhostFrom is false.
   GhostInfo **mGhostLookupTable;   ///< Table indexed by object id->GhostInfo, or NULL if mGhostFrom is false.

   SafePtr<NetObject> mScopeObject; ///< The local NetObject that performs scoping queries to determine what
                                    ///  objects to ghost to the client.

   void clearGhostInfo();
   void deleteLocalGhosts();
   bool validateGhostArray();

   void freeGhostInfo(GhostInfo *);

   /// Notifies subclasses that the remote host is about to start ghosting objects.
   virtual void onStartGhosting();                              

   /// Notifies subclasses that the server has stopped ghosting objects on this connection.
   virtual void onEndGhosting();

public:
   GhostConnection();
   ~GhostConnection();

   void setGhostFrom(bool ghostFrom); ///< Sets whether ghosts transmit from this side of the connection.
   void setGhostTo(bool ghostTo);     ///< Sets whether ghosts are allowed from the other side of the connection.

   bool doesGhostFrom() { return mGhostArray != NULL; } ///< Does this GhostConnection ghost NetObjects to the remote host?
   bool doesGhostTo() { return mLocalGhosts != NULL; }  ///< Does this GhostConnection receive ghosts from the remote host?

   /// Returns the sequence number of this ghosting session.
   U32 getGhostingSequence() { return mGhostingSequence; }

   enum GhostConstants {
      GhostIdBitSize = 10,            ///< Size, in bits, of the integer used to transmit ghost IDs
      GhostLookupTableSizeShift = 10, ///< The size of the hash table used to lookup source NetObjects by remote ghost ID is 1 << GhostLookupTableSizeShift.

      MaxGhostCount = (1 << GhostIdBitSize),   ///< Maximum number of ghosts that can be active at any one time.
      GhostCountBitSize = GhostIdBitSize + 1,  ///< Size of the field needed to transmit the total number of ghosts.

      GhostLookupTableSize = (1 << GhostLookupTableSizeShift), ///< Size of the hash table used to lookup source NetObjects by remote ghost ID.
      GhostLookupTableMask = (GhostLookupTableSize - 1),       ///< Hashing mask for table lookups.

   };

   void setScopeObject(NetObject *object);                           ///< Sets the object that is queried at each packet to determine
                                                                     ///  what NetObjects should be ghosted on this connection.
   NetObject *getScopeObject() { return (NetObject*)mScopeObject; }; ///< Returns the current scope object.

   void objectInScope(NetObject *object);          ///< Indicate that the specified object is currently in scope.
                                                   ///
                                                   ///  Method called by the scope object to indicate that the specified object is in scope.
   void objectLocalScopeAlways(NetObject *object); ///< The specified object should be always in scope for this connection.
   void objectLocalClearAlways(NetObject *object); ///< The specified object should not be always in scope for this connection.

   NetObject *resolveGhost(S32 id);                  ///< Given an object's ghost id, returns the ghost of the object (on the client side).
   NetObject *resolveGhostParent(S32 id);            ///< Given an object's ghost id, returns the source object (on the server side).
   void ghostPushNonZero(GhostInfo *gi);             ///< Moves the specified GhostInfo into the range of the ghost array for non-zero updateMasks.
   void ghostPushToZero(GhostInfo *gi);              ///< Moves the specified GhostInfo into the range of the ghost array for zero updateMasks.
   void ghostPushZeroToFree(GhostInfo *gi);          ///< Moves the specified GhostInfo into the range of the ghost array for free (unused) GhostInfos.
   inline void ghostPushFreeToZero(GhostInfo *info); ///< Moves the specified GhostInfo from the free area into the range of the ghost array for zero updateMasks.

   S32 getGhostIndex(NetObject *object); ///< Returns the client-side ghostIndex of the specified server object, or -1 if the object is not available on the client.

   /// Returns true if the object is available on the client.
   bool isGhostAvailable(NetObject *object) { return getGhostIndex(object) != -1; }

   void resetGhosting();                   ///< Stops ghosting objects from this GhostConnection to the remote host, which causes all ghosts to be destroyed on the client.
   void activateGhosting();                ///< Begins ghosting objects from this GhostConnection to the remote host, starting with the GhostAlways objects.
   bool isGhosting() { return mGhosting; } ///< Returns true if this connection is currently ghosting objects to the remote host.

   void detachObject(GhostInfo *info);                      ///< Notifies the GhostConnection that the specified GhostInfo should no longer be scoped to the client.

   /// RPC from server to client before the GhostAlwaysObjects are transmitted
   TNL_DECLARE_RPC(rpcStartGhosting, (U32 sequence));

   /// RPC from client to server sent when the client receives the rpcGhostAlwaysActivated
   TNL_DECLARE_RPC(rpcReadyForNormalGhosts, (U32 sequence));

   /// RPC from server to client sent to notify that ghosting should stop
   TNL_DECLARE_RPC(rpcEndGhosting, ());
};

//----------------------------------------------------------------------------

/// Each GhostInfo structure tracks the state of a single NetObject's ghost for a single GhostConnection.
struct GhostInfo
{
   // NOTE:
   // if the size of this structure changes, the
   // NetConnection::getGhostIndex function MUST be changed
   // to reflect.

   NetObject *obj; ///< The real object on the server.
   U32 updateMask; ///< The current out-of-date state mask for the object for this connection.
   GhostConnection::GhostRef *lastUpdateChain; ///< The GhostRef for this object in the last packet it was updated in,
                                               ///   or NULL if that last packet has been notified yet.
   GhostInfo *nextObjectRef;  ///< Next GhostInfo for this object in the doubly linked list of GhostInfos across
                              ///  all connections that scope this object
   GhostInfo *prevObjectRef;  ///< Previous GhostInfo for this object in the doubly linked list of GhostInfos across
                              ///  all connections that scope this object

   GhostConnection *connection; ///< The connection that owns this GhostInfo
   GhostInfo *nextLookupInfo;   ///< Next GhostInfo in the hash table for NetObject*->GhostInfo*
   U32 updateSkipCount;         ///< How many times this object has NOT been updated in writePacket

   U32 flags;      ///< Current flag status of this object for this connection.
   F32 priority;   ///< Priority for the update of this object, computed after the scoping process has run.
   U32 index;      ///< Fixed index of the object in the mGhostRefs array for the connection, and the ghostId of the object on the client.
   S32 arrayIndex; ///< Position of the object in the mGhostArray for the connection, which changes as the object is pushed to zero, non-zero and free.

    enum Flags
    {
      InScope = BIT(0),             ///< This GhostInfo's NetObject is currently in scope for this connection.
      ScopeLocalAlways = BIT(1),    ///< This GhostInfo's NetObject is always in scope for this connection.
      NotYetGhosted = BIT(2),       ///< This GhostInfo's NetObject has not been sent to or constructed on the remote host.
      Ghosting = BIT(3),            ///< This GhostInfo's NetObject has been sent to the client, but the packet it was sent in hasn't been acked yet.
      KillGhost = BIT(4),           ///< The ghost of this GhostInfo's NetObject should be destroyed ASAP.
      KillingGhost = BIT(5),        ///< The ghost of this GhostInfo's NetObject is in the process of being destroyed.

      /// Flag mask - if any of these are set, the object is not yet available for ghost ID lookup.
      NotAvailable = (NotYetGhosted | Ghosting | KillGhost | KillingGhost),
    };
};


inline void GhostConnection::ghostPushNonZero(GhostInfo *info)
{
   TNLAssert(info->arrayIndex >= mGhostZeroUpdateIndex && info->arrayIndex < mGhostFreeIndex, "Out of range arrayIndex.");
   TNLAssert(mGhostArray[info->arrayIndex] == info, "Invalid array object.");
   if(info->arrayIndex != mGhostZeroUpdateIndex)
   {
      mGhostArray[mGhostZeroUpdateIndex]->arrayIndex = info->arrayIndex;
      mGhostArray[info->arrayIndex] = mGhostArray[mGhostZeroUpdateIndex];
      mGhostArray[mGhostZeroUpdateIndex] = info;
      info->arrayIndex = mGhostZeroUpdateIndex;
   }
   mGhostZeroUpdateIndex++;
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

inline void GhostConnection::ghostPushToZero(GhostInfo *info)
{
   TNLAssert(info->arrayIndex < mGhostZeroUpdateIndex, "Out of range arrayIndex.");
   TNLAssert(mGhostArray[info->arrayIndex] == info, "Invalid array object.");
   mGhostZeroUpdateIndex--;
   if(info->arrayIndex != mGhostZeroUpdateIndex)
   {
      mGhostArray[mGhostZeroUpdateIndex]->arrayIndex = info->arrayIndex;
      mGhostArray[info->arrayIndex] = mGhostArray[mGhostZeroUpdateIndex];
      mGhostArray[mGhostZeroUpdateIndex] = info;
      info->arrayIndex = mGhostZeroUpdateIndex;
   }
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

inline void GhostConnection::ghostPushZeroToFree(GhostInfo *info)
{
   TNLAssert(info->arrayIndex >= mGhostZeroUpdateIndex && info->arrayIndex < mGhostFreeIndex, "Out of range arrayIndex.");
   TNLAssert(mGhostArray[info->arrayIndex] == info, "Invalid array object.");
   mGhostFreeIndex--;
   if(info->arrayIndex != mGhostFreeIndex)
   {
      mGhostArray[mGhostFreeIndex]->arrayIndex = info->arrayIndex;
      mGhostArray[info->arrayIndex] = mGhostArray[mGhostFreeIndex];
      mGhostArray[mGhostFreeIndex] = info;
      info->arrayIndex = mGhostFreeIndex;
   }
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

inline void GhostConnection::ghostPushFreeToZero(GhostInfo *info)
{
   TNLAssert(info->arrayIndex >= mGhostFreeIndex, "Out of range arrayIndex.");
   TNLAssert(mGhostArray[info->arrayIndex] == info, "Invalid array object.");
   if(info->arrayIndex != mGhostFreeIndex)
   {
      mGhostArray[mGhostFreeIndex]->arrayIndex = info->arrayIndex;
      mGhostArray[info->arrayIndex] = mGhostArray[mGhostFreeIndex];
      mGhostArray[mGhostFreeIndex] = info;
      info->arrayIndex = mGhostFreeIndex;
   }
   mGhostFreeIndex++;
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

};

#endif

// --- END OF FILE: tnl/tnlGhostConnection.h ---
// --- START OF FILE: tnl/tnlHuffmanStringProcessor.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_HUFFMANSTRINGPROCESSOR_H_
#define _TNL_HUFFMANSTRINGPROCESSOR_H_

namespace TNL {

/// HuffmanStringProcessor does Huffman coding on strings written into
/// BitStream objects.
namespace HuffmanStringProcessor
{
   /// Reads a Huffman compressed string out of a BitStream.
   bool readHuffBuffer(BitStream* pStream, char* out_pBuffer);

   /// Writes and Huffman compresses a string into a BitStream.
   ///
   /// @param   pStream         Stream to output compressed data to.
   /// @param   out_pBuffer     String to compress.
   /// @param   maxLen          Maximum length of the string. If string length
   ///                          exceeds this, then the string is terminated
   ///                          early.
   ///
   /// @note The Huffman encoder uses BitStream::writeString as a fallback.
   ///       WriteString can only write strings of up to 255 characters length.
   ///       Therefore, it is wise not to exceed that limit.
   bool writeHuffBuffer(BitStream* pStream, const char* out_pBuffer, U32 maxLen);
};

};

#endif

// --- END OF FILE: tnl/tnlHuffmanStringProcessor.h ---
// --- START OF FILE: tnl/tnlJournal.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_JOURNAL_H_
#define _TNL_JOURNAL_H_

#include "tnlMethodDispatch.h"
#include <stdio.h>

#define TNL_ENABLE_JOURNALING
//#define TNL_ENABLE_BIG_JOURNALS
namespace TNL
{

/// The Journal class represents the recordable entry point(s) into program execution.
/// When journaling is enabled by the TNL_ENABLE_JOURNALING macro, any calls into specially
/// marked Journal methods will be intercepted and potentially recorded for later playback.
/// If TNL_ENABLE_JOURNALING is not defined, all of the interception code will be disabled.

class Journal : public Object
{
   static FILE *mJournalFile;
   static BitStream mReadStream;
   static BitStream mWriteStream;
   static Journal *mJournal;
   static U32 mWritePosition;
   static U32 mReadBreakBitPos;
   static U32 mBreakBlockIndex;
   static U32 mBlockIndex;
public:
   enum Mode
   {
      Inactive,
      Record,
      Playback,
   };
protected:
   static Mode mCurrentMode;
   static bool mInsideEntrypoint;
   static void checkReadPosition();
   static void syncWriteStream();
public:
   Journal();
   void record(const char *fileName);
   void load(const char *fileName);

   void callEntry(const char *funcName, Functor *theCall);
   void processNextJournalEntry();

   static Mode getCurrentMode() { return mCurrentMode; }
   static Journal *get() { return mJournal; }
   static BitStream *getReadStream() { return &mReadStream; }
   static BitStream *getWriteStream() { return &mWriteStream; }
   static bool isInEntrypoint() { return mInsideEntrypoint; }

   static void beginBlock(U32 blockId, bool writeBlock);
   static void endBlock(U32 blockId, bool writeBlock);
};

struct JournalEntryRecord
{
   U32 mEntryIndex;
   const char *mFunctionName;
   JournalEntryRecord *mNext;
   Functor *mFunctor;

   static Vector<JournalEntryRecord *> *mEntryVector;

   JournalEntryRecord(const char *functionName);
   virtual ~JournalEntryRecord();
};

#ifdef TNL_ENABLE_JOURNALING
#define TNL_DECLARE_JOURNAL_ENTRYPOINT(func, args) \
      void func args; \
      void func##_body args

#define TNL_IMPLEMENT_JOURNAL_ENTRYPOINT(className, func, args, argNames) \
      struct Journal_##className##_##func##_er : public JournalEntryRecord { \
      FunctorDecl<void (className::*) args> mFunctorDecl; \
      Journal_##className##_##func##_er(const char *name) : JournalEntryRecord(name), mFunctorDecl(&className::func##_body) { mFunctor = &mFunctorDecl; } \
      } gJournal_##className##_##func##_er(#func); \
      void className::func args { \
      gJournal_##className##_##func##_er.mFunctorDecl.set argNames; \
         callEntry(#func, gJournal_##className##_##func##_er.mFunctor); \
      } \
      void className::func##_body args

class JournalToken
{
   bool mWriting;
   U32 mBlockType;
public:
   JournalToken(U32 blockType, bool writing)
   {
      mWriting = writing;
      mBlockType = blockType;
      TNL::Journal::beginBlock(mBlockType, mWriting);
   }

   ~JournalToken()
   {
      TNL::Journal::endBlock(mBlockType, mWriting);
   }
};

class JournalBlockTypeToken
{
   const char *mString;
   U32 mValue;
   JournalBlockTypeToken *mNext;
   static bool mInitialized;
   static JournalBlockTypeToken *mList;
public:
   JournalBlockTypeToken(const char *typeString);
   U32 getValue();
   static const char *findName(U32 value);
   const char *getString() { return mString; }
};

#define TNL_JOURNAL_WRITE_BLOCK(blockType, x) \
{ \
   if(TNL::Journal::getCurrentMode() == TNL::Journal::Record && TNL::Journal::isInEntrypoint()) \
   { \
      static TNL::JournalBlockTypeToken typeToken(#blockType);\
      TNL::JournalToken dummy(typeToken.getValue(), true); \
      { \
      x \
      } \
   } \
}

#define TNL_JOURNAL_READ_BLOCK(blockType, x) \
{ \
   if(TNL::Journal::getCurrentMode() == TNL::Journal::Playback && TNL::Journal::isInEntrypoint()) \
   { \
      static TNL::JournalBlockTypeToken typeToken(#blockType);\
      TNL::JournalToken dummy(typeToken.getValue(), false); \
      { \
      x \
      } \
   } \
}

#define TNL_JOURNAL_READ(x) \
   TNL::Journal::getReadStream()->read x

#define TNL_JOURNAL_WRITE(x) \
   TNL::Journal::getWriteStream()->write x

#else
#define TNL_DECLARE_JOURNAL_ENTRYPOINT(func, args) \
      void func args

#define TNL_IMPLEMENT_JOURNAL_ENTRYPOINT(className, func, args) \
   void className::func args

#define TNL_JOURNAL_WRITE_BLOCK(blockType, x)
#define TNL_JOURNAL_READ_BLOCK(blockType, x)

#endif
};


#endif


// --- END OF FILE: tnl/tnlJournal.h ---
// --- START OF FILE: tnl/tnlLog.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_LOG_H_
#define _TNL_LOG_H_

#include "tnlBitSet.h"
#include "tnl.h"

namespace TNL
{

/// Global interface to the TNL logging system.
///
/// The TNL logging system is consumer based. This is just a
/// global stub that routes the log string to all the consumers.
///
/// You should <b>really</b> be using the TNLLogMessage() and
/// TNLLogMessageV() macros instead of calling this.
///
/// @see LogConsumer
extern void logprintf(const char *format, ...);


/// LogConsumer is the base class for the message logging system in TNL.
///
/// TNL by default doesn't log messages anywhere, but users of the library
/// can instantiate subclasses that override the logString method.
/// Any instantiated subclass of LogConsumer will receive all general
/// logprintf's, as well as any TNLLogMessages that are enabled via
/// the TNLLogEnable macro.
class LogConsumer
{
   LogConsumer *mNextConsumer; ///< Next LogConsumer in the global linked list of log consumers.
   LogConsumer *mPrevConsumer; ///< Previous LogConsumer in the global linked list of log consumers.

   static LogConsumer *mLinkedList;       ///< Head of the global linked list of log consumers.

public:
   /// Constructor adds this LogConsumer to the global linked list.
   LogConsumer();

   /// Destructor removes this LogConsumer from the global linked list, and updates the log flags.
   virtual ~LogConsumer();

   /// Returns the head of the linked list of all log consumers.
   static LogConsumer *getLinkedList() { return mLinkedList; }

   /// Returns the next LogConsumer in the linked list.
   LogConsumer *getNext() { return mNextConsumer; }

   /// Writes a string to this instance of LogConsumer.
   ///
   /// By default the string is sent to the Platform::outputDebugString function. Subclasses
   /// might log to a file, a remote service, or even a message box.
   virtual void logString(const char *string);
};

struct LogType
{
   LogType *next;
   static LogType *linkedList;

   bool isEnabled;
   const char *typeName;
   static LogType *current;

#ifdef TNL_ENABLE_LOGGING
   static void setCurrent(LogType *theType) { current = theType; }
   static LogType *find(const char *name);
#endif
};

#ifdef TNL_ENABLE_LOGGING

struct LogTypeRef
{
   LogType *theLogType;
   LogTypeRef(const char *name)
   {
      theLogType = LogType::find(name);
   }
};

///   LogConnectionProtocol,
///   LogNetConnection,
///   LogEventConnection,
///   LogGhostConnection,
///   LogNetInterface,
///   LogPlatform,

/// Logs a message of the specified type to the currently active LogConsumers.
#define TNLLogMessage(logType, message) { static TNL::LogTypeRef theType(#logType); if(theType.theLogType->isEnabled) { TNL::LogType::setCurrent(theType.theLogType); logprintf("%s", message); TNL::LogType::setCurrent(NULL); } }

/// Logs a printf-style variable argument message of the specified type to the currently active LogConsumers.
#define TNLLogMessageV(logType, message) { static TNL::LogTypeRef theType(#logType); if(theType.theLogType->isEnabled) { TNL::LogType::setCurrent(theType.theLogType); logprintf message; TNL::LogType::setCurrent(NULL); } }

#define TNLLogEnable(logType, enabled) { static TNL::LogTypeRef theType(#logType); theType.theLogType->isEnabled = enabled; }

#define TNLLogBlock(logType, code) { static TNL::LogTypeRef theType(#logType); if(theType.theLogType->isEnabled) { TNL::LogType::setCurrent(theType.theLogType); { code } TNL::LogType::setCurrent(NULL); } }

#else
#define TNLLogMessage(logType, message)  { }
#define TNLLogMessageV(logType, message) { }
#define TNLLogEnable(logType, enabled) { }
#define TNLLogBlock(logType, code) { }
#endif

};

#endif

// --- END OF FILE: tnl/tnlLog.h ---
// --- START OF FILE: tnl/tnlMethodDispatch.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_METHODDISPATCH_H_
#define _TNL_METHODDISPATCH_H_

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

#ifndef _TNL_VECTOR_H_
#include "tnlVector.h"
#endif

#ifndef _TNL_BITSTREAM_H_
#include "tnlBitStream.h"
#endif

#ifndef _TNL_NETSTRINGTABLE_H_
#include "tnlNetStringTable.h"
#endif

#ifndef _TNL_STRING_H_
#include "tnlString.h"
#endif

namespace Types
{
   enum {
      VectorSizeBitSize = 8,
      ByteBufferSizeBitSize = 10,
   };

   /// Reads a string from a BitStream.
   extern void read(TNL::BitStream &s, TNL::StringPtr *val);
   /// Rrites a string into a BitStream.
   extern void write(TNL::BitStream &s, TNL::StringPtr &val);
   /// Reads a ByteBuffer from a BitStream.
   extern void read(TNL::BitStream &s, TNL::ByteBufferPtr *val);
   /// Writes a ByteBuffer into a BitStream.
   extern void write(TNL::BitStream &s, TNL::ByteBufferPtr &val);
   /// Reads an IP address from a BitStream.
   extern void read(TNL::BitStream &s, TNL::IPAddress *val);
   /// Writes an IP address into a BitStream.
   extern void write(TNL::BitStream &s, TNL::IPAddress &val);

   /// Reads a StringTableEntry from a BitStream.
   inline void read(TNL::BitStream &s, TNL::StringTableEntry *val)
   {
      s.readStringTableEntry(val);
   }
   /// Writes a StringTableEntry into a BitStream.
   inline void write(TNL::BitStream &s, TNL::StringTableEntry &val)
   {
      s.writeStringTableEntry(val);
   }

   /// Reads a generic object from a BitStream.  This can be used for any
   /// type supported by BitStream::read.
   template <typename T> inline void read(TNL::BitStream &s, T *val)
   { 
      s.read(val);
   }
   /// Writes a generic object into a BitStream.  This can be used for any
   /// type supported by BitStream::write.
   template <typename T> inline void write(TNL::BitStream &s, T &val)
   { 
      s.write(val);
   }
   /// Reads a Vector of objects from a BitStream.
   template <typename T> inline void read(TNL::BitStream &s, TNL::Vector<T> *val)
   {
      TNL::U32 size = s.readInt(VectorSizeBitSize);
      val->setSize(size);
      for(TNL::S32 i = 0; i < val->size(); i++)
         read(s, &((*val)[i]));
   }
   /// Writes a Vector of objects into a BitStream.
   template <typename T> void write(TNL::BitStream &s, TNL::Vector<T> &val)
   {
      s.writeInt(val.size(), VectorSizeBitSize);
      for(TNL::S32 i = 0; i < val.size(); i++)
         write(s, val[i]);
   }
   /// Reads a bit-compressed integer from a BitStream.
   template <TNL::U32 BitCount> inline void read(TNL::BitStream &s, TNL::Int<BitCount> *val)
   {
      val->value = s.readInt(BitCount);
   }
   /// Writes a bit-compressed integer into a BitStream.
   template <TNL::U32 BitCount> inline void write(TNL::BitStream &s,TNL::Int<BitCount> &val)
   {
      s.writeInt(val.value, BitCount);
   }

   /// Reads a bit-compressed signed integer from a BitStream.
   template <TNL::U32 BitCount> inline void read(TNL::BitStream &s, TNL::SignedInt<BitCount> *val)
   {
      val->value = s.readSignedInt(BitCount);
   }
   /// Writes a bit-compressed signed integer into a BitStream.
   template <TNL::U32 BitCount> inline void write(TNL::BitStream &s,TNL::SignedInt<BitCount> &val)
   {
      s.writeSignedInt(val.value, BitCount);
   }

   /// Reads a bit-compressed RangedU32 from a BitStream.
   template <TNL::U32 MinValue, TNL::U32 MaxValue> inline void read(TNL::BitStream &s, TNL::RangedU32<MinValue,MaxValue> *val)
   {
      val->value = s.readRangedU32(MinValue, MaxValue);
   }

   /// Writes a bit-compressed RangedU32 into a BitStream.
   template <TNL::U32 MinValue, TNL::U32 MaxValue> inline void write(TNL::BitStream &s,TNL::RangedU32<MinValue,MaxValue> &val)
   {
      s.writeRangedU32(val.value, MinValue, MaxValue);
   }

   /// Reads a bit-compressed SignedFloat (-1 to 1) from a BitStream.
   template <TNL::U32 BitCount> inline void read(TNL::BitStream &s, TNL::Float<BitCount> *val)
   {
      val->value = s.readFloat(BitCount);
   }
   /// Writes a bit-compressed SignedFloat (-1 to 1) into a BitStream.
   template <TNL::U32 BitCount> inline void write(TNL::BitStream &s,TNL::Float<BitCount> &val)
   {
      s.writeFloat(val.value, BitCount);
   }
   /// Reads a bit-compressed Float (0 to 1) from a BitStream.
   template <TNL::U32 BitCount> inline void read(TNL::BitStream &s, TNL::SignedFloat<BitCount> *val)
   {
      val->value = s.readSignedFloat(BitCount);
   }
   /// Writes a bit-compressed Float (0 to 1) into a BitStream.
   template <TNL::U32 BitCount> inline void write(TNL::BitStream &s,TNL::SignedFloat<BitCount> &val)
   {
      s.writeSignedFloat(val.value, BitCount);
   }
};

namespace TNL {

/// Base class for FunctorDecl template classes.  The Functor objects
/// store the parameters and member function pointer for the invocation
/// of some class member function.  Functor is used in TNL by the
/// RPC mechanism, the journaling system and the ThreadQueue to store
/// a function for later transmission and dispatch, either to a remote
/// host, a journal file, or another thread in the process.
struct Functor {
   /// Construct the Functor.
   Functor() {}
   /// Destruct the Functor.
   virtual ~Functor() {}
   /// Reads this Functor from a BitStream.
   virtual void read(BitStream &stream) = 0;
   /// Writes this Functor to a BitStream.
   virtual void write(BitStream &stream) = 0;
   /// Dispatch the function represented by the Functor.
   virtual void dispatch(void *t) = 0;
};

/// FunctorDecl template class.  This class is specialized based on the
/// member function call signature of the method it represents.  Other
/// specializations hold specific member function pointers and slots
/// for each of the function arguments.
template <class T> 
struct FunctorDecl : public Functor {
   FunctorDecl() {}
   void set() {}
   void read(BitStream &stream) {}
   void write(BitStream &stream) {}
   void dispatch(void *t) { }
};
template <class T> 
struct FunctorDecl<void (T::*)()> : public Functor {
   typedef void (T::*FuncPtr)();
   FuncPtr ptr;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set() {}
   void read(BitStream &stream) {}
   void write(BitStream &stream) {}
   void dispatch(void *t) { ((T *)t->*ptr)(); }
};
template <class T, class A> 
struct FunctorDecl<void (T::*)(A)> : public Functor {
   typedef void (T::*FuncPtr)(A);
   FuncPtr ptr; A a;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a) { a = _a; }
   void read(BitStream &stream) { Types::read(stream, &a); }
   void write(BitStream &stream) { Types::write(stream, a); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a); }
};
template <class T, class A, class B>
struct FunctorDecl<void (T::*)(A,B)>: public Functor {
   typedef void (T::*FuncPtr)(A,B);
   FuncPtr ptr; A a; B b;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b) { a = _a; b = _b;}
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b); }
};

template <class T, class A, class B, class C>
struct FunctorDecl<void (T::*)(A,B,C)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C);
   FuncPtr ptr; A a; B b; C c;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c) { a = _a; b = _b; c = _c;}
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c); }
};

template <class T, class A, class B, class C, class D>
struct FunctorDecl<void (T::*)(A,B,C,D)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D);
   FuncPtr ptr; A a; B b; C c; D d;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d) { a = _a; b = _b; c = _c; d = _d; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d); }
};

template <class T, class A, class B, class C, class D, class E>
struct FunctorDecl<void (T::*)(A,B,C,D,E)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D,E);
   FuncPtr ptr; A a; B b; C c; D d; E e;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d, E &_e) { a = _a; b = _b; c = _c; d = _d; e = _e; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); Types::read(stream, &e); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); Types::write(stream, e); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e); }
};

template <class T, class A, class B, class C, class D, class E, class F>
struct FunctorDecl<void (T::*)(A,B,C,D,E,F)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D,E,F);
   FuncPtr ptr; A a; B b; C c; D d; E e; F f;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d, E &_e, F &_f) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); Types::read(stream, &e); Types::read(stream, &f); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); Types::write(stream, e); Types::write(stream, f); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G>
struct FunctorDecl<void (T::*)(A,B,C,D,E,F,G)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D,E,F,G);
   FuncPtr ptr; A a; B b; C c; D d; E e; F f; G g;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d, E &_e, F &_f, G &_g) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); Types::read(stream, &e); Types::read(stream, &f); Types::read(stream, &g); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); Types::write(stream, e); Types::write(stream, f); Types::write(stream, g); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G, class H>
struct FunctorDecl<void (T::*)(A,B,C,D,E,F,G,H)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D,E,F,G,H);
   FuncPtr ptr; A a; B b; C c; D d; E e; F f; G g; H h;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d, E &_e, F &_f, G &_g, H &_h) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; h = _h; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); Types::read(stream, &e); Types::read(stream, &f); Types::read(stream, &g); Types::read(stream, &h); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); Types::write(stream, e); Types::write(stream, f); Types::write(stream, g); Types::write(stream, h); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g, h); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G, class H, class I>
struct FunctorDecl<void (T::*)(A,B,C,D,E,F,G,H,I)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D,E,F,G,H,I);
   FuncPtr ptr; A a; B b; C c; D d; E e; F f; G g; H h; I i;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d, E &_e, F &_f, G &_g, H &_h, I &_i) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; h = _h; i = _i; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); Types::read(stream, &e); Types::read(stream, &f); Types::read(stream, &g); Types::read(stream, &h); Types::read(stream, &i); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); Types::write(stream, e); Types::write(stream, f); Types::write(stream, g); Types::write(stream, h); Types::write(stream, i); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g, h, i); }
};

template <class T, class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
struct FunctorDecl<void (T::*)(A,B,C,D,E,F,G,H,I,J)>: public Functor {
   typedef void (T::*FuncPtr)(A,B,C,D,E,F,G,H,I,J);
   FuncPtr ptr; A a; B b; C c; D d; E e; F f; G g; H h; I i; J j;
   FunctorDecl(FuncPtr p) : ptr(p) {}
   void set(A &_a, B &_b, C &_c, D &_d, E &_e, F &_f, G &_g, H &_h, I &_i, J &_j) { a = _a; b = _b; c = _c; d = _d; e = _e; f = _f; g = _g; h = _h; i = _i; j = _j; }
   void read(BitStream &stream) { Types::read(stream, &a); Types::read(stream, &b); Types::read(stream, &c); Types::read(stream, &d); Types::read(stream, &e); Types::read(stream, &f); Types::read(stream, &g); Types::read(stream, &h); Types::read(stream, &i); Types::read(stream, &j); }
   void write(BitStream &stream) { Types::write(stream, a); Types::write(stream, b); Types::write(stream, c); Types::write(stream, d); Types::write(stream, e); Types::write(stream, f); Types::write(stream, g); Types::write(stream, h); Types::write(stream, i); Types::write(stream, j); }
   void dispatch(void *t) { (((T *)t)->*ptr)(a, b, c, d, e, f, g, h, i, j); }
};

};

#endif


// --- END OF FILE: tnl/tnlMethodDispatch.h ---
// --- START OF FILE: tnl/tnlNetBase.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NETBASE_H_
#define _TNL_NETBASE_H_

//------------------------------------------------------------------------------

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

#ifndef _TNL_PLATFORM_H_
#include "tnlPlatform.h"
#endif

#ifndef _TNL_ASSERT_H_
#include "tnlAssert.h"
#endif

#ifndef _TNL_BITSET_H_
#include "tnlBitSet.h"
#endif

#ifndef _TNL_VECTOR_H_
#include "tnlVector.h"
#endif

namespace TNL
{

/// NetClassTypes are used to define the ranges of individual
/// class identifiers, in order to reduce the number of bits
/// necessary to identify the class of an object across the
/// network.
///
/// For example, if there are only 21 classes declared
/// of NetClassTypeObject, the class identifier only needs to
/// be sent using 5 bits.
enum NetClassType {
   NetClassTypeNone = -1,  ///< Not an id'able network class
   NetClassTypeObject = 0, ///< Game object classes
   NetClassTypeDataBlock,  ///< Data block classes
   NetClassTypeEvent,      ///< Event classes
   NetClassTypeCount,
};

/// NetClassGroups are used to define different service types
/// for an application.
///
/// Each network-related class can be marked as valid across one or
/// more NetClassGroups.  Each network connection belongs to a
/// particular group, and can only transmit objects that are valid
/// in that group.
enum NetClassGroup {
   NetClassGroupGame,      ///< Group for game related network classes
   NetClassGroupCommunity, ///< Group for community server/authentication classes
   NetClassGroupMaster,    ///< Group for simple master server.
   NetClassGroupUnused2,   ///< Reserved group.
   NetClassGroupCount,
   NetClassGroupInvalid = NetClassGroupCount,
};

/// Mask values used to indicate which NetClassGroup(s) a NetObject or NetEvent
/// can be transmitted through.
enum NetClassMask {
   NetClassGroupGameMask      = 1 << NetClassGroupGame,
   NetClassGroupCommunityMask = 1 << NetClassGroupCommunity,
   NetClassGroupMasterMask    = 1 << NetClassGroupMaster,

   NetClassGroupAllMask = (1 << NetClassGroupCount) - 1,
};

class Object;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/// NetClassRep class instances represent each declared NetClass
/// and are used to construct object instances.
///
/// Core functionality for TNL-registered class manipulation.
///
/// @section NetClassRep_intro Introduction (or, Why AbstractClassRep?)
///
/// TNL requires the ability to programatically instantiate classes, by name or by
/// ID numbers. This is used for handling connections, ghosting, working with
/// events, and in many other areas. Torque uses this functionality for its scripting
/// language, too.
///
/// Since standard C++ doesn't provide a function to create a new instance of
/// an arbitrary class at runtime, one must be created. This is what
/// NetClassRep and NetClassRepInstance are all about. They allow the registration
/// and instantiation of arbitrary classes at runtime.
///
/// @see TNL::Object
///
/// @note In general, you will only access the functionality implemented in this class via
///       TNL::Object::create(). Most of the time, you will only ever need to use this part
///       part of the engine indirectly - ie, you will use the networking system.
///       <b>The following discussion is really only relevant for advanced TNL users.</b>
///
/// @section NetClassRep_netstuff NetClasses and Class IDs
///
/// TNL supports a notion of group, type, and direction for objects passed over
/// the network. Class IDs are assigned sequentially per-group, per-type, so that, for instance,
/// the IDs assigned to TNL::NetObjects are seperate from the IDs assigned to NetEvents.
/// This can translate into significant bandwidth savings (especially since the size of the fields
/// for transmitting these bits are determined at run-time based on the number of IDs given out.
///
/// @section NetClassRep_details NetClassRep Internals
///
/// NCR does some preparatory work at runtime before execution is passed to main(), through static
/// initialization. In actual fact, this preparatory work is done by the NetClassRepInstasnce
/// template. Let's examine this more closely.
///
/// If we examine TNL::Object, we see that two macros must be used in the definition of a
/// properly integrated objects. Let's look at an example:
///
/// @code
///      // This is from inside the class definition...
///      DECLARE_CONOBJECT(TestObject);
///
/// // And this is from outside the class definition...
/// IMPLEMENT_CONOBJECT(TestObject);
/// @endcode
///
/// What do these things actually do?
///
/// Not all that much, in fact. They expand to code something like this:
///
/// @code
///      // This is from inside the class definition...
///      static NetClassRepInstance<TestObject> dynClassRep;
///      static NetClassRep* getParentStaticClassRep();
///      static NetClassRep* getStaticClassRep();
///      virtual NetClassRep* getClassRep() const;
/// @endcode
///
/// @code
/// // And this is from outside the class definition...
/// NetClassRep* TestObject::getClassRep() const { return &TestObject::dynClassRep; }
/// NetClassRep* TestObject::getStaticClassRep() { return &dynClassRep; }
/// NetClassRep* TestObject::getParentStaticClassRep() { return Parent::getStaticClassRep(); }
/// NetClassRepInstance<TestObject> TestObject::dynClassRep("TestObject", 0, -1, 0);
/// @endcode
///
/// As you can see, getClassRep(), getStaticClassRep(), and getParentStaticClassRep() are just
/// accessors to allow access to various NetClassRepInstance instances. This is where the Parent
/// typedef comes into play as well - it lets getParentStaticClassRep() get the right
/// class rep.
///
/// In addition, dynClassRep is declared as a member of TestObject, and defined later
/// on. Much like Torque's ConsoleConstructor, NetClassRepInstances add themselves to a global linked
/// list in their constructor.
///
/// Then, when NetClassRep::initialize() is called, we iterate through
/// the list and perform the following tasks:
///      - Assigns network IDs for classes based on their NetGroup membership. Determines
///        bit allocations for network ID fields.
///
/// @nosubgrouping
class NetClassRep
{
   friend class Object;

protected:
   NetClassRep();
   virtual ~NetClassRep() {}
   U32 mClassGroupMask;                ///< Mask for which class groups this class belongs to.
   S32 mClassVersion;                  ///< The version number for this class.
   NetClassType mClassType;            ///< Which class type is this?
   U32 mClassId[NetClassGroupCount];   ///< The id for this class in each class group.
   char *mClassName;                   ///< The unmangled name of the class.

   U32 mInitialUpdateBitsUsed; ///< Number of bits used on initial updates of objects of this class.
   U32 mPartialUpdateBitsUsed; ///< Number of bits used on partial updates of objects of this class.
   U32 mInitialUpdateCount; ///< Number of objects of this class constructed over a connection.
   U32 mPartialUpdateCount; ///< Number of objects of this class updated over a connection.

   /// Next declared NetClassRep.
   ///
   /// These are stored in a linked list built by the macro constructs.
   NetClassRep *mNextClass;

   static NetClassRep *mClassLinkList;                                      ///< Head of the linked class list.
   static Vector<NetClassRep *> mClassTable[NetClassGroupCount][NetClassTypeCount]; ///< Table of NetClassReps for construction by class ID.
   static U32 mClassCRC[NetClassGroupCount];                                ///< Internally computed class group CRC.
   static bool mInitialized;                                                ///< Set once the class tables are built, from initialize.

   /// mNetClassBitSize is the number of bits needed to transmit the class ID for a group and type.
   static U32 mNetClassBitSize[NetClassGroupCount][NetClassTypeCount];

   /// @name Object Creation
   ///
   /// These helper functions let you create an instance of a class by name or ID.
   ///
   /// @note Call Object::create() instead of these.
   /// @{

   static Object *create(const char *className);
   static Object *create(const U32 groupId, const U32 typeId, const U32 classId);

   /// @}

public:
   U32 getClassId(NetClassGroup classGroup) const; ///< Returns the class ID, within its type, for the particular group.
   NetClassType getClassType() const;              ///< Returns the NetClassType of this class.
   S32 getClassVersion() const;                    ///< Returns the version of this class.
   const char *getClassName() const;               ///< Returns the string class name.

   /// Records bits used in the initial update of objects of this class.
   void addInitialUpdate(U32 bitCount)
   {
      mInitialUpdateCount++;
      mInitialUpdateBitsUsed += bitCount;
   }

   /// Records bits used in a partial update of an object of this class.
   void addPartialUpdate(U32 bitCount)
   {
      mPartialUpdateCount++;
      mPartialUpdateBitsUsed += bitCount;
   }

   virtual Object *create() const = 0;             ///< Creates an instance of the class this represents.

   /// Returns the number of classes registered under classGroup and classType.
   static U32 getNetClassCount(U32 classGroup, U32 classType)
      { return mClassTable[classGroup][classType].size(); }

   /// Returns the number of bits necessary to transmit class ids of the specified classGroup and classType.
   static U32 getNetClassBitSize(U32 classGroup, U32 classType)
      { return mNetClassBitSize[classGroup][classType]; }

   /// Returns true if the given class count is on a version boundary
   static bool isVersionBorderCount(U32 classGroup, U32 classType, U32 count)
      { return count == U32(mClassTable[classGroup][classType].size()) ||
               (count > 0 && mClassTable[classGroup][classType][count]->getClassVersion() !=
                        mClassTable[classGroup][classType][count - 1]->getClassVersion()); }

   static NetClassRep *getClass(U32 classGroup, U32 classType, U32 index)
      { return mClassTable[classGroup][classType][index]; }
      
   /// Returns a CRC of class data, for checking on connection.
   static U32 getClassGroupCRC(NetClassGroup classGroup);

   /// Initializes the class table and associated data - called from TNL::init().
   static void initialize();

   /// Logs the bit usage information of all the NetClassReps
   static void logBitUsage();
};

inline U32 NetClassRep::getClassId(NetClassGroup classGroup) const
{
   return mClassId[classGroup];
}

inline NetClassType NetClassRep::getClassType() const
{
   return mClassType;
}

inline S32 NetClassRep::getClassVersion() const
{
   return mClassVersion;
}

inline const char * NetClassRep::getClassName() const
{
   return mClassName;
}

inline U32 NetClassRep::getClassGroupCRC(NetClassGroup classGroup)
{
   return mClassCRC[classGroup];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/// NetClassRepInstance - one of these templates is instantiated for each
/// class that is declared via the IMPLEMENT_* macros below.
///
/// There will be an instance for each networkable class.
template <class T>
class NetClassRepInstance : public NetClassRep
{
public:
   /// Each class registers itself through the constructor of its NetClassInstance
   NetClassRepInstance(const char *className, U32 groupMask, NetClassType classType, S32 classVersion)
   {
      // Store data about ourselves
      mClassName      = strdup(className);
      mClassType      = classType;
      mClassGroupMask = groupMask;
      mClassVersion   = classVersion;
      for(U32 i = 0; i < NetClassGroupCount; i++)
         mClassId[i] = 0;

      // link the class into our global list
      mNextClass = mClassLinkList;
      mClassLinkList = this;
   }
   ~NetClassRepInstance()
   {
      free(mClassName);
   }

   /// Each NetClassRepInstance overrides the virtual create() function to construct its object instances.
   Object *create() const
   {
      return new T;
   }
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class SafeObjectRef;

/// Base class for all NetObject, NetEvent, NetConnection and NetInterface instances.
///
/// @section TNLObject_references Ways of Referencing Object
///
/// Object supports both reference counting and safe pointers.  Ref counted objects
/// are accessed via the RefPtr template class.  Once an object has been assigned using
/// a RefPtr, it will be deleted when all the reference pointers to it are destructed.
/// Object instances can also be safely referenced with the SafePtr template.  The
/// SafePtr template instances are automatically set NULL when the object they point
/// to is deleted.
///
/// @section TNLObject_basics The Basics
///
/// Any object which you want to work with the networking system should derive from this,
/// and access functionality through the static interface.
///
/// This class is always used with the TNL_DECLARE_CLASS and TNL_IMPLEMENT_CLASS macros.
///
/// @code
/// // A very basic example object. It will do nothing!
/// class TestObject : public NetObject {
///      // Must provide a Parent typedef so the console system knows what we inherit from.
///      typedef NetObject Parent;
///
///      // This does a lot of menial declaration for you.
///      TNL_DECLARE_CLASS(TestObject);
/// }
/// @endcode
///
/// @code
/// // And the accordant implementation...
/// TNL_IMPLEMENT_CLASS(TestObject);
///
/// @endcode
///
/// That's all you need to do to get a class registered with the TNL object system. At this point,
/// you can instantiate it via TNL::Object::create, ghost it, and so forth.
///
/// @see NetClassRepInstance for gory implementation details.
/// @nosubgrouping

class Object
{
   SafeObjectRef *mFirstObjectRef; ///< The head of the linked list of safe object references.
   U32 mRefCount;                  ///< Reference counter for RefPtr objects.

   friend class SafeObjectRef;
   friend class RefObjectRef;
public:
   /// Returns the NetClassRep associated with this object.
   virtual NetClassRep* getClassRep() const;

   Object();
   virtual ~Object();

   /// Object destroy self call (from RefPtr).
   ///
   /// @note Override if this class has specially allocated memory.
   virtual void destroySelf() { delete this; }

   /// Get our class ID within the specified NetClassGroup.
   U32 getClassId(NetClassGroup classGroup) const;

   /// Get our unmangled class name.
   const char *getClassName() const;

   /// @name Object Creation
   ///
   /// These helper functions let you create an instance of a class by name or ID.
   ///
   /// @note Call these instead of NetClassRep::create
   /// @{

   /// static function to create an instance of a named class
   static Object* create(const char* className) { return NetClassRep::create(className); }

   /// static function to create an instance of a class identified by a class group, class type and class id
   static Object* create( const NetClassGroup groupId,
                          const NetClassType typeId,
                          const U32 classId)
   {
      return NetClassRep::create(groupId, typeId, classId);
   }

   void incRef()
   {
      mRefCount++;
   }

   void decRef()
   {
      mRefCount--;
      if(!mRefCount)
         destroySelf();
   }
   /// @}
};

inline U32 Object::getClassId(NetClassGroup classGroup) const
{
   TNLAssert(getClassRep() != NULL,
               "Cannot get class id from non-declared dynamic class");
   return getClassRep()->getClassId(classGroup);
}

inline const char * Object::getClassName() const
{
   TNLAssert(getClassRep() != NULL,
               "Cannot get class name from non-declared dynamic class");
   return getClassRep()->getClassName();
}

/// Base class for Object reference counting.
class RefObjectRef
{
protected:
   Object *mObject; ///< The object this RefObjectRef references.

   /// Increments the reference count on the referenced object.
   void incRef()
   {
      if(mObject)
         mObject->incRef();
   }

   /// Decrements the reference count on the referenced object.
   void decRef()
   {
      if(mObject)
      {
         mObject->decRef();
      }
   }
public:

   /// Constructor, assigns from the object and increments its reference count if it's not NULL.
   RefObjectRef(Object *object = NULL)
   {
      mObject = object;
      incRef();
   }

   /// Destructor, dereferences the object, if there is one.
   ~RefObjectRef()
   {
      decRef();
   }

   /// Assigns this reference object from an existing Object instance.
   void set(Object *object)
   {
      decRef();
      mObject = object;
      incRef();
   }
};

/// Reference counted object template pointer class.
///
/// Instances of this template class can be used as pointers to
/// instances of Object and its subclasses.  The object will not
/// be deleted until all of the RefPtr instances pointing to it
/// have been destructed.
template <class T> class RefPtr : public RefObjectRef
{
public:
   RefPtr() : RefObjectRef() {}
   RefPtr(T *ptr) : RefObjectRef(ptr) {}
   RefPtr(const RefPtr<T>& ref) : RefObjectRef((T *) ref.mObject) {}

   RefPtr<T>& operator=(const RefPtr<T>& ref)
   {
      set((T *) ref.mObject);
      return *this;
   }
   RefPtr<T>& operator=(T *ptr)
   {
      set(ptr);
      return *this;
   }
   bool isNull() const   { return mObject == 0; }
   bool isValid() const  { return mObject != 0; }
   T* operator->() const { return static_cast<T*>(mObject); }
   T& operator*() const  { return *static_cast<T*>(mObject); }
   operator T*() const   { return static_cast<T*>(mObject); }
   operator T*() { return static_cast<T*>(mObject); }
   T* getPointer() { return static_cast<T*>(mObject); }
};

/// Base class for Object safe pointers.
class SafeObjectRef
{
   friend class Object;
protected:
   Object *mObject;               ///< The object this is a safe pointer to, or NULL if the object has been deleted.
   SafeObjectRef *mPrevObjectRef; ///< The previous SafeObjectRef for mObject.
   SafeObjectRef *mNextObjectRef; ///< The next SafeObjectRef for mObject.
public:
   SafeObjectRef(Object *object);
   SafeObjectRef();
   void set(Object *object);
   ~SafeObjectRef();

   void registerReference();   ///< Links this SafeObjectRef into the doubly linked list of SafeObjectRef instances for mObject.
   void unregisterReference(); ///< Unlinks this SafeObjectRef from the doubly linked list of SafeObjectRef instance for mObject.
};

inline void SafeObjectRef::unregisterReference()
{
   if(mObject)
   {
      if(mPrevObjectRef)
         mPrevObjectRef->mNextObjectRef = mNextObjectRef;
      else
         mObject->mFirstObjectRef = mNextObjectRef;
      if(mNextObjectRef)
         mNextObjectRef->mPrevObjectRef = mPrevObjectRef;
   }
}

inline void SafeObjectRef::registerReference()
{
   if(mObject)
   {
      mNextObjectRef = mObject->mFirstObjectRef;
      if(mNextObjectRef)
         mNextObjectRef->mPrevObjectRef = this;
      mPrevObjectRef = NULL;
      mObject->mFirstObjectRef = this;
   }
}

inline void SafeObjectRef::set(Object *object)
{
   unregisterReference();
   mObject = object;
   registerReference();
}

inline SafeObjectRef::~SafeObjectRef()
{
   unregisterReference();
}

inline SafeObjectRef::SafeObjectRef(Object *object)
{
   mObject = object;
   registerReference();
}

inline SafeObjectRef::SafeObjectRef()
{
   mObject = NULL;
}

/// Safe object template pointer class.
///
/// Instances of this template class can be used as pointers to
/// instances of Object and its subclasses.
///
/// When the object referenced by a SafePtr instance is deleted,
/// the pointer to the object is set to NULL in the SafePtr instance.
template <class T> class SafePtr : public SafeObjectRef
{
public:
   SafePtr() : SafeObjectRef() {}
   SafePtr(T *ptr) : SafeObjectRef(ptr) {}
   SafePtr(const SafePtr<T>& ref) : SafeObjectRef((T *) ref.mObject) {}

   SafePtr<T>& operator=(const SafePtr<T>& ref)
   {
      set((T *) ref.mObject);
      return *this;
   }
   SafePtr<T>& operator=(T *ptr)
   {
      set(ptr);
      return *this;
   }
   bool isNull() const   { return mObject == 0; }
   bool isValid() const  { return mObject != 0; }
   T* operator->() const { return static_cast<T*>(mObject); }
   T& operator*() const  { return *static_cast<T*>(mObject); }
   operator T*() const   { return static_cast<T*>(mObject); }
   operator T*() { return reinterpret_cast<T*>(mObject); }
   T* getPointer() { return static_cast<T*>(mObject); }
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Macros for declaring and implementing Object subclasses
// the TNL_DECLARE_CLASS(className) macro needs to be placed
// in the class declaraction for each network aware class.
//
// Different TNL_IMPLEMENT_* macros can be used depending on
// which NetClassType the class belongs to.

/// The TNL_DECLARE_CLASS macro should be called within the declaration of any network class
#define TNL_DECLARE_CLASS(className) \
   static TNL::NetClassRepInstance<className> dynClassRep;      \
   virtual TNL::NetClassRep* getClassRep() const

/// The TNL_IMPLEMENT_CLASS macro should be used for classes that will be auto-constructed
/// by name only.
#define TNL_IMPLEMENT_CLASS(className) \
   TNL::NetClassRep* className::getClassRep() const { return &className::dynClassRep; } \
   TNL::NetClassRepInstance<className> className::dynClassRep(#className, 0, TNL::NetClassTypeNone, 0)


};
#endif


// --- END OF FILE: tnl/tnlNetBase.h ---
// --- START OF FILE: tnl/tnlNetConnection.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NETCONNECTION_H_
#define _TNL_NETCONNECTION_H_

#ifndef _TNL_NETBASE_H_
#include "tnlNetBase.h"
#endif

#ifndef _TNL_LOG_H_
#include "tnlLog.h"
#endif

#ifndef _TNL_NONCE_H_
#include "tnlNonce.h"
#endif

#ifndef _TNL_SYMMETRICCIPHER_H_
#include "tnlSymmetricCipher.h"
#endif

#ifndef _TNL_CONNECTIONSTRINGTABLE_H_
#include "tnlConnectionStringTable.h"
#endif

namespace TNL {

class NetConnection;
class NetObject;
class BitStream;
class NetInterface;
class AsymmetricKey;
class Certificate;

/// NetConnectionRep maintians a linked list of valid connection classes.
struct NetConnectionRep
{
   static NetConnectionRep *mLinkedList;

   NetConnectionRep *mNext;
   NetClassRep *mClassRep;
   bool mCanRemoteCreate;

   NetConnectionRep(NetClassRep *classRep, bool canRemoteCreate)
   {
      mNext = mLinkedList;
      mLinkedList = this;
      mClassRep = classRep;
      mCanRemoteCreate = canRemoteCreate;
   }
   static NetConnection *create(const char *name);
};

#define TNL_DECLARE_NETCONNECTION(className) \
   TNL_DECLARE_CLASS(className); \
   TNL::NetClassGroup getNetClassGroup() const

#define TNL_IMPLEMENT_NETCONNECTION(className, classGroup, canRemoteCreate) \
   TNL::NetClassRep* className::getClassRep() const { return &className::dynClassRep; } \
   TNL::NetClassRepInstance<className> className::dynClassRep(#className, 0, TNL::NetClassTypeNone, 0); \
   TNL::NetClassGroup className::getNetClassGroup() const { return classGroup; } \
   static TNL::NetConnectionRep g##className##Rep(&className::dynClassRep, canRemoteCreate)


/// All data associated with the negotiation of the connection
struct ConnectionParameters
{
   bool mIsArranged;                 ///< True if this is an arranged connection
   bool mUsingCrypto;                ///< Set to true if this connection is using crypto (public key and symmetric)
   bool mPuzzleRetried;              ///< True if a puzzle solution was already rejected by the server once.
   Nonce mNonce;                     ///< Unique nonce generated for this connection to send to the server.
   Nonce mServerNonce;               ///< Unique nonce generated by the server for the connection.
   U32 mPuzzleDifficulty;            ///< Difficulty of the client puzzle solved by this client.
   U32 mPuzzleSolution;              ///< Solution to the client puzzle the server sends to the client.
   U32 mClientIdentity;              ///< The client identity as computed by the remote host.
   RefPtr<AsymmetricKey> mPublicKey; ///< The public key of the remote host.
   RefPtr<AsymmetricKey> mPrivateKey;///< The private key for this connection.  May be generated on the connection attempt.
   RefPtr<Certificate> mCertificate; ///< The certificate of the remote host.
   ByteBufferPtr mSharedSecret;      ///< The shared secret key 
   bool mRequestKeyExchange;         ///< The initiator of the connection wants a secure key exchange
   bool mRequestCertificate;         ///< The client is requesting a certificate
   U8 mSymmetricKey[SymmetricCipher::KeySize]; ///< The symmetric key for the connection, generated by the client
   U8 mInitVector[SymmetricCipher::KeySize]; ///< The init vector, generated by the server
   Vector<Address> mPossibleAddresses; ///< List of possible addresses for the remote host in an arranged connection.
   bool mIsInitiator;                ///< True if this host initiated the arranged connection.
   bool mIsLocal;                    ///< True if this is a connectLocal connection.
   ByteBufferPtr mArrangedSecret;    ///< The shared secret as arranged by the connection intermediary.
   bool mDebugObjectSizes;           ///< This connection's initiator requested debugging size information during packet writes.

   ConnectionParameters()
   {
      mIsInitiator = false;
      mPuzzleRetried = false;
      mUsingCrypto = false;
      mIsArranged = false;
#ifdef TNL_DEBUG
      mDebugObjectSizes = true;
#else
      mDebugObjectSizes = false;     
#endif
      mIsLocal = false;
   }
};

//----------------------------------------------------------------------------
/// TNL network connection base class.
///
/// NetConnection is the base class for the connection classes in TNL. It implements a
/// notification protocol on the unreliable packet transport of UDP (via the TNL::Net layer).
/// NetConnection manages the flow of packets over the network, and calls its subclasses
/// to read and write packet data, as well as handle packet delivery notification.
///
/// Because string data can easily soak up network bandwidth, for
/// efficiency NetConnection implements an optional networked string table.
/// Users can then notify the connection of strings it references often, such as player names,
/// and transmit only a tag, instead of the whole string.
///
class NetConnection : public Object
{
   friend class NetInterface;
   friend class ConnectionStringTable;

   typedef Object Parent;

   /// Constants controlling the data representation of each packet header
   enum HeaderConstants {
      // NOTE - IMPORTANT!
      // The first bytes of each packet are made up of:
      // 1 bit - game data packet flag
      // 2 bits - packet type
      // SequenceNumberBitSize bits - sequence number
      // AckSequenceNumberBitSize bits - high ack sequence received
      // these values should be set to align to a byte boundary, otherwise
      // bits will just be wasted.

      MaxPacketWindowSizeShift = 5,                            ///< Packet window size is 2^MaxPacketWindowSizeShift.
      MaxPacketWindowSize = (1 << MaxPacketWindowSizeShift),   ///< Maximum number of packets in the packet window.
      PacketWindowMask = MaxPacketWindowSize - 1,              ///< Mask for accessing the packet window.
      MaxAckMaskSize = 1 << (MaxPacketWindowSizeShift - 5),    ///< Each ack word can ack 32 packets.
      MaxAckByteCount = MaxAckMaskSize << 2,                   ///< The maximum number of ack bytes sent in each packet.
      SequenceNumberBitSize = 11,                              ///< Bit size of the send and sequence number.
      SequenceNumberWindowSize = (1 << SequenceNumberBitSize), ///< Size of the send sequence number window.
      SequenceNumberMask = -SequenceNumberWindowSize,          ///< Mask used to reconstruct the full send sequence number of the packet from the partial sequence number sent.
      AckSequenceNumberBitSize = 10,                           ///< Bit size of the ack receive sequence number.
      AckSequenceNumberWindowSize = (1 << AckSequenceNumberBitSize), ///< Size of the ack receive sequence number window.
      AckSequenceNumberMask = -AckSequenceNumberWindowSize,          ///< Mask used to reconstruct the full ack receive sequence number of the packet from the partial sequence number sent.

      PacketHeaderBitSize = 3 + AckSequenceNumberBitSize + SequenceNumberBitSize, ///< Size, in bits, of the packet header sequence number section
      PacketHeaderByteSize = (PacketHeaderBitSize + 7) >> 3, ///< Size, in bytes, of the packet header sequence number information
      PacketHeaderPadBits = (PacketHeaderByteSize << 3) - PacketHeaderBitSize, ///< Padding bits to get header bytes to align on a byte boundary, for encryption purposes.

      MessageSignatureBytes = 5, ///< Special data bytes written into the end of the packet to guarantee data consistency
   };
   U32 mLastPacketRecvTime; ///< Time of the receipt of the last data packet.
   U32 mLastSeqRecvdAtSend[MaxPacketWindowSize]; ///< The sequence number of the last packet received from the remote host when we sent the packet with sequence X & PacketWindowMask.
   U32 mLastSeqRecvd;                            ///< The sequence number of the most recently received packet from the remote host.
   U32 mHighestAckedSeq;                         ///< The highest sequence number the remote side has acknowledged.
   U32 mLastSendSeq;                             ///< The sequence number of the last packet sent.
   U32 mAckMask[MaxAckMaskSize];                 ///< long string of bits, each acking a packet sent by the remote host.
                                                 ///< The bit associated with mLastSeqRecvd is the low bit of the 0'th word of mAckMask.
   U32 mLastRecvAckAck; ///< The highest sequence this side knows the other side has received an ACK or NACK for.

   U32 mInitialSendSeq; ///< The first mLastSendSeq for this side of the connection.
   U32 mInitialRecvSeq; ///< The first mLastSeqRecvd (the first mLastSendSeq for the remote host).
   U32 mHighestAckedSendTime; ///< The send time of the highest packet sequence acked by the remote host.  Used in the computation of round trip time.
   /// Two-bit identifier for each connected packet.
   enum NetPacketType
   {
      DataPacket, ///< Standard data packet.  Each data packet sent increments the current packet sequence number (mLastSendSeq).
      PingPacket, ///< Ping packet, sent if this instance hasn't heard from the remote host for a while.  Sending a
                  ///  ping packet does not increment the packet sequence number.
      AckPacket,  ///< Packet sent in response to a ping packet.  Sending an ack packet does not increment the sequence number.
      InvalidPacketType,
   };
   /// Constants controlling the behavior of pings and timeouts
   enum DefaultPingConstants {
      AdaptiveInitialPingTimeout = 60000,
      AdaptivePingRetryCount = 4,
      DefaultPingTimeout = 5000,  ///< Default milliseconds to wait before sending a ping packet.
      DefaultPingRetryCount = 10, ///< Default number of unacknowledged pings to send before timing out.
      AdaptiveUnackedSentPingTimeout = 3000,
   };
   U32 mPingTimeout; ///< Milliseconds to wait before sending a ping packet.
   U32 mPingRetryCount; ///< Number of unacknowledged pings to send before timing out.

   /// Returns true if this connection has sent packets that have not yet been acked by the remote host.
   bool hasUnackedSentPackets() { return mLastSendSeq != mHighestAckedSeq; }
public:
   struct PacketNotify;

   NetConnection();
   ~NetConnection();

   enum TerminationReason {
      ReasonTimedOut,
      ReasonFailedConnectHandshake,
      ReasonRemoteHostRejectedConnection,
      ReasonRemoteDisconnectPacket,
      ReasonDuplicateConnectionAttempt,
      ReasonSelfDisconnect,
      ReasonError,
   };

protected:

   virtual void onConnectTerminated(TerminationReason reason, const char *rejectionString);     ///< Called when a pending connection is terminated
   virtual void onConnectionTerminated(TerminationReason, const char *errorDisconnectString);   ///< Called when this established connection is terminated for any reason
   virtual void onConnectionEstablished();  ///< Called when the connection is successfully established with the remote host.

   /// validates that the given certificate is a valid certificate for this
   /// connection.
   virtual bool validateCertficate(Certificate *theCertificate, bool isInitiator) { return true; }

   /// Validates that the given public key is valid for this connection.  If this
   /// host requires a valid certificate for the communication, this function
   /// should always return false.  It will only be called if the remote side
   /// of the connection did not provide a certificate.
   virtual bool validatePublicKey(AsymmetricKey *theKey, bool isInitiator) { return true; }

   /// Fills the connect request packet with additional custom data (from a subclass).
   virtual void writeConnectRequest(BitStream *stream);

   /// Called after this connection instance is created on a non-initiating host (server).
   ///
   /// Reads data sent by the writeConnectRequest method and returns true if the connection is accepted
   /// or false if it's not.  The errorString pointer should be filled if the connection is rejected.
   virtual bool readConnectRequest(BitStream *stream, const char **errorString);

   /// Writes any data needed to start the connection on the accept packet.
   virtual void writeConnectAccept(BitStream *stream);

   /// Reads out the extra data read by writeConnectAccept and returns true if it is processed properly.
   virtual bool readConnectAccept(BitStream *stream, const char **errorString);

   virtual void readPacket(BitStream *bstream);                      ///< Called to read a subclass's packet data from the packet.

   virtual void prepareWritePacket();  ///< Called to prepare the connection for packet writing.
                                       ///
                                       ///  Any setup work to determine if there isDataToTransmit() should happen in
                                       ///  this function.  prepareWritePacket should _always_ call the Parent:: function.

   virtual void writePacket(BitStream *bstream, PacketNotify *note); ///< Called to write a subclass's packet data into the packet.
                                                                     ///
                                                                     ///  Information about what the instance wrote into the packet can be attached
                                                                     ///  to the notify object.

   virtual void packetReceived(PacketNotify *note);                  ///< Called when the packet associated with the specified notify is known to have been received by the remote host.
                                                                     ///
                                                                     ///  Packets are guaranteed to be notified in the order in which they were sent.
   virtual void packetDropped(PacketNotify *note);                   ///< Called when the packet associated with the specified notify is known to have been not received by the remote host.
                                                                     ///
                                                                     ///  Packets are guaranteed to be notified in the order in which they were sent.

   /// Allocates a data record to track data sent on an individual packet.
   ///
   /// If you need to track additional notification information, you'll have to
   /// override this so you allocate a subclass of PacketNotify with extra fields.
   virtual PacketNotify *allocNotify() { return new PacketNotify; }

public:
   /// Returns the next send sequence that will be sent by this side.
   U32 getNextSendSequence() { return mLastSendSeq + 1; }

   /// Returns the sequence of the last packet sent by this connection, or
   /// the current packet's send sequence if called from within writePacket().
   U32 getLastSendSequence() { return mLastSendSeq; }

protected:
   /// Reads a raw packet from a BitStream, as dispatched from NetInterface.
   void readRawPacket(BitStream *bstream);
   /// Writes a full packet of the specified type into the BitStream
   void writeRawPacket(BitStream *bstream, NetPacketType packetType);

   /// Writes the notify protocol's packet header into the BitStream.
   void writePacketHeader(BitStream *bstream, NetPacketType packetType);
   /// Reads a notify protocol packet header from the BitStream and
   /// returns true if it was a data packet that needs more processing.
   bool readPacketHeader(BitStream *bstream);

   void writePacketRateInfo(BitStream *bstream, PacketNotify *note); ///< Writes any packet send rate change information into the packet.
   void readPacketRateInfo(BitStream *bstream);                      ///< Reads any packet send rate information requests from the packet.

   void sendPingPacket(); ///< Sends a ping packet to the remote host, to determine if it is still alive and what its packet window status is.
   void sendAckPacket();  ///< Sends an ack packet to the remote host, in response to receiving a ping packet.

   /// Dispatches a notify when a packet is ACK'd or NACK'd.
   void handleNotify(U32 sequence, bool recvd);

   /// Called when a packet is received to stop any timeout action in progress.
   void keepAlive();

   void clearAllPacketNotifies(); ///< Clears out the pending notify list.

public:
   /// Sets the initial sequence number of packets read from the remote host.
   void setInitialRecvSequence(U32 sequence);

   /// Returns the initial sequence number of packets sent from the remote host.
   U32 getInitialRecvSequence() { return mInitialRecvSeq; }

   /// Returns the initial sequence number of packets sent to the remote host.
   U32 getInitialSendSequence() { return mInitialSendSeq; }

   /// Connect to a server through a given network interface.
   /// The connection request can require that the connection use encryption, 
   /// or that the remote host's certificate be validated by a known Certificate Authority
   void connect(NetInterface *connectionInterface, const Address &address, bool requestKeyExchange = false, bool requestCertificate = false);

   /// Connects to a server interface within the same process.
   bool connectLocal(NetInterface *connectionInterface, NetInterface *localServerInterface);

   /// Connects to a remote host that is also connecting to this connection (negotiated by a third party)
   void connectArranged(NetInterface *connectionInterface, const Vector<Address> &possibleAddresses, Nonce &myNonce, Nonce &remoteNonce, ByteBufferPtr sharedSecret, bool isInitiator, bool requestsKeyExchange = false, bool requestsCertificate = false);

   /// Sends a disconnect packet to notify the remote host that this side is terminating the connection for the specified reason.
   /// This will remove the connection from its NetInterface, and may have the side
   /// effect that the connection is deleted, if there are no other objects with RefPtrs
   /// to the connection.
   void disconnect(const char *reason);

   /// Returns true if the packet send window is full and no more data packets can be sent.
   bool windowFull();

   /// Structure used to track what was sent in an individual packet for processing
   /// upon notification of delivery success or failure.
   struct PacketNotify
   {
      // packet stream notify stuff:
      bool rateChanged;  ///< True if this packet requested a change of rate.
      U32  sendTime;     ///< Platform::getRealMilliseconds() when packet was sent.
      ConnectionStringTable::PacketList stringList; ///< List of string table entries sent in this packet

      PacketNotify *nextPacket; ///< Pointer to the next packet sent on this connection
      PacketNotify();
   };

//----------------------------------------------------------------
// Connection functions
//----------------------------------------------------------------

   /// Flags specifying the type of the connection instance.
   enum NetConnectionTypeFlags {
      ConnectionToServer = BIT(0), ///< A connection to a "server", used for directing NetEvents
      ConnectionToClient = BIT(1), ///< A connection to a "client"
      ConnectionAdaptive = BIT(2), ///< Indicates that this connection uses the adaptive protocol.
      ConnectionRemoteAdaptive = BIT(3), ///< Indicates that the remote side of this connection requested the adaptive protocol.
   };

private:
   BitSet32 mTypeFlags;  ///< Flags describing the type of connection this is, OR'd from NetConnectionTypeFlags.
   U32 mLastUpdateTime;  ///< The last time a packet was sent from this instance.
   F32 mRoundTripTime;   ///< Running average round trip time.
   U32 mSendDelayCredit; ///< Metric to help compensate for irregularities on fixed rate packet sends.

   U32 mSimulatedLatency;    ///< Amount of additional time this connection delays its packet sends to simulate latency in the connection
   F32 mSimulatedPacketLoss; ///< Function to simulate packet loss on a network

   enum RateDefaults {
      DefaultFixedBandwidth  = 2500,  ///< The default send/receive bandwidth - 2.5 Kb per second.
      DefaultFixedSendPeriod = 96,    ///< The default delay between each packet send - approx 10 packets per second.
      MaxFixedBandwidth      = 65535, ///< The maximum bandwidth for a connection using the fixed rate transmission method.
      MaxFixedSendPeriod     = 2047,  ///< The maximum period between packets in the fixed rate send transmission method.
   };

   /// Rate management structure used specify the rate at which packets are sent and the maximum size of each packet.
   struct NetRate
   {
      U32 minPacketSendPeriod; ///< Minimum millisecond delay (maximum rate) between packet sends.
      U32 minPacketRecvPeriod; ///< Minimum millisecond delay the remote host should allow between sends.
      U32 maxSendBandwidth;    ///< Number of bytes per second we can send over the connection.
      U32 maxRecvBandwidth;    ///< Number of bytes per second max that the remote instance should send.
   };
   void computeNegotiatedRate(); ///< Called internally when the local or remote rate changes.
   NetRate mLocalRate;           ///< Current communications rate negotiated for this connection.
   NetRate mRemoteRate;          ///< Maximum allowable communications rate for this connection.

   bool mLocalRateChanged;       ///< Set to true when the local connection's rate has changed.
   U32 mCurrentPacketSendSize;   ///< Current size of each packet sent to the remote host.
   U32 mCurrentPacketSendPeriod; ///< Millisecond delay between sent packets.

   Address mNetAddress;       ///< The network address of the host this instance is connected to.

   // timeout management stuff:
   U32 mPingSendCount;    ///< Number of unacknowledged ping packets sent to the remote host
   U32 mLastPingSendTime; ///< Last time a ping packet was sent from this connection

protected:
   PacketNotify *mNotifyQueueHead;  ///< Linked list of structures representing the data in sent packets
   PacketNotify *mNotifyQueueTail;  ///< Tail of the notify queue linked list.  New packets are added to the end of the tail.

   /// Returns the notify structure for the current packet write, or last written packet.
   PacketNotify *getCurrentWritePacketNotify() { return mNotifyQueueTail; }


   SafePtr<NetConnection> mRemoteConnection;  ///< Safe pointer to a short-circuit remote connection on the same host.
                                              ///
                                              ///  This currently isn't enabled - see the end of netConnection.cpp for an example
                                              ///  of how to use this. If it's set, the code will use short circuited networking.
   ConnectionParameters mConnectionParameters;
public:
   ConnectionParameters &getConnectionParameters() { return mConnectionParameters; }

   /// returns true if this object initiated the connection with the remote host
   bool isInitiator() { return mConnectionParameters.mIsInitiator; }
   void setRemoteConnectionObject(NetConnection *connection) { mRemoteConnection = connection; };
   NetConnection *getRemoteConnectionObject() { return mRemoteConnection; }

   U32 mConnectSendCount;    ///< Number of challenge or connect requests sent to the remote host.
   U32 mConnectLastSendTime; ///< The send time of the last challenge or connect request.

protected:
   static char mErrorBuffer[256]; ///< String buffer that errors are written into
public:
   static char *getErrorBuffer() { return mErrorBuffer; } ///< returns the current error buffer
   static void setLastError(const char *fmt,...);         ///< Sets an error string and notifies the currently processing connection that it should terminate.

protected:
   SafePtr<NetInterface> mInterface;             ///< The NetInterface of which this NetConnection is a member.
public:
   void setInterface(NetInterface *myInterface); ///< Sets the NetInterface this NetConnection will communicate through.
   NetInterface *getInterface();                 ///< Returns the NetInterface this connection communicates through.

protected:
   RefPtr<SymmetricCipher> mSymmetricCipher;    ///< The helper object that performs symmetric encryption on packets
public:
   void setSymmetricCipher(SymmetricCipher *theCipher); ///< Sets the SymmetricCipher this NetConnection will use for encryption

public:
   /// Returns the class group of objects that can be transmitted over this NetConnection.
   virtual NetClassGroup getNetClassGroup() const { return NetClassGroupInvalid; }

   /// Sets the ping/timeout characteristics for a fixed-rate connection.  Total timeout is msPerPing * pingRetryCount.
   void setPingTimeouts(U32 msPerPing, U32 pingRetryCount)
      { mPingRetryCount = pingRetryCount; mPingTimeout = msPerPing; }
   
   /// Simulates a network situation with a percentage random packet loss and a connection one way latency as specified.
   void setSimulatedNetParams(F32 packetLoss, U32 latency)
      { mSimulatedPacketLoss = packetLoss; mSimulatedLatency = latency; }

   /// Specifies that this NetConnection instance is a connection to a "server."
   void setIsConnectionToServer() { mTypeFlags.set(ConnectionToServer); }

   /// Returns true if this is a connection to a "server."
   bool isConnectionToServer()  { return mTypeFlags.test(ConnectionToServer); }

   /// Specifies that this NetConnection instance is a connection to a "client."
   void setIsConnectionToClient() { mTypeFlags.set(ConnectionToClient); }

   /// Returns true if this is a connection to a "client."
   bool isConnectionToClient()  { return mTypeFlags.test(ConnectionToClient); }

   /// Returns true if the remote side of this connection is a NetConnection instance in on the same host.
   bool isLocalConnection() { return !mRemoteConnection.isNull(); }

   /// Returns true if the remote side if this connection is on a remote host.
   bool isNetworkConnection() { return mRemoteConnection.isNull(); }

   /// Returns the running average packet round trip time.
   F32 getRoundTripTime()
      { return mRoundTripTime; }

   /// Returns have of the average of the round trip packet time.
   F32 getOneWayTime()
      { return mRoundTripTime * 0.5f; }

   /// Returns the remote address of the host we're connected or trying to connect to.
   const Address &getNetAddress();

   /// Returns the remote address in string form.
   const char *getNetAddressString() const { return mNetAddress.toString(); }

   /// Sets the address of the remote host we want to connect to.
   void setNetAddress(const Address &address);

   /// Sends a packet that was written into a BitStream to the remote host, or the mRemoteConnection on this host.
   NetError sendPacket(BitStream *stream);

   /// Checks to see if the connection has timed out, possibly sending a ping packet to the remote host.  Returns true if the connection timed out.
   bool checkTimeout(U32 time);

   /// Checks to see if a packet should be sent at the currentTime to the remote host.
   ///
   /// If force is true and there is space in the window, it will always send a packet.
   void checkPacketSend(bool force, U32 currentTime);

   /// Connection state flags for a NetConnection instance.
   enum NetConnectionState {
      NotConnected=0,            ///< Initial state of a NetConnection instance - not connected.
      AwaitingChallengeResponse, ///< We've sent a challenge request, awaiting the response.
      SendingPunchPackets,       ///< The state of a pending arranged connection when both sides haven't heard from the other yet
      ComputingPuzzleSolution,   ///< We've received a challenge response, and are in the process of computing a solution to its puzzle.
      AwaitingConnectResponse,   ///< We've received a challenge response and sent a connect request.
      ConnectTimedOut,           ///< The connection timed out during the connection process.
      ConnectRejected,           ///< The connection was rejected.
      Connected,                 ///< We've accepted a connect request, or we've received a connect response accept.
      Disconnected,              ///< The connection has been disconnected.
      TimedOut,                  ///< The connection timed out.
      StateCount,
   };

   NetConnectionState mConnectionState; ///< Current state of this NetConnection.

   /// Sets the current connection state of this NetConnection.
   void setConnectionState(NetConnectionState state) { mConnectionState = state; }

   /// Gets the current connection state of this NetConnection.
   NetConnectionState getConnectionState() { return mConnectionState; }

   /// Returns true if the connection handshaking has completed successfully.
   bool isEstablished() { return mConnectionState == Connected; }

   /// @name Adaptive Protocol
   ///
   /// Functions and state for the adaptive rate protocol.
   ///
   /// TNL's adaptive rate uses rate control algorithms similar to
   /// TCP/IP's.
   ///
   /// There are a few state variables here that aren't documented.
   ///
   /// @{

public:

   /// Enables the adaptive protocol.
   ///
   /// By default NetConnection operates with a fixed rate protocol - that is, it sends a
   /// packet every few milliseconds, based on some configuration parameters. However,
   /// it is possible to use an adaptive rate protocol that attempts to maximize thoroughput
   /// over the connection.
   ///
   /// Calling this function enables this behavior.
   void setIsAdaptive();

   /// sets the fixed rate send and receive data sizes, and sets the connection to not behave as an adaptive rate connection
   void setFixedRateParameters( U32 minPacketSendPeriod, U32 minPacketRecvPeriod, U32 maxSendBandwidth, U32 maxRecvBandwidth );

   /// Query the adaptive status of the connection.
   bool isAdaptive()    { return mTypeFlags.test(ConnectionAdaptive | ConnectionRemoteAdaptive); }

   /// Returns true if this connection has data to transmit.
   ///
   /// The adaptive rate protocol needs to be able to tell if there is data
   /// ready to be sent, so that it can avoid sending unnecessary packets.
   /// Each subclass of NetConnection may need to send different data - events,
   /// ghost updates, or other things. Therefore, this hook is provided so
   /// that child classes can overload it and let the adaptive protocol
   /// function properly.
   ///
   /// @note Make sure this calls to its parents - the accepted idiom is:
   ///       @code
   ///       return Parent::isDataToTransmit() || localConditions();
   ///       @endcode
   virtual bool isDataToTransmit() { return false; }


private:
   F32 cwnd;
   F32 ssthresh;
   U32 mLastSeqRecvdAck;
   U32 mLastAckTime;

   /// @}
private:
   ConnectionStringTable *mStringTable; ///< Helper for managing translation between global NetStringTable ids to local ids for this connection.
public:
   /// Enables string tag translation on this connection.
   void setTranslatesStrings();
};

static const U32 MinimumPaddingBits = 128;       ///< Padding space that is required at the end of each packet for bit flag writes and such.

};

#endif

// --- END OF FILE: tnl/tnlNetConnection.h ---
// --- START OF FILE: tnl/tnlNetEvent.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NETEVENT_H_
#define _TNL_NETEVENT_H_

#ifndef _TNL_NETBASE_H_
#include "tnlNetBase.h"
#endif

namespace TNL {

//----------------------------------------------------------------------------

class EventConnection;
class BitStream;

/// An event to be sent over the network.
///
/// @note TNL implements two methods of network data passing; this is one of them.
/// See GhostConnection for details of the other, which is referred to as ghosting.
///
/// TNL lets you pass NetEvent objects across EventConnection instances. There are three
/// types of events:
///      - <b>Unguaranteed events</b> are events which are sent once. If they don't
///        make it through the link, they are not resent. This is good for quick,
///        frequent status updates which are of transient interest, like voice
///        communication fragments.
///      - <b>Guaranteed events</b> are events which are guaranteed to be
///        delivered. If they don't make it through the link, they are sent as
///        needed. This is good for important, one-time information,
///        like which team a user wants to play on, or the current weather.
///        Guaranteed events are processed when they are received, so they may be processed
///        in a different order than they were sent in.
///      - <b>GuaranteedOrdered events</b> are events which are guaranteed to be
///        delivered, and whose process methods will be executed in the order the events were sent.
///        This is good for information which is not only important, but also order-critical, like
///        chat messages or file transfers.
///
/// There are 3 methods that you need to implement if you want to make a
/// basic NetEvent subclass, and 2 macros you need to call.
///
/// @code
/// // A simple NetEvent to transmit a string over the network.
/// class SimpleMessageEvent : public NetEvent
/// {
///    typedef NetEvent Parent;
///    char *msg;
/// public:
///    SimpleMessageEvent(const char *message = NULL);
///    ~SimpleMessageEvent();
///
///    virtual void pack   (EventConnection *conn, BitStream *bstream)
///    virtual void unpack (EventConnection *conn, BitStream *bstream);
///    virtual void process(EventConnection *conn);
///
///    TNL_DECLARE_CLASS(SimpleMessageEvent);
/// };
///
/// TNL_IMPLEMENT_NETEVENT(SimpleMessageEvent, NetClassGroupGameMask,0);
/// @endcode
///
/// The first macro called, TNL_DECLARE_CLASS() registers the static class functions and NetClassRep object that will assign
/// this class a network ID and allow instances to be constructed by ID.
///
/// The second, TNL_IMPLEMENT_NETEVENT(), instantiates the NetClassRep and
/// tells it that the instances are NetEvent objects in the Game group.  The final
/// parameter to the TNL_IMPLEMENT_NETEVENT macro is the version number of the event
/// class.  Versioning allows a server to offer new event services without forcing
/// older clients to be updated.
///
/// In the constructor for the event the guarantee type of the event and the direction it will be
/// allowed to travel over the connection, must be specified by way of the constructor
/// for the base NetEvent class.  The guarantee type can be one of:
///      - <b>NetEvent::GuaranteedOrdered</b>, for guaranteed, ordered events
///      - <b>NetEvent::Guaranteed</b>, for guaranteed events
///      - <b>NetEvent::Unguaranteed</b>, for unguaranteed events
///
/// It is also a good idea to clearly specify which direction the event is allowed to travel.
/// If the program has a certain set of message events that are only sent from server to client,
/// then the network system can enforce that error checking automatically, in order to prevent hacks that may
/// otherwise crash or compromise the program.  The valid event directions are:
///      - <b>NetEvent::DirAny</b>, this event can be sent from server to client
///        or from client to server
///      - <b>NetEvent::DirServerToClient</b>, this event can only be sent from
///        server to client.  If the server receives an event of this type, it will
///        signal an error on the connection.
///      - <b>NetEvent::DirClientToServer</b>, this event can only be sent from client
///        to server.  If the client receives an event of this type, it will signal an
///        error on the connection.
///
/// @note TNL allows you to call NetConnection::setLastError() on the EventConnection passed to
///       the NetEvent. This will cause the connection to abort if invalid data is received, specifying
///       a reason to the user.
///
/// Of the 5 methods declared above; the constructor and destructor need only do
/// whatever book-keeping is needed for the specific implementation, in addition to calling
/// the NetEvent constructor with the direction and type information that the networking system
/// needs to function. In this case, the SimpleMessageEvent simply allocates/deallocates the space for the string,
/// and specifies the event as guaranteed ordered and bidirectional.
///
/// @code
///    SimpleMessageEvent::SimpleMessageEvent(const char *message = NULL)
///           : NetEvent(NetEvent::GuaranteedOrdered, NetEvent::DirAny)
///    {
///       // we marked this event as GuaranteedOrdered, and it can be sent in any direction
///       if(message)
///          msg = strdup(message);
///       else
///          msg = NULL;
///    }
///
///    SimpleMessageEvent::~SimpleMessageEvent()
///    {
///      free(msg);
///    }
/// @endcode
///
/// The 3 other functions that must be overridden for evern NetEvent are pack(), unpack() and process().
///
/// <b>pack()</b> is responsible for packing the event over the wire:
///
/// @code
/// void SimpleMessageEvent::pack(EventConnection* conn, BitStream *bstream)
/// {
///   bstream->writeString(msg);
/// }
/// @endcode
///
/// <b>unpack()</b> is responsible for unpacking the event on the other end:
///
/// @code
/// // The networking layer takes care of instantiating a new
/// // SimpleMessageEvent, which saves us a bit of effort.
/// void SimpleMessageEvent::unpack(EventConnection *conn, BitStream *bstream)
/// {
///   char buf[256];
///   bstream->readString(buf);
///   msg = strdup(buf);
/// }
/// @endcode
///
/// <b>process()</b> is called when the network layer is finished with things.
/// A typical case is that a GuaranteedOrdered event is unpacked and stored, but
/// not processed until the events preceding it in the sequence have been process()'d.
///
/// @code
/// // This just prints the event in the log. You might
/// // want to do something more clever here.
/// void SimpleMessageEvent::process(EventConnection *conn)
/// {
///   logprintf("Received a SimpleMessageEvent: %s", msg);
///
///   // An example of something more clever - kick people who say bad words.
///   // if(isBadWord(msg)) conn->setLastError("No swearing, naughtypants!");
/// }
/// @endcode
///
/// Posting an event to the remote host on a connection is simple:
///
/// @code
/// EventConnection *conn; // We assume you have filled this in.
///
/// conn->postNetEvent(new SimpleMessageEvent("This is a test!"));
/// @endcode
///
/// Finally, for more advanced applications, notifyPosted() is called when the event is posted
/// into the send queue, notifySent() is called whenever the event is
/// sent over the wire, in EventConnection::eventWritePacket(). notifyDelivered() is called
/// when the packet is finally received or (in the case of Unguaranteed packets) dropped.
///
/// @note the TNL_IMPLEMENT_NETEVENT groupMask specifies which "group" of EventConnections
/// the event can be sent over.  See TNL::Object for a further discussion of this.
class NetEvent : public Object
{
   friend class EventConnection;
public:
   enum EventDirection {
      DirUnset,          ///< Default value - NetConnection will Assert if an event is posted without a valid direction set.
      DirAny,            ///< This event can be sent from the server or the client
      DirServerToClient, ///< This event can only be sent from the server to the client
      DirClientToServer, ///< This event can only be sent from the client to the server
   } mEventDirection;   ///< Direction this event is allowed to travel in the network

   enum GuaranteeType {
      GuaranteedOrdered = 0, ///< Event delivery is guaranteed and will be processed in the order it
                             ///  was sent relative to other ordered events.
      Guaranteed = 1,        ///< Event delivery is guaranteed and will be processed in the order it
                             ///  was received.
      Unguaranteed = 2       ///< Event delivery is not guaranteed - however, the event will remain
                             ///  ordered relative to other unguaranteed events.
   } mGuaranteeType;        ///< Type of data guarantee this event supports

   /// Constructor - should always be called by subclasses.
   ///
   /// Subclasses MUST pass in an event direction and guarantee type, or else the network
   /// system will error on the event. Events are by default GuaranteedOrdered, however,
   /// the default direction is unset which will result in asserts.
   NetEvent(GuaranteeType gType = GuaranteedOrdered, EventDirection evDir = DirUnset)
   {
      mGuaranteeType = gType;
      mEventDirection = evDir;
   }

   /// Pack is called on the origin side of the connection to write an event's
   /// data into a packet.
   virtual void pack(EventConnection *ps, BitStream *bstream) = 0;

   /// Unpack is called on the destination side of the connection to read an event's
   /// data out of a packet.
   virtual void unpack(EventConnection *ps, BitStream *bstream) = 0;

   /// Process is called to process the event data when it has been unpacked.
   ///
   /// For a guaranteed, ordered event, process is called only once all prior events
   /// have been received and processed.  For unguaranteed events, process is called
   /// immediately after unpack.
   virtual void process(EventConnection *ps) = 0;

   /// notifyPosted is called on an event when it is posted to a particular EventConnection, before it is added to the send queue.  
   /// This allows events to post additional events to the connection that will be send _before_ this event
   virtual void notifyPosted(EventConnection *ps) {}

   /// notifySent is called on each event after all of the events for a packet have
   /// been written into the packet stream.
   virtual void notifySent(EventConnection *ps) {}

   /// notifyDelivered is called on the source event after it has been received
   /// and processed by the other side of the connection.
   ///
   /// If the packet delivery fails on an unguaranteed event, madeIt will be
   /// false, otherwise it will be true.
   virtual void notifyDelivered(EventConnection *ps, bool madeIt) {}

   /// getEventDirection returns the direction this event is allowed to travel in on a connection
   EventDirection getEventDirection()
   {
      return mEventDirection;
   }

   /// getDebugName is used to construct event names for packet
   /// logging in debug mode.
   virtual const char *getDebugName()
   {
      return getClassName();
   }
};


/// The IMPLEMENT_NETEVENT macro is used for implementing events
/// that can be sent from server to client or from client to server
#define TNL_IMPLEMENT_NETEVENT(className,groupMask,classVersion) \
   TNL::NetClassRep* className::getClassRep() const { return &className::dynClassRep; } \
   TNL::NetClassRepInstance<className> className::dynClassRep(#className,groupMask, TNL::NetClassTypeEvent, classVersion)

};

#endif

// --- END OF FILE: tnl/tnlNetEvent.h ---
// --- START OF FILE: tnl/tnlNetInterface.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NETINTERFACE_H_
#define _TNL_NETINTERFACE_H_

#ifndef _TNL_VECTOR_H_
#include "tnlVector.h"
#endif

#ifndef _TNL_NETBASE_H_
#include "tnlNetBase.h"
#endif

#include "tnlClientPuzzle.h"

#ifndef _TNL_NETOBJECT_H_
#include "tnlNetObject.h"
#endif

#ifndef _TNL_NETCONNECTION_H_
#include "tnlNetConnection.h"
#endif

namespace TNL {

class AsymmetricKey;
class Certificate;
struct ConnectionParameters;

/// NetInterface class.
///
/// Manages all valid and pending notify protocol connections for a port/IP. If you are
/// providing multiple services or servicing multiple networks, you may have more than
/// one NetInterface.
///
/// <b>Connection handshaking basic overview:</b>
///
/// TNL does a two phase connect handshake to prevent a several types of
/// Denial-of-Service (DoS) attacks.
///
/// The initiator of the connection (client) starts the connection by sending
/// a unique random nonce (number, used once) value to the server as part of
/// the ConnectChallengeRequest packet.
/// C->S: ConnectChallengeRequest, Nc
///
/// The server responds to the ConnectChallengeRequest with a "Client Puzzle"
/// that has the property that verifying a solution to the puzzle is computationally
/// simple, but can be of a specified computational, brute-force difficulty to
/// compute the solution itself.  The client puzzle is of the form:
/// secureHash(Ic, Nc, Ns, X) = Y >> k, where Ic is the identity of the client,
/// and X is a value computed by the client such that the high k bits of the value
/// y are all zero.  The client identity is computed by the server as a partial hash
/// of the client's IP address and port and some random data on the server.
/// its current nonce (Ns), Nc, k, and the server's authentication certificate.
/// S->C: ConnectChallengeResponse, Nc, Ns, Ic, Cs
///
/// The client, upon receipt of the ConnectChallengeResponse, validates the packet
/// sent by the server and computes a solution to the puzzle the server sent.  If
/// the connection is to be authenticated, the client can also validate the server's
/// certificate (if it's been signed by a Certificate Authority), and then generates
/// a shared secret from the client's key pair and the server's public key.  The client
/// response to the server consists of:
/// C->S: ConnectRequest, Nc, Ns, X, Cc, sharedSecret(key1, sequence1, NetConnectionClass, class-specific sendData)
///
/// The server then can validation the solution to the puzzle the client submitted, along
/// with the client identity (Ic).
/// Until this point the server has allocated no memory for the client and has
/// verified that the client is sending from a valid IP address, and that the client
/// has done some amount of work to prove its willingness to start a connection.
/// As the server load increases, the server may choose to increase the difficulty (k) of
/// the client puzzle, thereby making a resource depletion DoS attack successively more
/// difficult to launch.
///
/// If the server accepts the connection, it sends a connect accept packet that is
/// encrypted and hashed using the shared secret.  The contents of the packet are
/// another sequence number (sequence2) and another key (key2).  The sequence numbers 
/// are the initial send and receive sequence numbers for the connection, and the
/// key2 value becomes the IV of the symmetric cipher.  The connection subclass is
/// also allowed to write any connection specific data into this packet.
///
/// This system can operate in one of 3 ways: unencrypted, encrypted key exchange (ECDH),
/// or encrypted key exchange with server and/or client signed certificates (ECDSA).
/// 
/// The unencrypted communication mode is NOT secure.  Packets en route between hosts
/// can be modified without detection by the hosts at either end.  Connections using
/// the secure key exchange are still vulnerable to Man-in-the-middle attacks, but still
/// much more secure than the unencrypted channels.  Using certificate(s) signed by a
/// trusted certificate authority (CA), makes the communications channel as securely
/// trusted as the trust in the CA.
///
/// <b>Arranged Connection handshaking:</b>
///
/// NetInterface can also facilitate "arranged" connections.  Arranged connections are
/// necessary when both parties to the connection are behind firewalls or NAT routers.
/// Suppose there are two clients, A and B that want to esablish a direct connection with
/// one another.  If A and B are both logged into some common server S, then S can send
/// A and B the public (NAT'd) address, as well as the IP addresses each client detects
/// for itself.
///
/// A and B then both send "Punch" packets to the known possible addresses of each other.
/// The punch packet client A sends enables the punch packets client B sends to be 
/// delivered through the router or firewall since it will appear as though it is a service
/// response to A's initial packet.
///
/// Upon receipt of the Punch packet by the "initiator"
/// of the connection, an ArrangedConnectRequest packet is sent.
/// if the non-initiator of the connection gets an ArrangedPunch
/// packet, it simply sends another Punch packet to the
/// remote host, but narrows down its Address range to the address
/// it received the packet from.
/// The ArrangedPunch packet from the intiator contains the nonce 
/// for the non-initiator, and the nonce for the initiator encrypted
/// with the shared secret.
/// The ArrangedPunch packet for the receiver of the connection
/// contains all that, plus the public key/keysize or the certificate
/// of the receiver.


class NetInterface : public Object
{
   friend class NetConnection;
public:
   /// PacketType is encoded as the first byte of each packet.
   ///
   /// Subclasses of NetInterface can add custom, non-connected data
   /// packet types starting at FirstValidInfoPacketId, and overriding 
   /// handleInfoPacket to process them.
   ///
   /// Packets that arrive with the high bit of the first byte set
   /// (i.e. the first unsigned byte is greater than 127), are
   /// assumed to be connected protocol packets, and are dispatched to
   /// the appropriate connection for further processing.

   enum PacketType
   {
      ConnectChallengeRequest       = 0, ///< Initial packet of the two-phase connect process
      ConnectChallengeResponse      = 1, ///< Response packet to the ChallengeRequest containing client identity, a client puzzle, and possibly the server's public key.
      ConnectRequest                = 2, ///< A connect request packet, including all puzzle solution data and connection initiation data.
      ConnectReject                 = 3, ///< A packet sent to notify a host that a ConnectRequest was rejected.
      ConnectAccept                 = 4, ///< A packet sent to notify a host that a connection was accepted.
      Disconnect                    = 5, ///< A packet sent to notify a host that the specified connection has terminated.
      Punch                         = 6, ///< A packet sent in order to create a hole in a firewall or NAT so packets from the remote host can be received.
      ArrangedConnectRequest        = 7, ///< A connection request for an "arranged" connection.
      FirstValidInfoPacketId        = 8, ///< The first valid ID for a NetInterface subclass's info packets.
   };

protected:
   Vector<NetConnection *> mConnectionList;      ///< List of all the connections that are in a connected state on this NetInterface.
   Vector<NetConnection *> mConnectionHashTable; ///< A resizable hash table for all connected connections.  This is a flat hash table (no buckets).

   Vector<NetConnection *> mPendingConnections; ///< List of connections that are in the startup state, where the remote host has not fully
                                                ///  validated the connection.

   RefPtr<AsymmetricKey> mPrivateKey;  ///< The private key used by this NetInterface for secure key exchange.
   RefPtr<Certificate> mCertificate;   ///< A certificate, signed by some Certificate Authority, to authenticate this host.
   ClientPuzzleManager mPuzzleManager; ///< The object that tracks the current client puzzle difficulty, current puzzle and solutions for this NetInterface.

   /// @name NetInterfaceSocket Socket
   ///
   /// State regarding the socket this NetInterface controls.
   ///
   /// @{

   ///
   Socket    mSocket;   ///< Network socket this NetInterface communicates over.

   /// @}

   U32 mCurrentTime;            ///< Current time tracked by this NetInterface.
   bool mRequiresKeyExchange;   ///< True if all connections outgoing and incoming require key exchange.
   U32  mLastTimeoutCheckTime;  ///< Last time all the active connections were checked for timeouts.
   U8  mRandomHashData[12];    ///< Data that gets hashed with connect challenge requests to prevent connection spoofing.
   bool mAllowConnections;      ///< Set if this NetInterface allows connections from remote instances.

   /// Structure used to track packets that are delayed in sending for simulating a high-latency connection.
   ///
   /// The DelaySendPacket is allocated as sizeof(DelaySendPacket) + packetSize;
   struct DelaySendPacket
   {
      DelaySendPacket *nextPacket; ///< The next packet in the list of delayed packets.
      Address remoteAddress;    ///< The address to send this packet to.
      U32 sendTime;                ///< Time when we should send the packet.
      U32 packetSize;              ///< Size, in bytes, of the packet data.
      U8 packetData[1];            ///< Packet data.
   };
   DelaySendPacket *mSendPacketList; ///< List of delayed packets pending to send.

   enum NetInterfaceConstants {
      ChallengeRetryCount = 4,     ///< Number of times to send connect challenge requests before giving up.
      ChallengeRetryTime = 2500,   ///< Timeout interval in milliseconds before retrying connect challenge.

      ConnectRetryCount = 4,       ///< Number of times to send connect requests before giving up.
      ConnectRetryTime = 2500,     ///< Timeout interval in milliseconds before retrying connect request.

      PunchRetryCount = 6,         ///< Number of times to send groups of firewall punch packets before giving up.
      PunchRetryTime = 2500,       ///< Timeout interval in milliseconds before retrying punch sends.

      TimeoutCheckInterval = 1500, ///< Interval in milliseconds between checking for connection timeouts.
      PuzzleSolutionTimeout = 30000, ///< If the server gives us a puzzle that takes more than 30 seconds, time out.
   };

   /// Computes an identity token for the connecting client based on the address of the client and the
   /// client's unique nonce value.
   U32 computeClientIdentityToken(const Address &theAddress, const Nonce &theNonce);

   /// Finds a connection instance that this NetInterface has initiated.
   NetConnection *findPendingConnection(const Address &address);

   /// Adds a connection the list of pending connections.
   void addPendingConnection(NetConnection *conn);

   /// Removes a connection from the list of pending connections.
   void removePendingConnection(NetConnection *conn);

   /// Finds a connection by address from the pending list and removes it.
   void findAndRemovePendingConnection(const Address &address);

   /// Adds a connection to the internal connection list.
   void addConnection(NetConnection *connection);

   /// Remove a connection from the list.
   void removeConnection(NetConnection *connection);

   /// Begins the connection handshaking process for a connection.  Called from NetConnection::connect()
   void startConnection(NetConnection *conn);

   /// Sends a connect challenge request on behalf of the connection to the remote host.
   void sendConnectChallengeRequest(NetConnection *conn);

   /// Handles a connect challenge request by replying to the requestor of a connection with a
   /// unique token for that connection, as well as (possibly) a client puzzle (for DoS prevention),
   /// or this NetInterface's public key.
   void handleConnectChallengeRequest(const Address &addr, BitStream *stream);

   /// Sends a connect challenge request to the specified address.  This can happen as a result
   /// of receiving a connect challenge request, or during an "arranged" connection for the non-initiator
   /// of the connection.
   void sendConnectChallengeResponse(const Address &addr, Nonce &clientNonce, bool wantsKeyExchange, bool wantsCertificate);

   /// Processes a ConnectChallengeResponse, by issueing a connect request if it was for
   /// a connection this NetInterface has pending.
   void handleConnectChallengeResponse(const Address &address, BitStream *stream);

   /// Continues computation of the solution of a client puzzle, and issues a connect request
   /// when the solution is found.
   void continuePuzzleSolution(NetConnection *conn);

   /// Sends a connect request on behalf of a pending connection.
   void sendConnectRequest(NetConnection *conn);

   /// Handles a connection request from a remote host.
   ///
   /// This will verify the validity of the connection token, as well as any solution
   /// to a client puzzle this NetInterface sent to the remote host.  If those tests
   /// pass, it will construct a local connection instance to handle the rest of the
   /// connection negotiation.
   void handleConnectRequest(const Address &address, BitStream *stream);

   /// Sends a connect accept packet to acknowledge the successful acceptance of a connect request.
   void sendConnectAccept(NetConnection *conn);

   /// Handles a connect accept packet, putting the connection associated with the
   /// remote host (if there is one) into an active state.
   void handleConnectAccept(const Address &address, BitStream *stream);

   /// Sends a connect rejection to a valid connect request in response to possible error
   /// conditions (server full, wrong password, etc).
   void sendConnectReject(ConnectionParameters *theParams, const Address &theAddress, const char *reason);

   /// Handles a connect rejection packet by notifying the connection object
   /// that the connection was rejected.
   void handleConnectReject(const Address &address, BitStream *stream);

   /// Begins the connection handshaking process for an arranged connection.
   void startArrangedConnection(NetConnection *conn);

   /// Sends Punch packets to each address in the possible connection address list.
   void sendPunchPackets(NetConnection *conn);

   /// Handles an incoming Punch packet from a remote host.
   void handlePunch(const Address &theAddress, BitStream *stream);

   /// Sends an arranged connect request.
   void sendArrangedConnectRequest(NetConnection *conn);

   /// Handles an incoming connect request from an arranged connection.
   void handleArrangedConnectRequest(const Address &theAddress, BitStream *stream);
   
   /// Dispatches a disconnect packet for a specified connection.
   void handleDisconnect(const Address &address, BitStream *stream);

   /// Handles an error reported while reading a packet from this remote connection.
   void handleConnectionError(NetConnection *theConnection, const char *errorString);

   /// Disconnects the given connection and removes it from the NetInterface
   void disconnect(NetConnection *conn, NetConnection::TerminationReason reason, const char *reasonString);
   /// @}
public:
   /// @param   bindAddress    Local network address to bind this interface to.
   NetInterface(const Address &bindAddress);
   ~NetInterface();

   /// Returns the address of the first network interface in the list that the socket on this NetInterface is bound to.
   Address getFirstBoundInterfaceAddress();

   /// Sets the private key this NetInterface will use for authentication and key exchange
   void setPrivateKey(AsymmetricKey *theKey);

   /// Requires that all connections use encryption and key exchange
   void setRequiresKeyExchange(bool requires) { mRequiresKeyExchange = requires; }

   /// Sets the public certificate that validates the private key and stores
   /// information about this host.  If no certificate is set, this interface can
   /// still initiate and accept encrypted connections, but they will be vulnerable to
   /// man in the middle attacks, unless the remote host can validate the public key
   /// in another way.
   void setCertificate(Certificate *theCertificate);

   /// Returns whether or not this NetInterface allows connections from remote hosts.
   bool doesAllowConnections() { return mAllowConnections; }

   /// Sets whether or not this NetInterface allows connections from remote hosts.
   void setAllowsConnections(bool conn) { mAllowConnections = conn; }

   /// Returns the Socket associated with this NetInterface
   Socket &getSocket() { return mSocket; }

   /// Sends a packet to the remote address over this interface's socket.
   NetError sendto(const Address &address, BitStream *stream);

   /// Sends a packet to the remote address after millisecondDelay time has elapsed.
   ///
   /// This is used to simulate network latency on a LAN or single computer.
   void sendtoDelayed(const Address &address, BitStream *stream, U32 millisecondDelay);

   /// Dispatch function for processing all network packets through this NetInterface.
   void checkIncomingPackets();

   /// Processes a single packet, and dispatches either to handleInfoPacket or to
   /// the NetConnection associated with the remote address.
   virtual void processPacket(const Address &address, BitStream *packetStream);

   /// Handles all packets that don't fall into the category of connection handshake or game data.
   virtual void handleInfoPacket(const Address &address, U8 packetType, BitStream *stream);

   /// Checks all connections on this interface for packet sends, and for timeouts and all valid
   /// and pending connections.
   void processConnections();

   /// Returns the list of connections on this NetInterface.
   Vector<NetConnection *> &getConnectionList() { return mConnectionList; }

   /// looks up a connected connection on this NetInterface
   NetConnection *findConnection(const Address &remoteAddress);

   /// returns the current process time for this NetInterface
   U32 getCurrentTime() { return mCurrentTime; }
};

};

#endif

// --- END OF FILE: tnl/tnlNetInterface.h ---
// --- START OF FILE: tnl/tnlNetObject.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NETOBJECT_H_
#define _TNL_NETOBJECT_H_

#ifndef _TNL_NETBASE_H_
#include "tnlNetBase.h"
#endif

#ifndef _TNL_BITSTREAM_H_
#include "tnlBitStream.h"
#endif

#ifndef _TNL_RPC_H_
#include "tnlRPC.h"
#endif

namespace TNL {
//----------------------------------------------------------------------------
class GhostConnection;
class NetInterface;
class NetObjectRPCEvent;

struct GhostInfo;

//-----------------------------------------------------------------------------
/// Superclass for ghostable networked objects.
///
/// @section NetObject_intro Introduction To NetObject And Ghosting
///
/// One of the most powerful aspects of the Torque Network Library is its support
/// for ghosting and prioritized, most-recent-state network updates. The way
/// this works is a bit complex, but it is immensely efficient. Let's run
/// through the steps that the server goes through for each client in this part
/// of TNL's architecture:
///      - First, the server determines what objects are in-scope for the client.
///        This is done by calling performScopeQuery() on the object which is
///        considered the "scope" object. This could be a simulation avatar of the character,
///        a flyby camera, a vehicle the user is controlling, or something else.
///      - Second, it instructs the client to create "ghost" instances of those objects
///        to represent the source objects on the server.  Finally, it sends updates
///        to the ghosts of those objects whose state has been updated on the server,
///        prioritizing the updates based on how relevant the object is to that particular client.
///
/// There several significant advantages to using this networking system:
///      - Efficient network usage, since we only send data that has changed. In addition,
///        since we only care about most-recent data, if a packet is dropped, we don't waste
///        effort trying to deliver stale data.
///      - Cheating protection; since TNL doesn't deliver information about game objects which
///        aren't "in scope", the ability for clients to learn about objects outside their immediate
///        perceptions can be curtailed by an agressive scoping function.
///
/// @section NetObject_Implementation An Example Implementation
///
/// The basis of the ghost implementation in TNL is NetObject.  Each NetObject maintains an <b>updateMask</b>,
/// a 32 bit word representing up to 32 independent states for the object.  When a NetObject's state changes
/// it calls the setMaskBits method to notify the network layer that the state has changed and needs to be
/// updated on all clients that have that NetObject in scope.
///
/// Using a NetObject is very simple; let's go through a simple example implementation:
///
/// @code
/// class SimpleNetObject : public NetObject
/// {
/// public:
///   typedef NetObject Parent;
///   TNL_DECLARE_CLASS(SimpleNetObject);
/// @endcode
///
/// Above is the standard boilerplate code for a NetObject subclass.
///
/// @code
///    char message1[256];
///    char message2[256];
///    enum States {
///       Message1Mask = BIT(0),
///       Message2Mask = BIT(1),
///    };
/// @endcode
///
/// The example class has two object "states" that each instance keeps track of, message1 and message2.
/// A real game object might have states for health, velocity and position, or some other set of fields.
/// Each class has 32 bits to work with, so it's possible to be very specific when defining states.
/// In general, individual state bits should be assigned only to things that are updated independently -
/// so if you update the position field and the velocity at the same time always, you could use a single
/// bit to represent that state change.
///
/// @code
///    SimpleNetObject()
///    {
///       // in order for an object to be considered by the network system,
///       // the Ghostable net flag must be set.
///       // the ScopeAlways flag indicates that the object is always scoped
///       // on all active connections.
///       mNetFlags.set(ScopeAlways | Ghostable);
///       strcpy(message1, "Hello World 1!");
///       strcpy(message2, "Hello World 2!");
///    }
/// @endcode
///
/// Here is the constructor. The code initializes the net flags, indicating that
/// the SimpleNetObject should always be scoped, and that it can be ghosted to remote hosts
///
/// @code
///    U32 packUpdate(GhostConnection *, U32 mask, BitStream *stream)
///    {
///       // check which states need to be updated, and write updates
///       if(stream->writeFlag(mask & Message1Mask))
///          stream->writeString(message1);
///       if(stream->writeFlag(mask & Message2Mask))
///          stream->writeString(message2);
///
///       // the return value from packUpdate can set which states still
///       // need to be updated for this object.
///       return 0;
///    }
/// @endcode
///
/// Here's half of the meat of the networking code, the packUpdate() function. (The other half, unpackUpdate(),
/// is shown below.) The comments in the code pretty much explain everything, however, notice that the
/// code follows a pattern of if(writeFlag(mask & StateMask)) { ... write data ... }. The packUpdate()/unpackUpdate()
/// functions are responsible for reading and writing the update flags to the BitStream.  This means the GhostConnection
/// doesn't have to send the 32 bit updateMask with every packet.
///
/// @code
///    void unpackUpdate(GhostConnection *, BitStream *stream)
///    {
///       // the unpackUpdate function must be symmetrical to packUpdate
///       if(stream->readFlag())
///       {
///          stream->readString(message1);
///          logprintf("Got message1: %s", message1);
///       }
///       if(stream->readFlag())
///       {
///          stream->readString(message2);
///          logprintf("Got message2: %s", message2);
///       }
///    }
/// @endcode
///
/// The other half of the networking code in any NetObject, unpackUpdate(). In SimpleNetObject, all
/// the code does is print the new messages to the log; however, in a more advanced object, the code might
/// trigger animations, update complex object properties, or even spawn new objects, based on what packet
/// data is unpacked.
///
/// @code
///    void setMessage1(const char *msg)
///    {
///       setMaskBits(Message1Mask);
///       strcpy(message1, msg);
///    }
///    void setMessage2(const char *msg)
///    {
///       setMaskBits(Message2Mask);
///       strcpy(message2, msg);
///    }
/// @endcode
///
/// Here are the accessors for the two properties. It is good to encapsulate state
/// variables, so that you don't have to remember to make a call to setMaskBits every time you change
/// anything; the accessors can do it for you. In a more complex object, you might need to set
/// multiple mask bits when you change something; this can be done using the | operator, for instance,
/// setMaskBits( Message1Mask | Message2Mask ); if you changed both messages.
///
/// @code
/// TNL_IMPLEMENT_NETOBJECT(SimpleNetObject);
/// @endcode
///
/// Finally, we use the NetObject implementation macro, TNL_IMPLEMENT_NETOBJECT(), to implement our
/// NetObject. It is important that we use this, as it makes TNL perform certain initialization tasks
/// that allow us to send the object over the network. TNL_IMPLEMENT_CLASS() doesn't perform these tasks, see
/// the documentation on NetClassRep for more details.
///
/// @nosubgrouping

class NetObject : public Object
{
   friend class GhostConnection;
   friend class GhostAlwaysObjectEvent;
   friend class NetObjectRPCEvent;

   typedef Object Parent;

   NetObject *mPrevDirtyList;
   NetObject *mNextDirtyList;
   U32 mDirtyMaskBits;

   static NetObject *mDirtyList;
   U32 mNetIndex;              ///< The index of this ghost on the other side of the connection.
   GhostInfo *mFirstObjectRef; ///< Head of the linked list of GhostInfos for this object.

   static bool mIsInitialUpdate; ///< Managed by GhostConnection - set to true when this is an initial update
   SafePtr<NetObject> mServerObject; ///< Direct pointer to the parent object on the server if it is a local connection
   GhostConnection *mOwningConnection; ///< The connection that owns this ghost, if it's a ghost
protected:
   enum NetFlag
   {
    IsGhost =              BIT(1),  ///< Set if this is a ghost.
      ScopeLocal =         BIT(2),  ///< If set, this object ghosts only to the local client.
      Ghostable =          BIT(3),  ///< Set if this object can ghost at all.
    MaxNetFlagBit = 15
   };

   BitSet32 mNetFlags;  ///< Flags field describing this object, from NetFlag.

   /// RPC method source connection
   static GhostConnection *mRPCSourceConnection;

   /// NetObject RPC method destination connection.
   static GhostConnection *mRPCDestConnection;

   /// Returns true if this pack/unpackUpdate is the initial one for the object
   bool isInitialUpdate() { return mIsInitialUpdate; }
public:
   NetObject();
   ~NetObject();

   /// collapseDirtyList pushes all the mDirtyMaskBits down into
   /// the GhostInfo's for each object, and clears out the dirty
   /// list.
   static void collapseDirtyList();

   /// Returns the connection from which the current RPC method originated,
   /// or NULL if not currently within the processing of an RPC method call.
   static GhostConnection *getRPCSourceConnection() { return mRPCSourceConnection; }

   /// Sets the connection to which all NetObject RPCs will be destined.  Calling this function
   /// with a NULL value will target NetObject RPCs to every connection for which that object is
   /// currently ghosted.
   static void setRPCDestConnection(GhostConnection *destConnection) { mRPCDestConnection = destConnection; }

   /// Returns the connection that serves as the destination of NetObject RPC method calls.
   static GhostConnection *getRPCDestConnection() { return mRPCDestConnection; }

   /// onGhostAdd is called on the client side of a connection after
   /// the constructor and after the first call to unpackUpdate (the
   /// initial call).  Returning true signifies no error - returning
   /// false causes the connection to abort.
   virtual bool onGhostAdd(GhostConnection *theConnection);

   /// onGhostRemove is called on the client side before the destructor
   /// when ghost has gone out of scope and is about to be deleted from the client.
   virtual void onGhostRemove();

   /// onGhostAvailable is called on the server side after the server knows that
   /// the ghost is available and addressable via the getGhostIndex().
   virtual void onGhostAvailable(GhostConnection *theConnection);

   /// Notify the network system that one or more of this object's states have
   /// been changed.
   ///
   /// @note This is a server side call. It has no meaning for ghosts.
   void setMaskBits(U32 orMask);

   /// Notify the network system that one or more of its states does not
   /// need to be updated.
   ///
   /// @note This is a server side call. It has no meaning for ghosts.
   void clearMaskBits(U32 orMask);

   /// Called to determine the relative update priority of an object.
   ///
   /// All objects that are in scope and that have out of date
   /// states are queried and sorted by priority before being updated.  If there
   /// is not enough room in a single packet for all out of date objects, the skipped
   /// objects will have an incremented updateSkips the next time that connection
   /// prepares to send a packet. Typically the update priority is scaled by
   /// updateSkips so that as data becomes stale, it becomes more of a priority to
   /// update.
   virtual F32 getUpdatePriority(NetObject *scopeObject, U32 updateMask, S32 updateSkips);

   /// Write the object's state to a packet.
   ///
   /// packUpdate is called on an object when it is to be written into a
   /// packet stream for transmission to the client.  The updateMask parameter
   /// contains the out-of-date state mask for the object.  The initial update mask
   /// for any object to a client will always be 0xFFFFFFFF, signifying that all
   /// states are out of date. It is often useful to check for this mask, to write
   /// one-time initialization information for that object.
   virtual U32  packUpdate(GhostConnection *connection, U32 updateMask, BitStream *stream);

   /// Unpack data written by packUpdate().
   ///
   /// unpackUpdate is called on the client to read an update out of a
   /// packet's bit stream.  Because the update mask is not encoded by
   /// the network system directly, each unpack update function will have
   /// to determine from the bit stream which states are being updated.
   virtual void unpackUpdate(GhostConnection *connection, BitStream *stream);

   /// For a scope object, determine what is in scope.
   ///
   /// performScopeQuery is called on a NetConnection's scope object
   /// to determine which objects in the world are in scope for that
   /// connection.
   virtual void performScopeQuery(GhostConnection *connection);

   /// getNetIndex returns the index tag used to identify the server copy
   /// of a client object.
   U32 getNetIndex() { return mNetIndex; }

   /// isGhost returns true if this object is a ghost of a server object.
   bool isGhost() const;

   /// isScopeLocal returns true if this object is scoped always to the
   /// local client.
   bool isScopeLocal() const;

   /// isGhostable returns true if this object can be ghosted to any clients.
   bool isGhostable() const;

   /// Return a hash for this object.
   ///
   /// @note This is based on its location in memory.
   U32 getHashId() const;

   /// Internal method called by NetObject RPC events when they are packed.
   void postRPCEvent(NetObjectRPCEvent *theEvent);

};

inline bool NetObject::isGhost() const
{
    return mNetFlags.test(IsGhost);
}

inline bool NetObject::isScopeLocal() const
{
    return mNetFlags.test(ScopeLocal);
}

inline bool NetObject::isGhostable() const
{
    return mNetFlags.test(Ghostable);
}

inline U32 NetObject::getHashId() const
{
   const NetObject *ret = this;
   return *((U32 *) &ret);
}

/// The TNL_IMPLEMENT_NETOBJECT macro should be used for all subclasses of NetObject that
/// will be transmitted with the ghosting system.
#define TNL_IMPLEMENT_NETOBJECT(className) \
   TNL::NetClassRep* className::getClassRep() const { return &className::dynClassRep; } \
   TNL::NetClassRepInstance<className> className::dynClassRep(#className, TNL::NetClassGroupGameMask, TNL::NetClassTypeObject, 0)

/// Direction that a NetObject RPC method call should travel.
enum NetObjectRPCDirection {
   RPCToGhost = 1,
   RPCToGhostParent = 2,
};

/// Event class for handling RPC method invocations on NetObjects.
///
/// When an RPC method is called on the server side NetObject, it
/// is broadcast to all connections that have that NetObject in scope.
/// When an RPC method is called on a ghost (on the client), it is
/// handled by the originating server object.
class NetObjectRPCEvent : public RPCEvent
{
public:
   /// Destination object of the RPC invocation
   SafePtr<NetObject> mDestObject;

   /// Direction this RPC should travel
   NetObjectRPCDirection mRPCDirection;

   /// Constructor - initializes the base class's data
   NetObjectRPCEvent(NetObject *destObject, RPCGuaranteeType type, NetObjectRPCDirection dir) :
      RPCEvent(type, RPCDirAny) { mDestObject = destObject; mRPCDirection = dir; }
   void pack(EventConnection *ps, BitStream *bstream);
   void unpack(EventConnection *ps, BitStream *bstream);
   void process(EventConnection *ps);
};

/// Macro used to declare the implementation of an RPC method on a NetObject subclass.
///
/// The macro should be used in place of a member function parameter declaration,
/// with the body code (to be executed on the remote side of the RPC) immediately
/// following the TNL_IMPLEMENT_NETOBJECT_RPC macro call.
#define TNL_IMPLEMENT_NETOBJECT_RPC(className, name, args, argNames, groupMask, guaranteeType, eventDirection, rpcVersion) \
class RPCEV_##className##_##name : public TNL::NetObjectRPCEvent { \
public: \
   TNL::FunctorDecl<void (className::*)args> mFunctorDecl;\
   RPCEV_##className##_##name(TNL::NetObject *theObject = NULL) : TNL::NetObjectRPCEvent(theObject, guaranteeType, eventDirection), mFunctorDecl(&className::name##_remote) { mFunctor = &mFunctorDecl; } \
   TNL_DECLARE_CLASS( RPCEV_##className##_##name ); \
   bool checkClassType(TNL::Object *theObject) { return dynamic_cast<className *>(theObject) != NULL; } }; \
   TNL_IMPLEMENT_NETEVENT( RPCEV_##className##_##name, groupMask, rpcVersion ); \
   void className::name args { RPCEV_##className##_##name *theEvent = new RPCEV_##className##_##name(this); theEvent->mFunctorDecl.set argNames ; postRPCEvent(theEvent); } \
   TNL::NetEvent * className::name##_construct args { RPCEV_##className##_##name *theEvent = new RPCEV_##className##_##name(this); theEvent->mFunctorDecl.set argNames ; return theEvent; } \
   void className::name##_remote args

};

#endif

// --- END OF FILE: tnl/tnlNetObject.h ---
// --- START OF FILE: tnl/tnlNetStringTable.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NETSTRINGTABLE_H_
#define _TNL_NETSTRINGTABLE_H_

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

namespace TNL {


typedef U32 StringTableEntryId;

//--------------------------------------
/// A global table for the hashing and tracking of network strings.
///
namespace StringTable
{
   /// Adds a string to the string table, and returns the id of the string.
   ///
   /// @param  string   String to check in the table (and add).
   /// @param  caseSens Determines whether case matters.
   StringTableEntryId insert(const char *string, bool caseSens = true);

   /// Adds a string to the string table, and returns the id of the string.
   ///
   /// @param  string   String to check in the table (and add).
   /// @param  len      Length of the string in bytes.
   /// @param  caseSens Determines whether case matters.
   StringTableEntryId insertn(const char *string, S32 len, bool caseSens = true);

   /// Determines if a string is in the string table, and returns the id of the string, or 0 if the string is not in the table.
   ///
   /// @param  string   String to check in the table (but not add).
   /// @param  caseSens Determines whether case matters.
   StringTableEntryId lookup(const char *string, bool caseSens = true);

   /// Determines if a string is in the string table, and returns the id of the string, or 0 if the string is not in the table.
   ///
   /// @param  string   String to check in the table (but not add).
   /// @param  len      Length of string in bytes.
   /// @param  caseSens Determines whether case matters.
   StringTableEntryId lookupn(const char *string, S32 len, bool caseSens = true);

   /// Hash a string into a U32.
   U32 hashString(const char* in_pString);

   /// Hash a string of given length into a U32.
   U32 hashStringn(const char* in_pString, S32 len);

   void incRef(StringTableEntryId index);   
   void decRef(StringTableEntryId index);
   const char *getString(StringTableEntryId index);
};

/// The StringTableEntry class encapsulates an entry in the network StringTable.
/// StringTableEntry instances offer several benefits over normal strings in
/// a networked simulation.  First, the ConnectionStringTable class is able to
/// cache string transfers, meaning that recently used strings will be transmitted
/// as string data once and then referred to by a short id bitfield thereafter.
/// String comparison operations are also much less expensive with StringTableEntry
/// instances as they result in just a single integer comparison.

class StringTableEntry {
private:
   StringTableEntryId mIndex; ///< index of the string table entry in the master pointer list
public:
    /// empty constructor gets NULL string automatically.
   inline StringTableEntry()
   { 
      mIndex = 0;
   }
   inline StringTableEntry(const char *string, bool caseSensitive = true)
   {
      mIndex = StringTable::insert(string, caseSensitive);
   }
   inline StringTableEntry(const StringTableEntry &theString)
   {
      mIndex = theString.mIndex;
      incRef();
   }
   StringTableEntry &operator= (const StringTableEntry &s)
   {
      decRef();
      mIndex = s.mIndex;
      incRef();
      return *this;
   }
   StringTableEntry &operator= (const char *string)
   {
      decRef();
      mIndex = StringTable::insert(string);
      return *this;
   }

   inline void set(const char *string, bool caseSensitive = true)
   {
      decRef();
      mIndex = StringTable::insert(string, caseSensitive);
   }

   inline void setn(const char *string, U32 len, bool caseSensitive = true)
   {
      decRef();
      mIndex = StringTable::insertn(string, len, caseSensitive);
   }

   bool operator== (const StringTableEntry &s) const
   { 
      return s.mIndex == mIndex;
   }
   bool operator!= (const StringTableEntry &s) const
   {
      return s.mIndex != mIndex;
   }
   inline bool isNull() const
   {
      return mIndex == 0;
   }
   inline bool isNotNull() const
   {
      return mIndex != 0;
   }
   inline bool isValid() const
   {
      return mIndex != 0;
   }

   operator bool () const
   {
      return mIndex != 0;
   }

   inline void decRef()
   {
      if(mIndex)
         StringTable::decRef(mIndex);
   }

   inline void incRef()
   {
      if(mIndex)
         StringTable::incRef(mIndex);
   }

   inline U32 getIndex() const 
   { 
      return mIndex;
   }
   
   inline const char *getString() const
   {
      return StringTable::getString(mIndex);
   }
};

typedef const StringTableEntry &StringTableEntryRef;

};

#endif

// --- END OF FILE: tnl/tnlNetStringTable.h ---
// --- START OF FILE: tnl/tnlNonce.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_NONCE_H_
#define _TNL_NONCE_H_

#ifndef _TNL_BITSTREAM_H_
#include "tnlBitStream.h"
#endif

#ifndef _TNL_RANDOM_H_
#include "tnlRandom.h"
#endif

namespace TNL
{

struct Nonce
{
   enum {
      NonceSize = 8,
   };
   U8 data[NonceSize];

   Nonce() {}
   Nonce(const U8 *ptr) { memcpy(data, ptr, NonceSize); }

   bool operator==(const Nonce &theOtherNonce) const { return !memcmp(data, theOtherNonce.data, NonceSize); }
   bool operator!=(const Nonce &theOtherNonce) const { return memcmp(data, theOtherNonce.data, NonceSize) != 0; }

   void operator=(const Nonce &theNonce) { memcpy(data, theNonce.data, NonceSize); }
   
   void read(BitStream *stream) { stream->read(NonceSize, data); }
   void write(BitStream *stream) const { stream->write(NonceSize, data); }
   void getRandom() { Random::read(data, NonceSize); }
};

};

#endif

// --- END OF FILE: tnl/tnlNonce.h ---
// --- START OF FILE: tnl/tnlPlatform.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_PLATFORM_H_
#define _TNL_PLATFORM_H_

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

namespace TNL {

/// Platform specific functionality is gathered here to enable easier porting.
///
/// If you are embedding TNL in a complex application, you may want to replace
/// these with hooks to your own code - for instance the alerts really should
/// bring up an actual message box; currently they just emit a log item.
namespace Platform
{
   /// Prints a string to the OS specific debug log.
   void outputDebugString(const char *string);

   /// Stops the program in the debugger, if it is being debugged.
   void debugBreak();

   /// Forces the program to exit immediately.
   ///
   /// @note This is probably a bit strong for a networking library. -- BJG
   void forceQuit();

   /// Brings up a dialog window with a message and an "OK" button
   void AlertOK(const char *windowTitle, const char *message);

   /// Brings up a dialog window with the message, and "OK" and "Cancel" buttons
   bool AlertOKCancel(const char *windowTitle, const char *message);

   /// Brings up a dialog window with the message, and "Retry" and "Cancel" buttons
   bool AlertRetry(const char *windowTitle, const char *message);

   /// Elapsed time in milliseconds.
   ///
   /// Usually since last reboot, but it varies from platform to platform. It is
   /// guaranteed to always increase, however, up to the limit of a U32 - about
   /// 7 weeks worth of time. If you are developing a server you want to run for longer
   /// than that, prepared to see wraparounds.
   U32 getRealMilliseconds();

   /// Returns a high-precision time value, in a platform-specific time value
   S64 getHighPrecisionTimerValue();

   /// Converts a high precision timer delta into milliseconds
   F64 getHighPrecisionMilliseconds(S64 timerDelta);

   /// Put the process to sleep for the specified millisecond interva.
   void sleep(U32 msCount);

   /// checks the status of the memory allocation heap
   bool checkHeap();
};


#define TIME_BLOCK(name,block) { S64 st = Platform::getHighPrecisionTimerValue(); {block} S64 delta = Platform::getHighPrecisionTimerValue() - st; F64 ms = Platform::getHighPrecisionMilliseconds(delta); logprintf("Timer: %s Elapsed: %g ms", #name, ms); }

#if defined (TNL_SUPPORTS_VC_INLINE_X86_ASM) || defined (TNL_SUPPORTS_MWERKS_INLINE_X86_ASM)
#define TNL_DEBUGBREAK() { __asm { int 3 }; }
#elif defined(TNL_SUPPORTS_GCC_INLINE_X86_ASM)
#define TNL_DEBUGBREAK() { asm ( "int $3"); }
#else
/// Macro to do in-line debug breaks, used for asserts.  Does inline assembly where appropriate
#define TNL_DEBUGBREAK() Platform::debugBreak();
#endif

#define TNL_CHECK_HEAP() { bool status = TNL::Platform::checkHeap(); if(!status) TNL_DEBUGBREAK(); }

extern bool        atob(const char *str); ///< String to boolean conversion.

/// Printf into string with a buffer size.
///
/// This will print into the specified string until the buffer size is reached.
extern int    dSprintf(char *buffer, U32 bufferSize, const char *format, ...);

/// Vsprintf with buffer size argument.
///
/// This will print into the specified string until the buffer size is reached.
extern int    dVsprintf(char *buffer, U32 bufferSize, const char *format, void *arglist); ///< compiler independent

inline char dToupper(const char c) { if (c >= char('a') && c <= char('z')) return char(c + 'A' - 'a'); else return c; } ///< Converts an ASCII character to upper case.
inline char dTolower(const char c) { if (c >= char('A') && c <= char('Z')) return char(c - 'A' + 'a'); else return c; } ///< Converts an ASCII character to lower case.

#define QSORT_CALLBACK FN_CDECL

};

#include <string.h>
#include <stdlib.h>

#if defined (__GNUC__)

int stricmp(const char *str1, const char *str2);
int strnicmp(const char *str1, const char *str2, unsigned int len);

#endif


#endif

// --- END OF FILE: tnl/tnlPlatform.h ---
// --- START OF FILE: tnl/tnlRPC.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_RPC_H_
#define _TNL_RPC_H_

#ifndef _TNL_NETEVENT_H_
#include "tnlNetEvent.h"
#endif

#ifndef _TNL_METHODDISPATCH_H_
#include "tnlMethodDispatch.h"
#endif

namespace TNL {

/*! @page rpcdesc RPC in the Torque Network Library

The Torque Network Library has a powerful, yet simple to use Remote
Procedure Call framework for passing information through a network
connection.  Subclasses of EventConnection and NetObject can declare
member functions using the RPC macros so that when called the function 
arguments are sent to the remote EventConnection or NetObject(s) associated
with that object.

For example, suppose you have a connection class called SimpleEventConnection:
@code
class SimpleEventConnection : public EventConnection
{
public:
   TNL_DECLARE_RPC(rpcPrintString, (StringPtr theString, U32 messageCount));
};

TNL_IMPLEMENT_RPC(SimpleEventConnection, rpcPrintString, 
   (StringPtr theString, messageCount), (theString, messageCount),
   NetClassGroupGameMask, RPCGuaranteedOrdered, RPCDirAny, 0)
{
   for(U32 i = 0; i < messageCount; i++)
      printf("%s", theString.getString());
}
 
 ...
void somefunction(SimpleEventConnection *connection)
{
   connection->rpcPrintString("Hello World!", 5);
}
@endcode

In this example the class SimpleEventConnection is declared to have
a single RPC method named rpcPrintString.  The TNL_DECLARE_RPC macro
can just be viewed as a different way of declaring a class's member functions,
with the name as the first argument and the parenthesized parameter list
as the second.  Since RPC calls execute on a remote host, they never have
a return value - although a second RPC could be declared to pass messages
in the other direction.

The body of the RPC method is declared using the TNL_IMPLEMENT_RPC macro,
which has some additional arguments: the named parameter list without the types,
which NetClassMask the RPC is valid in, what level of data guarantee it uses, 
the direction it is allowed to be called on the connection and a version number.  
The body of the function, which in this case prints the passed message "Hello, World!" 5 times
to stdout, is executed on the remote host from which the method was originally invoked.

As the somefunction code demonstrates, RPC's are invoked in the same way
as any other member function in the class.

RPCs behave like virtual functions in that their bodies can be overridden
in subclasses that want to implement new behavior for the message.  The class
declaration for an overridden RPC should include the TNL_DECLARE_RPC_OVERRIDE
macro used for each method that will be redefined.  The TNL_IMPLEMENT_RPC_OVERRIDE
macro should be used outside the declaration of the class to implement
the body of the new RPC.

Internally the RPC macros construct new NetEvent classes and encapsulate the
function call arguments using the FunctorDecl template classes.  By default
the following types are allowed as parameters to RPC methods:

 - S8, U8
 - S16, U16
 - S32, U32
 - F32
 - Int<>
 - SignedInt<>
 - Float<>
 - SignedFloat<>
 - RangedU32<>
 - bool
 - StringPtr
 - StringTableEntry
 - ByteBufferPtr
 - IPAddress
 - Vector<> of all the preceding types

New types can be supported by implementing a template override for the
Types::read and Types::write functions.  All arguments to RPCs must be
passed by value (ie no reference or pointer types).

The Int, SignedInt, Float, SignedFloat and RangedU32 template types use the
template parameter(s) to specify the number of bits necessary to transmit that
variable across the network.  For example:

@code
  ...
 TNL_DECLARE_RPC(someTestFunction, (Int<4> fourBitInt, SignedFloat<7> aFloat, 
    RangedU32<100, 199> aRangedU32);
  ...
@endcode
The preceding RPC method would use 4 + 7 + 7 = 18 bits to transmit the arguments
to the function over the network, not including the RPC event overhead.
*/

/// Enumeration for valid directions that RPC messages can travel
enum RPCDirection {
   RPCDirAny            = NetEvent::DirAny,           ///< This RPC can be sent from the server or the client
   RPCDirServerToClient = NetEvent::DirServerToClient,///< This RPC can only be sent from the server to the client
   RPCDirClientToServer = NetEvent::DirClientToServer,///< This RPC can only be sent from the client to the server
};

/// Type of data guarantee this RPC should use
enum RPCGuaranteeType {
   RPCGuaranteedOrdered = NetEvent::GuaranteedOrdered, ///< RPC event delivery is guaranteed and will be processed in the order it was sent relative to other ordered events and RPCs
   RPCGuaranteed        = NetEvent::Guaranteed,        ///< RPC event delivery is guaranteed and will be processed in the order it was received
   RPCUnguaranteed      = NetEvent::Unguaranteed       ///< Event delivery is not guaranteed - however, the event will remain ordered relative to other unguaranteed events
};

/// Macro used to declare the implementation of an RPC method on an EventConnection subclass.
///
/// The macro should be used in place of a member function parameter declaration,
/// with the body code (to be executed on the remote side of the RPC) immediately
/// following the TNL_IMPLEMENT_RPC macro call.
#define TNL_IMPLEMENT_RPC(className, name, args, argNames, groupMask, guaranteeType, eventDirection, rpcVersion) \
class RPC_##className##_##name : public TNL::RPCEvent { \
public: \
   TNL::FunctorDecl<void (className::*) args > mFunctorDecl;\
   RPC_##className##_##name() : TNL::RPCEvent(guaranteeType, eventDirection), mFunctorDecl(&className::name##_remote) { mFunctor = &mFunctorDecl; } \
   TNL_DECLARE_CLASS( RPC_##className##_##name ); \
   bool checkClassType(TNL::Object *theObject) { return dynamic_cast<className *>(theObject) != NULL; } }; \
   TNL_IMPLEMENT_NETEVENT( RPC_##className##_##name, groupMask, rpcVersion ); \
   void className::name args { RPC_##className##_##name *theEvent = new RPC_##className##_##name; theEvent->mFunctorDecl.set argNames ; postNetEvent(theEvent); } \
   TNL::NetEvent * className::name##_construct args { RPC_##className##_##name *theEvent = new RPC_##className##_##name; theEvent->mFunctorDecl.set argNames ; return theEvent; } \
   void className::name##_test args { RPC_##className##_##name *theEvent = new RPC_##className##_##name; theEvent->mFunctorDecl.set argNames ; TNL::PacketStream ps; theEvent->pack(this, &ps); ps.setBytePosition(0); theEvent->unpack(this, &ps); theEvent->process(this); } \
   void className::name##_remote args

/// Base class for RPC events.
///
/// All declared RPC methods create subclasses of RPCEvent to send data across the wire
class RPCEvent : public NetEvent
{
public:
   Functor *mFunctor;
   /// Constructor call from within the rpc<i>Something</i> method generated by the TNL_IMPLEMENT_RPC macro.
   RPCEvent(RPCGuaranteeType gType, RPCDirection dir);
   void pack(EventConnection *ps, BitStream *bstream);
   void unpack(EventConnection *ps, BitStream *bstream);
   virtual bool checkClassType(Object *theObject) = 0;

   void process(EventConnection *ps);
};

/// Declares an RPC method within a class declaration.  Creates two method prototypes - one for the host side of the RPC call, and one for the receiver, which performs the actual method.
#define TNL_DECLARE_RPC(name, args) void name args; void name##_test args; virtual TNL::NetEvent * name##_construct args; virtual void name##_remote args

/// Declares an override to an RPC method declared in a parent class.
#define TNL_DECLARE_RPC_OVERRIDE(name, args) void name##_remote args

/// Macro used to declare the body of an overridden RPC method.
#define TNL_IMPLEMENT_RPC_OVERRIDE(className, name, args) \
   void className::name##_remote args

/// Constructs a NetEvent that will represent the specified RPC invocation.  This
/// macro is used to construct a single RPC that can then be posted to multiple
/// connections, instead of allocating an RPCEvent for each connection.
#define TNL_RPC_CONSTRUCT_NETEVENT(object, rpcMethod, args) (object)->rpcMethod##_construct args

};

#endif

// --- END OF FILE: tnl/tnlRPC.h ---
// --- START OF FILE: tnl/tnlRandom.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_RANDOM_H_
#define _TNL_RANDOM_H_


#ifndef _TNL_H_
#include "tnl.h"
#endif

namespace TNL {

/// The Random namespace is an interface to a cryptographically secure
/// pseudo random number generator (PRNG).  Internally the Random namespace
/// uses the Yarrow PRNG algorithm.
namespace Random {

/// Adds random "seed" data to the random number generator
void addEntropy(const U8 *randomData, U32 dataLen);

/// Reads random byte data from the random number generator
void read(U8 *outBuffer, U32 randomLen);

/// Reads a 0...U32_MAX random number from the random number generator
U32 readI();

/// Reads a random number between rangeStart and rangeEnd inclusive from the random number generator
U32 readI(U32 rangeStart, U32 rangeEnd);

/// Reads a floating point value from 0 to 1 from the random number generator
F32 readF();

/// Returns a single random bit.
bool readB();

/// Returns an opaque pointer to the random number generator's internal state
/// for use in certain encryption functions.
void *getState();
};

};

#endif //_TNL_RANDOM_H_

// --- END OF FILE: tnl/tnlRandom.h ---
// --- START OF FILE: tnl/tnlString.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_STRING_H_
#define _TNL_STRING_H_

namespace TNL
{

struct StringData
{
   U32 mRefCount;
   char mStringData[1];
};

class StringPtr
{
   StringData *mString;
   void alloc(const char *string)
   {
      mString = (StringData *) malloc(sizeof(StringData) + strlen(string));
      strcpy(mString->mStringData, string);
      mString->mRefCount = 1;
   }
   void decRef()
   {
      if(mString && !--mString->mRefCount)
         free(mString);
   }
public:
   StringPtr()
   {
      mString = NULL;
   }
   StringPtr(const char *string)
   {
      alloc(string);
   }
   StringPtr(const StringPtr &string)
   {
      mString = string.mString;
      if(mString)
         mString->mRefCount++;
   }
   ~StringPtr()
   {
      decRef();
   }
   StringPtr &operator=(const StringPtr &ref)
   {
      decRef();
      mString = ref.mString;
      mString->mRefCount++;
      return *this;
   }
   StringPtr &operator=(const char *string)
   {
      decRef();
      alloc(string);
      return *this;
   }
   operator const char *() const
   {
      if(mString)
         return mString->mStringData;
      else
         return "";
   }
   const char *getString() const
   {
      if(mString)
         return mString->mStringData;
      else
         return "";
   }
};

};

#endif

// --- END OF FILE: tnl/tnlString.h ---
// --- START OF FILE: tnl/tnlSymmetricCipher.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_SYMMETRICCIPHER_H_
#define _TNL_SYMMETRICCIPHER_H_

#ifndef _TNL_NETBASE_H_
#include "tnlNetBase.h"
#endif

namespace TNL
{

class ByteBuffer;

/// Class for symmetric encryption of data across a connection.  Internally it uses
/// the libtomcrypt AES algorithm to encrypt the data.
class SymmetricCipher : public Object
{
public:
   enum {
      BlockSize = 16,
      KeySize = 16,
   };
private:
   struct Key
   {
      U32 eK[64], dK[64];
      int Nr;
   };
   U32 mCounter[BlockSize >> 2];
   U32 mInitVector[BlockSize];
   U8 mPad[BlockSize];
   Key mSymmetricKey;
   U32 mPadLen;
public:
   SymmetricCipher(const U8 symmetricKey[KeySize], const U8 initVector[BlockSize]);
   SymmetricCipher(const ByteBuffer *theByteBuffer);

   void setupCounter(U32 counterValue1, U32 counterValue2, U32 counterValue3, U32 counterValue4);
   void encrypt(const U8 *plainText, U8 *cipherText, U32 len);
   void decrypt(const U8 *cipherText, U8 *plainText, U32 len);
};

};

#endif

// --- END OF FILE: tnl/tnlSymmetricCipher.h ---
// --- START OF FILE: tnl/tnlThread.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNLTHREAD_H_
#define _TNLTHREAD_H_

#include "tnl.h"
#include "tnlNetBase.h"
#include "tnlMethodDispatch.h"

#if defined (TNL_OS_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

namespace TNL
{

/// Platform independent semaphore class.
///
/// The semaphore class wraps OS specific semaphore functionality for thread synchronization.
class Semaphore
{
#ifdef TNL_OS_WIN32
   HANDLE mSemaphore;
#else
   sem_t mSemaphore;
#endif
public:
   /// Semaphore constructor - initialCount specifies how many wait calls
   /// will be let through before an increment is required.
   Semaphore(U32 initialCount = 0, U32 maximumCount = 1024);
   ~Semaphore();

   /// Thread calling wait will block as long as the semaphore's count
   /// is zero.  If the semaphore is incremented, one of the waiting threads
   /// will be awakened and the semaphore will decrement.
   void wait();

   /// Increments the semaphore's internal count.  This will wake
   /// count threads that are waiting on this semaphore.
   void increment(U32 count = 1);
};

/// Platform independent Mutual Exclusion implementation
class Mutex
{
#ifdef TNL_OS_WIN32
   CRITICAL_SECTION mLock;
#else
   pthread_mutex_t mMutex;
#endif
public:
   /// Mutex constructor
   Mutex();
   /// Mutex destructor
   ~Mutex();

   /// Locks the Mutex.  If another thread already has this Mutex
   /// locked, this call will block until it is unlocked.  If the lock
   /// method is called from a thread that has already locked this Mutex,
   /// the call will not block and the thread will have to unlock
   /// the mutex for as many calls as were made to lock before another
   /// thread will be allowed to lock the Mutex.
   void lock();

   /// Unlocks the Mutex.  The behavior of this method is undefined if called
   /// by a thread that has not previously locked this Mutex.
   void unlock();

   /// Attempts to acquire a lock to this Mutex, without blocking.
   /// Returns true if the calling thread was able to lock the Mutex and
   /// false if the Mutex was already locked by another thread.
   bool tryLock();
};

/// Platform independent Thread class.
class Thread : public Object
{
protected:
   U32 mReturnValue; ///< Return value from thread function

#ifdef TNL_OS_WIN32
   HANDLE mThread;
#else
   pthread_t mThread;
#endif
public:
   /// run function called when thread is started.
   virtual U32 run() = 0;
   /// Thread constructor.
   Thread();
   /// Thread destructor.
   ~Thread();

   /// starts the thread's main run function.
   void start();
};

/// Platform independent per-thread storage class.
class ThreadStorage
{
#ifdef TNL_OS_WIN32
   DWORD mTlsIndex;
#else
   pthread_key_t mThreadKey;
#endif
public:
   /// ThreadStorage constructor.
   ThreadStorage();
   /// ThreadStorage destructor.
   ~ThreadStorage();

   /// returns the per-thread stored void pointer for this ThreadStorage.  The default value is NULL.
   void *get();
   /// sets the per-thread stored void pointer for this ThreadStorage object.
   void set(void *data);
};

/// Managing object for a queue of worker threads that pass
/// messages back and forth to the main thread.  ThreadQueue
/// methods declared with the TNL_DECLARE_THREADQ_METHOD macro
/// are special -- if they are called from the main thread,
/// they will be executed on one of the worker threads and vice
/// versa.
class ThreadQueue : public Object
{
   class ThreadQueueThread : public Thread
   {
      ThreadQueue *mThreadQueue;
      public:
      ThreadQueueThread(ThreadQueue *);
      U32 run();
   };
   friend class ThreadQueueThread;
   /// list of worker threads on this ThreadQueue
   Vector<Thread *> mThreads;
   /// list of calls to be processed by the worker threads
   Vector<Functor *> mThreadCalls;
   /// list of calls to be processed by the main thread
   Vector<Functor *> mResponseCalls;
   /// Synchronization variable that manages worker threads
   Semaphore mSemaphore;
   /// Internal Mutex for synchronizing access to thread call vectors.
   Mutex mLock;
   /// Storage variable that tracks whether this is the main thread or a worker thread.
   ThreadStorage mStorage;
protected:
   /// Locks the ThreadQueue for access to member variables.
   void lock() { mLock.lock(); }
   /// Unlocks the ThreadQueue.
   void unlock() { mLock.unlock(); }
   /// Posts a marshalled call onto either the worker thread call list or the response call list.
   void postCall(Functor *theCall);
   /// Dispatches the next available worker thread call.  Called internally by the worker threads when they awaken from the semaphore.
   void dispatchNextCall();
   /// helper function to determine if the currently executing thread is a worker thread or the main thread.
   bool isMainThread() { return (bool) mStorage.get(); }
   ThreadStorage &getStorage() { return mStorage; }
   /// called by each worker thread when it starts for subclass initialization of worker threads.
   virtual void threadStart() { }
public:
   /// ThreadQueue constructor.  threadCount specifies the number of worker threads that will be created.
   ThreadQueue(U32 threadCount);
   ~ThreadQueue();

   /// Dispatches all ThreadQueue calls queued by worker threads.  This should
   /// be called periodically from a main loop.
   void dispatchResponseCalls();
};

/// Declares a ThreadQueue method on a subclass of ThreadQueue.
#define TNL_DECLARE_THREADQ_METHOD(func, args) \
   void func args; \
   void func##_body args

/// Declares the implementation of a ThreadQueue method.
#define TNL_IMPLEMENT_THREADQ_METHOD(className, func, args, argNames) \
   void className::func args { \
   FunctorDecl<void (className::*)args> *theCall = new FunctorDecl<void (className::*)args>(&className::func##_body); \
   theCall->set argNames; \
   postCall(theCall); \
   }\
   void className::func##_body args


};

#endif


// --- END OF FILE: tnl/tnlThread.h ---
// --- START OF FILE: tnl/tnlTypes.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_TYPES_H_
#define _TNL_TYPES_H_


//--------------------------------------
// Enable Asserts in all debug builds
#if defined(TNL_DEBUG)
#ifndef TNL_ENABLE_ASSERTS
#define TNL_ENABLE_ASSERTS
#endif
#endif
#include <stdlib.h>
//inline void* operator new(size_t size, void* ptr) { return ptr; }
#include <new>
namespace TNL {

#ifndef NULL
#  define NULL 0
#endif


//------------------------------------------------------------------------------
//-------------------------------------- Basic Types...

/// @defgroup BasicTypes Basic Compiler Independent Types
/// These types are defined so that we know exactly what we have, sign and bit wise.
///
/// The number represents number of bits, the letters represent <b>S</b>igned,
/// <b>U</b>nsigned, or <b>F</b>loating point (implicitly signed).
/// @{

typedef signed char         S8;      ///< Compiler independent signed char (8bit integer).
typedef unsigned char       U8;      ///< Compiler independent unsigned char (8bit integer).

typedef signed short        S16;     ///< Compiler independent signed 16-bit short integer.
typedef unsigned short      U16;     ///< Compiler independent unsigned 16-bit short integer.

typedef signed int          S32;     ///< Compiler independent signed 32-bit integer.
typedef unsigned int        U32;     ///< Compiler independent unsigned 32-bit integer.

typedef float               F32;     ///< Compiler independent 32-bit float.
typedef double              F64;     ///< Compiler independent 64-bit float.

/// @}

/// NetType serves as a base class for all bit-compressed versions of
/// the base types that can be transmitted using TNL's RPC mechanism.
/// In general, the type names are self-explanatory, providing simple
/// wrappers on the original base types.  The template argument for bit
/// counts or numeric ranges is necessary because TNL parses the actual
/// function prototype as a string in order to determine how many bits
/// to use for each RPC parameter.
///
/// Template parameters to the NetType templates can be either integer
/// constants or enumeration values.  If enumeration values are used,
/// the TNL_DECLARE_RPC_ENUM or TNL_DECLARE_RPC_MEM enum macros must
/// be used to register the enumerations with the RPC system.
struct NetType {

};

/// Unsigned integer bit-level RPC template wrapper.
///
/// When an Int<X> is in the parameter list for an RPC method, that parameter will
/// be transmitted using X bits.
template<U32 bitCount> struct Int : NetType
{
   U32 value;
   Int(U32 val=0) { value = val; }
   operator U32() const { return value; }
   U32 getPrecisionBits() { return bitCount; }
};

/// Signed integer bit-level RPC template wrapper.
///
/// When a SignedInt<X> is in the parameter list for an RPC method, that parameter will
/// be transmitted using X bits.
template<U32 bitCount> struct SignedInt : NetType
{
   S32 value;
   SignedInt(S32 val=0) { value = val; }
   operator S32() const { return value; }
   U32 getPrecisionBits() { return bitCount; }
};

/// Floating point 0...1 value bit-level RPC template wrapper.
///
/// When a Float<X> is in the parameter list for an RPC method, that parameter will
/// be transmitted using X bits.
template<U32 bitCount> struct Float : NetType
{
   F32 value;
   Float(F32 val=0) { value = val; }
   operator F32() const { return value; }
   U32 getPrecisionBits() { return bitCount; }
};

/// Floating point -1...1 value bit-level RPC template wrapper.
///
/// When a SignedFloat<X> is in the parameter list for an RPC method, that parameter will
/// be transmitted using X bits.
template<U32 bitCount> struct SignedFloat : NetType
{
   F32 value;
   SignedFloat(F32 val=0) { value = val; }
   operator F32() const { return value; }
   U32 getPrecisionBits() { return bitCount; }
};

/// Unsigned ranged integer bit-level RPC template wrapper.
///
/// The RangedU32 is used to specify a range of valid values for the parameter
/// in the parameter list for an RPC method.
template<U32 rangeStart, U32 rangeEnd> struct RangedU32 : NetType
{
   U32 value;
   RangedU32(U32 val=rangeStart) { value = val; }
   operator U32() const { return value; }
};

//------------------------------------------------------------------------------
//-------------------------------------- Type constants...

/// @defgroup BasicConstants Global Constants
///
/// Handy constants!
/// @{

#define __EQUAL_CONST_F F32(0.000001)                            ///< Constant float epsilon used for F32 comparisons

static const F32 FloatOne  = F32(1.0);                           ///< Constant float 1.0
static const F32 FloatHalf = F32(0.5);                           ///< Constant float 0.5
static const F32 FloatZero = F32(0.0);                           ///< Constant float 0.0

static const F32 FloatPi   = F32(3.14159265358979323846);            ///< Constant float PI
static const F32 Float2Pi  = F32(2.0 * 3.14159265358979323846);      ///< Constant float 2*PI
static const F32 FloatInversePi = F32(1.0 / 3.14159265358979323846); ///< Constant float 1 / PI
static const F32 FloatHalfPi = F32(0.5 * 3.14159265358979323846);    ///< Constant float 1/2 * PI
static const F32 Float2InversePi = F32(2.0 / 3.14159265358979323846);///< Constant float 2 / PI
static const F32 FloatInverse2Pi = F32(0.5 / 3.14159265358979323846);///< Constant float 2 / PI

static const F32 FloatSqrt2 = F32(1.41421356237309504880f);          ///< Constant float sqrt(2)
static const F32 FloatSqrtHalf = F32(0.7071067811865475244008443f);  ///< Constant float sqrt(0.5)

static const S8  S8_MIN  = S8(-128);                              ///< Constant Min Limit S8
static const S8  S8_MAX  = S8(127);                               ///< Constant Max Limit S8
static const U8  U8_MAX  = U8(255);                               ///< Constant Max Limit U8

static const S16 S16_MIN = S16(-32768);                           ///< Constant Min Limit S16
static const S16 S16_MAX = S16(32767);                            ///< Constant Max Limit S16
static const U16 U16_MAX = U16(65535);                            ///< Constant Max Limit U16

static const S32 S32_MIN = S32(-2147483647 - 1);                  ///< Constant Min Limit S32
static const S32 S32_MAX = S32(2147483647);                       ///< Constant Max Limit S32
static const U32 U32_MAX = U32(0xffffffff);                       ///< Constant Max Limit U32

static const F32 F32_MIN = F32(1.175494351e-38F);                 ///< Constant Min Limit F32
static const F32 F32_MAX = F32(3.402823466e+38F);                 ///< Constant Max Limit F32

//----------------------------------------------------------------------------------
// Identify the compiler and OS specific stuff we need:
//----------------------------------------------------------------------------------

#if defined (_MSC_VER)

typedef signed _int64   S64;
typedef unsigned _int64 U64;

#define TNL_COMPILER_VISUALC _MSC_VER

#if _MSC_VER < 1200
   // No support for old compilers
#  error "VC: Minimum Visual C++ 6.0 or newer required"
#else  //_MSC_VER >= 1200
#  define TNL_COMPILER_STRING "VisualC++"
#endif

#define for if(false) {} else for   ///< Hack to work around Microsoft VC's non-C++ compliance on variable scoping

// disable warning caused by memory layer
// see msdn.microsoft.com "Compiler Warning (level 1) C4291" for more details
#pragma warning(disable: 4291)
// disable performance warning of integer to bool conversions
#pragma warning(disable: 4800)

#elif defined(__MWERKS__) && defined(_WIN32)

typedef signed long long    S64;  ///< Compiler independent signed 64-bit integer
typedef unsigned long long  U64;  ///< Compiler independent unsigned 64-bit integer

#define TNL_COMPILER_STRING "Metrowerks CW Win32"

#elif defined(__GNUC__)

typedef signed long long    S64;  ///< Compiler independent signed 64-bit integer
typedef unsigned long long  U64;  ///< Compiler independent unsigned 64-bit integer

#if defined(__MINGW32__)
#  define TNL_COMPILER_STRING "GCC (MinGW)"
#  define TNL_COMPILER_MINGW
#elif defined(__CYGWIN__)
#  define TNL_COMPILER_STRING "GCC (Cygwin)"
#  define TNL_COMPILER_MINGW
#else
#  define TNL_COMPILER_STRING "GCC "
#endif

#else
#  error "TNL: Unknown Compiler"
#endif

//----------------------------------------------------------------------------------
// Identify the target Operating System
//----------------------------------------------------------------------------------

#if defined (_XBOX) || defined(__XBOX__)
#  define TNL_OS_STRING "XBox"
#  define TNL_OS_XBOX
#  define FN_CDECL __cdecl

#elif defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
#  define TNL_OS_STRING "Win32"
#  define TNL_OS_WIN32

#ifdef TNL_COMPILER_MINGW
#  define FN_CDECL
#else
#  define FN_CDECL __cdecl
#endif

#elif defined(linux)
#  define TNL_OS_STRING "Linux"
#  define TNL_OS_LINUX
#  define FN_CDECL

#elif defined(__OpenBSD__)
#  define TNL_OS_STRING "OpenBSD"
#  define TNL_OS_OPENBSD
#  define FN_CDECL

#elif defined(__FreeBSD__)
#  define TNL_OS_STRING "FreeBSD"
#  define TNL_OS_FREEBSD
#  define FN_CDECL

#elif defined(__APPLE__)
#  define TNL_OS_MAC
#  define TNL_OS_MAC_OSX
#  define FN_CDECL

#else
#  error "TNL: Unsupported Operating System"
#endif



//----------------------------------------------------------------------------------
// Identify the target CPU and assembly language options
//----------------------------------------------------------------------------------

#if defined(_M_IX86) || defined(i386)
#  define TNL_CPU_STRING "Intel x86"
#  define TNL_CPU_X86
#  define TNL_LITTLE_ENDIAN
#  define TNL_SUPPORTS_NASM

#  if defined (__GNUC__)
#    if __GNUC__ == 2
#      define TNL_GCC_2
#    elif __GNUC__ == 3
#      define TNL_GCC_3
#    else
#      error "TNL: Unsupported version of GCC (see tnlMethodDispatch.cpp)"
#    endif
#    define TNL_SUPPORTS_GCC_INLINE_X86_ASM
#  elif defined (__MWERKS__)
#    define TNL_SUPPORTS_MWERKS_INLINE_X86_ASM
#  else
#    define TNL_SUPPORTS_VC_INLINE_X86_ASM
#  endif

#elif defined(__ppc__)
#  define TNL_CPU_STRING "PowerPC"
#  define TNL_CPU_PPC
#  define TNL_BIG_ENDIAN
#  ifdef __GNUC__
#    define TNL_SUPPORTS_GCC_INLINE_PPC_ASM
#  endif
#else
#  error "TNL: Unsupported Target CPU"
#endif


/// @}

///@defgroup ObjTrickery Object Management Trickery
///
/// These functions are to construct and destruct objects in memory
/// without causing a free or malloc call to occur. This is so that
/// we don't have to worry about allocating, say, space for a hundred
/// NetAddresses with a single malloc call, calling delete on a single
/// NetAdress, and having it try to free memory out from under us.
///
/// @{

/// Constructs an object that already has memory allocated for it.
template <class T>
inline T* constructInPlace(T* p)
{
   return new(p) T;
}

/// Copy constructs an object that already has memory allocated for it.
template <class T>
inline T* constructInPlace(T* p, const T* copy)
{
   return new(p) T(*copy);
}

/// Destructs an object without freeing the memory associated with it.
template <class T>
inline void destructInPlace(T* p)
{
   p->~T();
}

/// @}

/// @name GeneralMath Math Helpers
///
/// Some general numeric utility functions.
///
/// @{

/// Determines if number is a power of two.
inline bool isPow2(const U32 number)
{
   return (number & (number - 1)) == 0;
}

/// Determines the binary logarithm of the input value rounded down to the nearest power of 2.
inline U32 getBinLog2(U32 value)
{
   F32 floatValue = F32(value);
   return (*((U32 *) &floatValue) >> 23) - 127;
}

/// Determines the binary logarithm of the next greater power of two of the input number.
inline U32 getNextBinLog2(U32 number)
{
   return getBinLog2(number) + (isPow2(number) ? 0 : 1);
}

/// Determines the next greater power of two from the value.  If the value is a power of two, it is returned.
inline U32 getNextPow2(U32 value)
{
   return isPow2(value) ? value : (1 << (getBinLog2(value) + 1));
}


/// @defgroup MinMaxFuncs Many version of min and max
///
/// We can't use template functions because MSVC6 chokes.
///
/// So we have these...
/// @{

#define DeclareTemplatizedMinMax(type) \
 inline type getMin(type a, type b) { return a > b ? b : a; } \
 inline type getMax(type a, type b) { return a > b ? a : b; }

DeclareTemplatizedMinMax(U32)
DeclareTemplatizedMinMax(S32)
DeclareTemplatizedMinMax(U16)
DeclareTemplatizedMinMax(S16)
DeclareTemplatizedMinMax(U8)
DeclareTemplatizedMinMax(S8)
DeclareTemplatizedMinMax(F32)
DeclareTemplatizedMinMax(F64)

/// @}

inline void writeU32ToBuffer(U32 value, U8 *buffer)
{
   buffer[0] = value >> 24;
   buffer[1] = value >> 16;
   buffer[2] = value >> 8;
   buffer[3] = value;
}

inline U32 readU32FromBuffer(const U8 *buf)
{
   return (U32(buf[0]) << 24) |
          (U32(buf[1]) << 16) |
          (U32(buf[2]) << 8 ) |
          U32(buf[3]);
}

inline void writeU16ToBuffer(U16 value, U8 *buffer)
{
   buffer[0] = value >> 8;
   buffer[1] = (U8) value;
}

inline U16 readU16FromBuffer(const U8 *buffer)
{
   return (U16(buffer[0]) << 8) |
          U16(buffer[1]);
}

inline U32 fourByteAlign(U32 value)
{
   return (value + 3) & ~3;
}

#define BIT(x) (1 << (x))                       ///< Returns value with bit x set (2^x)

};

#endif //_TNL_TYPES_H_

// --- END OF FILE: tnl/tnlTypes.h ---
// --- START OF FILE: tnl/tnlUDP.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_UDP_H_
#define _TNL_UDP_H_

#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

#ifndef _TNL_VECTOR_H_
#include "tnlVector.h"
#endif

// JMQ: hate...Xwindows...headers
#if defined(TNL_OS_LINUX) && defined(None)
#undef None
#endif


namespace TNL {
/// @}
//------------------------------------------------------------------------------

/// Enumeration for transport protocols for the TNL.
/// Currently only IP and IPX are supported - the IPv6 constant
/// is for future use.
enum TransportProtocol
{
   IPProtocol,    ///< The standard Internet routing protocol
   TCPProtocol,   ///< The standard Internet guaranteed delivery protocol
   IPXProtocol,   ///< Novell's IPX protocol
   IPv6Protocol,  ///< The next generation 128-bit address internet protocol (not currently supported by TNL)
};

struct IPAddress;

/// Representation of a network address.
struct Address
{
   /// One of: IPXAddress, IPAddress
   U16 transport;
   U16 port;         ///< <b>For IP:</b> sin_port <b>For IPX:</b> sa_socket
   U32 netNum[4];    ///< Address data, sized for future IPv6 usage

   /// Constants specify
   enum NamedAddress {
      None,
      Localhost,
      Broadcast,
      Any,
   };

   /// Constructs an address using a protocol, named address type and port
   Address(TransportProtocol type = IPProtocol, NamedAddress name = Any, U16 port = 0);

   /// Constructs an address from a string, of the form "transport:address:port"
   /// where transport is one of ip, ipx, or (in the future) ipv6
   Address(const char *string)
   {
      set(string);
   }

   /// Constructs an address from an IP address.
   Address(const IPAddress &theAddress)
   {
      set(theAddress);
   }

   /// Sets the address to the specified string, returning true if
   /// the string was a valid address.  Note that this call may block
   /// if the address portion of the string requires a DNS lookup.
   bool set(const char *string);

   /// Sets the address to the specified IPAddress.
   void set(const IPAddress &address);

   /// returns the formatted string corresponding to the address.
   const char *toString() const;

   /// returns true if theAddress is equal to this.
   bool operator==(const Address &theAddress) const;

   /// returns true if theAddress is not equal to this.
   bool operator!=(const Address &theAddress) const { return !operator==(theAddress); }

   /// returns true if the addresses are equal, not including the port.
   bool isEqualAddress(const Address &theAddress) const {
      return transport == theAddress.transport &&
             netNum[0] == theAddress.netNum[0] &&
             netNum[1] == theAddress.netNum[1] &&
             netNum[2] == theAddress.netNum[2] &&
             netNum[3] == theAddress.netNum[3]; }
           

   /// Returns a 32-bit hash of this network address.
   U32 hash() const { return netNum[0] ^ (U32(port) << 8) ^ (netNum[1] << 16) ^ (netNum[1] >> 16) ^ (netNum[2] << 5); }

   /// Returns a packed IPAddress of this address
   IPAddress toIPAddress() const;
};

/// Packed representaion of an IPAddress.
struct IPAddress
{
   U32 netNum; ///< Address of the host in IP address format.
   U16 port;   ///< Port field of the network address.
};

typedef const IPAddress &IPAddressRef;

/// Packet level network constants
enum Constants {
   MaxPacketDataSize = 1500,  ///< The maximum size of a data packet sent through the Net::sendto and Net::recvfrom functions
};

/// Error conditions that can be returned by sendto and recvfrom.
enum NetError {
   NoError,               ///< The operation succeeded without error.
   InvalidPacketProtocol, ///< The program attempted to send a packet to an address using a socket that didn't support the protocol of the address.
   WouldBlock,            ///< The operation would have blocked, for example if there was no data to read.
   UnknownError,          ///< There was some other, unknown error.
};

/// The Socket class encapsulates a platform's network socket.
class Socket
{
   S32 mPlatformSocket;    ///< The OS-level socket
   U32 mTransportProtocol; ///< The transport type this socket uses.
public:
   enum {
      DefaultBufferSize = 32768, ///< The default send and receive buffer sizes
   };

   /// Opens a socket on the specified address/port
   ///
   /// A connectPort of 0 will bind to any available port.
   /// Passing a valid address for ipBindInterface will attempt to bind this socket to a particular IP address on the local machine.
   Socket(const Address &bindAddress, U32 sendBufferSize = DefaultBufferSize, U32 recvBufferSize = DefaultBufferSize, bool acceptsBroadcast = true, bool nonblockingIO = true);

   /// Closes the socket.
   ~Socket();

   /// Returns true if the socket was created successfully.
   bool isValid();

   /// Sends a packet to the address through sourceSocket.
   NetError sendto(const Address &address, const U8 *buffer, S32 bufferSize);

   /// Read an incoming packet.
   ///
   /// @param   address         Address originating the packet.
   /// @param   buffer          Buffer in to which to read the packet.
   /// @param   bufferSize      Size of the buffer (this prevents buffer overflows!).
   /// @param   bytesRead       Specifies the number of bytes which were actually in the packet.
   NetError recvfrom(Address *address, U8 *buffer, S32 bufferSize, S32 *bytesRead);

   /// Returns the Address corresponding to this socket, as bound on the local machine.
   Address getBoundAddress();

   /// Returns the list of network addresses this host can be bound to.  Currently this only
   /// returns IP addresses, with the port field set to 0.
   static void getInterfaceAddresses(Vector<Address> *addressVector);

   NetError connect(const Address &theAddress);
   NetError recv(U8 *buffer, S32 bufferSize, S32 *bytesRead);
   NetError send(const U8 *buffer, S32 bufferSize);
};

};
#endif

// --- END OF FILE: tnl/tnlUDP.h ---
// --- START OF FILE: tnl/tnlVector.h ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#ifndef _TNL_VECTOR_H_
#define _TNL_VECTOR_H_

//Includes
#ifndef _TNL_TYPES_H_
#include "tnlTypes.h"
#endif

#ifndef _TNL_PLATFORM_H_
#include "tnlPlatform.h"
#endif

#define VectorBlockSize 16

namespace TNL {

//-----------------------------------------------------------------------------

/// VectorRep is an image of a Vector template object that is used
/// for marshalling and unmarshalling Vectors across RPCs.
struct VectorRep
{
   U32 elementCount;
   U32 arraySize;
   U8 *array;
};

// =============================================================================

/// A dynamic array template class.
///
/// The vector grows as you insert or append
/// elements.  Insertion is fastest at the end of the array.  Resizing
/// of the array can be avoided by pre-allocating space using the
/// reserve() method.
template<class T> class Vector
{
  protected:
   U32 mElementCount; ///< Number of elements currently in the Vector.
   U32 mArraySize;    ///< Number of elements allocated for the Vector.
   T*  mArray;        ///< Pointer to the Vector elements.

   void  checkSize(U32 newElementCount);///< checks the element count against the array size and resizes the array if necessary
   void  destroy(U32 start, U32 end);   ///< Destructs elements from <i>start</i> to <i>end-1</i>
   void  construct(U32 start, U32 end); ///< Constructs elements from <i>start</i> to <i>end-1</i>
   void  construct(U32 start, U32 end, const T* array);
  public:
   Vector(const U32 initialSize = 0);
   Vector(const Vector&);
   ~Vector();

   /// @name VectorSTL STL interface
   ///
   /// @{

   ///
   typedef T        value_type;
   typedef T&       reference;
   typedef const T& const_reference;

   typedef S32      difference_type;
   typedef U32      size_type;

   typedef difference_type (QSORT_CALLBACK *compare_func)(T *a, T *b);

   Vector<T>& operator=(const Vector<T>& p);

   S32 size() const;
   bool empty() const;

   T&       front();
   const T& front() const;
   T&       back();
   const T& back() const;

   void push_front(const T&);
   void push_back(const T&);
   void pop_front();
   void pop_back();

   T& operator[](U32);
   const T& operator[](U32) const;

   T& operator[](S32 i)              { return operator[](U32(i)); }
   const T& operator[](S32 i ) const { return operator[](U32(i)); }

   void reserve(U32);

   /// @}

   /// @name VectorExtended Extended Interface
   ///
   /// @{

   ///
   U32  memSize() const;
   T*   address() const;
   U32  setSize(U32);
   void insert(U32);
   void erase(U32);
   void erase_fast(U32);
   void clear();
   void compact();

   void sort(compare_func f);
   T& first();
   T& last();
   const T& first() const;
   const T& last() const;

   void set(void * addr, U32 sz);

   /// @}
};

template<class T> inline Vector<T>::~Vector()
{
   destroy(0, mElementCount);
   free(mArray);
}

template<class T> inline Vector<T>::Vector(const U32 initialSize)
{
   mArray        = 0;
   mElementCount = 0;
   mArraySize    = 0;
   if(initialSize)
      reserve(initialSize);
}

template<class T> inline Vector<T>::Vector(const Vector& p)
{
   mArray = 0;
   mArraySize = 0;
   mElementCount = 0;

   checkSize(p.mElementCount);
   mElementCount = p.mElementCount;
   construct(0, p.mElementCount, p.mArray);
}


template<class T> inline void  Vector<T>::destroy(U32 start, U32 end) // destroys from start to end-1
{
   while(start < end)
      destructInPlace(&mArray[start++]);
}

template<class T> inline void  Vector<T>::construct(U32 start, U32 end) // destroys from start to end-1
{
   while(start < end)
      constructInPlace(&mArray[start++]);
}

template<class T> inline void  Vector<T>::construct(U32 start, U32 end, const T* array) // destroys from start to end-1
{
   while(start < end)
   {
      constructInPlace(&mArray[start], &array[start]);
      start++;
   }
}

template<class T> inline T* Vector<T>::address() const
{
   return mArray;
}

template<class T> inline U32 Vector<T>::setSize(U32 size)
{
   checkSize(size);

   if(size > mElementCount)
   {
      construct(mElementCount, size);
      mElementCount = size;
   }
   else if(size < mElementCount)
   {
      destroy(size, mElementCount);
      mElementCount = size;
      if(!mElementCount)
      {
         free(mArray);
         mArray = NULL;
         mArraySize = 0;
      }
   }
   return mElementCount;
}

template<class T> inline void Vector<T>::insert(U32 index)
{
   checkSize(mElementCount + 1);
   constructInPlace(&mArray[mElementCount]);
   mElementCount++;

   for(U32 i = mElementCount - 1; i > index; i--)
      mArray[i] = mArray[i - 1];
   destructInPlace(&mArray[index]);
   constructInPlace(&mArray[index]);
}

template<class T> inline void Vector<T>::erase(U32 index)
{
   // Assert: index >= 0 && index < mElementCount
   for(U32 i = index; i < mElementCount - 1; i++)
      mArray[i] = mArray[i+1];
   destructInPlace(&mArray[mElementCount - 1]);
   mElementCount--;
}

template<class T> inline void Vector<T>::erase_fast(U32 index)
{
   // CAUTION: this operator does NOT maintain list order
   // Copy the last element into the deleted 'hole' and decrement the
   //   size of the vector.
   // Assert: index >= 0 && index < mElementCount

   if(index != mElementCount - 1)
      mArray[index] = mArray[mElementCount - 1];
   destructInPlace(&mArray[mElementCount - 1]);
   mElementCount--;
}

template<class T> inline T& Vector<T>::first()
{
   return mArray[0];
}

template<class T> inline const T& Vector<T>::first() const
{
   return mArray[0];
}

template<class T> inline T& Vector<T>::last()
{
   TNLAssert(mElementCount != 0, "Error, no last element of a zero sized array!");
   return mArray[mElementCount - 1];
}

template<class T> inline const T& Vector<T>::last() const
{
   return mArray[mElementCount - 1];
}

template<class T> inline void Vector<T>::clear()
{
   setSize(0);
}

//-----------------------------------------------------------------------------

template<class T> inline Vector<T>& Vector<T>::operator=(const Vector<T>& p)
{
   destroy(0, mElementCount);
   mElementCount = 0;
   checkSize(p.mElementCount);
   construct(0, p.mElementCount, p.mArray);
   mElementCount = p.mElementCount;
   return *this;
}

template<class T> inline S32 Vector<T>::size() const
{
   return (S32)mElementCount;
}

template<class T> inline bool Vector<T>::empty() const
{
   return (mElementCount == 0);
}

template<class T> inline T& Vector<T>::front()
{
   return *begin();
}

template<class T> inline const T& Vector<T>::front() const
{
   return *begin();
}

template<class T> inline T& Vector<T>::back()
{
   return *end();
}

template<class T> inline const T& Vector<T>::back() const
{
   return *end();
}

template<class T> inline void Vector<T>::push_front(const T &x)
{
   insert(0);
   mArray[0] = x;
}

template<class T> inline void Vector<T>::push_back(const T &x)
{
   checkSize(mElementCount + 1);
   mElementCount++;
   constructInPlace(mArray + mElementCount - 1, &x);
}

template<class T> inline void Vector<T>::pop_front()
{
   erase(U32(0));
}

template<class T> inline void Vector<T>::pop_back()
{
   mElementCount--;
   destructInPlace(mArray + mElementCount);
}

template<class T> inline T& Vector<T>::operator[](U32 index)
{
   return mArray[index];
}

template<class T> inline const T& Vector<T>::operator[](U32 index) const
{
   return mArray[index];
}

template<class T> inline void Vector<T>::reserve(U32 size)
{
   checkSize(size);
}

//template<class T> inline void Vector<T>::set(void * addr, U32 sz)
//{
//   setSize(sz);
//   if (addr)
//      memcpy(address(),addr,sz*sizeof(T));
//}

//-----------------------------------------------------------------------------

template<class T> inline void Vector<T>::checkSize(U32 newCount)
{
   if(newCount <= mArraySize)
      return;

   U32 blk = VectorBlockSize - (newCount % VectorBlockSize);
   newCount += blk;

   T *newArray = (T *) malloc(sizeof(T) * newCount);
   T *oldArray = mArray;

   mArray = newArray;
   construct(0, mElementCount, oldArray);
   mArray = oldArray;
   destroy(0, mElementCount);
   free(oldArray);
   mArray = newArray;
   mArraySize = newCount;
}

typedef int (QSORT_CALLBACK *qsort_compare_func)(const void *, const void *);

template<class T> inline void Vector<T>::sort(compare_func f)
{
   qsort(address(), size(), sizeof(T), (qsort_compare_func) f);
}

};

#endif //_TNL_TVECTOR_H_


// --- END OF FILE: tnl/tnlVector.h ---// --- Sources ---
// --- START OF FILE: tnl/assert.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlAssert.h"
#include <stdarg.h>
#include "tnlLog.h"

namespace TNL {

#ifdef TNL_ENABLE_ASSERTS

//-------------------------------------- STATIC Declaration
static bool processing = false;

//--------------------------------------
static bool displayMessageBox(const char *title, const char *message, bool retry)
{
   if (retry)
      return Platform::AlertRetry(title, message);

   Platform::AlertOK(title, message);
   return false;
}   

//--------------------------------------
void Assert::processAssert(const char *filename, U32 lineNumber, const char  *message)
{
   processing = true;

   char buffer[2048];
   dSprintf(buffer, sizeof(buffer), "Fatal: (%s: %ld)", filename, lineNumber);
#ifdef TNL_DEBUG
   // In debug versions, allow a retry even for ISVs...
   bool retry = displayMessageBox(buffer, message, true);
#else
   bool retry = displayMessageBox(buffer, message, ((assertType == Fatal) ? true : false) );
#endif
   if (!retry)
      Platform::forceQuit();

   processing = false;
}

bool Assert::processingAssert()
{
   return processing;
}

#endif

//--------------------------------------
const char* avar(const char *message, ...)
{
   static char buffer[4096];
   va_list args;
   va_start(args, message);
   dVsprintf(buffer, sizeof(buffer), message, args);
   return( buffer );
}

};

// --- END OF FILE: tnl/assert.cpp ---
// --- START OF FILE: tnl/asymmetricKey.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlAsymmetricKey.h"
#include "tnlRandom.h"
#include <mycrypt.h>
#include "tnlLog.h"

#define crypto_key            ecc_key
#define crypto_make_key       ecc_make_key
#define crypto_free           ecc_free
#define crypto_import         ecc_import
#define crypto_export         ecc_export
#define crypto_shared_secret  ecc_shared_secret

namespace TNL
{

enum {
   StaticCryptoBufferSize = 2048,
};

static U8 staticCryptoBuffer[StaticCryptoBufferSize];

AsymmetricKey::AsymmetricKey(U32 keySize)
{
   mIsValid = false;

   int descriptorIndex = register_prng ( &yarrow_desc );
   crypto_key *theKey = (crypto_key *) malloc(sizeof(crypto_key));

   if( crypto_make_key((prng_state *) Random::getState(), descriptorIndex,
      keySize, theKey) != CRYPT_OK )
      return;

   mKeyData = theKey;
   mKeySize = keySize;

   unsigned long bufferLen = sizeof(staticCryptoBuffer) - sizeof(U32) - 1;

   staticCryptoBuffer[0] = KeyTypePrivate;
   writeU32ToBuffer(mKeySize, staticCryptoBuffer + 1);

   crypto_export(staticCryptoBuffer + sizeof(U32) + 1, &bufferLen, PK_PRIVATE, theKey);
   bufferLen += sizeof(U32) + 1;

   mPrivateKey = new ByteBuffer(staticCryptoBuffer, bufferLen);
   mPrivateKey->takeOwnership();

   bufferLen = sizeof(staticCryptoBuffer) - sizeof(U32) - 1;

   staticCryptoBuffer[0] = KeyTypePublic;
   writeU32ToBuffer(mKeySize, staticCryptoBuffer + 1);

   crypto_export(staticCryptoBuffer + sizeof(U32) + 1, &bufferLen, PK_PUBLIC, theKey);
   bufferLen += sizeof(U32) + 1;

   mPublicKey = new ByteBuffer(staticCryptoBuffer, bufferLen);
   mPublicKey->takeOwnership();

   mHasPrivateKey = true;
   mIsValid = true;
}

AsymmetricKey::~AsymmetricKey()
{
   if(mKeyData)
   {
      crypto_free((crypto_key *) mKeyData);
      free(mKeyData);
   }
}

void AsymmetricKey::load(const ByteBuffer &theBuffer)
{
   mIsValid = false;

   crypto_key *theKey = (crypto_key *) malloc(sizeof(crypto_key));
   const U8 *bufferPtr = theBuffer.getBuffer();

   mHasPrivateKey = bufferPtr[0] == KeyTypePrivate;

   U32 bufferSize = theBuffer.getBufferSize();
   if(bufferSize < sizeof(U32) + 1)
      return;

   mKeySize = readU32FromBuffer(bufferPtr + 1);

   if( crypto_import(bufferPtr + sizeof(U32) + 1, bufferSize - sizeof(U32) - 1, theKey)
         != CRYPT_OK)
      return;

   mKeyData = theKey;

   if(mHasPrivateKey)
   {
      unsigned long bufferLen = sizeof(staticCryptoBuffer) - sizeof(U32) - 1;
      staticCryptoBuffer[0] = KeyTypePublic;

      writeU32ToBuffer(mKeySize, staticCryptoBuffer);

      if( crypto_export(staticCryptoBuffer + sizeof(U32) + 1, &bufferLen, PK_PUBLIC, theKey)
            != CRYPT_OK )
         return;

      bufferLen += sizeof(U32) + 1;

      mPublicKey = new ByteBuffer(staticCryptoBuffer, bufferLen);
      mPublicKey->takeOwnership();
      mPrivateKey = new ByteBuffer(theBuffer);
      mPrivateKey->takeOwnership();
   }
   else
   {
      mPublicKey = new ByteBuffer(theBuffer);
      mPublicKey->takeOwnership();
   }
   mIsValid = true;
}

ByteBufferPtr AsymmetricKey::computeSharedSecretKey(AsymmetricKey *publicKey)
{
   if(publicKey->getKeySize() != getKeySize() || !mHasPrivateKey)
      return NULL;

   U8 hash[32];
   unsigned long outLen = sizeof(staticCryptoBuffer);

   TIME_BLOCK(secretSubKeyGen,
   crypto_shared_secret((crypto_key *) mKeyData, (crypto_key *) publicKey->mKeyData,
      staticCryptoBuffer, &outLen);
   )
   hash_state hashState;
   sha256_init(&hashState);
   sha256_process(&hashState, staticCryptoBuffer, outLen);
   sha256_done(&hashState, hash);
   ByteBuffer *ret = new ByteBuffer(hash, 32);
   ret->takeOwnership();
   return ret;
}

ByteBufferPtr AsymmetricKey::hashAndSign(const ByteBuffer &theByteBuffer)
{
   int descriptorIndex = register_prng ( &yarrow_desc );

   U8 hash[32];
   hash_state hashState;

   sha256_init(&hashState);
   sha256_process(&hashState, theByteBuffer.getBuffer(), theByteBuffer.getBufferSize());
   sha256_done(&hashState, hash);

   unsigned long outlen = sizeof(staticCryptoBuffer);

   ecc_sign_hash(hash, 32,
      staticCryptoBuffer, &outlen,
      (prng_state *) Random::getState(), descriptorIndex, (crypto_key *) mKeyData);

   return new ByteBuffer(staticCryptoBuffer, (U32) outlen);
}

bool AsymmetricKey::verifySignature(const ByteBuffer &theByteBuffer, const ByteBuffer &theSignature)
{
   U8 hash[32];
   hash_state hashState;

   sha256_init(&hashState);
   sha256_process(&hashState, theByteBuffer.getBuffer(), theByteBuffer.getBufferSize());
   sha256_done(&hashState, hash);

   int stat;

   ecc_verify_hash(theSignature.getBuffer(), theSignature.getBufferSize(), hash, 32, &stat, (crypto_key *) mKeyData);
   return stat != 0;
}

};

// --- END OF FILE: tnl/asymmetricKey.cpp ---
// --- START OF FILE: tnl/bitStream.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlBitStream.h"
#include "tnlVector.h"
#include "tnlNetBase.h"
#include "tnlHuffmanStringProcessor.h"
#include "tnlSymmetricCipher.h"
#include <mycrypt.h>

#include <math.h>

namespace TNL {

void BitStream::setMaxSizes(U32 maxReadSize, U32 maxWriteSize)
{
   maxReadBitNum = maxReadSize << 3;
   maxWriteBitNum = maxWriteSize << 3;
}

void BitStream::setMaxBitSizes(U32 maxReadSize, U32 maxWriteSize)
{
   maxReadBitNum = maxReadSize;
   maxWriteBitNum = maxWriteSize;
}

void BitStream::reset()
{
   bitNum = 0;
   error = false;
   mCompressRelative = false;
   mStringBuffer[0] = 0;
   mStringTable = NULL;
}

U8 *BitStream::getBytePtr()
{
   return getBuffer() + getBytePosition();
}

void BitStream::writeClassId(U32 classId, U32 classType, U32 classGroup)
{
	TNLAssert(classType < NetClassTypeCount, "Out of range class type.");
	TNLAssert(classId < NetClassRep::getNetClassCount(classGroup, classType), "Out of range class id.");
	writeInt(classId, NetClassRep::getNetClassBitSize(classGroup, classType));
}

U32 BitStream::readClassId(U32 classType, U32 classGroup)
{
   TNLAssert(classType < NetClassTypeCount, "Out of range class type.");
   U32 ret = readInt(NetClassRep::getNetClassBitSize(classGroup, classType));
   if(ret >= NetClassRep::getNetClassCount(classGroup, classType))
      return 0xFFFFFFFF;
   return ret;
}

bool BitStream::resizeBits(U32 newBits)
{
   U32 newSize = ((maxWriteBitNum + newBits + 7) >> 3) + ResizePad;
   if(!resize(newSize))
   {
      error = true;
      return false;
   }
   maxReadBitNum = newSize << 3;
   maxWriteBitNum = newSize << 3;
   return true;
}

bool BitStream::writeBits(U32 bitCount, const void *bitPtr)
{
   if(!bitCount)
      return true;

   if(bitCount + bitNum > maxWriteBitNum)
      if(!resizeBits(bitCount + bitNum - maxWriteBitNum))
         return false;

   U32 upShift  = bitNum & 0x7;
   U32 downShift= 8 - upShift;

   const U8 *sourcePtr = (U8 *) bitPtr;
   U8 *destPtr = getBuffer() + (bitNum >> 3);

   // if this write is for <= 1 byte, and it will all fit in the
   // first dest byte, then do some special masking.
   if(downShift >= bitCount)
   {
      U8 mask = ((1 << bitCount) - 1) << upShift;
      *destPtr = (*destPtr & ~mask) | ((*sourcePtr << upShift) & mask);
      bitNum += bitCount;
      return true;
   }

   // check for byte aligned writes -- this will be
   // much faster than the shifting writes.
   if(!upShift) 
   {
      bitNum += bitCount;
      for(; bitCount >= 8; bitCount -= 8)
         *destPtr++ = *sourcePtr++;
      if(bitCount)
      {
         U8 mask = (1 << bitCount) - 1;
         *destPtr = (*sourcePtr & mask) | (*destPtr & ~mask);
      }
      return true;
   }

   // the write destination is not byte aligned.
   U8 sourceByte;
   U8 destByte = *destPtr & (0xFF >> downShift);
   U8 lastMask  = 0xFF >> (7 - ((bitNum + bitCount - 1) & 0x7));

   bitNum += bitCount;

   for(;bitCount >= 8; bitCount -= 8)
   {
      sourceByte = *sourcePtr++;
      *destPtr++ = destByte | (sourceByte << upShift);
      destByte = sourceByte >> downShift;
   }
   if(bitCount == 0)
   {
      *destPtr = (*destPtr & ~lastMask) | (destByte & lastMask);
      return true;
   }
   if(bitCount <= downShift)
   {
      *destPtr = (*destPtr & ~lastMask) | ((destByte | (*sourcePtr << upShift)) & lastMask);
      return true;
   }
   sourceByte = *sourcePtr;

   *destPtr++ = destByte | (sourceByte << upShift);
   *destPtr = (*destPtr & ~lastMask) | ((sourceByte >> downShift) & lastMask);
   return true;
}

bool BitStream::readBits(U32 bitCount, void *bitPtr)
{
   if(!bitCount)
      return true;
   if(bitCount + bitNum > maxReadBitNum)
   {
      error = true;
      return false;
   }

   U8 *sourcePtr = getBuffer() + (bitNum >> 3);
   U32 byteCount = (bitCount + 7) >> 3;

   U8 *destPtr = (U8 *) bitPtr;

   U32 downShift = bitNum & 0x7;
   U32 upShift = 8 - downShift;

   if(!downShift)
   {
      while(byteCount--)
         *destPtr++ = *sourcePtr++;
      bitNum += bitCount;
      return true;
   }

   U8 sourceByte = *sourcePtr >> downShift;
   bitNum += bitCount;

   for(; bitCount >= 8; bitCount -= 8)
   {
      U8 nextByte = *++sourcePtr;
      *destPtr++ = sourceByte | (nextByte << upShift);
      sourceByte = nextByte >> downShift;
   }
   if(bitCount)
   {
      if(bitCount <= upShift)
      {
         *destPtr = sourceByte;
         return true;
      }
      *destPtr = sourceByte | ( (*++sourcePtr) << upShift);
   }
   return true;
}

bool BitStream::setBit(U32 bitCount, bool set)
{
   if(bitCount >= maxWriteBitNum)
      if(!resizeBits(bitCount - maxWriteBitNum + 1))
         return false;
   if(set)
      *(getBuffer() + (bitCount >> 3)) |= (1 << (bitCount & 0x7));
   else
      *(getBuffer() + (bitCount >> 3)) &= ~(1 << (bitCount & 0x7));
   return true;
}

bool BitStream::testBit(U32 bitCount)
{
   return (*(getBuffer() + (bitCount >> 3)) & (1 << (bitCount & 0x7))) != 0;
}

bool BitStream::writeFlag(bool val)
{
   if(bitNum + 1 > maxWriteBitNum)
      if(!resizeBits(1))
         return false;
   if(val)
      *(getBuffer() + (bitNum >> 3)) |= (1 << (bitNum & 0x7));
   else
      *(getBuffer() + (bitNum >> 3)) &= ~(1 << (bitNum & 0x7));
   bitNum++;
   return (val);
}

bool BitStream::write(const ByteBuffer *theBuffer)
{
   U32 size = theBuffer->getBufferSize();
   if(size > 1023)
      return false;

   writeInt(size, 10);
   return write(size, theBuffer->getBuffer());
}

bool BitStream::read(ByteBuffer *theBuffer)
{
   U32 size = readInt(10);
   theBuffer->takeOwnership();
   theBuffer->resize(size);
   return read(size, theBuffer->getBuffer());
}

U32 BitStream::readInt(U8 bitCount)
{
   U32 ret = 0;
   readBits(bitCount, &ret);
   ret = convertLEndianToHost(ret);

   // Clear bits that we didn't read.
   if(bitCount == 32)
      return ret;
   else
      ret &= (1 << bitCount) - 1;

   return ret;
}

void BitStream::writeInt(U32 val, U8 bitCount)
{
   val = convertHostToLEndian(val);
   writeBits(bitCount, &val);
}

void BitStream::writeFloat(F32 f, U8 bitCount)
{
   writeInt(U32(f * ((1 << bitCount) - 1)), bitCount);
}

F32 BitStream::readFloat(U8 bitCount)
{
   return readInt(bitCount) / F32((1 << bitCount) - 1);
}

void BitStream::writeSignedFloat(F32 f, U8 bitCount)
{
   writeSignedInt(S32(f * ((1 << (bitCount - 1)) - 1)), bitCount);
}

F32 BitStream::readSignedFloat(U8 bitCount)
{
   return readSignedInt(bitCount) / F32((1 << (bitCount - 1)) - 1);
}

void BitStream::writeSignedInt(S32 value, U8 bitCount)
{
   if(writeFlag(value < 0))
      writeInt(-value, bitCount - 1);
   else
      writeInt(value, bitCount - 1);
}

S32 BitStream::readSignedInt(U8 bitCount)
{
   if(readFlag())
      return -(S32)readInt(bitCount - 1);
   else
      return (S32)readInt(bitCount - 1);
}

void BitStream::writeNormalVector(const Point3F& vec, U8 bitCount)
{
   F32 phi   = F32(atan2(vec.x, vec.y) * FloatInversePi );
   F32 theta = F32(atan2(vec.z, sqrt(vec.x*vec.x + vec.y*vec.y)) * Float2InversePi);

   writeSignedFloat(phi, bitCount+1);
   writeSignedFloat(theta, bitCount);
}

void BitStream::readNormalVector(Point3F *vec, U8 bitCount)
{
   F32 phi   = readSignedFloat(bitCount+1) * FloatPi;
   F32 theta = readSignedFloat(bitCount) * FloatHalfPi;

   vec->x = sin(phi)*cos(theta);
   vec->y = cos(phi)*cos(theta);
   vec->z = sin(theta);
}

Point3F BitStream::dumbDownNormal(const Point3F& vec, U8 bitCount)
{
   U8 buffer[128];
   BitStream temp(buffer, 128);

   temp.writeNormalVector(vec, bitCount);
   temp.setBitPosition(0);

   Point3F ret;
   temp.readNormalVector(&ret, bitCount);
   return ret;
}

void BitStream::writeNormalVector(const Point3F& vec, U8 angleBitCount, U8 zBitCount)
{
   // if this is a z up or down vector, just write out a couple of bits
   // if z is -1 or 1, x and y must both be 0

   // FIXME: this should test if the vec.z is within zBitCount precision of 1
   // BJG - fixed, but may be inefficient. Lookup table?
   if(writeFlag(fabs(vec.z) >= (1.0f-(1.0f/zBitCount))))
      writeFlag(vec.z < 0);
   else
   {
      // write out the z value and the angle that x and y make around the z axis
      writeSignedFloat( vec.z, zBitCount );
      writeSignedFloat( atan2(vec.x,vec.y) * FloatInverse2Pi, angleBitCount );
   }
}

void BitStream::readNormalVector(Point3F * vec, U8 angleBitCount, U8 zBitCount)
{
   if(readFlag())
   {
      vec->z = readFlag() ? -1.0f : 1.0f;
      vec->x = 0;
      vec->y = 0;
   }
   else
   {
      vec->z = readSignedFloat(zBitCount);
   
      F32 angle = Float2Pi * readSignedFloat(angleBitCount);

      F32 mult = (F32) sqrt(1.0f - vec->z * vec->z);
      vec->x = mult * cos(angle);
      vec->y = mult * sin(angle);
   }
}

//----------------------------------------------------------------------------

void BitStream::clearPointCompression()
{
   mCompressRelative = false;
}

void BitStream::setPointCompression(const Point3F& p)
{
   mCompressRelative = true;
   mCompressPoint = p;
}

static U32 gBitCounts[4] = {
   16, 18, 20, 32
};

void BitStream::writePointCompressed(const Point3F& p,F32 scale)
{
   // Same # of bits for all axis
   Point3F vec;
   F32 invScale = 1 / scale;
   U32 type;
   if(mCompressRelative)
   {
	  vec.x = p.x - mCompressPoint.x;
	  vec.y = p.y - mCompressPoint.y;
	  vec.z = p.z - mCompressPoint.z;

      F32 dist = (F32) sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z) * invScale;
      if(dist < (1 << 15))
         type = 0;
      else if(dist < (1 << 17))
         type = 1;
      else if(dist < (1 << 19))
         type = 2;
      else
         type = 3;
   }
   else
      type = 3;

   writeInt(type, 2);

   if (type != 3)
   {
      type = gBitCounts[type];
      writeSignedInt(S32(vec.x * invScale),type);
      writeSignedInt(S32(vec.y * invScale),type);
      writeSignedInt(S32(vec.z * invScale),type);
   }
   else
   {
      write(p.x);
      write(p.y);
      write(p.z);
   }
}

void BitStream::readPointCompressed(Point3F* p,F32 scale)
{
   // Same # of bits for all axis
   U32 type = readInt(2);

   if(type == 3)
   {
      read(&p->x);
      read(&p->y);
      read(&p->z);
   }
   else
   {
      type = gBitCounts[type];
      p->x = (F32) readSignedInt(type);
      p->y = (F32) readSignedInt(type);
      p->z = (F32) readSignedInt(type);

      p->x = mCompressPoint.x + p->x * scale;
      p->y = mCompressPoint.y + p->y * scale;
      p->z = mCompressPoint.z + p->z * scale;
   }
}

void BitStream::readString(char buf[256])
{
   if(readFlag())
   {
      S32 offset = readInt(8);
      HuffmanStringProcessor::readHuffBuffer(this, mStringBuffer + offset);
      strcpy(buf, mStringBuffer);
   }
   else
   {
      HuffmanStringProcessor::readHuffBuffer(this, buf);
      strcpy(mStringBuffer, buf);
   }
}

void BitStream::writeString(const char *string, U8 maxLen)
{
   if(!string)
      string = "";
   U8 j;
   for(j = 0; j < maxLen && mStringBuffer[j] == string[j] && string[j];j++)
      ;
   strncpy(mStringBuffer + j, string + j, maxLen - j);
   mStringBuffer[maxLen] = 0;

   if(writeFlag(j > 2))
   {
      writeInt(j, 8);
      HuffmanStringProcessor::writeHuffBuffer(this, string + j, maxLen - j);
   }
   else
      HuffmanStringProcessor::writeHuffBuffer(this, string, maxLen);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

//----------------------------------------------------------------------------

void BitStream::readStringTableEntry(StringTableEntry *ste)
{
   if(mStringTable)
      *ste = mStringTable->readStringTableEntry(this);
   else
   {
      char buf[256];
      readString(buf);
      ste->set(buf);
   }
}

void BitStream::writeStringTableEntry(const StringTableEntry &ste)
{
   if(mStringTable)
      mStringTable->writeStringTableEntry(this, ste);
   else
      writeString(ste.getString());
}
//------------------------------------------------------------------------------

void BitStream::hashAndEncrypt(U32 hashDigestSize, U32 encryptStartOffset, SymmetricCipher *theCipher)
{
   U32 digestStart = getBytePosition();
   setBytePosition(digestStart);
   hash_state hashState;

   U8 hash[32];

   // do a sha256 hash of the BitStream:
   sha256_init(&hashState);
   sha256_process(&hashState, getBuffer(), digestStart);
   sha256_done(&hashState, hash);

   // write the hash into the BitStream:
   write(hashDigestSize, hash);

   theCipher->encrypt(getBuffer() + encryptStartOffset,
                      getBuffer() + encryptStartOffset,
                      getBytePosition() - encryptStartOffset);   
}

bool BitStream::decryptAndCheckHash(U32 hashDigestSize, U32 decryptStartOffset, SymmetricCipher *theCipher)
{
   U32 bufferSize = getBufferSize();
   U8 *buffer = getBuffer();

   if(bufferSize < decryptStartOffset + hashDigestSize)
      return false;

   theCipher->decrypt(buffer + decryptStartOffset,
                      buffer + decryptStartOffset,
                      bufferSize - decryptStartOffset);

   hash_state hashState;
   U8 hash[32];

   sha256_init(&hashState);
   sha256_process(&hashState, buffer, bufferSize - hashDigestSize);
   sha256_done(&hashState, hash);

   bool ret = !memcmp(buffer + bufferSize - hashDigestSize, hash, hashDigestSize);
   if(ret)
      resize(bufferSize - hashDigestSize);
   return ret;
}

//------------------------------------------------------------------------------

NetError PacketStream::sendto(Socket &outgoingSocket, const Address &addr)
{
   return outgoingSocket.sendto(addr, buffer, getBytePosition());
}

NetError PacketStream::recvfrom(Socket &incomingSocket, Address *recvAddress)
{
   NetError error;
   S32 dataSize;
   error = incomingSocket.recvfrom(recvAddress, buffer, sizeof(buffer), &dataSize);
   setBuffer(buffer, dataSize);
   setMaxSizes(dataSize, 0);
   reset();
   return error;
}

};

// --- END OF FILE: tnl/bitStream.cpp ---
// --- START OF FILE: tnl/byteBuffer.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlByteBuffer.h"
#include <mycrypt.h>

namespace TNL {

RefPtr<ByteBuffer> ByteBuffer::encodeBase64() const
{
   unsigned long outLen = ((getBufferSize() / 3) + 1) * 4 + 4 + 1;
   ByteBuffer *ret = new ByteBuffer(outLen);
   base64_encode(getBuffer(), getBufferSize(), ret->getBuffer(), &outLen);
   ret->resize(outLen+1);
   ret->getBuffer()[outLen] = 0;
   return ret;
}

RefPtr<ByteBuffer> ByteBuffer::decodeBase64() const
{
   unsigned long outLen = getBufferSize();
   ByteBuffer *ret = new ByteBuffer(outLen);
   base64_decode(getBuffer(), getBufferSize(), ret->getBuffer(), &outLen);
   ret->resize(outLen);
   return ret;
}

RefPtr<ByteBuffer> ByteBuffer::encodeBase16() const
{
   U32 outLen = getBufferSize() * 2 + 1;
   ByteBuffer *ret = new ByteBuffer(outLen);
   const U8 *buffer = getBuffer();
   U8 *outBuffer = ret->getBuffer();

   S32 size = getBufferSize();
   for(S32 i = 0; i < size; i++)
   {
      U8 b = *buffer++;
      U32 nib1 = b >> 4;
      U32 nib2 = b & 0xF;
      if(nib1 > 9)
         *outBuffer++ = 'a' + nib1 - 10;
      else
         *outBuffer++ = '0' + nib1;
      if(nib2 > 9)
         *outBuffer++ = 'a' + nib2 - 10;
      else
         *outBuffer++ = '0' + nib2;
   }
   *outBuffer = 0;
   return ret;
}

RefPtr<ByteBuffer> ByteBuffer::decodeBase16() const
{
   U32 outLen = getBufferSize() >> 1;
   ByteBuffer *ret = new ByteBuffer(outLen);
   const U8 *src = getBuffer();
   U8 *dst = ret->getBuffer();
   for(U32 i = 0; i < outLen; i++)
   {
      U8 out = 0;
      U8 nib1 = *src++;
      U8 nib2 = *src++;
      if(nib1 >= '0' && nib1 <= '9')
         out = (nib1 - '0') << 4;
      else if(nib1 >= 'a' && nib1 <= 'f')
         out = (nib1 - 'a' + 10) << 4;
      else if(nib1 >= 'A' && nib1 <= 'A')
         out = (nib1 - 'A' + 10) << 4;
      if(nib2 >= '0' && nib2 <= '9')
         out |= nib2 - '0';
      else if(nib2 >= 'a' && nib2 <= 'f')
         out |= nib2 - 'a' + 10;
      else if(nib2 >= 'A' && nib2 <= 'A')
         out |= nib2 - 'A' + 10;
      *dst++ = out;
   }
   return ret;
}

RefPtr<ByteBuffer> ByteBuffer::computeMD5Hash(U32 len) const
{
   if(!len)
      len = getBufferSize();

   ByteBuffer *ret = new ByteBuffer(16);
   hash_state md;
   md5_init(&md);
   md5_process(&md, (unsigned char *) getBuffer(), len);
   md5_done(&md, ret->getBuffer());
   return ret;
}

U32 ByteBuffer::calculateCRC(U32 start, U32 end, U32 crcVal) const
{
   static U32 crcTable[256];
   static bool crcTableValid = false;

   if(!crcTableValid)
   {
      U32 val;

      for(S32 i = 0; i < 256; i++)
      {
         val = i;
         for(S32 j = 0; j < 8; j++)
         {
            if(val & 0x01)
               val = 0xedb88320 ^ (val >> 1);
            else
               val = val >> 1;
         }
         crcTable[i] = val;
      }
      crcTableValid = true;
   }
   
   if(start >= mBufSize)
      return 0;
   if(end > mBufSize)
      end = mBufSize;

   // now calculate the crc
   const U8 * buf = getBuffer();
   for(U32 i = start; i < end; i++)
      crcVal = crcTable[(crcVal ^ buf[i]) & 0xff] ^ (crcVal >> 8);
   return(crcVal);
}

};

// --- END OF FILE: tnl/byteBuffer.cpp ---
// --- START OF FILE: tnl/certificate.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlCertificate.h"

namespace TNL {

void Certificate::parse()
{
   BitStream aStream(getBuffer(), getBufferSize());

   mPayload = new ByteBuffer(0);
   aStream.read(mPayload);
   mPublicKey = new AsymmetricKey(&aStream);
   mSignature = new ByteBuffer(0);

   mSignatureByteSize = aStream.getBytePosition();

   // advance the bit stream to the next byte:
   aStream.setBytePosition(aStream.getBytePosition());

   aStream.read(mSignature);

   if(aStream.isValid() && getBufferSize() == aStream.getBytePosition() && mPublicKey->isValid())
      mIsValid = true;
}

bool Certificate::validate(RefPtr<AsymmetricKey> signatoryPublicKey)
{
   if(!mIsValid)
      return false;

   ByteBuffer signedBytes(getBuffer(), mSignatureByteSize);
   
   return signatoryPublicKey->verifySignature(signedBytes, *mSignature);
}

Certificate::Certificate(const ByteBuffer &payload, RefPtr<AsymmetricKey> publicKey, RefPtr<AsymmetricKey> theCAPrivateKey)
{
   mIsValid = false;
   mSignatureByteSize = 0;

   if(payload.getBufferSize() > MaxPayloadSize || !publicKey->isValid())
      return;

   ByteBufferPtr thePublicKey = publicKey->getPublicKey();
   PacketStream packet;

   packet.write(&payload);
   packet.write(thePublicKey);
   mSignatureByteSize = packet.getBytePosition();
   packet.setBytePosition(mSignatureByteSize);

   ByteBuffer theSignedBytes(packet.getBuffer(), packet.getBytePosition());

   mSignature = theCAPrivateKey->hashAndSign(theSignedBytes);
   packet.write(mSignature);

   setBuffer(packet.getBuffer(), packet.getBytePosition());
   takeOwnership();
}

};

// --- END OF FILE: tnl/certificate.cpp ---
// --- START OF FILE: tnl/clientPuzzle.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU
//   General Public License, alternative licensing options are available
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlClientPuzzle.h"
#include "tnlRandom.h"

#include <mycrypt.h>

namespace TNL {

void ClientPuzzleManager::NonceTable::reset()
{
   mChunker.freeBlocks();
   mHashTableSize = Random::readI(MinHashTableSize, MaxHashTableSize) * 2 + 1;
   mHashTable = (Entry **) mChunker.alloc(sizeof(Entry *) * mHashTableSize);
   for(U32 i = 0; i < mHashTableSize; i++)
      mHashTable[i] = NULL;
}

bool ClientPuzzleManager::NonceTable::checkAdd(Nonce &theNonce)
{
   U32 nonce1 = readU32FromBuffer(theNonce.data);
   U32 nonce2 = readU32FromBuffer(theNonce.data + 4);

   U64 fullNonce = (U64(nonce1) << 32) | nonce2;

   U32 hashIndex = U32(fullNonce % mHashTableSize);
   for(Entry *walk = mHashTable[hashIndex]; walk; walk = walk->mHashNext)
      if(walk->mNonce == theNonce)
         return false;
   Entry *newEntry = (Entry *) mChunker.alloc(sizeof(Entry));
   newEntry->mNonce = theNonce;
   newEntry->mHashNext = mHashTable[hashIndex];
   mHashTable[hashIndex] = newEntry;
   return true;
}

ClientPuzzleManager::ClientPuzzleManager()
{
   mCurrentDifficulty = InitialPuzzleDifficulty;
   mLastUpdateTime = 0;
   mLastTickTime = 0;
   Random::read(mCurrentNonce.data, Nonce::NonceSize);
   Random::read(mLastNonce.data, Nonce::NonceSize);

   mCurrentNonceTable = new NonceTable;
   mLastNonceTable = new NonceTable;
}

ClientPuzzleManager::~ClientPuzzleManager()
{
   delete mCurrentNonceTable;
   delete mLastNonceTable;
}

void ClientPuzzleManager::tick(U32 currentTime)
{
   if(!mLastTickTime)
      mLastTickTime = currentTime;

   // use delta of last tick time and current time to manage puzzle
   // difficulty.

   // not yet implemented.


   // see if it's time to refresh the current puzzle:
   U32 timeDelta = currentTime - mLastUpdateTime;
   if(timeDelta > PuzzleRefreshTime)
   {
      mLastUpdateTime = currentTime;
      mLastNonce = mCurrentNonce;
      NonceTable *tempTable = mLastNonceTable;
      mLastNonceTable = mCurrentNonceTable;
      mCurrentNonceTable = tempTable;

      mLastNonce = mCurrentNonce;
      mCurrentNonceTable->reset();
      Random::read(mCurrentNonce.data, Nonce::NonceSize);
   }
}

bool ClientPuzzleManager::checkOneSolution(U32 solution, Nonce &clientNonce, Nonce &serverNonce, U32 puzzleDifficulty, U32 clientIdentity)
{
   U8 buffer[8];
   writeU32ToBuffer(solution, buffer);
   writeU32ToBuffer(clientIdentity, buffer + 4);

   hash_state hashState;
   U8 hash[32];

   sha256_init(&hashState);
   sha256_process(&hashState, buffer, sizeof(buffer));
   sha256_process(&hashState, clientNonce.data, Nonce::NonceSize);
   sha256_process(&hashState, serverNonce.data, Nonce::NonceSize);
   sha256_done(&hashState, hash);

   U32 index = 0;
   while(puzzleDifficulty > 8)
   {
      if(hash[index])
         return false;
      index++;
      puzzleDifficulty -= 8;
   }
   U8 mask = 0xFF << (8 - puzzleDifficulty);
   return (mask & hash[index]) == 0;
}

ClientPuzzleManager::ErrorCode ClientPuzzleManager::checkSolution(U32 solution, Nonce &clientNonce, Nonce &serverNonce, U32 puzzleDifficulty, U32 clientIdentity)
{
   if(puzzleDifficulty != mCurrentDifficulty)
      return InvalidPuzzleDifficulty;
   NonceTable *theTable = NULL;
   if(serverNonce == mCurrentNonce)
      theTable = mCurrentNonceTable;
   else if(serverNonce == mLastNonce)
      theTable = mLastNonceTable;
   if(!theTable)
      return InvalidServerNonce;
   if(!checkOneSolution(solution, clientNonce, serverNonce, puzzleDifficulty, clientIdentity))
      return InvalidSolution;
   if(!theTable->checkAdd(clientNonce))
      return InvalidClientNonce;
   return Success;
}

bool ClientPuzzleManager::solvePuzzle(U32 *solution, Nonce &clientNonce, Nonce &serverNonce, U32 puzzleDifficulty, U32 clientIdentity)
{
   U32 startTime = Platform::getRealMilliseconds();
   U32 startValue = *solution;

   // Until we're done...
   for(;;)
   {
      U32 nextValue = startValue + SolutionFragmentIterations;
      for(;startValue < nextValue; startValue++)
      {
         if(checkOneSolution(startValue, clientNonce, serverNonce, puzzleDifficulty, clientIdentity))
         {
            *solution = startValue;
            return true;
         }
      }

      // Then we check to see if we're out of time...
      if(Platform::getRealMilliseconds() - startTime > MaxSolutionComputeFragment)
      {
         *solution = startValue;
         return false;
      }
   }
}

};

// --- END OF FILE: tnl/clientPuzzle.cpp ---
// --- START OF FILE: tnl/connectionStringTable.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"

#include "tnlEventConnection.h"
#include "tnlBitStream.h"

namespace TNL {

//--------------------------------------------------------------------
static ClassChunker<ConnectionStringTable::PacketEntry> packetEntryFreeList(4096);

ConnectionStringTable::ConnectionStringTable(NetConnection *parent)
{
   mParent = parent;
   for(U32 i = 0; i < EntryCount; i++)
   {
      mEntryTable[i].nextHash = NULL;
      mEntryTable[i].nextLink = &mEntryTable[i+1];
      mEntryTable[i].prevLink = &mEntryTable[i-1];
      mEntryTable[i].index = i;
      mHashTable[i] = NULL;
   }
   mLRUHead.nextLink = &mEntryTable[0];
   mEntryTable[0].prevLink = &mLRUHead;
   mLRUTail.prevLink = &mEntryTable[EntryCount-1];
   mEntryTable[EntryCount-1].nextLink = &mLRUTail;
}

void ConnectionStringTable::writeStringTableEntry(BitStream *stream, StringTableEntryRef string)
{
   // see if the entry is in the hash table right now
   U32 hashIndex = string.getIndex() % EntryCount;
   Entry *sendEntry = NULL;
   for(Entry *walk = mHashTable[hashIndex]; walk; walk = walk->nextHash)
   {
      if(walk->string == string)
      {
         // it's in the table
         // first, push it to the back of the LRU list.
         pushBack(walk);
         sendEntry = walk;
         break;
      }
   }
   if(!sendEntry)
   {
      // not in the hash table, means we have to add it
      // pull the new entry from the LRU list.
      sendEntry = mLRUHead.nextLink;

      // push it to the end of the LRU list
      pushBack(sendEntry);

      // remove the string from the hash table
      Entry **hashWalk;
      for (hashWalk = &mHashTable[sendEntry->string.getIndex() % EntryCount]; *hashWalk; hashWalk = &((*hashWalk)->nextHash))
      {
         if(*hashWalk == sendEntry)
         {
            *hashWalk = sendEntry->nextHash;
            break;
         }
      }
      
      sendEntry->string = string;
      sendEntry->receiveConfirmed = false;
      sendEntry->nextHash = mHashTable[hashIndex];
      mHashTable[hashIndex] = sendEntry;
   }
   stream->writeInt(sendEntry->index, EntryBitSize);
   if(!stream->writeFlag(sendEntry->receiveConfirmed))
   {
      stream->writeString(sendEntry->string.getString());
      PacketEntry *entry = packetEntryFreeList.alloc();

      entry->stringTableEntry = sendEntry;
      entry->string = sendEntry->string;
      entry->nextInPacket = NULL;

      PacketList *note = &mParent->getCurrentWritePacketNotify()->stringList;

      if(!note->stringHead)
         note->stringHead = entry;
      else
         note->stringTail->nextInPacket = entry;
      note->stringTail = entry;
   }
}

StringTableEntry ConnectionStringTable::readStringTableEntry(BitStream *stream)
{
   U32 index = stream->readInt(EntryBitSize);

   char buf[256];
   if(!stream->readFlag())
   {
      stream->readString(buf);
      mRemoteStringTable[index].set(buf);
   }
   return mRemoteStringTable[index];
}

void ConnectionStringTable::packetReceived(PacketList *note)
{
   PacketEntry *walk = note->stringHead;
   while(walk)
   {
      PacketEntry *next = walk->nextInPacket;
      if(walk->stringTableEntry->string == walk->string)
         walk->stringTableEntry->receiveConfirmed = true;
      packetEntryFreeList.free(walk);
      walk = next;
   }
}

void ConnectionStringTable::packetDropped(PacketList *note)
{
   PacketEntry *walk = note->stringHead;
   while(walk)
   {
      PacketEntry *next = walk->nextInPacket;
      packetEntryFreeList.free(walk);
      walk = next;
   }
}

};

// --- END OF FILE: tnl/connectionStringTable.cpp ---
// --- START OF FILE: tnl/dataChunker.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlDataChunker.h"


//----------------------------------------------------------------------------

namespace TNL {

DataChunker::DataChunker(S32 size)
{
   chunkSize          = size;
   curBlock           = new DataBlock(size);
   curBlock->next     = NULL;
   curBlock->curIndex = 0;
}

DataChunker::~DataChunker()
{
   freeBlocks();
}

void *DataChunker::alloc(S32 size)
{
   TNLAssert(size <= chunkSize, "Data chunk too large.");
   if(!curBlock || size + curBlock->curIndex > chunkSize)
   {
      DataBlock *temp = new DataBlock(chunkSize);
      temp->next = curBlock;
      temp->curIndex = 0;
      curBlock = temp;
   }
   void *ret = curBlock->data + curBlock->curIndex;
   curBlock->curIndex += (size + 3) & ~3; // dword align
   return ret;
}

DataChunker::DataBlock::DataBlock(S32 size)
{
   data = new U8[size];
}

DataChunker::DataBlock::~DataBlock()
{
   delete[] data;
}

void DataChunker::freeBlocks()
{
   while(curBlock)
   {
      DataBlock *temp = curBlock->next;
      delete curBlock;
      curBlock = temp;
   }
}

};

// --- END OF FILE: tnl/dataChunker.cpp ---
// --- START OF FILE: tnl/eventConnection.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlEventConnection.h"
#include "tnlBitStream.h"
#include "tnlLog.h"
#include "tnlNetInterface.h"

namespace TNL {

ClassChunker<EventConnection::EventNote> EventConnection::mEventNoteChunker;

EventConnection::EventConnection()
{
   // event management data:

   mNotifyEventList = NULL;
   mSendEventQueueHead = NULL;
   mSendEventQueueTail = NULL;
   mUnorderedSendEventQueueHead = NULL;
   mUnorderedSendEventQueueTail = NULL;
   mWaitSeqEvents = NULL;

   mNextSendEventSeq = FirstValidSendEventSeq;
   mNextRecvEventSeq = FirstValidSendEventSeq;
   mLastAckedEventSeq = -1;
   mEventClassCount = 0;
   mEventClassBitSize = 0;
}

EventConnection::~EventConnection()
{
   
   while(mNotifyEventList)
   {
      EventNote *temp = mNotifyEventList;
      mNotifyEventList = temp->mNextEvent;
      
      temp->mEvent->notifyDelivered(this, true);
      mEventNoteChunker.free(temp);
   }
   while(mUnorderedSendEventQueueHead)
   {
      EventNote *temp = mUnorderedSendEventQueueHead;
      mUnorderedSendEventQueueHead = temp->mNextEvent;
      
      temp->mEvent->notifyDelivered(this, true);
      mEventNoteChunker.free(temp);
   }
   while(mSendEventQueueHead)
   {
      EventNote *temp = mSendEventQueueHead;
      mSendEventQueueHead = temp->mNextEvent;
      
      temp->mEvent->notifyDelivered(this, true);
      mEventNoteChunker.free(temp);
   }
}

void EventConnection::writeConnectRequest(BitStream *stream)
{
   Parent::writeConnectRequest(stream);
   stream->write(NetClassRep::getNetClassCount(getNetClassGroup(), NetClassTypeEvent));
}

bool EventConnection::readConnectRequest(BitStream *stream, const char **errorString)
{
   if(!Parent::readConnectRequest(stream, errorString))
      return false;

   U32 classCount;
   stream->read(&classCount);

   U32 myCount = NetClassRep::getNetClassCount(getNetClassGroup(), NetClassTypeEvent);
   if(myCount <= classCount)
      mEventClassCount = myCount;
   else
   {
      mEventClassCount = classCount;
      if(!NetClassRep::isVersionBorderCount(getNetClassGroup(), NetClassTypeEvent, mEventClassCount))
         return false;
   }
   mEventClassVersion = NetClassRep::getClass(getNetClassGroup(), NetClassTypeEvent, mEventClassCount-1)->getClassVersion();
   mEventClassBitSize = getNextBinLog2(mEventClassCount);
   return true;
}

void EventConnection::writeConnectAccept(BitStream *stream)
{
   Parent::writeConnectAccept(stream);
   stream->write(mEventClassCount);
}

bool EventConnection::readConnectAccept(BitStream *stream, const char **errorString)
{
   if(!Parent::readConnectAccept(stream, errorString))
      return false;

   stream->read(&mEventClassCount);
   U32 myCount = NetClassRep::getNetClassCount(getNetClassGroup(), NetClassTypeEvent);

   if(mEventClassCount > myCount)
      return false;

   if(!NetClassRep::isVersionBorderCount(getNetClassGroup(), NetClassTypeEvent, mEventClassCount))
      return false;

   mEventClassBitSize = getNextBinLog2(mEventClassCount);
   return true;
}

void EventConnection::processEvent(NetEvent *theEvent)
{
   if(getConnectionState() == NetConnection::Connected)
      theEvent->process(this);
}

void EventConnection::packetDropped(PacketNotify *pnotify)
{
   Parent::packetDropped(pnotify);
   EventPacketNotify *notify = static_cast<EventPacketNotify *>(pnotify);

   EventNote *walk = notify->eventList;
   EventNote **insertList = &mSendEventQueueHead;
   EventNote *temp;
   
   while(walk)
   {
      switch(walk->mEvent->mGuaranteeType)
      {
         case NetEvent::GuaranteedOrdered:
            // It was a guaranteed ordered packet, reinsert it back into
            // mSendEventQueueHead in the right place (based on seq numbers)

            TNLLogMessageV(LogEventConnection, ("EventConnection %s: DroppedGuaranteed - %d", getNetAddressString(), walk->mSeqCount));
            while(*insertList && (*insertList)->mSeqCount < walk->mSeqCount)
               insertList = &((*insertList)->mNextEvent);
            
            temp = walk->mNextEvent;
            walk->mNextEvent = *insertList;
            if(!walk->mNextEvent)
               mSendEventQueueTail = walk;
            *insertList = walk;
            insertList = &(walk->mNextEvent);
            walk = temp;
            break;
         case NetEvent::Guaranteed:
            // It was a guaranteed packet, put it at the top of
            // mUnorderedSendEventQueueHead.
            temp = walk->mNextEvent;
            walk->mNextEvent = mUnorderedSendEventQueueHead;
            mUnorderedSendEventQueueHead = walk;
            if(!walk->mNextEvent)
               mUnorderedSendEventQueueTail = walk;
            walk = temp;
            break;
         case NetEvent::Unguaranteed:
            // Or else it was an unguaranteed packet, notify that
            // it was _not_ delivered and blast it.
            walk->mEvent->notifyDelivered(this, false);
            temp = walk->mNextEvent;
            mEventNoteChunker.free(walk);
            walk = temp;
      }
   }
}

void EventConnection::packetReceived(PacketNotify *pnotify)
{
   Parent::packetReceived(pnotify);

   EventPacketNotify *notify = static_cast<EventPacketNotify *>(pnotify);

   EventNote *walk = notify->eventList;
   EventNote **noteList = &mNotifyEventList;

   while(walk)
   {
      EventNote *next = walk->mNextEvent;
      if(walk->mEvent->mGuaranteeType != NetEvent::GuaranteedOrdered)
      {
         walk->mEvent->notifyDelivered(this, true);
         mEventNoteChunker.free(walk);
         walk = next;
      }
      else
      {
         while(*noteList && (*noteList)->mSeqCount < walk->mSeqCount)
            noteList = &((*noteList)->mNextEvent);
         
         walk->mNextEvent = *noteList;
         *noteList = walk;
         noteList = &walk->mNextEvent;
         walk = next;
      }
   }
   while(mNotifyEventList && mNotifyEventList->mSeqCount == mLastAckedEventSeq + 1)
   {
      mLastAckedEventSeq++;
      EventNote *next = mNotifyEventList->mNextEvent;
      TNLLogMessageV(LogEventConnection, ("EventConnection %s: NotifyDelivered - %d", getNetAddressString(), mNotifyEventList->mSeqCount));
      mNotifyEventList->mEvent->notifyDelivered(this, true);
      mEventNoteChunker.free(mNotifyEventList);
      mNotifyEventList = next;
   }
}

void EventConnection::writePacket(BitStream *bstream, PacketNotify *pnotify)
{
   Parent::writePacket(bstream, pnotify);
   EventPacketNotify *notify = static_cast<EventPacketNotify *>(pnotify);
   
   if(mConnectionParameters.mDebugObjectSizes)
      bstream->writeInt(DebugChecksum, 32);

   EventNote *packQueueHead = NULL, *packQueueTail = NULL;

   while(mUnorderedSendEventQueueHead)
   {
      if(bstream->isFull())
         break;
      // get the first event
      EventNote *ev = mUnorderedSendEventQueueHead;

      bstream->writeFlag(true);
      S32 start = bstream->getBitPosition();

      if(mConnectionParameters.mDebugObjectSizes)
         bstream->advanceBitPosition(BitStreamPosBitSize);
      
      S32 classId = ev->mEvent->getClassId(getNetClassGroup());
      bstream->writeInt(classId, mEventClassBitSize);

      ev->mEvent->pack(this, bstream);
      TNLLogMessageV(LogEventConnection, ("EventConnection %s: WroteEvent %s - %d bits", getNetAddressString(), ev->mEvent->getDebugName(), bstream->getBitPosition() - start));

      if(mConnectionParameters.mDebugObjectSizes)
         bstream->writeIntAt(bstream->getBitPosition(), BitStreamPosBitSize, start);

      if(bstream->getBitSpaceAvailable() < MinimumPaddingBits)
      {
         // rewind to before the event, and break out of the loop:
         bstream->setBitPosition(start - 1);
         bstream->clearError();
         break;
      }

      // dequeue the event and add this event onto the packet queue
      mUnorderedSendEventQueueHead = ev->mNextEvent;
      ev->mNextEvent = NULL;

      if(!packQueueHead)
         packQueueHead = ev;
      else
         packQueueTail->mNextEvent = ev;
      packQueueTail = ev;
   }
   
   bstream->writeFlag(false);   
   S32 prevSeq = -2;
   
   while(mSendEventQueueHead)
   {
      if(bstream->isFull())
         break;
      
      // if the event window is full, stop processing
      if(mSendEventQueueHead->mSeqCount > mLastAckedEventSeq + 126)
         break;

      // get the first event
      EventNote *ev = mSendEventQueueHead;
      S32 eventStart = bstream->getBitPosition();

      bstream->writeFlag(true);

      if(!bstream->writeFlag(ev->mSeqCount == prevSeq + 1))
         bstream->writeInt(ev->mSeqCount, 7);
      prevSeq = ev->mSeqCount;

      if(mConnectionParameters.mDebugObjectSizes)
         bstream->advanceBitPosition(BitStreamPosBitSize);

      S32 start = bstream->getBitPosition();

      S32 classId = ev->mEvent->getClassId(getNetClassGroup());
      bstream->writeInt(classId, mEventClassBitSize);
      ev->mEvent->pack(this, bstream);

      ev->mEvent->getClassRep()->addInitialUpdate(bstream->getBitPosition() - start);
      TNLLogMessageV(LogEventConnection, ("EventConnection %s: WroteEvent %s - %d bits", getNetAddressString(), ev->mEvent->getDebugName(), bstream->getBitPosition() - start));

      if(mConnectionParameters.mDebugObjectSizes)
         bstream->writeIntAt(bstream->getBitPosition(), BitStreamPosBitSize, start - BitStreamPosBitSize);

      if(bstream->getBitSpaceAvailable() < MinimumPaddingBits)
      {
         // rewind to before the event, and break out of the loop:
         bstream->setBitPosition(eventStart);
         bstream->clearError();
         break;
      }

      // dequeue the event:
      mSendEventQueueHead = ev->mNextEvent;      
      ev->mNextEvent = NULL;
      if(!packQueueHead)
         packQueueHead = ev;
      else
         packQueueTail->mNextEvent = ev;
      packQueueTail = ev;
   }
   for(EventNote *ev = packQueueHead; ev; ev = ev->mNextEvent)
      ev->mEvent->notifySent(this);
      
   notify->eventList = packQueueHead;
   bstream->writeFlag(0);
}

void EventConnection::readPacket(BitStream *bstream)
{
   Parent::readPacket(bstream);

   if(mConnectionParameters.mDebugObjectSizes)
   {
      U32 sum = bstream->readInt(32);
      TNLAssert(sum == DebugChecksum, "Invalid checksum.");
   }
   
   S32 prevSeq = -2;
   EventNote **waitInsert = &mWaitSeqEvents;
   bool unguaranteedPhase = true;
   
   while(true)
   {
      bool bit = bstream->readFlag();
      if(unguaranteedPhase && !bit)
      {
         unguaranteedPhase = false;
         bit = bstream->readFlag();
      }
      if(!unguaranteedPhase && !bit)
         break;
      
      S32 seq = -1;
      
      if(!unguaranteedPhase) // get the sequence
      {
         if(bstream->readFlag())
            seq = (prevSeq + 1) & 0x7f;
         else
            seq = bstream->readInt(7);
         prevSeq = seq;
      }

      U32 endingPosition;
      if(mConnectionParameters.mDebugObjectSizes)
         endingPosition = bstream->readInt(BitStreamPosBitSize);

      U32 classId = bstream->readInt(mEventClassBitSize);
      if(classId >= mEventClassCount)
      {
         setLastError("Invalid packet.");
         return;
      }
      NetEvent *evt = (NetEvent *) Object::create(getNetClassGroup(), NetClassTypeEvent, classId);
      if(!evt)
      {
         setLastError("Invalid packet.");
         return;
      }

      // check if the direction this event moves is a valid direction.
      if(   (evt->getEventDirection() == NetEvent::DirUnset)
         || (evt->getEventDirection() == NetEvent::DirServerToClient && isConnectionToClient())
         || (evt->getEventDirection() == NetEvent::DirClientToServer && isConnectionToServer()) )
      {
         setLastError("Invalid Packet.");
         return;
      }


      evt->unpack(this, bstream);
      if(mErrorBuffer[0])
         return;

      if(mConnectionParameters.mDebugObjectSizes)
      {
         TNLAssert(endingPosition == bstream->getBitPosition(),
            avar("unpack did not match pack for event of class %s.",
            evt->getClassName()) );
      }

      if(unguaranteedPhase)
      {
         processEvent(evt);
         delete evt;
         if(mErrorBuffer[0])
            return;
         continue;
      }
      seq |= (mNextRecvEventSeq & ~0x7F);
      if(seq < mNextRecvEventSeq)
         seq += 128;
      
      EventNote *note = mEventNoteChunker.alloc();
      note->mEvent = evt;
      note->mSeqCount = seq;
      TNLLogMessageV(LogEventConnection, ("EventConnection %s: RecvdGuaranteed %d", getNetAddressString(), seq));

      while(*waitInsert && (*waitInsert)->mSeqCount < seq)
         waitInsert = &((*waitInsert)->mNextEvent);
      
      note->mNextEvent = *waitInsert;
      *waitInsert = note;
      waitInsert = &(note->mNextEvent);
   }
   while(mWaitSeqEvents && mWaitSeqEvents->mSeqCount == mNextRecvEventSeq)
   {
      mNextRecvEventSeq++;
      EventNote *temp = mWaitSeqEvents;
      mWaitSeqEvents = temp->mNextEvent;
      
      TNLLogMessageV(LogEventConnection, ("EventConnection %s: ProcessGuaranteed %d", getNetAddressString(), temp->mSeqCount));
      processEvent(temp->mEvent);
      mEventNoteChunker.free(temp);
      if(mErrorBuffer[0])
         return;
   }
}

bool EventConnection::postNetEvent(NetEvent *theEvent)
{   
   S32 classId = theEvent->getClassId(getNetClassGroup());
   if(U32(classId) >= mEventClassCount && getConnectionState() == Connected)
      return false;

   theEvent->notifyPosted(this);

   EventNote *event = mEventNoteChunker.alloc();
   event->mEvent = theEvent;
   event->mNextEvent = NULL;

   if(event->mEvent->mGuaranteeType == NetEvent::GuaranteedOrdered)
   {
      event->mSeqCount = mNextSendEventSeq++;
      if(!mSendEventQueueHead)
         mSendEventQueueHead = event;
      else
         mSendEventQueueTail->mNextEvent = event;
      mSendEventQueueTail = event;
   }
   else
   {
      event->mSeqCount = InvalidSendEventSeq;
      if(!mUnorderedSendEventQueueHead)
         mUnorderedSendEventQueueHead = event;
      else
         mUnorderedSendEventQueueTail->mNextEvent = event;
      mUnorderedSendEventQueueTail = event;
   }
   return true;
}

bool EventConnection::isDataToTransmit()
{
   return mUnorderedSendEventQueueHead || mSendEventQueueHead || Parent::isDataToTransmit();
}

};

// --- END OF FILE: tnl/eventConnection.cpp ---
// --- START OF FILE: tnl/ghostConnection.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlGhostConnection.h"
#include "tnlBitStream.h"
#include "tnlNetBase.h"
#include "tnlNetObject.h"
#include "tnlNetInterface.h"

namespace TNL {

GhostConnection::GhostConnection()
{
   // ghost management data:
   mScopeObject = NULL;
   mGhostingSequence = 0;
   mGhosting = false;
   mScoping = false;
   mGhostArray = NULL;
   mGhostRefs = NULL;
   mGhostLookupTable = NULL;
   mLocalGhosts = NULL;
   mGhostZeroUpdateIndex = 0;
}

GhostConnection::~GhostConnection()
{
   clearAllPacketNotifies();

   // delete any ghosts that may exist for this connection, but aren't added
   if(mGhostArray)
      clearGhostInfo();
   deleteLocalGhosts();
   delete[] mLocalGhosts;
   delete[] mGhostLookupTable;
   delete[] mGhostRefs;
   delete[] mGhostArray;
}

void GhostConnection::setGhostTo(bool ghostTo)
{
   if(mLocalGhosts) // if ghosting to this is already enabled, silently return
      return;

   if(ghostTo)
   {
      mLocalGhosts = new NetObject *[MaxGhostCount];
      for(S32 i = 0; i < MaxGhostCount; i++)
         mLocalGhosts[i] = NULL;
   }
}

void GhostConnection::setGhostFrom(bool ghostFrom)
{
   if(mGhostArray)
      return;

   if(ghostFrom)
   {
      mGhostFreeIndex = mGhostZeroUpdateIndex = 0;
      mGhostArray = new GhostInfo *[MaxGhostCount];
      mGhostRefs = new GhostInfo[MaxGhostCount];
      S32 i;
      for(i = 0; i < MaxGhostCount; i++)
      {
         mGhostRefs[i].obj = NULL;
         mGhostRefs[i].index = i;
         mGhostRefs[i].updateMask = 0;
      }
      mGhostLookupTable = new GhostInfo *[GhostLookupTableSize];
      for(i = 0; i < GhostLookupTableSize; i++)
         mGhostLookupTable[i] = 0;
   }
}

void GhostConnection::packetDropped(PacketNotify *pnotify)
{
   Parent::packetDropped(pnotify);
   GhostPacketNotify *notify = static_cast<GhostPacketNotify *>(pnotify);

   GhostRef *packRef = notify->ghostList;
   // loop through all the packRefs in the packet
 
   while(packRef)
   {
      GhostRef *temp = packRef->nextRef;

      U32 updateFlags = packRef->mask;

      // figure out which flags need to be updated on the object
      for(GhostRef *walk = packRef->updateChain; walk && updateFlags; walk = walk->updateChain)
         updateFlags &= ~walk->mask;
     
      // for any flags we haven't updated since this (dropped) packet
      // or them into the mask so they'll get updated soon

      if(updateFlags)
      {
         if(!packRef->ghost->updateMask)
         {
            packRef->ghost->updateMask = updateFlags;
            ghostPushNonZero(packRef->ghost);
         }
         else
            packRef->ghost->updateMask |= updateFlags;
      }

      // make sure this packRef isn't the last one on the GhostInfo
      if(packRef->ghost->lastUpdateChain == packRef)
         packRef->ghost->lastUpdateChain = NULL;
      
      // if this packet was ghosting an object, set it
      // to re ghost at it's earliest convenience

      if(packRef->ghostInfoFlags & GhostInfo::Ghosting)
      {
         packRef->ghost->flags |= GhostInfo::NotYetGhosted;
         packRef->ghost->flags &= ~GhostInfo::Ghosting;
      }
      
      // otherwise, if it was being deleted,
      // set it to re-delete

      else if(packRef->ghostInfoFlags & GhostInfo::KillingGhost)
      {
         packRef->ghost->flags |= GhostInfo::KillGhost;
         packRef->ghost->flags &= ~GhostInfo::KillingGhost;
      }

      delete packRef;
      packRef = temp;
   }
}

void GhostConnection::packetReceived(PacketNotify *pnotify)
{
   Parent::packetReceived(pnotify);
   GhostPacketNotify *notify = static_cast<GhostPacketNotify *>(pnotify);

   GhostRef *packRef = notify->ghostList;

   // loop through all the notifies in this packet

   while(packRef)
   {
      // make sure this packRef isn't the last one on the GhostInfo
      if(packRef->ghost->lastUpdateChain == packRef)
         packRef->ghost->lastUpdateChain = NULL;

      GhostRef *temp = packRef->nextRef;      
      // if this object was ghosting , it is now ghosted

      if(packRef->ghostInfoFlags & GhostInfo::Ghosting)
      {
         packRef->ghost->flags &= ~GhostInfo::Ghosting;
         if(packRef->ghost->obj)
            packRef->ghost->obj->onGhostAvailable(this);
      }
      // otherwise, if it was dieing, free the ghost

      else if(packRef->ghostInfoFlags & GhostInfo::KillingGhost)
         freeGhostInfo(packRef->ghost);

      delete packRef;
      packRef = temp;
   }
}

static S32 QSORT_CALLBACK UQECompare(const void *a,const void *b)
{
   GhostInfo *ga = *((GhostInfo **) a);
   GhostInfo *gb = *((GhostInfo **) b);

   F32 ret = ga->priority - gb->priority;
   return (ret < 0) ? -1 : ((ret > 0) ? 1 : 0);
} 

void GhostConnection::prepareWritePacket()
{
   Parent::prepareWritePacket();

   if(!doesGhostFrom() && !mGhosting)
      return;
   // first step is to check all our polled ghosts:

   // 1. Scope query - find if any new objects have come into
   //    scope and if any have gone out.

   // Each packet we loop through all the objects with non-zero masks and
   // mark them as "out of scope" before the scope query runs.
   // if the object has a zero update mask, we wait to remove it until it requests
   // an update

   for(S32 i = 0; i < mGhostZeroUpdateIndex; i++)
   {
      // increment the updateSkip for everyone... it's all good
      GhostInfo *walk = mGhostArray[i];
      walk->updateSkipCount++;
      if(!(walk->flags & (GhostInfo::ScopeLocalAlways)))
         walk->flags &= ~GhostInfo::InScope;
   }

   if(mScopeObject)
      mScopeObject->performScopeQuery(this);
}

bool GhostConnection::isDataToTransmit()
{
   // once we've run the scope query - if there are no objects that need to be updated,
   // we return false
   return Parent::isDataToTransmit() || mGhostZeroUpdateIndex != 0;
}

void GhostConnection::writePacket(BitStream *bstream, PacketNotify *pnotify)
{
   Parent::writePacket(bstream, pnotify);
   GhostPacketNotify *notify = static_cast<GhostPacketNotify *>(pnotify);

   if(mConnectionParameters.mDebugObjectSizes)
      bstream->writeInt(DebugChecksum, 32);

   notify->ghostList = NULL;
   
   if(!doesGhostFrom())
      return;
   
   if(!bstream->writeFlag(mGhosting && mScopeObject.isValid()))
      return;
      
   // fill a packet (or two) with ghosting data

   // 2. call scoped objects' priority functions if the flag set is nonzero
   //    A removed ghost is assumed to have a high priority
   // 3. call updates based on sorted priority until the packet is
   //    full.  set flags to zero for all updated objects

   GhostInfo *walk;

   for(S32 i = mGhostZeroUpdateIndex - 1; i >= 0; i--)
   {
      if(!(mGhostArray[i]->flags & GhostInfo::InScope))
         detachObject(mGhostArray[i]);
   }

   U32 maxIndex = 0;
   for(S32 i = mGhostZeroUpdateIndex - 1; i >= 0; i--)
   {
      walk = mGhostArray[i];
      if(walk->index > maxIndex)
         maxIndex = walk->index;

      // clear out any kill objects that haven't been ghosted yet
      if((walk->flags & GhostInfo::KillGhost) && (walk->flags & GhostInfo::NotYetGhosted))
      {
         freeGhostInfo(walk);
         continue;
      }
      // don't do any ghost processing on objects that are being killed
      // or in the process of ghosting
      else if(!(walk->flags & (GhostInfo::KillingGhost | GhostInfo::Ghosting)))
      {
         if(walk->flags & GhostInfo::KillGhost)
            walk->priority = 10000;
         else
            walk->priority = walk->obj->getUpdatePriority(mScopeObject, walk->updateMask, walk->updateSkipCount);
      }
      else
         walk->priority = 0;
   }
   GhostRef *updateList = NULL;
   qsort(mGhostArray, mGhostZeroUpdateIndex, sizeof(GhostInfo *), UQECompare);
   // reset the array indices...
   for(S32 i = mGhostZeroUpdateIndex - 1; i >= 0; i--)
      mGhostArray[i]->arrayIndex = i;

   S32 sendSize = 1;
   while(maxIndex >>= 1)
      sendSize++;

   if(sendSize < 3)
      sendSize = 3;

   bstream->writeInt(sendSize - 3, 3); // 0-7 3 bit number

   U32 count = 0;
   // 
   for(S32 i = mGhostZeroUpdateIndex - 1; i >= 0 && !bstream->isFull(); i--)
   {
      GhostInfo *walk = mGhostArray[i];
		if(walk->flags & (GhostInfo::KillingGhost | GhostInfo::Ghosting))
		   continue;

      U32 updateStart = bstream->getBitPosition();
      U32 updateMask = walk->updateMask;
      U32 retMask;
		   
      bstream->writeFlag(true);
      bstream->writeInt(walk->index, sendSize);
      if(!bstream->writeFlag(walk->flags & GhostInfo::KillGhost))
      {
         // this is an update of some kind:
         if(mConnectionParameters.mDebugObjectSizes)
            bstream->advanceBitPosition(BitStreamPosBitSize);

         S32 startPos = bstream->getBitPosition();

         if(walk->flags & GhostInfo::NotYetGhosted)
         {
            S32 classId = walk->obj->getClassId(getNetClassGroup());
            bstream->writeClassId(classId, NetClassTypeObject, getNetClassGroup());
            NetObject::mIsInitialUpdate = true;
         }

         // update the object
         retMask = walk->obj->packUpdate(this, updateMask, bstream);

         if(NetObject::mIsInitialUpdate)
         {
            NetObject::mIsInitialUpdate = false;
            walk->obj->getClassRep()->addInitialUpdate(bstream->getBitPosition() - startPos);
         }
         else
            walk->obj->getClassRep()->addPartialUpdate(bstream->getBitPosition() - startPos);

         if(mConnectionParameters.mDebugObjectSizes)
            bstream->writeIntAt(bstream->getBitPosition(), BitStreamPosBitSize, startPos - BitStreamPosBitSize);

         TNLLogMessageV(LogGhostConnection, ("GhostConnection %s GHOST %d", walk->obj->getClassName(), bstream->getBitPosition() - 16 - startPos));

         TNLAssert((retMask & (~updateMask)) == 0, "Cannot set new bits in packUpdate return");
      }

      // check for packet overrun, and rewind this update if there
      // was one:
      if(bstream->getBitSpaceAvailable() < MinimumPaddingBits)
      {
         bstream->setBitPosition(updateStart);
         bstream->clearError();
         break;
      }

      // otherwise, create a record of this ghost update and
      // attach it to the packet.
      GhostRef *upd = new GhostRef;

      upd->nextRef = updateList;
      updateList = upd;

      if(walk->lastUpdateChain)
         walk->lastUpdateChain->updateChain = upd;
      walk->lastUpdateChain = upd;

      upd->ghost = walk;
      upd->ghostInfoFlags = 0;
      upd->updateChain = NULL;

      if(walk->flags & GhostInfo::KillGhost)
      {
         walk->flags &= ~GhostInfo::KillGhost;
         walk->flags |= GhostInfo::KillingGhost;
         walk->updateMask = 0;
         upd->mask = updateMask;
         ghostPushToZero(walk);
         upd->ghostInfoFlags = GhostInfo::KillingGhost;
      }
      else
      {
         if(walk->flags & GhostInfo::NotYetGhosted)
         {
            walk->flags &= ~GhostInfo::NotYetGhosted;
            walk->flags |= GhostInfo::Ghosting;
            upd->ghostInfoFlags = GhostInfo::Ghosting;
         }
         walk->updateMask = retMask;
         if(!retMask)
            ghostPushToZero(walk);
         upd->mask = updateMask & ~retMask;
         walk->updateSkipCount = 0;
         count++;
      }
   }
   // count # of ghosts have been updated,
   // mGhostZeroUpdateIndex # of ghosts remain to be updated.
   // no more objects...
   bstream->writeFlag(false);
   notify->ghostList = updateList;
}

void GhostConnection::readPacket(BitStream *bstream)
{
   Parent::readPacket(bstream);

   if(mConnectionParameters.mDebugObjectSizes)
   {
      U32 sum = bstream->readInt(32);
      TNLAssert(sum == DebugChecksum, "Invalid checksum.");
   }

   if(!doesGhostTo())
      return;
   if(!bstream->readFlag())
      return;

   S32 idSize;
   idSize = bstream->readInt( 3 );
   idSize += 3;

   // while there's an object waiting...

   while(bstream->readFlag())
   {
      U32 index;
      //S32 startPos = bstream->getCurPos();
      index = (U32) bstream->readInt(idSize);
      if(bstream->readFlag()) // is this ghost being deleted?
      {
         TNLAssert(mLocalGhosts[index] != NULL, "Error, NULL ghost encountered.");
		   if(mLocalGhosts[index])
         {
            mLocalGhosts[index]->onGhostRemove();
		      delete mLocalGhosts[index];
            mLocalGhosts[index] = NULL;
         }
      }
      else
      {
         U32 endPosition = 0;
         if(mConnectionParameters.mDebugObjectSizes)
            endPosition = bstream->readInt(BitStreamPosBitSize);

         if(!mLocalGhosts[index]) // it's a new ghost... cool
         {
            S32 classId = bstream->readClassId(NetClassTypeObject, getNetClassGroup());
            if(classId == -1)
            {
               setLastError("Invalid packet.");
               return;
            }

            NetObject *obj = (NetObject *) Object::create(getNetClassGroup(), NetClassTypeObject, classId);
            if(!obj)
            {
               setLastError("Invalid packet.");
               return;
            }
            obj->mOwningConnection = this;
            obj->mNetFlags = NetObject::IsGhost;

            // object gets initial update before adding to the manager

            obj->mNetIndex = index;
            mLocalGhosts[index] = obj;

            NetObject::mIsInitialUpdate = true;
            mLocalGhosts[index]->unpackUpdate(this, bstream);
            NetObject::mIsInitialUpdate = false;
            
            if(!obj->onGhostAdd(this))
            {
               if(!mErrorBuffer[0])
                  setLastError("Invalid packet.");
               return;
            }
            if(mRemoteConnection)
            {
               GhostConnection *gc = static_cast<GhostConnection *>(mRemoteConnection.getPointer());
               obj->mServerObject = gc->resolveGhostParent(index);
            }
         }
         else
         {
            mLocalGhosts[index]->unpackUpdate(this, bstream);
         }

         if(mConnectionParameters.mDebugObjectSizes)
         {
            TNLAssert(bstream->getBitPosition() == endPosition,
            avar("unpackUpdate did not match packUpdate for object of class %s. Expected %d bits, got %d bits.",
               mLocalGhosts[index]->getClassName(), endPosition, bstream->getBitPosition()) );
         }

         if(mErrorBuffer[0])
            return;
      }
   }
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

void GhostConnection::setScopeObject(NetObject *obj)
{
   if(((NetObject *) mScopeObject) == obj)
      return;
   mScopeObject = obj;
}

void GhostConnection::detachObject(GhostInfo *info)
{
   // mark it for ghost killin'
   info->flags |= GhostInfo::KillGhost;

   // if the mask is in the zero range, we've got to move it up...
   if(!info->updateMask)
   {
      info->updateMask = 0xFFFFFFFF;
      ghostPushNonZero(info);
   }
   if(info->obj)
   {
      if(info->prevObjectRef)
         info->prevObjectRef->nextObjectRef = info->nextObjectRef;
      else
         info->obj->mFirstObjectRef = info->nextObjectRef;
      if(info->nextObjectRef)
         info->nextObjectRef->prevObjectRef = info->prevObjectRef;
      // remove it from the lookup table
      
      U32 id = info->obj->getHashId();
      for(GhostInfo **walk = &mGhostLookupTable[id & GhostLookupTableMask]; *walk; walk = &((*walk)->nextLookupInfo))
      {
         GhostInfo *temp = *walk;
         if(temp == info)
         {
            *walk = temp->nextLookupInfo;
            break;
         }
      }
      info->prevObjectRef = info->nextObjectRef = NULL;
      info->obj = NULL;
   }
}

void GhostConnection::freeGhostInfo(GhostInfo *ghost)
{
   TNLAssert(ghost->arrayIndex < mGhostFreeIndex, "Ghost already freed.");
   if(ghost->arrayIndex < mGhostZeroUpdateIndex)
   {
      TNLAssert(ghost->updateMask != 0, "Invalid ghost mask.");
      ghost->updateMask = 0;
      ghostPushToZero(ghost);
   }
   ghostPushZeroToFree(ghost);
   TNLAssert(ghost->lastUpdateChain == NULL, "Ack!");
}

//-----------------------------------------------------------------------------

void GhostConnection::objectLocalScopeAlways(NetObject *obj)
{
   if(!doesGhostFrom())
      return;
   objectInScope(obj);
   for(GhostInfo *walk = mGhostLookupTable[obj->getHashId() & GhostLookupTableMask]; walk; walk = walk->nextLookupInfo)
   {
      if(walk->obj != obj)
         continue;
      walk->flags |= GhostInfo::ScopeLocalAlways;
      return;
   }
}

void GhostConnection::objectLocalClearAlways(NetObject *obj)
{
   if(!doesGhostFrom())
      return;
   for(GhostInfo *walk = mGhostLookupTable[obj->getHashId() & GhostLookupTableMask]; walk; walk = walk->nextLookupInfo)
   {
      if(walk->obj != obj)
         continue;
      walk->flags &= ~GhostInfo::ScopeLocalAlways;
      return;
   }
}

bool GhostConnection::validateGhostArray()
{
   TNLAssert(mGhostZeroUpdateIndex >= 0 && mGhostZeroUpdateIndex <= mGhostFreeIndex, "Invalid update index range.");
   TNLAssert(mGhostFreeIndex <= MaxGhostCount, "Invalid free index range.");
   S32 i;
   for(i = 0; i < mGhostZeroUpdateIndex; i ++)
   {
      TNLAssert(mGhostArray[i]->arrayIndex == i, "Invalid array index.");
      TNLAssert(mGhostArray[i]->updateMask != 0, "Invalid ghost mask.");
   }
   for(; i < mGhostFreeIndex; i ++)
   {
      TNLAssert(mGhostArray[i]->arrayIndex == i, "Invalid array index.");
      TNLAssert(mGhostArray[i]->updateMask == 0, "Invalid ghost mask.");
   }
   for(; i < MaxGhostCount; i++)
   {
      TNLAssert(mGhostArray[i]->arrayIndex == i, "Invalid array index.");
   }
   return true;
}

void GhostConnection::objectInScope(NetObject *obj)
{
   if (!mScoping || !doesGhostFrom())
      return;
	if (!obj->isGhostable() || (obj->isScopeLocal() && !isLocalConnection()))
		return;
   S32 index = obj->getHashId() & GhostLookupTableMask;
   
   // check if it's already in scope
   // the object may have been cleared out without the lookupTable being cleared
   // so validate that the object pointers are the same.

   for(GhostInfo *walk = mGhostLookupTable[index ]; walk; walk = walk->nextLookupInfo)
   {
      if(walk->obj != obj)
         continue;
      walk->flags |= GhostInfo::InScope;
      return;
   }

   if (mGhostFreeIndex == MaxGhostCount)
      return;

   GhostInfo *giptr = mGhostArray[mGhostFreeIndex];
   ghostPushFreeToZero(giptr);
   giptr->updateMask = 0xFFFFFFFF;
   ghostPushNonZero(giptr);

   giptr->flags = GhostInfo::NotYetGhosted | GhostInfo::InScope;
   
   giptr->obj = obj;
   giptr->lastUpdateChain = NULL;
   giptr->updateSkipCount = 0;

   giptr->connection = this;

   giptr->nextObjectRef = obj->mFirstObjectRef;
   if(obj->mFirstObjectRef)
      obj->mFirstObjectRef->prevObjectRef = giptr;
   giptr->prevObjectRef = NULL;
   obj->mFirstObjectRef = giptr;
   
   giptr->nextLookupInfo = mGhostLookupTable[index];
   mGhostLookupTable[index] = giptr;
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

//-----------------------------------------------------------------------------

void GhostConnection::activateGhosting()
{
   if(!doesGhostFrom())
      return;

   mGhostingSequence++;
   TNLLogMessageV(LogGhostConnection, ("Ghosting activated - %d", mGhostingSequence));
   
   TNLAssert((mGhostFreeIndex == 0) && (mGhostZeroUpdateIndex == 0), "Error: ghosts in the ghost list before activate.");
   
   // iterate through the ghost always objects and InScope them...
   // also post em all to the other side.

   S32 j;
   for(j = 0; j < MaxGhostCount; j++)
   {
      mGhostArray[j] = mGhostRefs + j;
      mGhostArray[j]->arrayIndex = j;
   }
   mScoping = true; // so that objectInScope will work

   rpcStartGhosting(mGhostingSequence);
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

TNL_IMPLEMENT_RPC(GhostConnection, rpcStartGhosting, 
                  (U32 sequence), (sequence),
      NetClassGroupGameMask, RPCGuaranteedOrdered, RPCDirAny, 0)
{
   TNLLogMessageV(LogGhostConnection, ("Got GhostingStarting %d", sequence));

   if(!doesGhostTo())
   {
      setLastError("Invalid packet.");
      return;
   }
   onStartGhosting();
   rpcReadyForNormalGhosts(sequence);
}

TNL_IMPLEMENT_RPC(GhostConnection, rpcReadyForNormalGhosts, 
                  (U32 sequence), (sequence),
      NetClassGroupGameMask, RPCGuaranteedOrdered, RPCDirAny, 0)
{
   TNLLogMessageV(LogGhostConnection, ("Got ready for normal ghosts %d %d", sequence, mGhostingSequence));
   if(!doesGhostFrom())
   {
      setLastError("Invalid packet.");
      return;
   }
   if(sequence != mGhostingSequence)
      return;
   mGhosting = true;
}

TNL_IMPLEMENT_RPC(GhostConnection, rpcEndGhosting, (), (),
      NetClassGroupGameMask, RPCGuaranteedOrdered, RPCDirAny, 0)
{
   if(!doesGhostTo())
   {
      setLastError("Invalid packet.");
      return;
   }
   deleteLocalGhosts();
   onEndGhosting();
}

void GhostConnection::deleteLocalGhosts()
{
   if(!mLocalGhosts)
      return;
   // just delete all the local ghosts,
   // and delete all the ghosts in the current save list
   for(S32 i = 0; i < MaxGhostCount; i++)
   {
      if(mLocalGhosts[i])
      {
         mLocalGhosts[i]->onGhostRemove();
         delete mLocalGhosts[i];
         mLocalGhosts[i] = NULL;
      }
   }
}

void GhostConnection::clearGhostInfo()
{
   // gotta clear out the ghosts...
   for(PacketNotify *walk = mNotifyQueueHead; walk; walk = walk->nextPacket)
   {
      GhostPacketNotify *note = static_cast<GhostPacketNotify *>(walk);
      GhostRef *delWalk = note->ghostList;
      note->ghostList = NULL;
      while(delWalk)
      {
         GhostRef *next = delWalk->nextRef;
         delete delWalk;
         delWalk = next;
      }
   }
   for(S32 i = 0; i < MaxGhostCount; i++)
   {
      if(mGhostRefs[i].arrayIndex < mGhostFreeIndex)
      {
         detachObject(&mGhostRefs[i]);
         mGhostRefs[i].lastUpdateChain = NULL;
         freeGhostInfo(&mGhostRefs[i]);
      }
   }
   TNLAssert((mGhostFreeIndex == 0) && (mGhostZeroUpdateIndex == 0), "Invalid indices.");
}

void GhostConnection::resetGhosting()
{
   if(!doesGhostFrom())
      return;
   // stop all ghosting activity
   // send a message to the other side notifying of this
   
   mGhosting = false;
   mScoping = false;
   rpcEndGhosting();
   mGhostingSequence++;
   clearGhostInfo();
   //TNLAssert(validateGhostArray(), "Invalid ghost array!");
}

//-----------------------------------------------------------------------------

NetObject *GhostConnection::resolveGhost(S32 id)
{
   if(id == -1)
      return NULL;

   return mLocalGhosts[id];
}

NetObject *GhostConnection::resolveGhostParent(S32 id)
{
   return mGhostRefs[id].obj;
}

S32 GhostConnection::getGhostIndex(NetObject *obj)
{
   if(!obj)
      return -1;
   if(!doesGhostFrom())
      return obj->mNetIndex;
   S32 index = obj->getHashId() & GhostLookupTableMask;

   for(GhostInfo *gptr = mGhostLookupTable[index]; gptr; gptr = gptr->nextLookupInfo)
   {
      if(gptr->obj == obj && (gptr->flags & (GhostInfo::KillingGhost | GhostInfo::Ghosting | GhostInfo::NotYetGhosted | GhostInfo::KillGhost)) == 0)
         return gptr->index;
   }
   return -1;
}

//-----------------------------------------------------------------------------

void GhostConnection::onEndGhosting()
{
}

void GhostConnection::onStartGhosting()
{
}

};

// --- END OF FILE: tnl/ghostConnection.cpp ---
// --- START OF FILE: tnl/huffmanStringProcessor.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlBitStream.h"
#include "tnlVector.h"
#include "tnlHuffmanStringProcessor.h"

namespace TNL {

namespace HuffmanStringProcessor {
   // This is extern so we can keep our huge hairy table definition at the end of the file.
   extern const U32 mCharFreqs[256];
   bool mTablesBuilt = false;

   struct HuffNode {
      U32 pop;

      S16 index0;
      S16 index1;
   };
   struct HuffLeaf {
      U32 pop;

      U8  numBits;
      U8  symbol;
      U32 code;   // no code should be longer than 32 bits.
   };

   Vector<HuffNode> mHuffNodes;
   Vector<HuffLeaf> mHuffLeaves;

   void buildTables();

   // We have to be a bit careful with these, since they are pointers...
   struct HuffWrap {
      HuffNode* pNode;
      HuffLeaf* pLeaf;

     public:
      HuffWrap() : pNode(NULL), pLeaf(NULL) { }

      void set(HuffLeaf* in_leaf) { pNode = NULL; pLeaf = in_leaf; }
      void set(HuffNode* in_node) { pLeaf = NULL; pNode = in_node; }

      U32 getPop() { if (pNode) return pNode->pop; else return pLeaf->pop; }
   };


   S16 determineIndex(HuffWrap&);

   void generateCodes(BitStream&, S32, S32);
};

//bool HuffmanStringProcessor::mTablesBuilt = false;
//Vector<HuffmanStringProcessor::HuffNode> HuffmanStringProcessor::mHuffNodes;
//Vector<HuffmanStringProcessor::HuffLeaf> HuffmanStringProcessor::mHuffLeaves;


void HuffmanStringProcessor::buildTables()
{
   TNLAssert(mTablesBuilt == false, "Cannot build tables twice!");
   mTablesBuilt = true;

   S32 i;

   // First, construct the array of wraps...
   //
   mHuffLeaves.setSize(256);
   mHuffNodes.reserve(256);
   mHuffNodes.setSize(mHuffNodes.size() + 1);
   for (i = 0; i < 256; i++) {
      HuffLeaf& rLeaf = mHuffLeaves[i];

      rLeaf.pop    = mCharFreqs[i] + 1;
      rLeaf.symbol = U8(i);

      memset(&rLeaf.code, 0, sizeof(rLeaf.code));
      rLeaf.numBits = 0;
   }

   S32 currWraps = 256;
   HuffWrap* pWrap = new HuffWrap[256];
   for (i = 0; i < 256; i++) {
      pWrap[i].set(&mHuffLeaves[i]);
   }

   while (currWraps != 1) {
      U32 min1 = 0xfffffffe, min2 = 0xffffffff;
      S32 index1 = -1, index2 = -1;

      for (i = 0; i < currWraps; i++) {
         if (pWrap[i].getPop() < min1) {
            min2   = min1;
            index2 = index1;

            min1   = pWrap[i].getPop();
            index1 = i;
         } else if (pWrap[i].getPop() < min2) {
            min2   = pWrap[i].getPop();
            index2 = i;
         }
      }
      TNLAssert(index1 != -1 && index2 != -1 && index1 != index2, "hrph");

      // Create a node for this...
      mHuffNodes.setSize(mHuffNodes.size() + 1);
      HuffNode& rNode = mHuffNodes.last();
      rNode.pop    = pWrap[index1].getPop() + pWrap[index2].getPop();
      rNode.index0 = determineIndex(pWrap[index1]);
      rNode.index1 = determineIndex(pWrap[index2]);

      S32 mergeIndex = index1 > index2 ? index2 : index1;
      S32 nukeIndex  = index1 > index2 ? index1 : index2;
      pWrap[mergeIndex].set(&rNode);

      if (index2 != (currWraps - 1)) {
         pWrap[nukeIndex] = pWrap[currWraps - 1];
      }
      currWraps--;
   }
   TNLAssert(currWraps == 1, "wrong wraps?");
   TNLAssert(pWrap[0].pNode != NULL && pWrap[0].pLeaf == NULL, "Wrong wrap type!");

   // Ok, now we have one wrap, which is a node.  we need to make sure that this
   //  is the first node in the node list.
   mHuffNodes[0] = *(pWrap[0].pNode);
   delete [] pWrap;

   U32 code = 0;
   BitStream bs((U8 *) &code, 4);

   generateCodes(bs, 0, 0);
}

void HuffmanStringProcessor::generateCodes(BitStream& rBS, S32 index, S32 depth)
{
   if (index < 0) {
      // leaf node, copy the code in, and back out...
      HuffLeaf& rLeaf = mHuffLeaves[-(index + 1)];

      memcpy(&rLeaf.code, rBS.getBuffer(), sizeof(rLeaf.code));
      rLeaf.numBits = depth;
   } else {
      HuffNode& rNode = mHuffNodes[index];

      S32 pos = rBS.getBitPosition();

      rBS.writeFlag(false);
      generateCodes(rBS, rNode.index0, depth + 1);

      rBS.setBitPosition(pos);
      rBS.writeFlag(true);
      generateCodes(rBS, rNode.index1, depth + 1);

      rBS.setBitPosition(pos);
   }
}

S16 HuffmanStringProcessor::determineIndex(HuffWrap& rWrap)
{
   if (rWrap.pLeaf != NULL) {
      TNLAssert(rWrap.pNode == NULL, "um, never.");

      return -((rWrap.pLeaf - mHuffLeaves.address()) + 1);
   } else {
      TNLAssert(rWrap.pNode != NULL, "um, never.");

      return rWrap.pNode - mHuffNodes.address();
   }
}

bool HuffmanStringProcessor::readHuffBuffer(BitStream* pStream, char* out_pBuffer)
{
   if (mTablesBuilt == false)
      buildTables();

   if (pStream->readFlag()) {
      U32 len = pStream->readInt(8);
      for (U32 i = 0; i < len; i++) {
         S32 index = 0;
         while (true) {
            if (index >= 0) {
               if (pStream->readFlag() == true) {
                  index = mHuffNodes[index].index1;
               } else {
                  index = mHuffNodes[index].index0;
               }
            } else {
               out_pBuffer[i] = mHuffLeaves[-(index+1)].symbol;
               break;
            }
         }
      }
      out_pBuffer[len] = '\0';
      return true;
   } else {
      // Uncompressed string...
      U32 len = pStream->readInt(8);
      pStream->read(len, out_pBuffer);
      out_pBuffer[len] = '\0';
      return true;
   }
}

bool HuffmanStringProcessor::writeHuffBuffer(BitStream* pStream, const char* out_pBuffer, U32 maxLen)
{
   if (out_pBuffer == NULL) {
      pStream->writeFlag(false);
      pStream->writeInt(0, 8);
      return true;
   }

   if (mTablesBuilt == false)
      buildTables();

   U32 len = out_pBuffer ? strlen(out_pBuffer) : 0;
   TNLAssertV(len <= 255, ("String \"%s\" TOO long for writeString", out_pBuffer));
   if (len > maxLen)
      len = maxLen;

   U32 numBits = 0;
   U32 i;
   for (i = 0; i < len; i++)
      numBits += mHuffLeaves[(unsigned char)out_pBuffer[i]].numBits;

   if (numBits >= (len * 8)) {
      pStream->writeFlag(false);
      pStream->writeInt(len, 8);
      pStream->write(len, out_pBuffer);
   } else {
      pStream->writeFlag(true);
      pStream->writeInt(len, 8);
      for (i = 0; i < len; i++) {
         HuffLeaf& rLeaf = mHuffLeaves[((unsigned char)out_pBuffer[i])];
         pStream->writeBits(rLeaf.numBits, &rLeaf.code);
      }
   }

   return true;
}

const U32 HuffmanStringProcessor::mCharFreqs[256] = {
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
329   ,
21    ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
2809  ,
68    ,
0     ,
27    ,
0     ,
58    ,
3     ,
62    ,
4     ,
7     ,
0     ,
0     ,
15    ,
65    ,
554   ,
3     ,
394   ,
404   ,
189   ,
117   ,
30    ,
51    ,
27    ,
15    ,
34    ,
32    ,
80    ,
1     ,
142   ,
3     ,
142   ,
39    ,
0     ,
144   ,
125   ,
44    ,
122   ,
275   ,
70    ,
135   ,
61    ,
127   ,
8     ,
12    ,
113   ,
246   ,
122   ,
36    ,
185   ,
1     ,
149   ,
309   ,
335   ,
12    ,
11    ,
14    ,
54    ,
151   ,
0     ,
0     ,
2     ,
0     ,
0     ,
211   ,
0     ,
2090  ,
344   ,
736   ,
993   ,
2872  ,
701   ,
605   ,
646   ,
1552  ,
328   ,
305   ,
1240  ,
735   ,
1533  ,
1713  ,
562   ,
3     ,
1775  ,
1149  ,
1469  ,
979   ,
407   ,
553   ,
59    ,
279   ,
31    ,
0     ,
0     ,
0     ,
68    ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0
};

};

// --- END OF FILE: tnl/huffmanStringProcessor.cpp ---
// --- START OF FILE: tnl/journal.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlJournal.h"
#include "tnlEndian.h"
#include "tnlLog.h"

namespace TNL
{

Vector<JournalEntryRecord *> *JournalEntryRecord::mEntryVector;

bool Journal::mInsideEntrypoint = false;
Journal::Mode Journal::mCurrentMode = Journal::Inactive;
U32 Journal::mReadBreakBitPos = 0;

FILE *Journal::mJournalFile = NULL;
BitStream Journal::mWriteStream;
BitStream Journal::mReadStream;
Journal *Journal::mJournal = NULL;
U32 Journal::mWritePosition = 0;
U32 Journal::mBreakBlockIndex = 0;
U32 Journal::mBlockIndex = 0;

JournalBlockTypeToken *JournalBlockTypeToken::mList = NULL;
bool JournalBlockTypeToken::mInitialized = false;

JournalBlockTypeToken::JournalBlockTypeToken(const char *typeString)
{
   mString = typeString;
   mValue = 0xFFFFFFFF;
   mNext = mList;
   mList = this;
}

S32 QSORT_CALLBACK JBTTCompare(JournalBlockTypeToken **a, JournalBlockTypeToken **b)
{
   return strcmp((*a)->getString(), (*b)->getString());
}

U32 JournalBlockTypeToken::getValue()
{
   if(!mInitialized)
   {
      mInitialized = true;
      Vector<JournalBlockTypeToken *> vec;
      for(JournalBlockTypeToken *walk = mList; walk; walk = walk->mNext)
         vec.push_back(walk);

      vec.sort(JBTTCompare);
      U32 lastValue = 0;
      const char *lastString = "";
      for(S32 i = 0; i < vec.size(); i++)
      {
         if(!strcmp(vec[i]->mString, lastString))
            vec[i]->mValue = lastValue;
         else
         {
            lastValue++;
            vec[i]->mValue = lastValue;
            lastString = vec[i]->mString;
         }
      }
   }
   return mValue;
}

const char *JournalBlockTypeToken::findName(U32 value)
{
   for(JournalBlockTypeToken *walk = mList; walk; walk = walk->mNext)
      if(walk->mValue == value)
         return walk->mString;
   return "INVALID";
}

JournalEntryRecord::JournalEntryRecord(const char *functionName)
{
   S32 i;
   if(!mEntryVector)
      mEntryVector = new Vector<JournalEntryRecord *>;

   for(i = 0; i < mEntryVector->size(); i++)
   {
      if(strcmp((*mEntryVector)[i]->mFunctionName, functionName) < 0)
         break;
   }
   mEntryVector->insert(i);
   (*mEntryVector)[i] = this;
   mFunctionName = functionName;
   mEntryIndex = 0;
}

JournalEntryRecord::~JournalEntryRecord()
{
   if(mEntryVector)
   {
      delete mEntryVector;
      mEntryVector = NULL;
   }
}

Journal::Journal()
{
   TNLAssert(mJournal == NULL, "Cannot construct more than one Journal instance.");
   mJournal = this;
}

// the journal stream is written as a single continuous bit stream.
// the first four bytes of the stream are the total number of bits in
// the stream.  As a block is written, the bits in the write stream are
// all erased except for those in the last byte, which are moved to the first.
void Journal::syncWriteStream()
{
   if(mWriteStream.getBytePosition() == 0)
      return;

   U32 totalBits = (mWritePosition << 3) + mWriteStream.getBitPosition();
   
   // seek back to the beginning
   fseek(mJournalFile, 0, SEEK_SET);

   // write the new total bits
   U32 writeBits = convertHostToLEndian(totalBits);
   fwrite(&writeBits, 1, sizeof(U32), mJournalFile);

   // seek to the writing position
   fseek(mJournalFile, mWritePosition, SEEK_SET);

   U32 bytesToWrite = mWriteStream.getBytePosition();
   // write the bytes to the file
   fwrite(mWriteStream.getBuffer(), 1, bytesToWrite, mJournalFile);
   fflush(mJournalFile);

   // adjust the write stream
   if(totalBits & 7)
   {
      U8 *buffer = mWriteStream.getBuffer();
      buffer[0] = buffer[bytesToWrite - 1];
      mWriteStream.setBitPosition(totalBits & 7);
      mWritePosition += bytesToWrite - 1;
   }
   else
   {
      mWritePosition += bytesToWrite;
      mWriteStream.setBitPosition(0);
   }
}

void Journal::record(const char *fileName)
{
   mJournalFile = fopen(fileName, "wb");
   if(mJournalFile)
   {
      mCurrentMode = Record;
      mWritePosition = sizeof(U32);
   }
}

void Journal::load(const char *fileName)
{
   FILE *theJournal = fopen(fileName, "rb");
   if(!theJournal)
      return;

   fseek(theJournal, 0, SEEK_END);
   U32 fileSize = ftell(theJournal);
   fseek(theJournal, 0, SEEK_SET);

   mReadStream.resize(fileSize);
   U32 bitCount;
   fread(mReadStream.getBuffer(), 1, fileSize, theJournal);
   mReadStream.read(&bitCount);
   mReadStream.setMaxBitSizes(bitCount);

   if(!mReadBreakBitPos || mReadBreakBitPos > bitCount)
      mReadBreakBitPos = bitCount;

   fclose(theJournal);
   mCurrentMode = Playback;
}

void Journal::callEntry(const char *funcName, Functor *theCall)
{
   if(mCurrentMode == Playback)
      return;

   TNLAssert(mInsideEntrypoint == false, "Journal entries cannot be reentrant!");
   mInsideEntrypoint = true;

   S32 entryIndex;
   for(entryIndex = 0; entryIndex < JournalEntryRecord::mEntryVector->size(); entryIndex++)
   {
      if(!strcmp((*JournalEntryRecord::mEntryVector)[entryIndex]->mFunctionName, funcName))
         break;
   }
   TNLAssert(entryIndex != JournalEntryRecord::mEntryVector->size(), "No entry point found!");

   if(mCurrentMode == Record)
   {
#ifdef TNL_ENABLE_BIG_JOURNALS
      TNL_JOURNAL_WRITE( (U16(0x1234)) );
#endif
      mWriteStream.writeRangedU32(entryIndex, 0, JournalEntryRecord::mEntryVector->size() - 1);
      theCall->write(mWriteStream);
#ifdef TNL_ENABLE_BIG_JOURNALS
      TNL_JOURNAL_WRITE( (U16(0x5678)) );
#endif
      syncWriteStream();
   }
   theCall->dispatch(this);
   mInsideEntrypoint = false;
}

void Journal::checkReadPosition()
{
   if(!mReadStream.isValid() || mReadStream.getBitPosition() >= mReadBreakBitPos)
      TNL_DEBUGBREAK();
}

void Journal::beginBlock(U32 blockId, bool writeBlock)
{
   if(writeBlock)
   {
#ifdef TNL_ENABLE_BIG_JOURNALS
      TNL_JOURNAL_WRITE( (U16(0x1234 ^ blockId)) );
#endif
   }
   else
   {
      mBlockIndex++;
      if(mBreakBlockIndex && mBlockIndex >= mBreakBlockIndex)
         TNL_DEBUGBREAK();

#ifdef TNL_ENABLE_BIG_JOURNALS
      U16 startToken;
      TNL_JOURNAL_READ( (&startToken) );
      if((startToken ^ 0x1234) != blockId)
      {
         logprintf("Expected token %s - got %s", JournalBlockTypeToken::findName(blockId), JournalBlockTypeToken::findName(startToken ^ 0x1234));
         TNL_DEBUGBREAK();
      }
#endif
   }
}

void Journal::endBlock(U32 blockId, bool writeBlock)
{
   if(writeBlock)
   {
#ifdef TNL_ENABLE_BIG_JOURNALS
      TNL_JOURNAL_WRITE( (U16(0x5678 ^ blockId)) );
#endif
      syncWriteStream();
   }
   else
   {
#ifdef TNL_ENABLE_BIG_JOURNALS
      U16 endToken;
      TNL_JOURNAL_READ( (&endToken) );
      if((endToken ^ 0x5678) != blockId)
      {
         logprintf("Expected token %s - got %s", JournalBlockTypeToken::findName(blockId), JournalBlockTypeToken::findName(endToken ^ 0x5678));
         TNL_DEBUGBREAK();
      }
#endif
      checkReadPosition();
   }
}

void Journal::processNextJournalEntry()
{
   if(mCurrentMode != Playback)
      return;

#ifdef TNL_ENABLE_BIG_JOURNALS
   U16 token;
   TNL_JOURNAL_READ( (&token) );
   if(token != 0x1234)
      TNL_DEBUGBREAK();
#endif

   U32 index = mReadStream.readRangedU32(0, JournalEntryRecord::mEntryVector->size());

   JournalEntryRecord *theEntry = (*JournalEntryRecord::mEntryVector)[index];

   // check for errors...
   if(!theEntry)
   {
      TNLAssert(0, "blech!");
   }
   theEntry->mFunctor->read(mReadStream);

#ifdef TNL_ENABLE_BIG_JOURNALS
   TNL_JOURNAL_READ( (&token) );
   if(token != 0x5678)
      TNL_DEBUGBREAK();
#endif

   checkReadPosition();

   mInsideEntrypoint = true;
   theEntry->mFunctor->dispatch(this);
   mInsideEntrypoint = false;
}

};


// --- END OF FILE: tnl/journal.cpp ---
// --- START OF FILE: tnl/log.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlLog.h"
#include "tnlDataChunker.h"
#include <stdarg.h>

namespace TNL
{

LogConsumer *LogConsumer::mLinkedList = NULL;

LogConsumer::LogConsumer()
{
   mNextConsumer = mLinkedList;
   if(mNextConsumer)
      mNextConsumer->mPrevConsumer = this;
   mPrevConsumer = NULL;
   mLinkedList = this;
}

LogConsumer::~LogConsumer()
{
   if(mNextConsumer)
      mNextConsumer->mPrevConsumer = mPrevConsumer;
   if(mPrevConsumer)
      mPrevConsumer->mNextConsumer = mNextConsumer;
   else
      mLinkedList = mNextConsumer;
}

LogType *LogType::linkedList = NULL;
LogType *LogType::current = NULL;

#ifdef TNL_ENABLE_LOGGING

LogType *LogType::find(const char *name)
{
   static ClassChunker<LogType> logTypeChunker(4096);

   for(LogType *walk = linkedList; walk; walk = walk->next)
      if(!strcmp(walk->typeName, name))
         return walk;
   LogType *ret = logTypeChunker.alloc();
   ret->next = linkedList;
   linkedList = ret;
   ret->isEnabled = false;
   ret->typeName = name;
   return ret;
}
#endif

void LogConsumer::logString(const char *string)
{
   // by default the LogConsumer will output to the platform debug 
   // string printer, but only if we're in debug mode
#ifdef TNL_DEBUG
   Platform::outputDebugString(string);
   Platform::outputDebugString("\n");
#endif
}

void logprintf(const char *format, ...)
{
   char buffer[4096];
   U32 bufferStart = 0;
   if(LogType::current)
   {
      strcpy(buffer, LogType::current->typeName);
      bufferStart = strlen(buffer);

      buffer[bufferStart] = ':';
      buffer[bufferStart+1] = ' ';
      bufferStart += 2;
   }
   va_list s;
   va_start( s, format );
   dVsprintf(buffer + bufferStart, sizeof(buffer) - bufferStart, format, s);
   for(LogConsumer *walk = LogConsumer::getLinkedList(); walk; walk = walk->getNext())
      walk->logString(buffer);
   va_end(s);
   Platform::outputDebugString(buffer);
   Platform::outputDebugString("\n");
}

};


// --- END OF FILE: tnl/log.cpp ---
// --- START OF FILE: tnl/netBase.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlNetBase.h"
#include "tnlVector.h"
#include "tnlLog.h"

namespace TNL
{

#define INITIAL_CRC_VALUE 0xFFFFFFFF

NetClassRep *NetClassRep::mClassLinkList = NULL;
U32 NetClassRep::mNetClassBitSize[NetClassGroupCount][NetClassTypeCount] = {{0, },};
Vector<NetClassRep *> NetClassRep::mClassTable[NetClassGroupCount][NetClassTypeCount];
U32 NetClassRep::mClassCRC[NetClassGroupCount] = {INITIAL_CRC_VALUE, };

bool NetClassRep::mInitialized = false;

NetClassRep::NetClassRep()
{
   mInitialUpdateCount = 0;
   mInitialUpdateBitsUsed = 0;
   mPartialUpdateCount = 0;
   mPartialUpdateBitsUsed = 0;
}

Object* NetClassRep::create(const char* className)
{
   TNLAssert(mInitialized, "creating an object before NetClassRep::initialize.");

   for (NetClassRep *walk = mClassLinkList; walk; walk = walk->mNextClass)
      if (!strcmp(walk->getClassName(), className))
         return walk->create();

   TNLAssertV(0,("Couldn't find class rep for dynamic class: %s", className));
   return NULL;
}

//--------------------------------------
Object* NetClassRep::create(const U32 groupId, const U32 typeId, const U32 classId)
{
   TNLAssert(mInitialized, "creating an object before NetClassRep::initialize.");
   TNLAssert(classId < U32(mClassTable[groupId][typeId].size()), "Class id out of range.");
   TNLAssert(mClassTable[groupId][typeId][classId] != NULL, "No class with declared id type.");

   if(mClassTable[groupId][typeId][classId])
      return mClassTable[groupId][typeId][classId]->create();
   return NULL;
}

//--------------------------------------

static S32 QSORT_CALLBACK ACRCompare(const void *aptr, const void *bptr)
{
   const NetClassRep *a = *((const NetClassRep **) aptr);
   const NetClassRep *b = *((const NetClassRep **) bptr);

   if(a->getClassVersion() != b->getClassVersion())
      return a->getClassVersion() - b->getClassVersion();
   return strcmp(a->getClassName(), b->getClassName());
}

void NetClassRep::initialize()
{
   if(mInitialized)
      return;
   Vector<NetClassRep *> dynamicTable;
      
   NetClassRep *walk;
   
   for (U32 group = 0; group < NetClassGroupCount; group++)
   {
      U32 groupMask = 1 << group;
      for(U32 type = 0; type < NetClassTypeCount; type++)
      {
         for (walk = mClassLinkList; walk; walk = walk->mNextClass)
         {
            if(walk->getClassType() == type && walk->mClassGroupMask & groupMask)
               dynamicTable.push_back(walk);
         }
         if(!dynamicTable.size())
            continue;

         qsort((void *) &dynamicTable[0], dynamicTable.size(), sizeof(NetClassRep *), ACRCompare);

         TNLLogBlock(LogNetBase,
            logprintf("Class Group: %d  Class Type: %d  count: %d",
               group, type, dynamicTable.size());
            for(S32 i = 0; i < dynamicTable.size(); i++)
               logprintf("%s", dynamicTable[i]->getClassName());
         )

         mClassTable[group][type] = dynamicTable;
   
         for(U32 i = 0; i < mClassTable[group][type].size();i++)
            mClassTable[group][type][i]->mClassId[group] = i;

         mNetClassBitSize[group][type] = 
               getBinLog2(getNextPow2(mClassTable[group][type].size() + 1));
         dynamicTable.clear();
      }
   }
   mInitialized = true;
}

void NetClassRep::logBitUsage()
{
   logprintf("Net Class Bit Usage:");
   for(NetClassRep *walk = mClassLinkList; walk; walk = walk->mNextClass)
   {
      if(walk->mInitialUpdateCount)
         logprintf("%s (Initial) - Count: %d   Avg Size: %g", walk->mClassName, walk->mInitialUpdateCount, walk->mInitialUpdateBitsUsed / F32(walk->mInitialUpdateCount));
      if(walk->mPartialUpdateCount)
         logprintf("%s (Partial) - Count: %d   Avg Size: %g", walk->mClassName, walk->mPartialUpdateCount, walk->mPartialUpdateBitsUsed / F32(walk->mPartialUpdateCount));
   }
}


Object::Object()
{
   mFirstObjectRef = NULL;
   mRefCount = 0;
}

Object::~Object()
{
   TNLAssert(mRefCount == 0, "Error! Object deleted with non-zero reference count!");
   // loop through the linked list of object references and NULL
   // out all pointers to this object.

   SafeObjectRef *walk = mFirstObjectRef;
   while(walk)
   {
      SafeObjectRef *next = walk->mNextObjectRef;
      walk->mObject = NULL;
      walk->mPrevObjectRef = NULL;
      walk->mNextObjectRef = NULL;
      walk = next;
   }
}

//--------------------------------------
NetClassRep* Object::getClassRep() const
{
   return NULL;
}

};

// --- END OF FILE: tnl/netBase.cpp ---
// --- START OF FILE: tnl/netConnection.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlNetBase.h"
#include "tnlNetConnection.h"
#include "tnlNetInterface.h"
#include "tnlLog.h"
#include "tnlRandom.h"
#include "tnlSymmetricCipher.h"
#include "tnlAsymmetricKey.h"
#include "tnlConnectionStringTable.h"

#include <stdarg.h>


namespace TNL {

NetConnectionRep *NetConnectionRep::mLinkedList = NULL;

NetConnection *NetConnectionRep::create(const char *name)
{
   for(NetConnectionRep *walk = mLinkedList; walk; walk = walk->mNext)
      if(walk->mCanRemoteCreate && !strcmp(name, walk->mClassRep->getClassName()))
      {
         Object *obj = walk->mClassRep->create();
         NetConnection *ret = dynamic_cast<NetConnection *>(obj);
         TNLAssert(ret != NULL, "Invalid TNL_IMPLEMENT_NETCONNECTION");
         if(!ret)
            delete obj;
         return ret;
      }

   return NULL;
}

static const char *packetTypeNames[] = 
{
   "DataPacket",
   "PingPacket",
   "AckPacket",
};

//-----------------------------------------------------------------

NetConnection::NetConnection()
{
   mInitialSendSeq = Random::readI();
   Random::read(mConnectionParameters.mNonce.data, Nonce::NonceSize);

   mSimulatedLatency = 0;
   mSimulatedPacketLoss = 0;

   mLastPacketRecvTime = 0;
   mLastUpdateTime = 0;
   mRoundTripTime = 0;
   mSendDelayCredit = 0;
   mConnectionState = NotConnected;
   
   mNotifyQueueHead = NULL;
   mNotifyQueueTail = NULL;
   
   mLocalRate.maxRecvBandwidth = DefaultFixedBandwidth;
   mLocalRate.maxSendBandwidth = DefaultFixedBandwidth;
   mLocalRate.minPacketRecvPeriod = DefaultFixedSendPeriod;
   mLocalRate.minPacketSendPeriod = DefaultFixedSendPeriod;

   mRemoteRate = mLocalRate;
   mLocalRateChanged = true;
   computeNegotiatedRate();

   mPingSendCount = 0;
   mLastPingSendTime = 0;

   mLastSeqRecvd = 0;
   mHighestAckedSeq = mInitialSendSeq;
   mLastSendSeq = mInitialSendSeq; // start sending at mInitialSendSeq + 1
   mAckMask[0] = 0;
   mLastRecvAckAck = 0;

   // Adaptive
   cwnd = 2;
   ssthresh = 30;
   mLastSeqRecvdAck = 0;

   mPingTimeout = DefaultPingTimeout;
   mPingRetryCount = DefaultPingRetryCount;
   mStringTable = NULL;
}

void NetConnection::setInitialRecvSequence(U32 sequence)
{ 
   mInitialRecvSeq = mLastSeqRecvd = mLastRecvAckAck = sequence;
}

void NetConnection::clearAllPacketNotifies()
{
   while(mNotifyQueueHead)
      handleNotify(0, false);
}

NetConnection::~NetConnection()
{
   clearAllPacketNotifies();
   delete mStringTable;

   TNLAssert(mNotifyQueueHead == NULL, "Uncleared notifies remain.");
}

//--------------------------------------------------------------------
void NetConnection::setNetAddress(const Address &addr)
{
   mNetAddress = addr;
}

const Address &NetConnection::getNetAddress()
{
   return mNetAddress;
}

NetConnection::PacketNotify::PacketNotify()
{
   rateChanged = false;
   sendTime = 0;
}

bool NetConnection::checkTimeout(U32 time)
{
   if(!isNetworkConnection())
      return false;

   if(!mLastPingSendTime)
      mLastPingSendTime = time;

   U32 timeout = mPingTimeout;
   U32 timeoutCount = mPingRetryCount;

   if(isAdaptive())
   {
      if(hasUnackedSentPackets())
      {
         timeout = AdaptiveUnackedSentPingTimeout;
      }
      else
      {
         timeoutCount = AdaptivePingRetryCount;
         if(!mPingSendCount)
            timeout = AdaptiveInitialPingTimeout;
      }
   }
   if((time - mLastPingSendTime) > timeout)
   {
      if(mPingSendCount >= timeoutCount)
         return true;
      mLastPingSendTime = time;
      mPingSendCount++;
      sendPingPacket();
   }
   return false;
}

void NetConnection::keepAlive()
{
   mLastPingSendTime = 0;
   mPingSendCount = 0;
}

//--------------------------------------------------------------------

char NetConnection::mErrorBuffer[256];

void NetConnection::setLastError(const char *fmt, ...)
{
   va_list argptr;
   va_start(argptr, fmt);
   dVsprintf(mErrorBuffer, sizeof(mErrorBuffer), fmt, argptr);
   // setLastErrors assert in debug builds
   
   TNLAssert(0, mErrorBuffer);
   va_end(argptr);
}

//--------------------------------------------------------------------

void NetConnection::writeRawPacket(BitStream *bstream, NetPacketType packetType)
{
   writePacketHeader(bstream, packetType);
   if(packetType == DataPacket)
   {
      PacketNotify *note = allocNotify();
      if(!mNotifyQueueHead)
         mNotifyQueueHead = note;
      else
         mNotifyQueueTail->nextPacket = note;
      mNotifyQueueTail = note;
      note->nextPacket = NULL;
      note->sendTime = mInterface->getCurrentTime();

      writePacketRateInfo(bstream, note);
      S32 start = bstream->getBitPosition();
      bstream->setStringTable(mStringTable);

      TNLLogMessageV(LogNetConnection, ("NetConnection %s: START %s", mNetAddress.toString(), getClassName()) );
      writePacket(bstream, note);
      TNLLogMessageV(LogNetConnection, ("NetConnection %s: END %s - %d bits", mNetAddress.toString(), getClassName(), bstream->getBitPosition() - start) );
   }
   if(!mSymmetricCipher.isNull())
   {
      mSymmetricCipher->setupCounter(mLastSendSeq, mLastSeqRecvd, packetType, 0);
      bstream->hashAndEncrypt(MessageSignatureBytes, PacketHeaderByteSize, mSymmetricCipher);
   }
}

void NetConnection::readRawPacket(BitStream *bstream)
{
   if(mSimulatedPacketLoss && Random::readF() < mSimulatedPacketLoss)
   {
      TNLLogMessageV(LogNetConnection, ("NetConnection %s: RECVDROP - %d", mNetAddress.toString(), getLastSendSequence()));
      return;
   }
   TNLLogMessageV(LogNetConnection, ("NetConnection %s: RECV- %d bytes", mNetAddress.toString(), bstream->getMaxReadBitPosition() >> 3));

   mErrorBuffer[0] = 0;
   if(readPacketHeader(bstream))
   {
      mLastPacketRecvTime = mInterface->getCurrentTime();

      readPacketRateInfo(bstream);
      bstream->setStringTable(mStringTable);
      readPacket(bstream);

      if(!bstream->isValid() && !mErrorBuffer[0])
         NetConnection::setLastError("Invalid Packet.");
      if(mErrorBuffer[0])
         getInterface()->handleConnectionError(this, mErrorBuffer);
      mErrorBuffer[0] = 0;
   }
}

//--------------------------------------------------------------------

void NetConnection::writePacketHeader(BitStream *stream, NetPacketType packetType)
{
   if(windowFull() && packetType == DataPacket)
      TNL_DEBUGBREAK();

   S32 ackByteCount = ((mLastSeqRecvd - mLastRecvAckAck + 7) >> 3);
   TNLAssert(ackByteCount <= MaxAckByteCount, "ackByteCount exceeds MaxAckByteCount!");
   
   if(packetType == DataPacket)
      mLastSendSeq++;
      
   stream->writeInt(packetType, 2);
   stream->writeInt(mLastSendSeq, 5); // write the first 5 bits of the send sequence
   stream->writeFlag(true); // high bit of first byte indicates this is a data packet.
   stream->writeInt(mLastSendSeq >> 5, SequenceNumberBitSize - 5); // write the rest of the send sequence
   stream->writeInt(mLastSeqRecvd, AckSequenceNumberBitSize);
   stream->writeInt(0, PacketHeaderPadBits);

   stream->writeRangedU32(ackByteCount, 0, MaxAckByteCount);

   U32 wordCount = (ackByteCount + 3) >> 2;

   for(U32 i = 0; i < wordCount; i++)
      stream->writeInt(mAckMask[i], i == wordCount - 1 ?
         (ackByteCount - (i * 4)) * 8 : 32);

   U32 sendDelay = mInterface->getCurrentTime() - mLastPacketRecvTime;
   if(sendDelay > 2047)
      sendDelay = 2047;
   stream->writeInt(sendDelay >> 3, 8);

   // if we're resending this header, we can't advance the
   // sequence recieved (in case this packet drops and the prev one
   // goes through) 

   if(packetType == DataPacket)
      mLastSeqRecvdAtSend[mLastSendSeq & PacketWindowMask] = mLastSeqRecvd;

   //if(isNetworkConnection())
   //{
   //   TNLLogMessageV(LogBlah, ("SND: mLSQ: %08x  pkLS: %08x  pt: %d abc: %d",
   //      mLastSendSeq, mLastSeqRecvd, packetType, ackByteCount));
   //}

   TNLLogMessageV(LogConnectionProtocol, ("build hdr %d %d", mLastSendSeq, packetType));
}

bool NetConnection::readPacketHeader(BitStream *pstream)
{
   // read in the packet header:
   //
   //   2 bits packet type
   //   low 5 bits of the packet sequence number
   //   1 bit game packet
   //   SequenceNumberBitSize-5 bits (packet seq number >> 5)
   //   AckSequenceNumberBitSize bits ackstart seq number
   //   PacketHeaderPadBits = 0 - padding to byte boundary
   //   after this point, if this is an encrypted packet, all the rest of the data will be encrypted

   //   rangedU32 - 0...MaxAckByteCount
   //
   // type is:
   //    00 data packet
   //    01 ping packet
   //    02 ack packet
   
   // next 0...ackByteCount bytes are ack flags
   //
   // return value is true if this is a valid data packet
   // or false if there is nothing more that should be read

   U32 pkPacketType     = pstream->readInt(2);
   U32 pkSequenceNumber = pstream->readInt(5);
   bool pkDataPacketFlg = pstream->readFlag();
   pkSequenceNumber = pkSequenceNumber | (pstream->readInt(SequenceNumberBitSize - 5) << 5);

   U32 pkHighestAck     = pstream->readInt(AckSequenceNumberBitSize);
   U32 pkPadBits        = pstream->readInt(PacketHeaderPadBits);

   if(pkPadBits != 0)
      return false;

   TNLAssert(pkDataPacketFlg, "Invalid packet header in NetConnection::readPacketHeader!");

   // verify packet ordering and acking and stuff
   // check if the 9-bit sequence is within the packet window
   // (within 31 packets of the last received sequence number).

   pkSequenceNumber |= (mLastSeqRecvd & SequenceNumberMask);
   // account for wrap around
   if(pkSequenceNumber < mLastSeqRecvd)
      pkSequenceNumber += SequenceNumberWindowSize;
   
   // in the following test, account for wrap around from 0
   if(pkSequenceNumber - mLastSeqRecvd > (MaxPacketWindowSize - 1))
   {
      // the sequence number is outside the window... must be out of order
      // discard.
      return false;
   }

   pkHighestAck |= (mHighestAckedSeq & AckSequenceNumberMask);
   // account for wrap around
   
   if(pkHighestAck < mHighestAckedSeq)
      pkHighestAck += AckSequenceNumberWindowSize;
   
   if(pkHighestAck > mLastSendSeq)
   {
      // the ack number is outside the window... must be an out of order
      // packet, discard.
      return false;
   }
   
   if(!mSymmetricCipher.isNull())
   {
      mSymmetricCipher->setupCounter(pkSequenceNumber, pkHighestAck, pkPacketType, 0);
      if(!pstream->decryptAndCheckHash(MessageSignatureBytes, PacketHeaderByteSize, mSymmetricCipher))
      {
         TNLLogMessage(LogNetConnection, "Packet failed crypto");
         return false;
      }
   }

   U32 pkAckByteCount   = pstream->readRangedU32(0, MaxAckByteCount);
   if(pkAckByteCount > MaxAckByteCount || pkPacketType >= InvalidPacketType)
      return false;
         
   U32 pkAckMask[MaxAckMaskSize];
   U32 pkAckWordCount = (pkAckByteCount + 3) >> 2;

   for(U32 i = 0; i < pkAckWordCount; i++)
      pkAckMask[i] = pstream->readInt(i == pkAckWordCount - 1 ? 
            (pkAckByteCount - (i * 4)) * 8 : 32);

   //if(isNetworkConnection())
   //{
   //   TNLLogMessageV(LogBlah, ("RCV: mHA: %08x  pkHA: %08x  mLSQ: %08x  pkSN: %08x  pkLS: %08x  pkAM: %08x",
   //      mHighestAckedSeq, pkHighestAck, mLastSendSeq, pkSequenceNumber, mLastSeqRecvd, pkAckMask[0]));
   //}

   U32 pkSendDelay = (pstream->readInt(8) << 3) + 4;

   TNLLogBlock(LogConnectionProtocol,
      for(U32 i = mLastSeqRecvd+1; i < pkSequenceNumber; i++)
         logprintf ("Not recv %d", i);
      logprintf("Recv %d %s", pkSequenceNumber, packetTypeNames[pkPacketType]);
   );

   // shift up the ack mask by the packet difference
   // this essentially nacks all the packets dropped

   U32 ackMaskShift = pkSequenceNumber - mLastSeqRecvd;

   // if we've missed more than a full word of packets, shift up by words
   while(ackMaskShift > 32)
   {
      for(S32 i = MaxAckMaskSize - 1; i > 0; i--)
         mAckMask[i] = mAckMask[i-1];
      mAckMask[0] = 0;
      ackMaskShift -= 32;
   }

   // the first word upshifts all NACKs, except for the low bit, which is a
   // 1 if this is a data packet (i.e. not a ping packet or an ack packet)
   U32 upShifted = (pkPacketType == DataPacket) ? 1 : 0; 

   for(U32 i = 0; i < MaxAckMaskSize; i++)
   {
      U32 nextShift = mAckMask[i] >> (32 - ackMaskShift);
      mAckMask[i] = (mAckMask[i] << ackMaskShift) | upShifted;
      upShifted = nextShift;
   }

   // do all the notifies...
   U32 notifyCount = pkHighestAck - mHighestAckedSeq;
   for(U32 i = 0; i < notifyCount; i++) 
   {
      U32 notifyIndex = mHighestAckedSeq + i + 1;

      U32 ackMaskBit = (pkHighestAck - notifyIndex) & 0x1F;
      U32 ackMaskWord = (pkHighestAck - notifyIndex) >> 5;

      bool packetTransmitSuccess = (pkAckMask[ackMaskWord] & (1 << ackMaskBit)) != 0;
      TNLLogMessageV(LogConnectionProtocol, ("Ack %d %d", notifyIndex, packetTransmitSuccess));

      mHighestAckedSendTime = 0;
      handleNotify(notifyIndex, packetTransmitSuccess);

      // Running average of roundTrip time
      if(mHighestAckedSendTime)
      {
         S32 roundTripDelta = mInterface->getCurrentTime() - (mHighestAckedSendTime + pkSendDelay);
         mRoundTripTime = mRoundTripTime * 0.9f + roundTripDelta * 0.1f;
         if(mRoundTripTime < 0)
            mRoundTripTime = 0;
      }      
      if(packetTransmitSuccess)
         mLastRecvAckAck = mLastSeqRecvdAtSend[notifyIndex & PacketWindowMask];
   }
   // the other side knows more about its window than we do.
   if(pkSequenceNumber - mLastRecvAckAck > MaxPacketWindowSize)
      mLastRecvAckAck = pkSequenceNumber - MaxPacketWindowSize;
   
   mHighestAckedSeq = pkHighestAck;

   // first things first...
   // ackback any pings or half-full windows

   keepAlive(); // notification that the connection is ok

   U32 prevLastSequence = mLastSeqRecvd;
   mLastSeqRecvd = pkSequenceNumber;

   if(pkPacketType == PingPacket || (pkSequenceNumber - mLastRecvAckAck > (MaxPacketWindowSize >> 1)))
   {
      // send an ack to the other side
      // the ack will have the same packet sequence as our last sent packet
      // if the last packet we sent was the connection accepted packet
      // we must resend that packet
      sendAckPacket();
   }
   return prevLastSequence != pkSequenceNumber && pkPacketType == DataPacket;
}

//--------------------------------------------------------------------

void NetConnection::writePacketRateInfo(BitStream *bstream, PacketNotify *note)
{
   note->rateChanged = mLocalRateChanged;
   mLocalRateChanged = false;
   if(bstream->writeFlag(note->rateChanged))
   {
      if(!bstream->writeFlag(mTypeFlags.test(ConnectionAdaptive)))
      {
         bstream->writeRangedU32(mLocalRate.maxRecvBandwidth, 0, MaxFixedBandwidth);
         bstream->writeRangedU32(mLocalRate.maxSendBandwidth, 0, MaxFixedBandwidth);
         bstream->writeRangedU32(mLocalRate.minPacketRecvPeriod, 1, MaxFixedSendPeriod);
         bstream->writeRangedU32(mLocalRate.minPacketSendPeriod, 1, MaxFixedSendPeriod);
      }
   }
}

void NetConnection::readPacketRateInfo(BitStream *bstream)
{
   if(bstream->readFlag())
   {
      if(bstream->readFlag())
         mTypeFlags.set(ConnectionRemoteAdaptive);
      else
      {
         mRemoteRate.maxRecvBandwidth = bstream->readRangedU32(0, MaxFixedBandwidth);
         mRemoteRate.maxSendBandwidth = bstream->readRangedU32(0, MaxFixedBandwidth);
         mRemoteRate.minPacketRecvPeriod = bstream->readRangedU32(1, MaxFixedSendPeriod);
         mRemoteRate.minPacketSendPeriod = bstream->readRangedU32(1, MaxFixedSendPeriod);
         computeNegotiatedRate();
      }
   }
}

void NetConnection::computeNegotiatedRate()
{
   mCurrentPacketSendPeriod = getMax(mLocalRate.minPacketSendPeriod, mRemoteRate.minPacketRecvPeriod);

   U32 maxBandwidth = getMin(mLocalRate.maxSendBandwidth, mRemoteRate.maxRecvBandwidth);
   mCurrentPacketSendSize = U32(maxBandwidth * mCurrentPacketSendPeriod * 0.001f);

   // make sure we don't try to overwrite the maximum packet size
   if(mCurrentPacketSendSize > MaxPacketDataSize)
      mCurrentPacketSendSize = MaxPacketDataSize;
}

void NetConnection::setIsAdaptive()
{
   mTypeFlags.set(ConnectionAdaptive);
   mLocalRateChanged = true;
}

void NetConnection::setFixedRateParameters(U32 minPacketSendPeriod, U32 minPacketRecvPeriod, U32 maxSendBandwidth, U32 maxRecvBandwidth)
{
   mTypeFlags.clear(ConnectionAdaptive);

   mLocalRate.maxRecvBandwidth = maxRecvBandwidth;
   mLocalRate.maxSendBandwidth = maxSendBandwidth;
   mLocalRate.minPacketRecvPeriod = minPacketRecvPeriod;
   mLocalRate.minPacketSendPeriod = minPacketSendPeriod;
   mLocalRateChanged = true;
   computeNegotiatedRate();
}

//--------------------------------------------------------------------

void NetConnection::sendPingPacket()
{
   PacketStream ps;
   writeRawPacket(&ps, PingPacket);
   TNLLogMessageV(LogConnectionProtocol, ("send ping %d", mLastSendSeq));

   sendPacket(&ps);
}

void NetConnection::sendAckPacket()
{
   PacketStream ps;
   writeRawPacket(&ps, AckPacket);
   TNLLogMessageV(LogConnectionProtocol, ("send ack %d", mLastSendSeq));

   sendPacket(&ps);
}

//--------------------------------------------------------------------

void NetConnection::handleNotify(U32 sequence, bool recvd)
{
   TNLLogMessageV(LogNetConnection, ("NetConnection %s: NOTIFY %d %s", mNetAddress.toString(), sequence, recvd ? "RECVD" : "DROPPED"));

   PacketNotify *note = mNotifyQueueHead;
   TNLAssert(note != NULL, "Error: got a notify with a null notify head.");
   mNotifyQueueHead = mNotifyQueueHead->nextPacket;

   if(note->rateChanged && !recvd)
      mLocalRateChanged = true;

   if(recvd)
   {
      mHighestAckedSendTime = note->sendTime;

      if(isAdaptive())
      {
         // Deal with updating our cwnd and ssthresh...
         if(cwnd < ssthresh)
         {
            // Slow start strategy
            cwnd++;
            TNLLogMessageV(LogNetConnection, ("PKT SSOK - ssthresh = %f     cwnd=%f", ssthresh, cwnd));

         } else {
            // We are in normal state..
            if(cwnd < MaxPacketWindowSize-2)
               cwnd += 1/cwnd;

            TNLLogMessageV(LogNetConnection, ("PKT   OK - ssthresh = %f     cwnd=%f", ssthresh, cwnd));

         }
      }

      packetReceived(note);
   }
   else
   {
      if(isAdaptive())
      {
         // Deal with updating our cwnd and ssthresh...
         ssthresh = (0.5f * ssthresh < 2) ? 2 : (0.5f * ssthresh);
         cwnd -= 1;
         if(cwnd < 2) cwnd = 2;

/*         TNLLogMessageV(LogNetConnection, ("  * ack=%f   pktDt=%d    time=%f (%d)     seq=%d %d %d %d",
                        ack,
                        ackDelta,
                        deltaT / 1000.0f,
                        deltaT,
                        mLastSeqRecvd,
                        mLastRecvAckAck,
                        mLastSeqRecvdAck,
                        mHighestAckedSeq
         )); */

      }

      packetDropped(note);
   }
   delete note;
}

//--------------------------------------------------------------------

void NetConnection::checkPacketSend(bool force, U32 curTime)
{
   U32 delay = mCurrentPacketSendPeriod;

   if(!force)
   {
      if(!isAdaptive())
      {
         if(curTime - mLastUpdateTime + mSendDelayCredit < delay)
            return;
      
         mSendDelayCredit = curTime - (mLastUpdateTime + delay - mSendDelayCredit);
         if(mSendDelayCredit > 1000)
            mSendDelayCredit = 1000;
      }
   }
   prepareWritePacket();
   if(windowFull() || !isDataToTransmit())
   {
      // there is nothing to transmit, or the window is full
      if(isAdaptive())
      {
         // Still, on an adaptive connection, we may need to send an ack here...

         // Check if we should ack. We use a heuristic to do this. (fuzzy logic!)
         S32 ackDelta = (mLastSeqRecvd - mLastSeqRecvdAck);
         F32 ack = ackDelta / 4.0f;

         // Multiply by the time since we've acked...
         // If we're much below 200, we don't want to ack; if we're much over we do.
         U32 deltaT = (curTime - mLastAckTime);
         ack = ack *  deltaT / 200.0f;

         if((ack > 1.0f || (ackDelta > (0.75*MaxPacketWindowSize))) && (mLastSeqRecvdAck != mLastSeqRecvd))
         {         
            mLastSeqRecvdAck = mLastSeqRecvd;
            mLastAckTime = curTime;
            sendAckPacket();
         }
      }
      return;
   }
   PacketStream stream(mCurrentPacketSendSize);
   mLastUpdateTime = curTime;

   writeRawPacket(&stream, DataPacket);   

   sendPacket(&stream);
}

bool NetConnection::windowFull()
{
   if(mLastSendSeq - mHighestAckedSeq >= (MaxPacketWindowSize - 2))
      return true;
   if(isAdaptive())
      return mLastSendSeq - mHighestAckedSeq >= cwnd;
   return false;
}

NetError NetConnection::sendPacket(BitStream *stream)
{
   if(mSimulatedPacketLoss && Random::readF() < mSimulatedPacketLoss)
   {
      TNLLogMessageV(LogNetConnection, ("NetConnection %s: SENDDROP - %d", mNetAddress.toString(), getLastSendSequence()));
      return NoError;
   }

   TNLLogMessageV(LogNetConnection, ("NetConnection %s: SEND - %d bytes", mNetAddress.toString(), stream->getBytePosition()));

   // do nothing on send if this is a demo replay.
   if(isLocalConnection())
   {
      // short circuit connection to the other side.
      // handle the packet, then force a notify.
      U32 size = stream->getBytePosition();

      stream->reset();
      stream->setMaxSizes(size, 0);
      
      mRemoteConnection->readRawPacket(stream);
      return NoError;
   }
   else
   {
      if(mSimulatedLatency)
      {
         mInterface->sendtoDelayed(getNetAddress(), stream, mSimulatedLatency);
         return NoError;
      }
      else
         return mInterface->sendto(getNetAddress(), stream);
   }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

// these are the virtual function defs for Connection -
// if your subclass has additional data to read / write / notify, add it in these functions.

void NetConnection::readPacket(BitStream *bstream)
{
}

void NetConnection::prepareWritePacket()
{
}

void NetConnection::writePacket(BitStream *bstream, PacketNotify *note)
{
}

void NetConnection::packetReceived(PacketNotify *note)
{
   if(mStringTable)
      mStringTable->packetReceived(&note->stringList);
}

void NetConnection::packetDropped(PacketNotify *note)
{
   if(mStringTable)
      mStringTable->packetDropped(&note->stringList);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void NetConnection::setTranslatesStrings()
{
   if(!mStringTable) 
      mStringTable = new ConnectionStringTable(this);
}

void NetConnection::setInterface(NetInterface *myInterface)
{
   mInterface = myInterface;
}

NetInterface *NetConnection::getInterface()
{
   return mInterface;
}

void NetConnection::setSymmetricCipher(SymmetricCipher *theCipher)
{
   mSymmetricCipher = theCipher;
}

void NetConnection::connect(NetInterface *theInterface, const Address &address, bool requestKeyExchange, bool requestCertificate)
{
   mConnectionParameters.mRequestKeyExchange = requestKeyExchange;
   mConnectionParameters.mRequestCertificate = requestCertificate;
   mConnectionParameters.mIsInitiator = true;

   setNetAddress(address);
   setInterface(theInterface);
   mInterface->startConnection(this);
}

void NetConnection::connectArranged(NetInterface *connectionInterface, const Vector<Address> &possibleAddresses, Nonce &nonce, Nonce &serverNonce, ByteBufferPtr sharedSecret, bool isInitiator, bool requestsKeyExchange, bool requestsCertificate)
{
   mConnectionParameters.mRequestKeyExchange = requestsKeyExchange;
   mConnectionParameters.mRequestCertificate = requestsCertificate;
   mConnectionParameters.mPossibleAddresses = possibleAddresses;
   mConnectionParameters.mIsInitiator = isInitiator;
   mConnectionParameters.mIsArranged = true;
   mConnectionParameters.mNonce = nonce;
   mConnectionParameters.mServerNonce = serverNonce;
   mConnectionParameters.mArrangedSecret = sharedSecret;
   mConnectionParameters.mArrangedSecret->takeOwnership();

   setInterface(connectionInterface);
   mInterface->startArrangedConnection(this);   
}

void NetConnection::disconnect(const char *reason)
{
   mInterface->disconnect(this, ReasonSelfDisconnect, reason);
}

void NetConnection::onConnectionEstablished()
{
   if(isInitiator())
      setIsConnectionToServer();
   else
      setIsConnectionToClient();
}

void NetConnection::onConnectionTerminated(TerminationReason, const char *)
{
}

void NetConnection::onConnectTerminated(TerminationReason, const char *)
{
}

void NetConnection::writeConnectRequest(BitStream *stream)
{
   stream->write(U32(getNetClassGroup()));
   stream->write(U32(NetClassRep::getClassGroupCRC(getNetClassGroup())));
}

bool NetConnection::readConnectRequest(BitStream *stream, const char **errorString)
{
   U32 classGroup, classCRC;
   stream->read(&classGroup);
   stream->read(&classCRC);

   if(classGroup == getNetClassGroup() && classCRC == NetClassRep::getClassGroupCRC(getNetClassGroup()))
      return true;

   *errorString = "CHR_INVALID";
   return false;
}

void NetConnection::writeConnectAccept(BitStream *stream)
{
   stream;
}

bool NetConnection::readConnectAccept(BitStream *stream, const char **errorString)
{
   stream;
   errorString;
   return true;
}

bool NetConnection::connectLocal(NetInterface *connectionInterface, NetInterface *serverInterface)
{
   Object *co = Object::create(getClassName());
   NetConnection *client = this;
   NetConnection *server = dynamic_cast<NetConnection *>(co);
   const char *error = NULL;
   PacketStream stream;

   if(!server)
	   goto errorOut;

   client->setInterface(connectionInterface);
   client->getConnectionParameters().mIsInitiator = true;
   client->getConnectionParameters().mIsLocal = true;
   server->getConnectionParameters().mIsLocal = true;

   server->setInterface(serverInterface);

   server->setInitialRecvSequence(client->getInitialSendSequence());
   client->setInitialRecvSequence(server->getInitialSendSequence());
   client->setRemoteConnectionObject(server);
   server->setRemoteConnectionObject(client);

   stream.setBytePosition(0);
   client->writeConnectRequest(&stream);
   stream.setBytePosition(0);
   if(!server->readConnectRequest(&stream, &error))
      goto errorOut;

   stream.setBytePosition(0);
   server->writeConnectAccept(&stream);
   stream.setBytePosition(0);

   if(!client->readConnectAccept(&stream, &error))
      goto errorOut;

   client->setConnectionState(NetConnection::Connected);
   server->setConnectionState(NetConnection::Connected);

   client->onConnectionEstablished();
   server->onConnectionEstablished();
   connectionInterface->addConnection(client);
   serverInterface->addConnection(server);
   return true;
errorOut:
   delete server;
   return false;
}

};

// --- END OF FILE: tnl/netConnection.cpp ---
// --- START OF FILE: tnl/netInterface.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlAsymmetricKey.h"
#include "tnlNetConnection.h"
#include "tnlNetInterface.h"
#include "tnlBitStream.h"
#include "tnlRandom.h"
#include "tnlNetObject.h"
#include "tnlClientPuzzle.h"
#include "tnlCertificate.h"
#include <mycrypt.h>

namespace TNL {

//-----------------------------------------------------------------------------
// NetInterface initialization/destruction
//-----------------------------------------------------------------------------

NetInterface::NetInterface(const Address &bindAddress) : mSocket(bindAddress)
{
   NetClassRep::initialize(); // initialize the net class reps, if they haven't been initialized already.

   mLastTimeoutCheckTime = 0;
   mAllowConnections = true;
   mRequiresKeyExchange = false;

   Random::read(mRandomHashData, sizeof(mRandomHashData));

   mConnectionHashTable.setSize(129);
   for(S32 i = 0; i < mConnectionHashTable.size(); i++)
      mConnectionHashTable[i] = NULL;
   mSendPacketList = NULL;
   mCurrentTime = Platform::getRealMilliseconds();
}

NetInterface::~NetInterface()
{
   // gracefully close all the connections on this NetInterface:
   while(mConnectionList.size())
   {
      NetConnection *c = mConnectionList[0];
      disconnect(c, NetConnection::ReasonSelfDisconnect, "Shutdown");
   }
}

Address NetInterface::getFirstBoundInterfaceAddress()
{
   Address theAddress = mSocket.getBoundAddress();

   if(theAddress.isEqualAddress(Address(IPProtocol, Address::Any, 0)))
   {
      Vector<Address> interfaceAddresses;
      Socket::getInterfaceAddresses(&interfaceAddresses);
      U16 savePort = theAddress.port;
      if(interfaceAddresses.size())
      {
         theAddress = interfaceAddresses[0];
         theAddress.port = savePort;
      }
   }
   return theAddress;
}

void NetInterface::setPrivateKey(AsymmetricKey *theKey)
{
   mPrivateKey = theKey;
}

//-----------------------------------------------------------------------------
// NetInterface packet sending functions
//-----------------------------------------------------------------------------

NetError NetInterface::sendto(const Address &address, BitStream *stream)
{
   return mSocket.sendto(address, stream->getBuffer(), stream->getBytePosition());
}

void NetInterface::sendtoDelayed(const Address &address, BitStream *stream, U32 millisecondDelay)
{
   U32 dataSize = stream->getBytePosition();

   // allocate the send packet, with the data size added on
   DelaySendPacket *thePacket = (DelaySendPacket *) malloc(sizeof(DelaySendPacket) + dataSize);
   thePacket->remoteAddress = address;
   thePacket->sendTime = getCurrentTime() + millisecondDelay;
   thePacket->packetSize = dataSize;
   memcpy(thePacket->packetData, stream->getBuffer(), dataSize);

   // insert it into the DelaySendPacket list, sorted by time
   DelaySendPacket **list;
   for(list = &mSendPacketList; *list && ((*list)->sendTime < thePacket->sendTime); list = &((*list)->nextPacket))
      ;
   thePacket->nextPacket = *list;
   *list = thePacket;
}

//-----------------------------------------------------------------------------
// NetInterface utility functions
//-----------------------------------------------------------------------------

U32 NetInterface::computeClientIdentityToken(const Address &address, const Nonce &theNonce)
{
   hash_state hashState;
   U32 hash[8];

   sha256_init(&hashState);
   sha256_process(&hashState, (const U8 *) &address, sizeof(Address));
   sha256_process(&hashState, theNonce.data, Nonce::NonceSize);
   sha256_process(&hashState, mRandomHashData, sizeof(mRandomHashData));
   sha256_done(&hashState, (U8 *) hash);

   return hash[0];
}

//-----------------------------------------------------------------------------
// NetInterface pending connection list management
//-----------------------------------------------------------------------------

void NetInterface::addPendingConnection(NetConnection *connection)
{
   // make sure we're not already connected to the host at the
   // connection's Address
   findAndRemovePendingConnection(connection->getNetAddress());
   NetConnection *temp = findConnection(connection->getNetAddress());
   if(temp)
      disconnect(temp, NetConnection::ReasonSelfDisconnect, "Reconnecting");

   // hang on to the connection and add it to the pending connection list
   connection->incRef();
   mPendingConnections.push_back(connection);
}

void NetInterface::removePendingConnection(NetConnection *connection)
{
   // search the pending connection list for the specified connection
   // and remove it.
   for(S32 i = 0; i < mPendingConnections.size(); i++)
      if(mPendingConnections[i] == connection)
      {
         connection->decRef();
         mPendingConnections.erase(i);
         return;
      }
}

NetConnection *NetInterface::findPendingConnection(const Address &address)
{
   // Loop through all the pending connections and compare the NetAddresses
   for(S32 i = 0; i < mPendingConnections.size(); i++)
      if(address == mPendingConnections[i]->getNetAddress())
         return mPendingConnections[i];
   return NULL;
}

void NetInterface::findAndRemovePendingConnection(const Address &address)
{
   // Search through the list by Address and remove any connection
   // that matches.
   for(S32 i = 0; i < mPendingConnections.size(); i++)
      if(address == mPendingConnections[i]->getNetAddress())
      {
         mPendingConnections[i]->decRef();
         mPendingConnections.erase(i);
         return;
      }
}

//-----------------------------------------------------------------------------
// NetInterface connection list management
//-----------------------------------------------------------------------------

NetConnection *NetInterface::findConnection(const Address &addr)
{
   // The connection hash table is a single vector, with hash collisions
   // resolved to the next open space in the table.

   // Compute the hash index based on the network address
   U32 hashIndex = addr.hash() % mConnectionHashTable.size();

   // Search through the table for an address that matches the source
   // address.  If the connection pointer is NULL, we've found an
   // empty space and a connection with that address is not in the table
   while(mConnectionHashTable[hashIndex] != NULL)
   {
      if(addr == mConnectionHashTable[hashIndex]->getNetAddress())
         return mConnectionHashTable[hashIndex];
      hashIndex++;
      if(hashIndex >= (U32) mConnectionHashTable.size())
         hashIndex = 0;
   }
   return NULL;
}

void NetInterface::removeConnection(NetConnection *conn)
{
   for(S32 i = 0; i < mConnectionList.size(); i++)
   {
      if(mConnectionList[i] == conn)
      {
         mConnectionList.erase_fast(i);
         break;
      }
   }
   U32 index = conn->getNetAddress().hash() % mConnectionHashTable.size();
   U32 startIndex = index;

   while(mConnectionHashTable[index] != conn)
   {
      index++;
      if(index >= (U32) mConnectionHashTable.size())
         index = 0;
      TNLAssert(index != startIndex, "Attempting to remove a connection that is not in the table.") // not in the table
      if(index == startIndex)
         return;
   }
   mConnectionHashTable[index] = NULL;

   // rehash all subsequent entries until we find a NULL entry:
   for(;;)
   {
      index++;
      if(index >= (U32) mConnectionHashTable.size())
         index = 0;
      if(!mConnectionHashTable[index])
         break;
      NetConnection *rehashConn = mConnectionHashTable[index];
      mConnectionHashTable[index] = NULL;
      U32 realIndex = rehashConn->getNetAddress().hash() % mConnectionHashTable.size();
      while(mConnectionHashTable[realIndex] != NULL)
      {
         realIndex++;
         if(realIndex >= (U32) mConnectionHashTable.size())
            realIndex = 0;
      }
      mConnectionHashTable[realIndex] = rehashConn;
   }
   conn->decRef();
}

void NetInterface::addConnection(NetConnection *conn)
{
   conn->incRef();
   mConnectionList.push_back(conn);
   S32 numConnections = mConnectionList.size();
   if(numConnections > mConnectionHashTable.size() / 2)
   {
      mConnectionHashTable.setSize(numConnections * 4 - 1);
      for(S32 i = 0; i < mConnectionHashTable.size(); i++)
         mConnectionHashTable[i] = NULL;
      for(S32 i = 0; i < numConnections; i++)
      {
         U32 index = mConnectionList[i]->getNetAddress().hash() % mConnectionHashTable.size();
         while(mConnectionHashTable[index] != NULL)
         {
            index++;
            if(index >= (U32) mConnectionHashTable.size())
               index = 0;
         }
         mConnectionHashTable[index] = mConnectionList[i];
      }
   }
   else
   {
      U32 index = mConnectionList[numConnections - 1]->getNetAddress().hash() % mConnectionHashTable.size();
      while(mConnectionHashTable[index] != NULL)
      {
         index++;
         if(index >= (U32) mConnectionHashTable.size())
            index = 0;
      }
      mConnectionHashTable[index] = mConnectionList[numConnections - 1];
   }
}

//-----------------------------------------------------------------------------
// NetInterface timeout and packet send processing
//-----------------------------------------------------------------------------

void NetInterface::processConnections()
{
   mCurrentTime = Platform::getRealMilliseconds();
   mPuzzleManager.tick(mCurrentTime);

   // first see if there are any delayed packets that need to be sent...
   while(mSendPacketList && mSendPacketList->sendTime < getCurrentTime())
   {
      DelaySendPacket *next = mSendPacketList->nextPacket;
      mSocket.sendto(mSendPacketList->remoteAddress,
            mSendPacketList->packetData, mSendPacketList->packetSize);
      free(mSendPacketList);
      mSendPacketList = next;
   }

   NetObject::collapseDirtyList(); // collapse all the mask bits...
   for(S32 i = 0; i < mConnectionList.size(); i++)
      mConnectionList[i]->checkPacketSend(false, getCurrentTime());

   if(getCurrentTime() > mLastTimeoutCheckTime + TimeoutCheckInterval)
   {
      for(S32 i = 0; i < mPendingConnections.size();)
      {
         NetConnection *pending = mPendingConnections[i];

         if(pending->getConnectionState() == NetConnection::AwaitingChallengeResponse &&
            getCurrentTime() > pending->mConnectLastSendTime + ChallengeRetryTime)
         {
            if(pending->mConnectSendCount > ChallengeRetryCount)
            {
               pending->setConnectionState(NetConnection::ConnectTimedOut);
               pending->onConnectTerminated(NetConnection::ReasonTimedOut, "Timeout");
               removePendingConnection(pending);
               continue;
            }
            else
               sendConnectChallengeRequest(pending);
         }
         else if(pending->getConnectionState() == NetConnection::AwaitingConnectResponse &&
            getCurrentTime() > pending->mConnectLastSendTime + ConnectRetryTime)
         {
            if(pending->mConnectSendCount > ConnectRetryCount)
            {
               pending->setConnectionState(NetConnection::ConnectTimedOut);
               pending->onConnectTerminated(NetConnection::ReasonTimedOut, "Timeout");
               removePendingConnection(pending);
               continue;
            }
            else
            {
               if(pending->getConnectionParameters().mIsArranged)
                  sendArrangedConnectRequest(pending);
               else
                  sendConnectRequest(pending);
            }
         }
         else if(pending->getConnectionState() == NetConnection::SendingPunchPackets &&
            getCurrentTime() > pending->mConnectLastSendTime + PunchRetryTime)
         {
            if(pending->mConnectSendCount > PunchRetryCount)
            {
               pending->setConnectionState(NetConnection::ConnectTimedOut);
               pending->onConnectTerminated(NetConnection::ReasonTimedOut, "Timeout");
               removePendingConnection(pending);
               continue;
            }
            else
               sendPunchPackets(pending);
         }
         else if(pending->getConnectionState() == NetConnection::ComputingPuzzleSolution &&
            getCurrentTime() > pending->mConnectLastSendTime + PuzzleSolutionTimeout)
         {
            pending->setConnectionState(NetConnection::ConnectTimedOut);
            pending->onConnectTerminated(NetConnection::ReasonTimedOut, "Timeout");
            removePendingConnection(pending);
         }
         i++;
      }
      mLastTimeoutCheckTime = getCurrentTime();

      for(S32 i = 0; i < mConnectionList.size();)
      {
         if(mConnectionList[i]->checkTimeout(getCurrentTime()))
         {
            mConnectionList[i]->setConnectionState(NetConnection::TimedOut);
            mConnectionList[i]->onConnectionTerminated(NetConnection::ReasonTimedOut, "Timeout");
            removeConnection(mConnectionList[i]);
         }
         else
            i++;
      }
   }

   // check if we're trying to solve any client connection puzzles
   for(S32 i = 0; i < mPendingConnections.size(); i++)
   {
      if(mPendingConnections[i]->getConnectionState() == NetConnection::ComputingPuzzleSolution)
      {
         continuePuzzleSolution(mPendingConnections[i]);
         break;
      }
   }
}

//-----------------------------------------------------------------------------
// NetInterface incoming packet dispatch
//-----------------------------------------------------------------------------

void NetInterface::checkIncomingPackets()
{
   PacketStream stream;
   NetError error;
   Address sourceAddress;

   mCurrentTime = Platform::getRealMilliseconds();

   // read out all the available packets:
   while((error = stream.recvfrom(mSocket, &sourceAddress)) == NoError)
      processPacket(sourceAddress, &stream);
}

void NetInterface::processPacket(const Address &sourceAddress, BitStream *pStream)
{

   // Determine what to do with this packet:

   if(pStream->getBuffer()[0] & 0x80) // it's a protocol packet...
   {
      // if the LSB of the first byte is set, it's a game data packet
      // so pass it to the appropriate connection.

      // lookup the connection in the addressTable
      // if this packet causes a disconnection, keep the conn around until this function exits
      RefPtr<NetConnection> conn = findConnection(sourceAddress);
      if(conn)
         conn->readRawPacket(pStream);
   }
   else
   {
      // Otherwise, it's either a game info packet or a
      // connection handshake packet.

      U8 packetType;
      pStream->read(&packetType);

      if(packetType >= FirstValidInfoPacketId)
         handleInfoPacket(sourceAddress, packetType, pStream);
      else
      {
         // check if there's a connection already:
         switch(packetType)
         {
            case ConnectChallengeRequest:
               handleConnectChallengeRequest(sourceAddress, pStream);
               break;
            case ConnectChallengeResponse:
               handleConnectChallengeResponse(sourceAddress, pStream);
               break;
            case ConnectRequest:
               handleConnectRequest(sourceAddress, pStream);
               break;
            case ConnectReject:
               handleConnectReject(sourceAddress, pStream);
               break;
            case ConnectAccept:
               handleConnectAccept(sourceAddress, pStream);
               break;
            case Disconnect:
               handleDisconnect(sourceAddress, pStream);
               break;
            case Punch:
               handlePunch(sourceAddress, pStream);
               break;
            case ArrangedConnectRequest:
               handleArrangedConnectRequest(sourceAddress, pStream);
               break;
         }
      }
   }
}

void NetInterface::handleInfoPacket(const Address &address, U8 packetType, BitStream *stream)
{
}

//-----------------------------------------------------------------------------
// NetInterface connection handshake initiaton and processing
//-----------------------------------------------------------------------------

void NetInterface::startConnection(NetConnection *conn)
{
   TNLAssert(conn->getConnectionState() == NetConnection::NotConnected,
         "Cannot start unless it is in the NotConnected state.");

   addPendingConnection(conn);
   conn->mConnectSendCount = 0;
   conn->setConnectionState(NetConnection::AwaitingChallengeResponse);
   sendConnectChallengeRequest(conn);
}

void NetInterface::sendConnectChallengeRequest(NetConnection *conn)
{
   TNLLogMessageV(LogNetInterface, ("Sending Connect Challenge Request to %s", conn->getNetAddress().toString()));
   PacketStream out;
   out.write(U8(ConnectChallengeRequest));
   ConnectionParameters &params = conn->getConnectionParameters();
   params.mNonce.write(&out);
   out.writeFlag(params.mRequestKeyExchange);
   out.writeFlag(params.mRequestCertificate);
 
   conn->mConnectSendCount++;
   conn->mConnectLastSendTime = getCurrentTime();
   out.sendto(mSocket, conn->getNetAddress());
}

void NetInterface::handleConnectChallengeRequest(const Address &addr, BitStream *stream)
{
   TNLLogMessageV(LogNetInterface, ("Received Connect Challenge Request from %s", addr.toString()));

   if(!mAllowConnections)
      return;
   Nonce clientNonce;
   clientNonce.read(stream);
   bool wantsKeyExchange = stream->readFlag();
   bool wantsCertificate = stream->readFlag();

   sendConnectChallengeResponse(addr, clientNonce, wantsKeyExchange, wantsCertificate);
}

void NetInterface::sendConnectChallengeResponse(const Address &addr, Nonce &clientNonce, bool wantsKeyExchange, bool wantsCertificate)
{
   PacketStream out;
   out.write(U8(ConnectChallengeResponse));
   clientNonce.write(&out);
   
   U32 identityToken = computeClientIdentityToken(addr, clientNonce);
   out.write(identityToken);

   // write out a client puzzle
   Nonce serverNonce = mPuzzleManager.getCurrentNonce();
   U32 difficulty = mPuzzleManager.getCurrentDifficulty();
   serverNonce.write(&out);
   out.write(difficulty);

   if(out.writeFlag(mRequiresKeyExchange || (wantsKeyExchange && !mPrivateKey.isNull())))
   {
      if(out.writeFlag(wantsCertificate && !mCertificate.isNull()))
         out.write(mCertificate);
      else
         out.write(mPrivateKey->getPublicKey());
   }
   TNLLogMessageV(LogNetInterface, ("Sending Challenge Response: %8x", identityToken));

   out.sendto(mSocket, addr);
}

//-----------------------------------------------------------------------------

void NetInterface::handleConnectChallengeResponse(const Address &address, BitStream *stream)
{
   NetConnection *conn = findPendingConnection(address);
   if(!conn || conn->getConnectionState() != NetConnection::AwaitingChallengeResponse)
      return;
   
   Nonce theNonce;
   theNonce.read(stream);

   ConnectionParameters &theParams = conn->getConnectionParameters();
   if(theNonce != theParams.mNonce)
      return;

   stream->read(&theParams.mClientIdentity);

   // see if the server wants us to solve a client puzzle
   theParams.mServerNonce.read(stream);
   stream->read(&theParams.mPuzzleDifficulty);

   if(theParams.mPuzzleDifficulty > ClientPuzzleManager::MaxPuzzleDifficulty)
      return;

   // see if the connection needs to be authenticated or uses key exchange
   if(stream->readFlag())
   {
      if(stream->readFlag())
      {
         theParams.mCertificate = new Certificate(stream);
         if(!theParams.mCertificate->isValid() || !conn->validateCertficate(theParams.mCertificate, true))
            return;         
         theParams.mPublicKey = theParams.mCertificate->getPublicKey();
      }
      else
      {
         theParams.mPublicKey = new AsymmetricKey(stream);
         if(!theParams.mPublicKey->isValid() || !conn->validatePublicKey(theParams.mPublicKey, true))
            return;
      }
      if(mPrivateKey.isNull() || mPrivateKey->getKeySize() != theParams.mPublicKey->getKeySize())
      {
         // we don't have a private key, so generate one for this connection
         theParams.mPrivateKey = new AsymmetricKey(theParams.mPublicKey->getKeySize());
      }
      else
         theParams.mPrivateKey = mPrivateKey;
      theParams.mSharedSecret = theParams.mPrivateKey->computeSharedSecretKey(theParams.mPublicKey);
      //logprintf("shared secret (client) %s", theParams.mSharedSecret->encodeBase64()->getBuffer());
      Random::read(theParams.mSymmetricKey, SymmetricCipher::KeySize);
      theParams.mUsingCrypto = true;
   }

   TNLLogMessageV(LogNetInterface, ("Received Challenge Response: %8x", theParams.mClientIdentity ));

   conn->setConnectionState(NetConnection::ComputingPuzzleSolution);
   conn->mConnectSendCount = 0;

   theParams.mPuzzleSolution = 0;
   conn->mConnectLastSendTime = getCurrentTime();
   continuePuzzleSolution(conn);   
}

void NetInterface::continuePuzzleSolution(NetConnection *conn)
{
   ConnectionParameters &theParams = conn->getConnectionParameters();
   bool solved = ClientPuzzleManager::solvePuzzle(&theParams.mPuzzleSolution, theParams.mNonce, theParams.mServerNonce, theParams.mPuzzleDifficulty, theParams.mClientIdentity);

   if(solved)
   {
      logprintf("Client puzzle solved in %d ms.", Platform::getRealMilliseconds() - conn->mConnectLastSendTime);
      conn->setConnectionState(NetConnection::AwaitingConnectResponse);
      sendConnectRequest(conn);
   }
}

//-----------------------------------------------------------------------------
// NetInterface connect request
//-----------------------------------------------------------------------------

void NetInterface::sendConnectRequest(NetConnection *conn)
{
   TNLLogMessageV(LogNetInterface, ("Sending Connect Request"));
   PacketStream out;
   ConnectionParameters &theParams = conn->getConnectionParameters();

   out.write(U8(ConnectRequest));
   theParams.mNonce.write(&out);
   theParams.mServerNonce.write(&out);
   out.write(theParams.mClientIdentity);
   out.write(theParams.mPuzzleDifficulty);
   out.write(theParams.mPuzzleSolution);

   U32 encryptPos = 0;

   if(out.writeFlag(theParams.mUsingCrypto))
   {
      out.write(theParams.mPrivateKey->getPublicKey());
      encryptPos = out.getBytePosition();
      out.setBytePosition(encryptPos);
      out.write(SymmetricCipher::KeySize, theParams.mSymmetricKey);
   }
   out.writeFlag(theParams.mDebugObjectSizes);
   out.write(conn->getInitialSendSequence());
   out.writeString(conn->getClassName());
   conn->writeConnectRequest(&out);

   if(encryptPos)
   {
      // if we're using crypto on this connection,
      // then write a hash of everything we wrote into the packet
      // key.  Then we'll symmetrically encrypt the packet from
      // the end of the public key to the end of the signature.

      SymmetricCipher theCipher(theParams.mSharedSecret);
      out.hashAndEncrypt(NetConnection::MessageSignatureBytes, encryptPos, &theCipher);      
   }

   conn->mConnectSendCount++;
   conn->mConnectLastSendTime = getCurrentTime();

   out.sendto(mSocket, conn->getNetAddress());
}

void NetInterface::handleConnectRequest(const Address &address, BitStream *stream)
{
   if(!mAllowConnections)
      return;

   ConnectionParameters theParams;
   theParams.mNonce.read(stream);
   theParams.mServerNonce.read(stream);
   stream->read(&theParams.mClientIdentity);

   if(theParams.mClientIdentity != computeClientIdentityToken(address, theParams.mNonce))
      return;

   stream->read(&theParams.mPuzzleDifficulty);
   stream->read(&theParams.mPuzzleSolution);

   // see if the connection is in the main connection table.
   // If the connection is in the connection table and it has
   // the same initiatorSequence, we'll just resend the connect
   // acceptance packet, assuming that the last time we sent it
   // it was dropped.
   NetConnection *connect = findConnection(address);
   if(connect)
   {
      ConnectionParameters &cp = connect->getConnectionParameters();
      if(cp.mNonce == theParams.mNonce && cp.mServerNonce == theParams.mServerNonce)
      {
         sendConnectAccept(connect);
         return;
      }
   }

   // check the puzzle solution
   ClientPuzzleManager::ErrorCode result = mPuzzleManager.checkSolution(
      theParams.mPuzzleSolution, theParams.mNonce, theParams.mServerNonce,
      theParams.mPuzzleDifficulty, theParams.mClientIdentity);

   if(result != ClientPuzzleManager::Success)
   {
      sendConnectReject(&theParams, address, "Puzzle");
      return;
   }

   if(stream->readFlag())
   {
      if(mPrivateKey.isNull())
         return;

      theParams.mUsingCrypto = true;
      theParams.mPublicKey = new AsymmetricKey(stream);
      theParams.mPrivateKey = mPrivateKey;

      U32 decryptPos = stream->getBytePosition();

      stream->setBytePosition(decryptPos);
      theParams.mSharedSecret = theParams.mPrivateKey->computeSharedSecretKey(theParams.mPublicKey);
      //logprintf("shared secret (server) %s", theParams.mSharedSecret->encodeBase64()->getBuffer());

      SymmetricCipher theCipher(theParams.mSharedSecret);

      if(!stream->decryptAndCheckHash(NetConnection::MessageSignatureBytes, decryptPos, &theCipher))
         return;

      // now read the first part of the connection's symmetric key
      stream->read(SymmetricCipher::KeySize, theParams.mSymmetricKey);
      Random::read(theParams.mInitVector, SymmetricCipher::KeySize);
   }

   U32 connectSequence;
   theParams.mDebugObjectSizes = stream->readFlag();
   stream->read(&connectSequence);
   TNLLogMessageV(LogNetInterface, ("Received Connect Request %8x", theParams.mClientIdentity));

   if(connect)
      disconnect(connect, NetConnection::ReasonSelfDisconnect, "NewConnection");

   char connectionClass[256];
   stream->readString(connectionClass);

   NetConnection *conn = NetConnectionRep::create(connectionClass);

   if(!conn)
      return;

   RefPtr<NetConnection> theConnection = conn;
   conn->getConnectionParameters() = theParams;

   conn->setNetAddress(address);
   conn->setInitialRecvSequence(connectSequence);
   conn->setInterface(this);

   if(theParams.mUsingCrypto)
      conn->setSymmetricCipher(new SymmetricCipher(theParams.mSymmetricKey, theParams.mInitVector));

   const char *errorString = NULL;
   if(!conn->readConnectRequest(stream, &errorString))
   {
      sendConnectReject(&theParams, address, errorString);
      return;
   }
   addConnection(conn);
   conn->setConnectionState(NetConnection::Connected);
   conn->onConnectionEstablished();
   sendConnectAccept(conn);
}

//-----------------------------------------------------------------------------
// NetInterface connection acceptance and handling
//-----------------------------------------------------------------------------

void NetInterface::sendConnectAccept(NetConnection *conn)
{
   TNLLogMessageV(LogNetInterface, ("Sending Connect Accept - connection established."));

   PacketStream out;
   out.write(U8(ConnectAccept));
   ConnectionParameters &theParams = conn->getConnectionParameters();

   theParams.mNonce.write(&out);
   theParams.mServerNonce.write(&out);
   U32 encryptPos = out.getBytePosition();
   out.setBytePosition(encryptPos);

   out.write(conn->getInitialSendSequence());
   conn->writeConnectAccept(&out);

   if(theParams.mUsingCrypto)
   {
      out.write(SymmetricCipher::KeySize, theParams.mInitVector);
      SymmetricCipher theCipher(theParams.mSharedSecret);
      out.hashAndEncrypt(NetConnection::MessageSignatureBytes, encryptPos, &theCipher);
   }
   out.sendto(mSocket, conn->getNetAddress());
}

void NetInterface::handleConnectAccept(const Address &address, BitStream *stream)
{
   Nonce nonce, serverNonce;

   nonce.read(stream);
   serverNonce.read(stream);
   U32 decryptPos = stream->getBytePosition();
   stream->setBytePosition(decryptPos);

   NetConnection *conn = findPendingConnection(address);
   if(!conn || conn->getConnectionState() != NetConnection::AwaitingConnectResponse)
      return;

   ConnectionParameters &theParams = conn->getConnectionParameters();

   if(theParams.mNonce != nonce || theParams.mServerNonce != serverNonce)
      return;

   if(theParams.mUsingCrypto)
   {
      SymmetricCipher theCipher(theParams.mSharedSecret);
      if(!stream->decryptAndCheckHash(NetConnection::MessageSignatureBytes, decryptPos, &theCipher))
         return;
   }
   U32 recvSequence;
   stream->read(&recvSequence);
   conn->setInitialRecvSequence(recvSequence);

   const char *errorString = NULL;
   if(!conn->readConnectAccept(stream, &errorString))
   {
      removePendingConnection(conn);
      return;
   }
   if(theParams.mUsingCrypto)
   {
      stream->read(SymmetricCipher::KeySize, theParams.mInitVector);
      conn->setSymmetricCipher(new SymmetricCipher(theParams.mSymmetricKey, theParams.mInitVector));
   }

   addConnection(conn); // first, add it as a regular connection
   removePendingConnection(conn); // remove from the pending connection list

   conn->setConnectionState(NetConnection::Connected);
   conn->onConnectionEstablished(); // notify the connection that it has been established
   TNLLogMessageV(LogNetInterface, ("Received Connect Accept - connection established."));
}

//-----------------------------------------------------------------------------
// NetInterface connection rejection and handling
//-----------------------------------------------------------------------------

void NetInterface::sendConnectReject(ConnectionParameters *conn, const Address &theAddress, const char *reason)
{
   if(!reason)
      return; // if the stream is NULL, we reject silently

   PacketStream out;
   out.write(U8(ConnectReject));
   conn->mNonce.write(&out);
   conn->mServerNonce.write(&out);
   out.writeString(reason);
   out.sendto(mSocket, theAddress);
}

void NetInterface::handleConnectReject(const Address &address, BitStream *stream)
{
   Nonce nonce;
   Nonce serverNonce;

   nonce.read(stream);
   serverNonce.read(stream);

   NetConnection *conn = findPendingConnection(address);
   if(!conn || (conn->getConnectionState() != NetConnection::AwaitingChallengeResponse &&
                conn->getConnectionState() != NetConnection::AwaitingConnectResponse))
      return;
   ConnectionParameters &p = conn->getConnectionParameters();
   if(p.mNonce != nonce || p.mServerNonce != serverNonce)
      return;

   char reason[256];
   stream->readString(reason);

   TNLLogMessageV(LogNetInterface, ("Received Connect Reject - reason %s", reason));
   // if the reason is a bad puzzle solution, try once more with a
   // new nonce.
   if(!strcmp(reason, "Puzzle") && !p.mPuzzleRetried)
   {
      p.mPuzzleRetried = true;
      conn->setConnectionState(NetConnection::AwaitingChallengeResponse);
      conn->mConnectSendCount = 0;
      p.mNonce.getRandom();
      sendConnectChallengeRequest(conn);
      return;
   }

   conn->setConnectionState(NetConnection::ConnectRejected);
   conn->onConnectTerminated(NetConnection::ReasonRemoteHostRejectedConnection, reason);
   removePendingConnection(conn);
}

//-----------------------------------------------------------------------------
// NetInterface arranged connection process
//-----------------------------------------------------------------------------

void NetInterface::startArrangedConnection(NetConnection *conn)
{

   conn->setConnectionState(NetConnection::SendingPunchPackets);
   addPendingConnection(conn);
   conn->mConnectSendCount = 0;
   conn->mConnectLastSendTime = getCurrentTime();
   sendPunchPackets(conn);
}

void NetInterface::sendPunchPackets(NetConnection *conn)
{
   ConnectionParameters &theParams = conn->getConnectionParameters();
   PacketStream out;
   out.write(U8(Punch));

   if(theParams.mIsInitiator)
      theParams.mNonce.write(&out);
   else
      theParams.mServerNonce.write(&out);

   U32 encryptPos = out.getBytePosition();
   out.setBytePosition(encryptPos);

   if(theParams.mIsInitiator)
      theParams.mServerNonce.write(&out);
   else
   {
      theParams.mNonce.write(&out);
      if(out.writeFlag(mRequiresKeyExchange || (theParams.mRequestKeyExchange && !mPrivateKey.isNull())))
      {
         if(out.writeFlag(theParams.mRequestCertificate && !mCertificate.isNull()))
            out.write(mCertificate);
         else
            out.write(mPrivateKey->getPublicKey());
      }
   }
   SymmetricCipher theCipher(theParams.mArrangedSecret);
   out.hashAndEncrypt(NetConnection::MessageSignatureBytes, encryptPos, &theCipher);

   for(S32 i = 0; i < theParams.mPossibleAddresses.size(); i++)
   {
      out.sendto(mSocket, theParams.mPossibleAddresses[i]);

      TNLLogMessageV(LogNetInterface, ("Sending punch packet (%s, %s) to %s",
         ByteBuffer(theParams.mNonce.data, Nonce::NonceSize).encodeBase64()->getBuffer(),
         ByteBuffer(theParams.mServerNonce.data, Nonce::NonceSize).encodeBase64()->getBuffer(),
         theParams.mPossibleAddresses[i].toString()));
   }
   conn->mConnectSendCount++;
   conn->mConnectLastSendTime = getCurrentTime();
}

void NetInterface::handlePunch(const Address &theAddress, BitStream *stream)
{
   S32 i, j;
   NetConnection *conn;

   Nonce firstNonce;
   firstNonce.read(stream);

   ByteBuffer b(firstNonce.data, Nonce::NonceSize);

   TNLLogMessageV(LogNetInterface, ("Received punch packet from %s - %s", theAddress.toString(), b.encodeBase64()->getBuffer()));

   for(i = 0; i < mPendingConnections.size(); i++)
   {
      conn = mPendingConnections[i];
      ConnectionParameters &theParams = conn->getConnectionParameters();

      if(conn->getConnectionState() != NetConnection::SendingPunchPackets)
         continue;

      if((theParams.mIsInitiator && firstNonce != theParams.mServerNonce) ||
            (!theParams.mIsInitiator && firstNonce != theParams.mNonce))
         continue;

      // first see if the address is in the possible addresses list:
      
      for(j = 0; j < theParams.mPossibleAddresses.size(); j++)
         if(theAddress == theParams.mPossibleAddresses[j])
            break;

      // if there was an exact match, just exit the loop, or
      // continue on to the next pending if this is not an initiator:
      if(j != theParams.mPossibleAddresses.size())
      {
         if(theParams.mIsInitiator)
            break;
         else
            continue;
      }

      // if there was no exact match, we may have a funny NAT in the
      // middle.  But since a packet got through from the remote host
      // we'll want to send a punch to the address it came from, as long
      // as only the port is not an exact match:
      for(j = 0; j < theParams.mPossibleAddresses.size(); j++)
         if(theAddress.isEqualAddress(theParams.mPossibleAddresses[j]))
            break;

      // if the address wasn't even partially in the list, just exit out
      if(j == theParams.mPossibleAddresses.size())
         continue;

      // otherwise, as long as we don't have too many ping addresses,
      // add this one to the list:
      if(theParams.mPossibleAddresses.size() < 5)
         theParams.mPossibleAddresses.push_back(theAddress);      

      // if this is the initiator of the arranged connection, then
      // process the punch packet from the remote host by issueing a
      // connection request.
      if(theParams.mIsInitiator)
         break;
   }
   if(i == mPendingConnections.size())
      return;

   ConnectionParameters &theParams = conn->getConnectionParameters();
   SymmetricCipher theCipher(theParams.mArrangedSecret);
   if(!stream->decryptAndCheckHash(NetConnection::MessageSignatureBytes, stream->getBytePosition(), &theCipher))
      return;

   Nonce nextNonce;
   nextNonce.read(stream);

   if(nextNonce != theParams.mNonce)
      return;

   // see if the connection needs to be authenticated or uses key exchange
   if(stream->readFlag())
   {
      if(stream->readFlag())
      {
         theParams.mCertificate = new Certificate(stream);
         if(!theParams.mCertificate->isValid() || !conn->validateCertficate(theParams.mCertificate, true))
            return;         
         theParams.mPublicKey = theParams.mCertificate->getPublicKey();
      }
      else
      {
         theParams.mPublicKey = new AsymmetricKey(stream);
         if(!theParams.mPublicKey->isValid() || !conn->validatePublicKey(theParams.mPublicKey, true))
            return;
      }
      if(mPrivateKey.isNull() || mPrivateKey->getKeySize() != theParams.mPublicKey->getKeySize())
      {
         // we don't have a private key, so generate one for this connection
         theParams.mPrivateKey = new AsymmetricKey(theParams.mPublicKey->getKeySize());
      }
      else
         theParams.mPrivateKey = mPrivateKey;
      theParams.mSharedSecret = theParams.mPrivateKey->computeSharedSecretKey(theParams.mPublicKey);
      //logprintf("shared secret (client) %s", theParams.mSharedSecret->encodeBase64()->getBuffer());
      Random::read(theParams.mSymmetricKey, SymmetricCipher::KeySize);
      theParams.mUsingCrypto = true;
   }
   conn->setNetAddress(theAddress);
   TNLLogMessageV(LogNetInterface, ("Punch from %s matched nonces - connecting...", theAddress.toString()));

   conn->setConnectionState(NetConnection::AwaitingConnectResponse);
   conn->mConnectSendCount = 0;
   conn->mConnectLastSendTime = getCurrentTime();

   sendArrangedConnectRequest(conn);
}

void NetInterface::sendArrangedConnectRequest(NetConnection *conn)
{
   TNLLogMessageV(LogNetInterface, ("Sending Arranged Connect Request"));
   PacketStream out;

   ConnectionParameters &theParams = conn->getConnectionParameters();

   out.write(U8(ArrangedConnectRequest));
   theParams.mNonce.write(&out);
   U32 encryptPos = out.getBytePosition();
   U32 innerEncryptPos = 0;

   out.setBytePosition(encryptPos);

   theParams.mServerNonce.write(&out);
   if(out.writeFlag(theParams.mUsingCrypto))
   {
      out.write(theParams.mPrivateKey->getPublicKey());
      innerEncryptPos = out.getBytePosition();
      out.setBytePosition(innerEncryptPos);
      out.write(SymmetricCipher::KeySize, theParams.mSymmetricKey);
   }
   out.writeFlag(theParams.mDebugObjectSizes);
   out.write(conn->getInitialSendSequence());
   conn->writeConnectRequest(&out);

   if(innerEncryptPos)
   {
      SymmetricCipher theCipher(theParams.mSharedSecret);
      out.hashAndEncrypt(NetConnection::MessageSignatureBytes, innerEncryptPos, &theCipher);
   }
   SymmetricCipher theCipher(theParams.mArrangedSecret);
   out.hashAndEncrypt(NetConnection::MessageSignatureBytes, encryptPos, &theCipher);

   conn->mConnectSendCount++;
   conn->mConnectLastSendTime = getCurrentTime();

   out.sendto(mSocket, conn->getNetAddress());
}

void NetInterface::handleArrangedConnectRequest(const Address &theAddress, BitStream *stream)
{
   S32 i, j;
   NetConnection *conn;
   Nonce nonce, serverNonce;
   nonce.read(stream);

   // see if the connection is in the main connection table.
   // If the connection is in the connection table and it has
   // the same initiatorSequence, we'll just resend the connect
   // acceptance packet, assuming that the last time we sent it
   // it was dropped.
   NetConnection *oldConnection = findConnection(theAddress);
   if(oldConnection)
   {
      ConnectionParameters &cp = oldConnection->getConnectionParameters();
      if(cp.mNonce == nonce)
      {
         sendConnectAccept(oldConnection);
         return;
      }
   }

   for(i = 0; i < mPendingConnections.size(); i++)
   {
      conn = mPendingConnections[i];
      ConnectionParameters &theParams = conn->getConnectionParameters();

      if(conn->getConnectionState() != NetConnection::SendingPunchPackets || theParams.mIsInitiator)
         continue;

      if(nonce != theParams.mNonce)
         continue;

      for(j = 0; j < theParams.mPossibleAddresses.size(); j++)
         if(theAddress.isEqualAddress(theParams.mPossibleAddresses[j]))
            break;
      if(j != theParams.mPossibleAddresses.size())
         break;
   }
   if(i == mPendingConnections.size())
      return;
   
   ConnectionParameters &theParams = conn->getConnectionParameters();
   SymmetricCipher theCipher(theParams.mArrangedSecret);
   if(!stream->decryptAndCheckHash(NetConnection::MessageSignatureBytes, stream->getBytePosition(), &theCipher))
      return;

   stream->setBytePosition(stream->getBytePosition());

   serverNonce.read(stream);
   if(serverNonce != theParams.mServerNonce)
      return;

   if(stream->readFlag())
   {
      if(mPrivateKey.isNull())
         return;
      theParams.mUsingCrypto = true;
      theParams.mPublicKey = new AsymmetricKey(stream);
      theParams.mPrivateKey = mPrivateKey;

      U32 decryptPos = stream->getBytePosition();
      stream->setBytePosition(decryptPos);
      theParams.mSharedSecret = theParams.mPrivateKey->computeSharedSecretKey(theParams.mPublicKey);
      SymmetricCipher theCipher(theParams.mSharedSecret);
      
      if(!stream->decryptAndCheckHash(NetConnection::MessageSignatureBytes, decryptPos, &theCipher))
         return;

      // now read the first part of the connection's session (symmetric) key
      stream->read(SymmetricCipher::KeySize, theParams.mSymmetricKey);
      Random::read(theParams.mInitVector, SymmetricCipher::KeySize);
   }

   U32 connectSequence;
   theParams.mDebugObjectSizes = stream->readFlag();
   stream->read(&connectSequence);
   TNLLogMessageV(LogNetInterface, ("Received Arranged Connect Request"));

   if(oldConnection)
      disconnect(oldConnection, NetConnection::ReasonSelfDisconnect, "");

   conn->setNetAddress(theAddress);
   conn->setInitialRecvSequence(connectSequence);
   if(theParams.mUsingCrypto)
      conn->setSymmetricCipher(new SymmetricCipher(theParams.mSymmetricKey, theParams.mInitVector));

   const char *errorString = NULL;
   if(!conn->readConnectRequest(stream, &errorString))
   {
      sendConnectReject(&theParams, theAddress, errorString);
      removePendingConnection(conn);
      return;
   }
   addConnection(conn);
   removePendingConnection(conn);
   conn->setConnectionState(NetConnection::Connected);
   conn->onConnectionEstablished();
   sendConnectAccept(conn);
}

//-----------------------------------------------------------------------------
// NetInterface disconnection and handling
//-----------------------------------------------------------------------------

void NetInterface::disconnect(NetConnection *conn, NetConnection::TerminationReason reason, const char *reasonString)
{
   if(conn->getConnectionState() == NetConnection::AwaitingChallengeResponse ||
      conn->getConnectionState() == NetConnection::AwaitingConnectResponse)
   {
      conn->onConnectTerminated(reason, reasonString);
      removePendingConnection(conn);
   }
   else if(conn->getConnectionState() == NetConnection::Connected)
   {
      conn->setConnectionState(NetConnection::Disconnected);
      conn->onConnectionTerminated(reason, reasonString);
      if(conn->isNetworkConnection())
      {
         // send a disconnect packet...
         PacketStream out;
         out.write(U8(Disconnect));
         ConnectionParameters &theParams = conn->getConnectionParameters();
         theParams.mNonce.write(&out);
         theParams.mServerNonce.write(&out);
         U32 encryptPos = out.getBytePosition();
         out.setBytePosition(encryptPos);
         out.writeString(reasonString);

         if(theParams.mUsingCrypto)
         {
            SymmetricCipher theCipher(theParams.mSharedSecret);
            out.hashAndEncrypt(NetConnection::MessageSignatureBytes, encryptPos, &theCipher);
         }
         out.sendto(mSocket, conn->getNetAddress());
      }
      removeConnection(conn);
   }
}

void NetInterface::handleDisconnect(const Address &address, BitStream *stream)
{
   NetConnection *conn = findConnection(address);
   if(!conn)
      return;

   ConnectionParameters &theParams = conn->getConnectionParameters();

   Nonce nonce, serverNonce;
   char reason[256];

   nonce.read(stream);
   serverNonce.read(stream);

   if(nonce != theParams.mNonce || serverNonce != theParams.mServerNonce)
      return;

   U32 decryptPos = stream->getBytePosition();
   stream->setBytePosition(decryptPos);

   if(theParams.mUsingCrypto)
   {
      SymmetricCipher theCipher(theParams.mSharedSecret);
      if(!stream->decryptAndCheckHash(NetConnection::MessageSignatureBytes, decryptPos, &theCipher))
         return;
   }
   stream->readString(reason);

   conn->setConnectionState(NetConnection::Disconnected);
   conn->onConnectionTerminated(NetConnection::ReasonRemoteDisconnectPacket, reason);
   removeConnection(conn);
}

void NetInterface::handleConnectionError(NetConnection *theConnection, const char *errorString)
{
   disconnect(theConnection, NetConnection::ReasonError, errorString);
}

};

// --- END OF FILE: tnl/netInterface.cpp ---
// --- START OF FILE: tnl/netObject.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlNetObject.h"
#include "tnlGhostConnection.h"
#include "tnlNetInterface.h"

namespace TNL {

GhostConnection *NetObject::mRPCSourceConnection = NULL;
GhostConnection *NetObject::mRPCDestConnection = NULL;
bool NetObject::mIsInitialUpdate = false;

NetObject::NetObject()
{
	// netFlags will clear itself to 0
	mNetIndex = U32(-1);
   mFirstObjectRef = NULL;
   mPrevDirtyList = NULL;
   mNextDirtyList = NULL;
   mDirtyMaskBits = 0;
}

NetObject::~NetObject()
{
   while(mFirstObjectRef)
      mFirstObjectRef->connection->detachObject(mFirstObjectRef);

   if(mDirtyMaskBits)
   {
      if(mPrevDirtyList)
         mPrevDirtyList->mNextDirtyList = mNextDirtyList;
      else
         mDirtyList = mNextDirtyList;
      if(mNextDirtyList)
         mNextDirtyList->mPrevDirtyList = mPrevDirtyList;
   }
}

NetObject *NetObject::mDirtyList = NULL;

void NetObject::setMaskBits(U32 orMask)
{
   TNLAssert(orMask != 0, "Invalid net mask bits set.");
   TNLAssert(mDirtyMaskBits == 0 || (mPrevDirtyList != NULL || mNextDirtyList != NULL || mDirtyList == this), "Invalid dirty list state.");
   if(!mDirtyMaskBits)
   {
      TNLAssert(mNextDirtyList == NULL && mPrevDirtyList == NULL, "Object with zero mask already in list.");
      if(mDirtyList)
      {
         mNextDirtyList = mDirtyList;
         mDirtyList->mPrevDirtyList = this;
      }
      mDirtyList = this;
   }
   mDirtyMaskBits |= orMask;
   TNLAssert(mDirtyMaskBits == 0 || (mPrevDirtyList != NULL || mNextDirtyList != NULL || mDirtyList == this), "Invalid dirty list state.");
}

void NetObject::clearMaskBits(U32 orMask)
{
   if(mDirtyMaskBits)
   {
      mDirtyMaskBits &= ~orMask;
      if(!mDirtyMaskBits)
      {
         if(mPrevDirtyList)
            mPrevDirtyList->mNextDirtyList = mNextDirtyList;
         else
            mDirtyList = mNextDirtyList;
         if(mNextDirtyList)
            mNextDirtyList->mPrevDirtyList = mPrevDirtyList;
         mNextDirtyList = mPrevDirtyList = NULL;
      }
   }
   
   for(GhostInfo *walk = mFirstObjectRef; walk; walk = walk->nextObjectRef)
   {
      if(walk->updateMask && walk->updateMask == orMask)
      {
         walk->updateMask = 0;
         walk->connection->ghostPushToZero(walk);
      }
      else
         walk->updateMask &= ~orMask;
   }
}

void NetObject::collapseDirtyList()
{
   Vector<NetObject *> tempV;
   for(NetObject *t = mDirtyList; t; t = t->mNextDirtyList)
      tempV.push_back(t);

   for(NetObject *obj = mDirtyList; obj; )
   {
      NetObject *next = obj->mNextDirtyList;
      U32 orMask = obj->mDirtyMaskBits;

      obj->mNextDirtyList = NULL;
      obj->mPrevDirtyList = NULL;
      obj->mDirtyMaskBits = 0;

      if(orMask)
      {
         for(GhostInfo *walk = obj->mFirstObjectRef; walk; walk = walk->nextObjectRef)
         {
            if(!walk->updateMask)
            {
               walk->updateMask = orMask;
               walk->connection->ghostPushNonZero(walk);
            }
            else
               walk->updateMask |= orMask;
         }
      }
      obj = next;
   }
   mDirtyList = NULL;
   for(S32 i = 0; i < tempV.size(); i++)
   {
      TNLAssert(tempV[i]->mNextDirtyList == NULL && tempV[i]->mPrevDirtyList == NULL && tempV[i]->mDirtyMaskBits == 0, "Error in collapse");
   }
}

bool NetObject::onGhostAdd(GhostConnection *theConnection)
{
   return true;
}


void NetObject::onGhostRemove()
{
}

void NetObject::onGhostAvailable(GhostConnection *)
{
}

//-----------------------------------------------------------------------------

F32 NetObject::getUpdatePriority(NetObject*, U32, S32 updateSkips)
{
   return F32(updateSkips) * 0.1f;

   //return 0;
}

U32 NetObject::packUpdate(GhostConnection*, U32, BitStream*)
{
   return 0;
}

void NetObject::unpackUpdate(GhostConnection*, BitStream*)
{
}

void NetObject::performScopeQuery(GhostConnection *connection)
{
   // default behavior - since we have no idea here about
   // the contents of the world, or why they matter, just scope
   // the scope object.
   connection->objectInScope(this);
}

void NetObject::postRPCEvent(NetObjectRPCEvent *theEvent)
{
   RefPtr<NetObjectRPCEvent> event = theEvent;

   TNLAssert((!isGhost() && theEvent->mRPCDirection == RPCToGhost) ||
               (isGhost() && theEvent->mRPCDirection == RPCToGhostParent),
         "Invalid RPC call - going in the wrong direction!");

   // ok, see what kind of an object this is:
   if(isGhost())
      mOwningConnection->postNetEvent(theEvent);
   else if(NetObject::getRPCDestConnection())
   {
      NetObject::getRPCDestConnection()->postNetEvent(theEvent);
   }
   else
   {
      for(GhostInfo *walk = mFirstObjectRef; walk; walk = walk->nextObjectRef)
      {
         if(!(walk->flags & GhostInfo::NotAvailable))
            walk->connection->postNetEvent(theEvent);
      }
   }
}

void NetObjectRPCEvent::pack(EventConnection *ps, BitStream *bstream)
{
   GhostConnection *gc = static_cast<GhostConnection *>(ps);
   S32 ghostIndex = -1;
   if(!mDestObject.isNull())
      ghostIndex = gc->getGhostIndex(mDestObject);

   if(bstream->writeFlag(ghostIndex != -1))
   {
      bstream->writeInt(ghostIndex, GhostConnection::GhostIdBitSize);
      RPCEvent::pack(ps, bstream);
   }
}

void NetObjectRPCEvent::unpack(EventConnection *ps, BitStream *bstream)
{
   // make sure this is a valid place for this event to be...
   GhostConnection *gc = static_cast<GhostConnection *>(ps);

   if( (gc->doesGhostTo() && mRPCDirection == RPCToGhost) ||
       (gc->doesGhostFrom() && mRPCDirection == RPCToGhostParent) )
   {
      if(bstream->readFlag())
      {
         S32 ghostIndex = bstream->readInt(GhostConnection::GhostIdBitSize);
         RPCEvent::unpack(ps, bstream);

         if(mRPCDirection == RPCToGhost)
            mDestObject = gc->resolveGhost(ghostIndex);
         else
            mDestObject = gc->resolveGhostParent(ghostIndex);
      }
   }
   else
      gc->setLastError("Invalid Packet.");
}

void NetObjectRPCEvent::process(EventConnection *ps)
{
   if(mDestObject.isNull())
      return;

   if(!checkClassType(mDestObject))
      return;

   void *thisPointer = (void *) mDestObject.getPointer();

   NetObject::mRPCSourceConnection = (GhostConnection *) ps;
   mFunctor->dispatch(thisPointer);

   NetObject::mRPCSourceConnection = NULL;
}

};

// --- END OF FILE: tnl/netObject.cpp ---
// --- START OF FILE: tnl/netStringTable.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlNetBase.h"
#include "tnlNetConnection.h"
#include "tnlNetStringTable.h"
#include "tnlDataChunker.h"
#include "tnlNetInterface.h"

namespace TNL {

namespace StringTable
{

/// @name Implementation details
/// @{

/// This is internal to the _StringTable class.
struct Node
{
   StringTableEntryId masterIndex; ///< index of the Node pointer in the master list
   StringTableEntryId nextIndex; ///< next string in this hash bucket.
   U32 hash; ///< stored hash value of this string.
   U16 stringLen; ///< length of string in this node.
   U16 refCount; ///< number of StringTableEntry's that reference this node
   char stringData[1]; ///< String data, with space for the NULL token.  Node structure is allocated as strlen + sizeof(Node);
};

enum {
   InitialHashTableSize = 1237, ///< Initial size of string hash table
   InitialNodeListSize = 2048, ///< Initial size of node id remap table
   CompactThreshold = 32768, ///< Number of string bytes freed before compaction occurs.
};

Node **mNodeList = NULL; ///< Master list of string table entry nodes
StringTableEntryId *mBuckets = NULL; ///< Hash table buckets, organized by string index

U32 mNumBuckets = 0; ///< number of hash buckets in the table
U32 mNodeListSize = 0; ///< number of elements in the node list
StringTableEntryId mNodeListFreeEntry = 0; ///< index of first free entry in the node list

U32 mItemCount = 0; ///< number of strings in the table

DataChunker *mMemPool = NULL; ///< memory pool from which string table data is allocated
U32 mFreeStringDataSize = 0; ///< number of bytes freed by deallocated strings.  When this number exceeds CompactThreshold, the table is compacted.

// a little note about the free list...
// the free list is essentially an index linked list encoded in the node
// list.  The first entry in the list is mNodeListFreeEntry.
// When the string table is compacted, we need to know which elements
// in the node list actually point to nodes, and which ones are members
// of the free list... so, what we do is, free list entries are
// actually coded with the next index shifted left 1 and or'd with a 1
// so we have the lowest bit set.  We know that pointers allocated by the
// data chunker are on at least 4-byte boundaries, so any pointer with
// the low bit set is assumed to be a free list entry.


/// Resize the StringTable to be able to hold newSize items. This 
/// is called automatically by the StringTable when the table is
/// full past a certain threshhold.
///
/// @param newSize   Number of new items to allocate space for.
void resizeHashTable(const U32 newSize);

/// compacts the string data associated with the string table.
void compact();

//---------------------------------------------------------------
//
// StringTable functions
//
//---------------------------------------------------------------

namespace {
bool sgToLowerTableInit = true;
U8   sgToLowerTable[256];

void initToLowerTable()
{
   for (U32 i = 0; i < 256; i++) {
      U8 c = dTolower(i);
      sgToLowerTable[i] = c * c;
   }

   sgToLowerTableInit = false;
}

} // namespace {}

U32 hashString(const char* str)
{
   if (sgToLowerTableInit)
      initToLowerTable();

   U32 ret = 0;
   char c;
   while((c = *str++) != 0) {
      ret <<= 1;
      ret ^= sgToLowerTable[c];
   }
   return ret;
}

U32 hashStringn(const char* str, S32 len)
{
   if (sgToLowerTableInit)
      initToLowerTable();

   U32 ret = 0;
   char c;
   while((c = *str++) != 0 && len--) {
      ret <<= 1;
      ret ^= sgToLowerTable[c];
   }
   return ret;
}

//--------------------------------------
void init()
{
   mMemPool = new DataChunker;
   mBuckets = (StringTableEntryId *) malloc(InitialHashTableSize * sizeof(StringTableEntryId));
   for(U32 i = 0; i < InitialHashTableSize; i++)
      mBuckets[i] = 0;

   mNumBuckets = InitialHashTableSize;
   mItemCount = 1;

   mNodeList = (Node **) malloc(InitialNodeListSize * sizeof(Node *));
   for(U32 i = 1; i < InitialNodeListSize; i++)
      mNodeList[i] = (Node *) (( (i + 1) << 1) | 1); // see the doco in stringTable.h for how free list entries are coded
   
   mNodeList[InitialNodeListSize - 1] = NULL;
   mNodeList[0] = (Node *) mMemPool->alloc(sizeof(Node));
   mNodeList[0]->stringData[0] = 0;
   mNodeList[0]->stringLen = 0;
   mNodeList[0]->refCount = 1;
   mNodeList[0]->masterIndex = mNodeList[0]->nextIndex = 0;
   mNodeListSize = InitialNodeListSize;

   mNodeListFreeEntry = (1 << 1) | 1;
   mFreeStringDataSize = 0;
}

void destroy()
{
   free(mBuckets);
   mBuckets = NULL;
   free(mNodeList);
   mNodeList = NULL;
   delete mMemPool;
}

//--------------------------------------

//--------------------------------------
StringTableEntryId insert(const char* val, const bool caseSens)
{
   if(!val)
      return 0;
   return insertn(val, strlen(val), caseSens);
}

//--------------------------------------
void validate()
{
   // count all the nodes in the node list:
   U32 nodeCount = 0;
   for(U32 i = 0; i < mNodeListSize; i++)
   {
      if(mNodeList[i] && !(U32(mNodeList[i]) & 1))
        nodeCount++;
   }
   TNLAssert(nodeCount == mItemCount, "Error!!!");
   U32 freeListCount = 0;
   StringTableEntryId walk = mNodeListFreeEntry;
   while(walk)
   {
      walk = StringTableEntryId(mNodeList[walk >> 1]);
      if(!((walk >> 1) < mNodeListSize))
         TNLAssert((walk >> 1) < mNodeListSize, "Out of range node index!!!");
      freeListCount++;
   }
   TNLAssert(freeListCount + nodeCount == mNodeListSize, "Error!!!!");
   // walk through all the bucket chains...
   // and make sure there are no free entries...

   for(U32 i = 0; i < mNumBuckets; i++)
   {
      StringTableEntryId walk = mBuckets[i];
      while(walk)
      {
        TNLAssert(walk < mNodeListSize, "Out of range node index!!!");
        Node *node = mNodeList[walk];
        TNLAssert((U32(node) & 1) == 0, "Free list entry in node chain!!!");
        TNLAssert(walk == node->masterIndex, "Master/node index mismatch.");
        walk = node->nextIndex;
      }
   }
}

StringTableEntryId insertn(const char* val, S32 len, const bool caseSens)
{
   if(!val || !*val || len == 0)
      return 0;
   if(!mBuckets)
      init();
   StringTableEntryId *walk;
   Node *stringNode;
   U32 key = hashStringn(val, len);
   walk = &mBuckets[key % mNumBuckets]; // find the bucket that the string would belong in

   // walk all the nodes in the bucket to see if the string is already in the table
   while(*walk)
   {
      stringNode = mNodeList[*walk];
      if((caseSens && !strncmp(stringNode->stringData, val, len) && stringNode->stringData[len] == 0) ||
         (!caseSens && !strnicmp(stringNode->stringData, val, len) && stringNode->stringData[len] == 0) )
      {
         // the string was found, so bump the reference count and return the node id
         stringNode->refCount++;
         return *walk;
      }
      // step to the next node in the hash bucket.
      walk = &(stringNode->nextIndex);
   }
   
   // the string was not found in the table.  So allocate a new node for the string

   // first, make sure there is a free node pointer:
   if(!mNodeListFreeEntry)
   {
      U32 oldNodeListSize = mNodeListSize;
      mNodeListSize += InitialNodeListSize;
      mNodeList = (Node **) realloc(mNodeList, mNodeListSize * sizeof(Node *));
      for(U32 i = oldNodeListSize; i < mNodeListSize; i++)
         mNodeList[i] = (Node *) (((i + 1) << 1) | 1);
      mNodeList[mNodeListSize - 1] = 0;
      mNodeListFreeEntry = (oldNodeListSize << 1) | 1;
   }
   // now allocate a new string node, and fill it in.
   stringNode = (Node *) mMemPool->alloc(sizeof(Node) + len);
   stringNode->stringLen = len;
   stringNode->refCount = 1;
   stringNode->masterIndex = mNodeListFreeEntry >> 1; // shift off the low bit flag for the free list
   stringNode->nextIndex = 0;
   stringNode->hash = key;
   *walk = stringNode->masterIndex;

   // dequeue the next free entry in the node list
   mNodeListFreeEntry = (StringTableEntryId) mNodeList[mNodeListFreeEntry >> 1];
   TNLAssert(!mNodeListFreeEntry || (mNodeListFreeEntry & 1), "Error in freeList!!");
   mNodeList[stringNode->masterIndex] = stringNode;
   
   strncpy(stringNode->stringData, val, len);
   stringNode->stringData[len] = 0;
   mItemCount++;

   // check for hash table resize
   if(mItemCount > 2 * mNumBuckets) {
      resizeHashTable(4 * mNumBuckets - 1);
   }
   return stringNode->masterIndex;
}

//--------------------------------------
StringTableEntryId lookup(const char* val, const bool  caseSens)
{
   StringTableEntryId *walk;
   Node *stringNode;
   U32 key = hashString(val);
   walk = &mBuckets[key % mNumBuckets];
   while(*walk)
   {
      stringNode = mNodeList[*walk];
      if((caseSens && !strcmp(stringNode->stringData, val)) ||
         (!caseSens && !stricmp(stringNode->stringData, val)) )
         return *walk;
      walk = &(stringNode->nextIndex);
   }
   return 0;
}

//--------------------------------------
StringTableEntryId lookupn(const char* val, S32 len, const bool  caseSens)
{
   StringTableEntryId *walk;
   Node *stringNode;
   U32 key = hashStringn(val, len);
   walk = &mBuckets[key % mNumBuckets];
   while(*walk)
   {
      stringNode = mNodeList[*walk];
      if((caseSens && !strncmp(stringNode->stringData, val, len) && stringNode->stringData[len] == 0) ||
         (!caseSens && !strnicmp(stringNode->stringData, val, len) && stringNode->stringData[len] == 0) )
         return *walk;
      walk = &(stringNode->nextIndex);
   }
   return 0; 
}

//--------------------------------------
void resizeHashTable(const U32 newSize)
{
   StringTableEntryId walk;
   StringTableEntryId head = 0;
   StringTableEntryId temp;

   U32 i;
   // reverse individual bucket lists
   // we do this because new strings are added at the end of bucket
   // lists so that case sens strings are always after their
   // corresponding case insens strings

   for(i = 0; i < mNumBuckets; i++) {
      walk = mBuckets[i];
      while(walk)
      {
         temp = mNodeList[walk]->nextIndex;
         mNodeList[walk]->nextIndex = head;
         head = walk;
         walk = temp;
      }
   }
   mBuckets = (StringTableEntryId *) realloc(mBuckets, newSize * sizeof(StringTableEntryId));
   for(i = 0; i < newSize; i++) {
      mBuckets[i] = 0;
   }
   mNumBuckets = newSize;
   walk = head;
   while(walk)
   {
      U32 key;
      Node *temp = mNodeList[walk];
      
      walk = temp->nextIndex;
      key = temp->hash;
      temp->nextIndex = mBuckets[key % newSize];
      mBuckets[key % newSize] = temp->masterIndex;
   }
}

void compact()
{
   DataChunker *newData = new DataChunker;
   for(U32 i = 1; i < mNodeListSize; i++)
   {
      Node *theNode, *newNode;
      theNode = mNodeList[i];
      
      // if the low bit is set, it's an entry in the free list.
      // if it is NULL it is the last entry (see the constructor)
      // this *may* not be the best fix, but it was a crash -pw
      if(((StringTableEntryId) theNode) & 1 || theNode == NULL)
         continue;
      newNode = (Node *) newData->alloc(sizeof(Node) + theNode->stringLen);
      newNode->stringLen = theNode->stringLen;
      newNode->refCount = theNode->refCount;
      newNode->masterIndex = theNode->masterIndex;
      newNode->nextIndex = theNode->nextIndex;
      newNode->hash = theNode->hash;
      strcpy(newNode->stringData, theNode->stringData);
      mNodeList[i] = newNode; 
   }
   delete mMemPool;
   mMemPool = newData;
   mFreeStringDataSize = 0;
}

void incRef(StringTableEntryId index)
{
   mNodeList[index]->refCount++;
}

void decRef(StringTableEntryId index)
{
   Node *theNode = mNodeList[index];
   if(--theNode->refCount)
      return;

   // remove from the hash table first:
   StringTableEntryId *walk = &mBuckets[theNode->hash % mNumBuckets];
   Node *stringNode;
   while(*walk)
   {
      stringNode = mNodeList[*walk];
      if(stringNode == theNode)
      {
         *walk = theNode->nextIndex;
         break;
      }
      walk = &(stringNode->nextIndex);
   }

   mFreeStringDataSize += mNodeList[index]->stringLen + sizeof(Node);
   mNodeList[index] = (Node *) mNodeListFreeEntry;
   mNodeListFreeEntry = (index << 1) | 1;

   if(mFreeStringDataSize > CompactThreshold)
      compact();
   mItemCount--;
   if(!mItemCount)
      destroy();
}

const char *getString(StringTableEntryId index)
{
   if(!index)
      return "";
   return mNodeList[index]->stringData;
}


};

};

// --- END OF FILE: tnl/netStringTable.cpp ---
// --- START OF FILE: tnl/platform.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlTypes.h"
#include "tnl.h"
#include "tnlJournal.h"

#include <string.h>
#if defined (TNL_OS_XBOX)
#include <xtl.h>

#elif defined (TNL_OS_WIN32)
#include <windows.h>

#include <malloc.h>

#else

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#endif

#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include "tnlRandom.h"
#include "tnlLog.h"

namespace TNL {

#if defined (TNL_OS_XBOX)

void Platform::outputDebugString(const char *string)
{
   OutputDebugString(string);
}

void Platform::debugBreak()
{
   DebugBreak();
}

void Platform::forceQuit()
{
   logprintf("-Force Quit-");
   // Reboot!
   LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
   XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
}

U32 Platform::getRealMilliseconds()
{
   U32 tickCount;
   TNL_JOURNAL_READ_BLOCK ( getRealMilliseconds,
      TNL_JOURNAL_READ( (&tickCount) );
      return tickCount;
   )

   tickCount = GetTickCount();

   TNL_JOURNAL_WRITE_BLOCK ( getRealMilliseconds,
      TNL_JOURNAL_WRITE( (tickCount) );
   )
   return tickCount;
}


//--------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
//   ShowCursor(true);
//   MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
   TNLLogMessageV(LogPlatform, ("AlertOK: %s - %s", message, windowTitle));
   return;
}

//--------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
//   ShowCursor(true);
//   return MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
   TNLLogMessageV(LogPlatform, ("AlertOKCancel: %s - %s", message, windowTitle));
   return false;
}

//--------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
//   ShowCursor(true);
//   return (MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
   TNLLogMessageV(LogPlatform, ("AlertRetry: %s - %s", message, windowTitle));
   return false;
}


class WinTimer
{
   private:
      F64 mPeriod;
      bool mUsingPerfCounter;
   public:
      WinTimer()
      {
         S64 frequency;
         mUsingPerfCounter = QueryPerformanceFrequency((LARGE_INTEGER *) &frequency);
         mPeriod = 1000.0f / F64(frequency);
      }
      S64 getCurrentTime()
      {
         if(mUsingPerfCounter)
         {
            S64 value;
            QueryPerformanceCounter( (LARGE_INTEGER *) &value);
            return value;
         }
         else
         {
            return GetTickCount();
         }
      }
      F64 convertToMS(S64 delta)
      {
         if(mUsingPerfCounter)
            return mPeriod * F64(delta);
         else
            return F64(delta);
      }
};

static WinTimer gTimer;

S64 Platform::getHighPrecisionTimerValue()
{
   return gTimer.getCurrentTime();
}

F64 Platform::getHighPrecisionMilliseconds(S64 timerDelta)
{
   return gTimer.convertToMS(timerDelta);
}

void Platform::sleep(U32 msCount)
{
	// no need to sleep on the xbox...
}

#elif defined (TNL_OS_WIN32)

bool Platform::checkHeap()
{
#ifdef TNL_COMPILER_VISUALC
   return _heapchk() == _HEAPOK;
#else
   return true;
#endif
}

void Platform::outputDebugString(const char *string)
{
   OutputDebugString(string);
}

void Platform::debugBreak()
{
   DebugBreak();
}

void Platform::forceQuit()
{
   ExitProcess(1);
}

U32 Platform::getRealMilliseconds()
{
   U32 tickCount;
   TNL_JOURNAL_READ_BLOCK ( getRealMilliseconds,
      TNL_JOURNAL_READ( (&tickCount) );
      return tickCount;
   )

   tickCount = GetTickCount();

   TNL_JOURNAL_WRITE_BLOCK ( getRealMilliseconds,
      TNL_JOURNAL_WRITE( (tickCount) );
   )
   return tickCount;
}

class WinTimer
{
   private:
      F64 mPeriod;
      bool mUsingPerfCounter;
   public:
      WinTimer()
      {
         S64 frequency;
         mUsingPerfCounter = QueryPerformanceFrequency((LARGE_INTEGER *) &frequency);
         mPeriod = 1000.0f / F64(frequency);
      }
      S64 getCurrentTime()
      {
         if(mUsingPerfCounter)
         {
            S64 value;
            QueryPerformanceCounter( (LARGE_INTEGER *) &value);
            return value;
         }
         else
         {
            return GetTickCount();
         }
      }
      F64 convertToMS(S64 delta)
      {
         if(mUsingPerfCounter)
            return mPeriod * F64(delta);
         else
            return F64(delta);
      }
};

static WinTimer gTimer;

S64 Platform::getHighPrecisionTimerValue()
{
   S64 currentTime;
   TNL_JOURNAL_READ_BLOCK ( getHighPrecisionTimerValue,
      TNL_JOURNAL_READ( (&currentTime) );
      return currentTime;
   )

   currentTime = gTimer.getCurrentTime();

   TNL_JOURNAL_WRITE_BLOCK ( getHighPrecisionTimerValue,
      TNL_JOURNAL_WRITE( (currentTime) );
   )

   return currentTime;
}

F64 Platform::getHighPrecisionMilliseconds(S64 timerDelta)
{
   F64 timerValue;
   TNL_JOURNAL_READ_BLOCK ( getHighPrecisionMilliseconds,
      TNL_JOURNAL_READ( (&timerValue) );
      return timerValue;
   )

   timerValue = gTimer.convertToMS(timerDelta);

   TNL_JOURNAL_WRITE_BLOCK ( getHighPrecisionMilliseconds,
      TNL_JOURNAL_WRITE( (timerValue) );
   )

   return timerValue;
}

void Platform::sleep(U32 msCount)
{
   Sleep(msCount);
}

//--------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
   ShowCursor(true);
   MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
}

//--------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
   ShowCursor(true);
   return MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
}

//--------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
   ShowCursor(true);
   return (MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
}

#else // osx and linux

void Platform::debugBreak()
{
   kill(getpid(), SIGTRAP);
}

void Platform::outputDebugString(const char *string)
{
   //printf("%s", string);
}

void Platform::forceQuit()
{
   debugBreak();
   exit(1);
}


U32 x86UNIXGetTickCount();
//--------------------------------------

U32 Platform::getRealMilliseconds()
{
   return x86UNIXGetTickCount();
}

static bool   sg_initialized = false;
static U32 sg_secsOffset  = 0;

U32 x86UNIXGetTickCount()
{
   // TODO: What happens when crossing a day boundary?
   //
   timeval t;

   if (sg_initialized == false) {
      sg_initialized = true;

      ::gettimeofday(&t, NULL);
      sg_secsOffset = t.tv_sec;
   }

   ::gettimeofday(&t, NULL);

   U32 secs  = t.tv_sec - sg_secsOffset;
   U32 uSecs = t.tv_usec;

   // Make granularity 1 ms
   return (secs * 1000) + (uSecs / 1000);
}

class UnixTimer
{
   public:
      UnixTimer()
      {
      }
      S64 getCurrentTime()
      {
         return x86UNIXGetTickCount();
      }
      F64 convertToMS(S64 delta)
      {
         return F64(delta);
      }
};

static UnixTimer gTimer;

S64 Platform::getHighPrecisionTimerValue()
{
   return gTimer.getCurrentTime();
}

F64 Platform::getHighPrecisionMilliseconds(S64 timerDelta)
{
   return gTimer.convertToMS(timerDelta);
}

void Platform::sleep(U32 msCount)
{
   usleep(msCount * 1000);
}

//--------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
//   ShowCursor(true);
//   MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
   TNLLogMessageV(LogPlatform, ("AlertOK: %s - %s", message, windowTitle));
   return;
}

//--------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
//   ShowCursor(true);
//   return MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
   TNLLogMessageV(LogPlatform, ("AlertOKCancel: %s - %s", message, windowTitle));
   return false;
}

//--------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
//   ShowCursor(true);
//   return (MessageBox(NULL, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
   TNLLogMessageV(LogPlatform, ("AlertRetry: %s - %s", message, windowTitle));
   return false;
}

#endif

/*
char *strdup(const char *src)
{
   char *buffer = (char *) malloc(strlen(src) + 1);
   strcpy(buffer, src);
   return buffer;
}*/
bool atob(const char *str)
{
   return !stricmp(str, "true") || atof(str);
}

S32 dSprintf(char *buffer, U32 bufferSize, const char *format, ...)
{
   va_list args;
   va_start(args, format);
#ifdef TNL_COMPILER_VISUALC
   S32 len = _vsnprintf(buffer, bufferSize, format, args);
#else
   S32 len = vsnprintf(buffer, bufferSize, format, args);
#endif
   return (len);
}


S32 dVsprintf(char *buffer, U32 bufferSize, const char *format, void *arglist)
{
#ifdef TNL_COMPILER_VISUALC
   S32 len = _vsnprintf(buffer, bufferSize, format, (va_list) arglist);
#else
   S32 len = vsnprintf(buffer, bufferSize, format, (char *) arglist);
#endif
   return len;
}

};


#if defined (__GNUC__)

int stricmp(const char *str1, const char *str2)
{
   while(toupper(*str1) == toupper(*str2) && *str1)
   {
      str1++;
      str2++;
   }
   return (toupper(*str1) > toupper(*str2)) ? 1 : ((toupper(*str1) < toupper(*str2)) ? -1 : 0);
}

int strnicmp(const char *str1, const char *str2, unsigned int len)
{
   for(unsigned int i = 0; i < len; i++)
   {
      if(toupper(str1[i]) == toupper(str2[i]))
         continue;
      return (toupper(str1[i]) > toupper(str2[i])) ? 1 : ((toupper(str1[i]) < toupper(str2[i])) ? -1 : 0);
   }
   return 0;
}

#endif



// --- END OF FILE: tnl/platform.cpp ---
// --- START OF FILE: tnl/random.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------


#include <mycrypt.h>
#include "tnl.h"
#include "tnlRandom.h"
#include "tnlJournal.h"

namespace TNL {

namespace Random {

static bool initialized = false;
static prng_state prng;
static U32 entropyAdded = 0;

static void initialize()
{
   initialized = true;
   yarrow_start(&prng);
   yarrow_ready(&prng);
}

void *getState()
{
   if(!initialized)
      initialize();

   return &prng;
}

void addEntropy(const U8 *randomData, U32 dataLen)
{
   if(!initialized)
      initialize();

   yarrow_add_entropy(randomData, dataLen, &prng);
   entropyAdded += dataLen;
   
   if(entropyAdded >= 16)
   {
      yarrow_ready(&prng);
      entropyAdded = 0;
   }
}

void read(U8 *outBuffer, U32 randomLen)
{
   if(!initialized)
      initialize();

   yarrow_read(outBuffer, randomLen, &prng);
}

U32 readI()
{
   U8 randomBuffer[4];
   read(randomBuffer, 4);
   U32 result = (U32(randomBuffer[0]) << 24) | (U32(randomBuffer[1]) << 16) |
                (U32(randomBuffer[2]) << 8 ) | U32(randomBuffer[3]);
   return result;
}

U32 readI(U32 rangeStart, U32 rangeEnd)
{
   TNLAssert(rangeStart <= rangeEnd, "Random::readI - invalid range.");

   return (readI() % (rangeEnd - rangeStart + 1)) + rangeStart;
}

F32 readF()
{
   return F32 ( F64(readI()) / F64(U32_MAX) );
}

bool readB()
{
   U8 randomBuffer;
   read(&randomBuffer, 1);

   return(randomBuffer & 1);
}
}; 

};

// --- END OF FILE: tnl/random.cpp ---
// --- START OF FILE: tnl/rpc.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlBitStream.h"
#include "tnlVector.h"
#include "tnlNetEvent.h"
#include "tnlRPC.h"
#include "tnlEventConnection.h"

namespace TNL {

RPCEvent::RPCEvent(RPCGuaranteeType gType, RPCDirection dir) :
      NetEvent((NetEvent::GuaranteeType) gType, (NetEvent::EventDirection) dir)
{
}

void RPCEvent::pack(EventConnection *ps, BitStream *bstream)
{
   mFunctor->write(*bstream);
}

void RPCEvent::unpack(EventConnection *ps, BitStream *bstream)
{
   mFunctor->read(*bstream);
}

void RPCEvent::process(EventConnection *ps)
{
   if(checkClassType(ps))
      mFunctor->dispatch(ps);
}

};

// --- END OF FILE: tnl/rpc.cpp ---
// --- START OF FILE: tnl/symmetricCipher.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlSymmetricCipher.h"
#include "tnlByteBuffer.h"
#include "mycrypt.h"

namespace TNL {

SymmetricCipher::SymmetricCipher(const U8 symmetricKey[SymmetricCipher::KeySize], const U8 initVector[SymmetricCipher::BlockSize])
{
   rijndael_setup(symmetricKey, KeySize, 0, (symmetric_key *) &mSymmetricKey);
   memcpy(mInitVector, initVector, BlockSize);
   memcpy(mCounter, initVector, BlockSize);
   rijndael_ecb_encrypt((U8 *) mCounter, mPad, (symmetric_key *) &mSymmetricKey);
   mPadLen = 0;
}

SymmetricCipher::SymmetricCipher(const ByteBuffer *theByteBuffer)
{
   if(theByteBuffer->getBufferSize() != KeySize * 2)
   {
      U8 buffer[KeySize];
      memset(buffer, 0, KeySize);
      rijndael_setup(buffer, KeySize, 0, (symmetric_key *) &mSymmetricKey);
      memcpy(mInitVector, buffer, BlockSize);
   }
   else
   {
      rijndael_setup(theByteBuffer->getBuffer(), KeySize, 0, (symmetric_key *) &mSymmetricKey);
      memcpy(mInitVector, theByteBuffer->getBuffer() + KeySize, BlockSize);
   }
   memcpy(mCounter, mInitVector, BlockSize);
   rijndael_ecb_encrypt((U8 *) mCounter, mPad, (symmetric_key *) &mSymmetricKey);
   mPadLen = 0;
}

void SymmetricCipher::setupCounter(U32 counterValue1, U32 counterValue2, U32 counterValue3, U32 counterValue4)
{   
   mCounter[0] = convertHostToLEndian(convertLEndianToHost(mInitVector[0]) + counterValue1);
   mCounter[1] = convertHostToLEndian(convertLEndianToHost(mInitVector[1]) + counterValue2);
   mCounter[2] = convertHostToLEndian(convertLEndianToHost(mInitVector[2]) + counterValue3);
   mCounter[3] = convertHostToLEndian(convertLEndianToHost(mInitVector[3]) + counterValue4);

   rijndael_ecb_encrypt((U8 *) mCounter, mPad, (symmetric_key *) &mSymmetricKey);
   mPadLen = 0;
}

void SymmetricCipher::encrypt(const U8 *plainText, U8 *cipherText, U32 len)
{
   while(len-- > 0)
   {
      if(mPadLen == BlockSize)
      {
         // we've reached the end of the pad, so compute a new pad
         rijndael_ecb_encrypt(mPad, mPad, (symmetric_key *) &mSymmetricKey);
         mPadLen = 0;
      }
      U8 encryptedChar = *plainText++ ^ mPad[mPadLen];
      mPad[mPadLen++] = *cipherText++ = encryptedChar;
   }
}

void SymmetricCipher::decrypt(const U8 *cipherText, U8 *plainText, U32 len)
{
   while(len-- > 0)
   {
      if(mPadLen == BlockSize)
      {
         rijndael_ecb_encrypt(mPad, mPad, (symmetric_key *) &mSymmetricKey);
         mPadLen = 0;
      }
      U8 encryptedChar = *cipherText++;
      *plainText++ = encryptedChar ^ mPad[mPadLen];
      mPad[mPadLen++] = encryptedChar;
   }
}

};

// --- END OF FILE: tnl/symmetricCipher.cpp ---
// --- START OF FILE: tnl/thread.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlThread.h"
#include "tnlLog.h"

namespace TNL
{

#ifdef TNL_OS_WIN32
Semaphore::Semaphore(U32 initialCount, U32 maximumCount)
{
   mSemaphore = CreateSemaphore(NULL, initialCount, maximumCount, NULL);
}

Semaphore::~Semaphore()
{
   CloseHandle(mSemaphore);
}

void Semaphore::wait()
{
   WaitForSingleObject(mSemaphore, INFINITE);
}

void Semaphore::increment(U32 count)
{
   ReleaseSemaphore(mSemaphore, count, NULL);
}

Mutex::Mutex()
{
   InitializeCriticalSection(&mLock);
}

Mutex::~Mutex()
{
   DeleteCriticalSection(&mLock);
}

void Mutex::lock()
{
   EnterCriticalSection(&mLock);
}

void Mutex::unlock()
{
   LeaveCriticalSection(&mLock);
}

bool Mutex::tryLock()
{
   return false;//   return TryEnterCriticalSection(&mLock);
}

ThreadStorage::ThreadStorage()
{
   mTlsIndex = TlsAlloc();
}

ThreadStorage::~ThreadStorage()
{
   TlsFree(mTlsIndex);
}

void *ThreadStorage::get()
{
   return TlsGetValue(mTlsIndex);
}

void ThreadStorage::set(void *value)
{
   TlsSetValue(mTlsIndex, value);
}

DWORD WINAPI ThreadProc( LPVOID lpParameter )
{
   return ((Thread *) lpParameter)->run();
}

U32 Thread::run()
{
   return 0;
}

void Thread::start()
{
   mThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
   mReturnValue = 0;
}

Thread::Thread()
{
}

Thread::~Thread()
{
   CloseHandle(mThread);
}

#else

Semaphore::Semaphore(U32 initialCount, U32 maximumCount)
{
   sem_init(&mSemaphore, 0, initialCount);
}

Semaphore::~Semaphore()
{
   sem_destroy(&mSemaphore);
}

void Semaphore::wait()
{
   sem_wait(&mSemaphore);
}

void Semaphore::increment(U32 count)
{
   for(U32 i = 0; i < count; i++)
      sem_post(&mSemaphore);
}

Mutex::Mutex()
{
   pthread_mutexattr_t attr;
   pthread_mutexattr_init(&attr);
#ifdef TNL_OS_LINUX
   pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#else
   pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif
   pthread_mutex_init(&mMutex, &attr);
   pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex()
{
   pthread_mutex_destroy(&mMutex);
}

void Mutex::lock()
{
   pthread_mutex_lock(&mMutex);
}

void Mutex::unlock()
{
   pthread_mutex_unlock(&mMutex);
}

bool Mutex::tryLock()
{
   return false;//   return TryEnterCriticalSection(&mLock);
}

ThreadStorage::ThreadStorage()
{
   pthread_key_create(&mThreadKey, NULL);
}

ThreadStorage::~ThreadStorage()
{
   pthread_key_delete(mThreadKey);
}

void *ThreadStorage::get()
{
   return pthread_getspecific(mThreadKey);
}

void ThreadStorage::set(void *value)
{
   pthread_setspecific(mThreadKey, value);
}

void *ThreadProc(void *lpParameter)
{
   return (void *) ((Thread *) lpParameter)->run();
}

Thread::Thread()
{
}

void Thread::start()
{
   int val = pthread_create(&mThread, NULL, ThreadProc, this);
   mReturnValue = 0;
}

Thread::~Thread()
{
}

#endif

ThreadQueue::ThreadQueueThread::ThreadQueueThread(ThreadQueue *q)
{
   mThreadQueue = q;
}

U32 ThreadQueue::ThreadQueueThread::run()
{
   mThreadQueue->threadStart();

   mThreadQueue->lock();
   ThreadStorage &sto = mThreadQueue->getStorage();
   sto.set((void *) 0);
   mThreadQueue->unlock();

   for(;;)
      mThreadQueue->dispatchNextCall();
   return 0;
}

ThreadQueue::ThreadQueue(U32 threadCount)
{
   mStorage.set((void *) 1);
   for(U32 i = 0; i < threadCount; i++)
   {
      Thread *theThread = new ThreadQueueThread(this);
      mThreads.push_back(theThread);
      theThread->start();
   }
}

ThreadQueue::~ThreadQueue()
{
}

void ThreadQueue::dispatchNextCall()
{
   mSemaphore.wait();
   lock();
   if(mThreadCalls.size() == 0)
   {
      unlock();
      return;
   }
   Functor *c = mThreadCalls.first();
   mThreadCalls.pop_front();
   unlock();
   c->dispatch(this);
   delete c;
}

void ThreadQueue::postCall(Functor *theCall)
{
   lock();
   if(isMainThread())
   {
      mThreadCalls.push_back(theCall);
      unlock();
      mSemaphore.increment();
   }
   else
   {
      mResponseCalls.push_back(theCall);
      unlock();
   }
}

void ThreadQueue::dispatchResponseCalls()
{
   lock();
   for(S32 i = 0; i < mResponseCalls.size(); i++)
   {
      Functor *c = mResponseCalls[i];
      c->dispatch(this);
      delete c;
   }
   mResponseCalls.clear();
   unlock();
}

};

// --- END OF FILE: tnl/thread.cpp ---
// --- START OF FILE: tnl/tnlMethodDispatch.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlMethodDispatch.h"
#include "tnlNetStringTable.h"
#include "tnlThread.h"

namespace Types
{
void read(TNL::BitStream &s, TNL::StringPtr *val)
{
   char buffer[256];
   s.readString(buffer);
   *val = buffer;
}

void write(TNL::BitStream &s, TNL::StringPtr &val)
{
   s.writeString(val.getString());
}

void read(TNL::BitStream &s, TNL::ByteBufferPtr *val)
{
   TNL::U32 size = s.readInt(ByteBufferSizeBitSize);
   *val = new TNL::ByteBuffer(size);
   s.read(size, (*val)->getBuffer());
}
void write(TNL::BitStream &s, TNL::ByteBufferPtr &val)
{
   s.writeInt(val->getBufferSize(), ByteBufferSizeBitSize);
   s.write(val->getBufferSize(), val->getBuffer());
}
void read(TNL::BitStream &s, TNL::IPAddress *val)
{
   s.read(&val->netNum);
   s.read(&val->port);
}

void write(TNL::BitStream &s, TNL::IPAddress &val)
{
   s.write(val.netNum);
   s.write(val.port);
}


};

namespace TNL
{


};

// --- END OF FILE: tnl/tnlMethodDispatch.cpp ---
// --- START OF FILE: tnl/udp.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnl.h"
#include "tnlJournal.h"

#if defined ( TNL_OS_XBOX )

#include <xtl.h>

typedef int socklen_t;
#define NO_IPX_SUPPORT

#elif defined ( TNL_OS_WIN32 )
#include <winsock.h>
#if defined(TNL_COMPILER_MINGW)
   // mingw does not include support for IPX sockets
   typedef void* SOCKADDR_IPX;
#  define NO_IPX_SUPPORT
#endif

typedef int socklen_t;

#elif defined ( TNL_OS_MAC_OSX )

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

/* for PROTO_IPX */
#include <sys/ioctl.h>   /* ioctl() */
#define NO_IPX_SUPPORT
typedef sockaddr_in SOCKADDR_IN;
typedef sockaddr * PSOCKADDR;
typedef sockaddr SOCKADDR;
typedef in_addr IN_ADDR;

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

#define closesocket close

#elif defined TNL_OS_LINUX


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

/* for PROTO_IPX */
#include <sys/ioctl.h>   /* ioctl() */
#define NO_IPX_SUPPORT
typedef sockaddr_in SOCKADDR_IN;
typedef sockaddr * PSOCKADDR;
typedef sockaddr SOCKADDR;
typedef in_addr IN_ADDR;

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

#define closesocket close

#else

#endif


#include <stdio.h>


#if !defined(NO_IPX_SUPPORT)
#  include <wsipx.h>
#endif

#include "tnlLog.h"

namespace TNL {

static NetError getLastError();
static S32 initCount = 0;

static bool init()
{
   bool success = true;
#if defined ( TNL_OS_WIN32 )
   if(!initCount)
   {
      WSADATA stWSAData;
      success = !WSAStartup(0x0101, &stWSAData);

      logprintf("Winsock initialization %s", success ? "succeeded." : "failed!");
   }
#elif defined( TNL_OS_XBOX )
      XNetStartupParams xnsp;
      memset( &xnsp, 0, sizeof(xnsp) );
      xnsp.cfgSizeOfStruct = sizeof(xnsp);
      xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
      INT iResult = XNetStartup( &xnsp );
      if( iResult != NO_ERROR )
         success = false;


      WSADATA WsaData;
      iResult = WSAStartup( 0x0101, &WsaData );
      if( iResult != NO_ERROR )
         success = false;

      logprintf("Winsock initialization %s", success ? "succeeded." : "failed!");
#endif
   initCount++;
   return success;
}

static void shutdown()
{
   initCount--;
#ifdef TNL_OS_WIN32
   if(!initCount)
   {
      WSACleanup();
   }
#endif
}

static void TNLToSocketAddress(const Address &address, SOCKADDR *sockAddr, socklen_t *addressSize)
{
   if(address.transport == IPProtocol || address.transport == TCPProtocol)
   {
      memset(sockAddr, 0, sizeof(SOCKADDR_IN));
      ((SOCKADDR_IN *) sockAddr)->sin_family = AF_INET;
      ((SOCKADDR_IN *) sockAddr)->sin_port = htons(address.port);
      ((SOCKADDR_IN *) sockAddr)->sin_addr.s_addr = htonl(address.netNum[0]);
      *addressSize = sizeof(SOCKADDR_IN);
   }
#ifndef NO_IPX_SUPPORT
   else if(address.transport == IPXProtocol)
   {
      memset(sockAddr, 0, sizeof(SOCKADDR_IPX));
      ((SOCKADDR_IPX *) sockAddr)->sa_family = AF_IPX;
      ((SOCKADDR_IPX *) sockAddr)->sa_socket = htons(address.port);
      writeU32ToBuffer(address.netNum[0], (U8 *) ((SOCKADDR_IPX *) sockAddr)->sa_netnum);
      writeU32ToBuffer(address.netNum[1], (U8 *) ((SOCKADDR_IPX *) sockAddr)->sa_nodenum);
      writeU16ToBuffer(address.netNum[2], (U8 *) ((SOCKADDR_IPX *) sockAddr)->sa_nodenum+4);
      *addressSize = sizeof(SOCKADDR_IPX);
   }
#endif
}

static void SocketToTNLAddress(const SOCKADDR *sockAddr, Address *address)
{
   if(sockAddr->sa_family == AF_INET)
   {
      address->transport = IPProtocol;
      address->port = htons(((SOCKADDR_IN *) sockAddr)->sin_port);
      U32 addr = htonl(((SOCKADDR_IN *) sockAddr)->sin_addr.s_addr);
      address->netNum[0] = addr;
      address->netNum[1] = 0;
      address->netNum[2] = 0;
      address->netNum[3] = 0;
   }
#ifndef NO_IPX_SUPPORT
   else if(sockAddr->sa_family == AF_IPX)
   {
      address->transport = IPXProtocol;
      address->port = htons(((SOCKADDR_IPX *) sockAddr)->sa_socket);
      address->netNum[0] = readU32FromBuffer((U8 *) ((SOCKADDR_IPX *) sockAddr)->sa_netnum);
      address->netNum[1] = readU32FromBuffer((U8 *) ((SOCKADDR_IPX *) sockAddr)->sa_nodenum);
      address->netNum[2] = readU16FromBuffer((U8 *) ((SOCKADDR_IPX *) sockAddr)->sa_nodenum+4);
      address->netNum[3] = 0;
   }
#endif
}

Socket::Socket(const Address &bindAddress, U32 sendBufferSize, U32 recvBufferSize, bool acceptsBroadcast, bool nonblockingIO)
{
   TNL_JOURNAL_READ_BLOCK(Socket::Socket,
         TNL_JOURNAL_READ( (&mPlatformSocket) );
      return;
   )
   init();
   mPlatformSocket = INVALID_SOCKET;
   mTransportProtocol = bindAddress.transport;

   const char *socketType = "UDP";

   if(bindAddress.transport == IPProtocol)
      mPlatformSocket = socket(AF_INET, SOCK_DGRAM, 0);
   else if(bindAddress.transport == TCPProtocol)
   {
      socketType = "TCP";
      mPlatformSocket = socket(AF_INET, SOCK_STREAM, 0);
   }
#if !defined(NO_IPX_SUPPORT)
   else if(bindAddress.transport == IPXProtocol)
   {
      socketType = "IPX";
      mPlatformSocket = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
   }
#endif
   else
   {
      TNLLogMessageV(LogUDP, ("Attempted to create a socket bound to an invalid transport."));
   }
   if(mPlatformSocket != INVALID_SOCKET)
   {
      S32 error = 0;
      SOCKADDR address;
      socklen_t addressSize = sizeof(address);

      TNLToSocketAddress(bindAddress, &address, &addressSize);
      error = bind(mPlatformSocket, &address, addressSize);

      Address boundAddress;
      addressSize = sizeof(address);

      getsockname(mPlatformSocket, (PSOCKADDR) &address, &addressSize);
      SocketToTNLAddress(&address, &boundAddress);

      TNLLogMessageV(LogUDP, ("%s socket created - bound to address: %s", socketType, boundAddress.toString()));

      // set the send and receive buffer sizes
      error = setsockopt(mPlatformSocket, SOL_SOCKET, SO_RCVBUF, (char *) &recvBufferSize, sizeof(recvBufferSize));
      if(!error)
      {
         TNLLogMessageV(LogUDP, ("%s socket receive buffer size set to %d.", socketType, recvBufferSize));
         error = setsockopt(mPlatformSocket, SOL_SOCKET, SO_SNDBUF, (char *) &sendBufferSize, sizeof(sendBufferSize));
      }
      else
         TNLLogMessageV(LogUDP, ("%s socket error: unable to set the receive buffer size on socket.", socketType));

      if(!error)
      {
         TNLLogMessageV(LogUDP, ("%s socket send buffer size set to %d.", socketType, recvBufferSize));

         if(mTransportProtocol != TCPProtocol)
         {
            // set the broadcast allowed flag
            S32 bc = acceptsBroadcast;
            error = setsockopt(mPlatformSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bc, sizeof(bc));
         }
      }
      else
         TNLLogMessageV(LogUDP, ("%s socket error: unable to set the send buffer size on socket.", socketType));


      // set the nonblocking IO flag
      if(!error)
      {
#if defined ( TNL_OS_WIN32 ) || defined ( TNL_OS_XBOX )
         DWORD notblock = nonblockingIO;
         S32 error = ioctlsocket(mPlatformSocket, FIONBIO, &notblock);
#else
         U32 notblock = nonblockingIO;
         S32 error = ioctl(mPlatformSocket, FIONBIO, &notblock);
#endif
      }
      else
      {
         TNLLogMessageV(LogUDP, ("%s socket error: unable to set broadcast mode on socket.", socketType));
      }

      if(!error)
      {
         TNLLogMessageV(LogUDP, ("%s socket non-blocking IO set.  Socket initialized.", socketType));
      }
      else
      {
         TNLLogMessageV(LogUDP, ("Error prevented successful initialization of %s socket.", socketType));
         closesocket(mPlatformSocket);
         mPlatformSocket = INVALID_SOCKET;
      }
   }
   TNL_JOURNAL_WRITE_BLOCK(Socket::Socket,
      TNL_JOURNAL_WRITE( (mPlatformSocket) );
   )
}

Socket::~Socket()
{
   TNL_JOURNAL_READ_BLOCK(Socket::~Socket,
      return;
   )

   TNL_JOURNAL_WRITE_BLOCK(Socket::~Socket, ;)

   if(mPlatformSocket != INVALID_SOCKET)
      closesocket(mPlatformSocket);
   shutdown();
}

NetError Socket::sendto(const Address &address, const U8 *buffer, S32 bufferSize)
{
   TNL_JOURNAL_READ_BLOCK(Socket::sendto,
      return NoError;
   )

   TNL_JOURNAL_WRITE_BLOCK(Socket::sendto, ;
   )

   if(address.transport != mTransportProtocol)
      return InvalidPacketProtocol;

   SOCKADDR destAddress;
   socklen_t addressSize;

   TNLToSocketAddress(address, &destAddress, &addressSize);
   if(::sendto(mPlatformSocket, (const char*)buffer, bufferSize, 0,
         &destAddress, addressSize) == SOCKET_ERROR)
      return getLastError();
   else
      return NoError;
}

NetError Socket::recvfrom(Address *address, U8 *buffer, S32 bufferSize, S32 *outSize)
{
   TNL_JOURNAL_READ_BLOCK(Socket::recvfrom,
      bool wouldBlock;
      TNL_JOURNAL_READ( (&wouldBlock) );
      if(wouldBlock)
         return WouldBlock;
      
      TNL_JOURNAL_READ( (&address->transport) );
      TNL_JOURNAL_READ( (&address->port) );
      TNL_JOURNAL_READ( (&address->netNum[0]) );
      TNL_JOURNAL_READ( (&address->netNum[1]) );
      TNL_JOURNAL_READ( (&address->netNum[2]) );
      TNL_JOURNAL_READ( (&address->netNum[3]) );
      TNL_JOURNAL_READ( (outSize) );
      TNL_JOURNAL_READ( (*outSize, buffer) );
      return NoError;
   )

   SOCKADDR sa;
   socklen_t addrLen = sizeof(sa);
   S32 bytesRead = SOCKET_ERROR;

   bytesRead = ::recvfrom(mPlatformSocket, (char *) buffer, bufferSize, 0, &sa, &addrLen);
   if(bytesRead == SOCKET_ERROR)
   {
      TNL_JOURNAL_WRITE_BLOCK(Socket::recvfrom,
         TNL_JOURNAL_WRITE ( (true) );
      )
      return WouldBlock;
   }

   SocketToTNLAddress(&sa, address);

   *outSize = bytesRead;

   TNL_JOURNAL_WRITE_BLOCK(Socket::recvfrom,
      TNL_JOURNAL_WRITE( (false) );
      TNL_JOURNAL_WRITE( (address->transport) );
      TNL_JOURNAL_WRITE( (address->port) );
      TNL_JOURNAL_WRITE( (address->netNum[0]) );
      TNL_JOURNAL_WRITE( (address->netNum[1]) );
      TNL_JOURNAL_WRITE( (address->netNum[2]) );
      TNL_JOURNAL_WRITE( (address->netNum[3]) );
      TNL_JOURNAL_WRITE( (*outSize) );
      TNL_JOURNAL_WRITE( (*outSize, buffer) );
   )
   return NoError;
}

NetError Socket::connect(const Address &theAddress)
{
   SOCKADDR destAddress;
   socklen_t addressSize;

   TNLToSocketAddress(theAddress, &destAddress, &addressSize);
   if(::connect(mPlatformSocket, &destAddress, addressSize) == -1)
      return getLastError();
   return NoError;
}

NetError Socket::send(const U8 *buffer, S32 bufferSize)
{
   if(::send(mPlatformSocket, (const char *) buffer, bufferSize, 0) == SOCKET_ERROR)
      return getLastError();
   return NoError;
}

NetError Socket::recv(U8 *buffer, S32 bufferSize, S32 *bytesRead)
{
   *bytesRead = ::recv(mPlatformSocket, (char *) buffer, bufferSize, 0);
   if(*bytesRead == -1)
      return getLastError();
   return NoError;
}

Address Socket::getBoundAddress()
{
   SOCKADDR address;
   Address returnAddress;

   socklen_t addressSize = sizeof(address);
   getsockname(mPlatformSocket, (PSOCKADDR) &address, &addressSize);
   SocketToTNLAddress(&address, &returnAddress);

   return returnAddress;
}

bool Socket::isValid()
{
   return mPlatformSocket != INVALID_SOCKET;
}

#if defined ( TNL_OS_WIN32 )
void Socket::getInterfaceAddresses(Vector<Address> *addressVector)
{
   typedef struct {
      DWORD dwAddr;
      DWORD dwIndex;
      DWORD dwMask;
      DWORD dwBCastAddr;
      DWORD dwReasmSize;
      unsigned short unused1;
      unsigned short unused2;
   } MIB_IPADDRROW;
   typedef struct {
      DWORD dwNumEntries;
      MIB_IPADDRROW table[1];
   } MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;
   typedef DWORD  (WINAPI *GetIPAddrTableFn_t)(PMIB_IPADDRTABLE,PULONG,BOOL);

   static GetIPAddrTableFn_t GetIpAddrTableFn = NULL;
  
   if(!GetIpAddrTableFn)
   {
      HMODULE module = LoadLibrary("iphlpapi.dll");
      if(!module)
         return;

      GetIpAddrTableFn = (GetIPAddrTableFn_t) GetProcAddress(module, "GetIpAddrTable");
   }

   // Under Win32, we use the IP helper library to query all
   // available network interfaces.
   PMIB_IPADDRTABLE pIPAddrTable;
   DWORD dwSize = 0;

   pIPAddrTable = (MIB_IPADDRTABLE*) malloc( sizeof( MIB_IPADDRTABLE) );

   // Make an initial call to GetIpAddrTable to get the
   // necessary size into the dwSize variable
   if (GetIpAddrTableFn(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
   {
      free( pIPAddrTable );
      pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwSize );
   }

   // Make a second call to GetIpAddrTable to get the
   // actual data we want
   DWORD dwRetVal;
   if ( (dwRetVal = GetIpAddrTableFn( pIPAddrTable, &dwSize, 0 )) == NO_ERROR ) { 
      for(U32 i = 0; i < pIPAddrTable->dwNumEntries; i++)
      {
         // construct an Address for this interface
         Address a;
         a.transport = IPProtocol;
         a.port = 0;
         a.netNum[0] = htonl(pIPAddrTable->table[i].dwAddr);
         a.netNum[1] = a.netNum[2] = a.netNum[3] = 0;
         // don't add the loopback address or the 
         if(a.netNum[0] != INADDR_ANY && a.netNum[0] != 0x7F000001)
            addressVector->push_back(a);
      }
   }
   free(pIPAddrTable);
}

#elif defined (TNL_OS_MAC_OSX)
#include <ifaddrs.h>

void Socket::getInterfaceAddresses(Vector<Address> *addressVector)
{
   struct ifaddrs *addrs;
   getifaddrs(&addrs);
   
   for(struct ifaddrs *walk = addrs; walk; walk = walk->ifa_next)
   {
      Address theAddress;
      SocketToTNLAddress(walk->ifa_addr, &theAddress);
	  if(theAddress.netNum[0] != INADDR_ANY && theAddress.netNum[0] != 0x7F000001)
	  {
	     //logprintf("found IF address %s", theAddress.toString());
		 addressVector->push_back(theAddress);
      }
   }
   
   freeifaddrs(addrs);
}

#elif defined (TNL_OS_LINUX)
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>

void Socket::getInterfaceAddresses(Vector<Address> *addressVector)
{
   int sfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sfd < 0)
     return;

   FILE *f = fopen("/proc/net/dev", "r");
   if(!f)
   {
      close(sfd);
      return;
   }
   char buf[1024];
   fgets(buf, 1024, f);
   fgets(buf, 1024, f);

   struct ifreq ifr;
   struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
   memset(&ifr, 0, sizeof(ifr));
   

   while(fgets(buf, 1024, f))
   {
      char *s = buf;
      while(*s == ' ')
         s++;
      char *end = strchr(s, ':');
      if(!end)
         continue;
      *end = 0;

      strcpy(ifr.ifr_name, s);
      sin->sin_family = AF_INET;
      if(ioctl(sfd, SIOCGIFADDR, &ifr) == 0)
      {
         Address theAddress;
         SocketToTNLAddress((struct sockaddr *) sin, &theAddress);
	 if(theAddress.netNum[0] != INADDR_ANY && theAddress.netNum[0] != 0x7F000001)
	 {
            addressVector->push_back(theAddress);
         }
      }
   }
   fclose(f);
   close(sfd);
}
#endif

bool Address::operator ==(const Address &theAddress) const
{
   return transport == theAddress.transport &&
      port == theAddress.port &&
      netNum[0] == theAddress.netNum[0] &&
      netNum[1] == theAddress.netNum[1] &&
      netNum[2] == theAddress.netNum[2] &&
      netNum[3] == theAddress.netNum[3];
}

Address::Address(TransportProtocol type, Address::NamedAddress name, U16 aPort)
{
   transport = type;
   port = aPort;
   if(transport == IPProtocol)
   {
      netNum[1] = netNum[2] = netNum[3] = 0;
      switch(name)
      {
         case None:
            netNum[0] = 0;
            break;
         case Localhost:
            netNum[0] = htonl(0x7F000001);
            break;
         case Broadcast:
            netNum[0] = htonl(INADDR_BROADCAST);
            break;
         case Any:
            netNum[0] = htonl(INADDR_ANY);
            break;
      }
   }
   else if(transport == IPXProtocol)
   {
      for(U32 i = 0; i < 4; i++)
         netNum[i] = 0xFFFFFFFF;
   }
}

void Address::set(const IPAddress &address)
{
   transport = IPProtocol;
   port = address.port;
   netNum[0] = address.netNum;
   netNum[1] = netNum[2] = netNum[3] = 0;
}

IPAddress Address::toIPAddress() const
{
   IPAddress ret;
   ret.port = port;
   ret.netNum = netNum[0];
   return ret;
}

bool Address::set(const char *addressString)
{
   init();
   if(strnicmp(addressString, "ipx:", 4))
   {
      bool isTCP = false;
      // assume IP if it doesn't have ipx: at the front.
      if(!strnicmp(addressString, "ip:", 3))
         addressString += 3;  // eat off the ip:
      else if(!strnicmp(addressString, "tcp:", 4))
      {
         addressString += 4;
         isTCP = true;
      }

      SOCKADDR_IN ipAddr;
      char remoteAddr[256];
      if(strlen(addressString) > 255)
         return false;

      strcpy(remoteAddr, addressString);

      char *portString = strchr(remoteAddr, ':');
      if(portString)
         *portString++ = 0;

      if(!stricmp(remoteAddr, "broadcast"))
         ipAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
      else if(!stricmp(remoteAddr, "localhost"))
         ipAddr.sin_addr.s_addr = htonl(0x7F000001);
      else if(!stricmp(remoteAddr, "any"))
         ipAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      else
      {
         ipAddr.sin_addr.s_addr = inet_addr(remoteAddr);
         if(ipAddr.sin_addr.s_addr == INADDR_NONE)
         {
#if defined (TNL_OS_XBOX)
            return false;
#else
            struct hostent *hp;
            if((hp = gethostbyname(remoteAddr)) == NULL)
               return false;
   	      else
   		      memcpy(&ipAddr.sin_addr.s_addr, hp->h_addr, sizeof(IN_ADDR));
#endif
         }
      }
      if(portString)
         ipAddr.sin_port = htons(atoi(portString));
      else
         ipAddr.sin_port = htons(0);
      ipAddr.sin_family = AF_INET;
      SocketToTNLAddress((SOCKADDR *) &ipAddr, this);
      if(isTCP)
         transport = TCPProtocol;
      return true;
   }
   else
   {
      S32 i;
      S32 aPort;

      transport = IPXProtocol;
      for(i = 0; i < 4; i++)
         netNum[i] = 0xFFFFFFFF;

      // it's an IPX string
      addressString += 4;
      if(!stricmp(addressString, "broadcast"))
      {
         port = 0;
         return true;
      }
      else if(sscanf(addressString, "broadcast:%d", &aPort) == 1)
      {
         port = aPort;
         return true;
      }
      else
      {
         U32 aNodeNum[6];
         U32 aNetNum[4];
         S32 count = sscanf(addressString, "%2x%2x%2x%2x:%2x%2x%2x%2x%2x%2x:%d",
            &aNetNum[0], &aNetNum[1], &aNetNum[2], &aNetNum[3],
            &aNodeNum[0], &aNodeNum[1], &aNodeNum[2], &aNodeNum[3], &aNodeNum[4], &aNodeNum[5],
            &aPort);

         if(count == 10)
         {
            aPort = 0;
            count++;
         }
         if(count != 11)
            return false;

         netNum[0] = (aNetNum[0] << 24) |
                     (aNetNum[1] << 16) |
                     (aNetNum[2] << 8 ) |
                     aNetNum[3];
         netNum[1] = (aNodeNum[0] << 24) |
                     (aNodeNum[1] << 16) |
                     (aNodeNum[2] << 8 ) |
                     aNodeNum[3];
         netNum[2] = (aNodeNum[4] << 8) |
                      aNodeNum[5];
         netNum[3] = 0;
         port = aPort;
         return true;
      }
   }
}

const char *Address::toString() const
{
   static char addressBuffer[256];
   if(transport == IPProtocol)
   {
      SOCKADDR_IN ipAddr;
      socklen_t addrLen = sizeof(ipAddr);
      TNLToSocketAddress(*this, (SOCKADDR *) &ipAddr, &addrLen);

      if(ipAddr.sin_addr.s_addr == htonl(INADDR_BROADCAST))
         dSprintf(addressBuffer, 256, "IP:Broadcast:%d", ntohs(ipAddr.sin_port));
      else if(ipAddr.sin_addr.s_addr == htonl(INADDR_ANY))
         dSprintf(addressBuffer, 256, "IP:Any:%d", ntohs(ipAddr.sin_port));         
      else
         dSprintf(addressBuffer, 256, "IP:%d.%d.%d.%d:%d", U8( netNum[0] >> 24 ),
            U8 (netNum[0] >> 16 ), U8 (netNum[0] >> 8), U8(netNum[0]), port);
   }
   else
   {
      dSprintf(addressBuffer, 256, "IPX:%.2X%.2X%.2X%.2X:%.2X%.2X%.2X%.2X%.2X%.2X:%d",
         U8(netNum[0] >> 24), U8(netNum[0] >> 16), U8(netNum[0] >> 8), U8(netNum[0]),
         U8(netNum[1] >> 24), U8(netNum[1] >> 16), U8(netNum[1] >> 8), U8(netNum[1]),
         U8(netNum[2] >> 8), U8(netNum[2]), port);
   }
   return addressBuffer;
}

NetError getLastError()
{
#if defined ( TNL_OS_WIN32 ) || defined ( TNL_OS_XBOX )
   S32 err = WSAGetLastError();
   switch(err)
   {
      case WSAEWOULDBLOCK:
         return WouldBlock;
      default:
         return UnknownError;
   }
#else
   if(errno == EAGAIN)
      return WouldBlock;
   return UnknownError;
#endif
}

};

// --- END OF FILE: tnl/udp.cpp ---
// --- START OF FILE: tnl/vector.cpp ---//-----------------------------------------------------------------------------------
//
//   Torque Network Library
//   Copyright (C) 2004 GarageGames.com, Inc.
//   For more information see http://www.opentnl.org
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For use in products that are not compatible with the terms of the GNU 
//   General Public License, alternative licensing options are available 
//   from GarageGames.com.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//------------------------------------------------------------------------------------

#include "tnlVector.h"

namespace TNL {

bool VectorResize(U32 *aSize, U32 *aCount, void **arrayPtr, U32 newCount, U32 elemSize)
{
   if (newCount > 0) {
      U32 blocks = newCount / VectorBlockSize;
      if (newCount % VectorBlockSize)
         blocks++;
      S32 mem_size = blocks * VectorBlockSize * elemSize;
      *arrayPtr = *arrayPtr ? realloc(*arrayPtr,mem_size) :
         malloc(mem_size);

      *aCount = newCount;
      *aSize = blocks * VectorBlockSize;
      return true;
   }
   if (*arrayPtr) {
      free(*arrayPtr);
      *arrayPtr = 0;
   }

   *aSize = 0;
   *aCount = 0;
   return true;
}

};

// --- END OF FILE: tnl/vector.cpp ---