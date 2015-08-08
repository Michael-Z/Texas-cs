#include "VEString.h"
#include "VEAssert.h"
#include "VEMemoryOverride.h"
#include "BitStream.h"
#include <stdarg.h>
#include <string.h>
#include "LinuxStrings.h"
#include "StringCompressor.h"
#include "SimpleMutex.h"

using namespace VENet;

VEString::SharedString VEString::emptyString={0,0,0,(char*) "",(char*) ""};
DataStructures::List<VEString::SharedString*> VEString::freeList;

class RakStringCleanup
{
public:
    ~RakStringCleanup()
    {
        VENet::VEString::FreeMemoryNoMutex();
    }
};

static RakStringCleanup cleanup;

SimpleMutex& GetPoolMutex(void)
{
    static SimpleMutex poolMutex;
    return poolMutex;
}

int VENet::VEString::RakStringComp( VEString const &key, VEString const &data )
{
    return key.StrCmp(data);
}

VEString::VEString()
{
    sharedString=&emptyString;
}
VEString::VEString( VEString::SharedString *_sharedString )
{
    sharedString=_sharedString;
}
VEString::VEString(char input)
{
    char str[2];
    str[0]=input;
    str[1]=0;
    Assign(str);
}
VEString::VEString(unsigned char input)
{
    char str[2];
    str[0]=(char) input;
    str[1]=0;
    Assign(str);
}
VEString::VEString(const unsigned char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    Assign((const char*) format,ap);
}
VEString::VEString(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    Assign(format,ap);
}
VEString::VEString( const VEString & rhs)
{
    if (rhs.sharedString==&emptyString)
    {
        sharedString=&emptyString;
        return;
    }

    rhs.sharedString->refCountMutex->Lock();
    if (rhs.sharedString->refCount==0)
    {
        sharedString=&emptyString;
    }
    else
    {
        rhs.sharedString->refCount++;
        sharedString=rhs.sharedString;
    }
    rhs.sharedString->refCountMutex->Unlock();
}
VEString::~VEString()
{
    Free();
}
VEString& VEString::operator = ( const VEString& rhs )
{
    Free();
    if (rhs.sharedString==&emptyString)
        return *this;

    rhs.sharedString->refCountMutex->Lock();
    if (rhs.sharedString->refCount==0)
    {
        sharedString=&emptyString;
    }
    else
    {
        sharedString=rhs.sharedString;
        sharedString->refCount++;
    }
    rhs.sharedString->refCountMutex->Unlock();
    return *this;
}
VEString& VEString::operator = ( const char *str )
{
    Free();
    Assign(str);
    return *this;
}
VEString& VEString::operator = ( char *str )
{
    return operator = ((const char*)str);
}
VEString& VEString::operator = ( const unsigned char *str )
{
    return operator = ((const char*)str);
}
VEString& VEString::operator = ( char unsigned *str )
{
    return operator = ((const char*)str);
}
VEString& VEString::operator = ( const char c )
{
    char buff[2];
    buff[0]=c;
    buff[1]=0;
    return operator = ((const char*)buff);
}
void VEString::Realloc(SharedString *sharedString, size_t bytes)
{
    if (bytes<=sharedString->bytesUsed)
        return;
    VEAssert(bytes>0);
    size_t oldBytes = sharedString->bytesUsed;
    size_t newBytes;
    const size_t smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
    newBytes = GetSizeToAllocate(bytes);
    if (oldBytes <=(size_t) smallStringSize && newBytes > (size_t) smallStringSize)
    {
        sharedString->bigString=(char*) rakMalloc_Ex(newBytes, _FILE_AND_LINE_);
        strcpy(sharedString->bigString, sharedString->smallString);
        sharedString->c_str=sharedString->bigString;
    }
    else if (oldBytes > smallStringSize)
    {
        sharedString->bigString=(char*) rakRealloc_Ex(sharedString->bigString,newBytes, _FILE_AND_LINE_);
        sharedString->c_str=sharedString->bigString;
    }
    sharedString->bytesUsed=newBytes;
}
VEString& VEString::operator +=( const VEString& rhs)
{
    if (rhs.IsEmpty())
        return *this;

    if (IsEmpty())
    {
        return operator=(rhs);
    }
    else
    {
        Clone();
        size_t strLen=rhs.GetLength()+GetLength()+1;
        Realloc(sharedString, strLen+GetLength());
        strcat(sharedString->c_str,rhs.C_String());
    }
    return *this;
}
VEString& VEString::operator +=( const char *str )
{
    if (str==0 || str[0]==0)
        return *this;

    if (IsEmpty())
    {
        Assign(str);
    }
    else
    {
        Clone();
        size_t strLen=strlen(str)+GetLength()+1;
        Realloc(sharedString, strLen);
        strcat(sharedString->c_str,str);
    }
    return *this;
}
VEString& VEString::operator +=( char *str )
{
    return operator += ((const char*)str);
}
VEString& VEString::operator +=( const unsigned char *str )
{
    return operator += ((const char*)str);
}
VEString& VEString::operator +=( unsigned char *str )
{
    return operator += ((const char*)str);
}
VEString& VEString::operator +=( const char c )
{
    char buff[2];
    buff[0]=c;
    buff[1]=0;
    return operator += ((const char*)buff);
}
unsigned char VEString::operator[] ( const unsigned int position ) const
{
    VEAssert(position<GetLength());
    return sharedString->c_str[position];
}
bool VEString::operator==(const VEString &rhs) const
{
    return strcmp(sharedString->c_str,rhs.sharedString->c_str)==0;
}
bool VEString::operator==(const char *str) const
{
    return strcmp(sharedString->c_str,str)==0;
}
bool VEString::operator==(char *str) const
{
    return strcmp(sharedString->c_str,str)==0;
}
bool VEString::operator < ( const VEString& right ) const
{
    return strcmp(sharedString->c_str,right.C_String()) < 0;
}
bool VEString::operator <= ( const VEString& right ) const
{
    return strcmp(sharedString->c_str,right.C_String()) <= 0;
}
bool VEString::operator > ( const VEString& right ) const
{
    return strcmp(sharedString->c_str,right.C_String()) > 0;
}
bool VEString::operator >= ( const VEString& right ) const
{
    return strcmp(sharedString->c_str,right.C_String()) >= 0;
}
bool VEString::operator!=(const VEString &rhs) const
{
    return strcmp(sharedString->c_str,rhs.sharedString->c_str)!=0;
}
bool VEString::operator!=(const char *str) const
{
    return strcmp(sharedString->c_str,str)!=0;
}
bool VEString::operator!=(char *str) const
{
    return strcmp(sharedString->c_str,str)!=0;
}
const VENet::VEString operator+(const VENet::VEString &lhs, const VENet::VEString &rhs)
{
    if (lhs.IsEmpty() && rhs.IsEmpty())
    {
        return VEString(&VEString::emptyString);
    }
    if (lhs.IsEmpty())
    {
        rhs.sharedString->refCountMutex->Lock();
        if (rhs.sharedString->refCount==0)
        {
            rhs.sharedString->refCountMutex->Unlock();
            lhs.sharedString->refCountMutex->Lock();
            lhs.sharedString->refCount++;
            lhs.sharedString->refCountMutex->Unlock();
            return VEString(lhs.sharedString);
        }
        else
        {
            rhs.sharedString->refCount++;
            rhs.sharedString->refCountMutex->Unlock();
            return VEString(rhs.sharedString);
        }
    }
    if (rhs.IsEmpty())
    {
        lhs.sharedString->refCountMutex->Lock();
        lhs.sharedString->refCount++;
        lhs.sharedString->refCountMutex->Unlock();
        return VEString(lhs.sharedString);
    }

    size_t len1 = lhs.GetLength();
    size_t len2 = rhs.GetLength();
    size_t allocatedBytes = len1 + len2 + 1;
    allocatedBytes = VEString::GetSizeToAllocate(allocatedBytes);
    VEString::SharedString *sharedString;

    VEString::LockMutex();
    if (VEString::freeList.Size()==0)
    {
        unsigned i;
        for (i=0; i < 128; i++)
        {
            VEString::SharedString *ss;
            ss = (VEString::SharedString*) rakMalloc_Ex(sizeof(VEString::SharedString), _FILE_AND_LINE_);
            ss->refCountMutex=VENet::OP_NEW<SimpleMutex>(_FILE_AND_LINE_);
            VEString::freeList.Insert(ss, _FILE_AND_LINE_);

        }
    }
    sharedString = VEString::freeList[VEString::freeList.Size()-1];
    VEString::freeList.RemoveAtIndex(VEString::freeList.Size()-1);
    VEString::UnlockMutex();

    const int smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
    sharedString->bytesUsed=allocatedBytes;
    sharedString->refCount=1;
    if (allocatedBytes <= (size_t) smallStringSize)
    {
        sharedString->c_str=sharedString->smallString;
    }
    else
    {
        sharedString->bigString=(char*)rakMalloc_Ex(sharedString->bytesUsed, _FILE_AND_LINE_);
        sharedString->c_str=sharedString->bigString;
    }

    strcpy(sharedString->c_str, lhs);
    strcat(sharedString->c_str, rhs);

    return VEString(sharedString);
}
const char * VEString::ToLower(void)
{
    Clone();

    size_t strLen = strlen(sharedString->c_str);
    unsigned i;
    for (i=0; i < strLen; i++)
        sharedString->c_str[i]=ToLower(sharedString->c_str[i]);
    return sharedString->c_str;
}
const char * VEString::ToUpper(void)
{
    Clone();

    size_t strLen = strlen(sharedString->c_str);
    unsigned i;
    for (i=0; i < strLen; i++)
        sharedString->c_str[i]=ToUpper(sharedString->c_str[i]);
    return sharedString->c_str;
}
void VEString::Set(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    Clear();
    Assign(format,ap);
}
bool VEString::IsEmpty(void) const
{
    return sharedString==&emptyString;
}
size_t VEString::GetLength(void) const
{
    return strlen(sharedString->c_str);
}
int porges_strlen2(char *s)
{
    int i = 0;
    int iBefore = 0;
    int count = 0;

    while (s[i] > 0)
ascii:
        i++;

    count += i-iBefore;
    while (s[i])
    {
        if (s[i] > 0)
        {
            iBefore = i;
            goto ascii;
        }
        else
            switch (0xF0 & s[i])
            {
            case 0xE0:
                i += 3;
                break;
            case 0xF0:
                i += 4;
                break;
            default:
                i += 2;
                break;
            }
        ++count;
    }
    return count;
}
size_t VEString::GetLengthUTF8(void) const
{
    return porges_strlen2(sharedString->c_str);
}
void VEString::Replace(unsigned index, unsigned count, unsigned char c)
{
    VEAssert(index+count < GetLength());
    Clone();
    unsigned countIndex=0;
    while (countIndex<count)
    {
        sharedString->c_str[index]=c;
        index++;
        countIndex++;
    }
}
void VEString::SetChar( unsigned index, unsigned char c )
{
    VEAssert(index < GetLength());
    Clone();
    sharedString->c_str[index]=c;
}
void VEString::SetChar( unsigned index, VENet::VEString s )
{
    VEAssert(index < GetLength());
    Clone();
    VENet::VEString firstHalf = SubStr(0, index);
    VENet::VEString secondHalf = SubStr(index+1, (unsigned int)-1);
    *this = firstHalf;
    *this += s;
    *this += secondHalf;
}

#ifdef _WIN32
WCHAR * VEString::ToWideChar(void)
{
    if ( (sharedString->c_str == NULL) || (*sharedString->c_str == '\0') )
    {
        return L"";
    }

    int cchUTF16 = ::MultiByteToWideChar(
                       CP_UTF8,                // convert from UTF-8
                       0,						// Flags
                       sharedString->c_str,            // source UTF-8 string
                       GetLength()+1,                 // total length of source UTF-8 string,
                       // in CHAR's (= bytes), including end-of-string \0
                       NULL,                   // unused - no conversion done in this step
                       0                       // request size of destination buffer, in WCHAR's
                   );

    if ( cchUTF16 == 0 )
    {
        VEAssert("RakString::ToWideChar exception from cchUTF16==0" && 0);
        return 0;
    }

    WCHAR * pszUTF16 = VENet::OP_NEW_ARRAY<WCHAR>(cchUTF16,__FILE__,__LINE__);

    int result = ::MultiByteToWideChar(
                     CP_UTF8,                // convert from UTF-8
                     0,						// Buffer
                     sharedString->c_str,            // source UTF-8 string
                     GetLength()+1,                 // total length of source UTF-8 string,
                     // in CHAR's (= bytes), including end-of-string \0
                     pszUTF16,               // destination buffer
                     cchUTF16                // size of destination buffer, in WCHAR's
                 );

    if ( result == 0 )
    {
        VEAssert("RakString::ToWideChar exception from MultiByteToWideChar" && 0);
        return 0;
    }

    return pszUTF16;
}
void VEString::DeallocWideChar(WCHAR * w)
{
    VENet::OP_DELETE_ARRAY(w,__FILE__,__LINE__);
}
#endif

size_t VEString::Find(const char *stringToFind,size_t pos)
{
    size_t len=GetLength();
    if (pos>=len || stringToFind==0 || stringToFind[0]==0)
    {
        return (size_t) -1;
    }
    size_t matchLen= strlen(stringToFind);
    size_t matchPos=0;
    size_t iStart=0;

    for (size_t i=pos; i<len; i++)
    {
        if (stringToFind[matchPos]==sharedString->c_str[i])
        {
            if (matchPos==0)
            {
                iStart=i;
            }
            matchPos++;
        }
        else
        {
            matchPos=0;
        }

        if (matchPos>=matchLen)
        {
            return iStart;
        }
    }

    return (size_t) -1;
}

void VEString::TruncateUTF8(unsigned int length)
{
    int i = 0;
    unsigned int count = 0;

    while (sharedString->c_str[i]!=0)
    {
        if (count==length)
        {
            sharedString->c_str[i]=0;
            return;
        }
        else if (sharedString->c_str[i]>0)
        {
            i++;
        }
        else
        {
            switch (0xF0 & sharedString->c_str[i])
            {
            case 0xE0:
                i += 3;
                break;
            case 0xF0:
                i += 4;
                break;
            default:
                i += 2;
                break;
            }
        }

        count++;
    }
}

void VEString::Truncate(unsigned int length)
{
    if (length < GetLength())
    {
        SetChar(length, 0);
    }
}

VEString VEString::SubStr(unsigned int index, unsigned int count) const
{
    size_t length = GetLength();
    if (index >= length || count==0)
        return VEString();
    VEString copy;
    size_t numBytes = length-index;
    if (count < numBytes)
        numBytes=count;
    copy.Allocate(numBytes+1);
    size_t i;
    for (i=0; i < numBytes; i++)
        copy.sharedString->c_str[i]=sharedString->c_str[index+i];
    copy.sharedString->c_str[i]=0;
    return copy;
}
void VEString::Erase(unsigned int index, unsigned int count)
{
    size_t len = GetLength();
    VEAssert(index+count <= len);

    Clone();
    unsigned i;
    for (i=index; i < len-count; i++)
    {
        sharedString->c_str[i]=sharedString->c_str[i+count];
    }
    sharedString->c_str[i]=0;
}
void VEString::TerminateAtLastCharacter(char c)
{
    int i, len=(int) GetLength();
    for (i=len-1; i >= 0; i--)
    {
        if (sharedString->c_str[i]==c)
        {
            Clone();
            sharedString->c_str[i]=0;
            return;
        }
    }
}
void VEString::TerminateAtFirstCharacter(char c)
{
    unsigned int i, len=(unsigned int) GetLength();
    for (i=0; i < len; i++)
    {
        if (sharedString->c_str[i]==c)
        {
            Clone();
            sharedString->c_str[i]=0;
            return;
        }
    }
}
int VEString::GetCharacterCount(char c)
{
    int count=0;
    unsigned int i, len=(unsigned int) GetLength();
    for (i=0; i < len; i++)
    {
        if (sharedString->c_str[i]==c)
        {
            ++count;
        }
    }
    return count;
}
void VEString::RemoveCharacter(char c)
{
    if (c==0)
        return;

    unsigned int readIndex, writeIndex=0;
    for (readIndex=0; sharedString->c_str[readIndex]; readIndex++)
    {
        if (sharedString->c_str[readIndex]!=c)
            sharedString->c_str[writeIndex++]=sharedString->c_str[readIndex];
        else
            Clone();
    }
    sharedString->c_str[writeIndex]=0;
}
int VEString::StrCmp(const VEString &rhs) const
{
    return strcmp(sharedString->c_str, rhs.C_String());
}
int VEString::StrNCmp(const VEString &rhs, size_t num) const
{
    return strncmp(sharedString->c_str, rhs.C_String(), num);
}
int VEString::StrICmp(const VEString &rhs) const
{
    return _stricmp(sharedString->c_str, rhs.C_String());
}
void VEString::Printf(void)
{
    VENET_DEBUG_PRINTF("%s", sharedString->c_str);
}
void VEString::FPrintf(FILE *fp)
{
    fprintf(fp,"%s", sharedString->c_str);
}
bool VEString::IPAddressMatch(const char *IP)
{
    unsigned characterIndex;

    if ( IP == 0 || IP[ 0 ] == 0 || strlen( IP ) > 15 )
        return false;

    characterIndex = 0;

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
    while ( true )
    {
        if (sharedString->c_str[ characterIndex ] == IP[ characterIndex ] )
        {
            if ( IP[ characterIndex ] == 0 )
            {
                return true;
            }

            characterIndex++;
        }

        else
        {
            if ( sharedString->c_str[ characterIndex ] == 0 || IP[ characterIndex ] == 0 )
            {
                break;
            }

            if ( sharedString->c_str[ characterIndex ] == '*' )
            {
                return true;
            }

            break;
        }
    }


    return false;
}
bool VEString::ContainsNonprintableExceptSpaces(void) const
{
    size_t strLen = strlen(sharedString->c_str);
    unsigned i;
    for (i=0; i < strLen; i++)
    {
        if (sharedString->c_str[i] < ' ' || sharedString->c_str[i] >126)
            return true;
    }
    return false;
}
bool VEString::IsEmailAddress(void) const
{
    if (IsEmpty())
        return false;
    size_t strLen = strlen(sharedString->c_str);
    if (strLen < 6) // a@b.de
        return false;
    if (sharedString->c_str[strLen-4]!='.' && sharedString->c_str[strLen-3]!='.') // .com, .net., .org, .de
        return false;
    unsigned i;
    for (i=0; i < strLen; i++)
    {
        if (sharedString->c_str[i] <= ' ' || sharedString->c_str[i] >126)
            return false;
    }
    int atCount=0;
    for (i=0; i < strLen; i++)
    {
        if (sharedString->c_str[i]=='@')
        {
            atCount++;
        }
    }
    if (atCount!=1)
        return false;
    int dotCount=0;
    for (i=0; i < strLen; i++)
    {
        if (sharedString->c_str[i]=='.')
        {
            dotCount++;
        }
    }
    if (dotCount==0)
        return false;

    return true;
}
VENet::VEString& VEString::URLEncode(void)
{
    VEString result;
    size_t strLen = strlen(sharedString->c_str);
    result.Allocate(strLen*3);
    char *output=result.sharedString->c_str;
    unsigned int outputIndex=0;
    unsigned i;
    char c;
    for (i=0; i < strLen; i++)
    {
        c=sharedString->c_str[i];
        if (
            (c<=47) ||
            (c>=58 && c<=64) ||
            (c>=91 && c<=96) ||
            (c>=123)
        )
        {
            VENet::VEString tmp("%2X", c);
            output[outputIndex++]='%';
            output[outputIndex++]=tmp.sharedString->c_str[0];
            output[outputIndex++]=tmp.sharedString->c_str[1];
        }
        else
        {
            output[outputIndex++]=c;
        }
    }

    output[outputIndex]=0;

    *this = result;
    return *this;
}
VENet::VEString& VEString::URLDecode(void)
{
    VEString result;
    size_t strLen = strlen(sharedString->c_str);
    result.Allocate(strLen);
    char *output=result.sharedString->c_str;
    unsigned int outputIndex=0;
    char c;
    char hexDigits[2];
    char hexValues[2];
    unsigned int i;
    for (i=0; i < strLen; i++)
    {
        c=sharedString->c_str[i];
        if (c=='%')
        {
            hexDigits[0]=sharedString->c_str[++i];
            hexDigits[1]=sharedString->c_str[++i];
            if (hexDigits[0]==' ')
                hexValues[0]=0;
            else if (hexDigits[0]>='A')
                hexValues[0]=hexDigits[0]-'A'+10;
            else
                hexValues[0]=hexDigits[0]-'0';
            if (hexDigits[1]>='A')
                hexValues[1]=hexDigits[1]-'A'+10;
            else
                hexValues[1]=hexDigits[1]-'0';
            output[outputIndex++]=hexValues[0]*16+hexValues[1];
        }
        else
        {
            output[outputIndex++]=c;
        }
    }

    output[outputIndex]=0;

    *this = result;
    return *this;
}
void VEString::SplitURI(VENet::VEString &header, VENet::VEString &domain, VENet::VEString &path)
{
    header.Clear();
    domain.Clear();
    path.Clear();

    size_t strLen = strlen(sharedString->c_str);

    char c;
    unsigned int i=0;
    if (strncmp(sharedString->c_str, "http://", 7)==0)
        i+=(unsigned int) strlen("http://");
    else if (strncmp(sharedString->c_str, "https://", 8)==0)
        i+=(unsigned int) strlen("https://");

    if (strncmp(sharedString->c_str, "www.", 4)==0)
        i+=(unsigned int) strlen("www.");

    if (i!=0)
    {
        header.Allocate(i+1);
        strncpy(header.sharedString->c_str, sharedString->c_str, i);
        header.sharedString->c_str[i]=0;
    }


    domain.Allocate(strLen-i+1);
    char *domainOutput=domain.sharedString->c_str;
    unsigned int outputIndex=0;
    for (; i < strLen; i++)
    {
        c=sharedString->c_str[i];
        if (c=='/')
        {
            break;
        }
        else
        {
            domainOutput[outputIndex++]=sharedString->c_str[i];
        }
    }

    domainOutput[outputIndex]=0;

    path.Allocate(strLen-header.GetLength()-outputIndex+1);
    outputIndex=0;
    char *pathOutput=path.sharedString->c_str;
    for (; i < strLen; i++)
    {
        pathOutput[outputIndex++]=sharedString->c_str[i];
    }
    pathOutput[outputIndex]=0;
}
VENet::VEString& VEString::SQLEscape(void)
{
    int strLen=(int)GetLength();
    int escapedCharacterCount=0;
    int index;
    for (index=0; index < strLen; index++)
    {
        if (sharedString->c_str[index]=='\'' ||
                sharedString->c_str[index]=='"' ||
                sharedString->c_str[index]=='\\')
            escapedCharacterCount++;
    }
    if (escapedCharacterCount==0)
        return *this;

    Clone();
    Realloc(sharedString, strLen+escapedCharacterCount);
    int writeIndex, readIndex;
    writeIndex = strLen+escapedCharacterCount;
    readIndex=strLen;
    while (readIndex>=0)
    {
        if (sharedString->c_str[readIndex]=='\'' ||
                sharedString->c_str[readIndex]=='"' ||
                sharedString->c_str[readIndex]=='\\')
        {
            sharedString->c_str[writeIndex--]=sharedString->c_str[readIndex--];
            sharedString->c_str[writeIndex--]='\\';
        }
        else
        {
            sharedString->c_str[writeIndex--]=sharedString->c_str[readIndex--];
        }
    }
    return *this;
}
VEString VEString::FormatForPOST(VEString &uri, VEString &contentType, unsigned int port, VEString &body)
{
    VEString out;
    VEString host;
    VEString remotePath;
    VENet::VEString header;

    uri.SplitURI(header, host, remotePath);
    if (host.IsEmpty() || remotePath.IsEmpty())
        return out;

    out.Set("POST %s HTTP/1.0\r\n"
            "Host: %s:%i\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %u\r\n"
            "\r\n"
            "%s",
            remotePath.C_String(),
            host.C_String(),
            port,
            contentType.C_String(),
            body.GetLength(),
            body.C_String());

    return out;
}
VEString VEString::FormatForGET(VEString &uri, unsigned int port)
{
    VEString out;
    VEString host;
    VEString remotePath;
    VENet::VEString header;

    uri.SplitURI(header, host, remotePath);
    if (host.IsEmpty() || remotePath.IsEmpty())
        return out;

    out.Set("GET %s HTTP/1.0\r\n"
            "Host: %s:%i\r\n"
            "\r\n",
            remotePath.C_String(),
            host.C_String(),
            port);

    return out;
}
VENet::VEString& VEString::MakeFilePath(void)
{
    if (IsEmpty())
        return *this;

    VENet::VEString fixedString = *this;
    fixedString.Clone();
    for (int i=0; fixedString.sharedString->c_str[i]; i++)
    {
#ifdef _WIN32
        if (fixedString.sharedString->c_str[i]=='/')
            fixedString.sharedString->c_str[i]='\\';
#else
        if (fixedString.sharedString->c_str[i]=='\\')
            fixedString.sharedString->c_str[i]='/';
#endif
    }

#ifdef _WIN32
    if (fixedString.sharedString->c_str[strlen(fixedString.sharedString->c_str)-1]!='\\')
    {
        fixedString+='\\';
    }
#else
    if (fixedString.sharedString->c_str[strlen(fixedString.sharedString->c_str)-1]!='/')
    {
        fixedString+='/';
    }
#endif

    if (fixedString!=*this)
        *this = fixedString;
    return *this;
}
void VEString::FreeMemory(void)
{
    LockMutex();
    FreeMemoryNoMutex();
    UnlockMutex();
}
void VEString::FreeMemoryNoMutex(void)
{
    for (unsigned int i=0; i < freeList.Size(); i++)
    {
        VENet::OP_DELETE(freeList[i]->refCountMutex,_FILE_AND_LINE_);
        veFree_Ex(freeList[i], _FILE_AND_LINE_ );
    }
    freeList.Clear(false, _FILE_AND_LINE_);
}
void VEString::Serialize(BitStream *bs) const
{
    Serialize(sharedString->c_str, bs);
}
void VEString::Serialize(const char *str, BitStream *bs)
{
    unsigned short l = (unsigned short) strlen(str);
    bs->Write(l);
    bs->WriteAlignedBytes((const unsigned char*) str, (const unsigned int) l);
}
void VEString::SerializeCompressed(BitStream *bs, uint8_t languageId, bool writeLanguageId) const
{
    SerializeCompressed(C_String(), bs, languageId, writeLanguageId);
}
void VEString::SerializeCompressed(const char *str, BitStream *bs, uint8_t languageId, bool writeLanguageId)
{
    if (writeLanguageId)
        bs->WriteCompressed(languageId);
    StringCompressor::Instance()->EncodeString(str,0xFFFF,bs,languageId);
}
bool VEString::Deserialize(BitStream *bs)
{
    Clear();

    bool b;
    unsigned short l;
    b=bs->Read(l);
    if (l>0)
    {
        Allocate(((unsigned int) l)+1);
        b=bs->ReadAlignedBytes((unsigned char*) sharedString->c_str, l);
        if (b)
            sharedString->c_str[l]=0;
        else
            Clear();
    }
    else
        bs->AlignReadToByteBoundary();
    return b;
}
bool VEString::Deserialize(char *str, BitStream *bs)
{
    bool b;
    unsigned short l;
    b=bs->Read(l);
    if (b && l>0)
        b=bs->ReadAlignedBytes((unsigned char*) str, l);

    if (b==false)
        str[0]=0;

    str[l]=0;
    return b;
}
bool VEString::DeserializeCompressed(BitStream *bs, bool readLanguageId)
{
    uint8_t languageId;
    if (readLanguageId)
        bs->ReadCompressed(languageId);
    else
        languageId=0;
    return StringCompressor::Instance()->DecodeString(this,0xFFFF,bs,languageId);
}
bool VEString::DeserializeCompressed(char *str, BitStream *bs, bool readLanguageId)
{
    uint8_t languageId;
    if (readLanguageId)
        bs->ReadCompressed(languageId);
    else
        languageId=0;
    return StringCompressor::Instance()->DecodeString(str,0xFFFF,bs,languageId);
}
const char *VEString::ToString(int64_t i)
{
    static int index=0;
    static char buff[64][64];
#if defined(_WIN32)
    sprintf(buff[index], "%I64d", i);
#else
    sprintf(buff[index], "%lld", (long long unsigned int) i);
#endif
    int lastIndex=index;
    if (++index==64)
        index=0;
    return buff[lastIndex];
}
const char *VEString::ToString(uint64_t i)
{
    static int index=0;
    static char buff[64][64];
#if defined(_WIN32)
    sprintf(buff[index], "%I64u", i);
#else
    sprintf(buff[index], "%llu", (long long unsigned int) i);
#endif
    int lastIndex=index;
    if (++index==64)
        index=0;
    return buff[lastIndex];
}
void VEString::Clear(void)
{
    Free();
}
void VEString::Allocate(size_t len)
{
    VEString::LockMutex();
    if (VEString::freeList.Size()==0)
    {
        unsigned i;
        for (i=0; i < 128; i++)
        {
            VEString::SharedString *ss;
            ss = (VEString::SharedString*) rakMalloc_Ex(sizeof(VEString::SharedString), _FILE_AND_LINE_);
            ss->refCountMutex=VENet::OP_NEW<SimpleMutex>(_FILE_AND_LINE_);
            VEString::freeList.Insert(ss, _FILE_AND_LINE_);
        }
    }
    sharedString = VEString::freeList[VEString::freeList.Size()-1];
    VEString::freeList.RemoveAtIndex(VEString::freeList.Size()-1);
    VEString::UnlockMutex();

    const size_t smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
    sharedString->refCount=1;
    if (len <= smallStringSize)
    {
        sharedString->bytesUsed=smallStringSize;
        sharedString->c_str=sharedString->smallString;
    }
    else
    {
        sharedString->bytesUsed=len<<1;
        sharedString->bigString=(char*)rakMalloc_Ex(sharedString->bytesUsed, _FILE_AND_LINE_);
        sharedString->c_str=sharedString->bigString;
    }
}
void VEString::Assign(const char *str)
{
    if (str==0 || str[0]==0)
    {
        sharedString=&emptyString;
        return;
    }

    size_t len = strlen(str)+1;
    Allocate(len);
    memcpy(sharedString->c_str, str, len);
}
void VEString::Assign(const char *str, va_list ap)
{
    if (str==0 || str[0]==0)
    {
        sharedString=&emptyString;
        return;
    }

    char stackBuff[512];
    if (vsnprintf(stackBuff, 512, str, ap)!=-1
#ifndef _WIN32
            && strlen(str) <511
#endif
       )
    {
        Assign(stackBuff);
        return;
    }
    char *buff=0, *newBuff;
    size_t buffSize=8096;
    while (1)
    {
        newBuff = (char*) rakRealloc_Ex(buff, buffSize,__FILE__,__LINE__);
        if (newBuff==0)
        {
            notifyOutOfMemory(_FILE_AND_LINE_);
            if (buff!=0)
            {
                Assign(buff);
                veFree_Ex(buff,__FILE__,__LINE__);
            }
            else
            {
                Assign(stackBuff);
            }
            return;
        }
        buff=newBuff;
        if (vsnprintf(buff, buffSize, str, ap)!=-1)
        {
            Assign(buff);
            veFree_Ex(buff,__FILE__,__LINE__);
            return;
        }
        buffSize*=2;
    }
}
VENet::VEString VEString::Assign(const char *str,size_t pos, size_t n )
{
    size_t incomingLen=strlen(str);

    Clone();

    if (str==0 || str[0]==0||pos>=incomingLen)
    {
        sharedString=&emptyString;
        return (*this);
    }

    if (pos+n>=incomingLen)
    {
        n=incomingLen-pos;

    }
    const char * tmpStr=&(str[pos]);

    size_t len = n+1;
    Allocate(len);
    memcpy(sharedString->c_str, tmpStr, len);
    sharedString->c_str[n]=0;

    return (*this);
}

VENet::VEString VEString::NonVariadic(const char *str)
{
    VENet::VEString rs;
    rs=str;
    return rs;
}
unsigned long VEString::ToInteger(const char *str)
{
    unsigned long hash = 0;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
unsigned long VEString::ToInteger(const VEString &rs)
{
    return VEString::ToInteger(rs.C_String());
}
void VEString::AppendBytes(const char *bytes, unsigned int count)
{
    Clone();
    Realloc(sharedString, count);
    unsigned int length=(unsigned int) GetLength();
    memcpy(sharedString->c_str+length, bytes, count);
    sharedString->c_str[length+count]=0;
}
void VEString::Clone(void)
{
    if (sharedString==&emptyString)
    {
        return;
    }

    sharedString->refCountMutex->Lock();
    if (sharedString->refCount==1)
    {
        sharedString->refCountMutex->Unlock();
        return;
    }

    sharedString->refCount--;
    sharedString->refCountMutex->Unlock();
    Assign(sharedString->c_str);
}
void VEString::Free(void)
{
    if (sharedString==&emptyString)
        return;
    sharedString->refCountMutex->Lock();
    sharedString->refCount--;
    if (sharedString->refCount==0)
    {
        sharedString->refCountMutex->Unlock();
        const size_t smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
        if (sharedString->bytesUsed>smallStringSize)
            veFree_Ex(sharedString->bigString, _FILE_AND_LINE_ );
        VEString::LockMutex();
        VEString::freeList.Insert(sharedString, _FILE_AND_LINE_);
        VEString::UnlockMutex();

        sharedString=&emptyString;
    }
    else
    {
        sharedString->refCountMutex->Unlock();
    }
    sharedString=&emptyString;
}
unsigned char VEString::ToLower(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c-'A'+'a';
    return c;
}
unsigned char VEString::ToUpper(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
        return c-'a'+'A';
    return c;
}
void VEString::LockMutex(void)
{
    GetPoolMutex().Lock();
}
void VEString::UnlockMutex(void)
{
    GetPoolMutex().Unlock();
}
