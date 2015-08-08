////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMain.cpp
//  Version:     v1.00
//  Created:     13/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"

//--------------------------------------------------------------------------
void VeInit()
{
	VeGlobalStringTableA::Create();
	VE_NEW VeAllocatorManager();
	VE_NEW VeStringParser();
	VE_NEW VeTime();
	VeLogger::SetLog(VeLogger::OUTPUT_DEBUGOUTPUT);
	VE_NEW VeResourceManager();
	VE_NEW VeURL();
	VE_NEW VeStartConfig(g_kParams.m_pcConfigName,
		g_kParams.m_pcConfigCustomDir, g_kParams.m_pcConfigDefaultDir);
	VE_NEW VeInputManager();
	VeRenderer::CreateRenderer(g_pStartConfig->GetAPI());
	VE_NEW VeMaterialManager();
	VE_NEW VeSceneManager();
}
//--------------------------------------------------------------------------
void VeTerm()
{
	VeApplication::Destory();
	VeSceneManager::Destory();
	VeMaterialManager::Destory();
	VeRenderer::DestoryRenderer();
	VeInputManager::Destory();
	VeStartConfig::Destory();
	VeURL::Destory();
	VeResourceManager::Destory();
	VeLogger::SetLog(VeLogger::OUTPUT_MAX);
	VeTime::Destory();
	VeStringParser::Destory();
	VeAllocatorManager::Destory();
	VeGlobalStringTableA::Destory();
#ifdef VE_MEM_DEBUG
	_VeMemoryExit();
#endif
}
//--------------------------------------------------------------------------
#ifdef VE_PLATFORM_PC
//--------------------------------------------------------------------------
void VeSetWindowText(const VeChar8* pcText)
{
#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_MOBILE_SIM)
	if(g_hWindow)
	{
		SetWindowTextA(g_hWindow, pcText);
	}
#elif defined(VE_PLATFORM_LINUX)
	if(g_pkDisplay && g_hWindow)
	{
		XTextProperty kTitle;
		XStringListToTextProperty((VeChar8**)&pcText, 1, &kTitle);
		XSetWMName(g_pkDisplay, g_hWindow, &kTitle);
		XFree(kTitle.value);
	}
#else
    VeDebugOutput("SetWindowText(%s)", pcText);
#endif
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
bool g_bMaximized = false;
bool g_bMinimized = false;
bool g_bFullscreen = false;
bool g_bActive = true;
bool g_bFullscreenInactive = false;
bool g_bHasShowWindow = false;
//--------------------------------------------------------------------------
#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_MOBILE_SIM)
//--------------------------------------------------------------------------
HWND g_hWindow(NULL);
HINSTANCE g_hInstance = (NULL);
//--------------------------------------------------------------------------
LRESULT CALLBACK VeWndProc(HWND hWnd, UINT uMessage, WPARAM wParam,
	LPARAM lParam)
{
	switch(uMessage)
	{
#ifdef VE_PLATFORM_WIN
	case WM_SIZE:
		switch(wParam)
		{
		case SIZE_MAXIMIZED:
			if(g_bHasShowWindow)
			{
				g_bMaximized = true;
				g_pStartConfig->SetShowMaximized(true);
				g_pRenderer->ResizeBuffer(LOWORD(lParam), HIWORD(lParam));
			}
			break;
		case SIZE_RESTORED:
			if(g_bHasShowWindow && (!g_bFullscreen))
			{
				g_bMaximized = false;
				g_bMinimized = false;
				g_pStartConfig->SetShowMaximized(false);
				g_pRenderer->ResizeBuffer(LOWORD(lParam), HIWORD(lParam));
			}
			break;
		case SIZE_MINIMIZED:
			g_bMinimized = true;
			g_bMaximized = false;
			break;
		default:
			return DefWindowProc(hWnd, uMessage, wParam, lParam);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		g_pMouse->OnButtonDown(VeMouse::BTN_BUTTON0, lParam);
		break;
	case WM_RBUTTONDOWN:
		g_pMouse->OnButtonDown(VeMouse::BTN_BUTTON1, lParam);
		break;
	case WM_MBUTTONDOWN:
		g_pMouse->OnButtonDown(VeMouse::BTN_BUTTON2, lParam);
		break;
	case WM_LBUTTONUP:
		g_pMouse->OnButtonUp(VeMouse::BTN_BUTTON0, lParam);
		break;
	case WM_RBUTTONUP:
		g_pMouse->OnButtonUp(VeMouse::BTN_BUTTON1, lParam);
		break;
	case WM_MBUTTONUP:
		g_pMouse->OnButtonUp(VeMouse::BTN_BUTTON2, lParam);
		break;
	case WM_MOUSEMOVE:
		if(!g_pMouse->GetStateFlag(VeMouse::CUR_LOCK))
		{
			g_pMouse->OnCursorMove(lParam);
		}
		break;
#endif
	case WM_KEYDOWN:
		g_pKeyboard->OnKeyDown(VeKeyboard::TranslateKey(wParam));
		break;
	case WM_KEYUP:
		g_pKeyboard->OnKeyUp(VeKeyboard::TranslateKey(wParam));
		break;
	case WM_ACTIVATE:
		if(LOWORD(wParam) != WA_INACTIVE)
		{
			g_bActive = true;
		}
		else if(LOWORD(wParam) == WA_INACTIVE)
		{
			g_bActive = false;
		}
		break;
	case WM_DESTROY:
		g_pApplication->QuitApp();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}

	return 0;
}
//--------------------------------------------------------------------------
#elif defined(VE_PLATFORM_LINUX)
//--------------------------------------------------------------------------
Display* g_pkDisplay = NULL;
XSetWindowAttributes g_kWindowAttr;
Window g_hWindow = 0;
//--------------------------------------------------------------------------
void VeWndProc(XEvent& kEvent)
{
	switch(kEvent.type)
	{
	case ClientMessage:
		VeDebugOutputString("ClientMessage");
		break;
	case ConfigureNotify:
		if(g_bHasShowWindow && (!g_bFullscreen))
		{
			XWindowAttributes kWindowAttrib;
			XGetWindowAttributes(g_pkDisplay, g_hWindow, &kWindowAttrib);
			g_bMinimized = false;
			g_pRenderer->ResizeBuffer(kWindowAttrib.width, kWindowAttrib.height);
		}
		break;
	case DestroyNotify:
		g_pApplication->QuitApp();
		break;
	case VisibilityNotify:
		switch(kEvent.xvisibility.state)
		{
		case VisibilityUnobscured:
			g_bActive = true;
			break;
		case VisibilityPartiallyObscured:
			g_bActive = true;
			break;
		case VisibilityFullyObscured:
			g_bActive = false;
			break;
		default:
			break;
		}
		break;
	case FocusIn:
		g_bActive = true;
		break;
	case FocusOut:
		g_bActive = false;
		break;
	case MapNotify:
		g_bMinimized = false;
		break;
	case UnmapNotify:
		g_bMinimized = true;
		break;
	case KeyPress:
		{
			KeySym eKey;
			kEvent.xkey.state &= ~ShiftMask;
			kEvent.xkey.state &= ~LockMask;
			XLookupString(&kEvent.xkey, 0, 0, &eKey, 0);
			g_pKeyboard->OnKeyDown(VeKeyboard::TranslateKey(eKey));
		}
		break;
	case KeyRelease:
		{
			KeySym eKey;
			kEvent.xkey.state &= ~ShiftMask;
			kEvent.xkey.state &= ~LockMask;
			XLookupString(&kEvent.xkey, 0, 0, &eKey, 0);
			g_pKeyboard->OnKeyUp(VeKeyboard::TranslateKey(eKey));
		}
		break;
	case ButtonPress:
		if(kEvent.xbutton.button < 4)
		{
			VeInt32 x, y;
			VeInt32 i32Tmp;
			VeUInt32 u32Tmp2;
			Window hFromRoot, hTmpWin;
			XQueryPointer(g_pkDisplay, g_hWindow, &hFromRoot, &hTmpWin, &i32Tmp, &i32Tmp, &x, &y, &u32Tmp2);
			VeSizeT stData = x | (y << 16);
			switch(kEvent.xbutton.button)
			{
			case 1:
				g_pMouse->OnButtonDown(VeMouse::BTN_BUTTON0, stData);
				break;
			case 3:
				g_pMouse->OnButtonDown(VeMouse::BTN_BUTTON1, stData);
				break;
			case 2:
				g_pMouse->OnButtonDown(VeMouse::BTN_BUTTON2, stData);
				break;
			default:
				break;
			}
		}
		break;
	case ButtonRelease:
		if(kEvent.xbutton.button < 4)
		{
			VeInt32 x, y;
			VeInt32 i32Tmp;
			VeUInt32 u32Tmp2;
			Window hFromRoot, hTmpWin;
			XQueryPointer(g_pkDisplay, g_hWindow, &hFromRoot, &hTmpWin, &i32Tmp, &i32Tmp, &x, &y, &u32Tmp2);
			VeSizeT stData = x | (y << 16);
			switch(kEvent.xbutton.button)
			{
			case 1:
				g_pMouse->OnButtonUp(VeMouse::BTN_BUTTON0, stData);
				break;
			case 3:
				g_pMouse->OnButtonUp(VeMouse::BTN_BUTTON1, stData);
				break;
			case 2:
				g_pMouse->OnButtonUp(VeMouse::BTN_BUTTON2, stData);
				break;
			default:
				break;
			}
		}
		break;
	case MotionNotify:
		g_pMouse->OnCursorMove(kEvent.xmotion.x | (kEvent.xmotion.y << 16));
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
VeStartParams g_kParams;
//--------------------------------------------------------------------------
#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_LINUX) || defined(VE_PLATFORM_MOBILE_SIM)
//--------------------------------------------------------------------------
VeInt32 VeStartEngine(VeStartParams& kParams)
{
	VeInt32 i32Res(0);
	g_kParams = kParams;
	VeInit();

#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_MOBILE_SIM)

	g_hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = VeWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = kParams.m_ptcApplicationName;
	wcex.hIconSm = 0;
	if(!RegisterClassEx(&wcex))
		return E_FAIL;

#	ifdef VE_PLATFORM_WIN
	
	DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
	VeStringParser::WinStyle eStyle = g_pStartConfig->GetConfig("WinStyle", VeStringParser::STYLE_FIXED);
	switch(eStyle)
	{
	case VeStringParser::STYLE_FIXED:
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		break;
	case VeStringParser::STYLE_VARIBLE:
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
		break;
	case VeStringParser::STYLE_POPUP:
		dwStyle |= WS_POPUP;
		break;
	}

#	else

	DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

#	endif

	VeStringParser::Resolution kResolution = g_pStartConfig->GetConfig("Resolution", kResolution);

	RECT kRect = { 0, 0, kResolution.m_u32Width, kResolution.m_u32Height };
	AdjustWindowRect(&kRect, dwStyle, FALSE);

	VeInt32 i32Left(CW_USEDEFAULT), i32Top(CW_USEDEFAULT);

#	ifdef VE_PLATFORM_WIN

	VeStringParser::WinDock eDock = g_pStartConfig->GetConfig("WinDock", VeStringParser::DOCK_DEFAULT);
	switch(eDock)
	{
	case VeStringParser::DOCK_DEFAULT:
		break;
	case VeStringParser::DOCK_CORNER:
		i32Left = 0;
		i32Top = 0;
		break;
	case VeStringParser::DOCK_CENTER:
		{
			VeInt32 i32ScreenW = GetSystemMetrics(SM_CXSCREEN);
			VeInt32 i32ScreenH = GetSystemMetrics(SM_CYSCREEN);
			i32Left = ((i32ScreenW - kResolution.m_u32Width) >> 1) + kRect.left;
			i32Top = ((i32ScreenH - kResolution.m_u32Height) >> 1) + kRect.top;
		}
		break;
	}

#	else

	VeInt32 i32ScreenW = GetSystemMetrics(SM_CXSCREEN);
	VeInt32 i32ScreenH = GetSystemMetrics(SM_CYSCREEN);
	i32Left = ((i32ScreenW - kResolution.m_u32Width) >> 1) + kRect.left;
	i32Top = ((i32ScreenH - kResolution.m_u32Height) >> 1) + kRect.top;

#	endif

	g_hWindow = CreateWindow(kParams.m_ptcApplicationName, kParams.m_ptcApplicationName, dwStyle,
		i32Left, i32Top, kRect.right - kRect.left, kRect.bottom - kRect.top, NULL, NULL, g_hInstance,
		NULL);
	VE_ASSERT(g_hWindow);

	if(g_pStartConfig->GetShowMaximized())
	{
		g_bMaximized = true;
		ShowWindow(g_hWindow, SW_SHOWMAXIMIZED);
	}
	else
	{
		g_bMaximized = false;
		ShowWindow(g_hWindow, SW_SHOWNORMAL);
	}
	g_bHasShowWindow = true;
	UpdateWindow(g_hWindow);

	g_pRenderer->Init();
	g_pInputManager->Init();
	VE_ASSERT(g_pApplication);
	g_pApplication->Init();
	
	MSG kMsg = {0};
	while((WM_QUIT != kMsg.message) && !(g_pApplication->IsAppQuit()))
	{
		if(PeekMessage(&kMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&kMsg);
			DispatchMessage(&kMsg);
		}
		else
		{
			g_pApplication->Update();
			g_pApplication->Render();
		}
	}

	g_pApplication->Term();
	g_pInputManager->Term();
	g_pRenderer->Term();

	DestroyWindow(g_hWindow);
	g_hWindow = NULL;

#elif defined(VE_PLATFORM_LINUX)

	g_pRenderer->Init();
	g_pInputManager->Init();
	VE_ASSERT(g_pApplication);
	g_pApplication->Init();

	XEvent kEvent;

	while(!(g_pApplication->IsAppQuit()))
	{
		while(XCheckWindowEvent(g_pkDisplay, g_hWindow
				, StructureNotifyMask | VisibilityChangeMask | FocusChangeMask
				| KeyPressMask | KeyReleaseMask
				| ButtonPressMask | ButtonReleaseMask | PointerMotionMask
				, &kEvent))
		{
			VeWndProc(kEvent);
		}

		while(XCheckTypedWindowEvent(g_pkDisplay, g_hWindow, ClientMessage, &kEvent))
		{
			VeWndProc(kEvent);
		}

		g_pApplication->Update();
		g_pApplication->Render();
	}

	g_pApplication->Term();
	g_pInputManager->Term();
	g_pRenderer->Term();
	XCloseDisplay(g_pkDisplay);

#endif

	VeTerm();

	return i32Res;
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
#if defined(VE_PLATFORM_ANDROID)
//--------------------------------------------------------------------------
static bool s_bSetuped = false;
static bool s_bChangeSurface(false);
//static VeUInt32 s_u32Width(0), s_u32Height(0);

android_app* g_pkAppState = NULL;

JNIEnv* VeAndroidGetJNIEnv()
{
	return g_pkAppState->activity->env;
}

AAssetManager* VeAndroidGetAssetManager()
{
	return g_pkAppState->activity->assetManager;
}

static void VeAndroidHandleCmd(android_app* pkAppState, int32_t i32Command)
{
	switch(i32Command)
	{
	case APP_CMD_INIT_WINDOW:
		if(pkAppState->window)
		{
			g_pRenderer->Init();

			//VeIRenderer::SetContextAlive(true);
			//g_pRenderer->RestoreData();
			if(!s_bSetuped)
			{
				VE_ASSERT(g_pApplication);
				g_pApplication->Init();
				//g_pRenderer->QuerySurfaceSize(s_u32Width, s_u32Height);
				//g_pRenderer->OnSurfaceChanged(s_u32Width, s_u32Height);
				s_bSetuped = true;
			}
		}
		break;
	case APP_CMD_TERM_WINDOW:
		//g_pRenderer->ReleaseData();
		//VeIRenderer::SetContextAlive(false);
		g_pRenderer->Term();
		break;
	case APP_CMD_GAINED_FOCUS:
		break;
	case APP_CMD_LOST_FOCUS:
		break;
	case APP_CMD_CONFIG_CHANGED:
		s_bChangeSurface = true;
		break;
	case APP_CMD_LOW_MEMORY:
		break;
	}
}

void VeOnTouchBegan(VeInt32 id, VeFloat32 x, VeFloat32 y)
{
	//VeUInt32 u32Time = g_pTime->GetTimeUInt();
	//VeCInputManager::TouchPointParam* pkParam = (VeCInputManager::TouchPointParam*) g_pInputManager->CreateVector4();
	//pkParam->x = x;
	//pkParam->y = y;
	//pkParam->id = id;
	//g_pInputManager->AppendMessage(VeCInputManager::DT_TOUCH, VeCInputManager::TIT_BEGAN, u32Time, pkParam);
}

void VeOnTouchEnded(VeInt32 id, VeFloat32 x, VeFloat32 y)
{
	//VeUInt32 u32Time = g_pTime->GetTimeUInt();
	//VeCInputManager::TouchPointParam* pkParam = (VeCInputManager::TouchPointParam*) g_pInputManager->CreateVector4();
	//pkParam->x = x;
	//pkParam->y = y;
	//pkParam->id = id;
	//g_pInputManager->AppendMessage(VeCInputManager::DT_TOUCH, VeCInputManager::TIT_ENDED, u32Time, pkParam);
}

//void VeOnTouchMoved(VeVector<VeCInputManager::TouchPointParam>* pkTouchPointList)
//{
	//VeUInt32 u32Time = g_pTime->GetTimeUInt();
	//g_pInputManager->AppendMessage(VeCInputManager::DT_TOUCH, VeCInputManager::TIT_MOVED, u32Time, pkTouchPointList);
//}

//void VeOnTouchCancelled(VeVector<VeCInputManager::TouchPointParam>* pkTouchPointList)
//{
	//VeUInt32 u32Time = g_pTime->GetTimeUInt();
	//g_pInputManager->AppendMessage(VeCInputManager::DT_TOUCH, VeCInputManager::TIT_CANCELLED, u32Time, pkTouchPointList);
//}

static int32_t VeAndroidHandleInput(android_app* pkAppState, AInputEvent* pkEvent)
{
	if(AInputEvent_getType(pkEvent) != AINPUT_EVENT_TYPE_MOTION) return 0;

	VeInt32 i32Action = AMotionEvent_getAction(pkEvent);

#ifdef VE_MULTITOUCH

	if(i32Action == AMOTION_EVENT_ACTION_DOWN)
	{
		VeOnTouchBegan(AMotionEvent_getPointerId(pkEvent, 0), AMotionEvent_getX(pkEvent, 0), AMotionEvent_getY(pkEvent, 0));
		return VE_TRUE;
	}
	else if((i32Action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_POINTER_DOWN)
	{
		VeInt32 i32Index = (i32Action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		VeOnTouchBegan(AMotionEvent_getPointerId(pkEvent, i32Index), AMotionEvent_getX(pkEvent, i32Index), AMotionEvent_getY(pkEvent, i32Index));
		return VE_TRUE;
	}
	else if(i32Action == AMOTION_EVENT_ACTION_UP)
	{
		VeOnTouchEnded(AMotionEvent_getPointerId(pkEvent, 0), AMotionEvent_getX(pkEvent, 0), AMotionEvent_getY(pkEvent, 0));
		return VE_TRUE;
	}
	else if((i32Action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_POINTER_UP)
	{
		VeInt32 i32Index = (i32Action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		VeOnTouchEnded(AMotionEvent_getPointerId(pkEvent, i32Index), AMotionEvent_getX(pkEvent, i32Index), AMotionEvent_getY(pkEvent, i32Index));
		return VE_TRUE;
	}
	else if(i32Action == AMOTION_EVENT_ACTION_MOVE)
	{
		VeSizeT stPointerCount = AMotionEvent_getPointerCount(pkEvent);
		VeTVector<VeCInputManager::TouchPointParam>* pkParams = (VeTVector<VeCInputManager::TouchPointParam>*)(g_pInputManager->CreateTouchPointList());
		(*pkParams).Resize(stPointerCount);
		for(VeSizeT i(0); i < stPointerCount; ++i)
		{
			VeCInputManager::TouchPointParam& kParam = (*pkParams)[i];
			kParam.id = AMotionEvent_getPointerId(pkEvent, i);
			kParam.x = AMotionEvent_getX(pkEvent, i);
			kParam.y = AMotionEvent_getY(pkEvent, i);
		}
		VeOnTouchMoved(pkParams);
		return VE_TRUE;
	}
	else if(i32Action == AMOTION_EVENT_ACTION_CANCEL)
	{
		VeSizeT stPointerCount = AMotionEvent_getPointerCount(pkEvent);
		VeTVector<VeCInputManager::TouchPointParam>* pkParams = (VeTVector<VeCInputManager::TouchPointParam>*)(g_pInputManager->CreateTouchPointList());
		(*pkParams).Resize(stPointerCount);
		for(VeSizeT i(0); i < stPointerCount; ++i)
		{
			VeCInputManager::TouchPointParam& kParam = (*pkParams)[i];
			kParam.id = AMotionEvent_getPointerId(pkEvent, i);
			kParam.x = AMotionEvent_getX(pkEvent, i);
			kParam.y = AMotionEvent_getY(pkEvent, i);
		}
		VeOnTouchCancelled(pkParams);
		return VE_TRUE;
	}

#else

	switch(i32Action)
	{
	case AMOTION_EVENT_ACTION_DOWN:
		VeOnTouchBegan(AMotionEvent_getPointerId(pkEvent, 0), AMotionEvent_getX(pkEvent, 0), AMotionEvent_getY(pkEvent, 0));
		return VE_TRUE;
	case AMOTION_EVENT_ACTION_UP:
		VeOnTouchEnded(AMotionEvent_getPointerId(pkEvent, 0), AMotionEvent_getX(pkEvent, 0), AMotionEvent_getY(pkEvent, 0));
		return VE_TRUE;
	case AMOTION_EVENT_ACTION_MOVE:
		/*{
			VeTVector<VeCInputManager::TouchPointParam>* pkParams = (VeTVector<VeCInputManager::TouchPointParam>*)(g_pInputManager->CreateVector4Array());
			(*pkParams).Resize(1);
			VeCInputManager::TouchPointParam& kParam = (*pkParams)[0];
			kParam.id = AMotionEvent_getPointerId(pkEvent, 0);
			kParam.x = AMotionEvent_getX(pkEvent, 0);
			kParam.y = AMotionEvent_getY(pkEvent, 0);
			VeOnTouchMoved(pkParams);
		}*/
		return VE_TRUE;
	case AMOTION_EVENT_ACTION_CANCEL:
		/*{
			VeTVector<VeCInputManager::TouchPointParam>* pkParams = (VeTVector<VeCInputManager::TouchPointParam>*)(g_pInputManager->CreateVector4Array());
			(*pkParams).Resize(1);
			VeCInputManager::TouchPointParam& kParam = (*pkParams)[0];
			kParam.id = AMotionEvent_getPointerId(pkEvent, 0);
			kParam.x = AMotionEvent_getX(pkEvent, 0);
			kParam.y = AMotionEvent_getY(pkEvent, 0);
			VeOnTouchCancelled(pkParams);
		}*/
		return VE_TRUE;
	default:
		break;
	}

#endif

	return VE_FALSE;
}

void VeStartEngine(VeStartParams& kParams)
{
	app_dummy();
	kParams.m_pkAppState->userData = NULL;
	kParams.m_pkAppState->onAppCmd = VeAndroidHandleCmd;
	kParams.m_pkAppState->onInputEvent = VeAndroidHandleInput;
	SetAssetManager(kParams.m_pkAppState->activity->assetManager);
	g_pkAppState = kParams.m_pkAppState;
	g_kParams = kParams;
	s_bChangeSurface = false;

	VeInit();

	while(true)
	{
		int iEvents;
		android_poll_source* pkSource;

		while(ALooper_pollAll(g_pRenderer->IsInited() ? 0 : -1, NULL, &iEvents, (void**)&pkSource) >= 0)
		{
			if(pkSource)
			{
				pkSource->process(g_pkAppState, pkSource);
			}

			if(g_pkAppState->destroyRequested)
			{
				g_pApplication->Term();
				g_pRenderer->Term();
				VeTerm();
				s_bSetuped = false;
				return;
			}
		}

		/*if(s_bChangeSurface)
		{
			VeUInt32 u32Width, u32Height;
			g_pRenderer->QuerySurfaceSize(u32Width, u32Height);
			if((u32Width != s_u32Width) || (u32Height != s_u32Height))
			{
				s_u32Width = u32Width;
				s_u32Height = u32Height;
				g_pRenderer->OnSurfaceChanged(s_u32Width, s_u32Height);
			}
		}*/

		if(g_pRenderer->IsInited())
		{
			g_pApplication->Update();
			g_pApplication->Render();
		}
	}
}

#endif
