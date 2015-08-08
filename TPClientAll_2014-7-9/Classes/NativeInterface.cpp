#include <cocos2d.h>
#include "NativeInterface.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "CocoaHelper.h"
#endif
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <Windows.h>
#include <stdio.h>
#include "Md5.h"
#pragma comment(lib, "Version.lib ")
#endif

USING_NS_CC;

Version NativeGetVersion()
{
	Version kVersion;
#	if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_GetVersionCode",
		"()I"))
	{
		jint iVersionCode = t.env->CallStaticIntMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
		kVersion.m_au8Version[0] = iVersionCode / 1000;
		iVersionCode -= kVersion.m_au8Version[0] * 1000;
		kVersion.m_au8Version[1] = iVersionCode / 100;
		iVersionCode -= kVersion.m_au8Version[1] * 100;
		kVersion.m_au8Version[2] = iVersionCode / 10;
		iVersionCode -= kVersion.m_au8Version[2] * 10;
		kVersion.m_au8Version[3] = iVersionCode;
	}
#	endif
#	if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CocoaHelper::GetAppVersion(kVersion.m_au8Version);
#	endif
#	if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	VeChar8 acFullPath[MAX_PATH]; 
	GetModuleFileNameA(NULL, acFullPath, sizeof(acFullPath));
	DWORD dwVerHnd(NULL);
	DWORD dwVerInfoSize = GetFileVersionInfoSize(acFullPath, &dwVerHnd);
	VE_ASSERT(dwVerInfoSize);
	HANDLE hMem; 
	LPVOID lpvMem; 
	VeUInt32 u32InfoSize(0); 
	hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize); 
	lpvMem = GlobalLock(hMem); 
	GetFileVersionInfo(acFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
	VS_FIXEDFILEINFO* pkFileInfo(NULL);
	VerQueryValue(lpvMem, "\\", (void**)&pkFileInfo, &u32InfoSize);
	VE_ASSERT(pkFileInfo);
	kVersion.m_au8Version[0] = HIWORD(pkFileInfo->dwProductVersionMS);
	kVersion.m_au8Version[1] = LOWORD(pkFileInfo->dwProductVersionMS);
	kVersion.m_au8Version[2] = HIWORD(pkFileInfo->dwProductVersionLS);
	kVersion.m_au8Version[3] = LOWORD(pkFileInfo->dwProductVersionLS);
	GlobalUnlock(hMem);
	GlobalFree(hMem);
#	endif
	VeDebugOutput("Version:%d.%d.%d.%d", kVersion.m_au8Version[0], kVersion.m_au8Version[1], kVersion.m_au8Version[2], kVersion.m_au8Version[3]);
	return kVersion;
}

void NativeOnEvent(const VeChar8* pcEvent)
{
	VeDebugOutput("NativeOnEvent:%s", pcEvent);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_OnEvent",
		"(Ljava/lang/String;)V"))
	{
		jstring strEvent = t.env->NewStringUTF(pcEvent);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, strEvent);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CocoaHelper::OnEvent(pcEvent);
#endif
}

void NativeOnEvent(const VeChar8* pcEvent, const VeVector< VePair<VeStringA,VeStringA> >& kValue)
{
	if(kValue.Empty())
	{
		NativeOnEvent(pcEvent);
		return;
	}
#	ifdef VE_DEBUG
	VeStringA kOutput("NativeOnEvent:");
	kOutput += pcEvent;
	for(VeUInt32 i(0); i < kValue.Size(); ++i)
	{
		const VePair<VeStringA,VeStringA>& kIter = kValue[i];
		kOutput += "(" + kIter.m_tFirst + "," + kIter.m_tSecond + ")";
	}
	VeDebugOutputString(kOutput);
#	endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_OnEventValues",
		"(Ljava/lang/String;[Ljava/lang/String;)V"))
	{
		jstring strEvent = t.env->NewStringUTF(pcEvent);
		VeUInt32 u32Size = kValue.Size();
		jclass strClass = t.env->FindClass("java/lang/String");
		jobjectArray astrValues = t.env->NewObjectArray(u32Size << 1, strClass, 0);
		jstring type,value;
		for(VeUInt32 i(0); i < u32Size; ++i)
		{
			type = t.env->NewStringUTF(kValue[i].m_tFirst);
			value = t.env->NewStringUTF(kValue[i].m_tSecond);
			t.env->SetObjectArrayElement(astrValues, i * 2, type);
			t.env->SetObjectArrayElement(astrValues, i * 2 + 1, value);
		}
		t.env->CallStaticVoidMethod(t.classID, t.methodID, strEvent, astrValues);
		t.env->DeleteLocalRef(t.classID);
		t.env->DeleteLocalRef(astrValues);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CocoaHelper::OnEvent(pcEvent, kValue);
#endif
}

void NativeOnEventBegin(const VeChar8* pcEvent)
{
	VeDebugOutput("NativeOnEventBegin:%s", pcEvent);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_OnEventBegin",
		"(Ljava/lang/String;)V"))
	{
		jstring strEvent = t.env->NewStringUTF(pcEvent);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, strEvent);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CocoaHelper::OnEventBegin(pcEvent);
#endif
}

void NativeOnEventEnd(const VeChar8* pcEvent)
{
	VeDebugOutput("NativeOnEventEnd:%s", pcEvent);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_OnEventEnd",
		"(Ljava/lang/String;)V"))
	{
		jstring strEvent = t.env->NewStringUTF(pcEvent);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, strEvent);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CocoaHelper::OnEventEnd(pcEvent);
#endif
}

void NativeOnPageStart(const VeChar8* pcPage)
{
	VeDebugOutput("NativeOnPageStart:%s", pcPage);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_OnPageStart",
		"(Ljava/lang/String;)V"))
	{
		jstring strEvent = t.env->NewStringUTF(pcPage);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, strEvent);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CocoaHelper::OnPageStart(pcPage);
#endif
}

void NativeOnPageEnd(const VeChar8* pcPage)
{
	VeDebugOutput("NativeOnPageEnd:%s", pcPage);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_OnPageEnd",
		"(Ljava/lang/String;)V"))
	{
		jstring strEvent = t.env->NewStringUTF(pcPage);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, strEvent);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CocoaHelper::OnPageEnd(pcPage);
#endif
}
