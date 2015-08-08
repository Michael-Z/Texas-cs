////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeRefObject.cpp
//  Version:     v1.00
//  Created:     7/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"
#ifdef VE_NO_INLINE
#	include "VeRefObject.inl"
#endif

//--------------------------------------------------------------------------
VeUInt32 VeRefObject::ms_u32Objects = 0;
//--------------------------------------------------------------------------
VeRefObject::~VeRefObject()
{
	--ms_u32Objects;
}
//--------------------------------------------------------------------------
void VeRefObject::DeleteThis()
{
	VE_DELETE(this);
}
//--------------------------------------------------------------------------
VeUInt32 VeSharedRefObject::ms_u32Objects = 0;
//--------------------------------------------------------------------------
VeSharedRefObject::~VeSharedRefObject()
{
	VeAtomicDecrement32(ms_u32Objects);
}
//--------------------------------------------------------------------------
VeSharedRefObject::VeSharedRefObject()
{
	m_u32RefCount = 0;
	VeAtomicIncrement32(ms_u32Objects);
}
//--------------------------------------------------------------------------
void VeSharedRefObject::IncRefCount()
{
	VeAtomicIncrement32(m_u32RefCount);
}
//--------------------------------------------------------------------------
void VeSharedRefObject::DecRefCount()
{
	if(VeAtomicDecrement32(m_u32RefCount) == 0)
		DeleteThis();
}
//--------------------------------------------------------------------------
void VeSharedRefObject::DeleteThis()
{
	VE_DELETE(this);
}
//--------------------------------------------------------------------------
