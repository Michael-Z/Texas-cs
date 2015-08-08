////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Test Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeGraphicsTest.cpp
//  Version:     v1.00
//  Created:     27/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeGraphicsTest.h"




VeGraphicsTest::VeGraphicsTest()
{

}

VeGraphicsTest::~VeGraphicsTest()
{

}

void VeGraphicsTest::Render()
{
	g_pRenderer->Render();
	g_pRenderer->Present();
}

#if defined(VE_PLATFORM_ANDROID)
#	define AUDIO_DATA_DIR "asset#Audio"
#else
#	define AUDIO_DATA_DIR "file#Audio"
#endif

void VeGraphicsTest::OnInit()
{
	//VeSleep(5000);
	VeResourceGroupPtr spSoundGroup = VE_NEW VeResourceGroup("Sound");
	spSoundGroup->PushReadDirectoryBack(AUDIO_DATA_DIR);
	m_spTestSound = VeLoadSimpleSoundSync("stereo.wav", "Sound");
}

void VeGraphicsTest::OnTerm()
{
	m_spTestSound = NULL;
}

void VeGraphicsTest::OnBufferResized(VeUInt32 u32Width, VeUInt32 u32Height)
{
	VeDebugOutput("OnBufferResized: %dx%d", u32Width, u32Height);
}

void VeGraphicsTest::OnUpdate()
{

}

void VeGraphicsTest::OnKeyPressed(VeKeyboard::InputKey eKey)
{
	if(eKey == VeKeyboard::KEY_F4)
	{
		QuitApp();
	}
}

void VeGraphicsTest::OnKeyReleased(VeKeyboard::InputKey eKey)
{
	if(eKey == VeKeyboard::KEY_C)
	{
		g_pMouse->SetCursorPosF(0.5f, 0.5f);
	}
	else if(eKey == VeKeyboard::KEY_H)
	{
		g_pMouse->SetHide(!g_pMouse->GetStateFlag(VeMouse::CUR_HIDE));
	}
	else if(eKey == VeKeyboard::KEY_L)
	{
		g_pMouse->SetLock(!g_pMouse->GetStateFlag(VeMouse::CUR_LOCK));
	}
#if defined(VE_PLATFORM_PC)
	else if(eKey == VeKeyboard::KEY_U)
	{
		g_pRenderer->ResizeWindow(1024, 768);
	}
	else if(eKey == VeKeyboard::KEY_I)
	{
		g_pRenderer->ResizeWindow(800, 600);
	}
	else if(VE_MASK_HAS_ALL(g_pKeyboard->GetModifiers(), VeKeyboard::CTRL) && eKey == VeKeyboard::KEY_F)
	{
		g_pRenderer->SetFullScreen(!g_pStartConfig->IsFullScreen());
	}
#elif defined(VE_PLATFORM_MOBILE_SIM)
	else if(eKey == VeKeyboard::KEY_U)
	{
		g_pRenderer->ResizeWindow(1024, 768);
	}
	else if(eKey == VeKeyboard::KEY_I)
	{
		g_pRenderer->ResizeWindow(960, 640);
	}
#endif
}

void VeGraphicsTest::OnMousePressed(VeMouse::State eBtn, VeUInt16 x, VeUInt16 y)
{
	VeDebugOutput("MouseButton%d:Pressed Pos:%d,%d", eBtn, x, y);
}

void VeGraphicsTest::OnMouseReleased(VeMouse::State eBtn, VeUInt16 x, VeUInt16 y)
{
	VeDebugOutput("MouseButton%d:Released Pos:%d,%d", eBtn, x, y);
}

void VeGraphicsTest::OnCursorMove(VeUInt16 x, VeUInt16 y)
{
	//VeDebugOutput("CursorMove:%d,%d", x, y);
}

void VeGraphicsTest::OnMouseMove(VeUInt32 u32OffsetX, VeUInt32 u32OffsetY, VeUInt32 u32OffsetZ)
{
	//VeDebugOutput("MouseMove:%d,%d, %d", u32OffsetX, u32OffsetY, u32OffsetZ);
}

void VeGraphicsTest::OnJoyStickPressed(VeUInt32 u32Button)
{
	VeDebugOutput("OnJoyStickPressed: %d", u32Button);
}

void VeGraphicsTest::OnJoyStickReleased(VeUInt32 u32Button)
{
	VeDebugOutput("OnJoyStickReleased: %d", u32Button);
}

void VeGraphicsTest::OnJoyStickAxisChanged(VeUInt32 u32Axis, VeUInt32 u32Value)
{
	VeDebugOutput("Axis%dChanged: %d", u32Axis, u32Value);
}

void VeGraphicsTest::OnTouchBegan(VeUInt32 u32Time, VeUInt32 id, VeFloat32 x, VeFloat32 y)
{
    VeDebugOutput("OnTouchBegan[time:%d,id:%d,x:%.0f,y:%.0f]", u32Time, id, x, y);
    g_pAudioSystem->Play(m_spTestSound);
}

void VeGraphicsTest::OnTouchEnded(VeUInt32 u32Time, VeUInt32 id, VeFloat32 x, VeFloat32 y)
{
    VeDebugOutput("OnTouchEnded[time:%d,id:%d,x:%.0f,y:%.0f]", u32Time, id, x, y);
}

void VeGraphicsTest::OnTouchMoved(VeUInt32 u32Time, VeVector<VeInputMessage::TouchPointParam>& kParams)
{
    for(VeVector<VeInputMessage::TouchPointParam>::iterator it = kParams.Begin(); it != kParams.End(); ++it)
    {
        VeDebugOutput("OnTouchMoved[time:%d,id:%d,x:%.0f,y:%.0f]", u32Time, it->id, it->x, it->y);
    }
}

void VeGraphicsTest::OnTouchCancelled(VeUInt32 u32Time, VeVector<VeInputMessage::TouchPointParam>& kParams)
{
    for(VeVector<VeInputMessage::TouchPointParam>::iterator it = kParams.Begin(); it != kParams.End(); ++it)
    {
        VeDebugOutput("OnTouchCancelled[time:%d,id:%d,x:%.0f,y:%.0f]", u32Time, it->id, it->x, it->y);
    }
}
