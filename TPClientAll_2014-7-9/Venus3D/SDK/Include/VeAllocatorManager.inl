////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Include File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeAllocatorManager.inl
//  Version:     v1.00
//  Created:     6/12/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
VE_INLINE VeAllocatorManager::VeAllocatorManager()
	: m_kPoolAllocatorMap(32, VE_NEW VePoolAllocator(
	sizeof(VeHashMap<VeUInt32,VePoolAllocatorPtr>::Node), 32))
{
	m_kPoolAllocatorMap[
		sizeof(VeHashMap<VeUInt32,VePoolAllocatorPtr>::Node)
	] = m_kPoolAllocatorMap.GetAllocator();
}
//--------------------------------------------------------------------------
VE_INLINE VeAllocatorManager::~VeAllocatorManager()
{

}
//--------------------------------------------------------------------------
