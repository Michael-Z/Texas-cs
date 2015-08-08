////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Include File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeBackgroundTaskQueue.inl
//  Version:     v1.00
//  Created:     6/12/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
VE_INLINE VeBackgroundTask::VeBackgroundTask()
{
	m_kNode.m_tContent = this;
}
//--------------------------------------------------------------------------
VE_INLINE VeBackgroundTaskQueue::VeBackgroundTaskQueue(VeUInt32 u32Priority
	, VeSizeT stStackSize) : VeThread(u32Priority, stStackSize)
	, m_u32ActiveBackgroundTask(0), m_u32ActiveForegroundTask(1)
	, m_bTickOnce(false)
{

}
//--------------------------------------------------------------------------
VE_INLINE void VeBackgroundTaskQueue::SetTickOnce(bool bEnable)
{
	m_bTickOnce = bEnable;
}
//--------------------------------------------------------------------------
