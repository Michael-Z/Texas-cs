////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Include File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeThread.inl
//  Version:     v1.00
//  Created:     6/12/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
VE_INLINE VeThread::Mutex::Mutex()
{
	VeThreadMutexInit(m_kMutex);
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::Mutex::~Mutex()
{
	VeThreadMutexTerm(m_kMutex);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Mutex::Lock()
{
	VeThreadMutexLock(m_kMutex);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Mutex::Unlock()
{
	VeThreadMutexUnlock(m_kMutex);
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::Mutex& VeThread::Mutex::GetRef()
{
	return *this;
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::MutexHolder::MutexHolder(Mutex& kMutex)
	: m_kMutex(kMutex)
{
	m_kMutex.Lock();
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::MutexHolder::~MutexHolder()
{
	m_kMutex.Unlock();
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::Event::Event(bool bState)
{
	VeThreadEventInit(&m_kEvent, bState);
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::Event::~Event()
{
	VeThreadEventTerm(&m_kEvent);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Event::Set()
{
	VeThreadEventSet(&m_kEvent);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Event::Reset()
{
	VeThreadEventReset(&m_kEvent);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Wait(Event& kEvent)
{
	VeThreadEventWait(&(kEvent.m_kEvent));
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Wait(Event& kEvent, VeUInt32 u32Milliseconds)
{
	VeThreadEventWait(&(kEvent.m_kEvent), u32Milliseconds);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Join(VeThread& kThread)
{
	Wait(kThread.m_pkParams->m_kEvent);
}
//--------------------------------------------------------------------------
VE_INLINE VeThread::VeThread(VeUInt32 u32Priority, VeSizeT stStackSize)
	: m_u32State(0)
{
	m_pkParams = VE_NEW ThreadParams;
	m_pkParams->m_kEvent.Set();
	m_pkParams->m_pkThis = this;
	m_hThread = VeCreateThread(Callback, m_pkParams, u32Priority, stStackSize);
	VE_ASSERT(m_hThread);
}
//--------------------------------------------------------------------------
VE_INLINE void VeThread::Start()
{
	if(!m_u32State)
	{
		m_kMutex.Lock();
		++m_u32State;
		m_pkParams->m_kEventLoop.Set();
		m_pkParams->m_kEvent.Reset();
		m_kMutex.Unlock();
	}
}
//--------------------------------------------------------------------------
VE_INLINE bool VeThread::IsRunning()
{
	m_kMutex.Lock();
	bool bRes = m_u32State > 0;
	m_kMutex.Unlock();
	return bRes;
}
//--------------------------------------------------------------------------
