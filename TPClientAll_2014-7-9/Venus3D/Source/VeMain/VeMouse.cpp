////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMouse.cpp
//  Version:     v1.00
//  Created:     23/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeMouse.inl"
#endif

//--------------------------------------------------------------------------
#ifdef VE_DIRECT_INPUT
#	define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#	include <dinput.h>
#endif
//--------------------------------------------------------------------------
VeMouse::VeMouse()
	: m_u32State(0)
#	if defined(VE_DIRECT_INPUT)
	, m_pkMouse(NULL)
#	elif defined(VE_PLATFORM_LINUX)
	, m_hCursor(0)
	, m_i32MouseX(0)
	, m_i32MouseY(0)
#	endif
{

}
//--------------------------------------------------------------------------
VeMouse::~VeMouse()
{

}
//--------------------------------------------------------------------------
void VeMouse::Init()
{
#	if defined(VE_DIRECT_INPUT)
	VE_ASSERT_EQ(g_pInputManager->m_pkDirectInput->CreateDevice(GUID_SysMouse, &m_pkMouse, NULL), S_OK);
	VE_ASSERT_EQ(m_pkMouse->SetDataFormat(&c_dfDIMouse), S_OK);
	VE_ASSERT_EQ(m_pkMouse->SetCooperativeLevel(g_hWindow, DISCL_FOREGROUND|DISCL_NONEXCLUSIVE), S_OK);
	DIPROPDWORD kDipdw;
	kDipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	kDipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	kDipdw.diph.dwObj        = 0;
	kDipdw.diph.dwHow        = DIPH_DEVICE;
	kDipdw.dwData            = VE_MOUSE_DX_BUFFERSIZE;
	VE_ASSERT_EQ(m_pkMouse->SetProperty(DIPROP_BUFFERSIZE, &kDipdw.diph), S_OK);
	m_pkMouse->Acquire();
#	elif defined(VE_PLATFORM_LINUX)
	Pixmap hNone;
	XColor kBlack, kDummy;
	Colormap kColorMap;
	VeChar8 acNoData[] = { 0,0,0,0,0,0,0,0 };
	kColorMap = DefaultColormap(g_pkDisplay, DefaultScreen(g_pkDisplay));
	XAllocNamedColor(g_pkDisplay, kColorMap, "black", &kBlack, &kDummy);
	hNone = XCreateBitmapFromData(g_pkDisplay, g_hWindow, acNoData, 8, 8);
	m_hCursor = XCreatePixmapCursor(g_pkDisplay, hNone, hNone, &kBlack, &kBlack, 0, 0);
#	endif
}
//--------------------------------------------------------------------------
void VeMouse::Term()
{
#	if defined(VE_DIRECT_INPUT)
	VE_SAFE_RELEASE(m_pkMouse);
#	elif defined(VE_PLATFORM_LINUX)
	XFreeCursor(g_pkDisplay, m_hCursor);
#	endif
}
//--------------------------------------------------------------------------
void VeMouse::OnButtonDown(State eBtn, VeSizeT stData)
{
	VE_ASSERT(eBtn <= BTN_BUTTON2);
	if(!VE_MASK_HAS_ANY(m_u32State, eBtn))
	{
		VE_MASK_ADD(m_u32State, eBtn);
		switch(eBtn)
		{
		case BTN_BUTTON0:
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_LBTN_PRESSED, g_pTime->GetTimeUInt(), stData);
			break;
		case BTN_BUTTON1:
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_RBTN_PRESSED, g_pTime->GetTimeUInt(), stData);
			break;
		case BTN_BUTTON2:
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_MBTN_PRESSED, g_pTime->GetTimeUInt(), stData);
			break;
		default:
			break;
		}
	}
}
//--------------------------------------------------------------------------
void VeMouse::OnButtonUp(State eBtn, VeSizeT stData)
{
	VE_ASSERT(eBtn <= BTN_BUTTON2);
	if(VE_MASK_HAS_ANY(m_u32State, eBtn))
	{
		VE_MASK_DEL(m_u32State, eBtn);
		switch(eBtn)
		{
		case BTN_BUTTON0:
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_LBTN_RELEASED, g_pTime->GetTimeUInt(), stData);
			break;
		case BTN_BUTTON1:
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_RBTN_RELEASED, g_pTime->GetTimeUInt(), stData);
			break;
		case BTN_BUTTON2:
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_MBTN_RELEASED, g_pTime->GetTimeUInt(), stData);
			break;
		default:
			break;
		}
	}
}
//--------------------------------------------------------------------------
void VeMouse::Update()
{
#	ifdef VE_DIRECT_INPUT
	DIDEVICEOBJECTDATA akData[VE_MOUSE_DX_BUFFERSIZE];
	DWORD dwEntries = VE_MOUSE_DX_BUFFERSIZE;
	HRESULT hRes = m_pkMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), akData, &dwEntries, 0);
	if(hRes != DI_OK)
	{
		hRes = m_pkMouse->Acquire();
		while(hRes == DIERR_INPUTLOST) 
			hRes = m_pkMouse->Acquire();
		hRes = m_pkMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), akData, &dwEntries, 0);
		if(FAILED(hRes)) return;
	}
	VeInt32 i32MoveX(0), i32MoveY(0), i32MoveZ(0);
	for(VeUInt32 i(0); i < dwEntries; ++i)
	{
		switch(akData[i].dwOfs)
		{
		case DIMOFS_X:
			i32MoveX += VeInt32(akData[i].dwData);
			break;
		case DIMOFS_Y:
			i32MoveY += VeInt32(akData[i].dwData);
			break;
		case DIMOFS_Z:
			i32MoveZ += VeInt32(akData[i].dwData);
			break;
		default:
			break;
		}
	}
	if(i32MoveX || i32MoveY || i32MoveZ)
	{
		VeInputMessage::MouseMoveParam* pkMouseMove;
		g_pInputManager->GetSingle(pkMouseMove);
		if(pkMouseMove)
		{
			pkMouseMove->x = i32MoveX;
			pkMouseMove->y = i32MoveY;
			pkMouseMove->z = i32MoveZ;
			g_pInputManager->AppendMessage(VeInputMessage::DT_MOUSE, VeInputMessage::MIT_MOUSE_MOVED, g_pTime->GetTimeUInt(), pkMouseMove);
		}
	}
#	endif
}
//--------------------------------------------------------------------------
void VeMouse::SetCursorPos(VeUInt16 x, VeUInt16 y)
{
#	if defined(VE_PLATFORM_WIN)
	POINT kPoint = {x, y};
	ClientToScreen(g_hWindow, &kPoint);
	::SetCursorPos(kPoint.x, kPoint.y);
#	endif
}
//--------------------------------------------------------------------------
void VeMouse::SetCursorPosF(VeFloat32 x, VeFloat32 y)
{
#	if defined(VE_PLATFORM_WIN)
	POINT kPoint = {LONG(x * g_pRenderer->GetWidthF()), LONG(y * g_pRenderer->GetHeightF())};
	ClientToScreen(g_hWindow, &kPoint);
	::SetCursorPos(kPoint.x, kPoint.y);
#	endif
}
//--------------------------------------------------------------------------
void VeMouse::SetHide(bool bHide)
{
	if(VE_MASK_HAS_ANY(m_u32State, CUR_HIDE) != bHide)
	{
		if(bHide)
		{
#			if defined(VE_PLATFORM_WIN)
			::ShowCursor(FALSE);
#			elif defined(VE_PLATFORM_LINUX)
			XDefineCursor(g_pkDisplay, g_hWindow, m_hCursor);
#			endif
			VE_MASK_ADD(m_u32State, CUR_HIDE);
		}
		else
		{
#			if defined(VE_PLATFORM_WIN)
			::ShowCursor(TRUE);
#			elif defined(VE_PLATFORM_LINUX)
			XUndefineCursor(g_pkDisplay, g_hWindow);
#			endif
			VE_MASK_DEL(m_u32State, CUR_HIDE);
		}
	}
}
//--------------------------------------------------------------------------
void VeMouse::SetLock(bool bLock)
{
	if(VE_MASK_HAS_ANY(m_u32State, CUR_LOCK) != bLock)
	{
		if(bLock)
		{
#			if defined(VE_PLATFORM_WIN)
			POINT kPoint = {g_pRenderer->GetWidth() >> 1, g_pRenderer->GetHeight() >> 1};
			ClientToScreen(g_hWindow, &kPoint);
			RECT kRect;
			kRect.left = kPoint.x;
			kRect.top = kPoint.y;
			kRect.right = kPoint.x;
			kRect.bottom = kPoint.y;
			ClipCursor(&kRect);
#			endif
			VE_MASK_ADD(m_u32State, CUR_LOCK);
		}
		else
		{
#			if defined(VE_PLATFORM_WIN)
			ClipCursor(NULL);
#			endif
			VE_MASK_DEL(m_u32State, CUR_LOCK);
		}
	}
}
//--------------------------------------------------------------------------
