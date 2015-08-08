#include "TexasPoker.h"
#include "UILayer.h"
#include "UIButton.h"
#include "SoundSystem.h"
#include "Login.h"
#include "Hall.h"
#include "Game.h"
#include "UIAlert.h"
#include "Knickknacks.h"
#include "UILayer.h"
#include "GetTime.h"
#include "Keno.h"
#include "TexasPokerHallClient.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"

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

TexasPoker* g_pkGame = NULL;

size_t ReceiveData(void *buffer, size_t size, size_t nmemb, void *user_p)
{
	VeString tmp((const VeChar8*)buffer, size*nmemb);
	VeString* res = (VeString*)(user_p);
	*res += tmp;
	return (size*nmemb);
}

class TexasPokerNoticeTask : public VeBackgroundTask
{
public:
	TexasPokerNoticeTask(const VeChar8* pcUrl)
	{
		m_kUrl = pcUrl;
		m_eResult = CURL_LAST;
	}

	virtual void DoBackgroundTask(VeBackgroundTaskQueue& kMgr)
	{
		CURL* pkCurl = curl_easy_init();
		if(pkCurl)
		{
			curl_easy_setopt(pkCurl, CURLOPT_URL, (const VeChar8*)m_kUrl);
			curl_easy_setopt(pkCurl, CURLOPT_WRITEFUNCTION, &ReceiveData);
			curl_easy_setopt(pkCurl, CURLOPT_WRITEDATA, &m_kHttpBody);
			m_eResult = curl_easy_perform(pkCurl);
			curl_easy_cleanup(pkCurl);
		}
		kMgr.AddMainThreadTask(this);
	}

	virtual void DoMainThreadTask(VeBackgroundTaskQueue& kMgr)
	{
		g_pkGame->OnServerNoticeUpdate(m_eResult == CURLE_OK ? m_kHttpBody : "");
	}

protected:
	VeStringA m_kUrl;
	VeStringA m_kHttpBody;
	CURLcode m_eResult;

};

TexasPoker::TexasPoker() : m_pkLogo(NULL), m_u32TexturesInLoading(0)
{
	m_pkManager = NULL;
	m_pcLanguage = MAIN_LAN;

	VeStringA kLan = GetSysLanguage();
	if(kLan == ZH_CN)
	{
		m_eLanguate = zh_CN;
		m_pcLanguage = ZH_CN;
	}
	else if(kLan == ZH_TW)
	{
		m_eLanguate = zh_TW;
		m_pcLanguage = ZH_CN;
	}
	else
	{
		m_eLanguate = en_US;
		m_pcLanguage = EN_US;
	}

	m_pcImagePath = NULL;
	m_f32ScaleHD = 1.0f;
	m_bNeedRestore = false;

	g_pSoundSystem->SetMusicVolume(g_pSaveData->ReadParam("music_volume", 1.0f));
	g_pSoundSystem->SetSoundVolume(g_pSaveData->ReadParam("sound_volume", 1.0f));
	VeUInt32 u32Switch = g_pSaveData->ReadParam("sound_switch", VeUInt32(0));
	g_pSoundSystem->SetSwitch(u32Switch ? false : true);

	m_u64ServerTime = 0;
	m_f64ServerSyncTime = 0;
	m_bNeedStopLoading = false;
}

TexasPoker::~TexasPoker()
{

}

bool TexasPoker::init()
{
	{
		std::string pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename("Res/text/string.xml");
		unsigned char* pbyBuffer = NULL;
		unsigned long ulSize = 0;
		pbyBuffer = CCFileUtils::sharedFileUtils()->getFileData(pathKey.c_str(), "r", &ulSize);
		if(ulSize)
		{
			VeChar8* pcMem = (VeChar8*)VeMalloc(ulSize + 1);
			VeMemcpy(pcMem, pbyBuffer, ulSize);
			pcMem[ulSize] = 0;
			VeXMLDocument kDoc;
			kDoc.Parse(pcMem);
			VeFree(pcMem);
			VE_ASSERT(!kDoc.Error());
			VeXMLElement* pkRoot = kDoc.RootElement();
			if(m_eLanguate == en_US)
			{
				pkRoot = pkRoot->FirstChildElement(EN_US);
			}
			else if(m_eLanguate == zh_CN)
			{
				pkRoot = pkRoot->FirstChildElement(ZH_CN);
			}
			else if(m_eLanguate == zh_TW)
			{
				pkRoot = pkRoot->FirstChildElement(ZH_TW);
			}
			else
			{
				pkRoot = pkRoot->FirstChildElement(MAIN_LAN);
			}
			VE_ASSERT(pkRoot);
			VeXMLElement* pkEntry = pkRoot->FirstChildElement();
			while(pkEntry)
			{
				const VeChar8* pcName = pkEntry->Value();
				const VeChar8* pcValue = pkEntry->GetAttribute("value");
				if(pcName && pcValue)
				{
					m_kStringMap[pcName] = pcValue;
				}
				pkEntry = pkEntry->NextSiblingElement();
			}
		}
		if(pbyBuffer) delete [] pbyBuffer;
	}

	{
		std::string pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename("config.xml");
		unsigned char* pbyBuffer = NULL;
		unsigned long ulSize = 0;
		pbyBuffer = CCFileUtils::sharedFileUtils()->getFileData(pathKey.c_str(), "r", &ulSize);
		if(ulSize)
		{
			VeChar8* pcMem = (VeChar8*)VeMalloc(ulSize + 1);
			VeMemcpy(pcMem, pbyBuffer, ulSize);
			pcMem[ulSize] = 0;
			VeXMLDocument kDoc;
			kDoc.Parse(pcMem);
			VeFree(pcMem);
			VE_ASSERT(!kDoc.Error());
			VeXMLElement* pkTemp(NULL);
			pkTemp = kDoc.RootElement()->FirstChildElement("HttpServer");
			VE_ASSERT(pkTemp);
			m_kHttpServer = pkTemp->GetAttribute("url");
			pkTemp = kDoc.RootElement()->FirstChildElement("HallServer");
			VE_ASSERT(pkTemp);
			m_kHallServerIp = pkTemp->GetAttribute("ip");
			m_u32HallServerPort = pkTemp->Attribute("port", 0);
		}
		if(pbyBuffer) delete [] pbyBuffer;
	}
	CCSize kScreenSize = g_pkDirector->getVisibleSize();
	if(kScreenSize.height <= 568)
	{
		UILayer::SetMainResolution(MAIN_WIDTH, MAIN_HEIGHT, MAX_ASPECT_RATIO, MIN_ASPECT_RATIO);
		m_pcImagePath = IMAGE_PATH;
		m_f32ScaleHD = 1.0f;
	}
	else
	{
		UILayer::SetMainResolution(MAIN_WIDTH_HD, MAIN_HEIGHT_HD, MAX_ASPECT_RATIO, MIN_ASPECT_RATIO);
		m_pcImagePath = IMAGE_PATH_HD;
		m_f32ScaleHD = 2.0f;
	}
	srand((unsigned int)VENet::GetTime());
	m_kVersion = NativeGetVersion();
	return CCScene::init();
}

const VeChar8* TexasPoker::GetString(const VeChar8* pcName)
{
	VeStringMap<VeFixedStringA>::iterator iter = m_kStringMap.Find(pcName);
	return iter ? *iter : "none";
}

const VeChar8* TexasPoker::GetNumber(VeUInt64 u64Number, VeUInt32 u32Show)
{
	static VeChar8 s_acBuffer[1024];
	if(u64Number <= u32Show)
	{
		VeSprintf(s_acBuffer, 1024, "%d", (VeUInt32)u64Number);
	}
	else if(m_eLanguate == zh_CN || m_eLanguate == zh_TW)
	{
		if(u64Number < 10000)
		{
			VeSprintf(s_acBuffer, 1024, "%d", (VeUInt32)u64Number);
		}
		else if(u64Number < 100000000)
		{
			u64Number /= 10000;
			VeSprintf(s_acBuffer, 1024, "%d%s", (VeUInt32)u64Number, TT("W"));
		}
		else if(u64Number < 1000000000000ull)
		{
			u64Number /= 100000000;
			VeSprintf(s_acBuffer, 1024, "%d%s", (VeUInt32)u64Number, TT("Y"));
		}
		else
		{
			u64Number /= 1000000000000ull;
			VeSprintf(s_acBuffer, 1024, "%d%s", (VeUInt32)u64Number, TT("WY"));
		}
	}
	else
	{
		if(u64Number < 1000)
		{
			VeSprintf(s_acBuffer, 1024, "%d", (VeUInt32)u64Number);
		}
		else if(u64Number < 1000000)
		{
			u64Number /= 1000;
			VeSprintf(s_acBuffer, 1024, "%d%s", (VeUInt32)u64Number, "k");
		}
		else if(u64Number < 1000000000)
		{
			u64Number /= 1000000;
			VeSprintf(s_acBuffer, 1024, "%d%s", (VeUInt32)u64Number, "m");
		}
		else
		{
			u64Number /= 1000000000;
			VeSprintf(s_acBuffer, 1024, "%d%s", (VeUInt32)u64Number, "b");
		}
	}

	return s_acBuffer;
}

VeUInt32 GetDigitNum(VeUInt64 u64Number)
{
	VeUInt32 u32Res(0);
	do 
	{
		++u32Res;
		u64Number /= 10;
	}
	while(u64Number);
	return u32Res;
}

const VeChar8* TexasPoker::GetNumberLimit(VeUInt64 u64Number, VeUInt32 u32Limit)
{
	static VeChar8 s_acBuffer[1024];
	VE_ASSERT(u32Limit >= 4);
	VeUInt32 u32DigitNum = GetDigitNum(u64Number);
	if(u32DigitNum <= u32Limit)
	{
		VeSprintf(s_acBuffer, 1024, "%llu", u64Number);
	}
	else if(u32DigitNum <= (u32Limit + 2))
	{
		VeSprintf(s_acBuffer, 1024, "%lluk", u64Number / 1000ull);
	}
	else if(u32DigitNum <= (u32Limit + 5))
	{
		VeSprintf(s_acBuffer, 1024, "%llum", u64Number / 1000000ull);
	}
	else if(u32DigitNum <= (u32Limit + 8))
	{
		VeSprintf(s_acBuffer, 1024, "%llub", u64Number / 1000000000ull);
	}
	else
	{
		VeSprintf(s_acBuffer, 1024, "nan");
	}
	return s_acBuffer;
}

VeFloat32 TexasPoker::GetScaleHD()
{
	return m_f32ScaleHD;
}

cocos2d::CCTexture2D* TexasPoker::GetLoginTexture(VeUInt32 u32Index)
{
	if(u32Index < m_kLoginTextureArray.Size())
	{
		return m_kLoginTextureArray[u32Index];
	}
	else
	{
		return NULL;
	}
}

void TexasPoker::onEnter()
{
	CCScene::onEnter();
	g_pkGame = this;
	LoadShaders();
	m_bNeedRestore = false;
	m_pkManager = VE_NEW UILayerManager;
	addChild(m_pkManager);
	LoadLogin();
	VE_NEW TexasPokerHallClient();
	g_pClient->Start();
}

void TexasPoker::onExit()
{
	while(m_kCurlQueue.HasTask())
	{
		m_kCurlQueue.Update();
		VeSleep(10);
	}
	g_pClient->Stop();
	g_pkGame = NULL;
	VE_SAFE_DELETE(m_pkManager);
	CCScene::onExit();
}

void TexasPoker::onEnterTransitionDidFinish()
{
	CCScene::onEnterTransitionDidFinish();
}

void TexasPoker::Restore()
{
	m_bNeedRestore = true;
}

void TexasPoker::visit()
{
	if(m_bNeedRestore)
	{
		RestoreShaders();
		m_bNeedRestore = false;
	}
	CCScene::visit();
}

VeStringA TexasPoker::LoadImage(const VeChar8* pcName, bool bLanguage)
{
	VeStringA kPath = GetImagePath(pcName, bLanguage);
	m_kLoading.PushBack(kPath + ".plist");
	++m_u32TexturesInLoading;
	CCTextureCache::sharedTextureCache()->addImageAsync(kPath + ".png", this, callfuncO_selector(TexasPoker::LoadingPvrcczCallBack));
	return kPath;
}

VeStringA TexasPoker::GetImagePath(const VeChar8* pcName, bool bLanguage)
{
	VeStringA kPath(RES_ROOT);
	kPath += "/";
	if(bLanguage)
	{
		kPath += m_pcLanguage;
		kPath += "/";
	}
	kPath += m_pcImagePath;
	kPath += "/";
	kPath += pcName;
	return kPath;
}

VeUInt32 TexasPoker::GetNumTexInLoading()
{
	return m_u32TexturesInLoading;
}

void TexasPoker::LoadSoundEffect(SoundEffect eSound, const VeChar8* pcName, bool bLanguage)
{
	VeStringA kPath(RES_ROOT);
	kPath += "/";
	if(bLanguage)
	{
		kPath += m_pcLanguage;
		kPath += "/";
	}
	kPath += "audio/";
	kPath += pcName;
	g_pSoundSystem->LoadEffect(eSound, kPath);
}

void TexasPoker::LoadImages()
{
	LoadImage("ui_2", false);
	LoadImage("drink", false);
}

void TexasPoker::LoadLogin()
{
	VeChar8 acBuffer[VE_MAX_PATH_LEN];
	VeSprintf(acBuffer, VE_MAX_PATH_LEN, "%s/%s/logo.png", RES_ROOT, m_pcImagePath);
	m_pkLogo = CCSprite::create(acBuffer);
	m_pkLogo->setScale(ui_main_scale);
	m_pkLogo->setPosition(ui_ccp(0.5f, 0.5f));
	m_pkLogo->setOpacity(0);
	addChild(m_pkLogo);
	m_pkLogo->runAction(CCSequence::create(CCFadeIn::create(0.5f), CCDelayTime::create(0.8f), CCFadeOut::create(0.5f), NULL));

	LoadPng("bg", false);
	LoadPng("game_logo_0", true);
	LoadPng("game_logo_1", true);
	LoadPng("start_frame", false);
	LoadPng("bottom_logo", false);
	LoadPng("start_text", true);
	UILayer::SetModalTextureName(LoadImage("alert", false) + ".png");

	schedule(schedule_selector(TexasPoker::Update), 0.0f);
	schedule(schedule_selector(TexasPoker::UpdateVenus3D), 0.0f);
	schedule(schedule_selector(TexasPoker::UpdateSyncServerTime), 30.0f);
	GetDeviceID();
}

void TexasPoker::UnloadLogin()
{
	for(VeVector<cocos2d::CCTexture2D*>::iterator it = m_kLoginTextureArray.Begin(); it != m_kLoginTextureArray.End(); ++it)
	{
		CCTextureCache::sharedTextureCache()->removeTexture(*it);
		(*it)->~CCTexture2D();
	}
	m_kLoginTextureArray.Clear();
}

class ExitAlertCallback : public UIAlert::Callback
{
public:
	ExitAlertCallback()
	{
		m_kTitle = TT("ExitTitle");
		m_kContent = TT("ExitContent");
		m_kEnter = TT("Confirm");
		m_kCancel = TT("Cancel");
	}

	virtual void OnConfirm()
	{
		CCDirector::sharedDirector()->end();
	}
};

class LoadingData : public UILoading::Data
{
public:
	LoadingData()
	{
		m_kContent = TT("Loading");
	}

};

class ToastCallback : public UIToast::Callback
{
public:
	ToastCallback()
	{
		m_bToastShow = false;
	}

	virtual void OnClose()
	{
		g_pkGame->scheduleOnce(schedule_selector(TexasPoker::UpdateToast), 0.0f);
	}

	void ProcessCache()
	{
		if(m_kContentCache.Size())
		{
			m_kContent = m_kContentCache.Front();
			g_pLayerManager->PushTopLayer("toast", this);
			m_bToastShow = true;
			m_kContentCache.PopFront();
		}
		else
		{
			m_bToastShow = false;
		}
	}

	void Toast(const VeChar8* pcContent, bool bInsert)
	{
		if(!m_bToastShow)
		{
			m_kContent = pcContent;
			g_pLayerManager->PushTopLayer("toast", this);
			m_bToastShow = true;
		}
		else
		{
			if(bInsert)
			{
				((UIToast*)g_pLayerManager->GetLayer("toast"))->SetText(pcContent);
			}
			else
			{
				m_kContentCache.PushBack(pcContent);
			}
		}
	}

protected:
	VeList<VeStringA> m_kContentCache;
	bool m_bToastShow;

};

void TexasPoker::Update(VeFloat32 f32Delta)
{
	if(!GetNumTexInLoading())
	{
		if(!(m_pkLogo->numberOfRunningActions()))
		{
			LoadSoundEffects();

			m_pkFadeQuad = CCSprite::createWithSpriteFrameName("point.png");
			m_pkFadeQuad->setScaleX(UILayer::Width());
			m_pkFadeQuad->setScaleY(UILayer::Height());
			m_pkFadeQuad->setPosition(ui_ccp(0.5f, 0.5f));
			m_pkFadeQuad->setColor(ccc3(0, 0, 0));
			addChild(m_pkFadeQuad, 5);
			m_pkFadeQuad->setVisible(true);
			g_pLayerManager->Lock();
			m_pkFadeQuad->setOpacity(255);
			m_f32FadeFactor = 0.0f;
			m_f32FadeType = 0;
			m_f32FadePassTime = 0;
			UpdateVolume();

			m_pkLogo->removeFromParent();
			m_pkLogo = NULL;
			VeChar8 acBuffer[VE_MAX_PATH_LEN];
			VeSprintf(acBuffer, VE_MAX_PATH_LEN, "%s/logo.png", m_pcImagePath);
			CCTextureCache::sharedTextureCache()->removeTextureForKey(acBuffer);
			unschedule(schedule_selector(TexasPoker::Update));
			g_pLayerManager->AddLayer(UIAlert::create());
			g_pLayerManager->AddLayer(UIAlertConfirm::create());
			g_pLayerManager->AddLayer(UILoading::create());
			g_pLayerManager->AddLayer(UINoticeBoard::create());
			g_pLayerManager->AddLayer(UIToast::create());
			g_pLayerManager->AddLayer(Login::create(), true);

			LoadImage("ui_0", false);
			LoadImage("ui_1", false);
			LoadImage("ui_shop", false);
			LoadImage("text_0", true);
			LoadImage("text_mini", true);
			LoadImage("text_help", true);
			LoadImage("card", false);
			LoadImage("head_icon", false);
			LoadImage("expression_icon", false);
			LoadImage("slot", false);
			LoadImage("keno", false);

			m_spExitCallback = VE_NEW ExitAlertCallback;
			m_spLoadingData = VE_NEW LoadingData;
			m_spToastCallback = VE_NEW ToastCallback;
			KeypadDelegateLayer* pkKeypad = KeypadDelegateLayer::create();
			addChild(pkKeypad);
			pkKeypad->setKeypadEnabled(true);
		}
	}
}

void TexasPoker::UpdateVenus3D(VeFloat32 f32Delta)
{
	VenusUpdate();
	m_kCurlQueue.Update();
	g_pClient->Update();
	if(m_bNeedStopLoading)
	{
		m_bNeedStopLoading = false;
		g_pLayerManager->PopAllLayers();
		g_pLayerManager->PopAllMiddleLayers();
	}
}

void TexasPoker::UpdateSyncServerTime(VeFloat32 f32Delta)
{
	if(g_pkHallAgent)
	{
		g_pkHallAgent->S_ReqSyncTime();
	}
}

bool TexasPoker::IsFading()
{
	return m_f32FadeType != 0;
}

void TexasPoker::FadeOut(VeFloat32 f32Time, FadeDelegate* kDelegate)
{
	if(IsFading()) return;
	if(kDelegate)
	{
		m_kFadeEvent.AddDelegate(*kDelegate);
	}
	m_f32FadeType = -1.0f;
	m_f32FadePassTime = f32Time;
	m_f32FadeTime = f32Time;
	m_f32FadeFactor = 1.0f;
	m_pkFadeQuad->setVisible(true);
	g_pLayerManager->Lock();
	m_pkFadeQuad->setOpacity(0);
	schedule(schedule_selector(TexasPoker::FadeUpdate), 0.0f);
	m_u32WaitTick = 2;
}

void TexasPoker::FadeIn(VeFloat32 f32Time, FadeDelegate* kDelegate)
{
	if(IsFading()) return;
	if(kDelegate)
	{
		m_kFadeEvent.AddDelegate(*kDelegate);
	}
	m_f32FadeType = 1.0f;
	m_f32FadePassTime = 0.0f;
	m_f32FadeTime = f32Time;
	m_f32FadeFactor = 0.0f;
	m_pkFadeQuad->setVisible(true);
	g_pLayerManager->Lock();
	m_pkFadeQuad->setOpacity(255);
	schedule(schedule_selector(TexasPoker::FadeUpdate), 0.0f);
	m_u32WaitTick = 2;
}

void TexasPoker::FadeUpdate(VeFloat32 f32Delta)
{
	if(m_u32WaitTick)
	{
		--m_u32WaitTick;
		return;
	}
	m_f32FadePassTime += f32Delta * m_f32FadeType;
	if(m_f32FadePassTime < m_f32FadeTime && m_f32FadePassTime > 0)
	{
		m_f32FadeFactor = m_f32FadePassTime / m_f32FadeTime;
		m_pkFadeQuad->setOpacity((1.0f - m_f32FadeFactor) * 255.0f);
	}
	else if(m_f32FadeType > 0)
	{
		m_f32FadeType = 0.0f;
		m_f32FadePassTime = 0.0f;
		m_f32FadeFactor = 1.0f;
		m_pkFadeQuad->setOpacity(0);
		m_pkFadeQuad->setVisible(false);
		g_pLayerManager->Unlock();
		unschedule(schedule_selector(TexasPoker::FadeUpdate));
		m_kFadeEvent.Callback();
		m_kFadeEvent.Clear();
	}
	else
	{
		m_f32FadeType = 0.0f;
		m_f32FadePassTime = 0.0f;
		m_f32FadeFactor = 0.0f;
		m_pkFadeQuad->setOpacity(255);
		m_pkFadeQuad->setVisible(true);
		g_pLayerManager->Lock();
		unschedule(schedule_selector(TexasPoker::FadeUpdate));
		m_kFadeEvent.Callback();
		m_kFadeEvent.Clear();
	}
	UpdateVolume();
}

void TexasPoker::Toast(const VeChar8* pcContent, bool bInsert)
{
	if(m_spToastCallback)
	{
		m_spToastCallback->Toast(pcContent, bInsert);
	}
}

void TexasPoker::UpdateToast(VeFloat32 f32Delta)
{
	if(m_spToastCallback)
	{
		m_spToastCallback->ProcessCache();
	}
	unschedule(schedule_selector(TexasPoker::UpdateToast));
}

void TexasPoker::LoadPng(const VeChar8* pcName, bool bLanguage)
{
	VeStringA kPath(RES_ROOT);
	kPath += "/";
	if(bLanguage)
	{
		kPath += m_pcLanguage;
		kPath += "/";
	}	
	kPath += m_pcImagePath;
	kPath += "/";
	kPath += pcName;
	++m_u32TexturesInLoading;
	CCTextureCache::sharedTextureCache()->addImageAsync(kPath + ".png", this, callfuncO_selector(TexasPoker::LoadingCallBack));
}

void TexasPoker::LoadingCallBack(CCObject* pkObject)
{
	VE_ASSERT(m_u32TexturesInLoading);
	--m_u32TexturesInLoading;
	m_kLoginTextureArray.PushBack((CCTexture2D*)pkObject);
}

void TexasPoker::LoadingPvrcczCallBack(CCObject* pkObject)
{
	VE_ASSERT(m_u32TexturesInLoading && m_kLoading.Size());
	--m_u32TexturesInLoading;
	m_kObjects.PushBack(pkObject);
	if(!m_u32TexturesInLoading)
	{
		VE_ASSERT(m_kLoading.Size() == m_kObjects.Size());
		for(VeUInt32 i(0); i < m_kLoading.Size(); ++i)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(m_kLoading[i], (CCTexture2D*)m_kObjects[i]);
		}
		m_kLoading.Clear();
		m_kObjects.Clear();
	}
}

void TexasPoker::LoadSoundEffects()
{
	LoadSoundEffect(SE_HELPOK, "Help_ok.ogg");
	LoadSoundEffect(SE_BANK_UP, "Bank_up.ogg");
	LoadSoundEffect(SE_GOLD, "Gold.ogg");
	LoadSoundEffect(SE_GLOBAL, "Keno_go.ogg");
	LoadSoundEffect(SE_RIGHT, "Keno_right.ogg");
	LoadSoundEffect(SE_BET, "Bet.ogg");
	LoadSoundEffect(SE_CHIPS_WIN, "Chips_Win.ogg");
	LoadSoundEffect(SE_BTN_CLICK, "button_click.wav");
	LoadSoundEffect(SE_SLOT_CARD, "slot_card.ogg");
	LoadSoundEffect(SE_DEAL_CARD, "Desk_new_card.ogg");
	LoadSoundEffect(SE_GET_CHIPS, "get_chips.ogg");
	LoadSoundEffect(SE_SLOT_ARMPULL, "slots_armPull.ogg");
	LoadSoundEffect(SE_SLOT_ARMRELEASE, "slots_armRelease.ogg");
	LoadSoundEffect(SE_SLOT_LOOP, "slots_reelLoop.ogg");
	LoadSoundEffect(SE_SLOT_REELSTOP, "slots_reelStop.ogg");
	LoadSoundEffect(SE_SLOT_WINTYPE0, "slotswin0.ogg");
	LoadSoundEffect(SE_SLOT_WINTYPE1, "slotswin1.ogg");
	LoadSoundEffect(SE_SLOT_WINTYPE2, "slotswin2.ogg");
	LoadSoundEffect(SE_POKER_ON_CLOCK, "poker_onclock.ogg");
	LoadSoundEffect(SE_POKER_ON_MYTURN, "poker_onmyturn.ogg");
	LoadSoundEffect(SE_POKER_WIN_0, "poker_win_0.ogg");
	LoadSoundEffect(SE_POKER_WIN_1, "poker_win_1.ogg");
	LoadSoundEffect(SE_POKER_WIN_2, "poker_win_2.ogg");
	LoadSoundEffect(SE_WIN_CATE_0, "win_cate_0.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_1, "win_cate_1.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_2, "win_cate_2.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_3, "win_cate_3.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_4, "win_cate_4.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_5, "win_cate_5.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_6, "win_cate_6.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_7, "win_cate_7.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_8, "win_cate_8.ogg",true);
	LoadSoundEffect(SE_WIN_CATE_9, "win_cate_9.ogg",true);
	LoadSoundEffect(SE_POKER_ON_ADD, "Eff_Add.ogg",true);
	LoadSoundEffect(SE_POKER_ON_ALLIN, "Eff_Allin.ogg",true);
	LoadSoundEffect(SE_POKER_ON_CALL, "Eff_Call.ogg",true);
	LoadSoundEffect(SE_POKER_ON_CHECK, "Eff_Check.ogg",true);
	LoadSoundEffect(SE_POKER_ON_DISS, "Eff_Diss.ogg",true);
	LoadSoundEffect(SE_POKER_ON_FLOP, "Eff_Flop.ogg",true);
	LoadSoundEffect(SE_POKER_ON_RIVER, "Eff_River.ogg",true);
	LoadSoundEffect(SE_POKER_ON_START, "Eff_Start.ogg",true);
	LoadSoundEffect(SE_POKER_ON_TURN, "Eff_Turn.ogg",true);
	LoadSoundEffect(SE_POKER_ON_WELCOME, "Eff_Welcome.ogg",true);
	LoadSoundEffect(SE_BANKERWIN, "Eff_Banker_win.ogg",true);
	LoadSoundEffect(SE_PLAYERWIN, "Eff_Player_win.ogg",true);
	LoadSoundEffect(SE_DRAWGAME, "Eff_Tie.ogg",true);
	UIButton::SetPressedSoundGlobal(SE_BTN_CLICK);
}

void TexasPoker::ShowLoading(bool bMiddle)
{
	if(bMiddle)
	{
		g_pLayerManager->PushMiddleLayer("loading", m_spLoadingData);
	}
	else
	{
		g_pLayerManager->PushLayer("loading", m_spLoadingData);
	}
}

#include "Shaders.h"

void TexasPoker::LoadShaders()
{
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE] = new CCGLProgram();
	RestoreShaders();
}

void TexasPoker::ReleaseShaders()
{
	for(VeUInt32 i(0); i < SHADER_MAX; ++i)
	{
		CC_SAFE_RELEASE(m_apkShaders[i]);
	}
	VeZeroMemory(m_apkShaders, sizeof(m_apkShaders));
}

void TexasPoker::RestoreShaders()
{
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->reset();
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->initWithVertexShaderByteArray(PERSPECTIVE_SPRITE_SHADER, ccPositionTextureColor_frag);
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->link();
	m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->updateUniforms();
	CHECK_GL_ERROR_DEBUG();
	m_au32Params[SHADER_PERSPECTIVE_SPRITE] = glGetUniformLocation(m_apkShaders[SHADER_PERSPECTIVE_SPRITE]->getProgram(), "g_v4Params");
}

CCGLProgram* TexasPoker::GetShader(Shader eShader)
{
	if(eShader < SHADER_MAX)
	{
		return m_apkShaders[eShader];
	}
	return NULL;
}

VeUInt32 TexasPoker::GetShaderParams(Shader eShader)
{
	if(eShader < SHADER_MAX)
	{
		return m_au32Params[SHADER_PERSPECTIVE_SPRITE];
	}
	return 0;
}

UILayerManager* TexasPoker::GetLayerManager()
{
	return m_pkManager;
}

void TexasPoker::UpdateVolume()
{
	g_pSoundSystem->SetMusicVolume(g_pSaveData->ReadParam("music_volume", 1.0f) * m_f32FadeFactor);
}

#if defined(VE_PLATFORM_MOBILE_SIM)
extern VeChar8 g_acLanguage[128];
#endif

VeString TexasPoker::GetSysLanguage()
{
#if defined(VE_PLATFORM_MOBILE_SIM)
	return g_acLanguage;
#elif defined(VE_PLATFORM_IOS)
	return CocoaHelper::GetLanguage();
#elif defined(VE_PLATFORM_ANDROID)
	VeString kRes = ZH_CN;
	JniMethodInfo t;
	jstring i;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_GetLanguage",
		"()Ljava/lang/String;"))
	{
		i = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
		kRes = jstringTostring2(t.env,i);
		t.env->DeleteLocalRef(t.classID);
	}
	return kRes;
#else
	return ZH_CN;
#endif
}

VeString TexasPoker::GetDeviceID()
{
	VeString  kDeviceID = "b6446d8efda1e1ffebcad715a86c150b";

#if defined(VE_PLATFORM_IOS)
	kDeviceID = CocoaHelper::GetDeviceID();
#elif defined(VE_PLATFORM_ANDROID)
	JniMethodInfo t;
	jstring i;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_GetDeviceID",
		"()Ljava/lang/String;"))
	{
		i = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
		kDeviceID = jstringTostring2(t.env,i);
		t.env->DeleteLocalRef(t.classID);
	}
#else
	kDeviceID = GetWindowsDeviceID();
#endif
	return kDeviceID;
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
const VeChar8* TexasPoker::jstringTostring2( JNIEnv* env, jstring jstr )
{
	static VeChar8 s_acBuffer[1024];
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("utf-8");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0)
	{
		VE_ASSERT(alen < 1024);
		VeMemcpy(s_acBuffer, ba, alen);
		s_acBuffer[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return s_acBuffer;
}
#endif

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
VeString TexasPoker::GetWindowsDeviceID()
{
	VeChar8 Root[] = "C:\\";
	VeChar8 Volumelabel[20];
	DWORD SerialNumber;
	DWORD MaxCLength;
	DWORD FileSysFlag;
	VeChar8 FileSysName[10];

	VeChar8 m_pcDeviceID[256] = "";
	GetVolumeInformation( "C:\\",Volumelabel,255,&SerialNumber,&MaxCLength,&FileSysFlag,FileSysName,255);
	sprintf(m_pcDeviceID,"0x%x",SerialNumber);

	MD5 md5;
	md5.update(m_pcDeviceID);

	VeString kDeviceID = md5.toString().c_str();
	return kDeviceID;
}
#endif

VeString TexasPoker::GetClientName()
{
	VeString  kDeviceName = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	jstring i;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"jni_GetDeviceName",
		"()Ljava/lang/String;"))
	{
		i = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
		kDeviceName = jstringTostring2(t.env,i);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	kDeviceName = TT("TestName");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	kDeviceName = CocoaHelper::GetMachineName();
#endif
	return kDeviceName;
}

TexasPoker::Language TexasPoker::GetLanguage()
{
	return m_eLanguate;
}

void TexasPoker::ShowFeedBack()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t,
		"cn/VenusIE/TexasPoker/TexasPokerActivity",
		"ShowFeedBack",
		"()V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast("windows not support");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CocoaHelper::ShowFeedBack();
#endif
}

void TexasPoker::ConnectToHallServer()
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	ShowLoading();
	CONNECT(TexasPokerHallClient, m_kHallServerIp, m_u32HallServerPort, "TexasPokerHall");
}

void TexasPoker::DisconnectFromHallServer()
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	DISCONNECT(TexasPokerHallClient);
}

extern const VeChar8* GetChannel();

void TexasPoker::LoginToHallServer()
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	ShowLoading();
	GET_AGENT(TexasPokerHallAgentC)->S_ReqLogin(GetDeviceID(), GetChannel(), GetClientName(), (LanguageS)GetLanguage(), m_kVersion.m_au8Version[0], m_kVersion.m_au8Version[1]);
}

void TexasPoker::ShowAlert(const UIUserDataPtr& spUserData)
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	g_pLayerManager->PushLayer("alert_confirm", spUserData);
}

void TexasPoker::UpdateServerTime(VeUInt64 u64Time)
{
	m_u64ServerTime = u64Time;
	m_f64ServerSyncTime = g_pTime->GetTime();
	VeDebugOutputString("ServerTimeUpdated");
}

VeUInt64 TexasPoker::GetServerTime()
{
	VeUInt64 u64Delta = (g_pTime->GetTime() - m_f64ServerSyncTime) * 100.0f;
	return m_u64ServerTime + u64Delta;
}

void TexasPoker::ShowKenoResult()
{
	RunKeno* pkRunKeno = GetUILayer(RunKeno, "run_keno");
	VE_ASSERT(pkRunKeno);
	if(!(pkRunKeno->IsPlaying()))
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		VeChar8 acBuffer[128];
		VeStrcpy(acBuffer, 128, pkPlayer->GetKenoResult());
		VeChar8* pcContext;
		VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);
		if(pcTemp && (!VeStrcmp(pcTemp, "res")))
		{
			VeUInt32 u32IssueNumber;
			VeVector<VeUInt32> kKenoNumber;
			VeVector<VeUInt32> kPlayerNumber;
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			while(pcTemp)
			{
				if(pcTemp && VeStrcmp(pcTemp, "buy"))
				{
					kKenoNumber.PushBack(VeAtoi(pcTemp));
				}
				else
				{
					break;
				}
				pcTemp = VeStrtok(NULL, ",", &pcContext);
			}
			if(pcTemp && (!VeStrcmp(pcTemp, "buy")))
			{
				pcTemp = VeStrtok(NULL, ",", &pcContext);
				u32IssueNumber = VeAtoi(pcTemp);
				pcTemp = VeStrtok(NULL, ",", &pcContext);
				while(pcTemp)
				{
					if(pcTemp && VeStrcmp(pcTemp, "bet"))
					{
						kPlayerNumber.PushBack(VeAtoi(pcTemp));
					}
					else
					{
						break;
					}
					pcTemp = VeStrtok(NULL, ",", &pcContext);
				}
			}
			VE_ASSERT(kKenoNumber.Size() == 10);
			VE_ASSERT(kPlayerNumber.Size() == 10);
			RunKeno::Callback* pkCallback = VE_NEW RunKeno::Callback;
			pkCallback->m_u32IssueNumber = u32IssueNumber;
			for(VeUInt32 i(0); i < 10; ++i)
			{
				pkCallback->m_au32KenoNumber[i] = kKenoNumber[i];
				pkCallback->m_au32PlayerNumber[i] = kPlayerNumber[i];
			}
			g_pLayerManager->PushLayer("run_keno", pkCallback);
		}
	}
}

void TexasPoker::KeypadDelegateLayer::keyBackClicked()
{
	g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
	if(g_pLayerManager->GetMiddleLayersNum())
	{
		g_pkGame->ShowExit();
	}
	else if(!g_pLayerManager->Back())
	{
		g_pkGame->ShowExit();
	}
}

void TexasPoker::ShowExit()
{
	g_pLayerManager->PushLayer("alert", m_spExitCallback);
}

void TexasPoker::KeypadDelegateLayer::keyMenuClicked()
{
	g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
	g_pLayerManager->Menu();
}

void TexasPoker::RefreshServerNotice()
{
	TexasPoker::Language eLan = GetLanguage();
	VeString kUrl = m_kHttpServer + "/GongGaoConfig.aspx";
	if(eLan==en_US)
		kUrl += "?lang=en";
	else if(eLan==zh_TW)
		kUrl += "?lang=tc";
	else
		kUrl += "?lang=sc";
	m_kCurlQueue.AddBackgroundTask(VE_NEW TexasPokerNoticeTask(kUrl));
}

VeString GetJson(const char* srcStr,const char* startStr,const char* endStr)
{
	VeString res="";
	if(!startStr || !endStr || !srcStr)
		return res;
	VeString src = srcStr;
	VeUInt32 start = src.Find(startStr);
	if(start!=VeUInt32(-1))
	{
		res = src.GetSubstring(start+strlen(startStr),endStr);
		res.Replace("\\r\\n", "\r\n");
	}
	return res;
}

void TexasPoker::OnServerNoticeUpdate(const VeChar8* pcResult)
{
	VeVector< VePair<VeStringA,VeStringA> > kBoard;
	if(*pcResult)
	{
		m_kServerNotice = ::GetJson(pcResult,"\"notice\":\"","\"");
		kBoard.Clear();
		VeChar8 acBuffer[4096];
		VeStrcpy(acBuffer, 4096, pcResult);
		VeChar8* pcStart = VeStrstr(acBuffer,"\"chips\"");

		while(pcStart)
		{
			VeStringA kNickName = ::GetJson(pcStart,"\"nick\":\"","\"");
			VeStringA kChips = ::GetJson(pcStart,"\"chips\":\"","\"");
			kBoard.PushBack(VePair<VeStringA,VeStringA>(kNickName, kChips));
			pcStart += VeStrlen("\"chips\"");
			pcStart = VeStrstr(pcStart,"\"chips\"");
		}
	}
	bool bBoardUpdate(false);
	if(kBoard.Size() && kBoard.Size() == m_kLeaderboard.Size())
	{
		for(VeUInt32 i(0); i < kBoard.Size(); ++i)
		{
			if(kBoard[i] != m_kLeaderboard[i])
			{
				bBoardUpdate = true;
				break;
			}
		}
	}
	else
	{
		bBoardUpdate = true;
	}
	if(bBoardUpdate)
	{
		m_kLeaderboard = kBoard;
	}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "login"))
	{
		g_pLayerManager->PopAllLayers();
		g_pLayerManager->PopAllMiddleLayers();
		static_cast<Login*>(pkMainLayer)->ShowNotice(m_kServerNotice);
	}
	else if(bBoardUpdate && (!VeStrcmp(pkMainLayer->GetName(), "hall")))
	{
		static_cast<Hall*>(pkMainLayer)->OnLeaderboardUpdate(m_kLeaderboard);
	}
}

const VeVector< VePair<VeStringA,VeStringA> >& TexasPoker::GetLeaderboard()
{
	return m_kLeaderboard;
}

void TexasPoker::StartRefreshPurchase()
{
	schedule(schedule_selector(TexasPoker::TickRefreshPurchase), 3.0f);
}

void TexasPoker::EndRefreshPurchase()
{
	unschedule(schedule_selector(TexasPoker::TickRefreshPurchase));
}

void TexasPoker::TickRefreshPurchase(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	pkPlayer->S_ReqRefreshPurchase();
}

void TexasPoker::OnPurchaseEnd(VeInt32 i32State)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	if(i32State == 1)
	{
		pkPlayer->S_ReqRefreshPurchase();
	}
	else if(i32State == 0)
	{
		ShowPurchaseAlert(true);
		StartRefreshPurchase();
	}
	else
	{
		ShowPurchaseAlert(false);
	}
}

class VerifyPurchaseTask : public VeBackgroundTask
{
public:
	VerifyPurchaseTask(const VeChar8* pcUrl)
	{
		m_kUrl = pcUrl;
	}

	virtual void DoBackgroundTask(VeBackgroundTaskQueue& kMgr)
	{
		VeInt32 i32State(0);
		while(i32State == 0)
		{
			m_eResult = CURL_LAST;
			m_kHttpBody = "";
			CURL* pkCurl = curl_easy_init();
			if(pkCurl)
			{
				curl_easy_setopt(pkCurl, CURLOPT_URL, (const VeChar8*)m_kUrl);
				curl_easy_setopt(pkCurl, CURLOPT_WRITEFUNCTION, &ReceiveData);
				curl_easy_setopt(pkCurl, CURLOPT_WRITEDATA, &m_kHttpBody);
				m_eResult = curl_easy_perform(pkCurl);
				curl_easy_cleanup(pkCurl);
			}
			VeStringA kState = GetJson(m_kHttpBody,"\"state\":","}");
			VeInt32 i32Temp;
			bool bRes = kState.To(i32Temp);
			i32State = (bRes ? i32Temp : -1);
		}
		kMgr.AddMainThreadTask(this);
	}

	virtual void DoMainThreadTask(VeBackgroundTaskQueue& kMgr)
	{
		VeStringA kState = GetJson(m_kHttpBody,"\"state\":","}");
		VeInt32 i32State;
		bool bRes = kState.To(i32State);
		g_pkGame->OnPurchaseEnd(bRes ? i32State : -1);
		g_pkGame->StartRefreshPurchase();
	}

protected:
	VeStringA m_kUrl;
	VeStringA m_kHttpBody;
	CURLcode m_eResult;

};

void TexasPoker::VerifyPurchase(const VeChar8* pcValue)
{
	g_pkGame->EndRefreshPurchase();
	m_kCurlQueue.AddBackgroundTask(VE_NEW VerifyPurchaseTask(m_kHttpServer + "/FeeInfo.aspx" + pcValue));
}

class VerifyIOSPurchaseTask : public VeBackgroundTask
{
public:
	VerifyIOSPurchaseTask(const VeChar8* pcUrl, const VeChar8* pcPostData)
	{
		m_kUrl = pcUrl;
		m_kPostData = pcPostData;
	}

	virtual void DoBackgroundTask(VeBackgroundTaskQueue& kMgr)
	{
		VeInt32 i32State(-1);
		{
			m_eResult = CURL_LAST;
			m_kHttpBody = "";
			CURL* pkCurl = curl_easy_init();
			if(pkCurl)
			{
				curl_easy_setopt(pkCurl, CURLOPT_URL, (const VeChar8*)m_kUrl);
				curl_easy_setopt(pkCurl, CURLOPT_WRITEFUNCTION, &ReceiveData);
				curl_easy_setopt(pkCurl, CURLOPT_WRITEDATA, &m_kHttpBody);
				curl_easy_setopt(pkCurl, CURLOPT_POST,1);
				curl_easy_setopt(pkCurl, CURLOPT_POSTFIELDS, (const VeChar8*)m_kPostData);
				curl_easy_setopt(pkCurl, CURLOPT_POSTFIELDSIZE,m_kPostData.Length());
				curl_slist *plist = curl_slist_append(NULL, "Content-Type: application/x-www-form-urlencoded");
				curl_easy_setopt(pkCurl, CURLOPT_HTTPHEADER, plist);
				m_eResult = curl_easy_perform(pkCurl);
				curl_easy_cleanup(pkCurl);
			}
			VeStringA kState = GetJson(m_kHttpBody,"\"state\":","}");
			VeInt32 i32Temp;
			bool bRes = kState.To(i32Temp);
			i32State = (bRes ? i32Temp : -1);
		}
		while(i32State == 0)
		{
			VeSleep(1000);
			m_eResult = CURL_LAST;
			m_kHttpBody = "";
			CURL* pkCurl = curl_easy_init();
			if(pkCurl)
			{
				curl_easy_setopt(pkCurl, CURLOPT_URL, (const VeChar8*)m_kUrl);
				curl_easy_setopt(pkCurl, CURLOPT_WRITEFUNCTION, &ReceiveData);
				curl_easy_setopt(pkCurl, CURLOPT_WRITEDATA, &m_kHttpBody);
				m_eResult = curl_easy_perform(pkCurl);
				curl_easy_cleanup(pkCurl);
			}
			VeStringA kState = GetJson(m_kHttpBody,"\"state\":","}");
			VeInt32 i32Temp;
			bool bRes = kState.To(i32Temp);
			i32State = (bRes ? i32Temp : -1);
		}
		kMgr.AddMainThreadTask(this);
	}

	virtual void DoMainThreadTask(VeBackgroundTaskQueue& kMgr)
	{
		VeStringA kState = GetJson(m_kHttpBody,"\"state\":","}");
		VeInt32 i32State;
		bool bRes = kState.To(i32State);
		g_pkGame->OnPurchaseEnd(bRes ? i32State : -1);
		g_pkGame->StartRefreshPurchase();
	}

protected:
	VeStringA m_kUrl;
	VeStringA m_kPostData;
	VeStringA m_kHttpBody;
	CURLcode m_eResult;

};

void TexasPoker::VerifyIOSPurchase(const VeChar8* pcValue, const VeChar8* pcPostData)
{
	g_pkGame->EndRefreshPurchase();
	m_kCurlQueue.AddBackgroundTask(VE_NEW VerifyIOSPurchaseTask(m_kHttpServer + "/FeeInfo.aspx" + pcValue, pcPostData));
}

void GetPurchaseList(TexasPoker::PurchaseInfoList& kList);

void TexasPoker::UpdatePurchaseList()
{
	PurchaseInfoList kList;
	GetPurchaseList(kList);
	PurchaseChoose& kChoose = GetUILayer(Shop, "shop")->GetChoose();
	kChoose.ClearAll();
	for(PurchaseInfoList::iterator it = kList.Begin(); it != kList.End(); ++it)
	{
		kChoose.AddItem(it->m_kInfo, it->m_u32Price, PurchaseItem::Type(it->m_eType), it->m_u32Chips, it->m_u32Icon, it->m_bVip);
	}
	kChoose.ScrollToTop();
}

class PurchaseAlertCallback : public UIAlertConfirm::Callback
{
public:
	PurchaseAlertCallback(bool bSuccess, const VeChar8* pcContent)
	{
		if(bSuccess)
		{
			if(pcContent)
			{
				m_kTitle = TT("PurchaseSucceed");
				m_kContent = pcContent;
			}
			else
			{
				m_kTitle = TT("PurchaseVerifying");
				m_kContent = TT("PurchaseSucceedContent");
			}
		}
		else
		{
			m_kTitle = TT("PurchaseFailed");
			m_kContent = TT("PurchaseFailedContent");
		}
		m_kEnter = TT("Confirm");
	}

	virtual void OnConfirm()
	{
		g_pLayerManager->PopAllLayers();
	}
};

void TexasPoker::ShowPurchaseAlert(bool bSuccess, const VeChar8* pcContent)
{
	ShowAlert(VE_NEW PurchaseAlertCallback(bSuccess, pcContent));
}

void TexasPoker::NeedStopLoading()
{
	m_bNeedStopLoading = true;
}

void TexasPoker::ShowNotice(const VeChar8* strNotice, VeUInt16 u16Show, VeUInt16 u16Interval)
{
	if(u16Show)
	{
		unschedule(schedule_selector(TexasPoker::OnNoticeCache));
		UILayer* pkLayer = g_pLayerManager->GetCurrentMainLayer();
		pkLayer->OnPushNotice(strNotice);
		m_kNoticeCache.PushBack(strNotice);
		while(m_kNoticeCache.Size() > 5)
		{
			m_kNoticeCache.PopFront();
		}
		schedule(schedule_selector(TexasPoker::OnNoticeCache), 60.0f);
	}
	else
	{
		m_kNoticeAlways.PushBack(strNotice);
	}
}

void TexasPoker::ClearAlwaysNotice()
{
	m_kNoticeAlways.Clear();
}

void TexasPoker::RestartNoticeAlways()
{
	unschedule(schedule_selector(TexasPoker::OnNoticeAlways));
	OnNoticeAlways(0);
	schedule(schedule_selector(TexasPoker::OnNoticeAlways), 120.0f);
}

void TexasPoker::OnNoticeCache(VeFloat32 f32Delta)
{
	UILayer* pkLayer = g_pLayerManager->GetCurrentMainLayer();
	for(VeList<VeStringA>::iterator it = m_kNoticeCache.Begin(); it != m_kNoticeCache.End(); ++it)
	{
		pkLayer->OnPushNotice(*it);
	}
}

void TexasPoker::OnNoticeAlways(VeFloat32 f32Delta)
{
	UILayer* pkLayer = g_pLayerManager->GetCurrentMainLayer();
	for(VeList<VeStringA>::iterator it = m_kNoticeAlways.Begin(); it != m_kNoticeAlways.End(); ++it)
	{
		pkLayer->OnPushNotice(*it);
	}
}
