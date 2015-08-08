////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeBackgroundTaskQueue.cpp
//  Version:     v1.00
//  Created:     6/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"
#ifdef VE_NO_INLINE
#	include "VeBackgroundTaskQueue.inl"
#endif

//--------------------------------------------------------------------------
VeBackgroundTask::~VeBackgroundTask()
{

}
//--------------------------------------------------------------------------
VeBackgroundTaskQueue::~VeBackgroundTaskQueue()
{
	VE_ASSERT(!IsRunning());
	VE_ASSERT(m_kBackgroundTasks[!m_u32ActiveBackgroundTask].Size() == 0);
	VE_ASSERT(m_kForegroundTasks[!m_u32ActiveForegroundTask].Size() == 0);

	{
		VeRefList<VeBackgroundTask*>& kTasks = m_kBackgroundTasks[m_u32ActiveBackgroundTask];
		kTasks.BeginIterator();
		while(!kTasks.IsEnd())
		{
			VeBackgroundTask* pkTask = kTasks.GetIterNode()->m_tContent;
			kTasks.Next();
			pkTask->m_kNode.Detach();
			pkTask->DecRefCount();
		}
		VE_ASSERT(kTasks.Size() == 0);
	}

	{
		VeRefList<VeBackgroundTask*>& kTasks = m_kForegroundTasks[m_u32ActiveForegroundTask];
		kTasks.BeginIterator();
		while(!kTasks.IsEnd())
		{
			VeBackgroundTask* pkTask = kTasks.GetIterNode()->m_tContent;
			kTasks.Next();
			pkTask->m_kNode.Detach();
			pkTask->DecRefCount();
		}
		VE_ASSERT(kTasks.Size() == 0);
	}
}
//--------------------------------------------------------------------------
void VeBackgroundTaskQueue::AddBackgroundTask(const VeBackgroundTaskPtr& spTask)
{
	spTask->IncRefCount();
	m_kBGMutex.Lock();
	m_kBackgroundTasks[m_u32ActiveBackgroundTask].AttachBack(spTask->m_kNode);
	m_kBGMutex.Unlock();
}
//--------------------------------------------------------------------------
void VeBackgroundTaskQueue::AddMainThreadTask(const VeBackgroundTaskPtr& spTask)
{
	spTask->IncRefCount();
	m_kFGMutex.Lock();
	m_kForegroundTasks[m_u32ActiveForegroundTask].AttachBack(spTask->m_kNode);
	m_kFGMutex.Unlock();
}
//--------------------------------------------------------------------------
void VeBackgroundTaskQueue::Run()
{
	m_kBGMutex.Lock();
	VeRefList<VeBackgroundTask*>& kTasks = m_kBackgroundTasks[!m_u32ActiveBackgroundTask];
	m_kBGMutex.Unlock();
	kTasks.BeginIterator();
	while(!kTasks.IsEnd())
	{
		VeBackgroundTask* pkTask = kTasks.GetIterNode()->m_tContent;
		kTasks.Next();
		pkTask->m_kNode.Detach();
		pkTask->DoBackgroundTask(*this);
		pkTask->DecRefCount();
	}
	VE_ASSERT(kTasks.Size() == 0);
}
//--------------------------------------------------------------------------
void VeBackgroundTaskQueue::Update()
{
	if(!IsRunning())
	{
		m_kBGMutex.Lock();
		VeUInt32 u32Size = m_kBackgroundTasks[m_u32ActiveBackgroundTask].Size();
		if(u32Size)
		{
			m_u32ActiveBackgroundTask = !m_u32ActiveBackgroundTask;
			VE_ASSERT(m_kBackgroundTasks[m_u32ActiveBackgroundTask].Size() == 0);
			Start();
		}
		m_kBGMutex.Unlock();
	}

	if(m_bTickOnce)
	{
		m_kFGMutex.Lock();
		if(m_kForegroundTasks[!m_u32ActiveForegroundTask].Size() == 0)
		{
			m_u32ActiveForegroundTask = !m_u32ActiveForegroundTask;
			VE_ASSERT(m_kForegroundTasks[m_u32ActiveForegroundTask].Size() == 0);
		}
		m_kFGMutex.Unlock();
		VeRefList<VeBackgroundTask*>& kTasks = m_kForegroundTasks[!m_u32ActiveForegroundTask];
		if(kTasks.Size())
		{
			VeBackgroundTask* pkTask = kTasks.GetHeadNode()->m_tContent;
			pkTask->m_kNode.Detach();
			pkTask->DoMainThreadTask(*this);
			pkTask->DecRefCount();
		}
	}
	else
	{
		m_kFGMutex.Lock();
		m_u32ActiveForegroundTask = !m_u32ActiveForegroundTask;
		VE_ASSERT(m_kForegroundTasks[m_u32ActiveForegroundTask].Size() == 0);
		m_kFGMutex.Unlock();

		VeRefList<VeBackgroundTask*>& kTasks = m_kForegroundTasks[!m_u32ActiveForegroundTask];
		kTasks.BeginIterator();
		while(!kTasks.IsEnd())
		{
			VeBackgroundTask* pkTask = kTasks.GetIterNode()->m_tContent;
			kTasks.Next();
			pkTask->m_kNode.Detach();
			pkTask->DoMainThreadTask(*this);
			pkTask->DecRefCount();
		}
		VE_ASSERT(kTasks.Size() == 0);
	}
}
//--------------------------------------------------------------------------
bool VeBackgroundTaskQueue::HasTask()
{
	if(IsRunning())
	{
		return true;
	}
	else
	{
		VE_AUTO_LOCK_MUTEX(m_kBGMutex);
		VE_AUTO_LOCK_MUTEX(m_kFGMutex);
		if(m_kBackgroundTasks[m_u32ActiveBackgroundTask].Size())
		{
			return true;
		}
		if(m_kForegroundTasks[m_u32ActiveForegroundTask].Size())
		{
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------
