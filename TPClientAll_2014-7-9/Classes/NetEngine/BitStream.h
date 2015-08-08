#if defined(_MSC_VER) && _MSC_VER < 1299
#include "BitStream_NoTemplate.h"
#else
#pragma once

#include "VEMemoryOverride.h"
#include "VENetDefines.h"
#include "Export.h"
#include "VENetTypes.h"
#include "VEString.h"
#include "VEWString.h"
#include "VEAssert.h"
#include <math.h>
#include <float.h>

#ifdef _MSC_VER
#pragma warning( push )
#endif

#ifndef _WIN32
#define _copysign copysign
#endif

namespace VENet
{
class VE_DLL_EXPORT BitStream
{

public:
    STATIC_FACTORY_DECLARATIONS(BitStream)

    BitStream();

    BitStream( const unsigned int initialBytesToAllocate );

    BitStream( unsigned char* _data, const unsigned int lengthInBytes, bool _copyData );

    ~BitStream();

    void Reset( void );

    template <class templateType>
    bool Serialize(bool writeToBitstream, templateType &inOutTemplateVar);

    template <class templateType>
    bool SerializeDelta(bool writeToBitstream, templateType &inOutCurrentValue, const templateType &lastValue);

    template <class templateType>
    bool SerializeDelta(bool writeToBitstream, templateType &inOutCurrentValue);

    template <class templateType>
    bool SerializeCompressed(bool writeToBitstream, templateType &inOutTemplateVar);

    template <class templateType>
    bool SerializeCompressedDelta(bool writeToBitstream, templateType &inOutCurrentValue, const templateType &lastValue);

    template <class templateType>
    bool SerializeCompressedDelta(bool writeToBitstream, templateType &inOutTemplateVar);

    bool Serialize(bool writeToBitstream,  char* inOutByteArray, const unsigned int numberOfBytes );

    bool SerializeFloat16(bool writeToBitstream, float &inOutFloat, float floatMin, float floatMax);

    template <class serializationType, class sourceType >
    bool SerializeCasted( bool writeToBitstream, sourceType &value );

    template <class templateType>
    bool SerializeBitsFromIntegerRange( bool writeToBitstream, templateType &value, const templateType minimum, const templateType maximum, bool allowOutsideRange=false );

    template <class templateType>
    bool SerializeBitsFromIntegerRange( bool writeToBitstream, templateType &value, const templateType minimum, const templateType maximum, const int requiredBits, bool allowOutsideRange=false );

    template <class templateType>
    bool SerializeNormVector(bool writeToBitstream,  templateType &x, templateType &y, templateType &z );

    template <class templateType>
    bool SerializeVector(bool writeToBitstream,  templateType &x, templateType &y, templateType &z );

    template <class templateType>
    bool SerializeNormQuat(bool writeToBitstream,  templateType &w, templateType &x, templateType &y, templateType &z);

    template <class templateType>
    bool SerializeOrthMatrix(
        bool writeToBitstream,
        templateType &m00, templateType &m01, templateType &m02,
        templateType &m10, templateType &m11, templateType &m12,
        templateType &m20, templateType &m21, templateType &m22 );

    bool SerializeBits(bool writeToBitstream, unsigned char* inOutByteArray, const BitSize_t numberOfBitsToSerialize, const bool rightAlignedBits = true );

    template <class templateType>
    void Write(const templateType &inTemplateVar);

    template <class templateType>
    void WritePtr(templateType *inTemplateVar);

    template <class templateType>
    void WriteDelta(const templateType &currentValue, const templateType &lastValue);

    template <class templateType>
    void WriteDelta(const templateType &currentValue);

    template <class templateType>
    void WriteCompressed(const templateType &inTemplateVar);

    template <class templateType>
    void WriteCompressedDelta(const templateType &currentValue, const templateType &lastValue);

    template <class templateType>
    void WriteCompressedDelta(const templateType &currentValue);

    template <class templateType>
    bool Read(templateType &outTemplateVar);

    template <class templateType>
    bool ReadDelta(templateType &outTemplateVar);

    template <class templateType>
    bool ReadCompressed(templateType &outTemplateVar);

    template <class templateType>
    bool ReadCompressedDelta(templateType &outTemplateVar);

    bool Read( BitStream *bitStream, BitSize_t numberOfBits );
    bool Read( BitStream *bitStream );
    bool Read( BitStream &bitStream, BitSize_t numberOfBits );
    bool Read( BitStream &bitStream );


    void Write( const char* inputByteArray, const unsigned int numberOfBytes );

    void Write( BitStream *bitStream, BitSize_t numberOfBits );
    void Write( BitStream *bitStream );
    void Write( BitStream &bitStream, BitSize_t numberOfBits );
    void Write( BitStream &bitStream );
    \

    void WriteFloat16( float x, float floatMin, float floatMax );

    template <class serializationType, class sourceType >
    void WriteCasted( const sourceType &value );

    template <class templateType>
    void WriteBitsFromIntegerRange( const templateType value, const templateType minimum, const templateType maximum, bool allowOutsideRange=false );
    template <class templateType>
    void WriteBitsFromIntegerRange( const templateType value, const templateType minimum, const templateType maximum, const int requiredBits, bool allowOutsideRange=false );

    template <class templateType>
    void WriteNormVector( templateType x, templateType y, templateType z );

    template <class templateType>
    void WriteVector( templateType x, templateType y, templateType z );

    template <class templateType>
    void WriteNormQuat( templateType w, templateType x, templateType y, templateType z);

    template <class templateType>
    void WriteOrthMatrix(
        templateType m00, templateType m01, templateType m02,
        templateType m10, templateType m11, templateType m12,
        templateType m20, templateType m21, templateType m22 );

    bool Read( char* output, const unsigned int numberOfBytes );

    bool ReadFloat16( float &outFloat, float floatMin, float floatMax );

    template <class serializationType, class sourceType >
    bool ReadCasted( sourceType &value );

    template <class templateType>
    bool ReadBitsFromIntegerRange( templateType &value, const templateType minimum, const templateType maximum, bool allowOutsideRange=false );
    template <class templateType>
    bool ReadBitsFromIntegerRange( templateType &value, const templateType minimum, const templateType maximum, const int requiredBits, bool allowOutsideRange=false );

    template <class templateType>
    bool ReadNormVector( templateType &x, templateType &y, templateType &z );

    template <class templateType>
    bool ReadVector( templateType &x, templateType &y, templateType &z );

    template <class templateType>
    bool ReadNormQuat( templateType &w, templateType &x, templateType &y, templateType &z);


    template <class templateType>
    bool ReadOrthMatrix(
        templateType &m00, templateType &m01, templateType &m02,
        templateType &m10, templateType &m11, templateType &m12,
        templateType &m20, templateType &m21, templateType &m22 );

    void ResetReadPointer( void );

    void ResetWritePointer( void );

    void AssertStreamEmpty( void );

    void PrintBits( char *out ) const;
    void PrintBits( void ) const;
    void PrintHex( char *out ) const;
    void PrintHex( void ) const;

    void IgnoreBits( const BitSize_t numberOfBits );

    void IgnoreBytes( const unsigned int numberOfBytes );

    void SetWriteOffset( const BitSize_t offset );

    inline BitSize_t GetNumberOfBitsUsed( void ) const
    {
        return GetWriteOffset();
    }
    inline BitSize_t GetWriteOffset( void ) const
    {
        return numberOfBitsUsed;
    }

    inline BitSize_t GetNumberOfBytesUsed( void ) const
    {
        return BITS_TO_BYTES( numberOfBitsUsed );
    }

    inline BitSize_t GetReadOffset( void ) const
    {
        return readOffset;
    }

    void SetReadOffset( const BitSize_t newReadOffset )
    {
        readOffset=newReadOffset;
    }

    inline BitSize_t GetNumberOfUnreadBits( void ) const
    {
        return numberOfBitsUsed - readOffset;
    }

    BitSize_t CopyData( unsigned char** _data ) const;

    void SetData( unsigned char *inByteArray );

    inline unsigned char* GetData( void ) const
    {
        return data;
    }

    void WriteBits( const unsigned char* inByteArray, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits = true );

    void WriteAlignedBytes( const unsigned char *inByteArray, const unsigned int numberOfBytesToWrite );

    void EndianSwapBytes( int byteOffset, int length );

    void WriteAlignedBytesSafe( const char *inByteArray, const unsigned int inputLength, const unsigned int maxBytesToWrite );

    bool ReadAlignedBytes( unsigned char *inOutByteArray, const unsigned int numberOfBytesToRead );

    bool ReadAlignedBytesSafe( char *inOutByteArray, int &inputLength, const int maxBytesToRead );
    bool ReadAlignedBytesSafe( char *inOutByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead );

    bool ReadAlignedBytesSafeAlloc( char **outByteArray, int &inputLength, const unsigned int maxBytesToRead );
    bool ReadAlignedBytesSafeAlloc( char **outByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead );

    inline void AlignWriteToByteBoundary( void )
    {
        numberOfBitsUsed += 8 - ( (( numberOfBitsUsed - 1 ) & 7) + 1 );
    }

    inline void AlignReadToByteBoundary( void )
    {
        readOffset += 8 - ( (( readOffset - 1 ) & 7 ) + 1 );
    }

    bool ReadBits( unsigned char *inOutByteArray, BitSize_t numberOfBitsToRead, const bool alignBitsToRight = true );

    void Write0( void );

    void Write1( void );

    bool ReadBit( void );

    void AssertCopyData( void );

    void SetNumberOfBitsAllocated( const BitSize_t lengthInBits );

    void AddBitsAndReallocate( const BitSize_t numberOfBitsToWrite );

    BitSize_t GetNumberOfBitsAllocated(void) const;

    bool Read(char *varString);
    bool Read(unsigned char *varString);

    void PadWithZeroToByteLength( unsigned int bytes );

    static int NumberOfLeadingZeroes( uint8_t x );
    static int NumberOfLeadingZeroes( uint16_t x );
    static int NumberOfLeadingZeroes( uint32_t x );
    static int NumberOfLeadingZeroes( uint64_t x );
    static int NumberOfLeadingZeroes( int8_t x );
    static int NumberOfLeadingZeroes( int16_t x );
    static int NumberOfLeadingZeroes( int32_t x );
    static int NumberOfLeadingZeroes( int64_t x );

    void WriteAlignedVar8(const char *inByteArray);
    bool ReadAlignedVar8(char *inOutByteArray);
    void WriteAlignedVar16(const char *inByteArray);
    bool ReadAlignedVar16(char *inOutByteArray);
    void WriteAlignedVar32(const char *inByteArray);
    bool ReadAlignedVar32(char *inOutByteArray);

    inline void Write(const char * const inStringVar)
    {
        VEString::Serialize(inStringVar, this);
    }
    inline void Write(const wchar_t * const inStringVar)
    {
        VEWString::Serialize(inStringVar, this);
    }
    inline void Write(const unsigned char * const inTemplateVar)
    {
        Write((const char*)inTemplateVar);
    }
    inline void Write(char * const inTemplateVar)
    {
        Write((const char*)inTemplateVar);
    }
    inline void Write(unsigned char * const inTemplateVar)
    {
        Write((const char*)inTemplateVar);
    }
    inline void WriteCompressed(const char * const inStringVar)
    {
        VEString::SerializeCompressed(inStringVar,this,0,false);
    }
    inline void WriteCompressed(const wchar_t * const inStringVar)
    {
        VEWString::Serialize(inStringVar,this);
    }
    inline void WriteCompressed(const unsigned char * const inTemplateVar)
    {
        WriteCompressed((const char*) inTemplateVar);
    }
    inline void WriteCompressed(char * const inTemplateVar)
    {
        WriteCompressed((const char*) inTemplateVar);
    }
    inline void WriteCompressed(unsigned char * const inTemplateVar)
    {
        WriteCompressed((const char*) inTemplateVar);
    }

#if defined(_MSC_VER) && _MSC_VER == 1300
    template <>
    void Write(const bool &var);

    template <>
    void Write(const SystemAddress &var);

    template <>
    void Write(const uint24_t &var);

    template <>
    void Write(const VENetGuid &var);

    template <>
    void Write(const char* const &var);
    template <>
    void Write(const unsigned char* const &var);
    template <>
    void Write(char* const &var);
    template <>
    void Write(unsigned char* const &var);
    template <>
    void Write(const VEString &var);
    template <>
    void Write(const VEWString &var);

    template <>
    void WriteDelta(const SystemAddress &currentValue, const SystemAddress &lastValue);

    template <>
    void WriteDelta(const uint24_t &currentValue, const uint24_t &lastValue);

    template <>
    void WriteDelta(const VENetGUID &currentValue, const VENetGUID &lastValue);

    template <>
    void WriteDelta(const bool &currentValue, const bool &lastValue);

    template <>
    void WriteCompressed(const SystemAddress &var);

    template <>
    void WriteCompressed(const uint24_t &var);

    template <>
    void WriteCompressed(const VENetGUID &var);

    template <>
    void WriteCompressed(const bool &var);

    template <>
    void WriteCompressed(const float &var);

    template <>
    void WriteCompressed(const double &var);

    template <>
    void WriteCompressed(const char* var);
    template <>
    void WriteCompressed(const unsigned char* var);
    template <>
    void WriteCompressed(char* var);
    template <>
    void WriteCompressed(unsigned char* var);
    template <>
    void WriteCompressed(const VEString &var);
    template <>
    void WriteCompressed(const VEWString &var);

    template <>
    void WriteCompressedDelta(const bool &currentValue, const bool &lastValue);

    template <>
    void WriteCompressedDelta(const bool &currentValue);

    template <>
    bool Read(bool &var);

    template <>
    bool Read(SystemAddress &var);

    template <>
    bool Read(uint24_t &var);

    template <>
    bool Read(VENetGUID &var);

    template <>
    bool Read(char *&var);
    template <>
    bool Read(wchar_t *&var);
    template <>
    bool Read(unsigned char *&var);
    template <>
    bool Read(VEString &var);
    template <>
    bool Read(VEWString &var);

    template <>
    bool ReadDelta(bool &var);

    template <>
    bool ReadCompressed(SystemAddress &var);

    template <>
    bool ReadCompressed(uint24_t &var);

    template <>
    bool ReadCompressed(VENetGUID &var);

    template <>
    bool ReadCompressed(bool &var);

    template <>
    bool ReadCompressed(float &var);

    template <>
    bool ReadCompressed(double &var);

    template <>
    bool ReadCompressed(char* &var);
    template <>
    bool ReadCompressed(wchar_t* &var);
    template <>
    bool ReadCompressed(unsigned char *&var);
    template <>
    bool ReadCompressed(VEString &var);
    template <>
    bool ReadCompressed(VEWString &var);

    template <>
    bool ReadCompressedDelta(bool &var);
#endif

    inline static bool DoEndianSwap(void)
    {
#ifndef __BITSTREAM_NATIVE_END
        return IsNetworkOrder()==false;
#else
return false;
#endif
    }
    inline static bool IsBigEndian(void)
    {
        return IsNetworkOrder();
    }
    inline static bool IsNetworkOrder(void)
    {
        static const bool r = IsNetworkOrderInternal();
        return r;
    }
    static bool IsNetworkOrderInternal(void);
    static void ReverseBytes(unsigned char *inByteArray, unsigned char *inOutByteArray, const unsigned int length);
    static void ReverseBytesInPlace(unsigned char *inOutData,const unsigned int length);

private:

    BitStream( const BitStream &invalid)
    {
        (void) invalid;
        VEAssert(0);
    }

    void WriteCompressed( const unsigned char* inByteArray, const unsigned int size, const bool unsignedData );

    bool ReadCompressed( unsigned char* inOutByteArray,	const unsigned int size, const bool unsignedData );


    BitSize_t numberOfBitsUsed;

    BitSize_t numberOfBitsAllocated;

    BitSize_t readOffset;

    unsigned char *data;

    bool copyData;

    unsigned char stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
};

template <class templateType>
inline bool BitStream::Serialize(bool writeToBitstream, templateType &inOutTemplateVar)
{
    if (writeToBitstream)
        Write(inOutTemplateVar);
    else
        return Read(inOutTemplateVar);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType &inOutCurrentValue, const templateType &lastValue)
{
    if (writeToBitstream)
        WriteDelta(inOutCurrentValue, lastValue);
    else
        return ReadDelta(inOutCurrentValue);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType &inOutCurrentValue)
{
    if (writeToBitstream)
        WriteDelta(inOutCurrentValue);
    else
        return ReadDelta(inOutCurrentValue);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeCompressed(bool writeToBitstream, templateType &inOutTemplateVar)
{
    if (writeToBitstream)
        WriteCompressed(inOutTemplateVar);
    else
        return ReadCompressed(inOutTemplateVar);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType &inOutCurrentValue, const templateType &lastValue)
{
    if (writeToBitstream)
        WriteCompressedDelta(inOutCurrentValue,lastValue);
    else
        return ReadCompressedDelta(inOutCurrentValue);
    return true;
}
template <class templateType>
inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType &inOutCurrentValue)
{
    if (writeToBitstream)
        WriteCompressedDelta(inOutCurrentValue);
    else
        return ReadCompressedDelta(inOutCurrentValue);
    return true;
}

inline bool BitStream::Serialize(bool writeToBitstream, char* inOutByteArray, const unsigned int numberOfBytes )
{
    if (writeToBitstream)
        Write(inOutByteArray, numberOfBytes);
    else
        return Read(inOutByteArray, numberOfBytes);
    return true;
}

template <class serializationType, class sourceType >
bool BitStream::SerializeCasted( bool writeToBitstream, sourceType &value )
{
    if (writeToBitstream) WriteCasted<serializationType>(value);
    else return ReadCasted<serializationType>(value);
    return true;
}

template <class templateType>
bool BitStream::SerializeBitsFromIntegerRange( bool writeToBitstream, templateType &value, const templateType minimum, const templateType maximum, bool allowOutsideRange )
{
    int requiredBits=BYTES_TO_BITS(sizeof(templateType))-NumberOfLeadingZeroes(templateType(maximum-minimum));
    return SerializeBitsFromIntegerRange(writeToBitstream,value,minimum,maximum,requiredBits,allowOutsideRange);
}
template <class templateType>
bool BitStream::SerializeBitsFromIntegerRange( bool writeToBitstream, templateType &value, const templateType minimum, const templateType maximum, const int requiredBits, bool allowOutsideRange )
{
    if (writeToBitstream) WriteBitsFromIntegerRange(value,minimum,maximum,requiredBits,allowOutsideRange);
    else return ReadBitsFromIntegerRange(value,minimum,maximum,requiredBits,allowOutsideRange);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeNormVector(bool writeToBitstream, templateType &x, templateType &y, templateType &z )
{
    if (writeToBitstream)
        WriteNormVector(x,y,z);
    else
        return ReadNormVector(x,y,z);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeVector(bool writeToBitstream,  templateType &x, templateType &y, templateType &z )
{
    if (writeToBitstream)
        WriteVector(x,y,z);
    else
        return ReadVector(x,y,z);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeNormQuat(bool writeToBitstream,  templateType &w, templateType &x, templateType &y, templateType &z)
{
    if (writeToBitstream)
        WriteNormQuat(w,x,y,z);
    else
        return ReadNormQuat(w,x,y,z);
    return true;
}

template <class templateType>
inline bool BitStream::SerializeOrthMatrix(
    bool writeToBitstream,
    templateType &m00, templateType &m01, templateType &m02,
    templateType &m10, templateType &m11, templateType &m12,
    templateType &m20, templateType &m21, templateType &m22 )
{
    if (writeToBitstream)
        WriteOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22);
    else
        return ReadOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22);
    return true;
}

inline bool BitStream::SerializeBits(bool writeToBitstream, unsigned char* inOutByteArray, const BitSize_t numberOfBitsToSerialize, const bool rightAlignedBits )
{
    if (writeToBitstream)
        WriteBits(inOutByteArray,numberOfBitsToSerialize,rightAlignedBits);
    else
        return ReadBits(inOutByteArray,numberOfBitsToSerialize,rightAlignedBits);
    return true;
}

template <class templateType>
inline void BitStream::Write(const templateType &inTemplateVar)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
    if (sizeof(inTemplateVar)==1)
        WriteBits( ( unsigned char* ) & inTemplateVar, sizeof( templateType ) * 8, true );
    else
    {
#ifndef __BITSTREAM_NATIVE_END
        if (DoEndianSwap())
        {
            unsigned char output[sizeof(templateType)];
            ReverseBytes((unsigned char*)&inTemplateVar, output, sizeof(templateType));
            WriteBits( ( unsigned char* ) output, sizeof(templateType) * 8, true );
        }
        else
#endif
            WriteBits( ( unsigned char* ) & inTemplateVar, sizeof(templateType) * 8, true );
    }
}

template <class templateType>
inline void BitStream::WritePtr(templateType *inTemplateVar)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
    if (sizeof(templateType)==1)
        WriteBits( ( unsigned char* ) inTemplateVar, sizeof( templateType ) * 8, true );
    else
    {
#ifndef __BITSTREAM_NATIVE_END
        if (DoEndianSwap())
        {
            unsigned char output[sizeof(templateType)];
            ReverseBytes((unsigned char*) inTemplateVar, output, sizeof(templateType));
            WriteBits( ( unsigned char* ) output, sizeof(templateType) * 8, true );
        }
        else
#endif
            WriteBits( ( unsigned char* ) inTemplateVar, sizeof(templateType) * 8, true );
    }
}

template <>
inline void BitStream::Write(const bool &inTemplateVar)
{
    if ( inTemplateVar )
        Write1();
    else
        Write0();
}


template <>
inline void BitStream::Write(const SystemAddress &inTemplateVar)
{
    Write(inTemplateVar.GetIPVersion());
    if (inTemplateVar.GetIPVersion()==4)
    {
        SystemAddress var2=inTemplateVar;
        uint32_t binaryAddress=~inTemplateVar.address.addr4.sin_addr.s_addr;
        WriteBits((unsigned char*)&binaryAddress, sizeof(binaryAddress)*8, true);
        unsigned short p = var2.GetPortNetworkOrder();
        WriteBits((unsigned char*)&p, sizeof(unsigned short)*8, true);
    }
    else
    {
#if VENET_SUPPORT_IPV6==1
        WriteBits((const unsigned char*) &inTemplateVar.address.addr6, sizeof(inTemplateVar.address.addr6)*8, true);
#endif
    }
}

template <>
inline void BitStream::Write(const uint24_t &inTemplateVar)
{
    AlignWriteToByteBoundary();
    AddBitsAndReallocate(3*8);

    if (IsBigEndian()==false)
    {
        data[( numberOfBitsUsed >> 3 ) + 0] = ((char *)&inTemplateVar.val)[0];
        data[( numberOfBitsUsed >> 3 ) + 1] = ((char *)&inTemplateVar.val)[1];
        data[( numberOfBitsUsed >> 3 ) + 2] = ((char *)&inTemplateVar.val)[2];
    }
    else
    {
        data[( numberOfBitsUsed >> 3 ) + 0] = ((char *)&inTemplateVar.val)[3];
        data[( numberOfBitsUsed >> 3 ) + 1] = ((char *)&inTemplateVar.val)[2];
        data[( numberOfBitsUsed >> 3 ) + 2] = ((char *)&inTemplateVar.val)[1];
    }

    numberOfBitsUsed+=3*8;
}

template <>
inline void BitStream::Write(const VENetGUID &inTemplateVar)
{
    Write(inTemplateVar.g);
}

template <>
inline void BitStream::Write(const VEString &inTemplateVar)
{
    inTemplateVar.Serialize(this);
}
template <>
inline void BitStream::Write(const VEWString &inTemplateVar)
{
    inTemplateVar.Serialize(this);
}
template <>
inline void BitStream::Write(const char * const &inStringVar)
{
    VEString::Serialize(inStringVar, this);
}
template <>
inline void BitStream::Write(const wchar_t * const &inStringVar)
{
    VEWString::Serialize(inStringVar, this);
}
template <>
inline void BitStream::Write(const unsigned char * const &inTemplateVar)
{
    Write((const char*)inTemplateVar);
}
template <>
inline void BitStream::Write(char * const &inTemplateVar)
{
    Write((const char*)inTemplateVar);
}
template <>
inline void BitStream::Write(unsigned char * const &inTemplateVar)
{
    Write((const char*)inTemplateVar);
}

template <class templateType>
inline void BitStream::WriteDelta(const templateType &currentValue, const templateType &lastValue)
{
    if (currentValue==lastValue)
    {
        Write(false);
    }
    else
    {
        Write(true);
        Write(currentValue);
    }
}

template <>
inline void BitStream::WriteDelta(const bool &currentValue, const bool &lastValue)
{
    (void) lastValue;

    Write(currentValue);
}

template <class templateType>
inline void BitStream::WriteDelta(const templateType &currentValue)
{
    Write(true);
    Write(currentValue);
}

template <class templateType>
inline void BitStream::WriteCompressed(const templateType &inTemplateVar)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
    if (sizeof(inTemplateVar)==1)
        WriteCompressed( ( unsigned char* ) & inTemplateVar, sizeof( templateType ) * 8, true );
    else
    {
#ifndef __BITSTREAM_NATIVE_END
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

        if (DoEndianSwap())
        {
            unsigned char output[sizeof(templateType)];
            ReverseBytes((unsigned char*)&inTemplateVar, output, sizeof(templateType));
            WriteCompressed( ( unsigned char* ) output, sizeof(templateType) * 8, true );
        }
        else
#endif
            WriteCompressed( ( unsigned char* ) & inTemplateVar, sizeof(templateType) * 8, true );
    }
}

template <>
inline void BitStream::WriteCompressed(const SystemAddress &inTemplateVar)
{
    Write(inTemplateVar);
}

template <>
inline void BitStream::WriteCompressed(const VENetGUID &inTemplateVar)
{
    Write(inTemplateVar);
}

template <>
inline void BitStream::WriteCompressed(const uint24_t &var)
{
    Write(var);
}

template <>
inline void BitStream::WriteCompressed(const bool &inTemplateVar)
{
    Write(inTemplateVar);
}

template <>
inline void BitStream::WriteCompressed(const float &inTemplateVar)
{
    VEAssert(inTemplateVar > -1.01f && inTemplateVar < 1.01f);
    float varCopy=inTemplateVar;
    if (varCopy < -1.0f)
        varCopy=-1.0f;
    if (varCopy > 1.0f)
        varCopy=1.0f;
    Write((unsigned short)((varCopy+1.0f)*32767.5f));
}

template <>
inline void BitStream::WriteCompressed(const double &inTemplateVar)
{
    VEAssert(inTemplateVar > -1.01 && inTemplateVar < 1.01);
    double varCopy=inTemplateVar;
    if (varCopy < -1.0f)
        varCopy=-1.0f;
    if (varCopy > 1.0f)
        varCopy=1.0f;
    Write((uint32_t)((varCopy+1.0)*2147483648.0));
}

template <>
inline void BitStream::WriteCompressed(const VEString &inTemplateVar)
{
    inTemplateVar.SerializeCompressed(this,0,false);
}
template <>
inline void BitStream::WriteCompressed(const VEWString &inTemplateVar)
{
    inTemplateVar.Serialize(this);
}
template <>
inline void BitStream::WriteCompressed(const char * const &inStringVar)
{
    VEString::SerializeCompressed(inStringVar,this,0,false);
}
template <>
inline void BitStream::WriteCompressed(const wchar_t * const &inStringVar)
{
    VEWString::Serialize(inStringVar,this);
}
template <>
inline void BitStream::WriteCompressed(const unsigned char * const &inTemplateVar)
{
    WriteCompressed((const char*) inTemplateVar);
}
template <>
inline void BitStream::WriteCompressed(char * const &inTemplateVar)
{
    WriteCompressed((const char*) inTemplateVar);
}
template <>
inline void BitStream::WriteCompressed(unsigned char * const &inTemplateVar)
{
    WriteCompressed((const char*) inTemplateVar);
}

template <class templateType>
inline void BitStream::WriteCompressedDelta(const templateType &currentValue, const templateType &lastValue)
{
    if (currentValue==lastValue)
    {
        Write(false);
    }
    else
    {
        Write(true);
        WriteCompressed(currentValue);
    }
}

template <>
inline void BitStream::WriteCompressedDelta(const bool &currentValue, const bool &lastValue)
{
    (void) lastValue;

    Write(currentValue);
}

template <class templateType>
inline void BitStream::WriteCompressedDelta(const templateType &currentValue)
{
    Write(true);
    WriteCompressed(currentValue);
}

template <>
inline void BitStream::WriteCompressedDelta(const bool &currentValue)
{
    Write(currentValue);
}

template <class templateType>
inline bool BitStream::Read(templateType &outTemplateVar)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
    if (sizeof(outTemplateVar)==1)
        return ReadBits( ( unsigned char* ) &outTemplateVar, sizeof(templateType) * 8, true );
    else
    {
#ifndef __BITSTREAM_NATIVE_END
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
        if (DoEndianSwap())
        {
            unsigned char output[sizeof(templateType)];
            if (ReadBits( ( unsigned char* ) output, sizeof(templateType) * 8, true ))
            {
                ReverseBytes(output, (unsigned char*)&outTemplateVar, sizeof(templateType));
                return true;
            }
            return false;
        }
        else
#endif
            return ReadBits( ( unsigned char* ) & outTemplateVar, sizeof(templateType) * 8, true );
    }
}

template <>
inline bool BitStream::Read(bool &outTemplateVar)
{
    if ( readOffset + 1 > numberOfBitsUsed )
        return false;

    if ( data[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) )
        outTemplateVar = true;
    else
        outTemplateVar = false;

    readOffset++;

    return true;
}

template <>
inline bool BitStream::Read(SystemAddress &outTemplateVar)
{
    unsigned char ipVersion;
    Read(ipVersion);
    if (ipVersion==4)
    {
        outTemplateVar.address.addr4.sin_family=AF_INET;
        uint32_t binaryAddress;
        ReadBits( ( unsigned char* ) & binaryAddress, sizeof(binaryAddress) * 8, true );
        outTemplateVar.address.addr4.sin_addr.s_addr=~binaryAddress;
        bool b = ReadBits(( unsigned char* ) & outTemplateVar.address.addr4.sin_port, sizeof(outTemplateVar.address.addr4.sin_port) * 8, true);
        outTemplateVar.debugPort=ntohs(outTemplateVar.address.addr4.sin_port);
        return b;
    }
    else
    {
#if VENET_SUPPORT_IPV6==1
        bool b = ReadBits((unsigned char*) &outTemplateVar.address.addr6, sizeof(outTemplateVar.address.addr6)*8, true);
        outTemplateVar.debugPort=ntohs(outTemplateVar.address.addr6.sin6_port);
        return b;
#else
return false;
#endif
    }
}

template <>
inline bool BitStream::Read(uint24_t &outTemplateVar)
{
    AlignReadToByteBoundary();
    if ( readOffset + 3*8 > numberOfBitsUsed )
        return false;

    if (IsBigEndian()==false)
    {
        ((char *)&outTemplateVar.val)[0]=data[ (readOffset >> 3) + 0];
        ((char *)&outTemplateVar.val)[1]=data[ (readOffset >> 3) + 1];
        ((char *)&outTemplateVar.val)[2]=data[ (readOffset >> 3) + 2];
        ((char *)&outTemplateVar.val)[3]=0;
    }
    else
    {

        ((char *)&outTemplateVar.val)[3]=data[ (readOffset >> 3) + 0];
        ((char *)&outTemplateVar.val)[2]=data[ (readOffset >> 3) + 1];
        ((char *)&outTemplateVar.val)[1]=data[ (readOffset >> 3) + 2];
        ((char *)&outTemplateVar.val)[0]=0;
    }

    readOffset+=3*8;
    return true;
}

template <>
inline bool BitStream::Read(VENetGUID &outTemplateVar)
{
    return Read(outTemplateVar.g);
}


template <>
inline bool BitStream::Read(VEString &outTemplateVar)
{
    return outTemplateVar.Deserialize(this);
}
template <>
inline bool BitStream::Read(VEWString &outTemplateVar)
{
    return outTemplateVar.Deserialize(this);
}
template <>
inline bool BitStream::Read(char *&varString)
{
    return VEString::Deserialize(varString,this);
}
template <>
inline bool BitStream::Read(wchar_t *&varString)
{
    return VEWString::Deserialize(varString,this);
}
template <>
inline bool BitStream::Read(unsigned char *&varString)
{
    return VEString::Deserialize((char*) varString,this);
}

template <class templateType>
inline bool BitStream::ReadDelta(templateType &outTemplateVar)
{
    bool dataWritten;
    bool success;
    success=Read(dataWritten);
    if (dataWritten)
        success=Read(outTemplateVar);
    return success;
}

template <>
inline bool BitStream::ReadDelta(bool &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <class templateType>
inline bool BitStream::ReadCompressed(templateType &outTemplateVar)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
    if (sizeof(outTemplateVar)==1)
        return ReadCompressed( ( unsigned char* ) &outTemplateVar, sizeof(templateType) * 8, true );
    else
    {
#ifndef __BITSTREAM_NATIVE_END
        if (DoEndianSwap())
        {
            unsigned char output[sizeof(templateType)];
            if (ReadCompressed( ( unsigned char* ) output, sizeof(templateType) * 8, true ))
            {
                ReverseBytes(output, (unsigned char*)&outTemplateVar, sizeof(templateType));
                return true;
            }
            return false;
        }
        else
#endif
            return ReadCompressed( ( unsigned char* ) & outTemplateVar, sizeof(templateType) * 8, true );
    }
}

template <>
inline bool BitStream::ReadCompressed(SystemAddress &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <>
inline bool BitStream::ReadCompressed(uint24_t &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <>
inline bool BitStream::ReadCompressed(VENetGUID &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <>
inline bool BitStream::ReadCompressed(bool &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <>
inline bool BitStream::ReadCompressed(float &outTemplateVar)
{
    unsigned short compressedFloat;
    if (Read(compressedFloat))
    {
        outTemplateVar = ((float)compressedFloat / 32767.5f - 1.0f);
        return true;
    }
    return false;
}

template <>
inline bool BitStream::ReadCompressed(double &outTemplateVar)
{
    uint32_t compressedFloat;
    if (Read(compressedFloat))
    {
        outTemplateVar = ((double)compressedFloat / 2147483648.0 - 1.0);
        return true;
    }
    return false;
}

template <>
inline bool BitStream::ReadCompressed(VEString &outTemplateVar)
{
    return outTemplateVar.DeserializeCompressed(this,false);
}
template <>
inline bool BitStream::ReadCompressed(VEWString &outTemplateVar)
{
    return outTemplateVar.Deserialize(this);
}
template <>
inline bool BitStream::ReadCompressed(char *&outTemplateVar)
{
    return VEString::DeserializeCompressed(outTemplateVar,this,false);
}
template <>
inline bool BitStream::ReadCompressed(wchar_t *&outTemplateVar)
{
    return VEWString::Deserialize(outTemplateVar,this);
}
template <>
inline bool BitStream::ReadCompressed(unsigned char *&outTemplateVar)
{
    return VEString::DeserializeCompressed((char*) outTemplateVar,this,false);
}

template <class templateType>
inline bool BitStream::ReadCompressedDelta(templateType &outTemplateVar)
{
    bool dataWritten;
    bool success;
    success=Read(dataWritten);
    if (dataWritten)
        success=ReadCompressed(outTemplateVar);
    return success;
}

template <>
inline bool BitStream::ReadCompressedDelta(bool &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <class destinationType, class sourceType >
void BitStream::WriteCasted( const sourceType &value )
{
    destinationType val = (destinationType) value;
    Write(val);
}

template <class templateType>
void BitStream::WriteBitsFromIntegerRange( const templateType value, const templateType minimum,const templateType maximum, bool allowOutsideRange )
{
    int requiredBits=BYTES_TO_BITS(sizeof(templateType))-NumberOfLeadingZeroes(templateType(maximum-minimum));
    WriteBitsFromIntegerRange(value,minimum,maximum,requiredBits,allowOutsideRange);
}
template <class templateType>
void BitStream::WriteBitsFromIntegerRange( const templateType value, const templateType minimum,const templateType maximum, const int requiredBits, bool allowOutsideRange )
{
    VEAssert(maximum>=minimum);
    VEAssert(allowOutsideRange==true || (value>=minimum && value<=maximum));
    if (allowOutsideRange)
    {
        if (value<minimum || value>maximum)
        {
            Write(true);
            Write(value);
            return;
        }
        Write(false);
    }
    templateType valueOffMin=value-minimum;
    if (IsBigEndian()==true)
    {
        unsigned char output[sizeof(templateType)];
        ReverseBytes((unsigned char*)&valueOffMin, output, sizeof(templateType));
        WriteBits(output,requiredBits);
    }
    else
    {
        WriteBits((unsigned char*) &valueOffMin,requiredBits);
    }
}

template <class templateType>
void BitStream::WriteNormVector( templateType x, templateType y, templateType z )
{
#ifdef _DEBUG
    VEAssert(x <= 1.01 && y <= 1.01 && z <= 1.01 && x >= -1.01 && y >= -1.01 && z >= -1.01);
#endif

    WriteFloat16((float)x,-1.0f,1.0f);
    WriteFloat16((float)y,-1.0f,1.0f);
    WriteFloat16((float)z,-1.0f,1.0f);
}

template <class templateType>
void BitStream::WriteVector( templateType x, templateType y, templateType z )
{
    templateType magnitude = sqrt(x * x + y * y + z * z);
    Write((float)magnitude);
    if (magnitude > 0.00001f)
    {
        WriteCompressed((float)(x/magnitude));
        WriteCompressed((float)(y/magnitude));
        WriteCompressed((float)(z/magnitude));
    }
}

template <class templateType>
void BitStream::WriteNormQuat( templateType w, templateType x, templateType y, templateType z)
{
    Write((bool)(w<0.0));
    Write((bool)(x<0.0));
    Write((bool)(y<0.0));
    Write((bool)(z<0.0));
    Write((unsigned short)(fabs(x)*65535.0));
    Write((unsigned short)(fabs(y)*65535.0));
    Write((unsigned short)(fabs(z)*65535.0));
}

template <class templateType>
void BitStream::WriteOrthMatrix(
    templateType m00, templateType m01, templateType m02,
    templateType m10, templateType m11, templateType m12,
    templateType m20, templateType m21, templateType m22 )
{

    double qw;
    double qx;
    double qy;
    double qz;

    float sum;
    sum = 1 + m00 + m11 + m22;
    if (sum < 0.0f) sum=0.0f;
    qw = sqrt( sum  ) / 2;
    sum = 1 + m00 - m11 - m22;
    if (sum < 0.0f) sum=0.0f;
    qx = sqrt( sum  ) / 2;
    sum = 1 - m00 + m11 - m22;
    if (sum < 0.0f) sum=0.0f;
    qy = sqrt( sum  ) / 2;
    sum = 1 - m00 - m11 + m22;
    if (sum < 0.0f) sum=0.0f;
    qz = sqrt( sum  ) / 2;
    if (qw < 0.0) qw=0.0;
    if (qx < 0.0) qx=0.0;
    if (qy < 0.0) qy=0.0;
    if (qz < 0.0) qz=0.0;
    qx = _copysign( (double) qx, (double) (m21 - m12) );
    qy = _copysign( (double) qy, (double) (m02 - m20) );
    qz = _copysign( (double) qz, (double) (m10 - m01) );

    WriteNormQuat(qw,qx,qy,qz);
}

template <class serializationType, class sourceType >
bool BitStream::ReadCasted( sourceType &value )
{
    serializationType val;
    bool success = Read(val);
    value=(sourceType) val;
    return success;
}

template <class templateType>
bool BitStream::ReadBitsFromIntegerRange( templateType &value, const templateType minimum, const templateType maximum, bool allowOutsideRange )
{
    int requiredBits=BYTES_TO_BITS(sizeof(templateType))-NumberOfLeadingZeroes(templateType(maximum-minimum));
    return ReadBitsFromIntegerRange(value,minimum,maximum,requiredBits,allowOutsideRange);
}
template <class templateType>
bool BitStream::ReadBitsFromIntegerRange( templateType &value, const templateType minimum, const templateType maximum, const int requiredBits, bool allowOutsideRange )
{
    VEAssert(maximum>=minimum);
    if (allowOutsideRange)
    {
        bool isOutsideRange;
        Read(isOutsideRange);
        if (isOutsideRange)
            return Read(value);
    }
    unsigned char output[sizeof(templateType)];
    memset(output,0,sizeof(output));
    bool success = ReadBits(output,requiredBits);
    if (success)
    {
        if (IsBigEndian()==true)
            ReverseBytesInPlace(output,sizeof(output));
        memcpy(&value,output,sizeof(output));

        value+=minimum;
    }

    return success;
}

template <class templateType>
bool BitStream::ReadNormVector( templateType &x, templateType &y, templateType &z )
{
    float xIn,yIn,zIn;
    ReadFloat16(xIn,-1.0f,1.0f);
    ReadFloat16(yIn,-1.0f,1.0f);
    ReadFloat16(zIn,-1.0f,1.0f);
    x=xIn;
    y=yIn;
    z=zIn;
    return true;
}

template <class templateType>
bool BitStream::ReadVector( templateType &x, templateType &y, templateType &z )
{
    float magnitude;
    if (!Read(magnitude))
        return false;
    if (magnitude>0.00001f)
    {
        float cx=0.0f,cy=0.0f,cz=0.0f;
        ReadCompressed(cx);
        ReadCompressed(cy);
        if (!ReadCompressed(cz))
            return false;
        x=cx;
        y=cy;
        z=cz;
        x*=magnitude;
        y*=magnitude;
        z*=magnitude;
    }
    else
    {
        x=0.0;
        y=0.0;
        z=0.0;
    }
    return true;
}

template <class templateType>
bool BitStream::ReadNormQuat( templateType &w, templateType &x, templateType &y, templateType &z)
{
    bool cwNeg=false, cxNeg=false, cyNeg=false, czNeg=false;
    unsigned short cx,cy,cz;
    Read(cwNeg);
    Read(cxNeg);
    Read(cyNeg);
    Read(czNeg);
    Read(cx);
    Read(cy);
    if (!Read(cz))
        return false;

    x=(templateType)(cx/65535.0);
    y=(templateType)(cy/65535.0);
    z=(templateType)(cz/65535.0);
    if (cxNeg) x=-x;
    if (cyNeg) y=-y;
    if (czNeg) z=-z;
    float difference = 1.0f - x*x - y*y - z*z;
    if (difference < 0.0f)
        difference=0.0f;
    w = (templateType)(sqrt(difference));
    if (cwNeg)
        w=-w;

    return true;
}

template <class templateType>
bool BitStream::ReadOrthMatrix(
    templateType &m00, templateType &m01, templateType &m02,
    templateType &m10, templateType &m11, templateType &m12,
    templateType &m20, templateType &m21, templateType &m22 )
{
    float qw,qx,qy,qz;
    if (!ReadNormQuat(qw,qx,qy,qz))
        return false;

    double sqw = (double)qw*(double)qw;
    double sqx = (double)qx*(double)qx;
    double sqy = (double)qy*(double)qy;
    double sqz = (double)qz*(double)qz;
    m00 =  (templateType)(sqx - sqy - sqz + sqw);
    m11 = (templateType)(-sqx + sqy - sqz + sqw);
    m22 = (templateType)(-sqx - sqy + sqz + sqw);

    double tmp1 = (double)qx*(double)qy;
    double tmp2 = (double)qz*(double)qw;
    m10 = (templateType)(2.0 * (tmp1 + tmp2));
    m01 = (templateType)(2.0 * (tmp1 - tmp2));

    tmp1 = (double)qx*(double)qz;
    tmp2 = (double)qy*(double)qw;
    m20 =(templateType)(2.0 * (tmp1 - tmp2));
    m02 = (templateType)(2.0 * (tmp1 + tmp2));
    tmp1 = (double)qy*(double)qz;
    tmp2 = (double)qx*(double)qw;
    m21 = (templateType)(2.0 * (tmp1 + tmp2));
    m12 = (templateType)(2.0 * (tmp1 - tmp2));

    return true;
}

//template <class templateType>
//BitStream& operator<<(BitStream& out, templateType& c)
//{
//    out.Write(c);
//    return out;
//}
//template <class templateType>
//BitStream& operator>>(BitStream& in, templateType& c)
//{
//    bool success = in.Read(c);
//    (void)success;
//
//    VEAssert(success);
//    return in;
//}

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
