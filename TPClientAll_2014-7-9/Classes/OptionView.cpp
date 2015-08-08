#include "OptionView.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "Knickknacks.h"
#include "SoundSystem.h"
#include "EventList.h"
#include "TexasPokerHallPlayerC.h"

USING_NS_CC;

OptionView::OptionView()
{
	UIInitDelegate(OptionView, OnPlayerInfo);
	UIInitDelegate(OptionView, OnEnvSetup);
	UIInitDelegate(OptionView, OnGameHelp);
	UIInitDelegate(OptionView, OnBugFeedback);
	UIInitDelegate(OptionView, OnBack);
	UIInitDelegate(OptionView, OnEditInfo);
	UIInitDelegate(OptionView, OnMusicVolume);
	UIInitDelegate(OptionView, OnSoundVolume);
	UIInitDelegate(OptionView, OnSilenceCheck);
	UIInitDelegate(OptionView, OnLeftEvent);
	UIInitDelegate(OptionView, OnMidEvent);
	UIInitDelegate(OptionView, OnRightEvent);
	UIInitDelegate(OptionView, OnLowLeftEvent);
	UIInitDelegate(OptionView, OnLowMidEvent);
	UIInitDelegate(OptionView, OnLowRightEvent);

	UIPage* pkRoot = VE_NEW UIPage;
	UIPage* pkContent = VE_NEW UIPage;
	AddPage(pkRoot);
	AddPage(pkContent);
	m_pkContentView = VE_NEW UIPageView;
	pkContent->AddChild(m_pkContentView);
	UIPage* pkPlayerInfo = VE_NEW UIPage;
	UIPage* pkEnvSetup = VE_NEW UIPage;
	UIPage* pkGameHelp = VE_NEW UIPage;
	m_pkContentView->AddPage(pkPlayerInfo);
	m_pkContentView->AddPage(pkEnvSetup);
	m_pkContentView->AddPage(pkGameHelp);
	CreateRoot(pkRoot);
	CreateContent(pkContent);
	CreateInfo(pkPlayerInfo);
	CreateSetup(pkEnvSetup);
	CreateHelp(pkGameHelp);
	Change(0);
}

OptionView::~OptionView()
{

}

void OptionView::Back()
{
	Change(0);
}

bool OptionView::OnBack()
{
	if(GetCurrent())
	{
		Back();
		return false;
	}
	else
	{
		return true;
	}
}

void OptionView::CreateRoot(UIWidget* pkParent)
{
	CCSprite* pkOptionFrameL = CCSprite::createWithSpriteFrameName("option_frame.png");
	pkOptionFrameL->setAnchorPoint(ccp(1.0f, 0.0f));
	pkOptionFrameL->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 1.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 124.0f * g_f32ScaleHD * ui_main_scale)));
	pkOptionFrameL->setScale(ui_main_scale);
	pkParent->addChild(pkOptionFrameL, 0);

	CCSprite* pkOptionFrameR = CCSprite::createWithSpriteFrameName("option_frame.png");
	pkOptionFrameR->setAnchorPoint(ccp(1.0f, 0.0f));
	pkOptionFrameR->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 1.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 126.0f * g_f32ScaleHD * ui_main_scale)));
	pkOptionFrameR->setScale(-ui_main_scale);
	pkParent->addChild(pkOptionFrameR, 0);

	UIButton* pkPlayerInfo = VE_NEW UIButton("player_info_n.png", "player_info_p.png");
	pkPlayerInfo->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 175.0f * g_f32ScaleHD * ui_main_scale)));
	pkPlayerInfo->setScale(ui_main_scale);
	pkParent->AddChild(pkPlayerInfo);

	UIButton* pkEnvSetup = VE_NEW UIButton("evn_setup_n.png", "evn_setup_p.png");
	pkEnvSetup->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 175.0f * g_f32ScaleHD * ui_main_scale)));
	pkEnvSetup->setScale(ui_main_scale);
	pkParent->AddChild(pkEnvSetup);

	UIButton* pkGameHelp = VE_NEW UIButton("game_help_n.png", "game_help_p.png");
	pkGameHelp->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 75.0f * g_f32ScaleHD * ui_main_scale)));
	pkGameHelp->setScale(ui_main_scale);
	pkParent->AddChild(pkGameHelp);

	UIButton* pkBugFeedBack = VE_NEW UIButton("bug_feedback_n.png", "bug_feedback_p.png");
	pkBugFeedBack->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 75.0f * g_f32ScaleHD * ui_main_scale)));
	pkBugFeedBack->setScale(ui_main_scale);
	pkParent->AddChild(pkBugFeedBack);

	UILinkDelegate(pkPlayerInfo, OnPlayerInfo);
	UILinkDelegate(pkEnvSetup, OnEnvSetup);
	UILinkDelegate(pkGameHelp, OnGameHelp);
	UILinkDelegate(pkBugFeedBack, OnBugFeedback);
}

void OptionView::CreateContent(UIWidget* pkParent)
{
	CCSprite* pkBarL = CCSprite::createWithSpriteFrameName("option_title_bar.png");
	pkBarL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarL->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 220.0f * g_f32ScaleHD * ui_main_scale)));
	pkBarL->setScale(ui_main_scale);
	CCSprite* pkBarR = CCSprite::createWithSpriteFrameName("option_title_bar.png");
	pkBarR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarR->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 220.0f * g_f32ScaleHD * ui_main_scale)));
	pkBarR->setScaleX(-ui_main_scale);
	pkBarR->setScaleY(ui_main_scale);
	m_pkContentView->addChild(pkBarL);
	m_pkContentView->addChild(pkBarR);

	UIButton* pkBack = VE_NEW UIButton("back_n.png", "back_p.png");
	pkBack->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 136.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 220.0f * g_f32ScaleHD * ui_main_scale)));
	pkBack->setScale(ui_main_scale);
	CCSize kSize = pkBack->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	pkBack->setContentSize(kSize);
	pkParent->AddChild(pkBack);
	UILinkDelegate(pkBack, OnBack);
}

void OptionView::CreateInfo(UIWidget* pkParent)
{
	VeChar8 acBuffer[64];

	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("player_info.png");
	pkTitle->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 221.0f * g_f32ScaleHD * ui_main_scale)));
	pkTitle->setScale(ui_main_scale);
	pkParent->addChild(pkTitle, 0);

	m_pkHeadArea = VE_NEW HeadArea;
	m_pkHeadArea->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 106.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 168.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->AddChild(m_pkHeadArea);

	UIButton* pkEditInfo = VE_NEW UIButton("edit_info_n.png", "edit_info_p.png");
	pkEditInfo->setScale(ui_main_scale);
	pkEditInfo->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 106.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 120.0f * g_f32ScaleHD * ui_main_scale)));
	{
		CCSize kSize = pkEditInfo->getContentSize();
		kSize.height *= 2.0f;
		pkEditInfo->setContentSize(kSize);
	}
	pkParent->AddChild(pkEditInfo);
	UILinkDelegate(pkEditInfo, OnEditInfo);

	CCSize kSize;
	CCLabelTTF* pkNameText = CCLabelTTF::create(TT("Nickname"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkNameText->getContentSize();
	pkNameText->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkNameText->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 190.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkNameText);
	pkNameText->setColor(ccc3(150, 150, 150));

	m_pkName = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkName->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 20.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 190.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkName);

	CCLabelTTF* pkTitleText = CCLabelTTF::create(TT("Title"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkTitleText->getContentSize();
	pkTitleText->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkTitleText->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 168.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkTitleText);
	pkTitleText->setColor(ccc3(150, 150, 150));

	m_pkTitle = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkTitle->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 20.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 168.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkTitle);

	m_pkVipCard = CCSprite::createWithSpriteFrameName("vip_card.png");
	m_pkVipCard->setAnchorPoint(ccp(0, 0.5f));
	m_pkVipCard->setScale(ui_main_scale);
	m_pkVipCard->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 64.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 190.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkVipCard);

	CCSprite* pkBankCard = CCSprite::createWithSpriteFrameName("bank_card.png");
	pkBankCard->setAnchorPoint(ccp(0, 0.5f));
	pkBankCard->setScale(ui_main_scale);
	pkBankCard->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 64.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 168.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkBankCard);

	m_pkBankDesc = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkBankDesc->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 94.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 168.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkBankDesc);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chips.png");
	pkChips->setAnchorPoint(ccp(0, 0.5f));
	pkChips->setScale(ui_main_scale);
	pkChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 140.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkChips);

	CCSprite* pkGold = CCSprite::createWithSpriteFrameName("gold.png");
	pkGold->setAnchorPoint(ccp(0, 0.5f));
	pkGold->setScale(ui_main_scale);
	pkGold->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 64.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 140.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkGold);

	m_pkChips = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 50.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 140.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkChips);

	m_pkGold = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkGold->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 146.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 140.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkGold);

	for(VeUInt32 i(0); i < 3; ++i)
	{
		VeSprintf(acBuffer, 64, "medal_%d.png", i);
		CCSprite* pkMedal = CCSprite::createWithSpriteFrameName(acBuffer);
		pkMedal->setScale(ui_main_scale);
		pkMedal->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (22.0f - 30.0f * i) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 106.0f * g_f32ScaleHD * ui_main_scale)));
		pkParent->addChild(pkMedal);

		m_apkMedal[i] = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
		m_apkMedal[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (22.0f - 30.0f * i) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 84.0f * g_f32ScaleHD * ui_main_scale)));
		pkParent->addChild(m_apkMedal[i]);
	}
	SetMedal(0, 0, 0);

	CCLabelTTF* pkAssets = CCLabelTTF::create(TT("Assets"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkAssets->getContentSize();
	pkAssets->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkAssets->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 144.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 86.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkAssets);
	pkAssets->setColor(ccc3(150, 150, 150));
	for(VeUInt32 i(0); i < 4; ++i)
	{
		VeSprintf(acBuffer, 64, "object_%d.png", i);
		CCSprite* pkObject = CCSprite::createWithSpriteFrameName(acBuffer);
		pkObject->setScale(ui_main_scale);
		pkObject->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (90.0f * (i & 1) - 130.0f) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + (61.0f - 28.0f * ((i & 2) >> 1)) * g_f32ScaleHD * ui_main_scale)));
		pkParent->addChild(pkObject);

		m_apkObject[i] = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
		m_apkObject[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (90.0f * (i & 1) - 60.0f) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + (61.0f - 28.0f * ((i & 2) >> 1)) * g_f32ScaleHD * ui_main_scale)));
		pkParent->addChild(m_apkObject[i]);
	}

	CCLabelTTF* pkWinGames = CCLabelTTF::create(TT("WinGames"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkWinGames->getContentSize();
	pkWinGames->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkWinGames->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkWinGames);
	pkWinGames->setColor(ccc3(150, 150, 150));

	CCLabelTTF* pkMaxWinBet = CCLabelTTF::create(TT("MaxWinBet"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkMaxWinBet->getContentSize();
	pkMaxWinBet->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkMaxWinBet->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 100.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkMaxWinBet);
	pkMaxWinBet->setColor(ccc3(150, 150, 150));

	CCLabelTTF* pkWinRate = CCLabelTTF::create(TT("WinRate"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkWinRate->getContentSize();
	pkWinRate->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkWinRate->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 84.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkWinRate);
	pkWinRate->setColor(ccc3(150, 150, 150));

	CCLabelTTF* pkBestType = CCLabelTTF::create(TT("BestType"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkBestType->getContentSize();
	pkBestType->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkBestType->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 68.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(pkBestType);
	pkBestType->setColor(ccc3(150, 150, 150));

	m_pkWinGames = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkWinGames->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 146.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkWinGames);

	m_pkMaxWinBet = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkMaxWinBet->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 146.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 100.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkMaxWinBet);

	m_pkBestType = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkBestType->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 146.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 84.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->addChild(m_pkBestType);

	for(VeUInt32 i(0); i < 5; ++i)
	{
		m_apkCardType[i] = VE_NEW MiniCard;
		m_apkCardType[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (54.0f + i * 20) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 38.0f * g_f32ScaleHD * ui_main_scale)));
		pkParent->AddChild(m_apkCardType[i]);
	}

	/*SetName(g_pkClientData->GetUserName());
	SetTitle(g_pkClientData->GetTitle());
	SetBankDesc(g_pkClientData->GetBankCardDec());
	SetChipsGold(g_pkClientData->GetChips(), g_pkClientData->GetGoldbar());
	SetMedal(g_pkClientData->GetMedalGold(), g_pkClientData->GetMedalSliver(), g_pkClientData->GetMedalCopper());
	SetObject(g_pkClientData->GetObjectDiamond(), g_pkClientData->GetObjectAntique(), g_pkClientData->GetObjectCar(), g_pkClientData->GetObjectPlane());
	SetGameData(g_pkClientData->GetWinCount(), g_pkClientData->GetMaxWinBet(), g_pkClientData->GetWinRate());

	SetBestType(g_pkClientData->kType, g_pkClientData->IsVip());*/
}

void OptionView::SetName(const VeChar8* pcName)
{
	m_pkName->setString(pcName);
	const CCSize& kSize = m_pkName->getContentSize();
	m_pkName->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void OptionView::SetTitle(const VeChar8* pcTitle)
{
	m_pkTitle->setString(pcTitle);
	const CCSize& kSize = m_pkTitle->getContentSize();
	m_pkTitle->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void OptionView::SetBankDesc(const VeChar8* pkBankDesc)
{
	m_pkBankDesc->setString(pkBankDesc);
	const CCSize& kSize = m_pkBankDesc->getContentSize();
	m_pkBankDesc->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void OptionView::SetChipsGold(VeUInt32 u32Chips, VeUInt32 u32Gold)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "%d", u32Chips);
	m_pkChips->setString(acBuffer);
	{
		const CCSize& kSize = m_pkChips->getContentSize();
		m_pkChips->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	VeSprintf(acBuffer, 64, "%d", u32Gold);
	m_pkGold->setString(acBuffer);
	{
		const CCSize& kSize = m_pkGold->getContentSize();
		m_pkGold->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
}

void OptionView::SetMedal(VeUInt32 u32Gold, VeUInt32 u32Sliver, VeUInt32 u32Copper)
{
	VeUInt32 au32Number[3] = { u32Copper, u32Sliver, u32Gold };
	VeChar8 acBuffer[64];

	for(VeUInt32 i(0); i < 3; ++i)
	{
		VeSprintf(acBuffer, 64, "%d", au32Number[i]);
		m_apkMedal[i]->setString(acBuffer);
		const CCSize& kSize = m_apkMedal[i]->getContentSize();
		m_apkMedal[i]->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
}

void OptionView::SetObject(VeUInt32 u32Diamond, VeUInt32 u32Antique, VeUInt32 u32Car, VeUInt32 u32Plane)
{
	VeUInt32 au32Number[4] = { u32Diamond, u32Antique, u32Car, u32Plane };
	VeChar8 acBuffer[64];

	for(VeUInt32 i(0); i < 4; ++i)
	{
		VeSprintf(acBuffer, 64, "%d", au32Number[i]);
		m_apkObject[i]->setString(acBuffer);
		const CCSize& kSize = m_apkObject[i]->getContentSize();
		m_apkObject[i]->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
}

void OptionView::SetGameData(VeUInt32 u32WinGames, VeUInt32 u32MaxWinBet, VeFloat32 f32WinRate)
{
	VeChar8 acBuffer[64];
	m_pkWinGames->setString(NTL(u32WinGames, 7));
	{
		const CCSize& kSize = m_pkWinGames->getContentSize();
		m_pkWinGames->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	m_pkMaxWinBet->setString(NTL(u32MaxWinBet, 7));
	{
		const CCSize& kSize = m_pkMaxWinBet->getContentSize();
		m_pkMaxWinBet->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	VeSprintf(acBuffer, 64, "%.2f%%", f32WinRate * 100.0f);
	m_pkBestType->setString(acBuffer);
	{
		const CCSize& kSize = m_pkBestType->getContentSize();
		m_pkBestType->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
}

void OptionView::SetBestType(const CardType& kType, bool bVip)
{
	VE_ASSERT(kType.Size() == 5);
	for(VeUInt32 i(0); i < 5; ++i)
	{
		const VePair<CardNum,CardSuit>& kCard = kType[i];
		if(kCard.m_tFirst < CardNum_MAX)
		{
			VE_ASSERT(kCard.m_tSecond < CardSuit_MAX);
			m_apkCardType[i]->SetContent(kCard.m_tFirst, kCard.m_tSecond);
			m_apkCardType[i]->SetAngle(0);
			m_apkCardType[i]->setZOrder(i);
		}
		else
		{
			m_apkCardType[i]->SetAngle(VE_MATH_PI_F);
			m_apkCardType[i]->setZOrder(5 - i);
		}
		
		m_apkCardType[i]->SetType(bVip ? 1 : 0);
	}
}

void OptionView::CreateSetup(UIWidget* pkParent)
{
	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("evn_setup.png");
	pkTitle->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 221.0f * g_f32ScaleHD * ui_main_scale)));
	pkTitle->setScale(ui_main_scale);
	pkParent->addChild(pkTitle, 0);

	CCSprite* pkTextMusic = CCSprite::createWithSpriteFrameName("text_music.png");
	pkTextMusic->setAnchorPoint(ccp(0.0f, 0.5f));
	pkTextMusic->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 92.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 166.0f * g_f32ScaleHD * ui_main_scale)));
	pkTextMusic->setScale(ui_main_scale);
	pkParent->addChild(pkTextMusic, 0);

	CCSprite* pkTextSound = CCSprite::createWithSpriteFrameName("text_sound.png");
	pkTextSound->setAnchorPoint(ccp(0.0f, 0.5f));
	pkTextSound->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 92.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 118.0f * g_f32ScaleHD * ui_main_scale)));
	pkTextSound->setScale(ui_main_scale);
	pkParent->addChild(pkTextSound, 0);

	CCSprite* pkTextSilence = CCSprite::createWithSpriteFrameName("text_silence.png");
	pkTextSilence->setAnchorPoint(ccp(0.0f, 0.5f));
	pkTextSilence->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 92.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 70.0f * g_f32ScaleHD * ui_main_scale)));
	pkTextSilence->setScale(ui_main_scale);
	pkParent->addChild(pkTextSilence, 0);

	Slider* pkMusic = VE_NEW Slider;
	pkMusic->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 166.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->AddChild(pkMusic);

	Slider* pkSound = VE_NEW Slider;
	pkSound->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 118.0f * g_f32ScaleHD * ui_main_scale)));
	pkParent->AddChild(pkSound);

	UICheckBox* pkSilence = VE_NEW UICheckBox("check_n.png", "check_p.png");
	pkSilence->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 40.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 70.0f * g_f32ScaleHD * ui_main_scale)));
	pkSilence->setScale(ui_main_scale);
	pkParent->AddChild(pkSilence);

	CCSize kSize = pkMusic->getContentSize();
	kSize.height *= 2.0f;
	pkMusic->setContentSize(kSize);
	kSize = pkSound->getContentSize();
	kSize.height *= 2.0f;
	pkSound->setContentSize(kSize);
	kSize = pkSilence->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	pkSilence->setContentSize(kSize);

	pkMusic->SetValue(g_pSaveData->ReadParam("music_volume", 1.0f));
	pkSound->SetValue(g_pSaveData->ReadParam("sound_volume", 1.0f));
	pkSilence->SetCheck(g_pSaveData->ReadParam("sound_switch", VeUInt32(0)) ? true : false);

	UILinkDelegate(pkMusic, OnMusicVolume);
	UILinkDelegate(pkSound, OnSoundVolume);
	UILinkDelegate(pkSilence, OnSilenceCheck);
}

void OptionView::CreateHelp(UIWidget* pkParent)
{
	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("game_help.png");
	pkTitle->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 221.0f * g_f32ScaleHD * ui_main_scale)));
	pkTitle->setScale(ui_main_scale);
	pkParent->addChild(pkTitle, 0);

	pkHelpScrollView = VE_NEW UIScrollView(320.0f * ui_main_scale * g_f32ScaleHD, 231.0f * ui_main_scale * g_f32ScaleHD,
		"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale);
	pkHelpScrollView->ScrollToTop();
	pkHelpScrollView->setPosition(ccp(ui_w(0.5f),VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 240 * g_f32ScaleHD * ui_main_scale)));
	pkHelpScrollView->setAnchorPoint(ccp(0.5f,1.0f));
	pkHelpScrollView->SetScrollBarColor(ccc3(56, 79, 124));
	pkHelpScrollView->SetScrollBarOpacity(128);
	pkHelpScrollView->UpdateBounding();
	pkParent->AddChild(pkHelpScrollView,3);

	{
		pkNormalLeft = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
		pkNormalLeft->setScaleX(-1);
		pkNormalLeft->setScaleY(-1);
		pkNormalLeft->SetMiddleVisible(false);

		pkPressedLeft = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
		pkPressedLeft->SetMiddleVisible(false);

		pkEventLeft = VE_NEW UIButton(pkNormalLeft, pkPressedLeft);
		pkEventLeft->setScale(ui_main_scale);
		pkEventLeft->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 100.0f * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
		pkParent->AddChild(pkEventLeft);

		pkHelpNodeL = CCNode::create();
		pkEventLeft->addChild(pkHelpNodeL);

		CCSprite* pkHelpFrame = CCSprite::createWithSpriteFrameName("help_frame.png");
		pkHelpFrame->setPositionY(-20.0f * g_f32ScaleHD);
		pkHelpNodeL->addChild(pkHelpFrame, 3);
		CCSprite* pkGameLogoMini = CCSprite::createWithSpriteFrameName("game_logo_mini.png");
		pkGameLogoMini->setPositionY(-16.0f * g_f32ScaleHD);
		pkGameLogoMini->setScale(0.91f);
		pkHelpNodeL->addChild(pkGameLogoMini, 3);
		CCSprite* pkHelpGameRule = CCSprite::createWithSpriteFrameName("help_game_rule.png");
		pkHelpGameRule->setPositionX(10.0f * g_f32ScaleHD);
		pkHelpGameRule->setPositionY(36.0f * g_f32ScaleHD);
		pkHelpNodeL->addChild(pkHelpGameRule, 3);
		CCSprite* pkHelpArrows = CCSprite::createWithSpriteFrameName("help_arrows.png");
		pkHelpArrows->setPositionX(-30.0f * g_f32ScaleHD);
		pkHelpArrows->setPositionY(37.0f * g_f32ScaleHD);
		pkHelpNodeL->addChild(pkHelpArrows, 3);
		UILinkDelegate(pkEventLeft, OnLeftEvent);
	}

	{
		pkNormalMid = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
		pkNormalMid->setScaleX(-1);
		pkNormalMid->setScaleY(-1);
		pkNormalMid->SetMiddleVisible(false);

		pkPressedMid = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
		pkPressedMid->SetMiddleVisible(false);

		pkEventMid = VE_NEW UIButton(pkNormalMid, pkPressedMid);
		pkEventMid->setScale(ui_main_scale);
		pkEventMid->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
		pkParent->AddChild(pkEventMid);

		pkHelpNodeM = CCNode::create();
		pkEventMid->addChild(pkHelpNodeM);

		CCSprite* pkHelpFrame = CCSprite::createWithSpriteFrameName("help_frame.png");
		pkHelpFrame->setPositionY(-20.0f * g_f32ScaleHD);
		pkHelpNodeM->addChild(pkHelpFrame, 3);
		CCSprite* pkLottoLogoMini = CCSprite::createWithSpriteFrameName("nini_lotto_n.png");
		pkLottoLogoMini->setPositionY(-16.0f * g_f32ScaleHD);
		pkLottoLogoMini->setScale(0.91f);
		pkHelpNodeM->addChild(pkLottoLogoMini, 3);
		CCSprite* pkHelpGameRule = CCSprite::createWithSpriteFrameName("help_game_rule.png");
		pkHelpGameRule->setPositionX(10.0f * g_f32ScaleHD);
		pkHelpGameRule->setPositionY(36.0f * g_f32ScaleHD);
		pkHelpNodeM->addChild(pkHelpGameRule, 3);
		CCSprite* pkHelpArrows = CCSprite::createWithSpriteFrameName("help_arrows.png");
		pkHelpArrows->setPositionX(-30.0f * g_f32ScaleHD);
		pkHelpArrows->setPositionY(37.0f * g_f32ScaleHD);
		pkHelpNodeM->addChild(pkHelpArrows, 3);
		UILinkDelegate(pkEventMid, OnMidEvent);
	}

	{
		pkNormalRight = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
		pkNormalRight->setScaleX(-1);
		pkNormalRight->setScaleY(-1);
		pkNormalRight->SetMiddleVisible(false);

		pkPressedRight = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
		pkPressedRight->SetMiddleVisible(false);

		pkEventRight = VE_NEW UIButton(pkNormalRight, pkPressedRight);
		pkEventRight->setScale(ui_main_scale);
		pkEventRight->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 100.0f * g_f32ScaleHD * ui_main_scale),
			VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
		pkParent->AddChild(pkEventRight);

		pkHelpNodeR = CCNode::create();
		pkEventRight->addChild(pkHelpNodeR);

		CCSprite* pkHelpFrame = CCSprite::createWithSpriteFrameName("help_frame.png");
		pkHelpFrame->setPositionY(-20.0f * g_f32ScaleHD);
		pkHelpNodeR->addChild(pkHelpFrame, 3);
		CCSprite* pkBaccLogoMini = CCSprite::createWithSpriteFrameName("mini_baccarat_n.png");
		pkBaccLogoMini->setPositionY(-16.0f * g_f32ScaleHD);
		pkBaccLogoMini->setScale(0.91f);
		pkHelpNodeR->addChild(pkBaccLogoMini, 3);
		CCSprite* pkHelpGameRule = CCSprite::createWithSpriteFrameName("help_game_rule.png");
		pkHelpGameRule->setPositionX(10.0f * g_f32ScaleHD);
		pkHelpGameRule->setPositionY(36.0f * g_f32ScaleHD);
		pkHelpNodeR->addChild(pkHelpGameRule, 3);
		CCSprite* pkHelpArrows = CCSprite::createWithSpriteFrameName("help_arrows.png");
		pkHelpArrows->setPositionX(-30.0f * g_f32ScaleHD);
		pkHelpArrows->setPositionY(37.0f * g_f32ScaleHD);
		pkHelpNodeR->addChild(pkHelpArrows, 3);
		UILinkDelegate(pkEventRight, OnRightEvent);
	}

	//{
	//	pkNormalLowLeft = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
	//	pkNormalLowLeft->setScaleX(-1);
	//	pkNormalLowLeft->setScaleY(-1);
	//	pkNormalLowLeft->SetMiddleVisible(false);

	//	pkPressedLowLeft = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
	//	pkPressedLowLeft->SetMiddleVisible(false);

	//	pkEventLowLeft = VE_NEW UIButton(pkNormalLowLeft, pkPressedLowLeft);
	//	pkEventLowLeft->setScale(ui_main_scale);
	//	pkEventLowLeft->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 100.0f * g_f32ScaleHD * ui_main_scale),
	//		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
	//	pkParent->AddChild(pkEventLowLeft);

	//	pkLowHelpNodeL = CCNode::create();
	//	pkEventLowLeft->addChild(pkLowHelpNodeL);

	//	CCSprite* pkHelpFrame = CCSprite::createWithSpriteFrameName("help_frame.png");
	//	pkHelpFrame->setPositionY(-20.0f * g_f32ScaleHD);
	//	pkLowHelpNodeL->addChild(pkHelpFrame, 3);
	//	CCSprite* pkGameLogoMini = CCSprite::createWithSpriteFrameName("game_logo_mini.png");
	//	pkGameLogoMini->setPositionY(-16.0f * g_f32ScaleHD);
	//	pkGameLogoMini->setScale(0.91f);
	//	pkLowHelpNodeL->addChild(pkGameLogoMini, 3);
	//	CCSprite* pkHelpGameRule = CCSprite::createWithSpriteFrameName("help_game_rule.png");
	//	pkHelpGameRule->setPositionX(10.0f * g_f32ScaleHD);
	//	pkHelpGameRule->setPositionY(36.0f * g_f32ScaleHD);
	//	pkLowHelpNodeL->addChild(pkHelpGameRule, 3);
	//	CCSprite* pkHelpArrows = CCSprite::createWithSpriteFrameName("help_arrows.png");
	//	pkHelpArrows->setPositionX(-30.0f * g_f32ScaleHD);
	//	pkHelpArrows->setPositionY(37.0f * g_f32ScaleHD);
	//	pkLowHelpNodeL->addChild(pkHelpArrows, 3);
	//	UILinkDelegate(pkEventLowLeft, OnLowLeftEvent);
	//}

	//{
	//	pkNormalLowMid = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
	//	pkNormalLowMid->setScaleX(-1);
	//	pkNormalLowMid->setScaleY(-1);
	//	pkNormalLowMid->SetMiddleVisible(false);

	//	pkPressedLowMid = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
	//	pkPressedLowMid->SetMiddleVisible(false);

	//	pkEventLowMid = VE_NEW UIButton(pkNormalLowMid, pkPressedLowMid);
	//	pkEventLowMid->setScale(ui_main_scale);
	//	pkEventLowMid->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 100.0f * g_f32ScaleHD * ui_main_scale),
	//		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
	//	pkParent->AddChild(pkEventLowMid);

	//	pkLowHelpNodeM = CCNode::create();
	//	pkEventLowMid->addChild(pkLowHelpNodeM);

	//	CCSprite* pkHelpFrame = CCSprite::createWithSpriteFrameName("help_frame.png");
	//	pkHelpFrame->setPositionY(-20.0f * g_f32ScaleHD);
	//	pkLowHelpNodeM->addChild(pkHelpFrame, 3);
	//	CCSprite* pkGameLogoMini = CCSprite::createWithSpriteFrameName("game_logo_mini.png");
	//	pkGameLogoMini->setPositionY(-16.0f * g_f32ScaleHD);
	//	pkGameLogoMini->setScale(0.91f);
	//	pkLowHelpNodeM->addChild(pkGameLogoMini, 3);
	//	CCSprite* pkHelpGameRule = CCSprite::createWithSpriteFrameName("help_game_rule.png");
	//	pkHelpGameRule->setPositionX(10.0f * g_f32ScaleHD);
	//	pkHelpGameRule->setPositionY(36.0f * g_f32ScaleHD);
	//	pkLowHelpNodeM->addChild(pkHelpGameRule, 3);
	//	CCSprite* pkHelpArrows = CCSprite::createWithSpriteFrameName("help_arrows.png");
	//	pkHelpArrows->setPositionX(-30.0f * g_f32ScaleHD);
	//	pkHelpArrows->setPositionY(37.0f * g_f32ScaleHD);
	//	pkLowHelpNodeM->addChild(pkHelpArrows, 3);
	//	UILinkDelegate(pkEventLowLeft, OnLowMidEvent);
	//}

	//{
	//	pkNormalLowRight = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
	//	pkNormalLowRight->setScaleX(-1);
	//	pkNormalLowRight->setScaleY(-1);
	//	pkNormalLowRight->SetMiddleVisible(false);

	//	pkPressedLowRight = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 93 * g_f32ScaleHD, 106 * g_f32ScaleHD);
	//	pkPressedLowRight->SetMiddleVisible(false);

	//	pkEventLowRight = VE_NEW UIButton(pkNormalLowRight, pkPressedLowRight);
	//	pkEventLowRight->setScale(ui_main_scale);
	//	pkEventLowRight->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 100.0f * g_f32ScaleHD * ui_main_scale),
	//		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 116.0f * g_f32ScaleHD * ui_main_scale)));
	//	pkParent->AddChild(pkEventLowRight);

	//	pkLowHelpNodeR = CCNode::create();
	//	pkEventLowRight->addChild(pkLowHelpNodeR);

	//	CCSprite* pkHelpFrame = CCSprite::createWithSpriteFrameName("help_frame.png");
	//	pkHelpFrame->setPositionY(-20.0f * g_f32ScaleHD);
	//	pkEventLowRight->addChild(pkHelpFrame, 3);
	//	CCSprite* pkGameLogoMini = CCSprite::createWithSpriteFrameName("game_logo_mini.png");
	//	pkGameLogoMini->setPositionY(-16.0f * g_f32ScaleHD);
	//	pkGameLogoMini->setScale(0.91f);
	//	pkEventLowRight->addChild(pkGameLogoMini, 3);
	//	CCSprite* pkHelpGameRule = CCSprite::createWithSpriteFrameName("help_game_rule.png");
	//	pkHelpGameRule->setPositionX(10.0f * g_f32ScaleHD);
	//	pkHelpGameRule->setPositionY(36.0f * g_f32ScaleHD);
	//	pkEventLowRight->addChild(pkHelpGameRule, 3);
	//	CCSprite* pkHelpArrows = CCSprite::createWithSpriteFrameName("help_arrows.png");
	//	pkHelpArrows->setPositionX(-30.0f * g_f32ScaleHD);
	//	pkHelpArrows->setPositionY(37.0f * g_f32ScaleHD);
	//	pkEventLowRight->addChild(pkHelpArrows, 3);
	//	UILinkDelegate(pkEventLowLeft, OnLowRightEvent);
	//}
}

UIImplementDelegate(OptionView, OnPlayerInfo, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		Change(1);
		m_pkContentView->Change(PG_INFO);
	}
}

UIImplementDelegate(OptionView, OnEnvSetup, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		Change(1);
		m_pkContentView->Change(PG_SETUP);
	}
}

UIImplementDelegate(OptionView, OnGameHelp, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		Change(1);
		m_pkContentView->Change(PG_HELP);
	}
}

UIImplementDelegate(OptionView, OnBugFeedback, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		VeDebugOutputString("OnBugFeedback");
		g_pkGame->ShowFeedBack();
	}
}

UIImplementDelegate(OptionView, OnBack, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		Back();
	}
}

UIImplementDelegate(OptionView, OnEditInfo, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Menu();
	}
}

UIImplementDelegate(OptionView, OnMusicVolume, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		g_pSoundSystem->SetMusicVolume(((UISlider&)kWidget).GetValue());
		VeDebugOutput("Music volume change to %f", ((UISlider&)kWidget).GetValue());
	}
	else if(u32State == UIWidget::VALUE_CHANGED)
	{
		g_pSoundSystem->SetMusicVolume(((UISlider&)kWidget).GetValue());
		g_pSaveData->WriteParam("music_volume", ((UISlider&)kWidget).GetValue());
		g_pSaveData->Save();
	}
}

UIImplementDelegate(OptionView, OnSoundVolume, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGED)
	{
		g_pSoundSystem->SetSoundVolume(((UISlider&)kWidget).GetValue());
		g_pSaveData->WriteParam("sound_volume", ((UISlider&)kWidget).GetValue());
		g_pSaveData->Save();
		g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
	}
}

UIImplementDelegate(OptionView, OnSilenceCheck, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		if(((UICheckBox&)kWidget).IsCheck())
		{
			g_pSoundSystem->SetSwitch(false);
			g_pSaveData->WriteParam("sound_switch", VeUInt32(1));
			g_pSaveData->Save();
		}
		else
		{
			g_pSoundSystem->SetSwitch(true);
			g_pSaveData->WriteParam("sound_switch", VeUInt32(0));
			g_pSaveData->Save();
		}
	}
}

UIImplementDelegate(OptionView, OnLeftEvent, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;

	if(kButton.Normal()->isVisible())
	{
		pkHelpNodeL->setScale(1.0f);
	}
	else
	{
		pkHelpNodeL->setScale(0.99f);
	}
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("texas_help");
	}
}

UIImplementDelegate(OptionView, OnMidEvent, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;

	if(kButton.Normal()->isVisible())
	{
		pkHelpNodeM->setScale(1.0f);
	}
	else
	{
		pkHelpNodeM->setScale(0.99f);
	}
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("lotto_help");
	}
}

UIImplementDelegate(OptionView, OnRightEvent, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;

	if(kButton.Normal()->isVisible())
	{
		pkHelpNodeR->setScale(1.0f);
	}
	else
	{
		pkHelpNodeR->setScale(0.99f);
	}
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("bacc_help");
	}
}

UIImplementDelegate(OptionView, OnLowLeftEvent, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;

	if(kButton.Normal()->isVisible())
	{
		pkLowHelpNodeL->setScale(1.0f);
	}
	else
	{
		pkLowHelpNodeL->setScale(0.99f);
	}
}

UIImplementDelegate(OptionView, OnLowMidEvent, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;

	if(kButton.Normal()->isVisible())
	{
		pkLowHelpNodeM->setScale(1.0f);
	}
	else
	{
		pkLowHelpNodeM->setScale(0.99f);
	}
}

UIImplementDelegate(OptionView, OnLowRightEvent, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;

	if(kButton.Normal()->isVisible())
	{
		pkLowHelpNodeR->setScale(1.0f);
	}
	else
	{
		pkLowHelpNodeR->setScale(0.99f);
	}
}

void OptionView::UpdateProperties(TexasPokerHallPlayerC* pkPlayer)
{
	VE_ASSERT(pkPlayer);
	VeChar8 acBuffer[64];
	SetName(pkPlayer->GetNickname());
	VeSprintf(acBuffer, 64, "TitleType%d", pkPlayer->GetTitle());
	SetTitle(TT(acBuffer));
	VeSprintf(acBuffer, 64, "TestBankDesc%d", pkPlayer->GetBankCard());
	m_pkVipCard->setVisible(pkPlayer->GetVip() > 0);
	SetBankDesc(TT(acBuffer));
	SetChipsGold(pkPlayer->GetChips(), pkPlayer->GetGoldBricks());
	SetMedal(pkPlayer->GetMedal2(), pkPlayer->GetMedal1(), pkPlayer->GetMedal0());
	SetObject(pkPlayer->GetObj0(), pkPlayer->GetObj1(), pkPlayer->GetObj2(), pkPlayer->GetObj3());
	VeFloat32 f32WinRate(0);
	if(pkPlayer->GetPlayCount())
		f32WinRate = VeFloat32(pkPlayer->GetWinCount()) / VeFloat32(pkPlayer->GetPlayCount());
	SetGameData(pkPlayer->GetWinCount(), pkPlayer->GetWinMaxBet(), f32WinRate);
	m_pkHeadArea->Set(pkPlayer->GetHeadIcon(), pkPlayer->GetSex() == 0, pkPlayer->GetVip() > 0);
	CardType kType;
	kType.Resize(5);
	for(VeUInt32 i(0); i < 5; ++i)
	{
		kType[i].m_tFirst = CardNum_MAX;
		kType[i].m_tSecond = CardSuit_MAX;
	}
	const VeChar8* pcBestType = pkPlayer->GetBestType();
	if(VeStrlen(pcBestType) == 5)
	{
		for(VeUInt32 i(0); i < 5; ++i)
		{
			kType[i].m_tFirst = CardNum((pcBestType[i] & 0xF) - 1);
			kType[i].m_tSecond = CardSuit((pcBestType[i] & 0xF0) >> 4);
		}
	}
	SetBestType(kType, pkPlayer->GetVip() > 0);
}
