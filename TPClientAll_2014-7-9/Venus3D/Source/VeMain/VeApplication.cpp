////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeApplication.cpp
//  Version:     v1.00
//  Created:     27/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeApplication.inl"
#endif

//--------------------------------------------------------------------------
VeApplication::~VeApplication()
{

}
//--------------------------------------------------------------------------
void VeApplication::Update()
{
#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_LINUX)
	if(m_bActive)
	{
		if(g_bMinimized || (!g_bActive))
		{
			m_bActive = false;
			OnInactive();
			g_pRenderer->OnInactive();
		}
	}
	else
	{
		if((!g_bMinimized) && g_bActive)
		{
			m_bActive = true;
			g_pRenderer->OnActive();
			OnActive();
		}
	}

	if(!m_bActive)
	{
		VeSleep(VE_INACTIVE_SLEEP);
	}
#endif

	g_pTime->Update();
	g_pURL->Update();
	g_pResourceManager->Update();

	if(!m_bPause)
	{
		g_pInputManager->Update();
		OnUpdate();
	}

#ifdef VE_PLATFORM_PC
	{
		static VeFloat32 s_f32TimeCount(0);
		static VeUInt32 s_u32FrameCount(0);

		s_f32TimeCount += g_pTime->GetDeltaTime();
		++s_u32FrameCount;

		if(s_f32TimeCount > 1.0f)
		{
			static VeChar8 s_acFPS[64];
			VeSprintf(s_acFPS, 64, "FPS:%d", s_u32FrameCount);
			VeSetWindowText(s_acFPS);
			s_f32TimeCount -= floorf(s_f32TimeCount);
			s_u32FrameCount = 0;
		}
	}
#endif
}
//--------------------------------------------------------------------------
void VeApplication::Render()
{
	g_pRenderer->Render();
	g_pRenderer->Present();
}
//--------------------------------------------------------------------------
void VeApplication::InputCallback(const VeInputMessage& kMessage)
{
	if(m_bProcessInput)
	{
		switch(kMessage.m_eDeviceType)
		{
		case VeInputMessage::DT_KEYBOARD:
			switch(kMessage.m_u32InputType)
			{
			case VeInputMessage::KIT_KEY_PRESSED:
				OnKeyPressed((VeKeyboard::InputKey)kMessage.m_stMessageInfo);
				break;
			case VeInputMessage::KIT_KEY_RELEASED:
				OnKeyReleased((VeKeyboard::InputKey)kMessage.m_stMessageInfo);
				break;
			default:
				break;
			}
			break;
		case VeInputMessage::DT_MOUSE:
			switch(kMessage.m_u32InputType)
			{
			case VeInputMessage::MIT_LBTN_PRESSED:
				OnMousePressed(VeMouse::BTN_BUTTON0, kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_RBTN_PRESSED:
				OnMousePressed(VeMouse::BTN_BUTTON1, kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_MBTN_PRESSED:
				OnMousePressed(VeMouse::BTN_BUTTON2, kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_LBTN_RELEASED:
				OnMouseReleased(VeMouse::BTN_BUTTON0, kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_RBTN_RELEASED:
				OnMouseReleased(VeMouse::BTN_BUTTON1, kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_MBTN_RELEASED:
				OnMouseReleased(VeMouse::BTN_BUTTON2, kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_CURSOR_MOVED:
				OnCursorMove(kMessage.m_kMouseCursorPos.x, kMessage.m_kMouseCursorPos.y);
				break;
			case VeInputMessage::MIT_MOUSE_MOVED:
				OnMouseMove(kMessage.m_kMouseMoveParam->x, kMessage.m_kMouseMoveParam->y, kMessage.m_kMouseMoveParam->z);
				break;
			default:
				break;
			}
			break;
		case VeInputMessage::DT_JOYSTICK:
			switch(kMessage.m_u32InputType)
			{
			case VeInputMessage::JIT_BTN_PRESSED:
				OnJoyStickPressed(VeUInt32(kMessage.m_stMessageInfo));
				break;
			case VeInputMessage::JIT_BTN_RELEASED:
				OnJoyStickReleased(VeUInt32(kMessage.m_stMessageInfo));
				break;
			default:
				if(kMessage.m_u32InputType >= VeInputMessage::JIT_AXIS_MOVED)
				{
					OnJoyStickAxisChanged(kMessage.m_u32InputType - VeInputMessage::JIT_AXIS_MOVED, VeUInt32(kMessage.m_stMessageInfo));
				}
				break;
			}
			break;
		case VeInputMessage::DT_TOUCH:
			break;
		default:
			break;
		}
	}
}
//--------------------------------------------------------------------------
