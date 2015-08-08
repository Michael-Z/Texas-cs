////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeApplication.h
//  Version:     v1.00
//  Created:     27/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#define VE_INACTIVE_SLEEP (30)

class VE_MAIN_API VeApplication : public VeSingleton<VeApplication>
{
public:
	typedef VeMemberDelegate<VeApplication, const VeInputMessage&> InputDelegate;

	virtual ~VeApplication();

	VE_INLINE void Init();

	VE_INLINE void Term();

	VE_INLINE void Pause();

	VE_INLINE void Resume();

	virtual void Update();

	virtual void Render();

	virtual void OnInit() {};

	virtual void OnTerm() {};

	virtual void OnRendererReleaseData() {};

	virtual void OnRendererRestoreData() {};

	virtual void OnBufferResized(VeUInt32 u32Width, VeUInt32 u32Height) {};

	virtual void OnPause() {};

	virtual void OnResume() {};

	virtual void OnUpdate() {};

	virtual void OnActive() {};

	virtual void OnInactive() {};

	virtual void OnKeyPressed(VeKeyboard::InputKey eKey) {};

	virtual void OnKeyReleased(VeKeyboard::InputKey eKey) {};

	virtual void OnMousePressed(VeMouse::State eBtn, VeUInt16 x, VeUInt16 y) {};

	virtual void OnMouseReleased(VeMouse::State eBtn, VeUInt16 x, VeUInt16 y) {};

	virtual void OnCursorMove(VeUInt16 x, VeUInt16 y) {};

	virtual void OnMouseMove(VeUInt32 u32OffsetX, VeUInt32 u32OffsetY, VeUInt32 u32OffsetZ) {};

	virtual void OnJoyStickPressed(VeUInt32 u32Button) {};

	virtual void OnJoyStickReleased(VeUInt32 u32Button) {};

	virtual void OnJoyStickAxisChanged(VeUInt32 u32Axis, VeUInt32 u32Value) {};

	VE_INLINE void SetProcessInput(bool bEnable);

	VE_INLINE bool GetProcessInput();

	VE_INLINE void QuitApp();

	VE_INLINE bool IsAppQuit();

protected:
	VE_INLINE VeApplication();

	void InputCallback(const VeInputMessage& kMessage);

	InputDelegate m_kInputDelegate;
	bool m_bQuit;
	bool m_bProcessInput;
	bool m_bActive;
	bool m_bPause;
	
};

#define g_pApplication VeApplication::GetSingletonPtr()

#ifndef VE_NO_INLINE
#	include "VeApplication.inl"
#endif
