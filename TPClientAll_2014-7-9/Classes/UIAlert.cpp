#include "UIAlert.h"
#include "TexasPoker.h"
#include "UIButton.h"

USING_NS_CC;

UIPanel::UIPanel()
{
	m_pkRoot = CCSpriteBatchNode::create(UILayer::GetModalTextureName());
	addChild(m_pkRoot);
}

UIPanel::~UIPanel()
{

}

void UIPanel::InitWithCltBtBl(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height)
{
	VeStringA kClt = VeStringA(pcFileName) + "_lt.png";
	VeStringA kBt = VeStringA(pcFileName) + "_t.png";
	VeStringA kBl = VeStringA(pcFileName) + "_l.png";
	VeStringA kFill = VeStringA(pcFileName) + "_m.png";

	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkCorner[i] = CCSprite::createWithSpriteFrameName(kClt);
		m_apkCorner[i]->setAnchorPoint(ccp(1.0f, 0.0f));
		m_apkCorner[i]->setScaleX(i & 1 ? -1.0f : 1.0f);
		m_apkCorner[i]->setScaleY(i & 2 ? -1.0f : 1.0f);
		m_pkRoot->addChild(m_apkCorner[i]);
	}
	for(VeUInt32 i(0); i < 2; ++i)
	{
		m_apkBar[i] = CCSprite::createWithSpriteFrameName(kBt);
		m_apkBar[i]->setAnchorPoint(ccp(0.5f, 0.0f));
		m_apkBar[i]->setScaleY(i & 1 ? -1.0f : 1.0f);
		m_pkRoot->addChild(m_apkBar[i]);
	}
	for(VeUInt32 i(2); i < 4; ++i)
	{
		m_apkBar[i] = CCSprite::createWithSpriteFrameName(kBl);
		m_apkBar[i]->setAnchorPoint(ccp(1.0f, 0.5f));
		m_apkBar[i]->setScaleX(i & 1 ? -1.0f : 1.0f);
		m_pkRoot->addChild(m_apkBar[i]);
	}
	m_pkFill = CCSprite::createWithSpriteFrameName(kFill);
	m_pkRoot->addChild(m_pkFill);

	m_kMinSize.width = m_apkCorner[0]->getContentSize().width * 2.0f;
	m_kMinSize.height = m_apkCorner[0]->getContentSize().height * 2.0f;

	SetSize(f32Width, f32Height);
}

void UIPanel::InitWithCltClbBtBbBl(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height)
{
	VeStringA kClt = VeStringA(pcFileName) + "_lt.png";
	VeStringA kClb = VeStringA(pcFileName) + "_lb.png";
	VeStringA kBt = VeStringA(pcFileName) + "_t.png";
	VeStringA kBb = VeStringA(pcFileName) + "_b.png";
	VeStringA kBl = VeStringA(pcFileName) + "_l.png";
	VeStringA kFill = VeStringA(pcFileName) + "_m.png";

	m_apkCorner[0] = CCSprite::createWithSpriteFrameName(kClt);
	m_apkCorner[0]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkCorner[0]->setScaleX(1.0f);
	m_apkCorner[0]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkCorner[0]);
	m_apkCorner[1] = CCSprite::createWithSpriteFrameName(kClt);
	m_apkCorner[1]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkCorner[1]->setScaleX(-1.0f);
	m_apkCorner[1]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkCorner[1]);
	m_apkCorner[2] = CCSprite::createWithSpriteFrameName(kClb);
	m_apkCorner[2]->setAnchorPoint(ccp(1.0f, 1.0f));
	m_apkCorner[2]->setScaleX(1.0f);
	m_apkCorner[2]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkCorner[2]);
	m_apkCorner[3] = CCSprite::createWithSpriteFrameName(kClb);
	m_apkCorner[3]->setAnchorPoint(ccp(1.0f, 1.0f));
	m_apkCorner[3]->setScaleX(-1.0f);
	m_apkCorner[3]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkCorner[3]);

	m_apkBar[0] = CCSprite::createWithSpriteFrameName(kBt);
	m_apkBar[0]->setAnchorPoint(ccp(0.5f, 0.0f));
	m_apkBar[0]->setScaleX(1.0f);
	m_apkBar[0]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkBar[0]);
	m_apkBar[1] = CCSprite::createWithSpriteFrameName(kBb);
	m_apkBar[1]->setAnchorPoint(ccp(0.5f, 1.0f));
	m_apkBar[1]->setScaleX(1.0f);
	m_apkBar[1]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkBar[1]);

	m_apkBar[2] = CCSprite::createWithSpriteFrameName(kBl);
	m_apkBar[2]->setAnchorPoint(ccp(1.0f, 0.5f));
	m_apkBar[2]->setScaleX(1.0f);
	m_apkBar[2]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkBar[2]);
	m_apkBar[3] = CCSprite::createWithSpriteFrameName(kBl);
	m_apkBar[3]->setAnchorPoint(ccp(1.0f, 0.5f));
	m_apkBar[3]->setScaleX(-1.0f);
	m_apkBar[3]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkBar[3]);

	m_pkFill = CCSprite::createWithSpriteFrameName(kFill);
	m_pkRoot->addChild(m_pkFill);

	m_kMinSize.width = m_apkCorner[0]->getContentSize().width * 2.0f;
	m_kMinSize.height = m_apkCorner[0]->getContentSize().height + m_apkCorner[2]->getContentSize().height;

	SetSize(f32Width, f32Height);
}

void UIPanel::InitWithCltClbCrtCrbBtBbBlBr(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height)
{
	VeStringA kClt = VeStringA(pcFileName) + "_lt.png";
	VeStringA kClb = VeStringA(pcFileName) + "_lb.png";
	VeStringA kCrt = VeStringA(pcFileName) + "_rt.png";
	VeStringA kCrb = VeStringA(pcFileName) + "_rb.png";
	VeStringA kBt = VeStringA(pcFileName) + "_t.png";
	VeStringA kBb = VeStringA(pcFileName) + "_b.png";
	VeStringA kBl = VeStringA(pcFileName) + "_l.png";
	VeStringA kBr = VeStringA(pcFileName) + "_r.png";
	VeStringA kFill = VeStringA(pcFileName) + "_m.png";

	m_apkCorner[0] = CCSprite::createWithSpriteFrameName(kClt);
	m_apkCorner[0]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_pkRoot->addChild(m_apkCorner[0]);
	m_apkCorner[1] = CCSprite::createWithSpriteFrameName(kCrt);
	m_apkCorner[1]->setAnchorPoint(ccp(0.0f, 0.0f));
	m_pkRoot->addChild(m_apkCorner[1]);
	m_apkCorner[2] = CCSprite::createWithSpriteFrameName(kClb);
	m_apkCorner[2]->setAnchorPoint(ccp(1.0f, 1.0f));
	m_pkRoot->addChild(m_apkCorner[2]);
	m_apkCorner[3] = CCSprite::createWithSpriteFrameName(kCrb);
	m_apkCorner[3]->setAnchorPoint(ccp(0.0f, 1.0f));
	m_pkRoot->addChild(m_apkCorner[3]);

	m_apkBar[0] = CCSprite::createWithSpriteFrameName(kBt);
	m_apkBar[0]->setAnchorPoint(ccp(0.5f, 0.0f));
	m_pkRoot->addChild(m_apkBar[0]);
	m_apkBar[1] = CCSprite::createWithSpriteFrameName(kBb);
	m_apkBar[1]->setAnchorPoint(ccp(0.5f, 1.0f));
	m_pkRoot->addChild(m_apkBar[1]);

	m_apkBar[2] = CCSprite::createWithSpriteFrameName(kBl);
	m_apkBar[2]->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkRoot->addChild(m_apkBar[2]);
	m_apkBar[3] = CCSprite::createWithSpriteFrameName(kBr);
	m_apkBar[3]->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkRoot->addChild(m_apkBar[3]);

	m_pkFill = CCSprite::createWithSpriteFrameName(kFill);
	m_pkRoot->addChild(m_pkFill);

	m_kMinSize.width = m_apkCorner[0]->getContentSize().width * 2.0f;
	m_kMinSize.height = m_apkCorner[0]->getContentSize().height + m_apkCorner[2]->getContentSize().height;

	SetSize(f32Width, f32Height);
}

void UIPanel::SetSize(VeFloat32 f32Width, VeFloat32 f32Height)
{
	f32Width = VE_MAX(f32Width, m_kMinSize.width);
	f32Height = VE_MAX(f32Height, m_kMinSize.height);
	m_kSize.width = f32Width;
	m_kSize.height = f32Height;

	f32Width -= m_kMinSize.width;
	f32Height -= m_kMinSize.height;

	m_apkCorner[0]->setPosition(ccp(-f32Width * 0.5f, f32Height * 0.5f));
	m_apkCorner[1]->setPosition(ccp(f32Width * 0.5f, f32Height * 0.5f));
	m_apkCorner[2]->setPosition(ccp(-f32Width * 0.5f, -f32Height * 0.5f));
	m_apkCorner[3]->setPosition(ccp(f32Width * 0.5f, -f32Height * 0.5f));

	m_apkBar[0]->setPositionY(f32Height * 0.5f);
	m_apkBar[1]->setPositionY(-f32Height * 0.5f);
	m_apkBar[0]->setScaleX(f32Width);
	m_apkBar[1]->setScaleX(f32Width);

	m_apkBar[2]->setPositionX(-f32Width * 0.5f);
	m_apkBar[3]->setPositionX(f32Width * 0.5f);
	m_apkBar[2]->setScaleY(f32Height);
	m_apkBar[3]->setScaleY(f32Height);

	m_pkFill->setScaleX(f32Width);
	m_pkFill->setScaleY(f32Height);

	setContentSize(CCSize(m_kSize.width, m_kSize.height));
}

UIPanel* UIPanel::createWithCltBlBt(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height)
{
	UIPanel* pkFrame = new UIPanel();
	if(pkFrame)
	{
		pkFrame->InitWithCltBtBl(pcFileName, f32Width, f32Height);
		pkFrame->autorelease();
		return pkFrame;
	}
	CC_SAFE_DELETE(pkFrame);
	return NULL;
}

UIPanel* UIPanel::createWithCltClbBtBbBl(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height)
{
	UIPanel* pkFrame = new UIPanel();
	if(pkFrame)
	{
		pkFrame->InitWithCltClbBtBbBl(pcFileName, f32Width, f32Height);
		pkFrame->autorelease();
		return pkFrame;
	}
	CC_SAFE_DELETE(pkFrame);
	return NULL;
}

UIPanel* UIPanel::createWithCltClbCrtCrbBtBbBlBr(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height)
{
	UIPanel* pkFrame = new UIPanel();
	if(pkFrame)
	{
		pkFrame->InitWithCltClbCrtCrbBtBbBlBr(pcFileName, f32Width, f32Height);
		pkFrame->autorelease();
		return pkFrame;
	}
	CC_SAFE_DELETE(pkFrame);
	return NULL;
}

void UIPanel::Show(VeFloat32 f32FadeIn, VeFloat32 f32Delay, VeFloat32 f32FadeOut)
{
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkCorner[i]->setOpacity(0);
		m_apkCorner[i]->runAction(CCSequence::create(CCFadeIn::create(f32FadeIn), CCDelayTime::create(f32Delay), CCFadeOut::create(f32FadeOut), NULL));
		m_apkBar[i]->setOpacity(0);
		m_apkBar[i]->runAction(CCSequence::create(CCFadeIn::create(f32FadeIn), CCDelayTime::create(f32Delay), CCFadeOut::create(f32FadeOut), NULL));
	}
	m_pkFill->setOpacity(0);
	m_pkFill->runAction(CCSequence::create(CCFadeIn::create(f32FadeIn), CCDelayTime::create(f32Delay), CCFadeOut::create(f32FadeOut), NULL));
}

void UIPanel::PanelFadeIn(VeFloat32 f32FadeIn, VeFloat32 f32Delay)
{
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkCorner[i]->setOpacity(0);
		m_apkCorner[i]->runAction(CCSequence::create(CCDelayTime::create(f32Delay),CCFadeIn::create(f32FadeIn),NULL));
		m_apkBar[i]->setOpacity(0);
		m_apkBar[i]->runAction(CCSequence::create(CCDelayTime::create(f32Delay),CCFadeIn::create(f32FadeIn), NULL));
	}
	m_pkFill->setOpacity(0);
	m_pkFill->runAction(CCSequence::create(CCDelayTime::create(f32Delay),CCFadeIn::create(f32FadeIn), NULL));
}

void UIPanel::setOpacity(GLubyte value)
{
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkCorner[i]->setOpacity(value);
		m_apkBar[i]->setOpacity(value);
	}
	m_pkFill->setOpacity(value);
}

void UIPanel::SetMiddleVisible(bool bVisible)
{
	m_pkFill->setVisible(bVisible);
}

UIBarH::UIBarH()
{
	m_pkRoot = CCSpriteBatchNode::create(UILayer::GetModalTextureName());
	addChild(m_pkRoot);
}

UIBarH::~UIBarH()
{

}

void UIBarH::SetLength(VeFloat32 f32Length)
{
	f32Length = VE_MAX(f32Length, m_f32MinLength);
	m_f32Length = f32Length;

	f32Length -= m_f32MinLength;

	m_apkEndpoint[0]->setPositionX(-f32Length * 0.5f);
	m_apkEndpoint[1]->setPositionX(f32Length * 0.5f);

	m_pkFill->setScaleX(f32Length);

	setContentSize(CCSize(f32Length, m_pkFill->getContentSize().height));
}

void UIBarH::InitWithEl(const VeChar8* pcFileName, VeFloat32 f32Length)
{
	VeStringA kEl = VeStringA(pcFileName) + "_l.png";
	VeStringA kFill = VeStringA(pcFileName) + "_m.png";

	m_apkEndpoint[0] = CCSprite::createWithSpriteFrameName(kEl);
	m_apkEndpoint[0]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkEndpoint[0]->setScaleX(1.0f);
	m_apkEndpoint[0]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkEndpoint[0]);
	m_apkEndpoint[1] = CCSprite::createWithSpriteFrameName(kEl);
	m_apkEndpoint[1]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkEndpoint[1]->setScaleX(-1.0f);
	m_apkEndpoint[1]->setScaleY(1.0f);
	m_pkRoot->addChild(m_apkEndpoint[1]);

	m_pkFill = CCSprite::createWithSpriteFrameName(kFill);
	m_pkFill->setAnchorPoint(ccp(0.5f, 0.0f));
	m_pkRoot->addChild(m_pkFill);

	m_f32MinLength = m_apkEndpoint[0]->getContentSize().width + m_apkEndpoint[1]->getContentSize().width;

	SetLength(f32Length);
}

void UIBarH::InitWithElEr(const VeChar8* pcFileName, VeFloat32 f32Length)
{
	VeStringA kEl = VeStringA(pcFileName) + "_l.png";
	VeStringA kEr = VeStringA(pcFileName) + "_r.png";
	VeStringA kFill = VeStringA(pcFileName) + "_m.png";

	m_apkEndpoint[0] = CCSprite::createWithSpriteFrameName(kEl);
	m_apkEndpoint[0]->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkRoot->addChild(m_apkEndpoint[0]);
	m_apkEndpoint[1] = CCSprite::createWithSpriteFrameName(kEr);
	m_apkEndpoint[1]->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkRoot->addChild(m_apkEndpoint[1]);

	m_pkFill = CCSprite::createWithSpriteFrameName(kFill);
	m_pkFill->setAnchorPoint(ccp(0.5f, 0.5f));
	m_pkRoot->addChild(m_pkFill);

	m_f32MinLength = m_apkEndpoint[0]->getContentSize().width + m_apkEndpoint[1]->getContentSize().width;

	SetLength(f32Length);
}

UIBarH* UIBarH::createWithEl(const VeChar8* pcFileName, VeFloat32 f32Length)
{
	UIBarH* pkBar = new UIBarH();
	if(pkBar)
	{
		pkBar->InitWithEl(pcFileName, f32Length);
		pkBar->autorelease();
		return pkBar;
	}
	CC_SAFE_DELETE(pkBar);
	return NULL;
}

UIBarH* UIBarH::createWithElEr(const VeChar8* pcFileName, VeFloat32 f32Length)
{
	UIBarH* pkBar = new UIBarH();
	if(pkBar)
	{
		pkBar->InitWithElEr(pcFileName, f32Length);
		pkBar->autorelease();
		return pkBar;
	}
	CC_SAFE_DELETE(pkBar);
	return NULL;
}

UIAlert::UIAlert() : UILayerModalExt<UIAlert>("alert")
{
	UIInitDelegate(UIAlert, OnEnter);
	UIInitDelegate(UIAlert, OnCancel);
}

UIAlert::~UIAlert()
{
	term();
}

bool UIAlert::init()
{
	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 280.0f * ui_main_scale * g_f32ScaleHD, 156.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanel);
	m_pkPanel->setPosition(ui_ccp(0.5f, 0.5f));

	m_pkTitle = UIBarH::createWithEl("alert_title", 280.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD);
	m_pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(m_pkTitle);
	m_pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 78.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	UIPanel* pkEnter_n = UIPanel::createWithCltClbBtBbBl("alert_btn_0", 128.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkEnter_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 128.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkEnter = VE_NEW UIButton(pkEnter_n, pkEnter_p);
	pkEnter->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 65.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 46.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("enter", pkEnter);
	UILinkDelegate(pkEnter, OnEnter);

	m_pkEnterText = CCLabelTTF::create("", TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkEnter->addChild(m_pkEnterText, 5);

	UIPanel* pkCancel_n = UIPanel::createWithCltClbBtBbBl("alert_btn_1", 128.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkCancel_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 128.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkCancel = VE_NEW UIButton(pkCancel_n, pkCancel_p);
	pkCancel->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 65.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 46.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	m_pkCancelText = CCLabelTTF::create("", TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkCancel->addChild(m_pkCancelText, 5);

	m_pkTitleText = CCLabelTTF::create("", TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
	addChild(m_pkTitleText, 1);
	m_pkTitleText->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 48.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));

	m_pkContentText = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale),
		CCSize(252.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
	addChild(m_pkContentText, 1);
	m_pkContentText->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 30.0f * ui_main_scale * g_f32ScaleHD)));

	return UILayerModalExt<UIAlert>::init();
}

void UIAlert::OnActive(const UIUserDataPtr& spData)
{
	m_spCallback = VeDynamicCast(Callback, spData);
	if(m_spCallback)
		SetText(m_spCallback->m_kTitle, m_spCallback->m_kContent, m_spCallback->m_kEnter, m_spCallback->m_kCancel);
}

void UIAlert::OnDeactive(const UIUserDataPtr& spData)
{
	if(m_spCallback)
	{
		m_spCallback->OnCancel();
		m_spCallback = NULL;
	}
}

void UIAlert::SetText(const VeChar8* pcTitle, const VeChar8* pcContent, const VeChar8* pcEnter, const VeChar8* pcCancel)
{
	CCSize kSize;
	m_pkTitleText->setString(pcTitle);
	kSize = m_pkTitleText->getContentSize();
	m_pkTitleText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, 0.0f));
	
	m_pkContentText->setString(pcContent);
	kSize = m_pkContentText->getContentSize();
	m_pkContentText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, 1.0f));

	m_pkEnterText->setString(pcEnter);
	kSize = m_pkEnterText->getContentSize();
	m_pkEnterText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	
	m_pkCancelText->setString(pcCancel);
	kSize = m_pkCancelText->getContentSize();
	m_pkCancelText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

UIImplementDelegate(UIAlert, OnEnter, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(m_spCallback) m_spCallback->OnConfirm();
	}
}

UIImplementDelegate(UIAlert, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

VeImplementRTTI(UIAlert::Callback, UIUserData);

UIAlertConfirm::UIAlertConfirm() : UILayerModalExt<UIAlertConfirm>("alert_confirm")
{
	UIInitDelegate(UIAlertConfirm, OnEnter);
}

UIAlertConfirm::~UIAlertConfirm()
{
	term();
}

bool UIAlertConfirm::init()
{
	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 280.0f * ui_main_scale * g_f32ScaleHD, 156.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanel);
	m_pkPanel->setPosition(ui_ccp(0.5f, 0.5f));

	m_pkTitle = UIBarH::createWithEl("alert_title", 280.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD);
	m_pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(m_pkTitle);
	m_pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 78.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	UIPanel* pkEnter_n = UIPanel::createWithCltClbBtBbBl("alert_btn_0", 274.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkEnter_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 274.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkEnter = VE_NEW UIButton(pkEnter_n, pkEnter_p);
	pkEnter->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 46.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("enter", pkEnter);
	UILinkDelegate(pkEnter, OnEnter);

	m_pkEnterText = CCLabelTTF::create("", TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkEnter->addChild(m_pkEnterText, 5);

	m_pkTitleText = CCLabelTTF::create("", TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
	addChild(m_pkTitleText, 1);
	m_pkTitleText->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 48.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));

	m_pkContentText = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale),
		CCSize(252.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
	addChild(m_pkContentText, 1);
	m_pkContentText->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 30.0f * ui_main_scale * g_f32ScaleHD)));

	return UILayerModalExt<UIAlertConfirm>::init();
}

void UIAlertConfirm::OnActive(const UIUserDataPtr& spData)
{
	m_spCallback = VeDynamicCast(Callback, spData);
	if(m_spCallback)
		SetText(m_spCallback->m_kTitle, m_spCallback->m_kContent, m_spCallback->m_kEnter);
}

void UIAlertConfirm::OnDeactive(const UIUserDataPtr& spData)
{
	m_spCallback = NULL;
}

bool UIAlertConfirm::OnBack()
{
	if(m_spCallback) m_spCallback->OnConfirm();
	return false;
}

void UIAlertConfirm::SetText(const VeChar8* pcTitle, const VeChar8* pcContent, const VeChar8* pcEnter)
{
	CCSize kSize;
	m_pkTitleText->setString(pcTitle);
	kSize = m_pkTitleText->getContentSize();
	m_pkTitleText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, 0.0f));

	m_pkContentText->setString(pcContent);
	kSize = m_pkContentText->getContentSize();
	m_pkContentText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, 1.0f));

	m_pkEnterText->setString(pcEnter);
	kSize = m_pkEnterText->getContentSize();
	m_pkEnterText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

UIImplementDelegate(UIAlertConfirm, OnEnter, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(m_spCallback) m_spCallback->OnConfirm();
	}
}

VeImplementRTTI(UIAlertConfirm::Callback, UIUserData);

UILoading::UILoading() : UILayerModalExt<UILoading>("loading")
{
	
}

UILoading::~UILoading()
{
	term();
}

bool UILoading::init()
{
	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 220.0f * ui_main_scale * g_f32ScaleHD, 80.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanel);
	m_pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 133 * g_f32ScaleHD * ui_main_scale)));

	m_pkLoading = CCSprite::createWithSpriteFrameName("loading.png");
	m_pkLoading->setScale(ui_main_scale);
	m_pkLoading->setPositionX(88.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD);
	m_pkPanel->addChild(m_pkLoading);

	m_pkContentText = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale),
		CCSize(140.0f * ui_main_scale * g_f32ScaleHD, 56.0f * ui_main_scale * g_f32ScaleHD), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
	m_pkPanel->addChild(m_pkContentText);
	m_pkContentText->setPosition(ccp(-VeFloorf(100.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD), 0));

	return UILayerModalExt<UILoading>::init();
}

void UILoading::OnActive(const UIUserDataPtr& spData)
{
	if(spData)
	{
		SetText(VeDynamicCast(Data, spData)->m_kContent);
	}

	m_pkLoading->stopAllActions();
	m_pkLoading->setOpacity(0);

	m_pkLoading->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 360.0f)));
	m_pkLoading->runAction(CCFadeIn::create(0.2f));
}

bool UILoading::OnBack()
{
	return false;
}

void UILoading::SetText(const VeChar8* pcContent)
{
	CCSize kSize;
	m_pkContentText->setString(pcContent);
	kSize = m_pkContentText->getContentSize();
	m_pkContentText->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

VeImplementRTTI(UILoading::Data, UIUserData);

UINoticeBoard::UINoticeBoard() : UILayerModalExt<UINoticeBoard>("notice_board")
{
	UIInitDelegate(UINoticeBoard, OnCancel);
}

UINoticeBoard::~UINoticeBoard()
{
	term();
}

bool UINoticeBoard::init()
{
	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 280.0f * ui_main_scale * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanel);
	m_pkPanel->setPosition(ui_ccp(0.5f, 0.5f));

	m_pkTitle = UIBarH::createWithEl("alert_title", 280.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD);
	m_pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(m_pkTitle);
	m_pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 180.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	m_pkTitleText = CCLabelTTF::create("", TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
	addChild(m_pkTitleText, 1);
	m_pkTitleText->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 150.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));

	m_pkPanelInner = UIPanel::createWithCltBlBt("alert_board2", 274.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD, 260.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanelInner);
	m_pkPanelInner->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkPanelInner->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 136.0f * ui_main_scale * g_f32ScaleHD)));

	m_pkContentText = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale),
		CCSize(268.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD, 256.0f * ui_main_scale * g_f32ScaleHD), kCCTextAlignmentLeft);
	addChild(m_pkContentText, 1);
	m_pkContentText->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 134.0f * ui_main_scale * g_f32ScaleHD)));

	UIPanel* pkCancel_n = UIPanel::createWithCltClbBtBbBl("alert_btn_1", 274.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkCancel_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 274.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkCancel = VE_NEW UIButton(pkCancel_n, pkCancel_p);
	pkCancel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 156.0f * ui_main_scale * g_f32ScaleHD + 8.0f * g_f32ScaleHD)));
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	m_pkCancelText = CCLabelTTF::create("", TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkCancel->addChild(m_pkCancelText, 5);

	return UILayerModalExt<UINoticeBoard>::init();
}

void UINoticeBoard::OnActive(const UIUserDataPtr& spData)
{
	m_spCallback = VeDynamicCast(Callback, spData);
	if(m_spCallback)
		SetText(m_spCallback->m_kTitle, m_spCallback->m_kContent, m_spCallback->m_kCancel);
}

void UINoticeBoard::OnDeactive(const UIUserDataPtr& spData)
{
	m_spCallback = NULL;
}

bool UINoticeBoard::OnBack()
{
	if(m_spCallback) m_spCallback->OnClose();
	return true;
}

void UINoticeBoard::SetText(const VeChar8* pcTitle, const VeChar8* pcContent, const VeChar8* pcCancel)
{
	CCSize kSize;
	m_pkTitleText->setString(pcTitle);
	kSize = m_pkTitleText->getContentSize();
	m_pkTitleText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, 0));

	m_pkCancelText->setString(pcCancel);
	kSize = m_pkCancelText->getContentSize();
	m_pkCancelText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkContentText->setString(pcContent);
	kSize = m_pkContentText->getContentSize();
	m_pkContentText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, 1.0f));
}

UIImplementDelegate(UINoticeBoard, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

VeImplementRTTI(UINoticeBoard::Callback, UIUserData);

UIToast::UIToast() : UILayerExt<UIToast>("toast")
{

}

UIToast::~UIToast()
{
	term();
}

bool UIToast::init()
{
	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 0 * ui_main_scale * g_f32ScaleHD, 40 * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanel);
	m_pkPanel->setPosition(ui_ccp(0.5f, 0.3f));

	m_pkContentText = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	m_pkPanel->addChild(m_pkContentText, 1);

	return UILayerExt<UIToast>::init();
}

void UIToast::OnActive(const UIUserDataPtr& spData)
{
	m_spCallback = VeDynamicCast(Callback, spData);
	if(m_spCallback)
		SetText(m_spCallback->m_kContent);

	m_f32Blend = 0.0f;
	UpdateBlend();
	/*VeFloat32 f32FadeIn(0.2f), f32Delay(2.0f), f32FadeOut(0.2f);
	m_pkContentText->setOpacity(0);
	m_pkContentText->runAction(CCSequence::create(CCFadeIn::create(f32FadeIn), CCDelayTime::create(f32Delay), CCFadeOut::create(f32FadeOut), NULL));
	m_pkPanel->Show(f32FadeIn, f32Delay, f32FadeOut);*/
	schedule(schedule_selector(UIToast::Update), 0.0f);
}

void UIToast::OnDeactive(const UIUserDataPtr& spData)
{
	if(m_spCallback)
	{
		m_spCallback->OnClose();
	}
	m_spCallback = NULL;
}

void UIToast::SetText(const VeChar8* pcContent)
{
	CCSize kSize;
	m_pkContentText->setString(pcContent);
	kSize = m_pkContentText->getContentSize();
	m_pkContentText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkPanel->SetSize(40 * ui_main_scale * g_f32ScaleHD + kSize.width, 40 * ui_main_scale * g_f32ScaleHD);
	m_f32ShowTime = 1.0f;
}

void UIToast::Update(VeFloat32 f32Delta)
{
	if(m_f32ShowTime > 0.0f)
	{
		if(m_f32Blend < 1.0f)
		{
			m_f32Blend += f32Delta * 5.0f;
			if(m_f32Blend >= 1.0f)
			{
				m_f32Blend = 1.0f;
			}
		}
		if(m_f32Blend >= 1.0f)
		{
			m_f32ShowTime -= f32Delta;
		}
	}
	if(m_f32ShowTime <= 0.0f)
	{
		m_f32ShowTime = 0.0f;
		if(m_f32Blend > 0.0f)
		{
			m_f32Blend -= f32Delta * 5.0f;
			if(m_f32Blend <= 0.0f)
			{
				m_f32Blend = 0.0f;
				unscheduleAllSelectors();
				g_pLayerManager->PopTopLayerAndDeavtive(m_spCallback);
			}
		}
	}
	UpdateBlend();
}

void UIToast::UpdateBlend()
{
	VeUInt8 u8Blend = m_f32Blend * 255.0f;
	m_pkContentText->setOpacity(u8Blend);
	m_pkPanel->setOpacity(u8Blend);
}

VeImplementRTTI(UIToast::Callback, UIUserData);

