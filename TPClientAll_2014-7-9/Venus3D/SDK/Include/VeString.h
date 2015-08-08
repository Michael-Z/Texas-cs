////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeString.h
//  Version:     v1.00
//  Created:     10/9/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wchar.h>
#include <stdlib.h>
#include "VeVector.h"

#define VE_MAX_FORMAT_SIZE (4096)

#define VeStrlen strlen
#define VeStrcmp strcmp
#define VeStrncmp strncmp
#define VeStrpbrk strpbrk
#define VeStrchr strchr
#define VeStrstr strstr
#define VeStrrchr strrchr
#define VeWcslen wcslen
#define VeWcscmp wcscmp
#define VeWcspbrk wcspbrk
#define VeWcschr wcschr
#define VeWcsstr wcsstr
#define VeWcsrchr wcsrchr

VE_POWER_API void VeStrToLower(VeChar8* pcDst, VeSizeT stDstSize, const VeChar8* pcSrc);

VE_POWER_API void VeStrToSupper(VeChar8* pcDst, VeSizeT stDstSize, const VeChar8* pcSrc);

#define VeIsAlpha isalpha
#define VeIsSpace isspace
#define VeIsAlnum isalnum
#define VeToLower tolower
#define VeToSupper toupper

#define VE_STR(str) #str

#define VeAtof atof
#define VeAtoi atoi
#define VeWtof(s) wcstod(s,NULL)
#define VeWtoi(s) wcstol(s,NULL,0)

VE_POWER_API VeInt32 VeStricmp(const VeChar8* dst, const VeChar8* src);

VE_POWER_API VeInt32 VeStrnicmp(const VeChar8* dst, const VeChar8* src, VeInt32 count);

VE_POWER_API VeChar8* VeStrcpy(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcSrc);

VE_POWER_API VeChar8* VeStrncpy(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcSrc, VeSizeT stCount);

VE_POWER_API VeChar8* VeStrcat(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcSrc);

VE_POWER_API VeChar8* VeStrtok(VeChar8* pcString, const VeChar8* pcDelimit, VeChar8** ppcContext);

VE_POWER_API VeInt32 VeSprintf(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcFormat, ...);

VE_POWER_API VeInt32 VeVsprintf(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcFormat, va_list kArgs);

VE_POWER_API VeInt32 VeSnprintf(VeChar8* pcDest, VeSizeT stDestSize, VeSizeT stCount, const VeChar8* pcFormat, ...);

VE_POWER_API VeInt32 VeVsnprintf(VeChar8* pcDest, VeSizeT stDestSize, VeSizeT stCount, const VeChar8* pcFormat, va_list kArgs);

VE_POWER_API VeInt32 VeMemcpy(void* pvDest, VeSizeT stDestSize, const void* pvSrc, VeSizeT stCount);

VE_POWER_API VeInt32 VeMemcpy(void* pvDest, const void* pvSrc, VeSizeT stCount);

VE_POWER_API VeInt32 VeMemmove(void* pvDest, VeSizeT stDestSize, const void* pvSrc, VeSizeT stCount);

VE_POWER_API VeInt32 VeWStricmp(const VeChar16* dst, const VeChar16* src);

VE_POWER_API VeInt32 VeWStrnicmp(const VeChar16* dst, const VeChar16* src, VeInt32 count);

VE_POWER_API VeChar16* VeWStrcpy(VeChar16* pkDest, VeSizeT stDestSize, const VeChar16* pkSrc);

VE_POWER_API VeChar16* VeWStrncpy(VeChar16* pkDest, VeSizeT stDestSize, const VeChar16* pkSrc, VeSizeT stCount);

VE_POWER_API VeChar16* VeWStrcat(VeChar16* pkDest, VeSizeT stDestSize, const VeChar16* pkSrc);

VE_POWER_API VeChar16* VeWStrtok(VeChar16* pkString, const VeChar16* pkDelimit, VeChar16** ppkContext);

VE_POWER_API VeInt32 VeWSprintf(VeChar16* pkDest, VeSizeT stDestSize, const VeChar16* pkFormat, ...);

class VeFixedStringA;

class VE_POWER_API VeStringA : public VeMemObject
{
public:
	VeStringA(VeUInt32 u32Length = 225);

	VeStringA(VeChar8 c8Char, VeUInt32 u32Length = 1);

	VeStringA(const VeChar8* pc8Str);

	VeStringA(const VeChar8* pc8Str, VeUInt32 u32Length);

	VeStringA(const VeStringA& kString);

	VeStringA(const VeFixedStringA& kStr);

	~VeStringA();

	VeUInt32 Length() const;

	bool IsEmpty() const;

	void Empty(); 

	VeChar8 GetAt(VeUInt32 u32Index) const;

	void SetAt(VeUInt32 u32Index, VeChar8 c);

	VeInt32 Compare(const VeChar8* pc8String) const;

	VeInt32 CompareNoCase(const VeChar8* pc8String) const;

	bool Equals(const VeChar8* pc8Str) const;

	bool EqualsNoCase(const VeChar8* pc8Str) const;

	VeStringA GetSubstring(VeUInt32 u32Begin, VeUInt32 u32End) const;

	VeStringA GetSubstring(VeUInt32 u32Begin, const VeChar8* pc8Delimeters) const;

	VeUInt32 Find(VeChar8 c, VeUInt32 u32Start = 0) const;

	VeUInt32 Find(const VeChar8* pc8Str, VeUInt32 u32Start = 0) const;

	bool Contains(const VeChar8* pc8Str) const;

	bool ContainsNoCase(const VeChar8* pc8Str) const;

	VeUInt32 FindReverse(VeChar8 c) const;

	VeUInt32 FindReverse(VeChar8 c, VeUInt32 u32StartIdx) const;

	VeUInt32 FindOneOf(const VeChar8* pc8Str, VeUInt32 u32Start = 0) const;

	VeStringA Mid(VeUInt32 u32Begin, VeUInt32 u32Count = VE_INFINITE) const;

	VeStringA Left(VeUInt32 u32Count) const;

	VeStringA Right(VeUInt32 u32Count) const;

	void Replace(const VeChar8* pcWhatToReplace , const VeChar8* pcReplaceWith);

	void Insert(const VeChar8* pcInsertString, VeUInt32 u32Index);

	void RemoveRange(VeUInt32 u32StartIdx, VeUInt32 u32NumChars);

	void Format(const VeChar8* pcFormatString ...);

	void ToUpper();

	void ToLower();

	void Reverse();

	void TrimLeft(VeChar8 c = ' ');

	void TrimRight(VeChar8 c = ' ');

	void Concatenate(const VeChar8* pcStr);

	void Concatenate(VeChar8 ch);

	bool ToBool(bool& b) const;

	bool To(bool& b) const;

	bool ToFloat(VeFloat32& f) const;

	bool To(VeFloat32& f) const;

	bool ToInt(VeInt32& i) const;

	bool To(VeInt32& i) const;
	
	bool To(VeInt16& i) const;
	
	bool To(VeInt8& i) const;

	bool ToUInt(VeUInt32& ui) const;

	bool To(VeUInt32& ui) const;
	
	bool To(VeUInt16& ui) const;
	
	bool To(VeUInt8& ui) const;

	bool ToVector2D(VeVector2D& kVec) const;

	bool To(VeVector2D& kVec) const;

	bool ToVector3D(VeVector3D& kVec) const;
	
	bool To(VeVector3D& kVec) const;

	bool ToVector4D(VeVector4D& kVec) const;

	bool To(VeVector4D& kVec) const;

	bool ToRGB(VeColor& kColor) const;

	bool To(VeColor& kColor) const;

	bool ToRGBA(VeColorA& kColor) const;

	bool To(VeColorA& kColor) const;

	static VeStringA FromBool(bool b);

	static VeStringA From(bool b);

	static VeStringA FromFloat(VeFloat32 f);

	static VeStringA From(VeFloat32 f);

	static VeStringA FromInt(VeInt32 i);

	static VeStringA From(VeInt32 i);

	static VeStringA From(VeInt16 i);

	static VeStringA From(VeInt8 i);

	static VeStringA FromUInt(VeUInt32 ui);

	static VeStringA From(VeUInt32 ui);

	static VeStringA From(VeUInt16 ui);

	static VeStringA From(VeUInt8 ui);

	static VeStringA FromVector2D(VeVector2D& kPt);

	static VeStringA From(VeVector2D& kPt);

	static VeStringA FromVector3D(VeVector3D& kPt);

	static VeStringA From(VeVector3D& kPt);

	static VeStringA FromVector4D(VeVector4D& kPt);

	static VeStringA From(VeVector4D& kPt);

	static VeStringA FromRGB(VeColor& kColor);

	static VeStringA From(VeColor& kColor);

	static VeStringA FromRGBA(VeColorA& kColor);

	static VeStringA From(VeColorA& kColor);

	static VeUInt32 CalculateHash(const VeChar8* pcString);

	static VeUInt32 KeyToHashCode(const VeChar8* pcString, VeUInt32 u32TableSize);

	VeUInt32 GetHashCode() const;

	operator const VeChar8* () const;

	VeChar8* GetString();

	VeStringA& operator = (VeStringA& stringSrc);

	VeStringA& operator = (const VeChar8* psz);

	VeStringA& operator = (const VeFixedStringA& kString);

	VeStringA& operator = (VeChar8 ch);

	VeStringA& operator += (VeStringA& string);

	VeStringA& operator += (VeChar8 ch);

	VeStringA& operator += (const VeChar8* pcString);

	VeStringA& operator += (const VeFixedStringA& kString);

	friend VeStringA VE_POWER_API operator + (const VeStringA& string1, const VeStringA& string2);

	friend VeStringA VE_POWER_API operator + (const VeStringA& string, VeChar8 ch);

	friend VeStringA VE_POWER_API operator + (VeChar8 ch, const VeStringA& string);

	friend VeStringA VE_POWER_API operator + (const VeStringA& string, const VeChar8* lpsz);

	friend VeStringA VE_POWER_API operator + (const VeStringA& string, const VeFixedStringA& kString2);

	friend VeStringA VE_POWER_API operator + (const VeChar8* lpsz, const VeStringA& string);

	friend VeStringA VE_POWER_API operator + (const VeFixedStringA& kString, const VeStringA& string);

	friend bool VE_POWER_API operator == (const VeStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator == (const VeStringA& s1, const VeChar8* s2);

	friend bool VE_POWER_API operator == (const VeStringA& s1, const VeFixedStringA& s2);

	friend bool VE_POWER_API operator == (const VeChar8* s1, const VeStringA& s2);

	friend bool VE_POWER_API operator == (const VeFixedStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator != (const VeStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator != (const VeStringA& s1, const VeChar8* s2);

	friend bool VE_POWER_API operator != (const VeStringA& s1, const VeFixedStringA& s2);

	friend bool VE_POWER_API operator != (const VeChar8* s1, const VeStringA& s2);

	friend bool VE_POWER_API operator != (const VeFixedStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator < (const VeStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator < (const VeStringA& s1, const VeChar8* s2);

	friend bool VE_POWER_API operator < (const VeStringA& s1, const VeFixedStringA& s2);

	friend bool VE_POWER_API operator < (const VeChar8* s1, const VeStringA& s2);

	friend bool VE_POWER_API operator < (const VeFixedStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator > (const VeStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator > (const VeStringA& s1, const VeChar8* s2);

	friend bool VE_POWER_API operator > (const VeStringA& s1, const VeFixedStringA& s2);

	friend bool VE_POWER_API operator > (const VeChar8* s1, const VeStringA& s2);

	friend bool VE_POWER_API operator > (const VeFixedStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator <= (const VeStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator <= (const VeStringA& s1, const VeChar8* s2);

	friend bool VE_POWER_API operator <= (const VeStringA& s1, const VeFixedStringA& s2);

	friend bool VE_POWER_API operator <= (const VeChar8* s1, const VeStringA& s2);

	friend bool VE_POWER_API operator <= (const VeFixedStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator >= (const VeStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API operator >= (const VeStringA& s1, const VeChar8* s2);

	friend bool VE_POWER_API operator >= (const VeStringA& s1, const VeFixedStringA& s2);
	
	friend bool VE_POWER_API operator >= (const VeChar8* s1, const VeStringA& s2);

	friend bool VE_POWER_API operator >= (const VeFixedStringA& s1, const VeStringA& s2);

	friend bool VE_POWER_API IsTrue(VeStringA strStr);

	VeChar8* MakeExternalCopy();

protected:
	typedef VeChar8* StringHandle;
	StringHandle m_hHandle;

	static StringHandle Allocate(VeUInt32 u32StrLength);

	static StringHandle AllocateAndCopy(const VeChar8* pcString);

	static StringHandle AllocateAndCopyHandle(StringHandle hHandle);

	static void Deallocate(StringHandle& hHandle);

	static void IncRefCount(StringHandle& hHandle);

    static void DecRefCount(StringHandle& hHandle);

    static void Swap(StringHandle& hHandle, const VeChar8* pcNewValue);
    
	static VeChar8* GetString(const StringHandle& hHandle);

	static VeUInt32 GetLength(const StringHandle& hHandle);
	
	static VeUInt32 GetRefCount(const StringHandle& hHandle);

    static VeUInt32 GetAllocationSize(const StringHandle& hHandle);
    
	static VeUInt32 GetBufferSize(const StringHandle& hHandle);

	static void SetLength(const StringHandle& hHandle, VeUInt32 u32Length);

	static VeChar8* GetRealBufferStart(const StringHandle& hHandle);

	static bool ValidateString(const StringHandle& hHandle);

	static VeUInt32 GetBestBufferSize(VeUInt32 u32ReqSize);

	bool Resize(VeUInt32 u32Delta);

	void SetBuffer(StringHandle& hHandle);

	void CalcLength();

	void CopyOnWrite(bool bForceCopy = false);

	void CopyOnWriteAndResize(VeUInt32 u32SizeDelta, bool bForceCopy = false);

	VeChar8* ConsumeWhitespace(const VeChar8* pcStr, VeUInt32 u32Length) const;

	void Format(const VeChar8* pcFmt, va_list argPtr);

};

class VE_POWER_API VeStringW : public VeMemObject
{
public:
	VeStringW(VeUInt32 u32Length = 225);

	VeStringW(VeChar16 c16Char, VeUInt32 u32Length = 1);

	VeStringW(const VeChar16* pc16Str);

	VeStringW(const VeStringW& kString);

	~VeStringW();

	VeUInt32 Length() const;

	bool IsEmpty() const;

	void Empty(); 

	VeChar16 GetAt(VeUInt32 u32Index) const;

	void SetAt(VeUInt32 u32Index, VeChar16 c);

	VeInt32 Compare(const VeChar16* pc16String) const;

	VeInt32 CompareNoCase(const VeChar16* pc16String) const;

	bool Equals(const VeChar16* pc16Str) const;

	bool EqualsNoCase(const VeChar16* pc16Str) const;

	VeStringW GetSubstring(VeUInt32 u32Begin, VeUInt32 u32End) const;

	VeStringW GetSubstring(VeUInt32 u32Begin, const VeChar16* pc16Delimeters) const;

	VeUInt32 Find(VeChar16 c, VeUInt32 u32Start = 0) const;

	VeUInt32 Find(const VeChar16* pc16Str, VeUInt32 u32Start = 0) const;

	bool Contains(const VeChar16* pc16Str) const;

	bool ContainsNoCase(const VeChar16* pc16Str) const;

	VeUInt32 FindReverse(VeChar16 c) const;

	VeUInt32 FindReverse(VeChar16 c, VeUInt32 u32StartIdx) const;

	VeUInt32 FindOneOf(const VeChar16* pc16Str, VeUInt32 u32Start = 0) const;

	VeStringW Mid(VeUInt32 u32Begin, VeUInt32 u32Count = VE_INFINITE) const;

	VeStringW Left(VeUInt32 u32Count) const;

	VeStringW Right(VeUInt32 u32Count) const;

	void Replace(const VeChar16* pcWhatToReplace , const VeChar16* pcReplaceWith);

	void Insert(const VeChar16* pcInsertString, VeUInt32 u32Index);

	void RemoveRange(VeUInt32 u32StartIdx, VeUInt32 u32NumChars);

	void ToUpper();

	void ToLower();

	void Reverse();

	void TrimLeft(VeChar16 c = L' ');

	void TrimRight(VeChar16 c = L' ');

	void Concatenate(const VeChar16* pcStr);

	void Concatenate(VeChar16 ch);

	bool ToBool(bool& b) const;

	bool To(bool& b) const;

	bool ToFloat(VeFloat32& f) const;

	bool To(VeFloat32& f) const;

	bool ToInt(VeInt32& i) const;

	bool To(VeInt32& i) const;

	bool To(VeInt16& i) const;

	bool To(VeInt8& i) const;

	bool ToUInt(VeUInt32& ui) const;

	bool To(VeUInt32& ui) const;

	bool To(VeUInt16& ui) const;

	bool To(VeUInt8& ui) const;

	bool ToVector2D(VeVector2D& kVec) const;

	bool To(VeVector2D& kVec) const;

	bool ToVector3D(VeVector3D& kVec) const;
	
	bool To(VeVector3D& kVec) const;

	bool ToVector4D(VeVector4D& kVec) const;

	bool To(VeVector4D& kVec) const;

	bool ToRGB(VeColor& kColor) const;

	bool To(VeColor& kColor) const;

	bool ToRGBA(VeColorA& kColor) const;

	bool To(VeColorA& kColor) const;

	static VeStringW FromBool(bool b);

	static VeStringW From(bool b);

	static VeStringW FromFloat(VeFloat32 f);

	static VeStringW From(VeFloat32 f);

	static VeStringW FromInt(VeInt32 i);

	static VeStringW From(VeInt32 i);

	static VeStringW From(VeInt16 i);

	static VeStringW From(VeInt8 i);

	static VeStringW FromUInt(VeUInt32 ui);

	static VeStringW From(VeUInt32 ui);

	static VeStringW From(VeUInt16 ui);

	static VeStringW From(VeUInt8 ui);

	static VeStringW FromVector2D(VeVector2D& kPt);

	static VeStringW From(VeVector2D& kPt);

	static VeStringW FromVector3D(VeVector3D& kPt);

	static VeStringW From(VeVector3D& kPt);

	static VeStringW FromVector4D(VeVector4D& kPt);

	static VeStringW From(VeVector4D& kPt);

	static VeStringW FromRGB(VeColor& kColor);

	static VeStringW From(VeColor& kColor);

	static VeStringW FromRGBA(VeColorA& kColor);

	static VeStringW From(VeColorA& kColor);

	operator const VeChar16* () const;

	VeStringW& operator = (VeStringW& stringSrc);

	VeStringW& operator = (const VeChar16* psz);

	VeStringW& operator = (VeChar16 ch);

	VeStringW& operator += (VeStringW& string);

	VeStringW& operator += (VeChar16 ch);

	VeStringW& operator += (const VeChar16* pcString);

	friend VeStringW VE_POWER_API operator + (const VeStringW& string1, const VeStringW& string2);

	friend VeStringW VE_POWER_API operator + (const VeStringW& string, VeChar16 ch);

	friend VeStringW VE_POWER_API operator + (VeChar16 ch, const VeStringW& string);

	friend VeStringW VE_POWER_API operator + (const VeStringW& string, const VeChar16* lpsz);

	friend VeStringW VE_POWER_API operator + (const VeChar16* lpsz, const VeStringW& string);

	friend bool VE_POWER_API operator == (const VeStringW& s1, const VeStringW& s2);

	friend bool VE_POWER_API operator == (const VeStringW& s1, const VeChar16* s2);

	friend bool VE_POWER_API operator == (const VeChar16* s1, const VeStringW& s2);

	friend bool VE_POWER_API operator != (const VeStringW& s1, const VeStringW& s2);

	friend bool VE_POWER_API operator != (const VeStringW& s1, const VeChar16* s2);

	friend bool VE_POWER_API operator != (const VeChar16* s1, const VeStringW& s2);

	friend bool VE_POWER_API operator < (const VeStringW& s1, const VeStringW& s2);

	friend bool VE_POWER_API operator < (const VeStringW& s1, const VeChar16* s2);

	friend bool VE_POWER_API operator < (const VeChar16* s1, const VeStringW& s2);

	friend bool VE_POWER_API operator > (const VeStringW& s1, const VeStringW& s2);

	friend bool VE_POWER_API operator > (const VeStringW& s1, const VeChar16* s2);

	friend bool VE_POWER_API operator > (const VeChar16* s1, const VeStringW& s2);

	friend bool VE_POWER_API operator <= (const VeStringW& s1, const VeStringW& s2);

	friend bool VE_POWER_API operator <= (const VeStringW& s1, const VeChar16* s2);

	friend bool VE_POWER_API operator <= (const VeChar16* s1, const VeStringW& s2);

	friend bool VE_POWER_API operator >= (const VeStringW& s1, const VeStringW& s2);

	friend bool VE_POWER_API operator >= (const VeStringW& s1, const VeChar16* s2);

	friend bool VE_POWER_API operator >= (const VeChar16* s1, const VeStringW& s2);

	friend bool VE_POWER_API IsTrue(VeStringW strStr);

	VeChar16* MakeExternalCopy();

protected:
	typedef VeChar16* StringHandle;
	StringHandle m_hHandle;

	static StringHandle Allocate(VeUInt32 u32StrLength);

	static StringHandle AllocateAndCopy(const VeChar16* pcString);

	static StringHandle AllocateAndCopyHandle(StringHandle hHandle);

	static void Deallocate(StringHandle& hHandle);

	static void IncRefCount(StringHandle& hHandle);

	static void DecRefCount(StringHandle& hHandle);

	static void Swap(StringHandle& hHandle, const VeChar16* pcNewValue);

	static VeChar16* GetString(const StringHandle& hHandle);

	static VeUInt32 GetLength(const StringHandle& hHandle);

	static VeUInt32 GetRefCount(const StringHandle& hHandle);

	static VeUInt32 GetAllocationSize(const StringHandle& hHandle);

	static VeUInt32 GetBufferSize(const StringHandle& hHandle);

	static void SetLength(const StringHandle& hHandle, VeUInt32 u32Length);

	static VeChar16* GetRealBufferStart(const StringHandle& hHandle);

	static bool ValidateString(const StringHandle& hHandle);

	static VeUInt32 GetBestBufferSize(VeUInt32 u32ReqSize);

	bool Resize(VeUInt32 u32Delta);

	void SetBuffer(StringHandle& hHandle);

	void CalcLength();

	void CopyOnWrite(bool bForceCopy = false);

	void CopyOnWriteAndResize(VeUInt32 u32SizeDelta, bool bForceCopy = false);

	VeChar16* ConsumeWhitespace(const VeChar16* pcStr, VeUInt32 u32Length) const;

};

#ifdef VE_UNICODE
	typedef VeStringW VeString;
#else
	typedef VeStringA VeString;
#endif

#define VE_NUM_GLOBAL_STRING_HASH_BUCKETS 512
#define VE_NUM_ENTRIES_PER_BUCKET_RESERVE 16

class VE_POWER_API VeGlobalStringTableA : public VeMemObject
{
public:
	typedef VeChar8* StringHandle;

	static void Create();

	static void Destory();

	static const StringHandle AddString(const VeChar8* pcString);

	static void IncRefCount(StringHandle& kHandle);

	static void DecRefCount(StringHandle& kHandle);
	
	static const VeChar8* GetString(const StringHandle& kHandle);
	
	static VeUInt32 GetLength(const StringHandle& kHandle);
	
	static VeUInt32 GetRefCount(const StringHandle& kHandle);
	
	static bool ValidateString(const StringHandle& kHandle);

	static VeUInt32 GetBucketSize(VeUInt32 uiWhichBucket);

	static VeUInt32 GetTotalNumberOfBuckets();
	
	static VeUInt32 GetMaxBucketSize();

	static const StringHandle NULL_STRING;

	static VeUInt32 ms_u32NumStrings;

	enum
	{
		GSTABLE_LEN_MASK        = 0x0000FFFF,
		GSTABLE_LEN_SHIFT       = 0,
		GSTABLE_HASH_MASK       = 0xFFFF0000,
		GSTABLE_HASH_SHIFT      = 16
	};

protected:
	const StringHandle FindString(const VeChar8* pcString, VeUInt32 u32Len);

	void InsertString(const StringHandle& kHandle, VeUInt32 uiHash);

	void RemoveString(const StringHandle& kHandle, VeUInt32 uiHashValue);

	static VeChar8* GetRealBufferStart(const StringHandle& kHandle);

	static VeUInt32 HashFunction(const VeChar8* pcKey, VeUInt32 u32Len);

	static VeUInt32 GetHashValue(const StringHandle& kHandle);

	VeGlobalStringTableA();

	static VeGlobalStringTableA* ms_pkTable;

	VeVector<StringHandle> m_kHashArray[VE_NUM_GLOBAL_STRING_HASH_BUCKETS];
	VeThread::Mutex m_kMutex;

};

class VE_POWER_API VeFixedStringA : public VeMemObject
{
public:
	VeFixedStringA();
	
	VeFixedStringA(const VeChar8* pcString);

	VeFixedStringA(const VeFixedStringA& kString);
	
	~VeFixedStringA();

	operator const VeChar8* () const;

	bool Exists() const;

	VeFixedStringA& operator = (const VeFixedStringA& kString);

	VeFixedStringA& operator = (const VeChar8* pcString);

	VeUInt32 GetLength() const;

	VeUInt32 GetRefCount() const;

	bool Equals(const VeChar8* pcStr) const;

	bool EqualsNoCase(const VeChar8* pcStr) const;

	bool Contains(const VeChar8* pcStr) const;

	bool ContainsNoCase(const VeChar8* pcStr) const;

	bool operator == (const VeFixedStringA& s) const;

	bool operator != (const VeFixedStringA& s) const;

	bool operator == (const VeChar8* s) const;

	bool operator != (const VeChar8* s) const;

	friend VE_POWER_API bool operator == (const VeChar8* s1, const VeFixedStringA& s2);

	friend VE_POWER_API bool operator != (const VeChar8* s1, const VeFixedStringA& s2);

protected:
	VeGlobalStringTableA::StringHandle m_kHandle;

};

typedef VeFixedStringA VeFixedString;
