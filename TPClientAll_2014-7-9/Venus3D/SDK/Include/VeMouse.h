////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMouse.h
//  Version:     v1.00
//  Created:     23/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_MAIN_API VeMouse : public VeSingleton<VeMouse>
{
public:
	enum State
	{
		BTN_BUTTON0		= 0x01,
		BTN_BUTTON1		= 0x02,
		BTN_BUTTON2		= 0x04,
		CUR_HIDE		= 0x08,
		CUR_LOCK		= 0x10
	};

	VeMouse();

	~VeMouse();

	void Init();

	void Term();
	
	void OnButtonDown(State eBtn, VeSizeT stData);

	void OnButtonUp(State eBtn, VeSizeT stData);

	VE_INLINE void OnCursorMove(VeSizeT stData);

	VE_INLINE bool GetStateFlag(State eBtn);

	void Update();

	void SetCursorPos(VeUInt16 x, VeUInt16 y);

	void SetCursorPosF(VeFloat32 x, VeFloat32 y);

	void SetHide(bool bHide);

	void SetLock(bool bLock);

protected:
	VeUInt32 m_u32State;
#	if defined(VE_DIRECT_INPUT)
	IDirectInputDevice8* m_pkMouse;
#	elif defined(VE_PLATFORM_LINUX)
	Cursor m_hCursor;
	VeInt32 m_i32MouseX;
	VeInt32 m_i32MouseY;
#	endif

};

#define g_pMouse VeMouse::GetSingletonPtr()

#ifndef VE_NO_INLINE
#	include "VeMouse.inl"
#endif
