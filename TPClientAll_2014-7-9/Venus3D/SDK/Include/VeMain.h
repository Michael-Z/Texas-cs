////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeMain.h
//  Version:     v1.00
//  Created:     13/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#if (defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_MOBILE_SIM)) && defined(VE_DYNAMIC_LIB)
#	ifdef VE_MAIN_EXPORT
#		define VE_MAIN_API __declspec (dllexport)
#	else
#		define VE_MAIN_API __declspec (dllimport)
#	endif
#else
#	define VE_MAIN_API
#endif

#ifndef VE_NO_INLINE
#	define VE_MAIN_INLINE inline
#else
#	define VE_MAIN_INLINE VE_MAIN_API
#endif

struct VeStartParams
{
	const VeChar8* m_pcConfigName;
	const VeChar8* m_pcConfigCustomDir;
	const VeChar8* m_pcConfigDefaultDir;
#if defined(VE_PLATFORM_PC) || defined(VE_PLATFORM_MOBILE_SIM)
	const VeTChar* m_ptcApplicationName;
#	if defined(VE_PLATFORM_LINUX) || defined(VE_PLATFORM_OSX)
	int m_iArgc;
	char** m_ppcArgv;
#	endif
#elif defined(VE_PLATFORM_IOS)
    int m_iArgc;
	char** m_ppcArgv;
#elif defined(VE_PLATFORM_ANDROID)
	android_app* m_pkAppState;
#endif
};

#ifndef VE_USER
	VE_MAIN_API extern VeStartParams g_kParams;
	VE_MAIN_API extern bool g_bMaximized;
	VE_MAIN_API extern bool g_bMinimized;
	VE_MAIN_API extern bool g_bFullscreen;
	VE_MAIN_API extern bool g_bActive;
	VE_MAIN_API extern bool g_bFullscreenInactive;
	VE_MAIN_API extern bool g_bHasShowWindow;
#endif

VE_MAIN_API void VeInit();

VE_MAIN_API void VeTerm();

#ifdef VE_PLATFORM_PC

VE_MAIN_API void VeSetWindowText(const VeChar8* pcText);

#endif

#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_LINUX) || defined(VE_PLATFORM_OSX) || defined(VE_PLATFORM_MOBILE_SIM) || defined(VE_PLATFORM_IOS)
	VE_MAIN_API VeInt32 VeStartEngine(VeStartParams& kParams);
#elif defined(VE_PLATFORM_ANDROID)
	VE_MAIN_API void VeStartEngine(VeStartParams& kParams);
#endif

#ifdef VE_PLATFORM_IOS

#define VE_START_ENGINE(delegate)                                                                                   \
    VeInt32 VeStartEngine(VeStartParams& kParams)                                                                   \
    {                                                                                                               \
        @autoreleasepool                                                                                            \
        {                                                                                                           \
            return UIApplicationMain(kParams.m_iArgc, kParams.m_ppcArgv, nil, NSStringFromClass([delegate class])); \
        }                                                                                                           \
    }

#endif

#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_MOBILE_SIM)
#	ifdef VE_UNICODE
#		define VE_MAIN_FUNCTION int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
#	else
#		define VE_MAIN_FUNCTION int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#	endif
#elif defined(VE_PLATFORM_LINUX) || defined(VE_PLATFORM_OSX) || defined(VE_PLATFORM_IOS)
#	define VE_MAIN_FUNCTION int main(int iArgc, char* apcArgv[])
#elif defined(VE_PLATFORM_ANDROID)
#	define VE_MAIN_FUNCTION void android_main(android_app* pkAppState)
#endif

#if defined(VE_PLATFORM_WIN) || defined(VE_PLATFORM_MOBILE_SIM)

#define VE_FILL_START_PARAMS(params, name)							\
	params.m_pcConfigName = "Config.xml";							\
	params.m_pcConfigCustomDir = "file#.";							\
	params.m_pcConfigDefaultDir = "file#.";							\
	params.m_ptcApplicationName = name;

#	ifndef VE_USER
	VE_MAIN_API extern HWND g_hWindow;
	VE_MAIN_API extern HINSTANCE g_hInstance;
#	endif

#elif defined(VE_PLATFORM_LINUX)

#define VE_FILL_START_PARAMS(params, name)							\
	params.m_pcConfigName = "Config.xml";							\
	params.m_pcConfigCustomDir = "file#.";							\
	params.m_pcConfigDefaultDir = "file#.";							\
	params.m_ptcApplicationName = name;								\
	params.m_iArgc = iArgc;											\
	params.m_ppcArgv = apcArgv

#	ifndef VE_USER
	extern Display* g_pkDisplay;
	extern XSetWindowAttributes g_kWindowAttr;
	extern Window g_hWindow;
#	endif

#elif defined(VE_PLATFORM_OSX)

#define VE_FILL_START_PARAMS(params, name)							\
    params.m_pcConfigName = "Config.xml";							\
    params.m_pcConfigCustomDir = "file#.";							\
    params.m_pcConfigDefaultDir = "file#.";							\
	params.m_iArgc = iArgc;											\
	params.m_ppcArgv = apcArgv

#elif defined(VE_PLATFORM_IOS)

#define VE_FILL_START_PARAMS(params, name)							\
    params.m_pcConfigName = "Config.xml";							\
    params.m_pcConfigCustomDir = "file#../Documents";				\
    params.m_pcConfigDefaultDir = "file#.";							\
    params.m_iArgc = iArgc;											\
    params.m_ppcArgv = apcArgv

#elif defined(VE_PLATFORM_ANDROID)

#define VE_FILL_START_PARAMS(params, name)							\
	params.m_pcConfigName = "Config.xml";							\
	params.m_pcConfigCustomDir = NULL;		\
	params.m_pcConfigDefaultDir = "asset#";					\
	params.m_pkAppState = pkAppState;
	
#	ifndef VE_USER
	extern android_app* g_pkAppState;
#	endif

JNIEnv* VeAndroidGetJNIEnv();

AAssetManager* VeAndroidGetAssetManager();

#endif
