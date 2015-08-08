////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeString.cpp
//  Version:     v1.00
//  Created:     10/9/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"

//--------------------------------------------------------------------------
void VeStrToLower(VeChar8* pcDst, VeSizeT stDstSize, const VeChar8* pcSrc)
{
	VeSizeT i;
	for(i = 0; i < (stDstSize - 1); ++i)
	{
		VeChar8 cChar = pcSrc[i];
		if(cChar)
		{
			pcDst[i] = (cChar >= 'A' && cChar <= 'Z') ? cChar + 0x20 : cChar;
		}
		else
		{
			break;
		}
	}
	pcDst[i] = 0;
}
//--------------------------------------------------------------------------
void VeStrToSupper(VeChar8* pcDst, VeSizeT stDstSize, const VeChar8* pcSrc)
{
	VeSizeT i;
	for(i = 0; i < (stDstSize - 1); ++i)
	{
		VeChar8 cChar = pcSrc[i];
		if(cChar)
		{
			pcDst[i] = (cChar >= 'a' && cChar <= 'z') ? cChar - 0x20 : cChar;
		}
		else
		{
			break;
		}
	}
	pcDst[i] = 0;
}
//--------------------------------------------------------------------------
VeInt32 VeStricmp(const VeChar8* dst, const VeChar8* src)
{
	VE_ASSERT(dst && src);
	VeInt32 f,l;
	do
	{
		if(((f = (VeUInt8)(*(dst++))) >= 'A') && (f <= 'Z') )
			f -= ('A' - 'a');

		if(((l = (VeUInt8)(*(src++))) >= 'A') && (l <= 'Z'))
			l -= ('A' - 'a');
	}
	while(f && (f == l));
	return(f - l);
}
//--------------------------------------------------------------------------
VeInt32 VeStrnicmp(const VeChar8* dst, const VeChar8* src, VeInt32 count)
{
	VE_ASSERT(dst && src);
	VeInt32 f,l;
	if(count)
	{
		do
		{
			if(((f = (VeUInt8)(*(dst++))) >= 'A') && (f <= 'Z'))
				f -= 'A' - 'a';

			if(((l = (VeUInt8)(*(src++))) >= 'A') && (l <= 'Z'))
				l -= 'A' - 'a';
		}
		while(--count && f && (f == l));

		return(f - l);
	}

	return(0);
}
//--------------------------------------------------------------------------
VeChar8* VeStrcpy(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcSrc)
{
#if _MSC_VER >= 1400

	strcpy_s(pcDest, stDestSize, pcSrc);
	return pcDest;

#else // #if _MSC_VER >= 1400

	VE_ASSERT(stDestSize != 0);
	VeSizeT stSrcLen = VeStrlen(pcSrc);
	VE_ASSERT(stDestSize > stSrcLen);
	VeSizeT stWrite;
	if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
		stWrite = stDestSize;
	else 
		stWrite = stSrcLen + 1;
	VeChar8 *pcResult = strncpy(pcDest, pcSrc, stWrite);
	pcResult[stDestSize - 1] = 0;
	return pcResult;

#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeChar8* VeStrncpy(VeChar8* pcDest, VeSizeT stDestSize,
	const VeChar8* pcSrc, VeSizeT stCount)
{
#if _MSC_VER >= 1400

	strncpy_s(pcDest, stDestSize, pcSrc, stCount);
	return pcDest;

#else // #if _MSC_VER >= 1400

	VE_ASSERT(pcDest != 0 && stDestSize != 0);
	VE_ASSERT(stCount < stDestSize || stCount == VE_SIZET_MAX);

	if (stCount >= stDestSize)
	{
		if (stCount != VE_SIZET_MAX)
		{
			pcDest[0] = 0;
			return pcDest;
		}
		else
		{
			stCount = stDestSize - 1;
		}
	}

	VeChar8* pcResult = strncpy(pcDest, pcSrc, stCount);
	pcResult[stCount] = 0;

	return pcResult;

#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeChar8* VeStrcat(VeChar8* pcDest, VeSizeT stDestSize, const VeChar8* pcSrc)
{
#if _MSC_VER >= 1400

	strcat_s(pcDest, stDestSize, pcSrc);
	return pcDest;

#else // #if _MSC_VER >= 1400

#ifdef VE_DEBUG
	VeSizeT stSrcLen = VeStrlen(pcSrc);
#endif
	VeSizeT stDestLen = VeStrlen(pcDest);
	VE_ASSERT(stSrcLen + stDestLen <= stDestSize - 1);
	VeSizeT stWrite = stDestSize - 1 - stDestLen;
	VeChar8* pcResult = strncat(pcDest, pcSrc, stWrite);
	pcResult[stDestSize - 1] = 0;
	return pcResult;

#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeChar8* VeStrtok(VeChar8* pcString, const VeChar8* pcDelimit,
	VeChar8** ppcContext)
{
#if _MSC_VER >= 1400

	return strtok_s(pcString, pcDelimit, ppcContext);

#else // #if _MSC_VER >= 1400

	return strtok(pcString, pcDelimit);

#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeInt32 VeSprintf(VeChar8* pcDest, VeSizeT stDestSize,
	const VeChar8* pcFormat, ...)
{
	VE_ASSERT(pcFormat);

	va_list kArgs;
	va_start(kArgs, pcFormat);
	int iRet = VeVsprintf(pcDest, stDestSize, pcFormat, kArgs);
	va_end(kArgs);

	return iRet;
}
//--------------------------------------------------------------------------
VeInt32 VeVsprintf(VeChar8* pcDest, VeSizeT stDestSize,
	const VeChar8* pcFormat, va_list kArgs)
{
	return VeVsnprintf(pcDest, stDestSize, VE_SIZET_MAX, pcFormat, kArgs);
}
//--------------------------------------------------------------------------
VeInt32 VeSnprintf(VeChar8* pcDest, VeSizeT stDestSize, VeSizeT stCount,
	const VeChar8* pcFormat, ...)
{
	VE_ASSERT(pcFormat);

	va_list kArgs;
	va_start(kArgs, pcFormat);
	VeInt32 iRet = VeVsnprintf(pcDest, stDestSize, stCount, pcFormat, kArgs);
	va_end(kArgs);

	return iRet;
}
//--------------------------------------------------------------------------
VeInt32 VeVsnprintf(VeChar8* pcDest, VeSizeT stDestSize, VeSizeT stCount,
	const VeChar8* pcFormat, va_list kArgs)
{
	if (stDestSize == 0)
	{
		return 0;
	}

	VE_ASSERT(pcDest);
	VE_ASSERT(stCount < stDestSize || stCount == VE_SIZET_MAX);
	VE_ASSERT(pcFormat);

	pcDest[0] = 0;

	bool bTruncate = (stCount == VE_SIZET_MAX);

#if _MSC_VER >= 1400
	int iRet = vsnprintf_s(pcDest, stDestSize, stCount, pcFormat, kArgs);
#else
	if (bTruncate)
	{
		stCount = stDestSize - 1;
	}
	int iRet = vsnprintf(pcDest, stCount, pcFormat, kArgs);
#endif

	if (iRet == -1 && !bTruncate)
	{
		iRet = (int)stCount;
	}

#if _MSC_VER >= 1400

#else
	if (iRet == -1)
	{
		pcDest[stDestSize - 1] = 0;
	}
	else
	{
		pcDest[iRet] = 0;
	}
#endif

	return iRet;
}
//--------------------------------------------------------------------------
VeInt32 VeMemcpy(void* pvDest, VeSizeT stDestSize, const void* pvSrc,
	VeSizeT stCount)
{
	VeInt32 iRet = 0;

#if _MSC_VER >= 1400
	iRet = memcpy_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
	if (stDestSize < stCount)
		iRet = -1;
	else
		memcpy(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

	VE_ASSERT(iRet != -1);
	return iRet;
}
//--------------------------------------------------------------------------
VeInt32 VeMemcpy(void* pvDest, const void* pvSrc, VeSizeT stCount)
{
	return VeMemcpy(pvDest, stCount, pvSrc, stCount);
}
//--------------------------------------------------------------------------
VeInt32 VeMemmove(void* pvDest, VeSizeT stDestSize, const void* pvSrc,
	VeSizeT stCount)
{
	VeInt32 iRet = 0;

#if _MSC_VER >= 1400
	iRet = memmove_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
	if (stDestSize < stCount)
		iRet = -1;
	else
		memmove(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

	VE_ASSERT(iRet != -1);
	return iRet;
}
//--------------------------------------------------------------------------
VeInt32 VeWStricmp(const VeChar16* dst, const VeChar16* src)
{
	VE_ASSERT(dst && src);
	VeInt32 f,l;
	do
	{
		if(((f = (VeUInt16)(*(dst++))) >= 'A') && (f <= 'Z') )
			f -= ('A' - 'a');

		if(((l = (VeUInt16)(*(src++))) >= 'A') && (l <= 'Z'))
			l -= ('A' - 'a');
	}
	while(f && (f == l));
	return(f - l);
}
//--------------------------------------------------------------------------
VeInt32 VeWStrnicmp(const VeChar16* dst, const VeChar16* src, VeInt32 count)
{
	VE_ASSERT(dst && src);
	VeInt32 f,l;
	if(count)
	{
		do
		{
			if(((f = (VeUInt16)(*(dst++))) >= 'A') && (f <= 'Z'))
				f -= 'A' - 'a';

			if(((l = (VeUInt16)(*(src++))) >= 'A') && (l <= 'Z'))
				l -= 'A' - 'a';
		}
		while(--count && f && (f == l));

		return(f - l);
	}

	return(0);
}
//--------------------------------------------------------------------------
VeChar16* VeWStrcpy(VeChar16* pkDest, VeSizeT stDestSize,
	const VeChar16* pkSrc)
{
#if _MSC_VER >= 1400
	wcscpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
	return pkDest;
#else // #if _MSC_VER >= 1400
	VE_ASSERT(stDestSize != 0);

	VeSizeT stSrcLen = wcslen(pkSrc);
	VE_ASSERT(stDestSize > stSrcLen); // > because need null character

	VeSizeT stWrite;

	if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
		stWrite = stDestSize;
	else 
		stWrite = stSrcLen + 1;

	VeChar16* pkResult = wcsncpy(pkDest, pkSrc, stWrite);
	pkResult[stDestSize - 1] = 0;
	return pkResult;
#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeChar16* VeWStrncpy(VeChar16* pkDest, VeSizeT stDestSize,
	const VeChar16* pkSrc, VeSizeT stCount)
{
#if _MSC_VER >= 1400
	wcsncpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc, stCount);
	return pkDest;
#else // #if _MSC_VER >= 1400
	VE_ASSERT(pkDest != 0 && stDestSize != 0);
	VE_ASSERT(stCount < stDestSize || stCount == VE_SIZET_MAX);

	if (stCount >= stDestSize)
	{
		if (stCount != VE_SIZET_MAX)
		{
			pkDest[0] = 0;
			return pkDest;
		}
		else
		{
			stCount = stDestSize - 1;
		}
	}

	VeChar16* pkResult = wcsncpy(pkDest, pkSrc, stCount);
	pkResult[stCount] = 0;

	return pkResult;
#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeChar16* VeWStrcat(VeChar16* pkDest, VeSizeT stDestSize,
	const VeChar16* pkSrc)
{
#if _MSC_VER >= 1400
	wcscat_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
	return pkDest;
#else // #if _MSC_VER >= 1400
#ifdef VE_DEBUG
	VeSizeT stSrcLen = wcslen(pkSrc);
#endif
	VeSizeT stDestLen = wcslen(pkDest);
	VE_ASSERT(stSrcLen + stDestLen <= stDestSize - 1);

	VeSizeT stWrite = stDestSize - 1 - stDestLen;  

	VeChar16* pkResult = wcsncat(pkDest, pkSrc, stWrite);
	pkResult[stDestSize - 1] = 0;
	return pkResult;

#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeChar16* VeWStrtok(VeChar16* pkString, const VeChar16* pkDelimit,
	VeChar16** ppkContext)
{
#if _MSC_VER >= 1400
	return (VeChar16 *)wcstok_s((VeChar16 *)pkString,
		(const VeChar16 *)pkDelimit, (VeChar16 **)ppkContext);
#else // #if _MSC_VER >= 1400
	return wcstok(pkString, pkDelimit, ppkContext);
#endif // #if _MSC_VER >= 1400
}
//--------------------------------------------------------------------------
VeInt32 VeWSprintf(VeChar16* pkDest, VeSizeT stDestSize,
	const VeChar16* pkFormat, ...)
{
	if (stDestSize == 0)
		return 0;

	va_list kArgs;
	va_start(kArgs, pkFormat);

#if _MSC_VER >= 1400
	VeInt32 iRet = vswprintf_s((VeChar16 *)pkDest, stDestSize,
		(const VeChar16 *)pkFormat, kArgs);
#else // #if _MSC_VER >= 1400
	VeInt32 iRet = vswprintf(pkDest, stDestSize, pkFormat, kArgs);

	if (iRet >= 0 && ((VeUInt32)iRet == stDestSize - 1) &&
		pkDest[stDestSize - 1] != 0)
	{
		iRet = -1;
	}
#endif // #if _MSC_VER >= 1400

	va_end(kArgs);
	pkDest[stDestSize - 1] = 0;

	return iRet;
}
//--------------------------------------------------------------------------
#define VE_BUFFER_SIZE_IDX		0
#define VE_REF_COUNT_IDX		1
#define VE_STRING_LENGTH_IDX	2
#define VE_HEADER_OFFSET		(3*sizeof(VeUInt32))
//--------------------------------------------------------------------------
VeStringA::StringHandle VeStringA::Allocate(VeUInt32 u32StrLength)
{
	if(u32StrLength == 0) u32StrLength = 1;

	VeUInt32 u32BufferSize = u32StrLength * sizeof(VeChar8)
		+ VE_HEADER_OFFSET + sizeof(VeChar8);
	u32BufferSize = GetBestBufferSize(u32BufferSize);

	VeChar8* pcBuffer = VeAlloc(VeChar8, u32BufferSize);
	VeUInt32* pu32Buffer = (VeUInt32*)pcBuffer;
	pu32Buffer[VE_BUFFER_SIZE_IDX] = u32BufferSize;
	pu32Buffer[VE_REF_COUNT_IDX] = 1;
	pu32Buffer[VE_STRING_LENGTH_IDX] = 0;
	return pcBuffer + VE_HEADER_OFFSET;
}
//--------------------------------------------------------------------------
VeStringA::StringHandle VeStringA::AllocateAndCopy(const VeChar8* pcString)
{
	if(pcString == NULL) return NULL;

	VeUInt32 u32Length = (VeUInt32)VeStrlen(pcString);
	StringHandle hHandle = Allocate(u32Length);
	
	if(hHandle == NULL) return NULL;

	VeCrazyCopy(hHandle, pcString, u32Length + 1);
	VE_ASSERT(VeStrcmp(hHandle, pcString) == 0);

	VeUInt32* pu32Buffer = (VeUInt32*)GetRealBufferStart(hHandle);
	pu32Buffer[VE_STRING_LENGTH_IDX] = u32Length;
	return hHandle;
}
//--------------------------------------------------------------------------
VeStringA::StringHandle VeStringA::AllocateAndCopyHandle(
	StringHandle hHandle)
{
	if(hHandle == NULL) return NULL;
	VeUInt32 u32Length = GetLength(hHandle);
	VeUInt32 u32BufferSize = GetAllocationSize(hHandle);
	VeChar8* pcBuffer = VeAlloc(VeChar8, u32BufferSize);
	VeUInt32* pu32Buffer = (VeUInt32*)pcBuffer;
	pu32Buffer[VE_BUFFER_SIZE_IDX] = u32BufferSize;
	pu32Buffer[VE_REF_COUNT_IDX] = 1;
	pu32Buffer[VE_STRING_LENGTH_IDX] = u32Length;
	StringHandle kNewHandle = pcBuffer + VE_HEADER_OFFSET;
	VeCrazyCopy(kNewHandle, hHandle, u32Length + 1);
	VE_ASSERT(VeStrcmp(kNewHandle, hHandle) == 0);
	return kNewHandle;
}
//--------------------------------------------------------------------------
void VeStringA::Deallocate(StringHandle& hHandle)
{
	if(hHandle)
	{
		VeChar8* pcMem = GetRealBufferStart(hHandle); 
		VeFree(pcMem);
		hHandle = NULL;
	}
}
//--------------------------------------------------------------------------
void VeStringA::IncRefCount(StringHandle& hHandle)
{
	if(hHandle == NULL) return;
	VE_ASSERT(ValidateString(hHandle));
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeAtomicIncrement32(pu32Mem[VE_REF_COUNT_IDX]);
}
//--------------------------------------------------------------------------
void VeStringA::DecRefCount(StringHandle& hHandle)
{
	if(hHandle == NULL) return;
	VE_ASSERT(ValidateString(hHandle));
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeAtomicDecrement32(pu32Mem[VE_REF_COUNT_IDX]);
	if(pu32Mem[VE_REF_COUNT_IDX] == 0) Deallocate(hHandle);
	hHandle = NULL;
}
//--------------------------------------------------------------------------
void VeStringA::Swap(StringHandle& hHandle, const VeChar8* pcNewValue)
{
	if(pcNewValue == NULL)
	{
		DecRefCount(hHandle);
		return;
	}

	if(hHandle == NULL)
	{
		hHandle = AllocateAndCopy(pcNewValue);
		return;
	}

	VE_ASSERT(ValidateString(hHandle));
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeAtomicDecrement32(pu32Mem[VE_REF_COUNT_IDX]);

	if(pu32Mem[VE_REF_COUNT_IDX] == 0)
	{
		VeUInt32 u32Length = (VeUInt32)VeStrlen(pcNewValue);
		if(pu32Mem[VE_BUFFER_SIZE_IDX] >= (u32Length + 1 + VE_HEADER_OFFSET))
		{
			pu32Mem[VE_REF_COUNT_IDX] = 1;
		}
		else
		{
			Deallocate(hHandle);
			hHandle = Allocate(u32Length);
			pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		}

		VeCrazyCopy(hHandle, pcNewValue, u32Length + 1);
		VE_ASSERT(VeStrcmp(hHandle, pcNewValue) == 0);

		pu32Mem[VE_STRING_LENGTH_IDX] = u32Length;
	}
	else
	{
		hHandle = AllocateAndCopy(pcNewValue);
	}
}
//--------------------------------------------------------------------------
VeChar8* VeStringA::GetString(const StringHandle& hHandle)
{
	VE_ASSERT(ValidateString(hHandle));
	return hHandle;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::GetLength(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_STRING_LENGTH_IDX];
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::GetRefCount(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_REF_COUNT_IDX];
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::GetAllocationSize(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_BUFFER_SIZE_IDX];
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::GetBufferSize(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_BUFFER_SIZE_IDX] - VE_HEADER_OFFSET;
	}
}
//--------------------------------------------------------------------------
void VeStringA::SetLength(const StringHandle& hHandle, VeUInt32 u32Length)
{
	if(hHandle == NULL) return;
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	pu32Mem[VE_STRING_LENGTH_IDX] = u32Length;
	VE_ASSERT(pu32Mem[VE_STRING_LENGTH_IDX] < pu32Mem[VE_BUFFER_SIZE_IDX]);
}
//--------------------------------------------------------------------------
VeChar8* VeStringA::GetRealBufferStart(const StringHandle& hHandle)
{
	VE_ASSERT(hHandle);
	return hHandle - VE_HEADER_OFFSET;
}
//--------------------------------------------------------------------------
bool VeStringA::ValidateString(const StringHandle& hHandle)
{
	if(hHandle == NULL) return true;
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeUInt32 u32Length = pu32Mem[VE_STRING_LENGTH_IDX];
	if(u32Length == (VeUInt32)VeStrlen(hHandle)) return true;
	else return false;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::GetBestBufferSize(VeUInt32 u32ReqSize)
{
	if (u32ReqSize < 32)
		return 32;
	else if (u32ReqSize < 64)
		return 64;
	else if (u32ReqSize < 128)
		return 128;
	else if (u32ReqSize < 256)
		return 255;
	else if (u32ReqSize < 512)
		return 512;
	else if (u32ReqSize < 1024)
		return 1024;
	else 
		return ((u32ReqSize + 0xF) >> 4) << 4;
}
//--------------------------------------------------------------------------
VeStringA::VeStringA(VeUInt32 u32Length)
{
	m_hHandle = Allocate(u32Length);
	m_hHandle[0] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringA::VeStringA(VeChar8 c8Char, VeUInt32 u32Length)
{
	m_hHandle = Allocate(u32Length);
	VeMemorySet(m_hHandle, c8Char, u32Length);
	m_hHandle[u32Length] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringA::VeStringA(const VeChar8* pc8Str)
{
	m_hHandle = AllocateAndCopy(pc8Str);
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringA::VeStringA(const VeChar8* pc8Str, VeUInt32 u32Length)
{
	m_hHandle = Allocate(u32Length);
	VeCrazyCopy(m_hHandle, pc8Str, u32Length);
	m_hHandle[u32Length] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringA::VeStringA(const VeStringA& kString)
{
	m_hHandle = kString.m_hHandle;
	IncRefCount(m_hHandle);
}
//--------------------------------------------------------------------------
VeStringA::VeStringA(const VeFixedStringA& kStr)
{
	m_hHandle = AllocateAndCopy((const VeChar8*)kStr);
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringA::~VeStringA()
{
	DecRefCount(m_hHandle);
}
//--------------------------------------------------------------------------
bool VeStringA::Resize(VeUInt32 u32Delta)
{
	VeUInt32 u32NewStringLength = Length() + u32Delta;
	if(((u32NewStringLength + 1) * sizeof(VeChar8)) >
		GetBufferSize(m_hHandle))
	{
		StringHandle kNewHandle = Allocate(u32NewStringLength);
		VeUInt32 u32EndLoc = Length();
		VeCrazyCopy(kNewHandle, m_hHandle, Length() * sizeof(VeChar8));
		kNewHandle[u32EndLoc] = 0;
		DecRefCount(m_hHandle);
		m_hHandle = kNewHandle;
		CalcLength();
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------
void VeStringA::SetBuffer(StringHandle& hHandle)
{
	if(hHandle == m_hHandle) return;
	DecRefCount(m_hHandle);
	IncRefCount(hHandle);
	m_hHandle = hHandle;
}
//--------------------------------------------------------------------------
void VeStringA::CalcLength()
{
	if(m_hHandle == NULL) return;
	SetLength(m_hHandle, (VeUInt32)VeStrlen(m_hHandle));
}
//--------------------------------------------------------------------------
void VeStringA::CopyOnWrite(bool bForceCopy)
{
	if((GetRefCount(m_hHandle) > 1) || bForceCopy)
	{
		StringHandle hHandle = m_hHandle;
		m_hHandle = AllocateAndCopyHandle(m_hHandle);
		DecRefCount(hHandle);
	}
}
//--------------------------------------------------------------------------
void VeStringA::CopyOnWriteAndResize(VeUInt32 u32SizeDelta, bool bForceCopy)
{
	if(Resize(u32SizeDelta)) return;

	if(GetRefCount(m_hHandle) > 1 || bForceCopy)
	{
		StringHandle kHandle = m_hHandle;
		m_hHandle = AllocateAndCopyHandle(m_hHandle);
		DecRefCount(kHandle);
	}
}
//--------------------------------------------------------------------------
VeChar8* VeStringA::ConsumeWhitespace(const VeChar8* pcStr,
	VeUInt32 u32Length) const
{
	bool bContinue = true;
	VeUInt32 u32Idx;
	for(u32Idx = 0; u32Idx < u32Length && bContinue; u32Idx++)
	{
		if(pcStr[u32Idx] != '\t' && pcStr[u32Idx] != ' ')
			bContinue = false;
	}
	return (VeChar8*)pcStr + u32Idx - 1;
}
//--------------------------------------------------------------------------
void VeStringA::Format(const VeChar8* pcFmt, va_list argPtr)
{
	VeChar8 acBuffer[VE_MAX_FORMAT_SIZE];
	VeInt32 i32NumChars = VeVsprintf(acBuffer, VE_MAX_FORMAT_SIZE,
		pcFmt, argPtr);
	VE_ASSERT(i32NumChars >= 0);
	if((VeUInt32)i32NumChars >= GetBufferSize(m_hHandle))
	{
		Deallocate(m_hHandle);
		m_hHandle = Allocate(i32NumChars + 1);
	}
	VeCrazyCopy(m_hHandle, acBuffer, i32NumChars);
	m_hHandle[i32NumChars] = 0;
}
//--------------------------------------------------------------------------
void VeStringA::Format( const VeChar8* pcFormatString ... )
{
	CopyOnWrite(true);
	va_list argList;
	va_start(argList, pcFormatString);
	Format(pcFormatString, argList);
	va_end(argList);
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::Length() const
{
	return GetLength(m_hHandle);
}
//--------------------------------------------------------------------------
bool VeStringA::IsEmpty() const
{
	return Length() == 0;
}
//--------------------------------------------------------------------------
void VeStringA::Empty()
{
	DecRefCount(m_hHandle);
}
//--------------------------------------------------------------------------
VeChar8 VeStringA::GetAt(VeUInt32 u32Index) const
{
	VeChar8 c;
	if (u32Index >= Length())
		c = 0;
	else
		c = GetString(m_hHandle)[u32Index];
	return c;
}
//--------------------------------------------------------------------------
void VeStringA::SetAt(VeUInt32 u32Index, VeChar8 c)
{
	if(u32Index >= Length()) return;

	CopyOnWrite();
	GetString(m_hHandle)[u32Index] = c;
	if(c == 0)
	{
		SetLength(m_hHandle, u32Index);
	}
}
//--------------------------------------------------------------------------
VeInt32 VeStringA::Compare(const VeChar8* pc8String) const
{
	if(pc8String == NULL && m_hHandle == NULL)
		return 0;
	else if(pc8String == NULL)
		return GetString(m_hHandle)[0];
	else if (m_hHandle == NULL)
		return -pc8String[0];
	return VeStrcmp(GetString(m_hHandle), pc8String);
}
//--------------------------------------------------------------------------
VeInt32 VeStringA::CompareNoCase(const VeChar8* pc8String) const
{
	if(pc8String == NULL && m_hHandle == (StringHandle) NULL)
		return 0;
	else if (pc8String == NULL)
		return GetString(m_hHandle)[0];
	else if (m_hHandle == (StringHandle) NULL)
		return -pc8String[0];
	return VeStricmp(GetString(m_hHandle), pc8String);
}
//--------------------------------------------------------------------------
bool VeStringA::Equals(const VeChar8* pc8Str) const
{
	if(pc8Str == NULL)
		return m_hHandle == (StringHandle)NULL;
	else if(VeStrlen(pc8Str) != Length())
		return false;

	return Compare(pc8Str) == 0;
}
//--------------------------------------------------------------------------
bool VeStringA::EqualsNoCase(const VeChar8* pc8Str) const
{
	if(pc8Str == NULL)
		return m_hHandle == NULL;
	else if(VeStrlen(pc8Str) != Length())
		return false;
	return CompareNoCase(pc8Str) == 0;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::GetSubstring(VeUInt32 u32Begin, VeUInt32 u32End) const
{
	if (u32Begin < u32End && u32Begin < Length()
		&& u32End <= Length() && m_hHandle != NULL)
	{
		VeStringA kString((u32End - u32Begin + 2));
		VeStrncpy(kString.m_hHandle,
			kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + u32Begin, u32End - u32Begin);
		kString.m_hHandle[u32End - u32Begin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar8*)NULL;
	}
}
//--------------------------------------------------------------------------
VeStringA VeStringA::GetSubstring(VeUInt32 u32Begin,
	const VeChar8* pc8Delimeters) const
{
	if (u32Begin < Length() && m_hHandle != NULL && pc8Delimeters != NULL)
	{
		VeChar8* pcStr = VeStrpbrk(GetString(m_hHandle)
			+ u32Begin, pc8Delimeters);
		if (pcStr == NULL)
			return (const VeChar8*)NULL;
		else
			return GetSubstring(u32Begin,
			(VeUInt32)(pcStr - GetString(m_hHandle)));
	}
	else
	{
		return (const VeChar8*)NULL;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::Find(VeChar8 c, VeUInt32 u32Start) const
{
	if (u32Start >= Length())
		return VE_INFINITE;

	VeChar8* pcStr = VeStrchr(GetString(m_hHandle) + u32Start, c);
	if (pcStr == NULL)
		return VE_INFINITE;

	VeInt32 iReturn = (VeInt32)(pcStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32)iReturn;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::Find(const VeChar8* pc8Str, VeUInt32 u32Start) const
{
	if (u32Start >= Length())
		return VE_INFINITE;
	if (pc8Str == NULL || pc8Str[0] == 0)
		return VE_INFINITE;
	VeChar8* pcSubStr = VeStrstr(GetString(m_hHandle) + u32Start, pc8Str);
	if (pcSubStr == NULL)
		return VE_INFINITE;

	VeInt32 iReturn = (VeInt32)(pcSubStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32)iReturn;
}
//--------------------------------------------------------------------------
bool VeStringA::Contains(const VeChar8* pc8Str) const
{
	return Find(pc8Str) != VE_INFINITE;
}
//--------------------------------------------------------------------------
bool VeStringA::ContainsNoCase(const VeChar8* pc8Str) const
{
	if (m_hHandle == pc8Str && pc8Str != NULL)
		return true;

	if (pc8Str == NULL || m_hHandle == NULL)
		return false;

	const VeChar8* pcMyString = (const VeChar8*) m_hHandle;
	VeSizeT stOtherLength = VeStrlen(pc8Str);
	for (VeUInt32 uiMyIdx = 0; uiMyIdx < Length(); uiMyIdx++)
	{
		if (VeStrnicmp(&pcMyString[uiMyIdx], pc8Str,
			(VeInt32)stOtherLength) == 0)
			return true;
	}
	return false;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::FindReverse(VeChar8 c) const
{
	if (m_hHandle == NULL)
		return VE_INFINITE;

	VeChar8* pcStr = VeStrrchr(GetString(m_hHandle), c);
	if (pcStr == NULL)
		return VE_INFINITE;
	else if (c == 0)
		return VE_INFINITE;
	VeInt32 iReturn = (VeInt32)(pcStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32) iReturn;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::FindReverse(VeChar8 c, VeUInt32 u32StartIdx) const
{
	if (m_hHandle == NULL || c == 0)
		return VE_INFINITE;

	if (u32StartIdx >= Length())
		u32StartIdx = Length() - 1;

	const VeChar8* pcValue = GetString(m_hHandle);
	while (u32StartIdx != VE_INFINITE)
	{
		if (pcValue[u32StartIdx] == c)
			return u32StartIdx;
		--u32StartIdx;
	}

	 return VE_INFINITE;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::FindOneOf(const VeChar8* pc8Str, VeUInt32 uiIndex) const
{
	if (pc8Str == NULL || pc8Str == 0 || m_hHandle == NULL)
		return VE_INFINITE;
	VeChar8* pcSubStr = VeStrpbrk(GetString(m_hHandle) + uiIndex, pc8Str);
	if (pcSubStr == NULL)
		return VE_INFINITE;
	VeInt32 iReturn = (VeInt32)(pcSubStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32) iReturn;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::Mid(VeUInt32 u32Begin, VeUInt32 u32Count) const
{
	VeUInt32 u32End = u32Begin + u32Count;
	if (u32End > Length() || u32Count == VE_INFINITE)
		u32End = Length();
	if (u32Begin < u32End && u32Begin < Length() && 
		u32End <= Length())
	{
		VeStringA kString(u32End - u32Begin + 2);
		VeStrncpy(kString.m_hHandle,
			kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + u32Begin, u32End - u32Begin);
		kString.m_hHandle[u32End - u32Begin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar8*)NULL;
	}
}
//--------------------------------------------------------------------------
VeStringA VeStringA::Left(VeUInt32 u32Count) const
{
	VeUInt32 u32Begin = 0;
	VeUInt32 u32End = u32Begin + u32Count;
	if (u32End > Length())
		u32End = Length();
	if (u32Begin < u32End && u32Begin < Length() && 
		u32End <= Length())
	{
		VeStringA kString(u32End - u32Begin + 2);
		VeStrncpy(kString.m_hHandle,
			kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + u32Begin, u32End - u32Begin);
		kString.m_hHandle[u32End - u32Begin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar8*)NULL;
	}
}
//--------------------------------------------------------------------------
VeStringA VeStringA::Right(VeUInt32 u32Count) const
{
	VeUInt32 uiEnd = Length();
	VeInt32 iBegin = uiEnd - u32Count;
	VeUInt32 uiBegin = 0;

	if (iBegin > 0)
		uiBegin = (VeUInt32) iBegin;

	if (uiEnd > Length())
		uiEnd = Length();
	if (uiBegin < uiEnd && uiBegin < Length() && 
		uiEnd <= Length())
	{
		VeStringA kString(uiEnd - uiBegin + 2);
		VeStrncpy(kString.m_hHandle,
			kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + uiBegin, uiEnd - uiBegin);
		kString.m_hHandle[uiEnd - uiBegin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar8*)NULL;
	}
}
//--------------------------------------------------------------------------
void VeStringA::Replace(const VeChar8* pcWhatToReplace,
	const VeChar8* pcReplaceWith)
{
	if (pcWhatToReplace == NULL || pcReplaceWith == NULL)
		return;

	VeUInt32 stWhatToReplaceLength = (VeUInt32)VeStrlen(pcWhatToReplace);
	VeUInt32 stReplaceWithLength = (VeUInt32)VeStrlen(pcReplaceWith);
	VeUInt32 uiOccuranceCount = 0;
	VeUInt32 uiOriginalLength = Length();

	VeUInt32 ui = 0;
	for (; ui < uiOriginalLength && ui != VE_INFINITE;)
	{
		ui = Find(pcWhatToReplace, ui);
		if (ui != VE_INFINITE)
		{
			uiOccuranceCount++;
			ui = (VeUInt32)(ui + stWhatToReplaceLength);
		}
	}

	if (uiOccuranceCount == 0)
		return;

	VeUInt32 uiNumCharsToReplace = 
		uiOccuranceCount * stWhatToReplaceLength;
	VeUInt32 uiNumCharsNeededToReplace = 
		uiOccuranceCount * stReplaceWithLength;
	VeUInt32 uiSizeNeeded = 
		uiOriginalLength - uiNumCharsToReplace + uiNumCharsNeededToReplace;

	if (uiSizeNeeded > uiOriginalLength)
		CopyOnWriteAndResize(uiSizeNeeded - uiOriginalLength);
	else
		CopyOnWrite();

	for(ui = 0; ui < uiSizeNeeded && uiOccuranceCount > 0; uiOccuranceCount--)
	{
		VeChar8* pcStart = VeStrstr(m_hHandle + ui, pcWhatToReplace);
		VE_ASSERT(pcStart != NULL);
		VeChar8* pcEnd = pcStart + stReplaceWithLength;

		VeUInt32 u32Size = (VeUInt32)VeStrlen(pcStart
			+ stWhatToReplaceLength) + 1;

		VeMemmove(pcStart + stReplaceWithLength, u32Size, pcStart
			+ stWhatToReplaceLength, u32Size);
		VeMemcpy(pcStart, pcReplaceWith, stReplaceWithLength);
		ui = (VeUInt32)(pcEnd - m_hHandle);
	}

	m_hHandle[uiSizeNeeded] = 0;

	CalcLength();
}
//--------------------------------------------------------------------------
void VeStringA::Insert(const VeChar8* pcInsertString, VeUInt32 u32Index)
{
	if(pcInsertString == NULL)
		return;

	VeUInt32 stOriginalLength = Length();
	VeUInt32 stIndex = u32Index;
	VeUInt32 stInsertLength = (VeUInt32)VeStrlen(pcInsertString);

#if defined(VE_DEBUG)
	const VeChar8* pcOriginalString = GetString(m_hHandle);
	VE_ASSERT(!(pcInsertString >= pcOriginalString && 
		pcInsertString <= pcOriginalString + stOriginalLength));
#endif

	if (stIndex >= stOriginalLength)
	{
		Concatenate(pcInsertString);
		return;
	}

	VeUInt32 stNewSize = stInsertLength + stOriginalLength + 1;

	CopyOnWriteAndResize(stInsertLength);
	VeChar8* pcBuffer = GetString(m_hHandle);
	VeMemmove(&pcBuffer[stIndex + stInsertLength], 
		stNewSize - stIndex - stInsertLength,
		&pcBuffer[stIndex], stOriginalLength - stIndex + 1);
	VeMemmove(&pcBuffer[stIndex], stNewSize - stIndex,
		pcInsertString, stInsertLength);
	CalcLength();
}
//--------------------------------------------------------------------------
void VeStringA::RemoveRange(VeUInt32 u32StartIdx, VeUInt32 u32NumChars)
{
	VeUInt32 uiEnd = Length();
	if (u32StartIdx >= uiEnd)
		return;

	if (u32StartIdx + u32NumChars > uiEnd)
		u32NumChars = uiEnd - u32StartIdx;

	CopyOnWrite();
	VeChar8* pcBuffer = GetString(m_hHandle);
	VeMemmove(&pcBuffer[u32StartIdx], uiEnd - u32StartIdx + 1,
		&pcBuffer[u32StartIdx + u32NumChars], 
		uiEnd - u32StartIdx - u32NumChars + 1);
	CalcLength();
}
//--------------------------------------------------------------------------
void VeStringA::ToUpper()
{
	if (m_hHandle == NULL)
		return;

	CopyOnWrite();
	for ( VeUInt32 ui = 0; ui < Length() ; ui++)
	{
		VeChar8 c = GetString(m_hHandle)[ui];
		if ( ('a' <= c) && (c <= 'z') )
			GetString(m_hHandle)[ui] -= 'a' - 'A';
	}
}
//--------------------------------------------------------------------------
void VeStringA::ToLower()
{
	if (m_hHandle == NULL)
		return;

	CopyOnWrite();
	for ( VeUInt32 ui = 0; ui < Length() ; ui++)
	{
		VeChar8 c = GetString(m_hHandle)[ui];
		if ( ('A' <= c) && (c <= 'Z') )
			GetString(m_hHandle)[ui] -= 'A' - 'a';
	}
}
//--------------------------------------------------------------------------
void VeStringA::Reverse()
{
	if (Length() < 2)
		return;

	CopyOnWrite();
	VeUInt32 uiEnd = Length() - 1;
	for (VeUInt32 uiBegin = 0; uiBegin < uiEnd; 
		uiBegin++, uiEnd--)
	{
		VeChar8 c = GetString(m_hHandle)[uiBegin];
		GetString(m_hHandle)[uiBegin] = GetString(m_hHandle)[uiEnd];
		GetString(m_hHandle)[uiEnd] = c;
	}
}
//--------------------------------------------------------------------------
void VeStringA::TrimLeft(VeChar8 c)
{
	if ( c == 0 || m_hHandle == NULL)
		return;

	CopyOnWrite();
	VeUInt32 uiTrimCount = 0;
	bool bContinue = true;
	for (VeUInt32 ui = 0; 
		ui < Length() && bContinue; ui++)
	{
		if (GetString(m_hHandle)[ui] == c)
			uiTrimCount++;
		else
			bContinue = false;
	}

	if (uiTrimCount > 0)
	{
		VeUInt32 u32Size = Length() - uiTrimCount + 1;
		VeMemmove(GetString(m_hHandle), u32Size, GetString(m_hHandle)
			+ uiTrimCount, u32Size);
		CalcLength();
	}
}
//--------------------------------------------------------------------------
void VeStringA::TrimRight(VeChar8 c)
{
	if (c == 0 || m_hHandle == NULL)
		return;

	CopyOnWrite();
	VeUInt32 uiTrimCount = 0;
	bool bContinue = true;
	for(VeInt32 i = Length() - 1; i >= 0 && bContinue; i--)
	{
		if (GetString(m_hHandle)[i] == c)
			uiTrimCount++;
		else
			bContinue = false;
	}

	if (uiTrimCount > 0)
	{
		VeChar8* pcStr = GetString(m_hHandle) + Length() - uiTrimCount;
		pcStr[0] = 0;
		CalcLength();
	}
}
//--------------------------------------------------------------------------
void VeStringA::Concatenate(const VeChar8* pcStr)
{
	if (pcStr == NULL)
		return;

	VeUInt32 stLen = (VeUInt32)VeStrlen(pcStr);
	if (stLen > 0)
	{
		StringHandle kHandle = m_hHandle;
		bool bInternalIncrementRefCount = false;
		if (pcStr == GetString(m_hHandle))
		{
			IncRefCount(kHandle);
			CopyOnWriteAndResize(stLen, true);
			bInternalIncrementRefCount = true;
		}
		else
		{
			CopyOnWriteAndResize(stLen, false);
		}

		VeStrcpy(GetString(m_hHandle) + Length(), stLen + 1, pcStr);
		CalcLength();
		if (bInternalIncrementRefCount)
			DecRefCount(kHandle);
	}
}
//--------------------------------------------------------------------------
void VeStringA::Concatenate(VeChar8 ch)
{
	CopyOnWriteAndResize(1);
	VeUInt32 stLength = Length();

	m_hHandle[stLength] = ch;
	m_hHandle[stLength+1] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
bool VeStringA::ToBool(bool& b) const
{
	VeChar8* pcStr = ConsumeWhitespace(GetString(m_hHandle), Length());

	if(VeStrlen(pcStr) < 4)
	{
		return false;
	}
	else if(VeStrnicmp(pcStr, "true", 4) == 0)
	{
		b = true;
		return true;
	}
	else if(VeStrnicmp(pcStr, "false", 4) == 0)
	{
		b = false;
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------
bool VeStringA::To(bool& b) const
{
	return ToBool(b);
}
//--------------------------------------------------------------------------
bool VeStringA::ToFloat(VeFloat32& f) const
{
	VeChar8* pcString = GetString(m_hHandle);

	if(pcString == NULL)
		return false;

	f = (VeFloat32)VeAtof(pcString);
	if(f == 0)
	{
		if(FindOneOf("-0") != VE_INFINITE)
			return true;
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeFloat32& f) const
{
	return ToFloat(f);
}

bool VeStringA::ToInt(VeInt32& i) const
{
	VeChar8* pcString = GetString(m_hHandle);

	if(pcString == NULL)
		return false;

	i = VeAtoi(pcString);
	if(i == 0)
	{
		if(FindOneOf("-0") != VE_INFINITE)
			return true;
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeInt32& i) const
{
	return ToInt(i);
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeInt16& i) const
{
	VeInt32 iTemp = 0;
	bool bResult = ToInt(iTemp);
	i = (VeInt16)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeInt8& i) const
{
	VeInt32 iTemp = 0;
	bool bResult = ToInt(iTemp);
	i = (VeInt8)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringA::ToUInt(VeUInt32& ui) const
{
	VeChar8* pcString = GetString(m_hHandle);

	if(pcString == NULL)
		return false;

	ui = VeAtoi(pcString);
	if(ui == 0)
	{
		if(FindOneOf("-0") != VE_INFINITE)
			return true;
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeUInt32& ui) const
{
	return ToUInt(ui);
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeUInt16& ui) const
{
	VeUInt32 iTemp = 0;
	bool bResult = ToUInt(iTemp);
	ui = (VeUInt16)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeUInt8& ui) const
{
	VeUInt32 iTemp = 0;
	bool bResult = ToUInt(iTemp);
	ui = (VeUInt8)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringA::ToVector2D(VeVector2D& kVec) const
{
	const VeChar8 acSeps [] = ",";
	VeChar8* pcContext;
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringA kString(VeStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kVec.x))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString2(VeStrtok(NULL, acSeps, &pcContext));
	if(!kString2.ToFloat(kVec.y))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeVector2D& kVec) const
{
	return ToVector2D(kVec);
}
//--------------------------------------------------------------------------
bool VeStringA::ToVector3D(VeVector3D& kVec) const
{
	const VeChar8 acSeps [] = ",";
	VeChar8* pcContext;
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringA kString(VeStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kVec.x))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString2(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString2.ToFloat(kVec.y))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString3(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString3.ToFloat(kVec.z))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeVector3D& kVec) const
{
	return ToVector3D(kVec);
}
//--------------------------------------------------------------------------
bool VeStringA::ToVector4D(VeVector4D& kVec) const
{
	const VeChar8 acSeps [] = ",";
	VeChar8* pcContext;
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringA kString(VeStrtok(GetString(kHandle), acSeps, &pcContext));
	if (!kString.ToFloat(kVec.x))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString2(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString2.ToFloat(kVec.y))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString3(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString3.ToFloat(kVec.z))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString4(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString4.ToFloat(kVec.w))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeVector4D& kVec) const
{
	return ToVector4D(kVec);
}
//--------------------------------------------------------------------------
bool VeStringA::ToRGB(VeColor& kColor) const
{
	const VeChar8 acSeps [] = ",";
	VeChar8* pcContext; 
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringA kString(VeStrtok(GetString(kHandle), acSeps, &pcContext));
	if (!kString.ToFloat(kColor.r))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString2(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString2.ToFloat(kColor.g))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString3(VeStrtok(NULL, acSeps, &pcContext));
	if (!kString3.ToFloat(kColor.b))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeColor& kColor) const
{
	return ToRGB(kColor);
}
//--------------------------------------------------------------------------
bool VeStringA::ToRGBA(VeColorA& kColor) const
{
	const VeChar8 acSeps [] = ",";
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeChar8* pcContext;

	VeStringA kString(VeStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kColor.r))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString2(VeStrtok(NULL, acSeps, &pcContext));
	if(!kString2.ToFloat(kColor.g))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString3(VeStrtok(NULL, acSeps, &pcContext));
	if(!kString3.ToFloat(kColor.b))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringA kString4(VeStrtok(NULL, acSeps, &pcContext));
	if(!kString4.ToFloat(kColor.a))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringA::To(VeColorA& kColor) const
{
	return ToRGBA(kColor);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromBool(bool b)
{
	if(b)
		return VeStringA("TRUE");
	else
		return VeStringA("FALSE");
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(bool b)
{
	return FromBool(b);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromFloat(VeFloat32 f)
{
	VeChar8 acString[50];
	VeSprintf(acString, 50, "%f", f);
	VeStringA kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeFloat32 f)
{
	return FromFloat(f);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromInt(VeInt32 i)
{
	VeChar8 acString[32];
	VeSprintf(acString, 32, "%d", i);
	return VeStringA(acString);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeInt32 i)
{
	return FromInt(i);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeInt16 i)
{
	return FromInt(i);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeInt8 i)
{
	return FromInt(i);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromUInt(VeUInt32 ui)
{
	VeChar8 acString[32];
	VeSprintf(acString, 32, "%d", ui);
	return VeStringA(acString);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeUInt32 ui)
{
	return FromUInt(ui);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeUInt16 ui)
{
	return FromUInt(ui);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeUInt8 ui)
{
	return FromUInt(ui);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromVector2D(VeVector2D& kPt)
{
	VeChar8 acString[128];
	VeSprintf(acString, 128, "%f,%f", kPt.x, kPt.y);
	VeStringA kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeVector2D& kPt)
{
	return FromVector2D(kPt);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromVector3D(VeVector3D& kPt)
{
	VeChar8 acString[128];
	VeSprintf(acString, 128, "%f,%f,%f", kPt.x, kPt.y, kPt.z);
	VeStringA kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeVector3D& kPt)
{
	return FromVector3D(kPt);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromVector4D(VeVector4D& kPt)
{
	VeChar8 acString[128];
	VeSprintf(acString, 128, "%f,%f,%f,%f", kPt.x, kPt.y, kPt.z, kPt.w);
	VeStringA kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeVector4D& kPt)
{
	return FromVector4D(kPt);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromRGB(VeColor& kColor)
{
	VeChar8 acString[128];
	VeSprintf(acString, 128, "%f,%f,%f)", kColor.r, kColor.g, kColor.b);
	VeStringA kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeColor& kColor)
{
	return FromRGB(kColor);
}
//--------------------------------------------------------------------------
VeStringA VeStringA::FromRGBA(VeColorA& kColor)
{
	VeChar8 acString[160];
	VeSprintf(acString, 160, "%f,%f,%f,%f)", kColor.r, kColor.g, kColor.b, kColor.a);
	VeStringA kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA VeStringA::From(VeColorA& kColor)
{
	return FromRGBA(kColor);
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::CalculateHash(const VeChar8* pcString)
{
	VeUInt32 u32Hash = 0;

	while(*pcString)
	{
		u32Hash = (*pcString++) + (u32Hash << 6) + (u32Hash << 16) - u32Hash;
	}

	return (u32Hash & 0x7FFFFFFF);
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::KeyToHashCode(const VeChar8* pcString, VeUInt32 u32TableSize)
{
	return CalculateHash(pcString) % u32TableSize;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA::GetHashCode() const
{
	return CalculateHash(GetString(m_hHandle));
}
//--------------------------------------------------------------------------
VeStringA::operator const VeChar8*() const
{
	return GetString(m_hHandle);
}
//--------------------------------------------------------------------------
VeChar8* VeStringA::GetString()
{
	return GetString(m_hHandle);
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator = (VeStringA& stringSrc)
{
	SetBuffer(stringSrc.m_hHandle);
	return *this;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator = (const VeChar8* psz)
{
	if(psz == GetString(m_hHandle))
		return *this;

	Swap(m_hHandle, psz);
	return *this;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator = (const VeFixedStringA& kString)
{
	return VeStringA::operator = ((const VeChar8*)kString);
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator = (VeChar8 ch)
{
	VeChar8 acString[2];
	acString[0] = ch;
	acString[1] = '\0';
	return VeStringA::operator = ((const VeChar8*)acString);
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator += (VeStringA& string)
{
	Concatenate(string);
	return *this;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator += (VeChar8 ch)
{
	Concatenate(ch);
	return *this;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator += (const VeChar8* pcString)
{
	Concatenate(pcString);
	return *this;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA::operator += (const VeFixedStringA& kString)
{
	Concatenate((const VeChar8*)kString);
	return *this;
}
//--------------------------------------------------------------------------
VeStringA operator + (const VeStringA& string1, const VeStringA& string2)
{
	VeStringA kString(string1);
	kString.Concatenate(string2);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA operator + (const VeStringA& string, VeChar8 ch)
{
	VeStringA kString(string);
	kString.Concatenate(ch);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA operator + (VeChar8 ch, const VeStringA& string)
{
	VeStringA kString(ch);
	kString.Concatenate(string);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA operator + (const VeStringA& string, const VeChar8* lpsz)
{
	VeStringA kString(string);
	kString.Concatenate(lpsz);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA operator + (const VeStringA& string, const VeFixedStringA& kString2)
{
	VeStringA kString(string);
	kString.Concatenate((const VeChar8*)kString2);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA operator + (const VeChar8* lpsz, const VeStringA& string)
{
	VeStringA kString(lpsz);
	kString.Concatenate(string);
	return kString;
}
//--------------------------------------------------------------------------
VeStringA operator + (const VeFixedStringA& kStr, const VeStringA& string)
{
	VeStringA kString((const VeChar8*)kStr);
	kString.Concatenate(string);
	return kString;
}
//--------------------------------------------------------------------------
bool operator == (const VeStringA& s1, const VeStringA& s2)
{
	return s1.Compare(s2) == 0;
}
//--------------------------------------------------------------------------
bool operator == (const VeStringA& s1, const VeChar8* s2)
{
	return s1.Compare(s2) == 0;
}
//--------------------------------------------------------------------------
bool operator == (const VeStringA& s1, const VeFixedStringA& s2)
{
	return s1.Compare((const VeChar8*)s2) == 0;
}
//--------------------------------------------------------------------------
bool operator == (const VeChar8* s1, const VeStringA& s2)
{
	return s2.Compare(s1) == 0;
}
//--------------------------------------------------------------------------
bool operator == (const VeFixedStringA& s1, const VeStringA& s2)
{
	return s2.Compare((const VeChar8*)s1) == 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeStringA& s1, const VeStringA& s2)
{
	return s1.Compare(s2) != 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeStringA& s1, const VeChar8* s2)
{
	return s1.Compare(s2) != 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeStringA& s1, const VeFixedStringA& s2)
{
	return s1.Compare((const VeChar8*)s2) != 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeChar8* s1, const VeStringA& s2)
{
	return s2.Compare(s1) != 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeFixedStringA& s1, const VeStringA& s2)
{
	return s2.Compare((const VeChar8*)s1) != 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeStringA& s1, const VeStringA& s2)
{
	return s1.Compare(s2) < 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeStringA& s1, const VeChar8* s2)
{
	return s1.Compare(s2) < 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeStringA& s1, const VeFixedStringA& s2)
{
	return s1.Compare((const VeChar8*)s2) < 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeChar8* s1, const VeStringA& s2)
{
	return s2.Compare(s1) > 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeFixedStringA& s1, const VeStringA& s2)
{
	return s2.Compare((const VeChar8*)s1) > 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeStringA& s1, const VeStringA& s2)
{
	return s1.Compare(s2) > 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeStringA& s1, const VeChar8* s2)
{
	return s1.Compare(s2) > 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeStringA& s1, const VeFixedStringA& s2)
{
	return s1.Compare((const VeChar8*)s2) > 0; 
}
//--------------------------------------------------------------------------
bool operator > (const VeChar8* s1, const VeStringA& s2)
{
	return s2.Compare(s1) < 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeFixedStringA& s1, const VeStringA& s2)
{
	return s2.Compare((const VeChar8*)s1) < 0; 
}
//--------------------------------------------------------------------------
bool operator <= (const VeStringA& s1, const VeStringA& s2)
{
	return s1.Compare(s2) <= 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeStringA& s1, const VeChar8* s2)
{
	return s1.Compare(s2) <= 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeStringA& s1, const VeFixedStringA& s2)
{
	return s1.Compare((const VeChar8*)s2) <= 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeChar8* s1, const VeStringA& s2)
{
	return s2.Compare(s1) >= 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeFixedStringA& s1, const VeStringA& s2)
{
	return s2.Compare((const VeChar8*)s1) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeStringA& s1, const VeStringA& s2)
{
	return s1.Compare(s2) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeStringA& s1, const VeChar8* s2)
{
	return s1.Compare(s2) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeStringA& s1, const VeFixedStringA& s2)
{
	return s1.Compare((const VeChar8*)s2) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeChar8* s1, const VeStringA& s2)
{
	return s2.Compare(s1) <= 0; 
}
//--------------------------------------------------------------------------
bool operator >= (const VeFixedStringA& s1, const VeStringA& s2)
{
	return s2.Compare((const VeChar8*)s1) <= 0;
}
//--------------------------------------------------------------------------
bool IsTrue(VeStringA strStr)
{
	bool b = false;
	if(strStr.ToBool(b))
		return b;

	VE_ASSERT(strStr.ToBool(b));
	return false;
}
//--------------------------------------------------------------------------
VeChar8* VeStringA::MakeExternalCopy()
{
	VeUInt32 uiSize = Length() + 1;
	VeChar8* pcStr = VeAlloc(VeChar8, uiSize);
	VeStrcpy(pcStr, uiSize, GetString(m_hHandle));
	return pcStr;
}
//--------------------------------------------------------------------------
VeStringW::StringHandle VeStringW::Allocate(VeUInt32 u32StrLength)
{
	if(u32StrLength == 0) u32StrLength = 1;

	VeUInt32 u32BufferSize = u32StrLength * sizeof(VeChar16) + VE_HEADER_OFFSET + sizeof(VeChar16);
	u32BufferSize = GetBestBufferSize(u32BufferSize);

	VeChar8* pcBuffer = VeAlloc(VeChar8, u32BufferSize);
	VeUInt32* pu32Buffer = (VeUInt32*)pcBuffer;
	pu32Buffer[VE_BUFFER_SIZE_IDX] = u32BufferSize;
	pu32Buffer[VE_REF_COUNT_IDX] = 1;
	pu32Buffer[VE_STRING_LENGTH_IDX] = 0;
	return (VeChar16*)(pcBuffer + VE_HEADER_OFFSET);
}
//--------------------------------------------------------------------------
VeStringW::StringHandle VeStringW::AllocateAndCopy(const VeChar16* pcString)
{
	if(pcString == NULL) return NULL;

	VeUInt32 u32Length = (VeUInt32)VeWcslen(pcString);
	StringHandle hHandle = Allocate(u32Length);

	if(hHandle == NULL) return NULL;

	VeCrazyCopy(hHandle, pcString, (u32Length + 1) * sizeof(VeChar16));
	VE_ASSERT(VeWcscmp(hHandle, pcString) == 0);

	VeUInt32* pu32Buffer = (VeUInt32*)GetRealBufferStart(hHandle);
	pu32Buffer[VE_STRING_LENGTH_IDX] = u32Length;
	return hHandle;
}
//--------------------------------------------------------------------------
VeStringW::StringHandle VeStringW::AllocateAndCopyHandle(StringHandle hHandle)
{
	if(hHandle == NULL) return NULL;
	VeUInt32 u32Length = GetLength(hHandle);
	VeUInt32 u32BufferSize = GetAllocationSize(hHandle);
	VeChar8* pcBuffer = VeAlloc(VeChar8, u32BufferSize);
	VeUInt32* pu32Buffer = (VeUInt32*)pcBuffer;
	pu32Buffer[VE_BUFFER_SIZE_IDX] = u32BufferSize;
	pu32Buffer[VE_REF_COUNT_IDX] = 1;
	pu32Buffer[VE_STRING_LENGTH_IDX] = u32Length;
	StringHandle kNewHandle = (VeChar16*)(pcBuffer + VE_HEADER_OFFSET);
	VeCrazyCopy(kNewHandle, hHandle, (u32Length + 1) * sizeof(VeChar16));
	VE_ASSERT(VeWcscmp(kNewHandle, hHandle) == 0);
	return kNewHandle;
}
//--------------------------------------------------------------------------
void VeStringW::Deallocate(StringHandle& hHandle)
{
	if(hHandle)
	{
		VeChar16* pcMem = GetRealBufferStart(hHandle); 
		VeFree(pcMem);
		hHandle = NULL;
	}
}
//--------------------------------------------------------------------------
void VeStringW::IncRefCount(StringHandle& hHandle)
{
	if(hHandle == NULL) return;
	VE_ASSERT(ValidateString(hHandle));
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeAtomicIncrement32(pu32Mem[VE_REF_COUNT_IDX]);
}
//--------------------------------------------------------------------------
void VeStringW::DecRefCount(StringHandle& hHandle)
{
	if(hHandle == NULL) return;
	VE_ASSERT(ValidateString(hHandle));
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeAtomicDecrement32(pu32Mem[VE_REF_COUNT_IDX]);
	if(pu32Mem[VE_REF_COUNT_IDX] == 0) Deallocate(hHandle);
	hHandle = NULL;
}
//--------------------------------------------------------------------------
void VeStringW::Swap(StringHandle& hHandle, const VeChar16* pcNewValue)
{
	if(pcNewValue == NULL)
	{
		DecRefCount(hHandle);
		return;
	}

	if(hHandle == NULL)
	{
		hHandle = AllocateAndCopy(pcNewValue);
		return;
	}

	VE_ASSERT(ValidateString(hHandle));
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeAtomicDecrement32(pu32Mem[VE_REF_COUNT_IDX]);

	if(pu32Mem[VE_REF_COUNT_IDX] == 0)
	{
		VeUInt32 u32Length = (VeUInt32)VeWcslen(pcNewValue);
		if(pu32Mem[VE_BUFFER_SIZE_IDX] >= ((u32Length + 1) * sizeof(VeChar16) + VE_HEADER_OFFSET))
		{
			pu32Mem[VE_REF_COUNT_IDX] = 1;
		}
		else
		{
			Deallocate(hHandle);
			hHandle = Allocate(u32Length);
			pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		}

		VeCrazyCopy(hHandle, pcNewValue, (u32Length + 1) * sizeof(VeChar16));
		VE_ASSERT(VeWcscmp(hHandle, pcNewValue) == 0);

		pu32Mem[VE_STRING_LENGTH_IDX] = u32Length;
	}
	else
	{
		hHandle = AllocateAndCopy(pcNewValue);
	}
}
//--------------------------------------------------------------------------
VeChar16* VeStringW::GetString(const StringHandle& hHandle)
{
	VE_ASSERT(ValidateString(hHandle));
	return hHandle;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::GetLength(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_STRING_LENGTH_IDX];
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::GetRefCount(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_REF_COUNT_IDX];
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::GetAllocationSize(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_BUFFER_SIZE_IDX];
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::GetBufferSize(const StringHandle& hHandle)
{
	if(hHandle == NULL)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(hHandle));
		VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
		return pu32Mem[VE_BUFFER_SIZE_IDX] - VE_HEADER_OFFSET;
	}
}
//--------------------------------------------------------------------------
void VeStringW::SetLength(const StringHandle& hHandle, VeUInt32 u32Length)
{
	if(hHandle == NULL) return;
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	pu32Mem[VE_STRING_LENGTH_IDX] = u32Length;
	VE_ASSERT(pu32Mem[VE_STRING_LENGTH_IDX] < pu32Mem[VE_BUFFER_SIZE_IDX]);
}
//--------------------------------------------------------------------------
VeChar16* VeStringW::GetRealBufferStart(const StringHandle& hHandle)
{
	VE_ASSERT(hHandle);
	return (VeChar16*)(((VeChar8*)hHandle) - VE_HEADER_OFFSET);
}
//--------------------------------------------------------------------------
bool VeStringW::ValidateString(const StringHandle& hHandle)
{
	if(hHandle == NULL) return true;
	VeUInt32* pu32Mem = (VeUInt32*)GetRealBufferStart(hHandle);
	VeUInt32 u32Length = pu32Mem[VE_STRING_LENGTH_IDX];
	if(u32Length == (VeUInt32)VeWcslen(hHandle)) return true;
	else return false;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::GetBestBufferSize(VeUInt32 u32ReqSize)
{
	if (u32ReqSize < 32)
		return 32;
	else if (u32ReqSize < 64)
		return 64;
	else if (u32ReqSize < 128)
		return 128;
	else if (u32ReqSize < 256)
		return 255;
	else if (u32ReqSize < 512)
		return 512;
	else if (u32ReqSize < 1024)
		return 1024;
	else 
		return ((u32ReqSize + 0xF) >> 4) << 4;
}
//--------------------------------------------------------------------------
VeStringW::VeStringW(VeUInt32 u32Length)
{
	m_hHandle = Allocate(u32Length);
	m_hHandle[0] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringW::VeStringW(VeChar16 c16Char, VeUInt32 u32Length)
{
	m_hHandle = Allocate(u32Length);
	for(VeUInt32 i(0); i < u32Length; ++i)
	{
		m_hHandle[i] = c16Char;
	}
	m_hHandle[u32Length] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringW::VeStringW(const VeChar16* pc16Str)
{
	m_hHandle = AllocateAndCopy(pc16Str);
	CalcLength();
}
//--------------------------------------------------------------------------
VeStringW::VeStringW(const VeStringW& kString)
{
	m_hHandle = kString.m_hHandle;
	IncRefCount(m_hHandle);
}
//--------------------------------------------------------------------------
VeStringW::~VeStringW()
{
	DecRefCount(m_hHandle);
}
//--------------------------------------------------------------------------
bool VeStringW::Resize(VeUInt32 u32Delta)
{
	VeUInt32 u32NewStringLength = Length() + u32Delta;
	if(((u32NewStringLength + 1) * sizeof(VeChar16)) > GetBufferSize(m_hHandle))
	{
		StringHandle kNewHandle = Allocate(u32NewStringLength);
		VeUInt32 u32EndLoc = Length();
		VeCrazyCopy(kNewHandle, m_hHandle, Length() * sizeof(VeChar16));
		kNewHandle[u32EndLoc] = 0;
		DecRefCount(m_hHandle);
		m_hHandle = kNewHandle;
		CalcLength();
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------
void VeStringW::SetBuffer(StringHandle& hHandle)
{
	if(hHandle == m_hHandle) return;
	DecRefCount(m_hHandle);
	IncRefCount(hHandle);
	m_hHandle = hHandle;
}
//--------------------------------------------------------------------------
void VeStringW::CalcLength()
{
	if(m_hHandle == NULL) return;
	SetLength(m_hHandle, (VeUInt32)VeWcslen(m_hHandle));
}
//--------------------------------------------------------------------------
void VeStringW::CopyOnWrite(bool bForceCopy)
{
	if((GetRefCount(m_hHandle) > 1) || bForceCopy)
	{
		StringHandle hHandle = m_hHandle;
		m_hHandle = AllocateAndCopyHandle(m_hHandle);
		DecRefCount(hHandle);
	}
}
//--------------------------------------------------------------------------
void VeStringW::CopyOnWriteAndResize(VeUInt32 u32SizeDelta, bool bForceCopy)
{
	if(Resize(u32SizeDelta)) return;

	if(GetRefCount(m_hHandle) > 1 || bForceCopy)
	{
		StringHandle kHandle = m_hHandle;
		m_hHandle = AllocateAndCopyHandle(m_hHandle);
		DecRefCount(kHandle);
	}
}
//--------------------------------------------------------------------------
VeChar16* VeStringW::ConsumeWhitespace(const VeChar16* pcStr, VeUInt32 u32Length) const
{
	bool bContinue = true;
	VeUInt32 u32Idx;
	for(u32Idx = 0; u32Idx < u32Length && bContinue; u32Idx++)
	{
		if(pcStr[u32Idx] != L'\t' && pcStr[u32Idx] != L' ')
			bContinue = false;
	}
	return (VeChar16*)pcStr + u32Idx - 1;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::Length() const
{
	return GetLength(m_hHandle);
}
//--------------------------------------------------------------------------
bool VeStringW::IsEmpty() const
{
	return Length() == 0;
}
//--------------------------------------------------------------------------
void VeStringW::Empty()
{
	DecRefCount(m_hHandle);
}
//--------------------------------------------------------------------------
VeChar16 VeStringW::GetAt(VeUInt32 u32Index) const
{
	VeChar16 c;
	if (u32Index >= Length())
		c = 0;
	else
		c = GetString(m_hHandle)[u32Index];
	return c;
}
//--------------------------------------------------------------------------
void VeStringW::SetAt(VeUInt32 u32Index, VeChar16 c)
{
	if(u32Index >= Length()) return;

	CopyOnWrite();
	GetString(m_hHandle)[u32Index] = c;
	if(c == 0)
	{
		SetLength(m_hHandle, u32Index);
	}
}
//--------------------------------------------------------------------------
VeInt32 VeStringW::Compare(const VeChar16* pc16String) const
{
	if(pc16String == NULL && m_hHandle == NULL)
		return 0;
	else if(pc16String == NULL)
		return GetString(m_hHandle)[0];
	else if (m_hHandle == NULL)
		return -pc16String[0];
	return VeWcscmp(GetString(m_hHandle), pc16String);
}
//--------------------------------------------------------------------------
VeInt32 VeStringW::CompareNoCase(const VeChar16* pc16String) const
{
	if(pc16String == NULL && m_hHandle == (StringHandle) NULL)
		return 0;
	else if (pc16String == NULL)
		return GetString(m_hHandle)[0];
	else if (m_hHandle == (StringHandle) NULL)
		return -pc16String[0];
	return VeWStricmp(GetString(m_hHandle), pc16String);
}
//--------------------------------------------------------------------------
bool VeStringW::Equals(const VeChar16* pc16Str) const
{
	if(pc16Str == NULL)
		return m_hHandle == (StringHandle)NULL;
	else if(VeWcslen(pc16Str) != Length())
		return false;

	return Compare(pc16Str) == 0;
}
//--------------------------------------------------------------------------
bool VeStringW::EqualsNoCase(const VeChar16* pc16Str) const
{
	if(pc16Str == NULL)
		return m_hHandle == NULL;
	else if(VeWcslen(pc16Str) != Length())
		return false;
	return CompareNoCase(pc16Str) == 0;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::GetSubstring(VeUInt32 u32Begin, VeUInt32 u32End) const
{
	if (u32Begin < u32End && u32Begin < Length() && u32End <= Length() && m_hHandle != NULL)
	{
		VeStringW kString((u32End - u32Begin + 2));
		VeWStrncpy(kString.m_hHandle, kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + u32Begin, u32End - u32Begin);
		kString.m_hHandle[u32End - u32Begin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar16*)NULL;
	}
}
//--------------------------------------------------------------------------
VeStringW VeStringW::GetSubstring(VeUInt32 u32Begin, const VeChar16* pc16Delimeters) const
{
	if (u32Begin < Length() && m_hHandle != NULL && pc16Delimeters != NULL)
	{
		VeChar16* pcStr = VeWcspbrk(GetString(m_hHandle) + u32Begin, pc16Delimeters);
		if (pcStr == NULL)
			return (const VeChar16*)NULL;
		else
			return GetSubstring(u32Begin, (VeUInt32)(pcStr - GetString(m_hHandle)));
	}
	else
	{
		return (const VeChar16*)NULL;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::Find(VeChar16 c, VeUInt32 u32Start) const
{
	if (u32Start >= Length())
		return VE_INFINITE;

	VeChar16* pcStr = VeWcschr(GetString(m_hHandle) + u32Start, c);
	if (pcStr == NULL)
		return VE_INFINITE;

	VeInt32 iReturn = (VeInt32)(pcStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32)iReturn;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::Find( const VeChar16* pc16Str, VeUInt32 u32Start  ) const
{
	if (u32Start >= Length())
		return VE_INFINITE;
	if (pc16Str == NULL || pc16Str[0] == 0)
		return VE_INFINITE;
	VeChar16* pcSubStr = VeWcsstr(GetString(m_hHandle) + u32Start, pc16Str);
	if (pcSubStr == NULL)
		return VE_INFINITE;

	VeInt32 iReturn = (VeInt32)(pcSubStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32)iReturn;
}
//--------------------------------------------------------------------------
bool VeStringW::Contains(const VeChar16* pc16Str) const
{
	return Find(pc16Str) != VE_INFINITE;
}
//--------------------------------------------------------------------------
bool VeStringW::ContainsNoCase(const VeChar16* pc16Str) const
{
	if (m_hHandle == pc16Str && pc16Str != NULL)
		return true;

	if (pc16Str == NULL || m_hHandle == NULL)
		return false;

	const VeChar16* pcMyString = (const VeChar16*) m_hHandle;
	VeSizeT stOtherLength = VeWcslen(pc16Str);
	for (VeUInt32 uiMyIdx = 0; uiMyIdx < Length(); uiMyIdx++)
	{
		if (VeWStrnicmp(&pcMyString[uiMyIdx], pc16Str, (VeInt32)stOtherLength) == 0)
			return true;
	}
	return false;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::FindReverse(VeChar16 c) const
{
	if (m_hHandle == NULL)
		return VE_INFINITE;

	VeChar16* pcStr = VeWcsrchr(GetString(m_hHandle), c);
	if (pcStr == NULL)
		return VE_INFINITE;
	else if (c == 0)
		return VE_INFINITE;
	VeInt32 iReturn = (VeInt32)(pcStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32) iReturn;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::FindReverse(VeChar16 c, VeUInt32 u32StartIdx) const
{
	if (m_hHandle == NULL || c == 0)
		return VE_INFINITE;

	if (u32StartIdx >= Length())
		u32StartIdx = Length() - 1;

	const VeChar16* pcValue = GetString(m_hHandle);
	while (u32StartIdx != VE_INFINITE)
	{
		if (pcValue[u32StartIdx] == c)
			return u32StartIdx;
		--u32StartIdx;
	}

	return VE_INFINITE;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringW::FindOneOf(const VeChar16* pc16Str, VeUInt32 uiIndex) const
{
	if (pc16Str == NULL || pc16Str == 0 || m_hHandle == NULL)
		return VE_INFINITE;
	VeChar16* pcSubStr = VeWcspbrk(GetString(m_hHandle) + uiIndex, pc16Str);
	if (pcSubStr == NULL)
		return VE_INFINITE;
	VeInt32 iReturn = (VeInt32)(pcSubStr - GetString(m_hHandle));
	if (iReturn < 0)
		return VE_INFINITE;
	else
		return (VeUInt32) iReturn;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::Mid(VeUInt32 u32Begin, VeUInt32 u32Count) const
{
	VeUInt32 u32End = u32Begin + u32Count;
	if (u32End > Length() || u32Count == VE_INFINITE)
		u32End = Length();
	if (u32Begin < u32End && u32Begin < Length() && 
		u32End <= Length())
	{
		VeStringW kString(u32End - u32Begin + 2);
		VeWStrncpy(kString.m_hHandle, kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + u32Begin, u32End - u32Begin);
		kString.m_hHandle[u32End - u32Begin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar16*)NULL;
	}
}
//--------------------------------------------------------------------------
VeStringW VeStringW::Left(VeUInt32 u32Count) const
{
	VeUInt32 u32Begin = 0;
	VeUInt32 u32End = u32Begin + u32Count;
	if (u32End > Length())
		u32End = Length();
	if (u32Begin < u32End && u32Begin < Length() && 
		u32End <= Length())
	{
		VeStringW kString(u32End - u32Begin + 2);
		VeWStrncpy(kString.m_hHandle, kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + u32Begin, u32End - u32Begin);
		kString.m_hHandle[u32End - u32Begin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar16*)NULL;
	}
}
//--------------------------------------------------------------------------
VeStringW VeStringW::Right(VeUInt32 u32Count) const
{
	VeUInt32 uiEnd = Length();
	VeInt32 iBegin = uiEnd - u32Count;
	VeUInt32 uiBegin = 0;

	if (iBegin > 0)
		uiBegin = (VeUInt32) iBegin;

	if (uiEnd > Length())
		uiEnd = Length();
	if (uiBegin < uiEnd && uiBegin < Length() && 
		uiEnd <= Length())
	{
		VeStringW kString(uiEnd - uiBegin + 2);
		VeWStrncpy(kString.m_hHandle, kString.GetBufferSize(kString.m_hHandle) - 1,
			GetString(m_hHandle) + uiBegin, uiEnd - uiBegin);
		kString.m_hHandle[uiEnd - uiBegin] = 0;
		kString.CalcLength();
		return kString;
	}
	else
	{
		return (const VeChar16*)NULL;
	}
}
//--------------------------------------------------------------------------
void VeStringW::Replace(const VeChar16* pcWhatToReplace , const VeChar16* pcReplaceWith)
{
	if (pcWhatToReplace == NULL || pcReplaceWith == NULL)
		return;

	VeUInt32 stWhatToReplaceLength = (VeUInt32)VeWcslen(pcWhatToReplace);
	VeUInt32 stReplaceWithLength = (VeUInt32)VeWcslen(pcReplaceWith);
	VeUInt32 uiOccuranceCount = 0;
	VeUInt32 uiOriginalLength = Length();

	VeUInt32 ui = 0;
	for (; ui < uiOriginalLength && ui != VE_INFINITE;)
	{
		ui = Find(pcWhatToReplace, ui);
		if (ui != VE_INFINITE)
		{
			uiOccuranceCount++;
			ui = (VeUInt32)(ui + stWhatToReplaceLength);
		}
	}

	if (uiOccuranceCount == 0)
		return;

	VeUInt32 uiNumCharsToReplace = 
		uiOccuranceCount * stWhatToReplaceLength;
	VeUInt32 uiNumCharsNeededToReplace = 
		uiOccuranceCount * stReplaceWithLength;
	VeUInt32 uiSizeNeeded = 
		uiOriginalLength - uiNumCharsToReplace + uiNumCharsNeededToReplace;

	if (uiSizeNeeded > uiOriginalLength)
		CopyOnWriteAndResize(uiSizeNeeded - uiOriginalLength);
	else
		CopyOnWrite();

	for (ui = 0; ui < uiSizeNeeded && uiOccuranceCount > 0; uiOccuranceCount--)
	{
		VeChar16* pcStart = VeWcsstr(m_hHandle + ui, pcWhatToReplace);
		VE_ASSERT(pcStart != NULL);
		VeChar16* pcEnd = pcStart + stReplaceWithLength;

		VeUInt32 u32Size = (VeUInt32)VeWcslen(pcStart + stWhatToReplaceLength) + 1;

		VeMemmove(pcStart + stReplaceWithLength, u32Size, pcStart + stWhatToReplaceLength, u32Size);
		VeMemcpy(pcStart, pcReplaceWith, stReplaceWithLength);
		ui = (VeUInt32)(pcEnd - m_hHandle);
	}

	m_hHandle[uiSizeNeeded] = 0;

	CalcLength();
}
//--------------------------------------------------------------------------
void VeStringW::Insert(const VeChar16* pcInsertString, VeUInt32 u32Index)
{
	if(pcInsertString == NULL)
		return;

	VeUInt32 stOriginalLength = Length();
	VeUInt32 stIndex = u32Index;
	VeUInt32 stInsertLength = (VeUInt32)VeWcslen(pcInsertString);

#if defined(VE_DEBUG)
	const VeChar16* pcOriginalString = GetString(m_hHandle);
	VE_ASSERT(!(pcInsertString >= pcOriginalString && 
		pcInsertString <= pcOriginalString + stOriginalLength));
#endif

	if (stIndex >= stOriginalLength)
	{
		Concatenate(pcInsertString);
		return;
	}

	VeUInt32 stNewSize = stInsertLength + stOriginalLength + 1;

	CopyOnWriteAndResize(stInsertLength);
	VeChar16* pcBuffer = GetString(m_hHandle);
	VeMemmove(&pcBuffer[stIndex + stInsertLength], 
		stNewSize - stIndex - stInsertLength,
		&pcBuffer[stIndex], stOriginalLength - stIndex + 1);
	VeMemmove(&pcBuffer[stIndex], stNewSize - stIndex,
		pcInsertString, stInsertLength);
	CalcLength();
}
//--------------------------------------------------------------------------
void VeStringW::RemoveRange( VeUInt32 u32StartIdx, VeUInt32 u32NumChars )
{
	VeUInt32 uiEnd = Length();
	if (u32StartIdx >= uiEnd)
		return;

	if (u32StartIdx + u32NumChars > uiEnd)
		u32NumChars = uiEnd - u32StartIdx;

	CopyOnWrite();
	VeChar16* pcBuffer = GetString(m_hHandle);
	VeMemmove(&pcBuffer[u32StartIdx], uiEnd - u32StartIdx + 1,
		&pcBuffer[u32StartIdx + u32NumChars], 
		uiEnd - u32StartIdx - u32NumChars + 1);
	CalcLength();
}
//--------------------------------------------------------------------------
void VeStringW::ToUpper()
{
	if (m_hHandle == NULL)
		return;

	CopyOnWrite();
	for ( VeUInt32 ui = 0; ui < Length() ; ui++)
	{
		VeChar16 c = GetString(m_hHandle)[ui];
		if ( ('a' <= c) && (c <= 'z') )
			GetString(m_hHandle)[ui] -= 'a' - 'A';
	}
}
//--------------------------------------------------------------------------
void VeStringW::ToLower()
{
	if (m_hHandle == NULL)
		return;

	CopyOnWrite();
	for ( VeUInt32 ui = 0; ui < Length() ; ui++)
	{
		VeChar16 c = GetString(m_hHandle)[ui];
		if ( ('A' <= c) && (c <= 'Z') )
			GetString(m_hHandle)[ui] -= 'A' - 'a';
	}
}
//--------------------------------------------------------------------------
void VeStringW::Reverse()
{
	if (Length() < 2)
		return;

	CopyOnWrite();
	VeUInt32 uiEnd = Length() - 1;
	for (VeUInt32 uiBegin = 0; uiBegin < uiEnd; 
		uiBegin++, uiEnd--)
	{
		VeChar16 c = GetString(m_hHandle)[uiBegin];
		GetString(m_hHandle)[uiBegin] = GetString(m_hHandle)[uiEnd];
		GetString(m_hHandle)[uiEnd] = c;
	}
}
//--------------------------------------------------------------------------
void VeStringW::TrimLeft(VeChar16 c)
{
	if ( c == 0 || m_hHandle == NULL)
		return;

	CopyOnWrite();
	VeUInt32 uiTrimCount = 0;
	bool bContinue = true;
	for (VeUInt32 ui = 0; 
		ui < Length() && bContinue; ui++)
	{
		if (GetString(m_hHandle)[ui] == c)
			uiTrimCount++;
		else
			bContinue = false;
	}

	if (uiTrimCount > 0)
	{
		VeUInt32 u32Size = Length() - uiTrimCount + 1;
		VeMemmove(GetString(m_hHandle), u32Size, GetString(m_hHandle) + uiTrimCount, u32Size);
		CalcLength();
	}
}
//--------------------------------------------------------------------------
void VeStringW::TrimRight(VeChar16 c)
{
	if (c == 0 || m_hHandle == NULL)
		return;

	CopyOnWrite();
	VeUInt32 uiTrimCount = 0;
	bool bContinue = true;
	for(VeInt32 i = Length() - 1; i >= 0 && bContinue; i--)
	{
		if (GetString(m_hHandle)[i] == c)
			uiTrimCount++;
		else
			bContinue = false;
	}

	if (uiTrimCount > 0)
	{
		VeChar16* pcStr = GetString(m_hHandle) + Length() - uiTrimCount;
		pcStr[0] = 0;
		CalcLength();
	}
}
//--------------------------------------------------------------------------
void VeStringW::Concatenate(const VeChar16* pcStr)
{
	if (pcStr == NULL)
		return;

	VeUInt32 stLen = (VeUInt32)VeWcslen(pcStr);
	if (stLen > 0)
	{
		StringHandle kHandle = m_hHandle;
		bool bInternalIncrementRefCount = false;
		if (pcStr == GetString(m_hHandle))
		{
			IncRefCount(kHandle);
			CopyOnWriteAndResize(stLen, true);
			bInternalIncrementRefCount = true;
		}
		else
		{
			CopyOnWriteAndResize(stLen, false);
		}

		VeWStrcpy(GetString(m_hHandle) + Length(), stLen + 1, pcStr);
		CalcLength();
		if (bInternalIncrementRefCount)
			DecRefCount(kHandle);
	}
}
//--------------------------------------------------------------------------
void VeStringW::Concatenate(VeChar16 ch)
{
	CopyOnWriteAndResize(1);
	VeUInt32 stLength = Length();

	m_hHandle[stLength] = ch;
	m_hHandle[stLength+1] = 0;
	CalcLength();
}
//--------------------------------------------------------------------------
bool VeStringW::ToBool(bool& b) const
{
	VeChar16* pcStr = ConsumeWhitespace(GetString(m_hHandle), Length());

	if(VeWcslen(pcStr) < 4)
	{
		return false;
	}
	else if(VeWStrnicmp(pcStr, L"true", 4) == 0)
	{
		b = true;
		return true;
	}
	else if(VeWStrnicmp(pcStr, L"false", 4) == 0)
	{
		b = false;
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------
bool VeStringW::To(bool& b) const
{
	return ToBool(b);
}
//--------------------------------------------------------------------------
bool VeStringW::ToFloat(VeFloat32& f) const
{
	VeChar16* pcString = GetString(m_hHandle);

	if(pcString == NULL)
		return false;
	
	f = (VeFloat32)VeWtof(pcString);
	if(f == 0)
	{
		if(FindOneOf(L"-0") != VE_INFINITE)
			return true;
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeFloat32& f) const
{
	return ToFloat(f);
}
//--------------------------------------------------------------------------
bool VeStringW::ToInt(VeInt32& i) const
{
	VeChar16* pcString = GetString(m_hHandle);

	if(pcString == NULL)
		return false;

	i = (VeInt32)VeWtoi(pcString);
	if(i == 0)
	{
		if(FindOneOf(L"-0") != VE_INFINITE)
			return true;
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeInt32& i) const
{
	return ToInt(i);
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeInt16& i) const
{
	VeInt32 iTemp = 0;
	bool bResult = ToInt(iTemp);
	i = (VeInt16)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeInt8& i) const
{
	VeInt32 iTemp = 0;
	bool bResult = ToInt(iTemp);
	i = (VeInt8)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringW::ToUInt(VeUInt32& ui) const
{
	VeChar16* pcString = GetString(m_hHandle);

	if(pcString == NULL)
		return false;

	ui = (VeInt32)VeWtoi(pcString);
	if(ui == 0)
	{
		if(FindOneOf(L"-0") != VE_INFINITE)
			return true;
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeUInt32& ui) const
{
	return ToUInt(ui);
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeUInt16& ui) const
{
	VeUInt32 iTemp = 0;
	bool bResult = ToUInt(iTemp);
	ui = (VeUInt16)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeUInt8& ui) const
{
	VeUInt32 iTemp = 0;
	bool bResult = ToUInt(iTemp);
	ui = (VeUInt8)iTemp;
	return bResult;
}
//--------------------------------------------------------------------------
bool VeStringW::ToVector2D(VeVector2D& kVec) const
{
	const VeChar16 acSeps [] = L",";
	VeChar16* pcContext;
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringW kString(VeWStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kVec.x))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString2(VeWStrtok(NULL, acSeps, &pcContext));
	if(!kString2.ToFloat(kVec.y))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeVector2D& kVec) const
{
	return ToVector2D(kVec);
}
//--------------------------------------------------------------------------
bool VeStringW::ToVector3D(VeVector3D& kVec) const
{
	const VeChar16 acSeps [] = L",";
	VeChar16* pcContext;
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringW kString(VeWStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kVec.x))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString2(VeWStrtok(NULL, acSeps, &pcContext));
	if (!kString2.ToFloat(kVec.y))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString3(VeWStrtok(NULL, acSeps, &pcContext));
	if (!kString3.ToFloat(kVec.z))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeVector3D& kVec) const
{
	return ToVector3D(kVec);
}
//--------------------------------------------------------------------------
bool VeStringW::ToVector4D(VeVector4D& kVec) const
{
	const VeChar16 acSeps [] = L",";
	VeChar16* pcContext;
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringW kString(VeWStrtok(GetString(kHandle), acSeps, &pcContext));
	if (!kString.ToFloat(kVec.x))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString2(VeWStrtok(NULL, acSeps, &pcContext));
	if (!kString2.ToFloat(kVec.y))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString3(VeWStrtok(NULL, acSeps, &pcContext));
	if (!kString3.ToFloat(kVec.z))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString4(VeWStrtok(NULL, acSeps, &pcContext));
	if (!kString4.ToFloat(kVec.w))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeVector4D& kVec) const
{
	return ToVector4D(kVec);
}
//--------------------------------------------------------------------------
bool VeStringW::ToRGB(VeColor& kColor) const
{
	const VeChar16 acSeps [] = L",";
	VeChar16* pcContext; 
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeStringW kString(VeWStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kColor.r))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString2(VeWStrtok(NULL, acSeps, &pcContext));
	if(!kString2.ToFloat(kColor.g))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString3(VeWStrtok(NULL, acSeps, &pcContext));
	if(!kString3.ToFloat(kColor.b))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeColor& kColor) const
{
	return ToRGB(kColor);
}
//--------------------------------------------------------------------------
bool VeStringW::ToRGBA(VeColorA& kColor) const
{
	const VeChar16 acSeps [] = L",";
	StringHandle kHandle = AllocateAndCopyHandle(m_hHandle);
	VeChar16* pcContext;

	VeStringW kString(VeWStrtok(GetString(kHandle), acSeps, &pcContext));
	if(!kString.ToFloat(kColor.r))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString2(VeWStrtok(NULL, acSeps, &pcContext));
	if(!kString2.ToFloat(kColor.g))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString3(VeWStrtok(NULL, acSeps, &pcContext));
	if(!kString3.ToFloat(kColor.b))
	{
		Deallocate(kHandle);
		return false;
	}

	VeStringW kString4(VeWStrtok(NULL, acSeps, &pcContext));
	if(!kString4.ToFloat(kColor.a))
	{
		Deallocate(kHandle);
		return false;
	}

	Deallocate(kHandle);
	return true;
}
//--------------------------------------------------------------------------
bool VeStringW::To(VeColorA& kColor) const
{
	return ToRGBA(kColor);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromBool(bool b)
{
	if(b)
		return VeStringW(L"TRUE");
	else
		return VeStringW(L"FALSE");
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(bool b)
{
	return FromBool(b);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromFloat(VeFloat32 f)
{
	VeChar16 acString[50];
	VeWSprintf(acString, 50, L"%f", f);
	VeStringW kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeFloat32 f)
{
	return FromFloat(f);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromInt(VeInt32 i)
{
	VeChar16 acString[32];
	VeWSprintf(acString, 32, L"%d", i);
	return VeStringW(acString);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeInt32 i)
{
	return FromInt(i);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeInt16 i)
{
	return FromInt(i);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeInt8 i)
{
	return FromInt(i);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromUInt(VeUInt32 ui)
{
	VeChar16 acString[32];
	VeWSprintf(acString, 32, L"%d", ui);
	return VeStringW(acString);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeUInt32 ui)
{
	return FromUInt(ui);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeUInt16 ui)
{
	return FromUInt(ui);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeUInt8 ui)
{
	return FromUInt(ui);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromVector2D(VeVector2D& kPt)
{
	VeChar16 acString[128];
	VeWSprintf(acString, 128, L"%f,%f", kPt.x, kPt.y);
	VeStringW kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeVector2D& kPt)
{
	return FromVector2D(kPt);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromVector3D(VeVector3D& kPt)
{
	VeChar16 acString[128];
	VeWSprintf(acString, 128, L"%f,%f,%f", kPt.x, kPt.y, kPt.z);
	VeStringW kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeVector3D& kPt)
{
	return FromVector3D(kPt);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromVector4D(VeVector4D& kPt)
{
	VeChar16 acString[128];
	VeWSprintf(acString, 128, L"%f,%f,%f,%f", kPt.x, kPt.y, kPt.z, kPt.w);
	VeStringW kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeVector4D& kPt)
{
	return FromVector4D(kPt);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromRGB(VeColor& kColor)
{
	VeChar16 acString[128];
	VeWSprintf(acString, 128, L"%f,%f,%f)", kColor.r, kColor.g, kColor.b);
	VeStringW kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeColor& kColor)
{
	return FromRGB(kColor);
}
//--------------------------------------------------------------------------
VeStringW VeStringW::FromRGBA(VeColorA& kColor)
{
	VeChar16 acString[160];
	VeWSprintf(acString, 160, L"%f,%f,%f,%f)", kColor.r, kColor.g, kColor.b, kColor.a);
	VeStringW kString(acString);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW VeStringW::From(VeColorA& kColor)
{
	return FromRGBA(kColor);
}
//--------------------------------------------------------------------------
VeStringW::operator const VeChar16* () const
{
	return GetString(m_hHandle);
}
//--------------------------------------------------------------------------
VeStringW& VeStringW::operator = (VeStringW& stringSrc)
{
	SetBuffer(stringSrc.m_hHandle);
	return *this;
}
//--------------------------------------------------------------------------
VeStringW& VeStringW::operator = (const VeChar16* psz)
{
	if(psz == GetString(m_hHandle))
		return *this;

	Swap(m_hHandle, psz);
	return *this;
}
//--------------------------------------------------------------------------
VeStringW& VeStringW::operator = (VeChar16 ch)
{
	VeChar16 acString[2];
	acString[0] = ch;
	acString[1] = 0;
	return VeStringW::operator = ((const VeChar16*)acString);
}
//--------------------------------------------------------------------------
VeStringW& VeStringW::operator += (VeStringW& string)
{
	Concatenate(string);
	return *this;
}
//--------------------------------------------------------------------------
VeStringW& VeStringW::operator += (VeChar16 ch)
{
	Concatenate(ch);
	return *this;
}
//--------------------------------------------------------------------------
VeStringW& VeStringW::operator += (const VeChar16* pcString)
{
	Concatenate(pcString);
	return *this;
}
//--------------------------------------------------------------------------
VeStringW operator + (const VeStringW& string1, const VeStringW& string2)
{
	VeStringW kString(string1);
	kString.Concatenate(string2);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW operator + (const VeStringW& string, VeChar16 ch)
{
	VeStringW kString(string);
	kString.Concatenate(ch);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW operator + (VeChar16 ch, const VeStringW& string)
{
	VeStringW kString(ch);
	kString.Concatenate(string);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW operator + (const VeStringW& string, const VeChar16* lpsz)
{
	VeStringW kString(string);
	kString.Concatenate(lpsz);
	return kString;
}
//--------------------------------------------------------------------------
VeStringW operator + (const VeChar16* lpsz, const VeStringW& string)
{
	VeStringW kString(lpsz);
	kString.Concatenate(string);
	return kString;
}
//--------------------------------------------------------------------------
bool operator == (const VeStringW& s1, const VeStringW& s2)
{
	return s1.Compare(s2) == 0;
}
//--------------------------------------------------------------------------
bool operator == (const VeStringW& s1, const VeChar16* s2)
{
	return s1.Compare(s2) == 0;
}
//--------------------------------------------------------------------------
bool operator == (const VeChar16* s1, const VeStringW& s2)
{
	return s2.Compare(s1) == 0; 
}
//--------------------------------------------------------------------------
bool operator != (const VeStringW& s1, const VeStringW& s2)
{
	return s1.Compare(s2) != 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeStringW& s1, const VeChar16* s2)
{
	return s1.Compare(s2) != 0;
}
//--------------------------------------------------------------------------
bool operator != (const VeChar16* s1, const VeStringW& s2)
{
	return s2.Compare(s1) != 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeStringW& s1, const VeStringW& s2)
{
	return s1.Compare(s2) < 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeStringW& s1, const VeChar16* s2)
{
	return s1.Compare(s2) < 0;
}
//--------------------------------------------------------------------------
bool operator < (const VeChar16* s1, const VeStringW& s2)
{
	return s2.Compare(s1) > 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeStringW& s1, const VeStringW& s2)
{
	return s1.Compare(s2) > 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeStringW& s1, const VeChar16* s2)
{
	return s1.Compare(s2) > 0;
}
//--------------------------------------------------------------------------
bool operator > (const VeChar16* s1, const VeStringW& s2)
{
	return s2.Compare(s1) < 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeStringW& s1, const VeStringW& s2)
{
	return s1.Compare(s2) <= 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeStringW& s1, const VeChar16* s2)
{
	return s1.Compare(s2) <= 0;
}
//--------------------------------------------------------------------------
bool operator <= (const VeChar16* s1, const VeStringW& s2)
{
	return s2.Compare(s1) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeStringW& s1, const VeStringW& s2)
{
	return s1.Compare(s2) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeStringW& s1, const VeChar16* s2)
{
	return s1.Compare(s2) >= 0;
}
//--------------------------------------------------------------------------
bool operator >= (const VeChar16* s1, const VeStringW& s2)
{
	return s2.Compare(s1) <= 0;
}
//--------------------------------------------------------------------------
bool IsTrue(VeStringW strStr)
{
	bool b = false;
	if(strStr.ToBool(b))
		return b;

	VE_ASSERT(strStr.ToBool(b));
	return false;
}
//--------------------------------------------------------------------------
VeChar16* VeStringW::MakeExternalCopy()
{
	VeUInt32 uiSize = Length()+1;
	VeChar16* pcStr = VeAlloc(VeChar16, uiSize);
	VeWStrcpy(pcStr, uiSize, GetString(m_hHandle));
	return pcStr;
}
//--------------------------------------------------------------------------
VeGlobalStringTableA* VeGlobalStringTableA::ms_pkTable = NULL;
//--------------------------------------------------------------------------
const VeGlobalStringTableA::StringHandle VeGlobalStringTableA::NULL_STRING = NULL;
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::ms_u32NumStrings = 0;
//--------------------------------------------------------------------------
VeGlobalStringTableA::VeGlobalStringTableA()
{
	VE_ASSERT(VE_NUM_GLOBAL_STRING_HASH_BUCKETS < 65536);
	VE_ASSERT(sizeof(VeSizeT) >= 4);

	for(VeUInt32 ui = 0; ui < VE_NUM_GLOBAL_STRING_HASH_BUCKETS; ui++)
	{
		m_kHashArray[ui].Reserve(VE_NUM_ENTRIES_PER_BUCKET_RESERVE);
	}
}
//--------------------------------------------------------------------------
const VeGlobalStringTableA::StringHandle VeGlobalStringTableA::AddString(const VeChar8* pcString)
{
	VE_AUTO_LOCK_MUTEX(ms_pkTable->m_kMutex);

	if(pcString == NULL)
	{
		return NULL_STRING;
	}

	VE_ASSERT(ms_pkTable);

	VeSizeT stStrLen = VeStrlen(pcString);
	VE_ASSERT(stStrLen < 65536);

	StringHandle kHandle = ms_pkTable->FindString(pcString, (VeUInt32)stStrLen);

	if(kHandle != NULL_STRING)
	{
		IncRefCount(kHandle);
		return kHandle;
	}

	VeUInt32 u32AllocLen = VeUInt32(stStrLen) + 2 * sizeof(VeUInt32) + sizeof(VeChar8);

	if(u32AllocLen % sizeof(VeUInt32) != 0)
	{
		u32AllocLen += sizeof(VeUInt32) - (u32AllocLen % sizeof(VeUInt32));
	}

	void* pvMem = VeMalloc(u32AllocLen);   

	VeChar8* pcMem = (VeChar8*)pvMem + 2 * sizeof(VeUInt32);
	kHandle = pcMem;     
	VeUInt32* pu32Mem = (VeUInt32*)pvMem;

	VeUInt32 uiHash = HashFunction(pcString, (VeUInt32)stStrLen);

	pu32Mem[0] = 2;
	pu32Mem[1] = (VeUInt32(stStrLen) << GSTABLE_LEN_SHIFT) | ((uiHash << GSTABLE_HASH_SHIFT) & GSTABLE_HASH_MASK);
	VeMemcpy(pcMem, pcString, stStrLen + 1);

	ms_pkTable->InsertString(kHandle, uiHash);
	return kHandle;
}
//--------------------------------------------------------------------------
void VeGlobalStringTableA::IncRefCount(StringHandle& kHandle)
{
	if(kHandle == NULL_STRING)
		return;

	VE_ASSERT(ValidateString(kHandle));
	VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
	VeAtomicIncrement32(pkMem[0]);
}
//--------------------------------------------------------------------------
void VeGlobalStringTableA::DecRefCount(StringHandle& kHandle)
{
	if(kHandle == NULL_STRING)
		return;

	VE_ASSERT(ValidateString(kHandle));
	VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
	VeUInt32 uiHash = GetHashValue(kHandle);
	if(VeAtomicDecrement32(pkMem[0]) == 1)
	{
		VE_ASSERT(ms_pkTable);
		ms_pkTable->RemoveString(kHandle, uiHash);
	}
}
//--------------------------------------------------------------------------
const VeChar8* VeGlobalStringTableA::GetString(const StringHandle& kHandle)
{
	VE_ASSERT(ValidateString(kHandle));
	return (const VeChar8*)kHandle;
}
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::GetLength(const StringHandle& kHandle)
{
	if(kHandle == NULL_STRING)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(kHandle));
		VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
		return (pkMem[1] & GSTABLE_LEN_MASK) >> GSTABLE_LEN_SHIFT;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::GetRefCount(const StringHandle& kHandle)
{
	if(kHandle == NULL_STRING)
	{
		return 0;
	}
	else
	{
		VE_ASSERT(ValidateString(kHandle));
		VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
		return pkMem[0];
	}
}
//--------------------------------------------------------------------------
bool VeGlobalStringTableA::ValidateString(const StringHandle& kHandle)
{
	if(kHandle == NULL_STRING)
		return true;

	VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
	VeUInt32 u32Length = (pkMem[1] & GSTABLE_LEN_MASK) >> GSTABLE_LEN_SHIFT;

	if(u32Length != VeStrlen((const VeChar8*)kHandle))
		return false;

	return true;
}
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::GetBucketSize(VeUInt32 uiWhichBucket)
{
	VE_ASSERT(ms_pkTable);
	VE_ASSERT(uiWhichBucket < VE_NUM_GLOBAL_STRING_HASH_BUCKETS);
	VE_AUTO_LOCK_MUTEX(ms_pkTable->m_kMutex);
	return ms_pkTable->m_kHashArray[uiWhichBucket].Size();
}
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::GetTotalNumberOfBuckets()
{
	return VE_NUM_GLOBAL_STRING_HASH_BUCKETS;
};
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::GetMaxBucketSize()
{
	VE_ASSERT(ms_pkTable);
	VE_AUTO_LOCK_MUTEX(ms_pkTable->m_kMutex);
	VeUInt32 u32Size = 0;
	for(VeUInt32 ui = 0; ui < VE_NUM_GLOBAL_STRING_HASH_BUCKETS; ui++)
	{
		if(u32Size < ms_pkTable->m_kHashArray[ui].Size())
			u32Size = ms_pkTable->m_kHashArray[ui].Size();
	}
	return u32Size;
}
//--------------------------------------------------------------------------
const VeGlobalStringTableA::StringHandle VeGlobalStringTableA::FindString(const VeChar8* pcString, VeUInt32 u32Len)
{
	VeUInt32 uiHash = HashFunction(pcString, u32Len);

	VE_AUTO_LOCK_MUTEX(m_kMutex);
	VeUInt32 uiBucketSize = m_kHashArray[uiHash].Size();
	for (unsigned int ui = 0; ui < uiBucketSize; ++ui)
	{
		VeGlobalStringTableA::StringHandle kPossibleMatchString = m_kHashArray[uiHash][ui];

		if (kPossibleMatchString == pcString || (GetString(kPossibleMatchString) && 
			GetLength(kPossibleMatchString) == u32Len && VeStrcmp(GetString(kPossibleMatchString), pcString) == 0))
		{
			return kPossibleMatchString;
		}
	}

	return NULL_STRING;
}
//--------------------------------------------------------------------------
void VeGlobalStringTableA::InsertString(const StringHandle& kHandle, VeUInt32 uiHash)
{
	VE_ASSERT(ValidateString(kHandle));

	VE_AUTO_LOCK_MUTEX(m_kMutex);
	m_kHashArray[uiHash].PushBack(kHandle);

	++ms_u32NumStrings;
}
//--------------------------------------------------------------------------
void VeGlobalStringTableA::RemoveString(const StringHandle& kHandle, VeUInt32 uiHashValue)
{
	const VeChar8* pcString = GetString(kHandle);

	VE_AUTO_LOCK_MUTEX(m_kMutex);
	VeUInt32 uiSize = m_kHashArray[uiHashValue].Size();
	for(VeUInt32 ui(0); ui < uiSize; ui++)
	{
		if (m_kHashArray[uiHashValue][ui] == pcString)
		{
			VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
			if(GetRefCount(kHandle) == 1 && VeAtomicDecrement32(pkMem[0]) == 0)
			{
				VeFree(GetRealBufferStart(kHandle));
				m_kHashArray[uiHashValue][ui] = 0;
				--ms_u32NumStrings;
			}
			break;
		}
	}
}
//--------------------------------------------------------------------------
VeChar8* VeGlobalStringTableA::GetRealBufferStart(const StringHandle& kHandle)
{
	VE_ASSERT(kHandle != NULL);
	return ((VeChar8*)kHandle - 2 * sizeof(VeUInt32));
}
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::HashFunction(const VeChar8* pcString, VeUInt32 u32Len)
{
	VeUInt32 uiHash = 0;

	VeUInt32 uiUnroll = (VeUInt32)(u32Len & ~0x3);

	for(VeUInt32 ui = 0; ui < uiUnroll; ui += 4)
	{
		VeUInt32 uiHash0 = *pcString;
		uiHash = (uiHash << 5) + uiHash + uiHash0;
		VeUInt32 uiHash1 = *(pcString + 1);
		uiHash = (uiHash << 5) + uiHash + uiHash1;
		VeUInt32 uiHash2 = *(pcString + 2);
		uiHash = (uiHash << 5) + uiHash + uiHash2;
		VeUInt32 uiHash3 = *(pcString + 3);
		uiHash = (uiHash << 5) + uiHash + uiHash3;
		pcString += 4;
	}

	while(*pcString)
		uiHash = (uiHash << 5) + uiHash + *pcString++;

	return uiHash % VE_NUM_GLOBAL_STRING_HASH_BUCKETS;
}
//--------------------------------------------------------------------------
VeUInt32 VeGlobalStringTableA::GetHashValue(const StringHandle& kHandle)
{
	VE_ASSERT(ValidateString(kHandle));
	VeUInt32* pkMem = (VeUInt32*)GetRealBufferStart(kHandle);
	return (pkMem[1] & GSTABLE_HASH_MASK) >> GSTABLE_HASH_SHIFT;
}
//--------------------------------------------------------------------------
void VeGlobalStringTableA::Create()
{
	VE_ASSERT(!ms_pkTable);
	ms_pkTable = VE_NEW VeGlobalStringTableA();
}
//--------------------------------------------------------------------------
void VeGlobalStringTableA::Destory()
{
	VE_SAFE_DELETE(ms_pkTable);
}
//--------------------------------------------------------------------------
VeFixedStringA::VeFixedStringA()
{
	m_kHandle = VeGlobalStringTableA::NULL_STRING;
}
//--------------------------------------------------------------------------
VeFixedStringA::VeFixedStringA(const VeChar8* pcString)
{
	m_kHandle = VeGlobalStringTableA::AddString(pcString);
}
//--------------------------------------------------------------------------
VeFixedStringA::VeFixedStringA(const VeFixedStringA& kString)
{
	VeGlobalStringTableA::IncRefCount((VeGlobalStringTableA::StringHandle&)kString.m_kHandle);
	m_kHandle = kString.m_kHandle;
}
//--------------------------------------------------------------------------
VeFixedStringA::~VeFixedStringA()
{
	VeGlobalStringTableA::DecRefCount(m_kHandle);
}
//--------------------------------------------------------------------------
VeFixedStringA::operator const VeChar8* () const
{
	return VeGlobalStringTableA::GetString(m_kHandle);
}
//--------------------------------------------------------------------------
bool VeFixedStringA::Exists() const
{
	return m_kHandle != VeGlobalStringTableA::NULL_STRING;
}
//--------------------------------------------------------------------------
VeFixedStringA& VeFixedStringA::operator = (const VeFixedStringA& kString)
{
	if(m_kHandle != kString.m_kHandle)
	{
		VeGlobalStringTableA::StringHandle kHandle = kString.m_kHandle;
		VeGlobalStringTableA::IncRefCount(kHandle);
		VeGlobalStringTableA::DecRefCount(m_kHandle);
		m_kHandle = kHandle;
	}
	return *this;
}
//--------------------------------------------------------------------------
VeFixedStringA& VeFixedStringA::operator = (const VeChar8* pcString)
{
	if (m_kHandle != pcString)
	{
		VeGlobalStringTableA::StringHandle kHandle = m_kHandle;
		m_kHandle = VeGlobalStringTableA::AddString(pcString);
		VeGlobalStringTableA::DecRefCount(kHandle);
	}
	return *this;
}
//--------------------------------------------------------------------------
VeUInt32 VeFixedStringA::GetLength() const
{
	return VeGlobalStringTableA::GetLength(m_kHandle);
}
//--------------------------------------------------------------------------
VeUInt32 VeFixedStringA::GetRefCount() const
{
	return (VeUInt32)VeGlobalStringTableA::GetRefCount(m_kHandle);
}
//--------------------------------------------------------------------------
bool VeFixedStringA::Equals(const VeChar8* pcStr) const
{
	if(m_kHandle == pcStr)
		return true;

	if(pcStr == NULL || m_kHandle == NULL)
		return false;

	return VeStrcmp((const VeChar8*)m_kHandle, pcStr) == 0;
}
//--------------------------------------------------------------------------
bool VeFixedStringA::EqualsNoCase(const VeChar8* pcStr) const
{
	if(m_kHandle == pcStr)
		return true;

	if(pcStr == NULL || m_kHandle == NULL)
		return false;

	return VeStricmp((const VeChar8*) m_kHandle, pcStr) == 0;
}
//--------------------------------------------------------------------------
bool VeFixedStringA::Contains(const VeChar8* pcStr) const
{
	if(m_kHandle == pcStr && pcStr != NULL)
		return true;

	if(pcStr == NULL || m_kHandle == NULL || pcStr[0] == '\0' || m_kHandle[0] == '\0')
	{
		return false;
	}

	return VeStrstr((const VeChar8*)m_kHandle, pcStr) != NULL;
}
//--------------------------------------------------------------------------
bool VeFixedStringA::ContainsNoCase(const VeChar8* pcStr) const
{
	if(m_kHandle == pcStr && pcStr != NULL)
		return true;

	if(pcStr == NULL || m_kHandle == NULL || pcStr[0] == '\0' || m_kHandle[0] == '\0')
	{
		return false;
	}

	const VeChar8* pcMyString = m_kHandle;
	VeUInt32 u32OtherLength = (VeUInt32)VeStrlen(pcStr);
	for(VeUInt32 uiMyIdx = 0; uiMyIdx < GetLength(); uiMyIdx++)
	{
		if(VeStrnicmp(&pcMyString[uiMyIdx], pcStr, u32OtherLength) == 0)
			return true;
	}
	return false;
}
//--------------------------------------------------------------------------
bool VeFixedStringA::operator == (const VeFixedStringA& s) const
{
	return Equals(s);
}
//--------------------------------------------------------------------------
bool VeFixedStringA::operator != (const VeFixedStringA& s) const
{
	return !Equals(s);
}
//--------------------------------------------------------------------------
bool VeFixedStringA::operator==(const VeChar8* s) const
{
	return Equals(s);
}
//--------------------------------------------------------------------------
bool VeFixedStringA::operator!=(const VeChar8* s) const
{
	return !Equals(s);
}
//--------------------------------------------------------------------------
bool operator == (const VeChar8* s1, const VeFixedStringA& s2)
{
	return s2.Equals(s1);
}
//--------------------------------------------------------------------------
bool operator != (const VeChar8* s1, const VeFixedStringA& s2)
{
	return !(s2.Equals(s1));
}
//--------------------------------------------------------------------------
