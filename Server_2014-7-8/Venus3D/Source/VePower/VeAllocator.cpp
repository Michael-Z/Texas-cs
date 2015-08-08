////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeAllocator.cpp
//  Version:     v1.00
//  Created:     28/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"
#ifdef VE_NO_INLINE
#	include "VeAllocator.inl"
#endif

//--------------------------------------------------------------------------
VePoolAllocator::VePoolAllocator(VeUInt32 u32ObjectSize
	, VeUInt32 u32PoolObjectCount)
	: m_u32ObjectSize(u32ObjectSize + sizeof(VeSizeT))
	, m_u32PoolObjectCount(u32PoolObjectCount)
	, m_pkPoolList(NULL)
	, m_pvFreeMemHead(NULL)
#	ifdef VE_MEM_DEBUG
	, m_u32MallocCount(0)
	, m_u32PoolCount(0)
#	endif
{
	AddPool();
}
//--------------------------------------------------------------------------
VePoolAllocator::~VePoolAllocator()
{
	m_pvFreeMemHead = NULL;

	while(m_pkPoolList)
	{
		Pool* pkPre = m_pkPoolList->m_pvPre;
		VeFree(m_pkPoolList->m_pvBuffer);
		VeFree(m_pkPoolList);
		m_pkPoolList = pkPre;
	}
}
//--------------------------------------------------------------------------
void VePoolAllocator::AddPool()
{
#	ifdef VE_MEM_DEBUG
	++m_u32PoolCount;
#	endif
	Pool* pkNewPool = VeAlloc(Pool, 1);
	pkNewPool->m_pvBuffer
		= VeMalloc(m_u32ObjectSize * m_u32PoolObjectCount);
	pkNewPool->m_pvFreeStart = (void*)(((VeSizeT)pkNewPool->m_pvBuffer)
		+ m_u32ObjectSize * m_u32PoolObjectCount);
	pkNewPool->m_pvPre = m_pkPoolList;
	m_pkPoolList = pkNewPool;
}
//--------------------------------------------------------------------------
void* VePoolAllocator::MallocInternal()
{
	VE_ASSERT(m_pkPoolList);
	if(m_pkPoolList->m_pvBuffer != m_pkPoolList->m_pvFreeStart)
	{
		m_pkPoolList->m_pvFreeStart =
			(void*)((VeSizeT)m_pkPoolList->m_pvFreeStart - m_u32ObjectSize);
		return m_pkPoolList->m_pvFreeStart;
	}

	if(m_pvFreeMemHead)
	{
		void* pvRes = m_pvFreeMemHead;
		m_pvFreeMemHead
			= (void*)(*(VeSizeT*)(((VeByte*)m_pvFreeMemHead)
			+ m_u32ObjectSize - sizeof(VeSizeT)));
		return pvRes;
	}

	AddPool();

	return MallocInternal();
}
//--------------------------------------------------------------------------
void VePoolAllocator::FreeInternal(void* pvMemory)
{
	*(VeSizeT*)((VeByte*)pvMemory + m_u32ObjectSize - sizeof(VeSizeT))
		= (VeSizeT)m_pvFreeMemHead;
	m_pvFreeMemHead = pvMemory;
}
//--------------------------------------------------------------------------
const VePoolAllocatorPtr& VePoolAllocator::GetPoolAllocator(
	VeUInt32 u32Size)
{
	return g_pAllocatorManager->GetPoolAllocator(u32Size);
}
//--------------------------------------------------------------------------
