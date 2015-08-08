#include "AppDelegate.h"
#include "TexasPoker.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#include <dirent.h>
#include <sys/types.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

extern "C"
{

void seekdir(DIR* drip, long loc)
{

}

long telldir(DIR* dir)
{
	return 0;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JniHelper::setJavaVM(vm);

    return JNI_VERSION_1_4;
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    if (!CCDirector::sharedDirector()->getOpenGLView())
    {
        CCEGLView *view = CCEGLView::sharedOpenGLView();
        view->setFrameSize(w, h);

        /*AppDelegate *pAppDelegate = */new AppDelegate();
        CCApplication::sharedApplication()->run();
    }
    else
    {
        ccDrawInit();
        ccGLInvalidateStateCache();

        CCShaderCache::sharedShaderCache()->reloadDefaultShaders();
        CCTextureCache::reloadAllTextures();
        CCNotificationCenter::sharedNotificationCenter()->postNotification(EVNET_COME_TO_FOREGROUND, NULL);
        CCDirector::sharedDirector()->setGLDefaultValues();
    }
}

void Java_cn_VenusIE_TexasPoker_TexasPokerActivity_OnPurchaseFeed(JNIEnv* env, jclass cls, jstring strChannel, jstring strOrderId, jstring strPayCode, jstring strOrderInfo)
{
	VeString kValue("?uuid=");
	kValue += g_pkGame->GetDeviceID();
	if(strChannel!=NULL)
	{
		kValue += "&qudao=";
		kValue += TexasPoker::jstringTostring2(env,strChannel);
	}
	if(strOrderId!=NULL)
	{
		kValue += "&orderid=";
		kValue += TexasPoker::jstringTostring2(env,strOrderId);
	}
	if(strPayCode!=NULL)
	{
		kValue += "&paycode=";
		kValue += TexasPoker::jstringTostring2(env,strPayCode);
	}
	if(strOrderInfo!=NULL)
	{
		kValue += "&orderinfo=";
		kValue += TexasPoker::jstringTostring2(env,strOrderInfo);
	}
	g_pkGame->ShowLoading();
	g_pkGame->VerifyPurchase(kValue);
}

void Java_cn_VenusIE_TexasPoker_TexasPokerActivity_OnWallClose(JNIEnv* env, jclass cls)
{
	g_pkGame->NeedStopLoading();
}

}

void GetPurchaseList(TexasPoker::PurchaseInfoList& kList)
{
	kList.Resize(5);

	kList[0].m_kInfo = TT("MM_BuyPackage0");
	kList[0].m_u32Price = 200;
	kList[0].m_eType = PURCHASE_TYPE_RMB;
	kList[0].m_u32Chips = 10000;
	kList[0].m_u32Icon = 0;
	kList[0].m_bVip = false;

	kList[1].m_kInfo = TT("MM_BuyPackage1");
	kList[1].m_u32Price = 500;
	kList[1].m_eType = PURCHASE_TYPE_RMB;
	kList[1].m_u32Chips = 30000;
	kList[1].m_u32Icon = 1;
	kList[1].m_bVip = false;

	kList[2].m_kInfo = TT("MM_BuyPackage2");
	kList[2].m_u32Price = 1000;
	kList[2].m_eType = PURCHASE_TYPE_RMB;
	kList[2].m_u32Chips = 70000;
	kList[2].m_u32Icon = 2;
	kList[2].m_bVip = true;

	kList[3].m_kInfo = TT("MM_BuyPackage3");
	kList[3].m_u32Price = 2000;
	kList[3].m_eType = PURCHASE_TYPE_RMB;
	kList[3].m_u32Chips = 160000;
	kList[3].m_u32Icon = 3;
	kList[3].m_bVip = true;

	kList[4].m_kInfo = TT("MM_BuyPackage4");
	kList[4].m_u32Price = 3000;
	kList[4].m_eType = PURCHASE_TYPE_RMB;
	kList[4].m_u32Chips = 270000;
	kList[4].m_u32Icon = 4;
	kList[4].m_bVip = true;
}

const VeChar8* GetChannel()
{
	return "MM_LIAN_YUAN";
}
