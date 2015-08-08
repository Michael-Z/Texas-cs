#include "VEWString.h"
#include "BitStream.h"
#include <string.h>
#include <wchar.h>
#include <stdlib.h>

using namespace VENet;

#define MAX_BYTES_PER_UNICODE_CHAR sizeof(wchar_t)

VEWString::VEWString()
{
    c_str=0;
    c_strCharLength=0;
}
VEWString::VEWString( const VEString &right )
{
    c_str=0;
    c_strCharLength=0;
    *this=right;
}
VEWString::VEWString( const char *input )
{
    c_str=0;
    c_strCharLength=0;
    *this = input;
}
VEWString::VEWString( const wchar_t *input )
{
    c_str=0;
    c_strCharLength=0;
    *this = input;
}
VEWString::VEWString( const VEWString & right)
{
    c_str=0;
    c_strCharLength=0;
    *this = right;
}
VEWString::~VEWString()
{
    veFree_Ex(c_str,_FILE_AND_LINE_);
}
VEWString& VEWString::operator = ( const VEWString& right )
{
    Clear();
    if (right.IsEmpty())
        return *this;
    c_str = (wchar_t *) rakMalloc_Ex( (right.GetLength() + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    if (!c_str)
    {
        c_strCharLength=0;
        notifyOutOfMemory(_FILE_AND_LINE_);
        return *this;
    }
    c_strCharLength = right.GetLength();
    memcpy(c_str,right.C_String(),(right.GetLength() + 1) * MAX_BYTES_PER_UNICODE_CHAR);

    return *this;
}
VEWString& VEWString::operator = ( const VEString& right )
{
    return *this = right.C_String();
}
VEWString& VEWString::operator = ( const wchar_t * const str )
{
    Clear();
    if (str==0)
        return *this;
    c_strCharLength = wcslen(str);
    if (c_strCharLength==0)
        return *this;
    c_str = (wchar_t *) rakMalloc_Ex( (c_strCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    if (!c_str)
    {
        c_strCharLength=0;
        notifyOutOfMemory(_FILE_AND_LINE_);
        return *this;
    }
    wcscpy(c_str,str);

    return *this;
}
VEWString& VEWString::operator = ( wchar_t *str )
{
    *this = ( const wchar_t * const) str;
    return *this;
}
VEWString& VEWString::operator = ( const char * const str )
{
    Clear();

#if !defined(ANDROID)
    if (str==0)
        return *this;
    if (str[0]==0)
        return *this;

    c_strCharLength = mbstowcs(NULL, str, 0);
    c_str = (wchar_t *) rakMalloc_Ex( (c_strCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    if (!c_str)
    {
        c_strCharLength=0;
        notifyOutOfMemory(_FILE_AND_LINE_);
        return *this;
    }

    c_strCharLength = mbstowcs(c_str, str, c_strCharLength+1);
    if (c_strCharLength == (size_t) (-1))
    {
        VENET_DEBUG_PRINTF("Couldn't convert string--invalid multibyte character.\n");
        Clear();
        return *this;
    }
#else
    VEAssert("mbstowcs not supported on Android" && 0);
#endif // defined(ANDROID)

    return *this;
}
VEWString& VEWString::operator = ( char *str )
{
    *this = ( const char * const) str;
    return *this;
}
VEWString& VEWString::operator +=( const VEWString& right)
{
    if (right.IsEmpty())
        return *this;
    size_t newCharLength = c_strCharLength + right.GetLength();
    wchar_t *newCStr;
    bool isEmpty = IsEmpty();
    if (isEmpty)
        newCStr = (wchar_t *) rakMalloc_Ex( (newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    else
        newCStr = (wchar_t *) rakRealloc_Ex( c_str, (newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    if (!newCStr)
    {
        notifyOutOfMemory(_FILE_AND_LINE_);
        return *this;
    }
    c_str = newCStr;
    c_strCharLength = newCharLength;
    if (isEmpty)
    {
        memcpy(newCStr,right.C_String(),(right.GetLength() + 1) * MAX_BYTES_PER_UNICODE_CHAR);
    }
    else
    {
        wcscat(c_str, right.C_String());
    }

    return *this;
}
VEWString& VEWString::operator += ( const wchar_t * const right )
{
    if (right==0)
        return *this;
    size_t rightLength = wcslen(right);
    size_t newCharLength = c_strCharLength + rightLength;
    wchar_t *newCStr;
    bool isEmpty = IsEmpty();
    if (isEmpty)
        newCStr = (wchar_t *) rakMalloc_Ex( (newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    else
        newCStr = (wchar_t *) rakRealloc_Ex( c_str, (newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
    if (!newCStr)
    {
        notifyOutOfMemory(_FILE_AND_LINE_);
        return *this;
    }
    c_str = newCStr;
    c_strCharLength = newCharLength;
    if (isEmpty)
    {
        memcpy(newCStr,right,(rightLength + 1) * MAX_BYTES_PER_UNICODE_CHAR);
    }
    else
    {
        wcscat(c_str, right);
    }

    return *this;
}
VEWString& VEWString::operator += ( wchar_t *right )
{
    return *this += (const wchar_t * const) right;
}
bool VEWString::operator==(const VEWString &right) const
{
    if (GetLength()!=right.GetLength())
        return false;
    return wcscmp(c_str,right.C_String())==0;
}
bool VEWString::operator < ( const VEWString& right ) const
{
    return wcscmp(c_str,right.C_String())<0;
}
bool VEWString::operator <= ( const VEWString& right ) const
{
    return wcscmp(c_str,right.C_String())<=0;
}
bool VEWString::operator > ( const VEWString& right ) const
{
    return wcscmp(c_str,right.C_String())>0;
}
bool VEWString::operator >= ( const VEWString& right ) const
{
    return wcscmp(c_str,right.C_String())>=0;
}
bool VEWString::operator!=(const VEWString &right) const
{
    if (GetLength()!=right.GetLength())
        return true;
    return wcscmp(c_str,right.C_String())!=0;
}
void VEWString::Set( wchar_t *str )
{
    *this = str;
}
bool VEWString::IsEmpty(void) const
{
    return GetLength()==0;
}
size_t VEWString::GetLength(void) const
{
    return c_strCharLength;
}
unsigned long VEWString::ToInteger(const VEWString &rs)
{
    unsigned long hash = 0;
    int c;

    const char *str = (const char *)rs.C_String();
    size_t i;
    for (i=0; i < rs.GetLength()*MAX_BYTES_PER_UNICODE_CHAR*sizeof(wchar_t); i++)
    {
        c = *str++;
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}
int VEWString::StrCmp(const VEWString &right) const
{
    return wcscmp(C_String(), right.C_String());
}
int VEWString::StrICmp(const VEWString &right) const
{
#ifdef _WIN32
    return _wcsicmp(C_String(), right.C_String());
#else
    return wcscmp(C_String(), right.C_String());
#endif
}
void VEWString::Clear(void)
{
    veFree_Ex(c_str,_FILE_AND_LINE_);
    c_str=0;
    c_strCharLength=0;
}
void VEWString::Printf(void)
{
    printf("%ls", C_String());
}
void VEWString::FPrintf(FILE *fp)
{
    fprintf(fp,"%ls", C_String());
}
void VEWString::Serialize(BitStream *bs) const
{
    Serialize(c_str, bs);
}
void VEWString::Serialize(const wchar_t * const str, BitStream *bs)
{
#if 0
    char *multiByteBuffer;
    size_t allocated = wcslen(str)*MAX_BYTES_PER_UNICODE_CHAR;
    multiByteBuffer = (char*) rakMalloc_Ex(allocated, _FILE_AND_LINE_);
    size_t used = wcstombs(multiByteBuffer, str, allocated);
    bs->WriteCasted<unsigned short>(used);
    bs->WriteAlignedBytes((const unsigned char*) multiByteBuffer,(const unsigned int) used);
    rakFree_Ex(multiByteBuffer, _FILE_AND_LINE_);
#else
    size_t mbByteLength = wcslen(str);
    bs->WriteCasted<unsigned short>(mbByteLength);
    for (unsigned int i=0; i < mbByteLength; i++)
    {
        uint16_t t;
        t = (uint16_t) str[i];
        bs->Write(t);
    }
#endif
}
bool VEWString::Deserialize(BitStream *bs)
{
    Clear();

    size_t mbByteLength;
    bs->ReadCasted<unsigned short>(mbByteLength);
    if (mbByteLength>0)
    {
#if 0
        char *multiByteBuffer;
        multiByteBuffer = (char*) rakMalloc_Ex(mbByteLength+1, _FILE_AND_LINE_);
        bool result = bs->ReadAlignedBytes((unsigned char*) multiByteBuffer,(const unsigned int) mbByteLength);
        if (result==false)
        {
            rakFree_Ex(multiByteBuffer, _FILE_AND_LINE_);
            return false;
        }
        multiByteBuffer[mbByteLength]=0;
        c_str = (wchar_t *) rakMalloc_Ex( (mbByteLength + 1) * MAX_BYTES_PER_UNICODE_CHAR, _FILE_AND_LINE_);
        c_strCharLength = mbstowcs(c_str, multiByteBuffer, mbByteLength);
        rakFree_Ex(multiByteBuffer, _FILE_AND_LINE_);
        c_str[c_strCharLength]=0;
#else
        c_str = (wchar_t*) rakMalloc_Ex(mbByteLength+1, _FILE_AND_LINE_);
        c_strCharLength = mbByteLength;
        for (unsigned int i=0; i < mbByteLength; i++)
        {
            uint16_t t;
            bs->Read(t);
            c_str[i]=t;
        }
        c_str[mbByteLength]=0;
#endif
        return true;
    }
    else
    {
        return true;
    }
}
bool VEWString::Deserialize(wchar_t *str, BitStream *bs)
{
    size_t mbByteLength;
    bs->ReadCasted<unsigned short>(mbByteLength);
    if (mbByteLength>0)
    {
#if 0
        char *multiByteBuffer;
        multiByteBuffer = (char*) rakMalloc_Ex(mbByteLength+1, _FILE_AND_LINE_);
        bool result = bs->ReadAlignedBytes((unsigned char*) multiByteBuffer,(const unsigned int) mbByteLength);
        if (result==false)
        {
            rakFree_Ex(multiByteBuffer, _FILE_AND_LINE_);
            return false;
        }
        multiByteBuffer[mbByteLength]=0;
        size_t c_strCharLength = mbstowcs(str, multiByteBuffer, mbByteLength);
        rakFree_Ex(multiByteBuffer, _FILE_AND_LINE_);
        str[c_strCharLength]=0;
#else
        for (unsigned int i=0; i < mbByteLength; i++)
        {
            uint16_t t;
            // Force endian swapping, and read 16 bits
            bs->Read(t);
            str[i]=t;
        }
        str[mbByteLength]=0;
#endif
        return true;
    }
    else
    {
        wcscpy(str,L"");
    }
    return true;
}