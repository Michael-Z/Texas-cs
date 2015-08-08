#include "Login.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "UIAlert.h"
#include "SoundSystem.h"
#include "Hall.h"
#include "Knickknacks.h"
#include "Keno.h"
#include "TexasPokerHallClient.h"
#include "NativeInterface.h"

USING_NS_CC;

class LoginNoticeCallback : public UINoticeBoard::Callback
{
public:
	LoginNoticeCallback()
	{
		m_kTitle = TT("ServerNoticeTitle");
		m_kCancel = TT("Close");
	}

	virtual void OnClose()
	{
		UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
		if(!VeStrcmp(pkMainLayer->GetName(), Login::GetName()))
		{
			Login* pkLogin = static_cast<Login*>(pkMainLayer);
			pkLogin->ShowStart();
		}
	}
};

VeSmartPointer(LoginNoticeCallback);

Login::Login() : UILayerExt<Login>(Login::GetName())
{
	m_kOnFadeIn.Set(this, &Login::OnFadeIn);
	m_kOnFadeOut.Set(this, &Login::OnFadeOut);
}

Login::~Login()
{
	term();
}

bool Login::init()
{
	CCSprite* pkBackground = CCSprite::createWithTexture(g_pkGame->GetLoginTexture(LOGIN_TEX_BACKGROUND));
	pkBackground->setScale(ui_main_scale);
	pkBackground->setPosition(ui_ccp(0.5f, 0.5f));
	pkBackground->setBlendFunc(kCCBlendFuncDisable);
	addChild(pkBackground);

	CCTexture2D* apkLogo[2] =
	{
		g_pkGame->GetLoginTexture(LOGIN_TEX_GAMELOGO_0),
		g_pkGame->GetLoginTexture(LOGIN_TEX_GAMELOGO_1)
	};
	CCRect kRect = CCRectMake(0,0, apkLogo[0]->getContentSize().width, apkLogo[0]->getContentSize().height);
	CCAnimation* pkAnimation = CCAnimation::create();
	pkAnimation->addSpriteFrameWithTexture(apkLogo[0], kRect);
	pkAnimation->addSpriteFrameWithTexture(apkLogo[1], kRect);
	pkAnimation->setDelayPerUnit(0.2f);
	pkAnimation->setRestoreOriginalFrame(false);
	CCSprite* pkGameLogo = CCSprite::create();
	pkGameLogo->setScale(ui_main_scale);
	pkGameLogo->setPosition(ui_ccp(0.5f, 0.7f));
	pkGameLogo->runAction(CCRepeatForever::create(CCAnimate::create(pkAnimation)));
	addChild(pkGameLogo);

	CCSprite* pkStartFrame = CCSprite::createWithTexture(g_pkGame->GetLoginTexture(LOGIN_TEX_STARTFRAME));
	pkStartFrame->setScale(ui_main_scale);
	pkStartFrame->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 130 * g_f32ScaleHD * ui_main_scale)));
	addChild(pkStartFrame);

	m_pkStartText = CCSprite::createWithTexture(g_pkGame->GetLoginTexture(LOGIN_TEX_STARTTEXT));
	m_pkStartText->setScale(ui_main_scale);
	m_pkStartText->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 60 * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f - 132 * g_f32ScaleHD * ui_main_scale)));
	addChild(m_pkStartText);
	m_pkStartText->setVisible(false);

	CCSprite* pkBottom = CCSprite::createWithTexture(g_pkGame->GetLoginTexture(LOGIN_TEX_BOTTOMLOGO));
	pkBottom->setScale(ui_main_scale);
	pkBottom->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 208 * g_f32ScaleHD * ui_main_scale)));
	addChild(pkBottom);

	return UILayer::init();
}

void Login::term()
{
	UILayer::term();
}

void Login::OnActive(const UIUserDataPtr& spData)
{
	g_pSoundSystem->PlayMusic(LOGIN_MUSIC);
	g_pkGame->FadeIn(FADE_IN_TIME, &m_kOnFadeIn);
	g_pkGame->ClearAlwaysNotice();
	g_pkGame->ShowNotice(TT("Welcome"), 0, 0);
	NativeOnEventBegin("login");
}

void Login::OnDeactive(const UIUserDataPtr& spData)
{
	g_pSoundSystem->StopMusic();
	NativeOnEventEnd("login");
}

#include "VeSoundSystem.h"

bool Login::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(!UILayer::ProcessTouch(eType, pkTouch))
	{
		if(eType == UILayer::TOUCH_BEGAN)
		{
			if(m_pkStartText->isVisible())
			{
				HideStart();
				g_pkGame->ConnectToHallServer();
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Login::OnFadeIn(void)
{
	g_pkGame->RefreshServerNotice();
	g_pkGame->ShowLoading();
}

void Login::OnFadeOut(void)
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	g_pLayerManager->AddLayer(EditInfo::create());
	g_pLayerManager->AddLayer(Bank::create());
	g_pLayerManager->AddLayer(Shop::create());
	g_pLayerManager->AddLayer(RunKeno::create());
	g_pLayerManager->AddLayer(Hall::create(), true, NULL, false);
	g_pLayerManager->AddLayer(TexasHelp::create());
	g_pLayerManager->AddLayer(LottoHelp::create());
	g_pLayerManager->AddLayer(BaccHelp::create());
	g_pkGame->UpdatePurchaseList();
	g_pkGame->UnloadLogin();
	g_pkGame->LoadImages();
	g_pkGame->FadeIn(FADE_IN_TIME);
}

void Login::ShowNotice(const VeChar8* pcNotice)
{
	LoginNoticeCallbackPtr spCallback = VE_NEW LoginNoticeCallback;
	spCallback->m_kContent = pcNotice;
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	g_pLayerManager->PushLayer("notice_board", spCallback);
}

void Login::ShowStart()
{
	m_pkStartText->setVisible(true);
	m_pkStartText->setOpacity(0);
	m_pkStartText->stopAllActions();
	m_pkStartText->runAction(CCRepeatForever::create(CCSequence::create(
		CCFadeIn::create(0.3f), CCDelayTime::create(0.7f), CCFadeOut::create(0.2f), CCDelayTime::create(0.1f), NULL)));
}

void Login::HideStart()
{
	m_pkStartText->setVisible(false);
	m_pkStartText->setOpacity(0);
	m_pkStartText->stopAllActions();
}

void Login::IntoHall()
{
	schedule(schedule_selector(Login::WaitLoading), 0.0f);
}

const VeChar8* Login::GetName()
{
	return "login";
}

void Login::WaitLoading(VeFloat32 f32Delta)
{
	if(!(g_pkGame->GetNumTexInLoading()))
	{
		unschedule(schedule_selector(Login::WaitLoading));
		g_pkGame->FadeOut(FADE_OUT_TIME, &m_kOnFadeOut);
	}
}
