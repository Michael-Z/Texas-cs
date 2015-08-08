#if defined(_MSC_VER) && _MSC_VER < 1299
#include "BitStream_NoTemplate.cpp"
#else

#include "BitStream.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "VEString.h"

#include "SocketIncludes.h"
#include "VENetDefines.h"
#include "StringCompressor.h"

#if   defined(_WIN32)
#include <winsock2.h>
#include <memory.h>
#include <cmath>
#include <float.h>


#else
#include <arpa/inet.h>
#include <memory.h>
#if defined(ANDROID)
#include <math.h>
#else
#include <cmath>
#endif
#include <float.h>
#endif

#ifndef _WIN32
#define _copysign copysign
#endif

using namespace VENet;

#ifdef _MSC_VER
#pragma warning( push )
#endif

STATIC_FACTORY_DEFINITIONS(BitStream,BitStream)

BitStream::BitStream()
{
    numberOfBitsUsed = 0;
    numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
    readOffset = 0;
    data = ( unsigned char* ) stackData;

#ifdef _DEBUG
#endif
    copyData = true;
}

BitStream::BitStream( const unsigned int initialBytesToAllocate )
{
    numberOfBitsUsed = 0;
    readOffset = 0;
    if (initialBytesToAllocate <= BITSTREAM_STACK_ALLOCATION_SIZE)
    {
        data = ( unsigned char* ) stackData;
        numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
    }
    else
    {
        data = ( unsigned char* ) rakMalloc_Ex( (size_t) initialBytesToAllocate, _FILE_AND_LINE_ );
        numberOfBitsAllocated = initialBytesToAllocate << 3;
    }
#ifdef _DEBUG
    VEAssert( data );
#endif
    copyData = true;
}

BitStream::BitStream( unsigned char* _data, const unsigned int lengthInBytes, bool _copyData )
{
    numberOfBitsUsed = lengthInBytes << 3;
    readOffset = 0;
    copyData = _copyData;
    numberOfBitsAllocated = lengthInBytes << 3;

    if ( copyData )
    {
        if ( lengthInBytes > 0 )
        {
            if (lengthInBytes < BITSTREAM_STACK_ALLOCATION_SIZE)
            {
                data = ( unsigned char* ) stackData;
                numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE << 3;
            }
            else
            {
                data = ( unsigned char* ) rakMalloc_Ex( (size_t) lengthInBytes, _FILE_AND_LINE_ );
            }
#ifdef _DEBUG
            VEAssert( data );
#endif
            memcpy( data, _data, (size_t) lengthInBytes );
        }
        else
            data = 0;
    }
    else
        data = ( unsigned char* ) _data;
}

void BitStream::SetNumberOfBitsAllocated( const BitSize_t lengthInBits )
{
#ifdef _DEBUG
    VEAssert( lengthInBits >= ( BitSize_t ) numberOfBitsAllocated );
#endif
    numberOfBitsAllocated = lengthInBits;
}

BitStream::~BitStream()
{
    if ( copyData && numberOfBitsAllocated > (BITSTREAM_STACK_ALLOCATION_SIZE << 3))
        veFree_Ex( data , _FILE_AND_LINE_ );
}

void BitStream::Reset( void )
{
    if ( numberOfBitsUsed > 0 )
    {
    }

    numberOfBitsUsed = 0;

    readOffset = 0;
}

void BitStream::Write( const char* inputByteArray, const unsigned int numberOfBytes )
{
    if (numberOfBytes==0)
        return;

    if ((numberOfBitsUsed & 7) == 0)
    {
        AddBitsAndReallocate( BYTES_TO_BITS(numberOfBytes) );
        memcpy(data+BITS_TO_BYTES(numberOfBitsUsed), inputByteArray, (size_t) numberOfBytes);
        numberOfBitsUsed+=BYTES_TO_BITS(numberOfBytes);
    }
    else
    {
        WriteBits( ( unsigned char* ) inputByteArray, numberOfBytes * 8, true );
    }

}
void BitStream::Write( BitStream *bitStream)
{
    Write(bitStream, bitStream->GetNumberOfBitsUsed()-bitStream->GetReadOffset());
}
void BitStream::Write( BitStream *bitStream, BitSize_t numberOfBits )
{
    AddBitsAndReallocate( numberOfBits );
    BitSize_t numberOfBitsMod8;

    if ((bitStream->GetReadOffset()&7)==0 && (numberOfBitsUsed&7)==0)
    {
        int readOffsetBytes=bitStream->GetReadOffset()/8;
        int numBytes=numberOfBits/8;
        memcpy(data + (numberOfBitsUsed >> 3), bitStream->GetData()+readOffsetBytes, numBytes);
        numberOfBits-=BYTES_TO_BITS(numBytes);
        bitStream->SetReadOffset(BYTES_TO_BITS(numBytes+readOffsetBytes));
        numberOfBitsUsed+=BYTES_TO_BITS(numBytes);
    }

    while (numberOfBits-->0 && bitStream->readOffset + 1 <= bitStream->numberOfBitsUsed)
    {
        numberOfBitsMod8 = numberOfBitsUsed & 7;
        if ( numberOfBitsMod8 == 0 )
        {
            if (bitStream->data[ bitStream->readOffset >> 3 ] & ( 0x80 >> ( bitStream->readOffset & 7 ) ) )
            {
                data[ numberOfBitsUsed >> 3 ] = 0x80;
            }
            else
            {
                data[ numberOfBitsUsed >> 3 ] = 0;
            }

        }
        else
        {
            if (bitStream->data[ bitStream->readOffset >> 3 ] & ( 0x80 >> ( bitStream->readOffset & 7 ) ) )
                data[ numberOfBitsUsed >> 3 ] |= 0x80 >> ( numberOfBitsMod8 );
        }

        bitStream->readOffset++;
        numberOfBitsUsed++;
    }
}
void BitStream::Write( BitStream &bitStream, BitSize_t numberOfBits )
{
    Write(&bitStream, numberOfBits);
}
void BitStream::Write( BitStream &bitStream )
{
    Write(&bitStream);
}
bool BitStream::Read( BitStream *bitStream, BitSize_t numberOfBits )
{
    if (GetNumberOfUnreadBits() < numberOfBits)
        return false;
    bitStream->Write(this, numberOfBits);
    return true;
}
bool BitStream::Read( BitStream *bitStream )
{
    bitStream->Write(this);
    return true;
}
bool BitStream::Read( BitStream &bitStream, BitSize_t numberOfBits )
{
    if (GetNumberOfUnreadBits() < numberOfBits)
        return false;
    bitStream.Write(this, numberOfBits);
    return true;
}
bool BitStream::Read( BitStream &bitStream )
{
    bitStream.Write(this);
    return true;
}

bool BitStream::Read( char* outByteArray, const unsigned int numberOfBytes )
{
    if ((readOffset & 7) == 0)
    {
        if ( readOffset + ( numberOfBytes << 3 ) > numberOfBitsUsed )
            return false;

        memcpy( outByteArray, data + ( readOffset >> 3 ), (size_t) numberOfBytes );

        readOffset += numberOfBytes << 3;
        return true;
    }
    else
    {
        return ReadBits( ( unsigned char* ) outByteArray, numberOfBytes * 8 );
    }
}

void BitStream::ResetReadPointer( void )
{
    readOffset = 0;
}

void BitStream::ResetWritePointer( void )
{
    numberOfBitsUsed = 0;
}

void BitStream::Write0( void )
{
    AddBitsAndReallocate( 1 );

    if ( ( numberOfBitsUsed & 7 ) == 0 )
        data[ numberOfBitsUsed >> 3 ] = 0;

    numberOfBitsUsed++;
}

void BitStream::Write1( void )
{
    AddBitsAndReallocate( 1 );

    BitSize_t numberOfBitsMod8 = numberOfBitsUsed & 7;

    if ( numberOfBitsMod8 == 0 )
        data[ numberOfBitsUsed >> 3 ] = 0x80;
    else
        data[ numberOfBitsUsed >> 3 ] |= 0x80 >> ( numberOfBitsMod8 );

    numberOfBitsUsed++;
}

bool BitStream::ReadBit( void )
{
    bool result = ( data[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) ) !=0;
    readOffset++;
    return result;
}

void BitStream::WriteAlignedBytes( const unsigned char* inByteArray, const unsigned int numberOfBytesToWrite )
{
    AlignWriteToByteBoundary();
    Write((const char*) inByteArray, numberOfBytesToWrite);
}
void BitStream::EndianSwapBytes( int byteOffset, int length )
{
    if (DoEndianSwap())
    {
        ReverseBytesInPlace(data+byteOffset, length);
    }
}
void BitStream::WriteAlignedBytesSafe( const char *inByteArray, const unsigned int inputLength, const unsigned int maxBytesToWrite )
{
    if (inByteArray==0 || inputLength==0)
    {
        WriteCompressed((unsigned int)0);
        return;
    }
    WriteCompressed(inputLength);
    WriteAlignedBytes((const unsigned char*) inByteArray, inputLength < maxBytesToWrite ? inputLength : maxBytesToWrite);
}

bool BitStream::ReadAlignedBytes( unsigned char* inOutByteArray, const unsigned int numberOfBytesToRead )
{
#ifdef _DEBUG
    VEAssert( numberOfBytesToRead > 0 );
#endif

    if ( numberOfBytesToRead <= 0 )
        return false;

    AlignReadToByteBoundary();

    if ( readOffset + ( numberOfBytesToRead << 3 ) > numberOfBitsUsed )
        return false;

    memcpy( inOutByteArray, data + ( readOffset >> 3 ), (size_t) numberOfBytesToRead );

    readOffset += numberOfBytesToRead << 3;

    return true;
}
bool BitStream::ReadAlignedBytesSafe( char *inOutByteArray, int &inputLength, const int maxBytesToRead )
{
    return ReadAlignedBytesSafe(inOutByteArray,(unsigned int&) inputLength,(unsigned int)maxBytesToRead);
}
bool BitStream::ReadAlignedBytesSafe( char *inOutByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead )
{
    if (ReadCompressed(inputLength)==false)
        return false;
    if (inputLength > maxBytesToRead)
        inputLength=maxBytesToRead;
    if (inputLength==0)
        return true;
    return ReadAlignedBytes((unsigned char*) inOutByteArray, inputLength);
}
bool BitStream::ReadAlignedBytesSafeAlloc( char **outByteArray, int &inputLength, const unsigned int maxBytesToRead )
{
    return ReadAlignedBytesSafeAlloc(outByteArray,(unsigned int&) inputLength, maxBytesToRead);
}
bool BitStream::ReadAlignedBytesSafeAlloc( char ** outByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead )
{
    veFree_Ex(*outByteArray, _FILE_AND_LINE_ );
    *outByteArray=0;
    if (ReadCompressed(inputLength)==false)
        return false;
    if (inputLength > maxBytesToRead)
        inputLength=maxBytesToRead;
    if (inputLength==0)
        return true;
    *outByteArray = (char*) rakMalloc_Ex( (size_t) inputLength, _FILE_AND_LINE_ );
    return ReadAlignedBytes((unsigned char*) *outByteArray, inputLength);
}

void BitStream::WriteBits( const unsigned char* inByteArray, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits )
{

    AddBitsAndReallocate( numberOfBitsToWrite );

    const BitSize_t numberOfBitsUsedMod8 = numberOfBitsUsed & 7;

    if (numberOfBitsUsedMod8==0 && (numberOfBitsToWrite&7)==0)
    {
        memcpy( data + ( numberOfBitsUsed >> 3 ), inByteArray, numberOfBitsToWrite>>3);
        numberOfBitsUsed+=numberOfBitsToWrite;
        return;
    }

    unsigned char dataByte;
    const unsigned char* inputPtr=inByteArray;

    while ( numberOfBitsToWrite > 0 )
    {
        dataByte = *( inputPtr++ );

        if ( numberOfBitsToWrite < 8 && rightAlignedBits )
            dataByte <<= 8 - numberOfBitsToWrite;

        if ( numberOfBitsUsedMod8 == 0 )
            * ( data + ( numberOfBitsUsed >> 3 ) ) = dataByte;
        else
        {
            *( data + ( numberOfBitsUsed >> 3 ) ) |= dataByte >> ( numberOfBitsUsedMod8 );

            if ( 8 - ( numberOfBitsUsedMod8 ) < 8 && 8 - ( numberOfBitsUsedMod8 ) < numberOfBitsToWrite )
            {
                *( data + ( numberOfBitsUsed >> 3 ) + 1 ) = (unsigned char) ( dataByte << ( 8 - ( numberOfBitsUsedMod8 ) ) );
            }
        }

        if ( numberOfBitsToWrite >= 8 )
        {
            numberOfBitsUsed += 8;
            numberOfBitsToWrite -= 8;
        }
        else
        {
            numberOfBitsUsed += numberOfBitsToWrite;
            numberOfBitsToWrite=0;
        }
    }
}

void BitStream::SetData( unsigned char *inByteArray )
{
    data=inByteArray;
    copyData=false;
}

void BitStream::WriteCompressed( const unsigned char* inByteArray,
                                 const unsigned int size, const bool unsignedData )
{
    BitSize_t currentByte = ( size >> 3 ) - 1;

    unsigned char byteMatch;

    if ( unsignedData )
    {
        byteMatch = 0;
    }

    else
    {
        byteMatch = 0xFF;
    }

    while ( currentByte > 0 )
    {
        if ( inByteArray[ currentByte ] == byteMatch )
        {
            bool b = true;
            Write( b );
        }
        else
        {
            bool b = false;
            Write( b );

            WriteBits( inByteArray, ( currentByte + 1 ) << 3, true );

            return ;
        }

        currentByte--;
    }

    if ( ( unsignedData && ( ( *( inByteArray + currentByte ) ) & 0xF0 ) == 0x00 ) ||
            ( unsignedData == false && ( ( *( inByteArray + currentByte ) ) & 0xF0 ) == 0xF0 ) )
    {
        bool b = true;
        Write( b );
        WriteBits( inByteArray + currentByte, 4, true );
    }

    else
    {
        bool b = false;
        Write( b );
        WriteBits( inByteArray + currentByte, 8, true );
    }
}

bool BitStream::ReadBits( unsigned char *inOutByteArray, BitSize_t numberOfBitsToRead, const bool alignBitsToRight )
{
#ifdef _DEBUG
#endif
    if (numberOfBitsToRead<=0)
        return false;

    if ( readOffset + numberOfBitsToRead > numberOfBitsUsed )
        return false;


    const BitSize_t readOffsetMod8 = readOffset & 7;

    if (readOffsetMod8==0 && (numberOfBitsToRead&7)==0)
    {
        memcpy( inOutByteArray, data + ( readOffset >> 3 ), numberOfBitsToRead>>3);
        readOffset+=numberOfBitsToRead;
        return true;
    }



    BitSize_t offset = 0;

    memset( inOutByteArray, 0, (size_t) BITS_TO_BYTES( numberOfBitsToRead ) );

    while ( numberOfBitsToRead > 0 )
    {
        *( inOutByteArray + offset ) |= *( data + ( readOffset >> 3 ) ) << ( readOffsetMod8 );

        if ( readOffsetMod8 > 0 && numberOfBitsToRead > 8 - ( readOffsetMod8 ) )
            *( inOutByteArray + offset ) |= *( data + ( readOffset >> 3 ) + 1 ) >> ( 8 - ( readOffsetMod8 ) );

        if (numberOfBitsToRead>=8)
        {
            numberOfBitsToRead -= 8;
            readOffset += 8;
            offset++;
        }
        else
        {
            int neg = (int) numberOfBitsToRead - 8;

            if ( neg < 0 )
            {

                if ( alignBitsToRight )
                    * ( inOutByteArray + offset ) >>= -neg;

                readOffset += 8 + neg;
            }
            else
                readOffset += 8;

            offset++;

            numberOfBitsToRead=0;
        }
    }

    return true;
}

bool BitStream::ReadCompressed( unsigned char* inOutByteArray,
                                const unsigned int size, const bool unsignedData )
{
    unsigned int currentByte = ( size >> 3 ) - 1;


    unsigned char byteMatch, halfByteMatch;

    if ( unsignedData )
    {
        byteMatch = 0;
        halfByteMatch = 0;
    }

    else
    {
        byteMatch = 0xFF;
        halfByteMatch = 0xF0;
    }

    while ( currentByte > 0 )
    {
        bool b;

        if ( Read( b ) == false )
            return false;

        if ( b )
        {
            inOutByteArray[ currentByte ] = byteMatch;
            currentByte--;
        }
        else
        {
            if ( ReadBits( inOutByteArray, ( currentByte + 1 ) << 3 ) == false )
                return false;

            return true;
        }
    }

    if ( readOffset + 1 > numberOfBitsUsed )
        return false;

    bool b=false;

    if ( Read( b ) == false )
        return false;

    if ( b )
    {

        if ( ReadBits( inOutByteArray + currentByte, 4 ) == false )
            return false;

        inOutByteArray[ currentByte ] |= halfByteMatch;
    }
    else
    {
        if ( ReadBits( inOutByteArray + currentByte, 8 ) == false )
            return false;
    }

    return true;
}

void BitStream::AddBitsAndReallocate( const BitSize_t numberOfBitsToWrite )
{
    BitSize_t newNumberOfBitsAllocated = numberOfBitsToWrite + numberOfBitsUsed;

    if ( numberOfBitsToWrite + numberOfBitsUsed > 0 && ( ( numberOfBitsAllocated - 1 ) >> 3 ) < ( ( newNumberOfBitsAllocated - 1 ) >> 3 ) )
    {
#ifdef _DEBUG
        VEAssert( copyData == true );
#endif
        newNumberOfBitsAllocated = ( numberOfBitsToWrite + numberOfBitsUsed ) * 2;
        if (newNumberOfBitsAllocated - ( numberOfBitsToWrite + numberOfBitsUsed ) > 1048576 )
            newNumberOfBitsAllocated = numberOfBitsToWrite + numberOfBitsUsed + 1048576;

        BitSize_t amountToAllocate = BITS_TO_BYTES( newNumberOfBitsAllocated );
        if (data==(unsigned char*)stackData)
        {
            if (amountToAllocate > BITSTREAM_STACK_ALLOCATION_SIZE)
            {
                data = ( unsigned char* ) rakMalloc_Ex( (size_t) amountToAllocate, _FILE_AND_LINE_ );
                VEAssert(data);

                memcpy ((void *)data, (void *)stackData, (size_t) BITS_TO_BYTES( numberOfBitsAllocated ));
            }
        }
        else
        {
            data = ( unsigned char* ) rakRealloc_Ex( data, (size_t) amountToAllocate, _FILE_AND_LINE_ );
        }

#ifdef _DEBUG
        VEAssert( data );
#endif
    }

    if ( newNumberOfBitsAllocated > numberOfBitsAllocated )
        numberOfBitsAllocated = newNumberOfBitsAllocated;
}
BitSize_t BitStream::GetNumberOfBitsAllocated(void) const
{
    return numberOfBitsAllocated;
}
void BitStream::PadWithZeroToByteLength( unsigned int bytes )
{
    if (GetNumberOfBytesUsed() < bytes)
    {
        AlignWriteToByteBoundary();
        unsigned int numToWrite = bytes - GetNumberOfBytesUsed();
        AddBitsAndReallocate( BYTES_TO_BITS(numToWrite) );
        memset(data+BITS_TO_BYTES(numberOfBitsUsed), 0, (size_t) numToWrite);
        numberOfBitsUsed+=BYTES_TO_BITS(numToWrite);
    }
}

int BitStream::NumberOfLeadingZeroes( int8_t x )
{
    return NumberOfLeadingZeroes((uint8_t)x);
}
int BitStream::NumberOfLeadingZeroes( uint8_t x )
{
    uint8_t y;
    int n;

    n = 8;
    y = x >> 4;
    if (y != 0)
    {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0)
    {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) return n - 2;
    return (int)(n - x);
}
int BitStream::NumberOfLeadingZeroes( int16_t x )
{
    return NumberOfLeadingZeroes((uint16_t)x);
}
int BitStream::NumberOfLeadingZeroes( uint16_t x )
{
    uint16_t y;
    int n;

    n = 16;
    y = x >> 8;
    if (y != 0)
    {
        n = n - 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0)
    {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0)
    {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) return n - 2;
    return (int)(n - x);
}
int BitStream::NumberOfLeadingZeroes( int32_t x )
{
    return NumberOfLeadingZeroes((uint32_t)x);
}
int BitStream::NumberOfLeadingZeroes( uint32_t x )
{
    uint32_t y;
    int n;

    n = 32;
    y = x >>16;
    if (y != 0)
    {
        n = n -16;
        x = y;
    }
    y = x >> 8;
    if (y != 0)
    {
        n = n - 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0)
    {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0)
    {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) return n - 2;
    return (int)(n - x);
}
int BitStream::NumberOfLeadingZeroes( int64_t x )
{
    return NumberOfLeadingZeroes((uint64_t)x);
}
int BitStream::NumberOfLeadingZeroes( uint64_t x )
{
    uint64_t y;
    int n;

    n = 64;
    y = x >>32;
    if (y != 0)
    {
        n = n -32;
        x = y;
    }
    y = x >>16;
    if (y != 0)
    {
        n = n -16;
        x = y;
    }
    y = x >> 8;
    if (y != 0)
    {
        n = n - 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0)
    {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0)
    {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) return n - 2;
    return (int)(n - x);
}

void BitStream::AssertStreamEmpty( void )
{
    VEAssert( readOffset == numberOfBitsUsed );
}
void BitStream::PrintBits( char *out ) const
{
    if ( numberOfBitsUsed <= 0 )
    {
        strcpy(out, "No bits\n" );
        return;
    }

    unsigned int strIndex=0;
    for ( BitSize_t counter = 0; counter < BITS_TO_BYTES( numberOfBitsUsed ) && strIndex < 2000 ; counter++ )
    {
        BitSize_t stop;

        if ( counter == ( numberOfBitsUsed - 1 ) >> 3 )
            stop = 8 - ( ( ( numberOfBitsUsed - 1 ) & 7 ) + 1 );
        else
            stop = 0;

        for ( BitSize_t counter2 = 7; counter2 >= stop; counter2-- )
        {
            if ( ( data[ counter ] >> counter2 ) & 1 )
                out[strIndex++]='1';
            else
                out[strIndex++]='0';

            if (counter2==0)
                break;
        }

        out[strIndex++]=' ';
    }

    out[strIndex++]='\n';

    out[strIndex++]=0;
}
void BitStream::PrintBits( void ) const
{
    char out[2048];
    PrintBits(out);
    VENET_DEBUG_PRINTF("%s", out);
}
void BitStream::PrintHex( char *out ) const
{
    BitSize_t i;
    for ( i=0; i < GetNumberOfBytesUsed(); i++)
    {
        sprintf(out+i*3, "%02x ", data[i]);
    }
}
void BitStream::PrintHex( void ) const
{
    char out[2048];
    PrintHex(out);
    VENET_DEBUG_PRINTF("%s", out);
}

BitSize_t BitStream::CopyData( unsigned char** _data ) const
{
#ifdef _DEBUG
    VEAssert( numberOfBitsUsed > 0 );
#endif

    *_data = (unsigned char*) rakMalloc_Ex( (size_t) BITS_TO_BYTES( numberOfBitsUsed ), _FILE_AND_LINE_ );
    memcpy( *_data, data, sizeof(unsigned char) * (size_t) ( BITS_TO_BYTES( numberOfBitsUsed ) ) );
    return numberOfBitsUsed;
}

void BitStream::IgnoreBits( const BitSize_t numberOfBits )
{
    readOffset += numberOfBits;
}

void BitStream::IgnoreBytes( const unsigned int numberOfBytes )
{
    IgnoreBits(BYTES_TO_BITS(numberOfBytes));
}

void BitStream::SetWriteOffset( const BitSize_t offset )
{
    numberOfBitsUsed = offset;
}

void BitStream::AssertCopyData( void )
{
    if ( copyData == false )
    {
        copyData = true;

        if ( numberOfBitsAllocated > 0 )
        {
            unsigned char * newdata = ( unsigned char* ) rakMalloc_Ex( (size_t) BITS_TO_BYTES( numberOfBitsAllocated ), _FILE_AND_LINE_ );
#ifdef _DEBUG

            VEAssert( data );
#endif

            memcpy( newdata, data, (size_t) BITS_TO_BYTES( numberOfBitsAllocated ) );
            data = newdata;
        }

        else
            data = 0;
    }
}
bool BitStream::IsNetworkOrderInternal(void)
{
    static const bool isNetworkOrder=(htonl(12345) == 12345);
    return isNetworkOrder;

}
void BitStream::ReverseBytes(unsigned char *inByteArray, unsigned char *inOutByteArray, const unsigned int length)
{
    for (BitSize_t i=0; i < length; i++)
        inOutByteArray[i]=inByteArray[length-i-1];
}
void BitStream::ReverseBytesInPlace(unsigned char *inOutData,const unsigned int length)
{
    unsigned char temp;
    BitSize_t i;
    for (i=0; i < (length>>1); i++)
    {
        temp = inOutData[i];
        inOutData[i]=inOutData[length-i-1];
        inOutData[length-i-1]=temp;
    }
}

bool BitStream::Read(char *varString)
{
    return VEString::Deserialize(varString,this);
}
bool BitStream::Read(unsigned char *varString)
{
    return VEString::Deserialize((char*) varString,this);
}
void BitStream::WriteAlignedVar8(const char *inByteArray)
{
    VEAssert((numberOfBitsUsed&7)==0);
    AddBitsAndReallocate(1*8);
    data[( numberOfBitsUsed >> 3 ) + 0] = inByteArray[0];
    numberOfBitsUsed+=1*8;
}
bool BitStream::ReadAlignedVar8(char *inOutByteArray)
{
    VEAssert((readOffset&7)==0);
    if ( readOffset + 1*8 > numberOfBitsUsed )
        return false;

    inOutByteArray[0] = data[( readOffset >> 3 ) + 0];
    readOffset+=1*8;
    return true;
}
void BitStream::WriteAlignedVar16(const char *inByteArray)
{
    VEAssert((numberOfBitsUsed&7)==0);
    AddBitsAndReallocate(2*8);
#ifndef __BITSTREAM_NATIVE_END
    if (DoEndianSwap())
    {
        data[( numberOfBitsUsed >> 3 ) + 0] = inByteArray[1];
        data[( numberOfBitsUsed >> 3 ) + 1] = inByteArray[0];
    }
    else
#endif
    {
        data[( numberOfBitsUsed >> 3 ) + 0] = inByteArray[0];
        data[( numberOfBitsUsed >> 3 ) + 1] = inByteArray[1];
    }

    numberOfBitsUsed+=2*8;
}
bool BitStream::ReadAlignedVar16(char *inOutByteArray)
{
    VEAssert((readOffset&7)==0);
    if ( readOffset + 2*8 > numberOfBitsUsed )
        return false;
#ifndef __BITSTREAM_NATIVE_END
    if (DoEndianSwap())
    {
        inOutByteArray[0] = data[( readOffset >> 3 ) + 1];
        inOutByteArray[1] = data[( readOffset >> 3 ) + 0];
    }
    else
#endif
    {
        inOutByteArray[0] = data[( readOffset >> 3 ) + 0];
        inOutByteArray[1] = data[( readOffset >> 3 ) + 1];
    }

    readOffset+=2*8;
    return true;
}
void BitStream::WriteAlignedVar32(const char *inByteArray)
{
    VEAssert((numberOfBitsUsed&7)==0);
    AddBitsAndReallocate(4*8);
#ifndef __BITSTREAM_NATIVE_END
    if (DoEndianSwap())
    {
        data[( numberOfBitsUsed >> 3 ) + 0] = inByteArray[3];
        data[( numberOfBitsUsed >> 3 ) + 1] = inByteArray[2];
        data[( numberOfBitsUsed >> 3 ) + 2] = inByteArray[1];
        data[( numberOfBitsUsed >> 3 ) + 3] = inByteArray[0];
    }
    else
#endif
    {
        data[( numberOfBitsUsed >> 3 ) + 0] = inByteArray[0];
        data[( numberOfBitsUsed >> 3 ) + 1] = inByteArray[1];
        data[( numberOfBitsUsed >> 3 ) + 2] = inByteArray[2];
        data[( numberOfBitsUsed >> 3 ) + 3] = inByteArray[3];
    }

    numberOfBitsUsed+=4*8;
}
bool BitStream::ReadAlignedVar32(char *inOutByteArray)
{
    VEAssert((readOffset&7)==0);
    if ( readOffset + 4*8 > numberOfBitsUsed )
        return false;
#ifndef __BITSTREAM_NATIVE_END
    if (DoEndianSwap())
    {
        inOutByteArray[0] = data[( readOffset >> 3 ) + 3];
        inOutByteArray[1] = data[( readOffset >> 3 ) + 2];
        inOutByteArray[2] = data[( readOffset >> 3 ) + 1];
        inOutByteArray[3] = data[( readOffset >> 3 ) + 0];
    }
    else
#endif
    {
        inOutByteArray[0] = data[( readOffset >> 3 ) + 0];
        inOutByteArray[1] = data[( readOffset >> 3 ) + 1];
        inOutByteArray[2] = data[( readOffset >> 3 ) + 2];
        inOutByteArray[3] = data[( readOffset >> 3 ) + 3];
    }

    readOffset+=4*8;
    return true;
}
bool BitStream::ReadFloat16( float &outFloat, float floatMin, float floatMax )
{
    unsigned short percentile;
    if (Read(percentile))
    {
        VEAssert(floatMax>floatMin);
        outFloat = floatMin + ((float) percentile / 65535.0f) * (floatMax-floatMin);
        if (outFloat<floatMin)
            outFloat=floatMin;
        else if (outFloat>floatMax)
            outFloat=floatMax;
        return true;
    }
    return false;
}
bool BitStream::SerializeFloat16(bool writeToBitstream, float &inOutFloat, float floatMin, float floatMax)
{
    if (writeToBitstream)
        WriteFloat16(inOutFloat, floatMin, floatMax);
    else
        return ReadFloat16(inOutFloat, floatMin, floatMax);
    return true;
}
void BitStream::WriteFloat16( float inOutFloat, float floatMin, float floatMax )
{
    VEAssert(floatMax>floatMin);
    if (inOutFloat>floatMax+.001)
    {
        VEAssert(inOutFloat<=floatMax+.001);
    }
    if (inOutFloat<floatMin-.001)
    {
        VEAssert(inOutFloat>=floatMin-.001);
    }
    float percentile=65535.0f * (inOutFloat-floatMin)/(floatMax-floatMin);
    if (percentile<0.0)
        percentile=0.0;
    if (percentile>65535.0f)
        percentile=65535.0f;
    Write((unsigned short)percentile);
}


#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif

