#include "Game.h"
#include "UITableView.h"
#include "TexasPoker.h"
#include "Card.h"
#include "Knickknacks.h"
#include "UIButton.h"
#include "SoundSystem.h"
#include "Hall.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"
#include "NativeInterface.h"

USING_NS_CC;

static bool s_bGameBackAlertShow;

Game::Game() : UILayerExt<Game>("game")
{
	UIInitDelegate(Game, OnFill);
	UIInitDelegate(Game, OnFillConfirm);
	UIInitDelegate(Game, OnDiscard);
	UIInitDelegate(Game, OnCheck);
	UIInitDelegate(Game, OnCall);
	UIInitDelegate(Game, OnFillValue);
	UIInitDelegate(Game, OnAutoCheck);
	UIInitDelegate(Game, OnCheckDiscard);
	UIInitDelegate(Game, OnCallAny);
	UIInitDelegate(Game, OnHelp);
	UIInitDelegate(Game, OnDrink);
	UIInitDelegate(Game, OnChat);
	UIInitDelegate(Game, OnMenu);

	m_u32SelfBetNumber = 0;
	m_eAutoState = AUTO_NULL;
	s_bGameBackAlertShow = false;
}

Game::~Game()
{
	term();
}

bool Game::init()
{
	CCSize kSize;
	VeChar8 acBuffer[64];

	CreateGradePage();

	m_pkCategories = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale), CCSize(60.0f * g_f32ScaleHD * ui_main_scale, 30.0f * g_f32ScaleHD * ui_main_scale), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
	m_pkCategories->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - (140.0f * ui_width_scale + 16.0f * ui_main_scale) * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 98.0f - 45.0f * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkCategories);

	GameTableBoard* pkTableBoard = VE_NEW GameTableBoard;
	pkTableBoard->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250.0f - ui_main_scale * 150.0f) * g_f32ScaleHD)));
	AddWidget("table_board", pkTableBoard);

	ChipsBar* pkPond = VE_NEW ChipsBar;
	pkPond->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - ui_main_scale * g_f32ScaleHD * 41.0f),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250.0f - ui_main_scale * 118.0f) * g_f32ScaleHD)));
	AddWidget("pond_chips", pkPond);

	CCPoint kDealPos(VeFloorf(UILayer::Width() * 0.5f),
		VeFloorf(UILayer::Height() * 1.0f - (40.0f * ui_main_scale + (ui_height_scale - ui_main_scale) * 150.0f) * g_f32ScaleHD));

	m_apkSelfCard[0] = VE_NEW Card;
	m_apkSelfCard[0]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 10.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(32.0f * ui_height_scale * g_f32ScaleHD)));
	AddWidget("self_card_l", m_apkSelfCard[0]);
	m_apkSelfCard[0]->SetDealPos(kDealPos);

	m_apkSelfCard[1] = VE_NEW Card;
	m_apkSelfCard[1]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 10.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(32.0f * ui_height_scale * g_f32ScaleHD)));
	AddWidget("self_card_r", m_apkSelfCard[1]);
	m_apkSelfCard[1]->SetDealPos(kDealPos);

	m_apkSelfCard[0]->setZOrder(4);
	m_apkSelfCard[1]->setZOrder(5);

	m_apkTableCard[0] = VE_NEW Card;
	m_apkTableCard[0]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 46.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250 - ui_main_scale * 74) * g_f32ScaleHD)));
	AddWidget("table_card_0", m_apkTableCard[0]);

	m_apkTableCard[1] = VE_NEW Card;
	m_apkTableCard[1]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 0.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250 - ui_main_scale * 74) * g_f32ScaleHD)));
	AddWidget("table_card_1", m_apkTableCard[1]);

	m_apkTableCard[2] = VE_NEW Card;
	m_apkTableCard[2]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 46.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250 - ui_main_scale * 74) * g_f32ScaleHD)));
	AddWidget("table_card_2", m_apkTableCard[2]);

	m_apkTableCard[3] = VE_NEW Card;
	m_apkTableCard[3]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 23.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250 - ui_main_scale * 10) * g_f32ScaleHD)));
	AddWidget("table_card_3", m_apkTableCard[3]);

	m_apkTableCard[4] = VE_NEW Card;
	m_apkTableCard[4]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 23.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250 - ui_main_scale * 10) * g_f32ScaleHD)));
	AddWidget("table_card_4", m_apkTableCard[4]);

	for(VeUInt32 i(0); i < 5; ++i)
	{
		m_apkTableCard[i]->SetDealPos(kDealPos);
		m_apkTableCard[i]->SetFace(false);
	}

	UIPage* pkNameCards = VE_NEW UIPage;
	AddWidget("name_cards", pkNameCards);

	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkLeft[i] = VE_NEW NameCard(i);
		m_apkLeft[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - (112) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * (70.0f + i * 80.0f)) * g_f32ScaleHD)));
		pkNameCards->AddChild(m_apkLeft[i]);
		m_apkLeft[i]->SetDrink(-1);

		m_apkLeft[i]->SetName("Name");

		m_apkLeft[i]->GetLeft()->SetDealPos(kDealPos);
		m_apkLeft[i]->GetRight()->SetDealPos(kDealPos);
	}

	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkRight[i] = VE_NEW NameCard(i + 4);
		m_apkRight[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (112) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * (70.0f + i * 80.0f)) * g_f32ScaleHD)));
		pkNameCards->AddChild(m_apkRight[i]);
		m_apkRight[i]->SetDrink(-1);
		m_apkRight[i]->SetRight();

		m_apkRight[i]->SetName("Name");

		m_apkRight[i]->GetLeft()->SetDealPos(kDealPos);
		m_apkRight[i]->GetRight()->SetDealPos(kDealPos);
	}

	m_pkPlayerCard = VE_NEW PlayerCard;
	AddWidget("player_card", m_pkPlayerCard);

	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_chatBarLeft[i] = VE_NEW HeadChatBar();
		m_chatBarLeft[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - (112) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * (50.0f + i * 80.0f)) * g_f32ScaleHD)));
		pkNameCards->AddChild(m_chatBarLeft[i], 12);
	}

	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_chatBarRight[i] = VE_NEW HeadChatBar();
		m_chatBarRight[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (112) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * (50.0f + i * 80.0f)) * g_f32ScaleHD)));
		pkNameCards->AddChild(m_chatBarRight[i], 12);
	}

	m_pkPlayerChatBar = VE_NEW HeadChatBar;
	m_pkPlayerChatBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (0) * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.0f + ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * (50.0f + 120.0f)) * g_f32ScaleHD)));
	AddWidget("player_chatbar", m_pkPlayerChatBar, 12);

	UIButton* pkFill = VE_NEW UIButton("cyc_btn_n.png", "cyc_btn_p.png", "cyc_btn_d.png");
	pkFill->setScale(ui_main_scale);
	pkFill->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f - 10.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillText = CCSprite::createWithSpriteFrameName("btn_fill_text.png");
	m_pkFillText->setPositionY(2.0f * g_f32ScaleHD);
	pkFill->addChild(m_pkFillText, 5);
	AddWidget("btn_fill", pkFill);
	UILinkDelegate(pkFill, OnFill);
	pkFill->SetVisible(true);

	m_pkFillBar = VE_NEW UIPage;
	AddWidget("fill_bar", m_pkFillBar, 7);
	m_pkFillBar->SetVisible(false);

	UIButton* pkFillBarConfirm = VE_NEW UIButton("cyc_btn_s_n.png", "cyc_btn_s_p.png", "cyc_btn_d.png");
	pkFillBarConfirm->setScale(ui_main_scale);
	pkFillBarConfirm->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f - 10.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillBarConfirmText = CCSprite::createWithSpriteFrameName("btn_confirm_text.png");
	m_pkFillBarConfirmText->setPositionY(2.0f * g_f32ScaleHD);
	pkFillBarConfirm->addChild(m_pkFillBarConfirmText, 5);
	m_pkFillBar->AddChild(pkFillBarConfirm, 4);
	UILinkDelegate(pkFillBarConfirm, OnFillConfirm);

	m_pkSlider = VE_NEW FillSlider;
	m_pkSlider->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f + 90.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillBar->AddChild(m_pkSlider);
	UILinkDelegate(m_pkSlider, OnFillValue);
	CCSize kSliderSize = m_pkSlider->getContentSize();
	kSliderSize.width *= 2.0f;
	m_pkSlider->setContentSize(kSliderSize);

	m_pkBetNumber = CCSprite::createWithSpriteFrameName("bet_number_n.png");
	m_pkBetNumber->setScale(ui_main_scale);
	m_pkBetNumber->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f + 194.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillBar->addChild(m_pkBetNumber, 5);

	m_pkBetNumberText = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkBetNumberText->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f + 194.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillBar->addChild(m_pkBetNumberText, 5);
	SetBetNumber(0);

	m_pkBetAllin = CCSprite::createWithSpriteFrameName("bet_number_p.png");
	m_pkBetAllin->setScale(ui_main_scale);
	m_pkBetAllin->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f + 194.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillBar->addChild(m_pkBetAllin, 5);
	m_pkBetAllin->setVisible(false);
	m_pkAllinText = CCSprite::createWithSpriteFrameName("all_in.png");
	m_pkAllinText->setScale(ui_main_scale);
	m_pkAllinText->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f + 194.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkFillBar->addChild(m_pkAllinText, 5);
	m_pkAllinText->setVisible(false);

	UIButton* pkDiscard = VE_NEW UIButton("cyc_btn_n.png", "cyc_btn_p.png", "cyc_btn_d.png");
	pkDiscard->setScale(ui_main_scale);
	pkDiscard->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 55.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 98.0f - 25.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkDiscardText = CCSprite::createWithSpriteFrameName("btn_discard_text.png");
	m_pkDiscardText->setPositionY(2.0f * g_f32ScaleHD);
	pkDiscard->addChild(m_pkDiscardText, 5);
	AddWidget("btn_discard", pkDiscard);
	UILinkDelegate(pkDiscard, OnDiscard);
	pkDiscard->SetVisible(true);

	UIButton* pkCheck = VE_NEW UIButton("cyc_btn_n.png", "cyc_btn_p.png", "cyc_btn_d.png");
	pkCheck->setScale(ui_main_scale);
	pkCheck->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 55.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 98.0f - 25.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkCheckText = CCSprite::createWithSpriteFrameName("btn_check_text.png");
	m_pkCheckText->setPositionY(2.0f * g_f32ScaleHD);
	pkCheck->addChild(m_pkCheckText, 5);
	AddWidget("btn_check", pkCheck);
	UILinkDelegate(pkCheck, OnCheck);
	pkCheck->SetVisible(true);

	UIButton* pkCall = VE_NEW UIButton("cyc_btn_n.png", "cyc_btn_p.png", "cyc_btn_d.png");
	pkCall->setScale(ui_main_scale);
	pkCall->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 55.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 98.0f - 25.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkCallText = CCSprite::createWithSpriteFrameName("btn_call_text.png");
	m_pkCallText->setPositionY(2.0f * g_f32ScaleHD);
	pkCall->addChild(m_pkCallText, 5);
	AddWidget("btn_call", pkCall);
	UILinkDelegate(pkCall, OnCall);
	pkCall->SetVisible(false);

	UIButton* pkAutoCheck = VE_NEW UIButton("cyc_btn2_n.png", "cyc_btn2_p.png", "cyc_btn_d.png");
	pkAutoCheck->setScale(ui_main_scale);
	pkAutoCheck->setPosition(ccp(ui_w(0.5f),
		VeFloorf((ui_height_scale * 98.0f - 10.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkAutoCheckNode = CCNode::create();
	pkAutoCheck->addChild(m_pkAutoCheckNode, 5);
	CCSprite* pkAutoCheckText = CCSprite::createWithSpriteFrameName("btn_auto_check.png");
	pkAutoCheckText->setPositionY(-3.0f * g_f32ScaleHD);
	m_pkAutoCheckNode->addChild(pkAutoCheckText);
	AddWidget("btn_auto_check", pkAutoCheck);
	UILinkDelegate(pkAutoCheck, OnAutoCheck);
	pkAutoCheck->SetVisible(false);

	m_pkAutoCheckState[0] = CCSprite::createWithSpriteFrameName("check_s_n.png");
	m_pkAutoCheckState[0]->setPositionY(10.0f * g_f32ScaleHD);
	m_pkAutoCheckNode->addChild(m_pkAutoCheckState[0]);
	m_pkAutoCheckState[1] = CCSprite::createWithSpriteFrameName("check_s_p.png");
	m_pkAutoCheckState[1]->setPositionY(10.0f * g_f32ScaleHD);
	m_pkAutoCheckState[1]->setVisible(false);
	m_pkAutoCheckNode->addChild(m_pkAutoCheckState[1]);
	
	UIButton* pkCheckDiscard = VE_NEW UIButton("cyc_btn2_n.png", "cyc_btn2_p.png", "cyc_btn_d.png");
	pkCheckDiscard->setScale(ui_main_scale);
	pkCheckDiscard->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 55.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 98.0f - 25.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkCheckDiscardNode = CCNode::create();
	pkCheckDiscard->addChild(m_pkCheckDiscardNode, 5);
	CCSprite* pkCheckDiscardText = CCSprite::createWithSpriteFrameName("btn_check_discard.png");
	pkCheckDiscardText->setPositionY(-3.0f * g_f32ScaleHD);
	m_pkCheckDiscardNode->addChild(pkCheckDiscardText);
	AddWidget("btn_check_discard", pkCheckDiscard);
	UILinkDelegate(pkCheckDiscard, OnCheckDiscard);
	pkCheckDiscard->SetVisible(false);

	m_pkCheckDiscardState[0] = CCSprite::createWithSpriteFrameName("check_s_n.png");
	m_pkCheckDiscardState[0]->setPositionY(10.0f * g_f32ScaleHD);
	m_pkCheckDiscardNode->addChild(m_pkCheckDiscardState[0]);
	m_pkCheckDiscardState[1] = CCSprite::createWithSpriteFrameName("check_s_p.png");
	m_pkCheckDiscardState[1]->setPositionY(10.0f * g_f32ScaleHD);
	m_pkCheckDiscardState[1]->setVisible(false);
	m_pkCheckDiscardNode->addChild(m_pkCheckDiscardState[1]);

	UIButton* pkCallAny = VE_NEW UIButton("cyc_btn2_n.png", "cyc_btn2_p.png", "cyc_btn_d.png");
	pkCallAny->setScale(ui_main_scale);
	pkCallAny->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 55.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 98.0f - 25.0f * ui_main_scale) * g_f32ScaleHD)));
	m_pkCallAnyNode = CCNode::create();
	pkCallAny->addChild(m_pkCallAnyNode, 5);
	CCSprite* pkCallAnyText = CCSprite::createWithSpriteFrameName("btn_call_any.png");
	pkCallAnyText->setPositionY(-3.0f * g_f32ScaleHD);
	m_pkCallAnyNode->addChild(pkCallAnyText);
	AddWidget("btn_call_any", pkCallAny);
	UILinkDelegate(pkCallAny, OnCallAny);
	pkCallAny->SetVisible(false);

	m_pkCallAnyState[0] = CCSprite::createWithSpriteFrameName("check_s_n.png");
	m_pkCallAnyState[0]->setPositionY(10.0f * g_f32ScaleHD);
	m_pkCallAnyNode->addChild(m_pkCallAnyState[0]);
	m_pkCallAnyState[1] = CCSprite::createWithSpriteFrameName("check_s_p.png");
	m_pkCallAnyState[1]->setPositionY(10.0f * g_f32ScaleHD);
	m_pkCallAnyState[1]->setVisible(false);
	m_pkCallAnyNode->addChild(m_pkCallAnyState[1]);

	UIButton* pkHelp = VE_NEW UIButton("help_n.png", "help_p.png");
	pkHelp->setScale(ui_main_scale);
	pkHelp->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 140.0f * ui_width_scale * g_f32ScaleHD),
		VeFloorf(20.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("btn_help", pkHelp);
	UILinkDelegate(pkHelp, OnHelp);
	kSize = pkHelp->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	pkHelp->setContentSize(kSize);

	UIButton* pkDrink = VE_NEW UIButton("drink_n.png", "drink_p.png");
	pkDrink->setScale(ui_main_scale);
	pkDrink->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 140.0f * ui_width_scale * g_f32ScaleHD),
		VeFloorf(20.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("btn_drink", pkDrink);
	UILinkDelegate(pkDrink, OnDrink);
	kSize = pkDrink->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	pkDrink->setContentSize(kSize);

	NoticeBar* pkNoticeBar = VE_NEW NoticeBar;
	pkNoticeBar->setPositionY(VeFloorf(UILayer::Height() * 0.5f + 230.0f * g_f32ScaleHD * ui_height_scale));
	AddWidget("notice_bar", pkNoticeBar, 0);

	UIButton* pkChat = VE_NEW UIButton("drink_n.png", "drink_p.png");
	pkChat->setScale(ui_main_scale);
	pkChat->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 140.0f * ui_width_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 230.0f * g_f32ScaleHD * ui_height_scale)));
	AddWidget("btn_chat", pkChat);
	UILinkDelegate(pkChat, OnChat);
	kSize = pkChat->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	pkChat->setContentSize(kSize);

	m_pkSelfDrink = CCSprite::create();
	m_pkSelfDrink->setScale(ui_main_scale);
	m_pkSelfDrink->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 60.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf((ui_height_scale * 48.0f - 24.0f * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkSelfDrink, 5);
	m_pkSelfDrink->setVisible(false);
	SetSelfDrink(-1);

	UIButton* pkMenu = VE_NEW UIButton("game_back_n.png", "game_back_p.png", "game_back_n.png");
	pkMenu->setScale(ui_main_scale);
	pkMenu->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 140.0f * ui_width_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() - 14.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("btn_menu", pkMenu);
	UILinkDelegate(pkMenu, OnMenu);

	CCPoint kChipsFadeOut(ui_w(0.5f), VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250.0f - ui_main_scale * 118.0f) * g_f32ScaleHD));
	CCPoint kChipsFadeIn(ui_w(0.5f), VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * 332) * g_f32ScaleHD));

	CCPoint m_showPlayerChipsPos = ccp(VeFloorf(UILayer::Width() * 0.5f - 65.0f * ui_width_scale * g_f32ScaleHD),
		VeFloorf(17.0f * ui_main_scale * g_f32ScaleHD));

	m_pkPlayerChips = VE_NEW GameChips;
	m_pkPlayerChips->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * 286) * g_f32ScaleHD)));
	AddWidget("player_chips", m_pkPlayerChips, 6);
	m_pkPlayerChips->SetFadePos(m_showPlayerChipsPos, kChipsFadeOut);
	m_pkPlayerChips->SetVisible(false);

	for(VeUInt32 i(0); i < 9; ++i)
	{
		VeSprintf(acBuffer, 64, "move_chips_%d", i);
		m_apkChipsMove[i] = VE_NEW GameChipsMove;
		AddWidget(acBuffer, m_apkChipsMove[i], 6);
		m_apkChipsMove[i]->SetVisible(false);
	}
	
	m_pkShowPlayerChips = VE_NEW GameChips;
	m_pkShowPlayerChips->setPosition(m_showPlayerChipsPos);
	AddWidget("show_player_chips", m_pkShowPlayerChips, 6);
	m_pkShowPlayerChips->SetFadePos(m_showPlayerChipsPos, m_showPlayerChipsPos);
	m_pkShowPlayerChips->SetVisible(true);

	VeFloat32 af32Pos[4] = {60.0f, 130.0f, 240.0f, 300.0f};

	for(VeUInt32 i(0); i < 4; ++i)
	{
		VeSprintf(acBuffer, 64, "left_chips_%d", i);
		m_apkLeftChips[i] = VE_NEW GameChips;
		m_apkLeftChips[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - (58) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * af32Pos[i]) * g_f32ScaleHD)));
		AddWidget(acBuffer, m_apkLeftChips[i], 6);
		m_apkLeftChips[i]->SetFadePos(m_apkLeft[i]->getPosition(), kChipsFadeOut);
		m_apkLeftChips[i]->SetVisible(false);

		VeSprintf(acBuffer, 64, "right_chips_%d", i);
		m_apkRightChips[i] = VE_NEW GameChips;
		m_apkRightChips[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + (58) * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * af32Pos[i]) * g_f32ScaleHD)));
		AddWidget(acBuffer, m_apkRightChips[i], 6);
		m_apkRightChips[i]->SetFadePos(m_apkRight[i]->getPosition(), kChipsFadeOut);
		m_apkRightChips[i]->SetVisible(false);
	}

	UIWidget* pkDrinkRoot = VE_NEW UIWidget;
	AddWidget("drinks_move", pkDrinkRoot, 9);

	for (VeUInt32 i(0); i < 4; ++i)
	{
		m_apkDrinkLeft[i] = VE_NEW GameDrinksMove;
		m_apkDrinkLeft[i]->SetFadeOutPos(m_apkLeft[i]->getPosition());
		pkDrinkRoot->AddChild(m_apkDrinkLeft[i]);
	}

	for (VeUInt32 i(0); i < 4; ++i)
	{
		m_apkDrinkRight[i] = VE_NEW GameDrinksMove;
		m_apkDrinkRight[i]->SetFadeOutPos(m_apkRight[i]->getPosition());
		pkDrinkRoot->AddChild(m_apkDrinkRight[i]);
	}

	m_pkPlayerDrink = VE_NEW GameDrinksMove;
	m_pkPlayerDrink->SetFadeOutPos(m_pkPlayerCard->GetPosition());
	pkDrinkRoot->AddChild(m_pkPlayerDrink);

	m_pkPlayWin = VE_NEW PlayerWin();
	AddWidget("play_win", m_pkPlayWin, 8);
	m_pkPlayWin->SetVisible(true);

	return UILayer::init();
}

void Game::SetSelfDrink(VeInt32 i32Index)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	WIDGET(UIButton, btn_drink);
	if(pkPlayer->GetRoomInfo().m_tType == ROOM_TYPE_PRACTICE)
	{
		btn_drink.SetVisible(false);
		m_pkSelfDrink->setVisible(false);
	}
	else
	{
		btn_drink.SetVisible(true);
		if(i32Index < 0)
		{
			m_pkSelfDrink->setVisible(false);
		}
		else
		{
			m_pkSelfDrink->setVisible(true);
			VeChar8 acBuffer[64];
			VeSprintf(acBuffer, 64, "drink_%d.png", i32Index);
			m_pkSelfDrink->initWithSpriteFrameName(acBuffer);
		}
	}
}

void Game::SetCategories(VeUInt32 u32Categories)
{
	if(u32Categories < Categories_MAX)
	{
		m_pkCategories->setVisible(true);
		VeChar8 acBuffer[64];
		VeSprintf(acBuffer, 64, "Categories_%d", u32Categories);
		m_pkCategories->setString(TT(acBuffer));
		CCSize kSize = m_pkCategories->getContentSize();
		TexasPoker::Language eLan = g_pkGame->GetLanguage();
		if(eLan != TexasPoker::en_US && u32Categories > CATE_STRAIGHT_FLUSH)
			m_pkCategories->setAnchorPoint(ccp(VeFloorf(-kSize.width * 0.07f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		else
			m_pkCategories->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	else
	{
		m_pkCategories->setVisible(false);
	}
}

void Game::SetBetNumber(VeUInt32 u32BetNumber)
{
	m_pkBetNumberText->setString(NTL(u32BetNumber, 5));
	CCSize kSize = m_pkBetNumberText->getContentSize();
	m_pkBetNumberText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Game::term()
{
	UILayer::term();
}

void Game::CreateGradePage()
{
	UIPageView* pkGradePageView = VE_NEW UIPageView;
	AddWidget("grade_page_view", pkGradePageView);

	for(VeUInt32 i(0); i < GRADE_MAX; ++i)
	{
		UIPage* pkGrade = VE_NEW UIPage;
		pkGradePageView->AddPage(pkGrade);
		CCSpriteBatchNode* pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
		pkGrade->addChild(pkRoot);

		for(VeUInt32 j(0); j < 4; ++j)
		{
			CCSprite* pkBackground = CCSprite::createWithSpriteFrameName("bg_1.png");
			pkBackground->setAnchorPoint(ccp(1.0f, 0.0f));
			pkBackground->setPosition(ui_ccp(0.5f, 0.5f));
			pkBackground->setBlendFunc(kCCBlendFuncDisable);
			switch(j)
			{
			case 1:
				pkBackground->setScaleX(-ui_main_scale);
				pkBackground->setScaleY(ui_main_scale);
				break;
			case 2:
				pkBackground->setScaleX(ui_main_scale);
				pkBackground->setScaleY(-ui_main_scale);
				break;
			case 3:
				pkBackground->setScaleX(-ui_main_scale);
				pkBackground->setScaleY(-ui_main_scale);
				break;
			default:
				pkBackground->setScale(ui_main_scale);
				break;
			}		
			pkRoot->addChild(pkBackground, 0);
		}

		VeChar8 acBuffer[64];
		VeSprintf(acBuffer, 64, "table_%d.png", i);
		CCSprite* pkTableL = CCSprite::createWithSpriteFrameName(acBuffer);
		pkTableL->setAnchorPoint(ccp(1.0f, 1.0f));
		pkTableL->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale - ui_main_scale) * 250.0f * g_f32ScaleHD)));
		pkTableL->setScale(ui_main_scale);
		pkRoot->addChild(pkTableL, 0);
		CCSprite* pkTableR = CCSprite::createWithSpriteFrameName(acBuffer);
		pkTableR->setAnchorPoint(ccp(1.0f, 1.0f));
		pkTableR->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale - ui_main_scale) * 250.0f * g_f32ScaleHD)));
		pkTableR->setScaleX(-ui_main_scale);
		pkTableR->setScaleY(ui_main_scale);
		pkRoot->addChild(pkTableR, 0);

		VeSprintf(acBuffer, 64, "board_%d.png", i);
		CCSprite* pkBoardL = CCSprite::createWithSpriteFrameName(acBuffer);
		pkBoardL->setAnchorPoint(ccp(1.0f, 0.0f));
		pkBoardL->setPosition(ui_ccp(0.5f, 0.0f));
		pkBoardL->setScale(ui_main_scale);
		pkRoot->addChild(pkBoardL, 0);
		CCSprite* pkBoardR = CCSprite::createWithSpriteFrameName(acBuffer);
		pkBoardR->setAnchorPoint(ccp(1.0f, 0.0f));
		pkBoardR->setPosition(ui_ccp(0.5f, 0.0f));
		pkBoardR->setScaleX(-ui_main_scale);
		pkBoardR->setScaleY(ui_main_scale);
		pkRoot->addChild(pkBoardR, 0);

		pkBoardL->setPositionY(VeFloorf((ui_height_scale * 98.0f - 116.0f * ui_main_scale) * g_f32ScaleHD));
		pkBoardR->setPositionY(VeFloorf((ui_height_scale * 98.0f - 116.0f * ui_main_scale) * g_f32ScaleHD));

		VeUInt32 u32Tag;
		if (i == GRADE_NORMAL)
		{
			u32Tag = 2;
		}
		else
		{
			u32Tag = 1;
		}
		VeSprintf(acBuffer, 64, "table_girl_%d.png", u32Tag);
		CCSprite* pkGirl = CCSprite::createWithSpriteFrameName(acBuffer);
		pkGirl->setAnchorPoint(ccp(0.5f, 1.0f));
		pkGirl->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f),
			VeFloorf(UILayer::Height() * 1.0f - (20.0f * ui_main_scale + (ui_height_scale - ui_main_scale) * 150.0f) * g_f32ScaleHD)));
		pkGirl->setScale(ui_main_scale);
		pkRoot->addChild(pkGirl, 0);

		if(i)
		{
			CCSprite* pkVip = CCSprite::createWithSpriteFrameName("table_vip.png");
			pkVip->setPosition(ccp(ui_w(0.5f),
				VeFloorf(UILayer::Height() * 1.0f - (180.0f * ui_main_scale + (ui_height_scale - ui_main_scale) * 250.0f) * g_f32ScaleHD)));
			pkVip->setScale(ui_main_scale);
			pkRoot->addChild(pkVip, 0);
		}
	}
}

void Game::OnActive(const UIUserDataPtr& spData)
{
	m_eStateLastUpdate = TableState_MAX;
	m_u32ProcessLastUpdate = VE_UINT32_MAX;
	m_u64ProcessLastExpireUpdate = VeUInt64(-1);
	m_eAutoState = AUTO_NULL;
	m_pkAutoCheckState[0]->setVisible(true);
	m_pkAutoCheckState[1]->setVisible(false);
	m_pkCheckDiscardState[0]->setVisible(true);
	m_pkCheckDiscardState[1]->setVisible(false);
	m_pkCallAnyState[0]->setVisible(true);
	m_pkCallAnyState[1]->setVisible(false);
	UpdateTable(false);
	g_pSoundSystem->PlayMusic(GAME_MUSIC);
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	const TablePlayer* pkSelf = pkPlayer->GetTablePlayer(pkPlayer->GetSeatIndex());
	if(pkSelf && pkSelf->m_tState == TABLE_PLAYER_HOLD)
	{
		g_pSoundSystem->PlayEffect(SE_POKER_ON_WELCOME);
	}
	NativeOnEventBegin("game");
}

void Game::OnDeactive(const UIUserDataPtr& spData)
{
	g_pSoundSystem->StopMusic();
	NativeOnEventEnd("game");
}

class GameBackAlertCallback : public UIAlert::Callback
{
public:
	GameBackAlertCallback()
	{
		m_kTitle = TT("GameBackConfirmTitle");
		m_kContent = TT("GameBackConfirmContent");
		m_kEnter = TT("Confirm");
		m_kCancel = TT("Cancel");
	}

	virtual void OnConfirm()
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqLeavePokerTable();
		g_pLayerManager->PopAllLayers();
		g_pkGame->ShowLoading();
	}
};

bool Game::OnBack()
{
	if(s_bGameBackAlertShow)
	{
		s_bGameBackAlertShow = false;
		return true;
	}
	else if(g_pLayerManager->GetMiddleLayersNum())
	{
		return false;
	}
	else
	{
		g_pLayerManager->PushLayer("alert", VE_NEW GameBackAlertCallback);
		return false;
	}	
}

void Game::ToHall()
{
	s_bGameBackAlertShow = true;
	g_pLayerManager->Back();
}

void Game::OnMenu()
{
	if(g_pLayerManager->GetLayersNum() == 0)
	{
		g_pLayerManager->PushLayer("game_menu");
	}
}

UIImplementDelegate(Game, OnFill, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkFillText->setScale(1.0f);
	}
	else
	{
		m_pkFillText->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		kButton.SetVisible(false);
		m_pkFillBar->SetVisible(true);
		m_pkSlider->SetValue(0);
		UpdateSlider();
	}
}

UIImplementDelegate(Game, OnFillConfirm, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkFillBarConfirmText->setScale(1.0f);
	}
	else
	{
		m_pkFillBarConfirmText->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{

		m_pkFillBar->SetVisible(false);
		WIDGET(UIButton, btn_fill);
		btn_fill.SetVisible(true);
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		if (m_u32SelfBetNumber)
		{
			pkPlayer->S_ReqFill(m_u32SelfBetNumber);
			DisableSetCotrolBtn();
		}
	}
}

UIImplementDelegate(Game, OnDiscard, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkDiscardText->setScale(1.0f);
	}
	else
	{
		m_pkDiscardText->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqFold();
		DisableSetCotrolBtn();
	}
}

UIImplementDelegate(Game, OnCheck, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkCheckText->setScale(1.0f);
	}
	else
	{
		m_pkCheckText->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqCall();
		DisableSetCotrolBtn();
	}
}

UIImplementDelegate(Game, OnCall, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkCallText->setScale(1.0f);
	}
	else
	{
		m_pkCallText->setScale(0.98f);
	}
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqCall();
		DisableSetCotrolBtn();
	}
};

UIImplementDelegate(Game, OnFillValue, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		UpdateSlider();
	}
};

UIImplementDelegate(Game, OnAutoCheck, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkAutoCheckNode->setScale(1.0f);
	}
	else
	{
		m_pkAutoCheckNode->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		if(m_pkAutoCheckState[0]->isVisible())
		{
			m_pkAutoCheckState[0]->setVisible(false);
			m_pkAutoCheckState[1]->setVisible(true);
			m_pkCheckDiscardState[0]->setVisible(true);
			m_pkCheckDiscardState[1]->setVisible(false);
			m_pkCallAnyState[0]->setVisible(true);
			m_pkCallAnyState[1]->setVisible(false);
			m_eAutoState = AUTO_CHECK;
		}
		else
		{
			m_pkAutoCheckState[0]->setVisible(true);
			m_pkAutoCheckState[1]->setVisible(false);
			m_eAutoState = AUTO_NULL;
		}
	}
};

UIImplementDelegate(Game, OnCheckDiscard, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkCheckDiscardNode->setScale(1.0f);
	}
	else
	{
		m_pkCheckDiscardNode->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		if(m_pkCheckDiscardState[0]->isVisible())
		{
			m_pkCheckDiscardState[0]->setVisible(false);
			m_pkCheckDiscardState[1]->setVisible(true);
			m_pkAutoCheckState[0]->setVisible(true);
			m_pkAutoCheckState[1]->setVisible(false);
			m_pkCallAnyState[0]->setVisible(true);
			m_pkCallAnyState[1]->setVisible(false);
			m_eAutoState = AUTO_CHECK_OR_DISCARD;
		}
		else
		{
			m_pkCheckDiscardState[0]->setVisible(true);
			m_pkCheckDiscardState[1]->setVisible(false);
			m_eAutoState = AUTO_NULL;
		}
	}
};

UIImplementDelegate(Game, OnCallAny, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkCallAnyNode->setScale(1.0f);
	}
	else
	{
		m_pkCallAnyNode->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		if(m_pkCallAnyState[0]->isVisible())
		{
			m_pkCallAnyState[0]->setVisible(false);
			m_pkCallAnyState[1]->setVisible(true);

			m_pkAutoCheckState[0]->setVisible(true);
			m_pkAutoCheckState[1]->setVisible(false);

			m_pkCheckDiscardState[0]->setVisible(true);
			m_pkCheckDiscardState[1]->setVisible(false);
			m_eAutoState = AUTO_CALL_ANY;
		}
		else
		{
			m_pkCallAnyState[0]->setVisible(true);
			m_pkCallAnyState[1]->setVisible(false);
			m_eAutoState = AUTO_NULL;
		}
	}
}

UIImplementDelegate(Game, OnHelp, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("game_help");
	}
};

UIImplementDelegate(Game, OnDrink, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		g_pLayerManager->PushLayer("canteen", VE_NEW UILayerChangeParams(0, pkPlayer->GetSeatIndex()));
	}
}

UIImplementDelegate(Game, OnChat, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		g_pLayerManager->PushLayer("chat_window");	
	}
}

UIImplementDelegate(Game, OnMenu, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		OnMenu();
	}
}

void Game::SetSelfSeatIndex(VeUInt32 u32Index)
{
	VE_ASSERT(u32Index <= 8);
	m_pkPlayerCard->SetSeatIndex(u32Index);
	for(VeUInt32 i(0); i <= 8; ++i)
	{
		if(i == u32Index) continue;
		GetCard(i)->SetSeatIndex(i);
	}
}

NameCard* Game::GetCard(VeInt32 i32Target)
{
	VeInt32 i32Self = m_pkPlayerCard->GetSeatIndex();
	VE_ASSERT(i32Self >= 0 && i32Self <= 8);
	VE_ASSERT(i32Target >= 0 && i32Target <= 8);
	VE_ASSERT(i32Self != i32Target);

	VeInt32 i32TargetMap = i32Target - i32Self - 1;

	if(i32TargetMap < 0)
	{
		i32TargetMap += 9;
	}

	 if (i32TargetMap < 4)
	{
		return m_apkLeft[3 - i32TargetMap];
	}
	else
	{
		return m_apkRight[i32TargetMap - 4];
	}
}

void Game::SetChipsOnTable(VeInt32 i32Target, VeUInt32 u32Chips, bool bMotion)
{
	VE_ASSERT(m_pkPlayerCard->GetSeatIndex() >= 0 && m_pkPlayerCard->GetSeatIndex() <= 8);
	VE_ASSERT(i32Target >= 0 && i32Target <= 8);
	GameChips* pkChips = GetChips(i32Target);
	GameChipsMove* pkMove = m_apkChipsMove[i32Target];
	VeUInt32 u32Number = m_au32Chips[i32Target];
	if(bMotion && u32Chips > u32Number)
	{
		if(u32Number)
		{
			pkMove->SetVisible(true);
			pkMove->SetFadePos(pkChips->FadeInPos(), pkChips->getPosition());
			pkMove->Move(u32Chips - u32Number);
			pkChips->ChangeNumberDelay(u32Chips, 0.3f);
		}
		else
		{
			pkChips->SetNumber(u32Chips);
			pkChips->FadeIn();
		}
		g_pSoundSystem->PlayEffect(SE_BET);
	}
	else if(bMotion && (!u32Chips) && u32Number)
	{
		pkChips->FadeOut();
		g_pSoundSystem->PlayEffect(SE_CHIPS_WIN);
	}
	else
	{
		pkChips->SetNumber(u32Chips);
		pkChips->SetVisible(u32Chips ? true : false);
	}
	m_au32Chips[i32Target] = u32Chips;
}

HeadChatBar* Game::GetHeadChatBar(VeInt32 i32Target)
{
	VeInt32 i32Self = m_pkPlayerCard->GetSeatIndex();
	VE_ASSERT(i32Self >= 0 && i32Self <= 8);
	VE_ASSERT(i32Target >= 0 && i32Target <= 8);
	if(i32Self == i32Target)
	{
		return m_pkPlayerChatBar;
	}
	else
	{
		VeInt32 i32TargetMap = i32Target - i32Self - 1;

		if(i32TargetMap < 0)
		{
			i32TargetMap += 9;
		}

		if (i32TargetMap < 4)
		{
			return m_chatBarLeft[3 - i32TargetMap];
		}
		else
		{
			return m_chatBarRight[i32TargetMap - 4];
		}
	}
}

GameDrinksMove* Game::GetDrink(VeInt32 i32Target)
{
	VeInt32 i32Self = m_pkPlayerCard->GetSeatIndex();
	VE_ASSERT(i32Self >= 0 && i32Self <= 8);
	VE_ASSERT(i32Target >= 0 && i32Target <= 8);
	if(i32Self == i32Target)
	{
		return m_pkPlayerDrink;
	}
	else
	{
		VeInt32 i32TargetMap = i32Target - i32Self - 1;

		if(i32TargetMap < 0)
		{
			i32TargetMap += 9;
		}

		if (i32TargetMap < 4)
		{
			return m_apkDrinkLeft[3 - i32TargetMap];
		}
		else
		{
			return m_apkDrinkRight[i32TargetMap - 4];
		}
	}
}

GameChips* Game::GetChips(VeInt32 i32Target)
{
	VeInt32 i32Self = m_pkPlayerCard->GetSeatIndex();
	VE_ASSERT(i32Self >= 0 && i32Self <= 8);
	VE_ASSERT(i32Target >= 0 && i32Target <= 8);
	if(i32Self == i32Target)
	{
		return m_pkPlayerChips;
	}
	else
	{
		VeInt32 i32TargetMap = i32Target - i32Self - 1;

		if(i32TargetMap < 0)
		{
			i32TargetMap += 9;
		}

		if (i32TargetMap < 4)
		{
			return m_apkLeftChips[3 - i32TargetMap];
		}
		else
		{
			return m_apkRightChips[i32TargetMap - 4];
		}
	}
}

void Game::UpdateTurnCard(VeFloat32 f32Delta)
{
	g_pSoundSystem->PlayEffect(SE_POKER_ON_START);
	m_apkSelfCard[0]->Turn();
	m_apkSelfCard[1]->Turn();
}

void Game::SetCotrolBtnState(ControlBtnState state, bool bEnable)
{
	m_pkFillBar->SetVisible(false);
	WIDGET(UIButton,btn_discard);
	WIDGET(UIButton,btn_fill);
	WIDGET(UIButton,btn_check);
	WIDGET(UIButton,btn_call);
	WIDGET(UIButton,btn_auto_check);
	WIDGET(UIButton,btn_check_discard);
	WIDGET(UIButton,btn_call_any);
	switch(state)
	{
	case CONTROL_CALL:
		btn_discard.SetEnable(bEnable);
		btn_call.SetEnable(bEnable);
		btn_fill.SetEnable(bEnable);
		btn_call_any.SetVisible(false);
		btn_auto_check.SetVisible(false);
		btn_check_discard.SetVisible(false);
		btn_check.SetVisible(false);
		btn_discard.SetVisible(true);
		btn_call.SetVisible(true);
		btn_fill.SetVisible(true);
		return;
	case CONTROL_CHECK:
		btn_discard.SetEnable(bEnable);
		btn_check.SetEnable(bEnable);
		btn_fill.SetEnable(bEnable);
		btn_call_any.SetVisible(false);
		btn_auto_check.SetVisible(false);
		btn_check_discard.SetVisible(false);
		btn_call.SetVisible(false);
		btn_discard.SetVisible(true);
		btn_check.SetVisible(true);
		btn_fill.SetVisible(true);
		return;
	case CONTROL_AUTO:
		if(!bEnable)
		{
			m_pkAutoCheckState[0]->setVisible(true);
			m_pkAutoCheckState[1]->setVisible(false);
			m_pkCheckDiscardState[0]->setVisible(true);
			m_pkCheckDiscardState[1]->setVisible(false);
			m_pkCallAnyState[0]->setVisible(true);
			m_pkCallAnyState[1]->setVisible(false);
		}
		btn_call_any.SetEnable(bEnable);
		btn_auto_check.SetEnable(bEnable);
		btn_check_discard.SetEnable(bEnable);
		btn_discard.SetVisible(false);
		btn_check.SetVisible(false);
		btn_fill.SetVisible(false);
		btn_call.SetVisible(false);
		btn_call_any.SetVisible(true);
		btn_auto_check.SetVisible(true);
		btn_check_discard.SetVisible(true);
		return;
	default:
		break;
	}
}

void Game::DisableSetCotrolBtn()
{
	m_pkFillBar->SetVisible(false);
	WIDGET(UIButton,btn_discard);
	WIDGET(UIButton,btn_fill);
	WIDGET(UIButton,btn_check);
	WIDGET(UIButton,btn_call);
	btn_discard.SetEnable(false);
	btn_fill.SetEnable(false);
	btn_check.SetEnable(false);
	btn_call.SetEnable(false);
}

void Game::TurnTableCard( CCNode* pSender,void* TargetId )
{
	switch((int)TargetId)
	{
	case DRAW_CARD:
		g_pSoundSystem->PlayEffect(SE_POKER_ON_FLOP);
		for (unsigned int i(0);i<3;++i)
		{
			m_apkTableCard[i]->Turn();
		}
		break;
	case TURN_CARD:
		g_pSoundSystem->PlayEffect(SE_POKER_ON_TURN);
		m_apkTableCard[3]->Turn();
		break;
	case RIVER_CARD:
		g_pSoundSystem->PlayEffect(SE_POKER_ON_RIVER);
		m_apkTableCard[4]->Turn();
		break;
	case ALL_TABLE_CARD:
		g_pSoundSystem->PlayEffect(SE_POKER_ON_FLOP);
		for (unsigned int i(0);i<5;++i)
		{
			m_apkTableCard[i]->Turn();
		}
		break;
	case TURN_AND_RIVER_CARD:
		g_pSoundSystem->PlayEffect(SE_POKER_ON_FLOP);
		for (unsigned int i(3);i<5;++i)
		{
			m_apkTableCard[i]->Turn();
		}
		break;
	default:
		break;
	}
}

void Game::SetPlayerChipsNumberEffect(VeUInt32 u32Number)
{
	m_pkShowPlayerChips->SetNumber(u32Number);
	m_pkShowPlayerChips->SetVisible(u32Number ? true : false);
}

void Game::UpdateTable(bool bMotion)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeChar8 acBuffer[64];
	const RoomInfo& kInfo = pkPlayer->GetRoomInfo();
	VeUInt32 u8Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	const VeVector<TablePlayer>& kPlayers = pkPlayer->GetTablePlayers();
	SetSelfSeatIndex(u8Self);
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkLeft[i]->ShowSeat();
		m_apkRight[i]->ShowSeat();
	}
	VeHashSet<VeUInt32> kPlayingPlayers;
	for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
	{
		kPlayingPlayers.Insert(kTable.m_tPlayerIndex[i]);
	}
	for(VeUInt32 i(0); i < kPlayers.Size(); ++i)
	{
		const TablePlayer& kTablePlayer = kPlayers[i];
		bool bIsPlaying = kTable.m_tState && kPlayingPlayers.Find(kTablePlayer.m_u32Index);
		if(kTablePlayer.m_u32Index != u8Self)
		{
			VE_ASSERT(kTablePlayer.m_tState);
			NameCard* pkCard = GetCard(kTablePlayer.m_u32Index);
			pkCard->ShowBoard();
			pkCard->SetHeadIcon(kTablePlayer.m_u8HeadIcon, kTablePlayer.m_u8Sex ? false : true, kTablePlayer.m_u8Vip ? true : false);
			pkCard->SetName(kTablePlayer.m_strName);
			pkCard->SetDealer(kTablePlayer.m_u32Index == kTable.m_u8Dealer && kTable.m_tState > TABLE_WAIT);
			pkCard->SetOverlay(NameCard::OVERLAY_NULL);
			pkCard->SetDrink(kTablePlayer.m_u8Drink < 20 ? kTablePlayer.m_u8Drink : -1);
			if(!bIsPlaying)
			{
				pkCard->GetLeft()->SetVisible(false);
				pkCard->GetRight()->SetVisible(false);
				SetChipsOnTable(kTablePlayer.m_u32Index, 0, false);
				pkCard->SetProcess(PlayerCard::PROCESS_NULL);
				pkCard->SetChips(kTablePlayer.m_u32Chips);
				if(kTable.m_tState)
				{
					switch(kTablePlayer.m_tState)
					{
					case TABLE_PLAYER_HOLD:
						pkCard->SetOverlay(NameCard::OVERLAY_HOLD);
						break;
					case TABLE_PLAYER_READY:
						pkCard->SetOverlay(NameCard::OVERLAY_READY);
						break;
					case TABLE_PLAYER_FOLD:
						pkCard->SetOverlay(NameCard::OVERLAY_FOLD);
						break;
					default:
						break;
					}
				}
				pkCard->SetWin(0);
			}
			else if(kTable.m_tState < TABLE_CLEAR)
			{
				pkCard->SetWin(0);
			}
		}
		else
		{
			if(kTable.m_tState < TABLE_CLEAR)
			{
				m_pkPlayerCard->SetWin(false);
				m_pkPlayWin->SetVisible(false);
			}
			m_pkPlayerCard->SetHeadIcon(pkPlayer->GetHeadIcon(), pkPlayer->GetSex() ? false : true, pkPlayer->GetVip() ? true : false);
			m_pkPlayerCard->SetName(pkPlayer->GetNickname());
			VeSprintf(acBuffer, 64, "TitleType%d", pkPlayer->GetTitle());
			m_pkPlayerCard->SetTitle(TT(acBuffer));
			SetSelfDrink(kTablePlayer.m_u8Drink < 20 ? kTablePlayer.m_u8Drink : -1);
			VeFloat32 f32WinRate(0);
			if(pkPlayer->GetPlayCount())
				f32WinRate = VeFloat32(pkPlayer->GetWinCount()) / VeFloat32(pkPlayer->GetPlayCount());
			m_pkPlayerCard->SetGameData(pkPlayer->GetWinMaxBet(), f32WinRate);
			m_pkPlayerCard->SetStrength(0, false);
			m_pkPlayerCard->SetDealer(u8Self == kTable.m_u8Dealer && kTable.m_tState > TABLE_WAIT);
			if(kTablePlayer.m_tState == TABLE_PLAYER_HOLD)
			{
				if(GetUILayer(ChipsCarry, "chips_carry")->IsShow() == false)
					g_pLayerManager->PushLayer("chips_carry");
			}
			if(!bIsPlaying)
			{	
				m_apkSelfCard[0]->SetVisible(false);
				m_apkSelfCard[1]->SetVisible(false);
				SetChipsOnTable(u8Self, 0, false);
				m_pkPlayerCard->SetProcess(PlayerCard::PROCESS_NULL);
				SetPlayerChipsNumberEffect(kTablePlayer.m_u32Chips);
				m_pkPlayerCard->SetWin(false);
				m_pkPlayWin->SetVisible(false);
			}
		}
	}
	SetCategories(pkPlayer->GetCategories());
	for(VeUInt32 i(0); i < 2; ++i)
	{
		m_apkSelfCard[i]->SetLight(pkPlayer->IsHighLight(i));
	}
	for(VeUInt32 i(0); i < 5; ++i)
	{
		m_apkTableCard[i]->SetLight(pkPlayer->IsHighLight(i + 2));
	}
	
	bool bChange = !(m_eStateLastUpdate == kTable.m_tState
		&& m_u32ProcessLastUpdate == kTable.m_u8Process
		&& m_u64ProcessLastExpireUpdate == kTable.m_u64ProcessExpire);
	if(bMotion && (!bChange)) return;
	if(!bMotion)
	{
		ResetAllCards();
		ResetAllChips();
		SetAllCardsType(VE_MASK_HAS_ALL(kInfo.m_u8Flags, ROOM_FLAG_VIP) ? 1 : 0);
		WIDGET(GameTableBoard, table_board);
		table_board.Set(kInfo.m_u32SB, kInfo.m_u32BB);
		unscheduleAllSelectors();
		VeZeroMemory(m_au32Chips, sizeof(m_au32Chips));
	}
	if(bChange || ((!bMotion) && (!bChange)))
	{
		ResetAllNameCards();
		const TablePlayer& kSelfPlayer = *pkPlayer->GetTablePlayer(u8Self);
		bool bIsPlaying = kTable.m_tState && kPlayingPlayers.Find(u8Self);
		if(!bIsPlaying)
		{
			SetCotrolBtnState(CONTROL_AUTO, false);
		}
		else if(kSelfPlayer.m_tState == TABLE_PLAYER_ALL_IN || kSelfPlayer.m_tState == TABLE_PLAYER_FOLD || kTable.m_tState >= TABLE_CLEAR || kSelfPlayer.m_tState == TABLE_PLAYER_HOLD)
		{
			SetCotrolBtnState(CONTROL_AUTO, false);
		}
		else if(kTable.m_tState >= TABLE_PLAY_0 && u8Self == kTable.m_tPlayerIndex[kTable.m_u8Process])
		{
			bool bCanProcess = kTable.m_u64ProcessExpire ? true : false;
			bool bCheck = kTable.m_u32BetChips == kSelfPlayer.m_u32ChipsOnTable;
			if(bCanProcess && m_eAutoState)
			{
				switch(m_eAutoState)
				{
				case AUTO_CALL_ANY:
					bCanProcess = false;
					pkPlayer->S_ReqCall();
					m_eAutoState = AUTO_MAX;
					break;
				case AUTO_CHECK:
					if(bCheck)
					{
						bCanProcess = false;
						pkPlayer->S_ReqCall();
						m_eAutoState = AUTO_MAX;
					}
					break;
				case AUTO_CHECK_OR_DISCARD:
					if(bCheck)
					{
						bCanProcess = false;
						pkPlayer->S_ReqCall();
					}
					else
					{
						bCanProcess = false;
						pkPlayer->S_ReqFold();
					}
					m_eAutoState = AUTO_MAX;
				default:
					break;
				}
				m_pkAutoCheckState[0]->setVisible(true);
				m_pkAutoCheckState[1]->setVisible(false);
				m_pkCheckDiscardState[0]->setVisible(true);
				m_pkCheckDiscardState[1]->setVisible(false);
				m_pkCallAnyState[0]->setVisible(true);
				m_pkCallAnyState[1]->setVisible(false);
			}
			if(bCanProcess && bMotion && m_eAutoState < AUTO_MAX)
			{
				g_pSoundSystem->PlayEffect(SE_POKER_ON_MYTURN);
			}
			SetCotrolBtnState(bCheck ? CONTROL_CHECK : CONTROL_CALL, bCanProcess);
			if((kSelfPlayer.m_u32Chips + kSelfPlayer.m_u32ChipsOnTable) < (kTable.m_u32BetChips + kInfo.m_u32BB))
			{
				WIDGET(UIButton,btn_fill);
				btn_fill.SetEnable(false);
			}
		}
		else
		{
			if(m_eAutoState >= AUTO_MAX)
				m_eAutoState = AUTO_NULL;
			SetCotrolBtnState(CONTROL_AUTO, true);
		}
	}
	unschedule(schedule_selector(Game::UpdateProcessTime));
	if(m_eStateLastUpdate != kTable.m_tState && bMotion)
	{
		switch(kTable.m_tState)
		{
		case TABLE_DEAL_0:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_START);
			break;
		case TABLE_DEAL_1:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_FLOP);
			break;
		case TABLE_DEAL_2:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_TURN);
			break;
		case TABLE_DEAL_3:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_RIVER);
			break;
		default:
			break;
		}
	}
	if(kTable.m_tState && m_u64ProcessLastExpireUpdate != kTable.m_u64ProcessExpire && kTable.m_u64ProcessExpire == 0)
	{
		const TablePlayer& kProcessPlayer = *pkPlayer->GetTablePlayer(kTable.m_tPlayerIndex[kTable.m_u8Process]);
		switch(kProcessPlayer.m_tState)
		{
		case TABLE_PLAYER_FOLD:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_DISS);
			break;
		case TABLE_PLAYER_CHECK:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_CHECK);
			break;
		case TABLE_PLAYER_ALL_IN:
			g_pSoundSystem->PlayEffect(SE_POKER_ON_ALLIN);
			break;
		default:
			break;
		}
	}

	if(kTable.m_tState == TABLE_WAIT)
	{
		WIDGET(ChipsBar, pond_chips);
		pond_chips.SetNumber(0);
		return;
	}

	if(bMotion && kTable.m_tState == TABLE_DEAL_0)
	{
		VeFloat32 f32Delay(0);
		scheduleOnce(schedule_selector(Game::ShowSB), f32Delay);
		f32Delay += NORMAL_DELAY;
		scheduleOnce(schedule_selector(Game::ShowBB), f32Delay);
		f32Delay += SEND_CARD_DELAY;
		for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
		{
			VeUInt32 u32Index = kTable.m_tPlayerIndex[(i+1)%kTable.m_tPlayerIndex.Size()];
			const TablePlayer& kTablePlayer = *pkPlayer->GetTablePlayer(u32Index);
			PlayerCard::Process eProcess = kTablePlayer.m_tState > TABLE_PLAYER_PLAY ? PlayerCard::Process(kTablePlayer.m_tState - TABLE_PLAYER_PLAY) : PlayerCard::PROCESS_NULL;
			if(eProcess == PlayerCard::PROCESS_SMALL_BLIND || eProcess == PlayerCard::PROCESS_BIG_BLIND) eProcess = PlayerCard::PROCESS_NULL;
			if(kTablePlayer.m_u32Index != u8Self)
			{
				VE_ASSERT(kTablePlayer.m_tState);
				NameCard* pkCard = GetCard(kTablePlayer.m_u32Index);
				pkCard->SetProcess(eProcess);
				pkCard->SetChips(kTablePlayer.m_u32Chips);
				pkCard->GetRight()->Deal(f32Delay);
				f32Delay += SEND_CARD_DELAY;
				pkCard->GetLeft()->Deal(f32Delay);
				f32Delay += SEND_CARD_DELAY;
			}
			else
			{
				m_pkPlayerCard->SetProcess(eProcess);
				SetPlayerChipsNumberEffect(kTablePlayer.m_u32Chips);
				m_apkSelfCard[1]->Deal(f32Delay);
				f32Delay += SEND_CARD_DELAY;
				m_apkSelfCard[0]->Deal(f32Delay);
				f32Delay += SEND_CARD_DELAY;
				for(VeUInt32 j(0); j < 2; ++j)
				{
					const CardData& kCard = pkPlayer->GetHandCards()[j];
					m_apkSelfCard[j]->SetContent(kCard.m_tNumber, kCard.m_tSuit);
					m_apkSelfCard[j]->Turn(f32Delay);
				}
			}
		}
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("room", VeStringA::From(kInfo.m_u32ChipsMax)));
		NativeOnEvent("StartPoker", kEventValue);
	}
	else if(bMotion && kTable.m_tState > TABLE_DEAL_0 && kTable.m_tState <= TABLE_DEAL_3)
	{
		WIDGET(ChipsBar, pond_chips);
		pond_chips.SetNumber(pkPlayer->GetTablePool());
		for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
		{
			VeUInt32 u32Index = kTable.m_tPlayerIndex[i];
			const TablePlayer& kTablePlayer = *pkPlayer->GetTablePlayer(u32Index);
			PlayerCard::Process eProcess = PlayerCard::PROCESS_NULL;
			if(kTablePlayer.m_u32Index != u8Self)
			{
				NameCard* pkCard = GetCard(kTablePlayer.m_u32Index);
				pkCard->SetProcess(eProcess);
				pkCard->SetChips(kTablePlayer.m_u32Chips);
			}
			else
			{
				m_pkPlayerCard->SetProcess(eProcess);
				SetPlayerChipsNumberEffect(kTablePlayer.m_u32Chips);
			}
			SetChipsOnTable(kTablePlayer.m_u32Index, kTablePlayer.m_u32ChipsOnTable, bMotion);
		}
		switch((kTable.m_tState - 1) % 4)
		{
		case 1:
			for(VeUInt32 i(0); i < 3; ++i)
			{
				VE_ASSERT(kTable.m_tCardList.Size() == 3);
				const CardData& kCardData = kTable.m_tCardList[i];
				m_apkTableCard[i]->SetVisible(true);
				m_apkTableCard[i]->SetContent(kCardData.m_tNumber, kCardData.m_tSuit);
				m_apkTableCard[i]->Deal(SEND_CARD_DELAY * (i + 1));
				m_apkTableCard[i]->Turn(SEND_CARD_DELAY * 4);
			}
			break;
		case 2:
			{
				VE_ASSERT(kTable.m_tCardList.Size() == 4);
				const CardData& kCardData = kTable.m_tCardList[3];
				m_apkTableCard[3]->SetVisible(true);
				m_apkTableCard[3]->SetContent(kCardData.m_tNumber, kCardData.m_tSuit);
				m_apkTableCard[3]->Deal(SEND_CARD_DELAY * 1);
				m_apkTableCard[3]->Turn(SEND_CARD_DELAY * 2);
			}
			break;
		default:
			{
				VE_ASSERT(kTable.m_tCardList.Size() == 5);
				const CardData& kCardData = kTable.m_tCardList[4];
				m_apkTableCard[4]->SetVisible(true);
				m_apkTableCard[4]->SetContent(kCardData.m_tNumber, kCardData.m_tSuit);
				m_apkTableCard[4]->Deal(SEND_CARD_DELAY * 1);
				m_apkTableCard[4]->Turn(SEND_CARD_DELAY * 2);
			}
			break;
		}
	}
	else if(bMotion && kTable.m_tState >= TABLE_CLEAR)
	{
		VeUInt32 u32Pool(0);
		for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
		{
			VeUInt32 u32Index = kTable.m_tPlayerIndex[i];
			const TablePlayer& kTablePlayer = *pkPlayer->GetTablePlayer(u32Index);
			SetChipsOnTable(u32Index, 0, bMotion);
			u32Pool += kTablePlayer.m_u32ChipsOnTable;
		}
		WIDGET(ChipsBar, pond_chips);
		pond_chips.SetNumber(u32Pool);
		if(kTable.m_tState == TABLE_CLEAR)
		{
			scheduleOnce(schedule_selector(Game::OpenCards), 2.0f);
		}
		else
		{
			scheduleOnce(schedule_selector(Game::WinChips), 2.0f);
		}
	}
	else
	{
		UpdateTablePlayer(bMotion);
	}
	m_eStateLastUpdate = kTable.m_tState;
	m_u32ProcessLastUpdate = kTable.m_u8Process;
	m_u64ProcessLastExpireUpdate = kTable.m_u64ProcessExpire;
}

void Game::UpdateTablePlayer(bool bMotion)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_pkPlayWin->SetVisible(false);
	VeUInt32 u8Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	WIDGET(ChipsBar, pond_chips);
	pond_chips.SetNumber(pkPlayer->GetTablePool());
	for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
	{
		VeUInt32 u32Index = kTable.m_tPlayerIndex[i];
		const TablePlayer& kTablePlayer = *pkPlayer->GetTablePlayer(u32Index);
		PlayerCard::Process eProcess = kTablePlayer.m_tState > TABLE_PLAYER_PLAY ? PlayerCard::Process(kTablePlayer.m_tState - TABLE_PLAYER_PLAY) : PlayerCard::PROCESS_NULL;
		bool bFold = kTablePlayer.m_tState == TABLE_PLAYER_FOLD;
		if(kTablePlayer.m_u32Index != u8Self)
		{
			NameCard* pkCard = GetCard(kTablePlayer.m_u32Index);
			pkCard->SetChips(kTablePlayer.m_u32Chips);
			pkCard->GetLeft()->SetVisible(!bFold);
			pkCard->GetRight()->SetVisible(!bFold);
			if(kTable.m_tState == TABLE_CLEAR)
			{
				const CardData& kLeft = kTable.m_tHandCardList[i*2];
				pkCard->GetLeft()->SetContent(kLeft.m_tNumber, kLeft.m_tSuit);
				pkCard->GetLeft()->SetFace(true);
				const CardData& kRight = kTable.m_tHandCardList[i*2+1];
				pkCard->GetRight()->SetContent(kRight.m_tNumber, kRight.m_tSuit);
				pkCard->GetRight()->SetFace(true);
				if(kTablePlayer.m_tState != TABLE_PLAYER_FOLD)
				{
					pkCard->SetProcess(PlayerCard::Process(pkPlayer->GetCategories(i) + PlayerCard::PROCESS_CATE_ROYAL_FLUSH));
					pkCard->SetWin(kTablePlayer.m_u32ChipsOnTable);
				}
				else
				{
					pkCard->SetProcess(eProcess);
					pkCard->SetWin(0);
				}
			}
			else
			{
				pkCard->GetLeft()->SetFace(false);
				pkCard->GetRight()->SetFace(false);
				pkCard->SetProcess(eProcess);
				pkCard->SetWin(kTable.m_tState == TABLE_ALL_FOLD ? kTablePlayer.m_u32ChipsOnTable : 0);
			}
		}
		else
		{
			SetPlayerChipsNumberEffect(kTablePlayer.m_u32Chips);
			for(VeUInt32 j(0); j < 2; ++j)
			{
				m_apkSelfCard[j]->SetVisible(!bFold);
				m_apkSelfCard[j]->SetFace(true);
				const CardData& kCard = pkPlayer->GetHandCards()[j];
				m_apkSelfCard[j]->SetContent(kCard.m_tNumber, kCard.m_tSuit);
			}
			if(kTable.m_tState == TABLE_CLEAR)
			{
				if(kTablePlayer.m_tState != TABLE_PLAYER_FOLD)
				{
					m_pkPlayerCard->SetProcess(PlayerCard::Process(pkPlayer->GetCategories() + PlayerCard::PROCESS_CATE_ROYAL_FLUSH));
					m_pkPlayerCard->SetWin(kTablePlayer.m_u32ChipsOnTable ? true : false);
					if(kTablePlayer.m_u32ChipsOnTable)
					{
						m_pkPlayWin->Show(kTablePlayer.m_u32ChipsOnTable, false);
					}
				}
				else
				{
					m_pkPlayerCard->SetProcess(eProcess);
					m_pkPlayerCard->SetWin(false);
				}
			}
			else
			{
				m_pkPlayerCard->SetProcess(eProcess);
				m_pkPlayerCard->SetWin(kTable.m_tState == TABLE_ALL_FOLD && kTablePlayer.m_u32ChipsOnTable);
				if(kTable.m_tState == TABLE_ALL_FOLD && kTablePlayer.m_u32ChipsOnTable)
				{
					m_pkPlayWin->Show(kTablePlayer.m_u32ChipsOnTable, false);
				}
			}
		}
		if(kTable.m_tState < TABLE_CLEAR)
		{
			SetChipsOnTable(kTablePlayer.m_u32Index, kTablePlayer.m_u32ChipsOnTable, bMotion);
			if(kTable.m_tState && kTable.m_tState >= TABLE_PLAY_0)
			{
				if(u8Self != kTable.m_tPlayerIndex[kTable.m_u8Process] || m_eAutoState != AUTO_MAX)
				{
					schedule(schedule_selector(Game::UpdateProcessTime), 0.0f);
				}				
			}
		}
		for(VeUInt32 i(0); i < 5; ++i)
		{
			if(i < kTable.m_tCardList.Size())
			{
				const CardData& kCardData = kTable.m_tCardList[i];
				m_apkTableCard[i]->SetVisible(true);
				m_apkTableCard[i]->SetContent(kCardData.m_tNumber, kCardData.m_tSuit);
				m_apkTableCard[i]->SetFace(true);
			}
			else
			{
				m_apkTableCard[i]->SetVisible(false);
			}
		}
	}
}

void Game::ResetAllCards()
{
	for(VeUInt32 i(0); i < 2; ++i)
	{
		m_apkSelfCard[i]->Reset();
	}
	for(VeUInt32 i(0); i < 5; ++i)
	{
		m_apkTableCard[i]->Reset();
	}
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkLeft[i]->GetLeft()->Reset();
		m_apkLeft[i]->GetRight()->Reset();
		m_apkRight[i]->GetLeft()->Reset();
		m_apkRight[i]->GetRight()->Reset();
	}
}

void Game::SetAllCardsType(VeUInt32 u32Type)
{
	for(VeUInt32 i(0); i < 2; ++i)
	{
		m_apkSelfCard[i]->SetType(u32Type);
	}
	for(VeUInt32 i(0); i < 5; ++i)
	{
		m_apkTableCard[i]->SetType(u32Type);
	}
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkLeft[i]->GetLeft()->SetType(u32Type);
		m_apkLeft[i]->GetRight()->SetType(u32Type);
		m_apkRight[i]->GetLeft()->SetType(u32Type);
		m_apkRight[i]->GetRight()->SetType(u32Type);
	}
	WIDGET(UIPageView, grade_page_view);
	grade_page_view.Change(u32Type);
}

void Game::ResetAllChips()
{
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkLeftChips[i]->Reset();
		m_apkRightChips[i]->Reset();
	}
	m_pkPlayerChips->Reset();
}

void Game::ResetAllNameCards()
{
	m_pkPlayerCard->SetRate(1.0f);
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkLeft[i]->SetRate(1.0f);
		m_apkRight[i]->SetRate(1.0f);
	}
}

void Game::ShowSB(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeUInt32 u32Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	VeUInt32 u32SBIndex = kTable.m_tPlayerIndex[kTable.m_u8SmallBlind];
	const TablePlayer* pkSBPlayer = pkPlayer->GetTablePlayer(u32SBIndex);
	VE_ASSERT(pkSBPlayer);
	PlayerCard::Process eProcess = PlayerCard::PROCESS_SMALL_BLIND;
	if(pkSBPlayer->m_u32Index != u32Self)
	{
		NameCard* pkCard = GetCard(pkSBPlayer->m_u32Index);
		pkCard->SetProcess(eProcess);
		pkCard->SetChips(pkSBPlayer->m_u32Chips);
	}
	else
	{
		m_pkPlayerCard->SetProcess(eProcess);
		SetPlayerChipsNumberEffect(pkSBPlayer->m_u32Chips);
	}
	SetChipsOnTable(pkSBPlayer->m_u32Index, pkSBPlayer->m_u32ChipsOnTable, true);
}

void Game::ShowBB(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeUInt32 u32Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	VeUInt32 u32BBIndex = kTable.m_tPlayerIndex[(kTable.m_u8SmallBlind + 1) % kTable.m_tPlayerIndex.Size()];
	const TablePlayer* pkBBPlayer = pkPlayer->GetTablePlayer(u32BBIndex);
	VE_ASSERT(pkBBPlayer);
	PlayerCard::Process eProcess = PlayerCard::PROCESS_BIG_BLIND;
	if(pkBBPlayer->m_u32Index != u32Self)
	{
		NameCard* pkCard = GetCard(pkBBPlayer->m_u32Index);
		pkCard->SetProcess(eProcess);
		pkCard->SetChips(pkBBPlayer->m_u32Chips);
	}
	else
	{
		m_pkPlayerCard->SetProcess(eProcess);
		SetPlayerChipsNumberEffect(pkBBPlayer->m_u32Chips);
	}
	SetChipsOnTable(pkBBPlayer->m_u32Index, pkBBPlayer->m_u32ChipsOnTable, true);
}

void Game::WinChips(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeUInt32 u32Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	WIDGET(ChipsBar, pond_chips);
	pond_chips.SetNumber(pkPlayer->GetTablePool());
	Categories eMax = Categories_MAX;
	for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
	{
		VeUInt32 u32Index = kTable.m_tPlayerIndex[i];
		const TablePlayer& kTablePlayer = *pkPlayer->GetTablePlayer(u32Index);
		if(kTablePlayer.m_u32ChipsOnTable)
		{
			GameChips* pkChips = GetChips(u32Index);
			GameChipsMove* pkMove = m_apkChipsMove[u32Index];
			pkMove->SetFadePos(pkChips->FadeOutPos(), pkChips->FadeInPos());
			pkMove->Move(kTablePlayer.m_u32ChipsOnTable);
		}
		if(kTablePlayer.m_u32Index != u32Self)
		{
			NameCard* pkCard = GetCard(kTablePlayer.m_u32Index);
			pkCard->SetChips(kTablePlayer.m_u32Chips);
			if(kTable.m_tState == TABLE_CLEAR)
			{
				const CardData& kLeft = kTable.m_tHandCardList[i*2];
				pkCard->GetLeft()->SetContent(kLeft.m_tNumber, kLeft.m_tSuit);
				pkCard->GetLeft()->SetFace(true);
				const CardData& kRight = kTable.m_tHandCardList[i*2+1];
				pkCard->GetRight()->SetContent(kRight.m_tNumber, kRight.m_tSuit);
				pkCard->GetRight()->SetFace(true);
				if(kTablePlayer.m_tState != TABLE_PLAYER_FOLD)
				{
					Categories eCate = pkPlayer->GetCategories(i);
					eMax = VE_MIN(eCate, eMax);
					pkCard->SetProcess(PlayerCard::Process(eCate + PlayerCard::PROCESS_CATE_ROYAL_FLUSH));
				}
				else
				{
					pkCard->SetProcess(PlayerCard::PROCESS_FOLD);
				}
			}
			pkCard->SetWin(kTablePlayer.m_u32ChipsOnTable);
		}
		else
		{
			if(kTable.m_tState == TABLE_CLEAR)
			{
				if(kTablePlayer.m_tState != TABLE_PLAYER_FOLD)
				{
					Categories eCate = pkPlayer->GetCategories();
					eMax = VE_MIN(eCate, eMax);
					m_pkPlayerCard->SetProcess(PlayerCard::Process(eCate + PlayerCard::PROCESS_CATE_ROYAL_FLUSH));
				}
				else
				{
					m_pkPlayerCard->SetProcess(PlayerCard::PROCESS_FOLD);
				}
			}
			m_pkPlayerCard->SetWin(kTablePlayer.m_u32ChipsOnTable ? true : false);
			SetPlayerChipsNumberEffect(kTablePlayer.m_u32Chips);
			if(kTablePlayer.m_u32ChipsOnTable)
			{
				m_pkPlayWin->Show(kTablePlayer.m_u32ChipsOnTable, true);
			}
		}
	}
	if(eMax < Categories_MAX)
	{
		if(eMax < CATE_FLUSH)
		{
			g_pSoundSystem->PlayEffect(SE_POKER_WIN_2);
		}
		else if(eMax < CATE_ONE_PAIR)
		{
			g_pSoundSystem->PlayEffect(SE_POKER_WIN_1);
		}
		else
		{
			g_pSoundSystem->PlayEffect(SE_POKER_WIN_0);
		}
		g_pSoundSystem->PlayEffect(SE_WIN_CATE_0 + eMax);
	}
	else
	{
		g_pSoundSystem->PlayEffect(SE_CHIPS_WIN);
	}
}

void Game::OpenCards(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeUInt32 u32Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	for(VeUInt32 i(0); i < kTable.m_tPlayerIndex.Size(); ++i)
	{
		VeUInt32 u32Index = kTable.m_tPlayerIndex[i];
		const TablePlayer& kTablePlayer = *pkPlayer->GetTablePlayer(u32Index);
		if(kTablePlayer.m_u32Index != u32Self)
		{
			NameCard* pkCard = GetCard(kTablePlayer.m_u32Index);
			const CardData& kLeft = kTable.m_tHandCardList[i*2];
			pkCard->GetLeft()->SetContent(kLeft.m_tNumber, kLeft.m_tSuit);
			pkCard->GetLeft()->Turn();
			const CardData& kRight = kTable.m_tHandCardList[i*2+1];
			pkCard->GetRight()->SetContent(kRight.m_tNumber, kRight.m_tSuit);
			pkCard->GetRight()->Turn();
		}
	}
	scheduleOnce(schedule_selector(Game::WinChips), 0.5f);
}

void Game::UpdateProcessTime(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent)
	{
		unschedule(schedule_selector(Game::UpdateProcessTime));
		return;
	}
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer)
	{
		unschedule(schedule_selector(Game::UpdateProcessTime));
		return;
	}
	VeUInt32 u32Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	VeUInt32 u32Index = kTable.m_tPlayerIndex[kTable.m_u8Process];
	VeUInt64 u64Pass = 0;
	if(kTable.m_u64ProcessExpire > g_pkGame->GetServerTime())
	{
		u64Pass = kTable.m_u64ProcessExpire - g_pkGame->GetServerTime();
	}
	VeFloat32 f32Rate;
	if(u64Pass > 1680)
	{
		f32Rate = 0.0f;
	}
	else if(u64Pass > 180)
	{
		f32Rate = 1.0f - VeFloat32(u64Pass - 180) / VeFloat32(1500);
	}
	else
	{
		f32Rate = 1.0f;
	}
	if(u32Index == u32Self)
	{
		m_pkPlayerCard->SetRate(f32Rate);
	}
	else
	{
		NameCard* pkCard = GetCard(u32Index);
		pkCard->SetRate(f32Rate);
	}
}

void Game::UpdateSlider()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	const RoomInfo& kInfo = pkPlayer->GetRoomInfo();
	VeUInt32 u32Self = pkPlayer->GetSeatIndex();
	const Table& kTable = pkPlayer->GetTableState();
	VeUInt32 u32Index = kTable.m_tPlayerIndex[kTable.m_u8Process];
	if(u32Index == u32Self)
	{
		const TablePlayer& kSelf = *pkPlayer->GetTablePlayer(u32Self);
		if(m_pkSlider->GetValue() == 1.0f)
		{
			m_pkBetNumber->setVisible(false);
			m_pkBetAllin->setVisible(true);
			m_pkAllinText->setVisible(true);
			m_u32SelfBetNumber = ((kSelf.m_u32Chips + kSelf.m_u32ChipsOnTable) - kTable.m_u32BetChips);
		}
		else
		{
			VeUInt32 u32BetNumber = m_pkSlider->GetValue() * ((kSelf.m_u32Chips + kSelf.m_u32ChipsOnTable) - kTable.m_u32BetChips);
			u32BetNumber += kInfo.m_u32BB - 1;
			u32BetNumber -= u32BetNumber % kInfo.m_u32BB;
			SetBetNumber(u32BetNumber + kTable.m_u32BetChips);
			m_pkBetNumber->setVisible(true);
			m_pkBetAllin->setVisible(false);
			m_pkAllinText->setVisible(false);
			m_u32SelfBetNumber = u32BetNumber;
		}
	}
}

void Game::OnBuy(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	if(u8From != u8To)
	{
		GiftItem::Type eType = u8Type & 0x80 ? GiftItem::TYPE_OBJECT : GiftItem::TYPE_DRINK;
		u8Type &= 0x7F;
		GameDrinksMove* pkFrom = GetDrink(u8From);
		GameDrinksMove* pkTo = GetDrink(u8To);
		pkTo->SetFadeInPos(pkFrom->GetFadeOutPos());
		pkTo->Move(eType, u8Type);
	}
}

void Game::OnBuyAll(VeUInt8 u8Type, VeUInt8 u8From)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	const VeVector<TablePlayer>& kPlayers = pkPlayer->GetTablePlayers();
	for(VeUInt32 i(0); i < kPlayers.Size(); ++i)
	{
		const TablePlayer& kPlayer = kPlayers[i];
		OnBuy(u8Type, u8From, kPlayer.m_u32Index);
	}
}

void Game::OnChat(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To, const VeChar8* strChat)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	//if(u8From != u8To)
	{
		GiftItem::Type eType = u8Type & 0x80 ? GiftItem::TYPE_OBJECT : GiftItem::TYPE_DRINK;
		u8Type &= 0x7F;
		HeadChatBar* pkChatBar = GetHeadChatBar(u8From);
		if(pkChatBar != NULL)
		{
			pkChatBar->Chat(strChat, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale));
		}
	}
}

void Game::OnChatAll(VeUInt8 u8Type, VeUInt8 u8From, const VeChar8* strChat)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	const VeVector<TablePlayer>& kPlayers = pkPlayer->GetTablePlayers();
	for(VeUInt32 i(0); i < kPlayers.Size(); ++i)
	{
		if(u8From == i)
		{
			const TablePlayer& kPlayer = kPlayers[i];
			OnChat(u8Type, u8From, kPlayer.m_u32Index, strChat);
		}
	}
}

void Game::OnPushNotice(const VeChar8* pcNotice)
{
	WIDGET(NoticeBar, notice_bar);
	notice_bar.Notice(pcNotice, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale));
}
