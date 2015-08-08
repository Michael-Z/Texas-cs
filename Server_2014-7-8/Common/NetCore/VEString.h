////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 RakString.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Export.h"
#include "DS_List.h"
#include "VENetTypes.h" // int64_t
#include <stdio.h>
#include "stdarg.h"


#ifdef _WIN32



#include "WindowsIncludes.h"
#endif

namespace VENet
{
class SimpleMutex;
class BitStream;

class VE_DLL_EXPORT VEString
{
public:
    VEString();
    VEString(char input);
    VEString(unsigned char input);
    VEString(const unsigned char *format, ...);
    VEString(const char *format, ...);
    ~VEString();
    VEString( const VEString & rhs);

    operator const char* () const
    {
        return sharedString->c_str;
    }

    const char *C_String(void) const
    {
        return sharedString->c_str;
    }

    char *C_StringUnsafe(void)
    {
        Clone();
        return sharedString->c_str;
    }

    VEString& operator = ( const VEString& rhs );
    VEString& operator = ( const char *str );
    VEString& operator = ( char *str );
    VEString& operator = ( const unsigned char *str );
    VEString& operator = ( char unsigned *str );
    VEString& operator = ( const char c );

    VEString& operator +=( const VEString& rhs);
    VEString& operator += ( const char *str );
    VEString& operator += ( char *str );
    VEString& operator += ( const unsigned char *str );
    VEString& operator += ( char unsigned *str );
    VEString& operator += ( const char c );

    unsigned char operator[] ( const unsigned int position ) const;

#ifdef _WIN32
    WCHAR * ToWideChar(void);
    void DeallocWideChar(WCHAR * w);
#endif

    size_t Find(const char *stringToFind,size_t pos = 0 );

    bool operator==(const VEString &rhs) const;
    bool operator==(const char *str) const;
    bool operator==(char *str) const;

    bool operator < ( const VEString& right ) const;
    bool operator <= ( const VEString& right ) const;
    bool operator > ( const VEString& right ) const;
    bool operator >= ( const VEString& right ) const;

    bool operator!=(const VEString &rhs) const;
    bool operator!=(const char *str) const;
    bool operator!=(char *str) const;

    const char * ToLower(void);

    const char * ToUpper(void);

    void Set(const char *format, ...);

    VEString Assign(const char *str,size_t pos, size_t n );

    bool IsEmpty(void) const;

    size_t GetLength(void) const;
    size_t GetLengthUTF8(void) const;

    void Replace(unsigned index, unsigned count, unsigned char c);

    void SetChar( unsigned index, unsigned char c );

    void SetChar( unsigned index, VENet::VEString s );

    void Truncate(unsigned int length);
    void TruncateUTF8(unsigned int length);

    VEString SubStr(unsigned int index, unsigned int count) const;

    void Erase(unsigned int index, unsigned int count);

    void TerminateAtFirstCharacter(char c);
    void TerminateAtLastCharacter(char c);

    int GetCharacterCount(char c);

    void RemoveCharacter(char c);

    static VENet::VEString NonVariadic(const char *str);

    static unsigned long ToInteger(const char *str);
    static unsigned long ToInteger(const VEString &rs);

    void AppendBytes(const char *bytes, unsigned int count);

    int StrCmp(const VEString &rhs) const;

    int StrNCmp(const VEString &rhs, size_t num) const;

    int StrICmp(const VEString &rhs) const;

    void Clear(void);

    void Printf(void);

    void FPrintf(FILE *fp);

    bool IPAddressMatch(const char *IP);

    bool ContainsNonprintableExceptSpaces(void) const;

    bool IsEmailAddress(void) const;

    VENet::VEString& URLEncode(void);

    VENet::VEString& URLDecode(void);

    void SplitURI(VENet::VEString &header, VENet::VEString &domain, VENet::VEString &path);

    VENet::VEString& SQLEscape(void);

    static VENet::VEString FormatForPOST(VEString &uri, VEString &contentType, unsigned int port, VEString &body);

    static VENet::VEString FormatForGET(VEString &uri, unsigned int port);

    VENet::VEString& MakeFilePath(void);

    static void FreeMemory(void);
    static void FreeMemoryNoMutex(void);

    void Serialize(BitStream *bs) const;

    static void Serialize(const char *str, BitStream *bs);

    void SerializeCompressed(BitStream *bs, uint8_t languageId=0, bool writeLanguageId=false) const;

    static void SerializeCompressed(const char *str, BitStream *bs, uint8_t languageId=0, bool writeLanguageId=false);

    bool Deserialize(BitStream *bs);

    static bool Deserialize(char *str, BitStream *bs);

    bool DeserializeCompressed(BitStream *bs, bool readLanguageId=false);

    static bool DeserializeCompressed(char *str, BitStream *bs, bool readLanguageId=false);

    static const char *ToString(int64_t i);
    static const char *ToString(uint64_t i);

    static size_t GetSizeToAllocate(size_t bytes)
    {
        const size_t smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
        if (bytes<=smallStringSize)
            return smallStringSize;
        else
            return bytes*2;
    }

    struct SharedString
    {
        SimpleMutex *refCountMutex;
        unsigned int refCount;
        size_t bytesUsed;
        char *bigString;
        char *c_str;
        char smallString[128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2];
    };

    VEString( SharedString *_sharedString );

    SharedString *sharedString;

    static SharedString emptyString;

    static DataStructures::List<SharedString*> freeList;

    static int RakStringComp( VEString const &key, VEString const &data );

    static void LockMutex(void);
    static void UnlockMutex(void);

protected:
    void Allocate(size_t len);
    void Assign(const char *str);
    void Assign(const char *str, va_list ap);

    void Clone(void);
    void Free(void);
    unsigned char ToLower(unsigned char c);
    unsigned char ToUpper(unsigned char c);
    void Realloc(SharedString *sharedString, size_t bytes);
};

}

const VENet::VEString VE_DLL_EXPORT operator+(const VENet::VEString &lhs, const VENet::VEString &rhs);