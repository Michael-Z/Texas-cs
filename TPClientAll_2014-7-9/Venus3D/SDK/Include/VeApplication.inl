////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Include File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeApplication.inl
//  Version:     v1.00
//  Created:     7/12/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
VE_INLINE VeApplication::VeApplication() : m_bQuit(false)
	, m_bProcessInput(true), m_bActive(true), m_bPause(false)
{
	m_kInputDelegate.Set(this, &VeApplication::InputCallback);
}
//--------------------------------------------------------------------------
VE_INLINE void VeApplication::Init()
{
	g_pInputManager->RegisterListener(m_kInputDelegate);
	OnInit();
}
//--------------------------------------------------------------------------
VE_INLINE void VeApplication::Term()
{
	OnTerm();
	g_pInputManager->UnregisterListener(m_kInputDelegate);
}
//--------------------------------------------------------------------------
VE_INLINE void VeApplication::Pause()
{
	m_bPause = true; 
	OnPause();
}
//--------------------------------------------------------------------------
VE_INLINE void VeApplication::Resume()
{
	m_bPause = false;
	OnResume();
}
//--------------------------------------------------------------------------
VE_INLINE void VeApplication::SetProcessInput(bool bEnable)
{
	m_bProcessInput = bEnable;
}
//--------------------------------------------------------------------------
VE_INLINE bool VeApplication::GetProcessInput()
{
	return m_bProcessInput;
}
//--------------------------------------------------------------------------
VE_INLINE void VeApplication::QuitApp()
{
	m_bQuit = true;
}
//--------------------------------------------------------------------------
VE_INLINE bool VeApplication::IsAppQuit()
{
	return m_bQuit;
}
//--------------------------------------------------------------------------
