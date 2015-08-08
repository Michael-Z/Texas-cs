#include "Hall.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "UITableView.h"
#include "RoomChoose.h"
#include "Knickknacks.h"
#include "OptionView.h"
#include "SoundSystem.h"
#include "EventList.h"
#include "Game.h"
#include "Slot.h"
#include "Keno.h"
#include "Baccarat.h"
#include "Course.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"
#include "NativeInterface.h"

USING_NS_CC;

Hall::Hall() : UILayerExt<Hall>("hall"), m_pkLight(NULL), m_bIsGameCreate(false), m_bIsMiniGameCreate(false)
{
	UIInitDelegate(Hall, OnPageChange);
	UIInitDelegate(Hall, OnRoomClicked);
	UIInitDelegate(Hall, OnGetChipsClicked);
	UIInitDelegate(Hall, OnBankClicked);

	UIInitDelegate(Hall, OnMini0);
	UIInitDelegate(Hall, OnMini1);
	UIInitDelegate(Hall, OnMini2);

	UIInitDelegate(Hall, OnEventAction);

	m_kOnFadeOut.Set(this, &Hall::OnFadeOut);
	m_kOnFadeOutMini.Set(this, &Hall::OnFadeOutMini);

	m_u32MiniGameNeeded = VE_INFINITE;
}

Hall::~Hall()
{
	term();
}

bool Hall::init()
{
	CCSpriteBatchNode* pkBatchNodeHall = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	addChild(pkBatchNodeHall, 0);

	for(VeUInt32 i(0); i < 4; ++i)
	{
		CCSprite* pkBackground = CCSprite::createWithSpriteFrameName("bg_0.png");
		pkBackground->setAnchorPoint(ccp(1.0f, 0.0f));
		pkBackground->setPosition(ui_ccp(0.5f, 0.5f));
		pkBackground->setBlendFunc(kCCBlendFuncDisable);
		switch(i)
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
		pkBatchNodeHall->addChild(pkBackground, 0);
	}
	CCSprite* pkRoomL = CCSprite::createWithSpriteFrameName("bg_room.png");
	pkRoomL->setAnchorPoint(ccp(1.0f, 0.0f));
	pkRoomL->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale)));
	pkRoomL->setScale(ui_main_scale);
	CCSprite* pkRoomR = CCSprite::createWithSpriteFrameName("bg_room.png");
	pkRoomR->setAnchorPoint(ccp(1.0f, 0.0f));
	pkRoomR->setPosition(ccp(ui_w(0.5f),
		VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale)));
	pkRoomR->setScaleX(-ui_main_scale);
	pkRoomR->setScaleY(ui_main_scale);
	pkBatchNodeHall->addChild(pkRoomL, 0);
	pkBatchNodeHall->addChild(pkRoomR, 0);

	CCSprite* pkMiniL = CCSprite::createWithSpriteFrameName("bg_mini.png");
	pkMiniL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkMiniL->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 84.0f * ui_main_scale) * g_f32ScaleHD)));
	pkMiniL->setScale(ui_main_scale);
	CCSprite* pkMiniR = CCSprite::createWithSpriteFrameName("bg_mini.png");
	pkMiniR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkMiniR->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 84.0f * ui_main_scale) * g_f32ScaleHD)));
	pkMiniR->setScaleX(-ui_main_scale);
	pkMiniR->setScaleY(ui_main_scale);
	pkBatchNodeHall->addChild(pkMiniL, 0);
	pkBatchNodeHall->addChild(pkMiniR, 0);

	UIButton* pkGetChips = VE_NEW UIButton("get_chips_n.png", "get_chips_p.png");
	pkGetChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 120.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(40.0f * g_f32ScaleHD * ui_main_scale)));
	pkGetChips->setScale(ui_main_scale);
	AddWidget("get_chips", pkGetChips);
	UILinkDelegate(pkGetChips, OnGetChipsClicked);

	UIButton* pkBank = VE_NEW UIButton("bank_n.png", "bank_p.png");
	pkBank->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 46.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(56.0f * g_f32ScaleHD * ui_main_scale)));
	pkBank->setScale(ui_main_scale);
	AddWidget("bank", pkBank);
	UILinkDelegate(pkBank, OnBankClicked);

	HeadArea* pkHeadArea = VE_NEW HeadArea;
	pkHeadArea->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 118.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(42.0f * g_f32ScaleHD * ui_main_scale)));
	AddWidget("head_area", pkHeadArea, 0);

	NoticeBar* pkNoticeBar = VE_NEW NoticeBar;
	pkNoticeBar->setPositionY(VeFloorf(92.0f * g_f32ScaleHD * ui_main_scale));
	AddWidget("notice_bar", pkNoticeBar, 0);

	NameLabel* pkName = VE_NEW NameLabel;
	pkName->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 74.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(58.0f * g_f32ScaleHD * ui_main_scale)));
	AddWidget("name", pkName, 0);

	ChipsBar* pkChipsBar = VE_NEW ChipsBar;
	pkChipsBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 82.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale)));
	AddWidget("chips_bar", pkChipsBar, 0);

	GoldBar* pkGoldBar = VE_NEW GoldBar;
	pkGoldBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 0 * g_f32ScaleHD * ui_main_scale),
		VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale)));
	AddWidget("gold_bar", pkGoldBar, 0);

	AllOnline* pkAllOnline = VE_NEW AllOnline;
	pkAllOnline->setPosition(ccp(VeFloorf(6.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(84.0f * g_f32ScaleHD * ui_height_scale)));
	AddWidget("all_online", pkAllOnline, 1);

	UITableView* pkMainView = VE_NEW UITableView(3);
	UIChooseButton* pkBtnGame = VE_NEW UIChooseButton("btn_game_n.png", "btn_game_p.png");
	pkBtnGame->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 105.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - 16.0f * g_f32ScaleHD * ui_main_scale)));
	pkBtnGame->setScale(ui_main_scale);
	UIChooseButton* pkBtnAction = VE_NEW UIChooseButton("btn_action_n.png", "btn_action_p.png");
	pkBtnAction->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 1.0f - 16.0f * g_f32ScaleHD * ui_main_scale)));
	pkBtnAction->setScale(ui_main_scale);

	m_pkNotify = UIBarH::createWithEl("notification", 20 * g_f32ScaleHD);
	m_pkNotify->setPosition(ccp(-VeFloorf(18.0f * g_f32ScaleHD), -VeFloorf(7.0f * g_f32ScaleHD)));
	pkBtnAction->addChild(m_pkNotify, 5);
	m_pkEventNumber = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkEventNumber->setScale(1.0f / ui_main_scale);
	m_pkEventNumber->setPosition(ccp(-VeFloorf(18.0f * g_f32ScaleHD), VeFloorf(5.0f * g_f32ScaleHD)));
	pkBtnAction->addChild(m_pkEventNumber, 6);
	SetEventNumber(0);

	UIChooseButton* pkBtnOption = VE_NEW UIChooseButton("btn_option_n.png", "btn_option_p.png");
	pkBtnOption->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 105.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 1.0f - 16.0f * g_f32ScaleHD * ui_main_scale)));
	pkBtnOption->setScale(ui_main_scale);
	CCSize kSize = pkBtnGame->getContentSize();
	kSize.height *= 2.0f;
	pkBtnGame->setContentSize(kSize);
	pkBtnAction->setContentSize(kSize);
	pkBtnOption->setContentSize(kSize);
	pkMainView->AddButton(pkBtnGame);
	pkMainView->AddButton(pkBtnAction);
	pkMainView->AddButton(pkBtnOption);
	UIPage* pkPageGame = VE_NEW UIPage;
	UIPage* pkPageAction = VE_NEW UIPage;
	UIPage* pkPageOption = VE_NEW UIPage;
	pkMainView->AddPage(pkPageGame);
	pkMainView->AddPage(pkPageAction);
	pkMainView->AddPage(pkPageOption);
	UILinkDelegate(pkMainView, OnPageChange);
	AddWidget("main_view", pkMainView);
	m_pkLight = CCSprite::createWithSpriteFrameName("tab_light.png");
	m_pkLight->setAnchorPoint(ccp(0.5f, 1.0f));
	m_pkLight->setPosition(ui_ccp(0.5f, 1.0f));
	m_pkLight->setScale(ui_main_scale);
	pkMainView->addChild(m_pkLight, 5);
	m_akLightPos[0] = ccp(VeFloorf(UILayer::Width() * 0.5f - 105.0f * g_f32ScaleHD * ui_main_scale), UILayer::Height());
	m_akLightPos[1] = ccp(VeFloorf(UILayer::Width() * 0.5f), UILayer::Height());
	m_akLightPos[2] = ccp(VeFloorf(UILayer::Width() * 0.5f + 105.0f * g_f32ScaleHD * ui_main_scale), UILayer::Height());

	{
		CCSpriteBatchNode* pkBatchNodeGame = CCSpriteBatchNode::create(g_pkGame->GetImagePath("text_0", true) + ".png");
		pkPageGame->addChild(pkBatchNodeGame, 0);

		CCSprite* pkGameTitle = CCSprite::createWithSpriteFrameName("logo_0.png");
		pkGameTitle->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (-60.0f * ui_height_scale + 180.0f * ui_main_scale) * g_f32ScaleHD)));
		pkGameTitle->setScale(ui_main_scale);
		pkBatchNodeGame->addChild(pkGameTitle, 5);

		CCSprite* pkMiniGameTitle = CCSprite::createWithSpriteFrameName("mini_game_title.png");
		pkMiniGameTitle->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (180.0f * ui_height_scale + 20.0f * ui_main_scale) * g_f32ScaleHD)));
		pkMiniGameTitle->setScale(ui_main_scale);
		pkBatchNodeGame->addChild(pkMiniGameTitle, 5);		

		m_pkRoomChoose = VE_NEW RoomChoose;
		UILinkDelegate(m_pkRoomChoose, OnRoomClicked);
		pkPageGame->AddChild(m_pkRoomChoose);

		UIButton* pkMini0 = VE_NEW UIButton("nini_slots_n.png", "nini_slots_p.png");
		pkMini0->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 90.0f * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 84.0f * ui_main_scale) * g_f32ScaleHD)));
		pkMini0->setScale(ui_main_scale * 0.7f);
		pkPageGame->AddChild(pkMini0);
		kSize = pkMini0->getContentSize();
		kSize.width *= 1.5f;
		kSize.height *= 1.5f;
		pkMini0->setContentSize(kSize);
		UILinkDelegate(pkMini0, OnMini0);

		UIButton* pkMini1 = VE_NEW UIButton("nini_lotto_n.png", "nini_lotto_p.png");
		pkMini1->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 84.0f * ui_main_scale) * g_f32ScaleHD)));
		pkMini1->setScale(ui_main_scale * 0.7f);
		pkPageGame->AddChild(pkMini1);
		kSize = pkMini1->getContentSize();
		kSize.width *= 1.5f;
		kSize.height *= 1.5f;
		pkMini1->setContentSize(kSize);
		UILinkDelegate(pkMini1, OnMini1);

		UIButton* pkMini2 = VE_NEW UIButton("mini_baccarat_n.png", "mini_baccarat_p.png");
		pkMini2->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 90.0f * g_f32ScaleHD * ui_main_scale),
			VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 84.0f * ui_main_scale) * g_f32ScaleHD)));
		pkMini2->setScale(ui_main_scale * 0.7f);
		pkPageGame->AddChild(pkMini2);
		kSize = pkMini2->getContentSize();
		kSize.width *= 1.5f;
		kSize.height *= 1.5f;
		pkMini2->setContentSize(kSize);
		UILinkDelegate(pkMini2, OnMini2);
	}

	{
		CCSpriteBatchNode* pkBatchNodeAction = CCSpriteBatchNode::create(g_pkGame->GetImagePath("text_0", true) + ".png");
		pkPageAction->addChild(pkBatchNodeAction, 0);

		CCSprite* pkGameTitle = CCSprite::createWithSpriteFrameName("logo_0.png");
		pkGameTitle->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (-60.0f * ui_height_scale + 180.0f * ui_main_scale) * g_f32ScaleHD)));
		pkGameTitle->setScale(ui_main_scale);
		pkBatchNodeAction->addChild(pkGameTitle, 5);

		CCSprite* pkActionTitle = CCSprite::createWithSpriteFrameName("action_title.png");
		pkActionTitle->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (180.0f * ui_height_scale + 20.0f * ui_main_scale) * g_f32ScaleHD)));
		pkActionTitle->setScale(ui_main_scale);
		pkBatchNodeAction->addChild(pkActionTitle, 5);

		CCSprite* pkRichList = CCSprite::createWithSpriteFrameName("rich_list.png");
		pkRichList->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 80.0f * ui_main_scale) * g_f32ScaleHD)));
		pkRichList->setScale(ui_main_scale);
		pkBatchNodeAction->addChild(pkRichList);

		m_pkEventList = VE_NEW EventList;
		pkPageAction->AddChild(m_pkEventList);
		UILinkDelegate(m_pkEventList, OnEventAction);

		m_pkRankList = VE_NEW RankList;
		pkPageAction->AddChild(m_pkRankList);
	}

	{
		CCSprite* pkAdvertising = CCSprite::createWithSpriteFrameName("Advertising_0_a.png");
		pkAdvertising->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 84.0f * ui_main_scale) * g_f32ScaleHD)));
		pkAdvertising->setScale(ui_main_scale);
		pkPageOption->addChild(pkAdvertising, 0);
		CCAnimation* pkAnimation = CCAnimation::create();
		pkAnimation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("Advertising_0_a.png"));
		pkAnimation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("Advertising_0_b.png"));
		pkAnimation->setDelayPerUnit(1.0f / 3.0f);
		pkAnimation->setRestoreOriginalFrame(true);
		pkAdvertising->runAction(CCRepeatForever::create(CCAnimate::create(pkAnimation)));

		CCSpriteBatchNode* pkBatchNodeOption = CCSpriteBatchNode::create(g_pkGame->GetImagePath("text_0", true) + ".png");
		pkPageOption->addChild(pkBatchNodeOption, 0);

		CCSprite* pkGameTitle = CCSprite::createWithSpriteFrameName("logo_0.png");
		pkGameTitle->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (-60.0f * ui_height_scale + 180.0f * ui_main_scale) * g_f32ScaleHD)));
		pkGameTitle->setScale(ui_main_scale);
		pkBatchNodeOption->addChild(pkGameTitle, 5);

		CCSprite* pkOptionTitle = CCSprite::createWithSpriteFrameName("option_title.png");
		pkOptionTitle->setPosition(ccp(ui_w(0.5f),
			VeFloorf(UILayer::Height() * 0.5f + (180.0f * ui_height_scale + 20.0f * ui_main_scale) * g_f32ScaleHD)));
		pkOptionTitle->setScale(ui_main_scale);
		pkBatchNodeOption->addChild(pkOptionTitle, 5);

		m_pkOptionView = VE_NEW OptionView;
		pkPageOption->AddChild(m_pkOptionView);
	}

	UIButton::SetPressedSoundGlobal(SE_NULL);
	pkMainView->Change(0);
	UIButton::SetPressedSoundGlobal(SE_BTN_CLICK);
	m_pkRoomChoose->ScrollToTop();
	m_pkEventList->ScrollToTop();
	return UILayer::init();
}

void Hall::term()
{
	UILayer::term();
}

void Hall::OnActive(const UIUserDataPtr& spData)
{
	g_pSoundSystem->PlayMusic(HALL_MUSIC);
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	UpdateProperties();
	pkPlayer->S_ReqInitHall();
	g_pkGame->ShowLoading();
	schedule(schedule_selector(Hall::Tick), 3.0f);
	OnLeaderboardUpdate(g_pkGame->GetLeaderboard());
	schedule(schedule_selector(Hall::UpdateLeaderboard), 60.0f);
	NativeOnEventBegin("hall");
	g_pkGame->RestartNoticeAlways();
}

void Hall::OnDeactive(const UIUserDataPtr& spData)
{
	g_pSoundSystem->StopMusic();
	unscheduleAllSelectors();
	NativeOnEventEnd("hall");
}

bool Hall::OnBack()
{
	return m_pkOptionView->OnBack();
}

void Hall::OnMenu()
{
	if(!(g_pLayerManager->GetLayersNum()))
	{
		g_pLayerManager->PushLayer("edit_info");
	}
}

void Hall::Tick(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	pkPlayer->S_ReqTickHall();
}

void Hall::UpdateRoomList(const VeVector<RoomInfo>& tRoomList)
{
	m_pkRoomChoose->ClearAll();
	for(VeUInt32 i(1); i < tRoomList.Size(); ++i)
	{
		const RoomInfo& kInfo = tRoomList[i];
		m_pkRoomChoose->AddRoomBar(kInfo.m_u32ChipsMax, kInfo.m_u32SB,
			kInfo.m_u32BB, (RoomBar::Type)kInfo.m_tType,
			VE_MASK_HAS_ANY(kInfo.m_u8Flags, ROOM_FLAG_VIP),
			VE_MASK_HAS_ANY(kInfo.m_u8Flags, ROOM_FLAG_HOT));
	}
	m_pkRoomChoose->ScrollToTop();
	m_kRoomList = tRoomList;
	UpdateRoomEnable();
}

void Hall::UpdateRoomEnable()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;

	for(VeUInt32 i(1); i < m_kRoomList.Size(); ++i)
	{
		const RoomInfo& kInfo = m_kRoomList[i];
		bool bHot = VE_MASK_HAS_ALL(kInfo.m_u8Flags, ROOM_FLAG_HOT);
		bool bEnable = true;
		if(VE_MASK_HAS_ALL(kInfo.m_u8Flags, ROOM_FLAG_VIP))
		{
			if(!pkPlayer->GetVip())
			{
				bEnable = false;
			}
		}
		if(kInfo.m_u32ChipsMin > pkPlayer->GetChips())
		{
			bEnable = false;
		}
		m_pkRoomChoose->SetEnable(i - 1, bEnable, bHot);
	}
}

void Hall::UpdateProperties()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	WIDGET(NameLabel, name);
	WIDGET(ChipsBar, chips_bar);
	WIDGET(GoldBar, gold_bar);
	WIDGET(HeadArea, head_area);
	name.SetName(pkPlayer->GetNickname());
	chips_bar.SetNumber(pkPlayer->GetChips());
	gold_bar.SetNumber(pkPlayer->GetGoldBricks());
	head_area.Set(pkPlayer->GetHeadIcon(), pkPlayer->GetSex() == 0, pkPlayer->GetVip() > 0);
	m_pkOptionView->UpdateProperties(pkPlayer);
	UpdateRoomEnable();
	m_pkEventList->UpdateEvents();
}

void Hall::UpdateOnline(VeUInt32 u32Number)
{
	WIDGET(AllOnline, all_online);
	all_online.SetNumber(u32Number);
}

void Hall::OnFadeOut()
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	if(!g_pSaveData->ReadParam("show_course", VeUInt32(0)))
	{
		g_pSaveData->WriteParam("show_course", VeUInt32(1));
		g_pSaveData->Save();
		g_pLayerManager->SetMainLayer("course");
	}
	else
	{
		g_pLayerManager->SetMainLayer("game");
	}
	g_pkGame->FadeIn(FADE_IN_TIME);
}

void Hall::UpdateForLoading(VeFloat32 f32Delta)
{
	if((!g_pkGame->GetNumTexInLoading()) && (!g_pkGame->IsFading()))
	{
		g_pLayerManager->PopAllLayers();
		g_pLayerManager->PopAllMiddleLayers();
		unschedule(schedule_selector(Hall::UpdateForLoading));
		g_pkGame->FadeOut(FADE_OUT_TIME, &m_kOnFadeOut);
		if(!m_bIsGameCreate)
		{
			g_pLayerManager->AddLayer(ChipsCarry::create());
			g_pLayerManager->AddLayer(Canteen::create());
			g_pLayerManager->AddLayer(ChatWindow::create());
			g_pLayerManager->AddLayer(GameMenu::create());
			g_pLayerManager->AddLayer(GameHelp::create());
			g_pLayerManager->AddLayer(Course::create());
			g_pLayerManager->AddLayer(Game::create());
			m_bIsGameCreate = true;
		}
	}
}

void Hall::OnFadeOutMini()
{
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	switch(m_u32MiniGameNeeded)
	{
	case 0:
		g_pLayerManager->SetMainLayer("slot");
		break;
	case 1:
		g_pLayerManager->SetMainLayer("keno");
		break;
	case 2:
		g_pLayerManager->SetMainLayer("baccarat");
		break;
	default:
		break;
	}
	m_u32MiniGameNeeded = VE_INFINITE;
	g_pkGame->FadeIn(FADE_IN_TIME);
}

void Hall::UpdateForLoadingMini(VeFloat32 f32Delta)
{
	if((!g_pkGame->GetNumTexInLoading()) && (!g_pkGame->IsFading()))
	{
		g_pLayerManager->PopAllLayers();
		g_pLayerManager->PopAllMiddleLayers();
		unschedule(schedule_selector(Hall::UpdateForLoadingMini));
		TurnMiniGame();
	}
}

void Hall::TurnGame()
{
	schedule(schedule_selector(Hall::UpdateForLoading), 0.0f);
}

void Hall::TurnMiniGame()
{
	if(!m_bIsMiniGameCreate)
	{
		g_pLayerManager->AddLayer(Slot::create());
		g_pLayerManager->AddLayer(Keno::create());
		g_pLayerManager->AddLayer(Baccarat::create());
		g_pLayerManager->AddLayer(SlotHelp::create());
		m_bIsMiniGameCreate = true;
	}
	g_pkGame->FadeOut(FADE_OUT_TIME, &m_kOnFadeOutMini);
}

UIImplementDelegate(Hall, OnPageChange, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		UITableView& kMainView = (UITableView&)kWidget;
		VeUInt32 u32Index = kMainView.GetCurrent();
		if(u32Index < 3)
		{
			m_pkLight->setPosition(m_akLightPos[u32Index]);
		}
		m_pkOptionView->Back();
	}
}

UIImplementDelegate(Hall, OnRoomClicked, kWidget, u32State)
{
	if(u32State & 0x80000000)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		VeUInt32 u32Index = u32State & 0x7FFFFFFF;
		if(u32Index)
		{
			pkPlayer->S_ReqEnterPokerTable(u32Index);
		}
		else
		{
			VeUInt32 u32Rate = (pkPlayer->GetVip()) ? 1 : 5;
			RoomInfo* pkInfo = NULL;
			for(VeUInt32 i(1); i < m_kRoomList.Size(); ++i)
			{
				RoomInfo& kInfo = m_kRoomList[i];
				if(VE_MASK_HAS_ANY(kInfo.m_u8Flags, ROOM_FLAG_VIP) != ((pkPlayer->GetVip()) ? true : false))
					continue;
				if((kInfo.m_u32ChipsMax * u32Rate) > pkPlayer->GetChips())
					continue;
				if(pkInfo && (pkInfo->m_u32ChipsMax >= kInfo.m_u32ChipsMax))
					continue;
				pkInfo = &kInfo;
				u32Index = i;
			}
			if(!u32Index)
			{
				for(VeUInt32 i(1); i < m_kRoomList.Size(); ++i)
				{
					RoomInfo& kInfo = m_kRoomList[i];
					if(VE_MASK_HAS_ANY(kInfo.m_u8Flags, ROOM_FLAG_VIP) != ((pkPlayer->GetVip()) ? true : false))
						continue;
					if(kInfo.m_u32ChipsMax > pkPlayer->GetChips())
						continue;
					if(pkInfo && (pkInfo->m_u32ChipsMax >= kInfo.m_u32ChipsMax))
						continue;
					pkInfo = &kInfo;
					u32Index = i;
				}
			}
			if(!u32Index)
			{
				for(VeUInt32 i(1); i < m_kRoomList.Size(); ++i)
				{
					RoomInfo& kInfo = m_kRoomList[i];
					if(VE_MASK_HAS_ANY(kInfo.m_u8Flags, ROOM_FLAG_VIP) != ((pkPlayer->GetVip()) ? true : false))
						continue;
					if(kInfo.m_u32ChipsMin > pkPlayer->GetChips())
						continue;
					if(pkInfo && (pkInfo->m_u32ChipsMax >= kInfo.m_u32ChipsMax))
						continue;
					pkInfo = &kInfo;
					u32Index = i;
				}
			}
			pkPlayer->S_ReqEnterPokerTable(u32Index);
		}
		g_pkGame->ShowLoading();
	}
}

UIImplementDelegate(Hall, OnGetChipsClicked, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("shop");
	}
}

UIImplementDelegate(Hall, OnBankClicked, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("bank");
	}
}

UIImplementDelegate(Hall, OnMini0, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED && m_u32MiniGameNeeded == VE_INFINITE)
	{
		TurnLoadingMiniGame(0);
	}
}

UIImplementDelegate(Hall, OnMini1, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED && m_u32MiniGameNeeded == VE_INFINITE)
	{
		TurnLoadingMiniGame(1);
	}
}

UIImplementDelegate(Hall, OnMini2, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED && m_u32MiniGameNeeded == VE_INFINITE)
	{
		TurnLoadingMiniGame(2);
	}
}

UIImplementDelegate(Hall, OnEventAction, kWidget, u32State)
{
	switch(u32State)
	{
	case 1000:
		OnRoomClicked(kWidget, 0x80000000);
		break;
	case 1001:
		TurnLoadingMiniGame(0);
		break;
	case 1002:
		TurnLoadingMiniGame(1);
		break;
	case 1003:
		TurnLoadingMiniGame(2);
		break;
	default:
		break;
	}
}

void Hall::TurnLoadingMiniGame(VeUInt32 u32Index)
{
	m_u32MiniGameNeeded = u32Index;
	schedule(schedule_selector(Hall::UpdateForLoadingMini), 0.0f);
}

void Hall::OnForceUpdate()
{
	UpdateProperties();
	g_pkGame->ShowKenoResult();
}

void Hall::OnLeaderboardUpdate(const VeVector< VePair<VeStringA,VeStringA> >& kLeaderboard)
{
	m_pkRankList->GetRankData(kLeaderboard);
}

void Hall::UpdateLeaderboard(VeFloat32 f32Delta)
{
	g_pkGame->RefreshServerNotice();
}

void Hall::SetEventNumber(VeUInt32 u32Number)
{
	if(u32Number)
	{
		m_pkNotify->setVisible(true);
		m_pkEventNumber->setVisible(true);
		m_pkEventNumber->setString(NTL(u32Number, 4));
		CCSize kSize = m_pkEventNumber->getContentSize();
		m_pkEventNumber->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	else
	{
		m_pkNotify->setVisible(false);
		m_pkEventNumber->setVisible(false);
	}
}

void Hall::OnPushNotice(const VeChar8* pcNotice)
{
	WIDGET(NoticeBar, notice_bar);
	notice_bar.Notice(pcNotice, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale));
}
