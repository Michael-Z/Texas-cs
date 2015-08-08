////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeJoyStick.cpp
//  Version:     v1.00
//  Created:     24/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeJoyStick.inl"
#endif

//--------------------------------------------------------------------------
#ifdef VE_DIRECT_INPUT
//--------------------------------------------------------------------------
#define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#include <dinput.h>
//--------------------------------------------------------------------------
#undef DIJOFS_BUTTON
#undef DIJOFS_POV
//--------------------------------------------------------------------------
#define DIJOFS_BUTTON(n)  (FIELD_OFFSET(DIJOYSTATE2, rgbButtons) + (n))
#define DIJOFS_POV(n)     (FIELD_OFFSET(DIJOYSTATE2, rgdwPOV)+(n)*sizeof(DWORD))
#define DIJOFS_SLIDER0(n) (FIELD_OFFSET(DIJOYSTATE2, rglSlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER1(n) (FIELD_OFFSET(DIJOYSTATE2, rglVSlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER2(n) (FIELD_OFFSET(DIJOYSTATE2, rglASlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER3(n) (FIELD_OFFSET(DIJOYSTATE2, rglFSlider)+(n) * sizeof(LONG))
//--------------------------------------------------------------------------
BOOL CALLBACK VeEnumJoyStickObjectsCallback(LPCDIDEVICEOBJECTINSTANCE pkDeviceInstance, LPVOID pvData)
{
	VeJoyStick::Device& kDevice = *(VeJoyStick::Device*)pvData;

	if(pkDeviceInstance->guidType == GUID_Slider)
	{
		kDevice.m_kSliders.Resize(kDevice.m_kSliders.Size() + 1);
		kDevice.m_kAxes.PopBack();
	}
	else
	{
		DIPROPPOINTER kDiptr;
		kDiptr.diph.dwSize       = sizeof(DIPROPPOINTER);
		kDiptr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		kDiptr.diph.dwHow        = DIPH_BYID;
		kDiptr.diph.dwObj        = pkDeviceInstance->dwType;
		kDiptr.uData             = 0x80000000 | kDevice.m_u32AxisNumber;

		if(FAILED(kDevice.m_pkJoyStick->SetProperty(DIPROP_APPDATA, &kDiptr.diph)))
		{
			return DIENUM_CONTINUE;
		}
	}

	++kDevice.m_u32AxisNumber;

	DIPROPRANGE kDiprg;
	kDiprg.diph.dwSize       = sizeof(DIPROPRANGE);
	kDiprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	kDiprg.diph.dwHow        = DIPH_BYID;
	kDiprg.diph.dwObj        = pkDeviceInstance->dwType;
	kDiprg.lMin              = VE_INT16_MIN;
	kDiprg.lMax              = VE_INT16_MAX;

	VE_ASSERT_EQ(kDevice.m_pkJoyStick->SetProperty(DIPROP_RANGE, &kDiprg.diph), S_OK);

	return DIENUM_CONTINUE;
}
//--------------------------------------------------------------------------
BOOL CALLBACK VeEnumJoyStickCallback(LPCDIDEVICEINSTANCE pkDeviceInstance, LPVOID pvData)
{
	IDirectInput8* pkDirectInput = (IDirectInput8*)pvData;
	VeVector<VeJoyStick::Device>& kDeviceArray = g_pJoyStick->GetDeviceArray();
	kDeviceArray.Resize(kDeviceArray.Size() + 1);
	VeJoyStick::Device& kDevice = kDeviceArray.Back();
	HRESULT hRes = pkDirectInput->CreateDevice(pkDeviceInstance->guidInstance, &kDevice.m_pkJoyStick, NULL);
	if(SUCCEEDED(hRes))
	{
		VE_ASSERT_EQ(kDevice.m_pkJoyStick->SetDataFormat(&c_dfDIJoystick2), S_OK);
		VE_ASSERT_EQ(kDevice.m_pkJoyStick->SetCooperativeLevel(g_hWindow, DISCL_FOREGROUND|DISCL_EXCLUSIVE), S_OK);
		DIPROPDWORD kDipdw;
		kDipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		kDipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		kDipdw.diph.dwObj        = 0;
		kDipdw.diph.dwHow        = DIPH_DEVICE;
		kDipdw.dwData            = VE_JOYSTICK_DX_BUFFERSIZE;
		VE_ASSERT_EQ(kDevice.m_pkJoyStick->SetProperty(DIPROP_BUFFERSIZE, &kDipdw.diph), S_OK);
		DIDEVCAPS kJoyCaps;
		kJoyCaps.dwSize = sizeof(DIDEVCAPS);
		VE_ASSERT_EQ(kDevice.m_pkJoyStick->GetCapabilities(&kJoyCaps), S_OK);
		kDevice.m_kButtons.Resize(kJoyCaps.dwButtons);
		kDevice.m_kAxes.Resize(kJoyCaps.dwAxes);
		kDevice.m_kPovDirections.Resize(kJoyCaps.dwPOVs);
		kDevice.m_pkJoyStick->EnumObjects(VeEnumJoyStickObjectsCallback, &kDevice, DIDFT_AXIS);
		kDevice.Reset();
	}
	return DIENUM_CONTINUE;
}
//--------------------------------------------------------------------------
void VeChangePov(VeJoyStick::Device& kDevice, VeUInt32 u32Pov, DIDEVICEOBJECTDATA& kData)
{
	VE_ASSERT(u32Pov < 4);
	if(LOWORD(kData.dwData) == 0xFFFF)
	{
		kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_CENTERED;
	}
	else
	{
		switch(kData.dwData)
		{
		case 0:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_NORTH;
			break;
		case 4500:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_NORTH_EAST;
			break;
		case 9000:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_EAST;
			break;
		case 13500:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_SOUTH_EAST;
			break;
		case 18000:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_SOUTH;
			break;
		case 22500:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_SOUTH_WEST;
			break;
		case 27000:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_WEST;
			break;
		case 31500:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_NORTH_WEST;
			break;
		default:
			kDevice.m_kPovDirections[u32Pov] = VeJoyStick::POV_CENTERED;
			break;
		}
	}
	g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_POV0_CHANGED + u32Pov, g_pTime->GetTimeUInt(), kDevice.m_kPovDirections[u32Pov]);
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
void VeJoyStick::Init()
{
#	ifdef VE_DIRECT_INPUT
	VE_ASSERT_EQ(g_pInputManager->m_pkDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, VeEnumJoyStickCallback, g_pInputManager->m_pkDirectInput, DIEDFL_ATTACHEDONLY), S_OK);
#	endif
}
//--------------------------------------------------------------------------
void VeJoyStick::Term()
{
	m_kJoyStickArray.Clear();
}
//--------------------------------------------------------------------------
void VeJoyStick::Update()
{
	for(VeVector<Device>::iterator it = m_kJoyStickArray.Begin(); it != m_kJoyStickArray.End(); ++it)
	{
		it->Update();
	}
}
//--------------------------------------------------------------------------
VeVector<VeJoyStick::Device>& VeJoyStick::GetDeviceArray()
{
	return m_kJoyStickArray;
}
//--------------------------------------------------------------------------
VeJoyStick::Device::Device()
#	ifdef VE_DIRECT_INPUT
	: m_pkJoyStick(NULL)
	, m_u32AxisNumber(0)
#	endif
{

}
//--------------------------------------------------------------------------
VeJoyStick::Device::~Device()
{
#	ifdef VE_DIRECT_INPUT
	VE_SAFE_RELEASE(m_pkJoyStick);
#	endif
}
//--------------------------------------------------------------------------
void VeJoyStick::Device::Reset()
{
	for(VeVector<bool>::iterator it = m_kButtons.Begin(); it != m_kButtons.End(); ++it)
	{
		*it = false;
	}
	for(VeVector<VeInt32>::iterator it = m_kAxes.Begin(); it != m_kAxes.End(); ++it)
	{
		*it = 0;
	}
	for(VeVector<PovDirection>::iterator it = m_kPovDirections.Begin(); it != m_kPovDirections.End(); ++it)
	{
		*it = POV_CENTERED;
	}
	for(VeVector<Slider>::iterator it = m_kSliders.Begin(); it != m_kSliders.End(); ++it)
	{
		it->x = 0;
		it->y = 0;
	}
}
//--------------------------------------------------------------------------
void VeJoyStick::Device::Update()
{
#	ifdef VE_DIRECT_INPUT
	DIDEVICEOBJECTDATA akData[VE_JOYSTICK_DX_BUFFERSIZE];
	DWORD dwEntries = VE_JOYSTICK_DX_BUFFERSIZE;
	HRESULT hRes = m_pkJoyStick->Poll();
	if(SUCCEEDED(hRes))
		hRes = m_pkJoyStick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), akData, &dwEntries, 0);
	if(FAILED(hRes))
	{
		hRes = m_pkJoyStick->Acquire();
		while(hRes == DIERR_INPUTLOST)
			hRes = m_pkJoyStick->Acquire();
		m_pkJoyStick->Poll();
		hRes = m_pkJoyStick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), akData, &dwEntries, 0);
		if(FAILED(hRes))
			return;
	}
	for(VeUInt32 i(0); i < dwEntries; ++i)
	{
		if(akData[i].uAppData != 0xFFFFFFFF && akData[i].uAppData > 0)
		{
			VeInt32 i32Axis = (VeInt32)(0x7FFFFFFF & akData[i].uAppData);
			if(i32Axis >= 0 && i32Axis < VeInt32(m_kAxes.Size()))
			{
				m_kAxes[i32Axis] = akData[i].dwData;
				g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_AXIS_MOVED + i32Axis, g_pTime->GetTimeUInt(), akData[i].dwData);
				return;
			}
		}
		
		switch(akData[i].dwOfs)
		{
		case DIJOFS_SLIDER0(0):
			m_kSliders[0].x = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER0_MOVED_X, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_SLIDER0(1):
			m_kSliders[0].y = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER0_MOVED_Y, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;

		case DIJOFS_SLIDER1(0):
			m_kSliders[1].x = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER1_MOVED_X, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_SLIDER1(1):
			m_kSliders[1].y = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER1_MOVED_Y, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_SLIDER2(0):
			m_kSliders[2].x = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER2_MOVED_X, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_SLIDER2(1):
			m_kSliders[2].y = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER2_MOVED_Y, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_SLIDER3(0):
			m_kSliders[3].x = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER3_MOVED_X, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_SLIDER3(1):
			m_kSliders[3].y = akData[i].dwData;
			g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_SLIDER3_MOVED_Y, g_pTime->GetTimeUInt(), akData[i].dwData);
			break;
		case DIJOFS_POV(0):
			VeChangePov(*this, 0, akData[i]);
			break;
		case DIJOFS_POV(1):
			VeChangePov(*this, 1, akData[i]);
			break;
		case DIJOFS_POV(2):
			VeChangePov(*this, 2, akData[i]);
			break;
		case DIJOFS_POV(3):
			VeChangePov(*this, 3, akData[i]);
			break;
		default:
			if(akData[i].dwOfs >= DIJOFS_BUTTON(0) && akData[i].dwOfs < DIJOFS_BUTTON(m_kButtons.Size()))
			{
				VeUInt32 u32Index = akData[i].dwOfs - DIJOFS_BUTTON(0);
				if(akData[i].dwData & 0x80)
				{
					m_kButtons[u32Index] = true;
					g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_BTN_PRESSED, g_pTime->GetTimeUInt(), u32Index);
				}
				else
				{
					m_kButtons[u32Index] = false;
					g_pInputManager->AppendMessage(VeInputMessage::DT_JOYSTICK, VeInputMessage::JIT_BTN_RELEASED, g_pTime->GetTimeUInt(), u32Index);
				}
			}
			break;
		}
	}
#	endif
}
//--------------------------------------------------------------------------
