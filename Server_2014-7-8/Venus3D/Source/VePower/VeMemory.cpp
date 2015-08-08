////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMemory.cpp
//  Version:     v1.00
//  Created:     5/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"
#ifdef VE_NO_INLINE
#	include "VeMemory.inl"
#endif

//--------------------------------------------------------------------------
void* operator new(VeSizeT stSize, void* pvMemory)
{
	return pvMemory;
}
//--------------------------------------------------------------------------
#ifdef VE_PLATFORM_APPLE
void* memalign(VeSizeT stAlignment, VeSizeT stByteSize)
{
	void* pvPointer(NULL);
	if(VE_SUCCEEDED(posix_memalign(&pvPointer, stAlignment, stByteSize)))
	{
		return pvPointer;
	}
	else
	{
		return NULL;
	}
}
#endif
//--------------------------------------------------------------------------
#ifdef VE_MEM_DEBUG
//--------------------------------------------------------------------------
static volatile VeSizeT s_stMallocCount(0);
static volatile VeSizeT s_stAlignedMallocCount(0);
//--------------------------------------------------------------------------
void _VeMemoryExit()
{
	VeMemObject::ReleaseDeleteCallStack();
	VE_ASSERT(!s_stMallocCount);
	VE_ASSERT(!s_stAlignedMallocCount);
}
//--------------------------------------------------------------------------
void* _VeMalloc(VeSizeT stSizeInBytes, const VeChar8* pcSourceFile,
	VeInt32 iSourceLine, const VeChar8* pcFunction)
{
	VeAtomicIncrement32(s_stMallocCount);
	return VeExternalMalloc(stSizeInBytes);
}
//--------------------------------------------------------------------------
void* _VeAlignedMalloc(VeSizeT stSizeInBytes, VeSizeT stAlignment,
	const VeChar8* pcSourceFile, VeInt32 iSourceLine,
	const VeChar8* pcFunction)
{
	VeAtomicIncrement32(s_stAlignedMallocCount);
	return VeExternalAlignedMalloc(stSizeInBytes, stAlignment);
}
//--------------------------------------------------------------------------
void* _VeRealloc(void* pvMemblock, VeSizeT stSizeInBytes,
	const VeChar8* pcSourceFile, VeInt32 iSourceLine, const VeChar8* pcFunction)
{
	return VeExternalRealloc(pvMemblock, stSizeInBytes);
}
//--------------------------------------------------------------------------
void _VeFree(void* pvMemory, const VeChar8* pcSourceFile,
	VeInt32 iSourceLine, const VeChar8* pcFunction)
{
	VeAtomicDecrement32(s_stMallocCount);
	return VeExternalFree(pvMemory);
}
//--------------------------------------------------------------------------
void _VeAlignedFree(void* pvMemory, const VeChar8* pcSourceFile,
	VeInt32 iSourceLine, const VeChar8* pcFunction)
{
	VeAtomicDecrement32(s_stAlignedMallocCount);
	return VeExternalAlignedFree(pvMemory);
}
//--------------------------------------------------------------------------
void* operator new (VeSizeT stSize, const VeChar8* pcSourceFile,
	VeInt32 iSourceLine, const VeChar8* pcFunction)
{
	return (void*)NULL;
}
//--------------------------------------------------------------------------
void* operator new [] (VeSizeT stSize, const VeChar8* pcSourceFile,
	VeInt32 iSourceLine, const VeChar8* pcFunction)
{
	return (void*)NULL;
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
