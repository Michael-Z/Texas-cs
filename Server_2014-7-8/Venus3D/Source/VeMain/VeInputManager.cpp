////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeInputManager.cpp
//  Version:     v1.00
//  Created:     21/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeInputManager.inl"
#endif
#ifdef VE_DIRECT_INPUT
#	define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#	include <dinput.h>
#endif

//--------------------------------------------------------------------------
VeInputManager::VeInputManager()
	: m_u32VectorUse(0)
	, m_u32VectorArrayUse(0)
	,m_bMessageHooked(false)
#	ifdef VE_DIRECT_INPUT
	, m_pkDirectInput(NULL)
#	endif
{
#	ifdef VE_INPUT_KEYBOARD
	VE_NEW VeKeyboard();
#	endif
#	ifdef VE_INPUT_MOUSE
	VE_NEW VeMouse();
#	endif
#	ifdef VE_INPUT_JOYSTICK
	VE_NEW VeJoyStick();
#	endif
}
//--------------------------------------------------------------------------
VeInputManager::~VeInputManager()
{
#	ifdef VE_INPUT_JOYSTICK
	VeJoyStick::Destory();
#	endif
#	ifdef VE_INPUT_MOUSE
	VeMouse::Destory();
#	endif
#	ifdef VE_INPUT_KEYBOARD
	VeKeyboard::Destory();
#	endif
}
//--------------------------------------------------------------------------
void VeInputManager::Init()
{
#	ifdef VE_DIRECT_INPUT
	VE_ASSERT_EQ(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_pkDirectInput, NULL), S_OK);
#	endif
#	ifdef VE_INPUT_KEYBOARD
	g_pKeyboard->Init();
#	endif
#	ifdef VE_INPUT_MOUSE
	g_pMouse->Init();
#	endif
#	ifdef VE_INPUT_JOYSTICK
	g_pJoyStick->Init();
#	endif
}
//--------------------------------------------------------------------------
void VeInputManager::Term()
{
#	ifdef VE_INPUT_JOYSTICK
	g_pJoyStick->Term();
#	endif
#	ifdef VE_INPUT_MOUSE
	g_pMouse->Term();
#	endif
#	ifdef VE_INPUT_KEYBOARD
	g_pKeyboard->Term();
#	endif
#	ifdef VE_DIRECT_INPUT
	VE_SAFE_RELEASE(m_pkDirectInput);
#	endif
}
//--------------------------------------------------------------------------
void VeInputManager::Update(bool bUpdateDevice)
{
	if(bUpdateDevice)
	{
#	ifdef VE_INPUT_KEYBOARD
		g_pKeyboard->Update();
#	endif
#	ifdef VE_INPUT_MOUSE
		g_pMouse->Update();
#	endif
#	ifdef VE_INPUT_JOYSTICK
		g_pJoyStick->Update();
#	endif
	}
	while(m_kMessageQueue.Size())
	{
		ProcessMessage(m_kMessageQueue.Front());
		PopMessage();
	}
	VE_ASSERT(m_kMessageQueue.Empty());
	m_u32VectorUse = 0;
	m_u32VectorArrayUse = 0;
}
//--------------------------------------------------------------------------
