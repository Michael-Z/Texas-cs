////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeRefObject.h
//  Version:     v1.00
//  Created:     7/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_POWER_API VeRefObject : public VeMemObject
{
public:
	VE_INLINE VeRefObject();

	virtual ~VeRefObject();

	VE_INLINE void IncRefCount();

	VE_INLINE void DecRefCount();

	VE_INLINE VeUInt32 GetRefCount() const;

	VE_INLINE static VeUInt32 GetTotalObjectCount();

protected:
	virtual void DeleteThis();

private:
	VeUInt32 m_u32RefCount;
	static VeUInt32 ms_u32Objects;
};

class VE_POWER_API VeSharedRefObject : public VeMemObject
{
public:
	VeSharedRefObject();

	virtual ~VeSharedRefObject();

	void IncRefCount();

	void DecRefCount();

	VE_INLINE VeUInt32 GetRefCount() const;

	VE_INLINE static VeUInt32 GetTotalObjectCount();

protected:
	virtual void DeleteThis();

private:
	volatile VeUInt32 m_u32RefCount;
	static VeUInt32 ms_u32Objects;
};

#ifndef VE_NO_INLINE
#	include "VeRefObject.inl"
#endif
