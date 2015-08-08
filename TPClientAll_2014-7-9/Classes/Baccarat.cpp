#include "Slot.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "Knickknacks.h"
#include "Hall.h"
#include "SoundSystem.h"
#include "Baccarat.h"
#include "Keno.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"
#include "NativeInterface.h"

USING_NS_CC;

const VeUInt32 s_au32ChipsMap[6] =
{
	10, 50, 100, 500, 1000, 5000
};

Baccarat::Baccarat() : UILayerExt<Baccarat>("baccarat")
{
	UIInitDelegate(Baccarat, OnPurchase);
	UIInitDelegate(Baccarat, RepeatBet);
	UIInitDelegate(Baccarat, DoubleBet);
	UIInitDelegate(Baccarat, OnBack);
	UIInitDelegate(Baccarat, OnAreaEvent);
	m_u32CountDownTime = 0;
	m_bPlayRes = false;
}

Baccarat::~Baccarat()
{
	term();
}

bool Baccarat::init()
{
	CCSize kSize;

	CCSpriteBatchNode* pkKenoRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("keno", false) + ".png");
	addChild(pkKenoRoot);

	CCSpriteBatchNode* pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("slot", false) + ".png");
	addChild(pkRoot);

	CCSpriteBatchNode* pkUi0Root = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	addChild(pkUi0Root);

	CCSprite* pkCurtainL = CCSprite::createWithSpriteFrameName("curtain.png");
	pkCurtainL->setAnchorPoint(ccp(1.0f, 1.0f));
	pkCurtainL->setScale(ui_main_scale);
	pkCurtainL->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 284.0f * g_f32ScaleHD * ui_main_scale)));
	pkRoot->addChild(pkCurtainL);

	CCSprite* pkCurtainR = CCSprite::createWithSpriteFrameName("curtain.png");
	pkCurtainR->setAnchorPoint(ccp(1.0f, 1.0f));
	pkCurtainR->setScaleX(-ui_main_scale);
	pkCurtainR->setScaleY(ui_main_scale);
	pkCurtainR->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 284.0f * g_f32ScaleHD * ui_main_scale)));
	pkRoot->addChild(pkCurtainR);

	UIButton* pkBack = VE_NEW UIButton("back_n.png", "back_p.png");
	pkBack->setPosition(ccp(VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() - 46.0f * g_f32ScaleHD * ui_main_scale)));
	pkBack->setScale(ui_main_scale);
	kSize = pkBack->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	pkBack->setContentSize(kSize);
	AddWidget("back", pkBack);
	UILinkDelegate(pkBack, OnBack);

	CCSprite* pkBaccTitleL = CCSprite::createWithSpriteFrameName("bacc_title.png");
	pkBaccTitleL->setAnchorPoint(ccp(1.0f, 1.0f));
	pkBaccTitleL->setScale(ui_main_scale);
	pkBaccTitleL->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - (100 * ui_height_scale - 78 * ui_main_scale) * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBaccTitleL,3);

	CCSprite* pkBaccTitleR = CCSprite::createWithSpriteFrameName("bacc_title.png");
	pkBaccTitleR->setAnchorPoint(ccp(1.0f, 1.0f));
	pkBaccTitleR->setScaleX(-ui_main_scale);
	pkBaccTitleR->setScaleY(ui_main_scale);
	pkBaccTitleR->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - (100 * ui_height_scale - 78 * ui_main_scale) * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBaccTitleR,3);

	CCSprite* pkOnlineText = CCSprite::createWithSpriteFrameName("bacc_online.png");
	pkOnlineText->setAnchorPoint(ccp(1.0f, 1.0f));
	pkOnlineText->setScale(ui_main_scale);
	pkOnlineText->setPosition(ccp(UILayer::Width() * 0.5f - 39.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() - (100 * ui_height_scale - 66 * ui_main_scale) * g_f32ScaleHD)));
	addChild(pkOnlineText,3);

	CCSprite* pkAllBetText = CCSprite::createWithSpriteFrameName("bacc_all_bet.png");
	pkAllBetText->setAnchorPoint(ccp(1.0f, 1.0f));
	pkAllBetText->setScale(ui_main_scale);
	pkAllBetText->setPosition(ccp(UILayer::Width() * 0.5f - 39.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() - (100 * ui_height_scale - 47 * ui_main_scale) * g_f32ScaleHD)));
	addChild(pkAllBetText,3);

	CCSprite* pkBaccShadow = CCSprite::createWithSpriteFrameName("bacc_shadow.png");
	pkBaccShadow->setAnchorPoint(ccp(0.5f, 1.0f));
	pkBaccShadow->setScaleX(UILayer::Width());
	pkBaccShadow->setScaleY(ui_main_scale);
	pkBaccShadow->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - (100 * ui_height_scale - 5 * ui_main_scale) * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBaccShadow,5);
	
	for(VeUInt32 j(0); j < 4; ++j)
	{
		CCSprite *bg = CCSprite::createWithSpriteFrameName("bacc_bg.png");
		bg->setAnchorPoint(ccp(1.0f, 0.0f));
		bg->setPosition(ui_ccp(0.5f, 0.5f));
		bg->setBlendFunc(kCCBlendFuncDisable);
		pkKenoRoot->addChild(bg, 0);

		switch(j)
		{
		case 1:
			bg->setScaleX(-ui_main_scale);
			bg->setScaleY(ui_main_scale);
			break;
		case 2:
			bg->setScaleX(ui_main_scale);
			bg->setScaleY(-ui_main_scale);
			break;
		case 3:
			bg->setScaleX(-ui_main_scale);
			bg->setScaleY(-ui_main_scale);
			break;
		default:
			bg->setScale(ui_main_scale);
			break;
		}
	}

	for(VeUInt32 i(0); i < 5; ++i)
	{
		CCSprite* pkBottom = CCSprite::createWithSpriteFrameName("keno_bottom.png");
		pkBottom->setScale(ui_main_scale);
		pkBottom->setAnchorPoint(ccp(0.5f, 0.0f));
		pkBottom->setPosition(ccp(UILayer::Width() * 0.5f + (72.0f * i - 144) * g_f32ScaleHD * ui_main_scale, 0));
		pkKenoRoot->addChild(pkBottom,2);
	}

	for(VeUInt32 i(0); i < 5; ++i)
	{
		CCSprite* pkBottom = CCSprite::createWithSpriteFrameName("keno_bottom.png");
		pkBottom->setScale(ui_main_scale);
		pkBottom->setAnchorPoint(ccp(0.5f, 0.0f));
		pkBottom->setPosition(ccp(UILayer::Width() * 0.5f + (72.0f * i - 144) * g_f32ScaleHD * ui_main_scale, 56.0f * g_f32ScaleHD * ui_main_scale));
		pkKenoRoot->addChild(pkBottom,1);
	}

	VeFloat32 f32BoardHeight = VeFloorf(UILayer::Height() - (100 * ui_height_scale - 77 * ui_main_scale) * g_f32ScaleHD);

	for(VeUInt32 i(0); i < 5; ++i)
	{
		CCSprite* pkBottom = CCSprite::createWithSpriteFrameName("keno_bottom.png");
		pkBottom->setScale(ui_main_scale);
		pkBottom->setAnchorPoint(ccp(0.5f, 0.0f));
		pkBottom->setPosition(ccp(UILayer::Width() * 0.5f + (72.0f * i - 144) * g_f32ScaleHD * ui_main_scale, f32BoardHeight));
		pkKenoRoot->addChild(pkBottom,1);
	}

	for(VeUInt32 i(0); i < 5; ++i)
	{
		CCSprite* pkBottom = CCSprite::createWithSpriteFrameName("keno_bottom.png");
		pkBottom->setScaleX(ui_main_scale);
		pkBottom->setScaleY(-ui_main_scale);
		pkBottom->setAnchorPoint(ccp(0.5f, 0.0f));
		pkBottom->setPosition(ccp(UILayer::Width() * 0.5f + (72.0f * i - 144) * g_f32ScaleHD * ui_main_scale, f32BoardHeight));
		pkKenoRoot->addChild(pkBottom,2);
	}

	NoticeBar* pkNoticeBar = VE_NEW NoticeBar;
	pkNoticeBar->setPositionY(VeFloorf(UILayer::Height() * 0.5f + 230.0f * g_f32ScaleHD * ui_height_scale));
	AddWidget("notice_bar", pkNoticeBar, 0);

	UIButton* pkPurchase = VE_NEW UIButton("btn_shop_n.png", "btn_shop_p.png");
	pkPurchase->setPosition(ccp(UILayer::Width() * 0.5f + 124.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() - (100 * ui_height_scale - 51 * ui_main_scale) * g_f32ScaleHD)));
	pkPurchase->setScale(ui_main_scale);
	CCSize PurchaseSize = pkPurchase->getContentSize();
	PurchaseSize.width *= 1.4f;
	PurchaseSize.height *= 1.2f;
	pkPurchase->setContentSize(PurchaseSize);
	CCSprite* pkPurchaseText = CCSprite::createWithSpriteFrameName("mini_buy_chips.png");
	pkPurchaseText->setPosition(ccp(8 * g_f32ScaleHD, 6.0f * g_f32ScaleHD));
	pkPurchase->addChild(pkPurchaseText, 5);
	AddWidget("purchase", pkPurchase);
	UILinkDelegate(pkPurchase, OnPurchase);

	CCSprite* pkBaccTime = CCSprite::createWithSpriteFrameName("bacc_time.png");
	pkBaccTime->setPosition(ccp(ui_w(0.5f),VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 96 * ui_main_scale) * g_f32ScaleHD)));
	pkBaccTime->setScaleX(ui_main_scale);
	pkBaccTime->setScaleY(ui_main_scale);
	addChild(pkBaccTime, 0);

	m_kCountDownTimes = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	m_kCountDownTimes->setPosition(ccp(UILayer::Width() * 0.5f + 5.0f * g_f32ScaleHD * ui_main_scale,VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 88 * ui_main_scale) * g_f32ScaleHD)));
	m_kCountDownTimes->setScale(ui_main_scale * 0.7f);
	m_kCountDownTimes->setAnchorPoint(ccp(0.0f, 0.5f));
	AddWidget("count_down_time", m_kCountDownTimes);

	CCSprite* pkBaccColon = CCSprite::createWithSpriteFrameName("bacc_colon.png");
	pkBaccColon->setPosition(ccp(ui_w(0.5f),VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 58 * ui_main_scale) * g_f32ScaleHD)));
	pkBaccColon->setScaleX(ui_main_scale);
	pkBaccColon->setScaleY(ui_main_scale);
	pkKenoRoot->addChild(pkBaccColon, 3);

	for(VeUInt32 i(0); i < 6; ++i)
	{
		CCSprite* pkBottom = CCSprite::createWithSpriteFrameName("bacc_cell.png");
		pkBottom->setScaleX(ui_main_scale);
		pkBottom->setScaleY(61.0f * ui_main_scale * g_f32ScaleHD);
		pkBottom->setAnchorPoint(ccp(0.5f, 0.0f));
		pkBottom->setPosition(ccp(UILayer::Width() * 0.5f + (52.0f * i - 130) * g_f32ScaleHD * ui_main_scale, 56.0f * g_f32ScaleHD * ui_main_scale));
		pkKenoRoot->addChild(pkBottom,1);
	}

	UIBarH* pkAlert = UIBarH::createWithEl("notice",228.0f * ui_main_scale * g_f32ScaleHD);
	pkAlert->setAnchorPoint(ccp(0.0f, 1.0f));
	pkAlert->setScaleY(ui_main_scale);
	addChild(pkAlert);
	pkAlert->setPosition(ccp(ui_w(0.5f), 
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 74 * ui_main_scale) * g_f32ScaleHD)));

	CCLabelTTF* m_pkPlayerNumber = CCLabelTTF::create(TT("BaccPlayer"), TTF_NAME, VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	m_pkPlayerNumber->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 110.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 64 * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkPlayerNumber,3);
	kSize = m_pkPlayerNumber->getContentSize();
	m_pkPlayerNumber->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));

	CCLabelTTF* m_pkTieNumber = CCLabelTTF::create(TT("BaccTie"), TTF_NAME, VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	m_pkTieNumber->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 35.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 64 * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkTieNumber,3);
	kSize = m_pkTieNumber->getContentSize();
	m_pkTieNumber->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));

	CCLabelTTF* m_pkBankerNumber = CCLabelTTF::create(TT("BaccBanker"), TTF_NAME, VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	m_pkBankerNumber->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 30.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 64 * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkBankerNumber,3);
	kSize = m_pkBankerNumber->getContentSize();
	m_pkBankerNumber->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkBetChipsPlayer = CCLabelTTF::create("", TTF_NAME, VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	m_pkBetChipsPlayer->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 40.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 64 * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkBetChipsPlayer);
	kSize = m_pkBetChipsPlayer->getContentSize();
	m_pkBetChipsPlayer->setAnchorPoint(ccp(VeFloorf(kSize.width * 1.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkBetChipsTie = CCLabelTTF::create("", TTF_NAME, VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	m_pkBetChipsTie->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 64 * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkBetChipsTie);
	kSize = m_pkBetChipsTie->getContentSize();
	m_pkBetChipsTie->setAnchorPoint(ccp(VeFloorf(kSize.width * 1.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkBetChipsBanker = CCLabelTTF::create("", TTF_NAME, VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	m_pkBetChipsBanker->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 104.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 64 * ui_main_scale) * g_f32ScaleHD)));
	addChild(m_pkBetChipsBanker);
	kSize = m_pkBetChipsBanker->getContentSize();
	m_pkBetChipsBanker->setAnchorPoint(ccp(VeFloorf(kSize.width * 1.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkRepeatBet = VE_NEW UIButton("keno_btn_n_y.png", "keno_btn_p_y.png", "keno_btn_d.png");
	m_pkRepeatBet->setScale(ui_main_scale);
	m_pkRepeatBet->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 136 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(28 * ui_main_scale * g_f32ScaleHD)));
	CCSize RepeatBetSize = m_pkRepeatBet->getContentSize();
	RepeatBetSize.width *= 1.6f;
	RepeatBetSize.height *= 1.6f;
	m_pkRepeatBet->setContentSize(RepeatBetSize);
	AddWidget("repeat_bet", m_pkRepeatBet);
	CCSprite* pkRepeatBetText = CCSprite::createWithSpriteFrameName("bacc_repeat.png");
	pkRepeatBetText->setPositionY(1.0f * g_f32ScaleHD);
	m_pkRepeatBet->addChild(pkRepeatBetText, 5);
	UILinkDelegate(m_pkRepeatBet, RepeatBet);

	m_pkDoubleBet = VE_NEW UIButton("keno_btn_n_y.png", "keno_btn_p_y.png", "keno_btn_d.png");
	m_pkDoubleBet->setScale(ui_main_scale);
	m_pkDoubleBet->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 136 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(28 * ui_main_scale * g_f32ScaleHD)));
	CCSize DoubleBetSize = m_pkDoubleBet->getContentSize();
	DoubleBetSize.width *= 1.6f;
	DoubleBetSize.height *= 1.6f;
	m_pkDoubleBet->setContentSize(DoubleBetSize);
	AddWidget("double_bet", m_pkDoubleBet);
	CCSprite* pkDoubleBetText = CCSprite::createWithSpriteFrameName("bacc_double.png");
	pkDoubleBetText->setPositionY(1.0f * g_f32ScaleHD);
	m_pkDoubleBet->addChild(pkDoubleBetText, 5);
	UILinkDelegate(m_pkDoubleBet, DoubleBet);
	m_pkDoubleBet->SetEnable(false);

	CCSprite* pkWinFrame = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkWinFrame->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 40.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD)));
	pkWinFrame->setScale(ui_main_scale * 0.9f);
	addChild(pkWinFrame);

	CCSprite* pkChipsFrame = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkChipsFrame->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 64.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD)));
	pkChipsFrame->setScale(ui_main_scale * 0.9f);
	addChild(pkChipsFrame);

	CCSprite* pkYouWin = CCSprite::createWithSpriteFrameName("slot_win_icon.png");
	pkYouWin->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 88.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD)));
	pkYouWin->setScale(ui_main_scale * 0.9f);
	addChild(pkYouWin);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chips.png");
	pkChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 24.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD)));
	pkChips->setScale(ui_main_scale * 0.9f);
	addChild(pkChips);

	m_pkWinChips = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkWinChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 8.0f * ui_main_scale * g_f32ScaleHD), 
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD)));
	addChild(m_pkWinChips,2);

	m_pkCredit = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkCredit->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 96.0f * ui_main_scale * g_f32ScaleHD), 
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD)));
	addChild(m_pkCredit,2);

	m_kBetPlayerNum = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	m_kBetPlayerNum->setPosition(ccp(UILayer::Width() * 0.5f + 95.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() - (100 * ui_height_scale - 57 * ui_main_scale) * g_f32ScaleHD)));
	m_kBetPlayerNum->setScale(ui_main_scale * 0.75f);
	AddWidget("online_people_number", m_kBetPlayerNum);

	m_kAllBetChips = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	m_kAllBetChips->setPosition(ccp(UILayer::Width() * 0.5f + 95.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() - (100 * ui_height_scale - 39 * ui_main_scale) * g_f32ScaleHD)));
	m_kAllBetChips->setScale(ui_main_scale * 0.75f);
	AddWidget("online_people_number", m_kAllBetChips);

	CCPoint kDealPos(VeFloorf(UILayer::Width() * 0.5f),
		VeFloorf(UILayer::Height() * 1.0f - (40.0f * ui_main_scale + (ui_height_scale - ui_main_scale) * 150.0f) * g_f32ScaleHD));

	m_apkPlayerCard[0] = VE_NEW MiniCard;
	m_apkPlayerCard[0]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 106.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 56 * ui_main_scale) * g_f32ScaleHD)));
	AddWidget("player_card_0", m_apkPlayerCard[0]);
	m_apkPlayerCard[0]->SetDealPos(kDealPos);

	m_apkPlayerCard[1] = VE_NEW MiniCard;
	m_apkPlayerCard[1]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 80.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 56 * ui_main_scale) * g_f32ScaleHD)));
	AddWidget("player_card_1", m_apkPlayerCard[1]);
	m_apkPlayerCard[1]->SetDealPos(kDealPos);

	m_apkPlayerCard[2] = VE_NEW MiniCard;
	m_apkPlayerCard[2]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 54.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 56 * ui_main_scale) * g_f32ScaleHD)));
	AddWidget("player_card_2", m_apkPlayerCard[2]);
	m_apkPlayerCard[2]->SetDealPos(kDealPos);

	m_apkBankerCard[0] = VE_NEW MiniCard;
	m_apkBankerCard[0]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 54.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 56 * ui_main_scale) * g_f32ScaleHD)));
	AddWidget("banker_card_0", m_apkBankerCard[0]);
	m_apkBankerCard[0]->SetDealPos(kDealPos);

	m_apkBankerCard[1] = VE_NEW MiniCard;
	m_apkBankerCard[1]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 80.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 56 * ui_main_scale) * g_f32ScaleHD)));
	AddWidget("banker_card_1", m_apkBankerCard[1]);
	m_apkBankerCard[1]->SetDealPos(kDealPos);

	m_apkBankerCard[2] = VE_NEW MiniCard;
	m_apkBankerCard[2]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 106.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 56 * ui_main_scale) * g_f32ScaleHD)));
	AddWidget("banker_card_2", m_apkBankerCard[2]);
	m_apkBankerCard[2]->SetDealPos(kDealPos);

	m_pkDrawGameLeft = CCSprite::createWithSpriteFrameName("baccarat_tag_draw.png");
	m_pkDrawGameLeft->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 80.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 75 * ui_main_scale) * g_f32ScaleHD)));
	m_pkDrawGameLeft->setScale(ui_main_scale);
	m_pkDrawGameLeft->setVisible(false);
	addChild(m_pkDrawGameLeft, 3);

	m_pkDrawGameRight = CCSprite::createWithSpriteFrameName("baccarat_tag_draw.png");
	m_pkDrawGameRight->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 80.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 75 * ui_main_scale) * g_f32ScaleHD)));
	m_pkDrawGameRight->setScale(ui_main_scale);
	m_pkDrawGameRight->setVisible(false);
	addChild(m_pkDrawGameRight, 3);

	m_pkWin = CCSprite::createWithSpriteFrameName("baccarat_tag_win.png");
	m_pkWin->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 80.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 75 * ui_main_scale) * g_f32ScaleHD)));
	m_pkWin->setScale(ui_main_scale);
	m_pkWin->setVisible(false);
	addChild(m_pkWin, 3);

	m_pkLos = CCSprite::createWithSpriteFrameName("baccarat_tag_lose.png");
	m_pkLos->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 80.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 75 * ui_main_scale) * g_f32ScaleHD)));
	m_pkLos->setScale(ui_main_scale);
	m_pkLos->setVisible(false);
	addChild(m_pkLos, 3);

	m_pkPlayerScore = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	m_pkPlayerScore->setPosition(ccp(UILayer::Width() * 0.5f - 23.0f * g_f32ScaleHD * ui_main_scale,
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 58 * ui_main_scale) * g_f32ScaleHD)));
	m_pkPlayerScore->setScale(ui_main_scale);
	AddWidget("player_score", m_pkPlayerScore);

	m_pkBankerScore = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	m_pkBankerScore->setPosition(ccp(UILayer::Width() * 0.5f + 23.0f * g_f32ScaleHD * ui_main_scale,
		VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 58 * ui_main_scale) * g_f32ScaleHD)));
	m_pkBankerScore->setScale(ui_main_scale);
	AddWidget("banker_score", m_pkBankerScore);

	BetArea* pkBetArea = VE_NEW BetArea;
	AddWidget("bet_area", pkBetArea, 3);
	UILinkDelegate(pkBetArea, OnAreaEvent);

	m_u32TieNum = 0;
	m_u32PlayerNum = 0;
	m_u32BankerNum = 0;
	m_u32TieNumNow = 0;
	m_u32PlayerNumNow = 0;
	m_u32BankerNumNow = 0;

	return UILayer::init();
}

void Baccarat::term()
{
	UILayerExt<Baccarat>::term();
}

void Baccarat::OnActive(const UIUserDataPtr& spData)
{
	VeSrand(g_pTime->GetCurrentCount());
	UpdateBaccResult(false);
	schedule(schedule_selector(Baccarat::UpdateServer), 3.0f);
	schedule(schedule_selector(Baccarat::Update), 0.0f);
	g_pSoundSystem->PlayMusic(BACC_MUSIC);
	NativeOnEventBegin("baccarat");
}

void Baccarat::OnDeactive(const UIUserDataPtr& spData)
{
	unscheduleAllSelectors();
	NativeOnEventEnd("baccarat");
}

bool Baccarat::OnBack()
{
	if(m_bPlayRes)
	{
		g_pkGame->Toast(TT("Bacc_roll_back"));
		return false;
	}
	return true;
}

void Baccarat::SetChips(VeUInt64 u64Win, VeUInt64 u64Credit)
{
	CCSize kSize;
	m_pkWinChips->setString(NTL(u64Win, 8));
	kSize = m_pkWinChips->getContentSize();
	m_pkWinChips->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkCredit->setString(NTL(u64Credit, 8));
	kSize = m_pkCredit->getContentSize();
	m_pkCredit->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Baccarat::SetWin(VeUInt64 u64Win)
{
	CCSize kSize;
	m_pkWinChips->setString(NTL(u64Win, 8));
	kSize = m_pkWinChips->getContentSize();
	m_pkWinChips->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Baccarat::SetCountDownTime(VeUInt32 u32Second)
{
	if(u32Second != m_u32CountDownTime && u32Second == 2)
	{
		g_pSoundSystem->PlayEffect(SE_POKER_ON_MYTURN);
	}
	m_kCountDownTimes->SetNumber(u32Second);
	if(u32Second < 10)
	{
		m_kCountDownTimes->setPosition(ccp(UILayer::Width() * 0.5f - 0.0f * g_f32ScaleHD * ui_main_scale,VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 88 * ui_main_scale) * g_f32ScaleHD)));
	}
	else
	{
		m_kCountDownTimes->setPosition(ccp(UILayer::Width() * 0.5f + 5.0f * g_f32ScaleHD * ui_main_scale,VeFloorf(UILayer::Height() * 0.5f + (22 * ui_height_scale + 88 * ui_main_scale) * g_f32ScaleHD)));
	}
	m_u32CountDownTime = u32Second;
}

void Baccarat::SetBetChips(VeUInt32 u32Player, VeUInt32 u32Banker, VeUInt32 u32Tie)
{
	CCSize kSize;
	m_pkBetChipsPlayer->setString(NTL(u32Player, 5));
	kSize = m_pkBetChipsPlayer->getContentSize();
	m_pkBetChipsPlayer->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkBetChipsBanker->setString(NTL(u32Banker, 5));
	kSize = m_pkBetChipsBanker->getContentSize();
	m_pkBetChipsBanker->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkBetChipsTie->setString(NTL(u32Tie, 5));
	kSize = m_pkBetChipsTie->getContentSize();
	m_pkBetChipsTie->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_u32TieNumNow = u32Tie;
	m_u32PlayerNumNow = u32Player;
	m_u32BankerNumNow = u32Banker;
}

void Baccarat::SetEnable(bool bEnable)
{
	m_pkRepeatBet->SetEnable(bEnable);
}

UIImplementDelegate(Baccarat, OnPurchase, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("shop");
	}
}

UIImplementDelegate(Baccarat, RepeatBet, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqBaccaratBetAll(m_u32BankerNum, m_u32PlayerNum, m_u32TieNum);
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_banker", VeStringA::From(VeUInt32(m_u32BankerNum))));
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_player", VeStringA::From(VeUInt32(m_u32PlayerNum))));
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_tie", VeStringA::From(VeUInt32(m_u32TieNum))));
		NativeOnEvent("ReqBaccaratBetAll", kEventValue);
	}
}

UIImplementDelegate(Baccarat, DoubleBet, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqBaccaratBetAll(m_u32BankerNumNow, m_u32PlayerNumNow, m_u32TieNumNow);
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_banker", VeStringA::From(VeUInt32(m_u32BankerNum))));
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_player", VeStringA::From(VeUInt32(m_u32PlayerNum))));
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_tie", VeStringA::From(VeUInt32(m_u32TieNum))));
		NativeOnEvent("ReqBaccaratBetAll", kEventValue);
	}
}

UIImplementDelegate(Baccarat, OnBack, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Baccarat, OnAreaEvent, kWidget, u32State)
{
	if(VE_MASK_HAS_ANY(u32State, BACC_EVENT_FLAG))
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		BaccaratBetType eType = BaccaratBetType((u32State & BACC_EVENT_TYPE_MASK) >> 8);
		VE_ASSERT(eType < BaccaratBetType_MAX);
		VeUInt32 u32Chips = s_au32ChipsMap[u32State & BACC_EVENT_CHIPS_MASK];
		pkPlayer->S_ReqBaccaratBet(eType, u32Chips);

		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		switch(eType)
		{
		case BACC_BANKER:
			kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_banker", VeStringA::From(VeUInt32(u32Chips))));
			break;
		case BACC_PLAYER:
			kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_player", VeStringA::From(VeUInt32(u32Chips))));
			break;
		default:
			kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_tie", VeStringA::From(VeUInt32(u32Chips))));
			break;
		}
		NativeOnEvent("ReqBaccaratBet", kEventValue);
	}
}

void Baccarat::Update(VeFloat32 f32Delta)
{
	UpdateCountDownTime();
}

void Baccarat::UpdateCountDownTime()
{
	if(m_bPlayRes)
	{
		SetCountDownTime(0);
	}
	else
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		VeUInt64 u64ExpireTime = pkPlayer->GetBaccExpireTime();
		VeUInt64 u64ServerTime = g_pkGame->GetServerTime();
		VeUInt32 u32Time = u64ExpireTime > u64ServerTime ? VeUInt32(u64ExpireTime - u64ServerTime) : 0;
		if(!u32Time) pkPlayer->S_ReqBaccaratUpdate();
		u32Time /= 100;
		SetCountDownTime(u32Time);
	}
}

void Baccarat::UpdateServer(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	pkPlayer->S_ReqBaccaratUpdate();
}

void Baccarat::UpdateAllPlayers()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_kBetPlayerNum->SetNumber(pkPlayer->GetBaccBetPlayer());
	m_kAllBetChips->SetNumber(pkPlayer->GetBaccAllBet());
}

bool Baccarat::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(m_bNeedProcessTouch)
	{
		m_kWidgets.BeginIteratorFromLast();
		while(!m_kWidgets.IsEnd())
		{
			UIWidget* pkWidget = m_kWidgets.GetIterNode()->m_tContent;
			m_kWidgets.Pre();
			VE_ASSERT(pkWidget);
			if(pkWidget->TestInput(eType, pkTouch))
			{
				UIWidget* pkRes = pkWidget->ProcessTouch(eType, pkTouch);
				if(pkRes)
				{
					if(pkRes != m_pkLastProcessWidget)
					{
						if(m_pkLastProcessWidget)
						{
							m_pkLastProcessWidget->EventRelease();
						}
						m_pkLastProcessWidget = pkRes;
					}
					return true;
				}
			}
		}

		if(m_pkLastProcessWidget)
		{
			m_pkLastProcessWidget->EventRelease();
			m_pkLastProcessWidget = NULL;
		}
	}
	return false;
}

void Baccarat::SendCard()
{
	VE_ASSERT(m_kBankerCard.Size() >= 2 && m_kPlayerCard.Size() >= 2);
	VE_ASSERT(m_kBankerCard.Size() <= 3 && m_kPlayerCard.Size() <= 3);
	for(VeUInt32 i(0); i < 2; ++i)
	{
		m_apkPlayerCard[i]->SetContent(m_kPlayerCard[i].m_tFirst, m_kPlayerCard[i].m_tSecond);
		m_apkPlayerCard[i]->Deal(0.3f + 0.6f * i);
		m_apkPlayerCard[i]->Turn(1.5f);
	}

	for(VeUInt32 i(0); i < 2; ++i)
	{
		m_apkBankerCard[i]->SetContent(m_kBankerCard[i].m_tFirst, m_kBankerCard[i].m_tSecond);
		m_apkBankerCard[i]->Deal(0.6f + 0.6f * i);
		m_apkBankerCard[i]->Turn(1.5f);
	}
	scheduleOnce(schedule_selector(Baccarat::OnSendCardStopFirst), 1.8f);
}

void Baccarat::OnSendCardStopFirst(VeFloat32 f32Delta)
{
	VeUInt32 u32Player(0), u32Banker(0);
	for(VeUInt32 i(0); i < 2; ++i)
	{
		if(m_kBankerCard[i].m_tFirst < CARD_10)
		{
			u32Banker += m_kBankerCard[i].m_tFirst + 1;
		}
		if(m_kPlayerCard[i].m_tFirst < CARD_10)
		{
			u32Player += m_kPlayerCard[i].m_tFirst + 1;
		}
	}
	m_pkPlayerScore->SetNumber(u32Player % 10);
	m_pkBankerScore->SetNumber(u32Banker % 10);

	if(m_kPlayerCard.Size() == 2 && m_kBankerCard.Size() == 2)
	{
		WinLosMusicEffect();
	}
	else
	{
		VeFloat32 f32Deal(0.3f);
		if(m_kPlayerCard.Size() == 3)
		{
			m_apkPlayerCard[2]->SetContent(m_kPlayerCard[2].m_tFirst, m_kPlayerCard[2].m_tSecond);
			m_apkPlayerCard[2]->Deal(f32Deal);
			f32Deal += 0.3f;
		}
		if(m_kBankerCard.Size() == 3)
		{
			m_apkBankerCard[2]->SetContent(m_kBankerCard[2].m_tFirst, m_kBankerCard[2].m_tSecond);
			m_apkBankerCard[2]->Deal(f32Deal);
			f32Deal += 0.3f;
		}
		if(m_kPlayerCard.Size() == 3)
		{
			m_apkPlayerCard[2]->Turn(f32Deal);
		}
		if(m_kBankerCard.Size() == 3)
		{
			m_apkBankerCard[2]->Turn(f32Deal);
		}
		scheduleOnce(schedule_selector(Baccarat::OnSendCardStopSecond), f32Deal + 0.3f);
	}
}

void Baccarat::OnSendCardStopSecond(VeFloat32 f32Delta)
{
	VeUInt32 u32Player(0), u32Banker(0);
	for(VeUInt32 i(0); i < 3; ++i)
	{
		if(m_kBankerCard.Size() > i && m_kBankerCard[i].m_tFirst < CARD_10)
		{
			u32Banker += m_kBankerCard[i].m_tFirst + 1;
		}
		if(m_kPlayerCard.Size() > i && m_kPlayerCard[i].m_tFirst < CARD_10)
		{
			u32Player += m_kPlayerCard[i].m_tFirst + 1;
		}
	}
	m_pkPlayerScore->SetNumber(u32Player % 10);
	m_pkBankerScore->SetNumber(u32Banker % 10);
	WinLosMusicEffect();
}

void Baccarat::WinLosMusicEffect()
{
	switch(m_eWinType)
	{
	case BACC_BANKER:
		{
			CCPoint kBgPointLeft = m_pkWin->getPosition();
			CCPoint kBgPointRight = m_pkLos->getPosition();
			m_pkWin->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCShow::create(),CCFadeIn::create(0.5f),NULL));
			m_pkWin->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCPlace::create(kBgPointRight),CCMoveTo::create(0.5f,m_apkBankerCard[1]->getPosition()),CCDelayTime::create(0.8f),CCHide::create(),CCPlace::create(kBgPointLeft),NULL));
			m_pkLos->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCShow::create(),CCFadeIn::create(0.5f),NULL));
			m_pkLos->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCPlace::create(kBgPointLeft),CCMoveTo::create(0.5f,m_apkPlayerCard[1]->getPosition()),CCDelayTime::create(0.8f),CCHide::create(),CCPlace::create(kBgPointRight),CCCallFunc::create(this,callfunc_selector(Baccarat::BankerWinMusicCallfunc)),NULL));
		}
		break;
	case BACC_PLAYER:
		{
			CCPoint kBgPointLeft = m_pkWin->getPosition();
			CCPoint kBgPointRight = m_pkLos->getPosition();
			m_pkWin->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCShow::create(),CCFadeIn::create(0.5f),NULL));
			m_pkWin->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCMoveTo::create(0.5f,m_apkPlayerCard[1]->getPosition()),CCDelayTime::create(0.8f),CCHide::create(),CCPlace::create(kBgPointLeft),NULL));
			m_pkLos->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCShow::create(),CCFadeIn::create(0.5f),NULL));
			m_pkLos->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCMoveTo::create(0.5f,m_apkBankerCard[1]->getPosition()),CCDelayTime::create(0.8f),CCHide::create(),CCPlace::create(kBgPointRight),CCCallFunc::create(this,callfunc_selector(Baccarat::PlayerWinMusicCallfunc)),NULL));
		}
		break;
	case BACC_TIE:
		{
			CCPoint kBgPointLeft = m_pkDrawGameLeft->getPosition();
			CCPoint kBgPointRight = m_pkDrawGameRight->getPosition();
			m_pkDrawGameLeft->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCShow::create(),CCFadeIn::create(0.5f),NULL));
			m_pkDrawGameLeft->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCMoveTo::create(0.5f,m_apkPlayerCard[1]->getPosition()),CCDelayTime::create(0.8f),CCHide::create(),CCPlace::create(kBgPointLeft),NULL));
			m_pkDrawGameRight->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCShow::create(),CCFadeIn::create(0.5f),NULL));
			m_pkDrawGameRight->runAction(CCSequence::create(CCDelayTime::create(1.3f),CCMoveTo::create(0.5f,m_apkBankerCard[1]->getPosition()),CCDelayTime::create(0.8f),CCHide::create(),CCPlace::create(kBgPointRight),CCCallFunc::create(this,callfunc_selector(Baccarat::DrawGameMusicCallfunc)),NULL));
		}
		break;
	default:
		break;
	}
}

void Baccarat::BankerWinMusicCallfunc()
{
	g_pSoundSystem->PlayEffect(SE_BANKERWIN);
	AllBetFadeOut();
}

void Baccarat::PlayerWinMusicCallfunc()
{
	g_pSoundSystem->PlayEffect(SE_PLAYERWIN);
	AllBetFadeOut();
}

void Baccarat::DrawGameMusicCallfunc()
{
	g_pSoundSystem->PlayEffect(SE_DRAWGAME);
	AllBetFadeOut();
}

void Baccarat::AllBetFadeOut()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	WIDGET(BetArea, bet_area);
	m_u64WinChips = bet_area.AllBetFadeOut(m_eWinType);
	m_u64Credit = pkPlayer->GetChips();
	if(m_u64WinChips)
	{
		scheduleOnce(schedule_selector(Baccarat::AfterAllBetFadeOut), 1.6f);
	}
	else
	{
		scheduleOnce(schedule_selector(Baccarat::AfterAllBetFadeOut), 2.4f);
	}
}

void Baccarat::AfterAllBetFadeOut(VeFloat32 f32Delta)
{
	SetWin(m_u64WinChips);
	if(m_u64WinChips)
	{
		scheduleOnce(schedule_selector(Baccarat::ScheduleWinChipsUpdate), 0.4f);
	}
	else
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqBaccaratPlay();
		m_bPlayRes = false;
	}
}

void Baccarat::ScheduleWinChipsUpdate(VeFloat32 f32Delta)
{
	schedule(schedule_selector(Baccarat::WinChipsUpdate),0.01f);
}

void Baccarat::WinChipsUpdate(VeFloat32 f32Delta)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;

	unsigned int subtractor = 0;

	if ((m_u64WinChips > 0) && (m_u64WinChips <= 50))
	{
		subtractor = 1;
	}
	else if ((m_u64WinChips > 50) && (m_u64WinChips <= 100))
	{
		subtractor = 10;
	}
	else if ((m_u64WinChips > 100) && (m_u64WinChips <= 1000))
	{
		subtractor = 100;
	}
	else if ((m_u64WinChips > 1000) && (m_u64WinChips <= 10000))
	{
		subtractor = 1000;
	}
	else if ((m_u64WinChips > 10000) && (m_u64WinChips <= 100000))
	{
		subtractor = 10000;
	}
	else if ((m_u64WinChips > 100000) && (m_u64WinChips <= 1000000))
	{
		subtractor = 100000;
	}
	else if ((m_u64WinChips > 1000000) && (m_u64WinChips <= 10000000))
	{
		subtractor = 1000000;
	}
	else if ((m_u64WinChips > 10000000) && (m_u64WinChips <= 100000000))
	{
		subtractor = 10000000;
	}
	else
	{
		if (m_u64WinChips!=0)
		{
			subtractor = 100000000;
		}
	}
	if (m_u64WinChips>0)
	{
		if (m_u64WinChips>=subtractor)
		{
			m_u64WinChips -= subtractor;
			m_u64Credit += subtractor;
			SetChips(m_u64WinChips,m_u64Credit);
		}
		else
		{
			m_u64WinChips -= m_u64WinChips;
			SetChips(m_u64WinChips,m_u64Credit+m_u64WinChips);
		}

	}
	else
	{
		unschedule(schedule_selector(Baccarat::WinChipsUpdate));
		pkPlayer->S_ReqBaccaratPlay();
		m_bPlayRes = false;
	}
}

void Baccarat::UpdateBaccResult(bool bMotion)
{
	if(m_bPlayRes) return;
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_pkPlayerScore->SetNumber(0);
	m_pkBankerScore->SetNumber(0);
	for(VeUInt32 i(0); i < 3; ++i)
	{
		m_apkPlayerCard[i]->Reset();
		m_apkBankerCard[i]->Reset();
		m_apkPlayerCard[i]->SetVisible(false);
		m_apkBankerCard[i]->SetVisible(false);
		m_apkPlayerCard[i]->SetType(pkPlayer->GetVip() ? 1 : 0);
		m_apkBankerCard[i]->SetType(pkPlayer->GetVip() ? 1 : 0);
	}
	m_pkDrawGameLeft->setVisible(false);
	m_pkDrawGameRight->setVisible(false);
	m_pkWin->setVisible(false);
	m_pkLos->setVisible(false);
	m_pkDrawGameLeft->stopAllActions();
	m_pkDrawGameRight->stopAllActions();
	m_pkWin->stopAllActions();
	m_pkLos->stopAllActions();
	SetChips(0, pkPlayer->GetChips());
	WIDGET(BetArea, bet_area);
	VeChar8 acBuffer[128];
	VeStrcpy(acBuffer, 128, pkPlayer->GetBaccaratResult());
	VeChar8* pcContext;
	VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);

	if(pcTemp && (!VeStrcmp(pcTemp, "res")))
	{
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		BaccaratBetType eWinType = (BaccaratBetType)VeAtoi(pcTemp);
		VE_ASSERT(eWinType < BaccaratBetType_MAX);
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		VeUInt32 u32Seed = VeAtoi(pcTemp);
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		CalculateRes(eWinType, u32Seed);
		SendCard();
		m_bPlayRes = true;
	}
	else
	{
		m_bPlayRes = false;
	}
	
	VeUInt32 u32Banker(0), u32Player(0), u32Tie(0);
	if(pcTemp && (!VeStrcmp(pcTemp, "buy")))
	{
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		if(pcTemp) u32Banker = VeAtoi(pcTemp);
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		if(pcTemp) u32Player = VeAtoi(pcTemp);
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		if(pcTemp) u32Tie = VeAtoi(pcTemp);
	}
	SetBetChips(u32Player, u32Banker, u32Tie);
	VeUInt32 u32AllBet = u32Banker + u32Player + u32Tie;
	VeUInt32 u32Limit = pkPlayer->GetBaccBetLimit();
	u32Limit = u32Limit >= u32AllBet ? u32Limit - u32AllBet : 0;
	bet_area.UpdateCanBet(VE_MIN(u32Limit, pkPlayer->GetChips()));
	bet_area.UpdateChips(u32Banker, u32Player, u32Tie, u32AllBet ? bMotion : false);
	bet_area.SetPlayRes(m_bPlayRes);
	VeUInt32 u32LastBet = m_u32TieNum + m_u32PlayerNum + m_u32BankerNum;
	UpdateCountDownTime();
	if(m_bPlayRes)
	{
		m_pkRepeatBet->SetEnable(false);
		m_pkDoubleBet->SetEnable(false);
		m_u32TieNum = u32Tie;
		m_u32PlayerNum = u32Player;
		m_u32BankerNum = u32Banker;
	}
	else
	{
		m_pkRepeatBet->SetEnable(u32AllBet ? false : (u32LastBet > 0 && u32LastBet <= u32Limit));
		m_pkDoubleBet->SetEnable(u32AllBet > 0 && u32AllBet <= u32Limit);
	}
}

void Baccarat::OnForceUpdate()
{
	UpdateBaccResult(true);
}

void CalculateCard(VeUInt32 u32Point, VeUInt32 u32Threshold, VeVector<VeUInt32>& kOut)
{
	kOut.Clear();
	VE_ASSERT(u32Point <= 9 && u32Threshold <= 9);
	VeUInt32 u32Num = 3;
	if(u32Point >= u32Threshold)
	{
		u32Num = (VeUInt32(VeRand() % 10) >= u32Threshold) ? 2 : 3;
	}
	VeUInt32 u32Point0 = VeRand() % 10;
	if(u32Num == 2)
	{
		VeUInt32 u32Point1;
		if(u32Point0 > u32Point)
		{
			u32Point1 = (u32Point + 10) - u32Point0;
		}
		else
		{
			u32Point1 = u32Point - u32Point0;
		}
		VE_ASSERT(u32Point0 <= 9 && u32Point1 <= 9);
		kOut.Resize(2);
		kOut[0] = u32Point0;
		kOut[1] = u32Point1;
	}
	else
	{
		VeUInt32 u32Point01 = VeRand() % u32Threshold;
		VeUInt32 u32Point1 = u32Point01 >= u32Point0 ? u32Point01 - u32Point0 : (u32Point01 + 10) - u32Point0;
		VE_ASSERT((u32Point0 + u32Point1) % 10 == u32Point01);
		VeUInt32 u32Point2 = u32Point >= u32Point01 ? u32Point - u32Point01 : (u32Point + 10) - u32Point01;
		VE_ASSERT(u32Point0 <= 9 && u32Point1 <= 9 && u32Point2 <= 9);
		kOut.Resize(3);
		kOut[0] = u32Point0;
		kOut[1] = u32Point1;
		kOut[2] = u32Point2;
	}
}

bool HasCard(const VePair<CardNum, CardSuit>& kCard, const VeVector< VePair<CardNum, CardSuit> >& kUsed)
{
	for(VeUInt32 i(0); i < kUsed.Size(); ++i)
	{
		if(kUsed[i] == kCard)
		{
			return true;
		}
	}
	return false;
}

bool VerifyCard(const VeVector<VeUInt32>& kBanker, const VeVector<VeUInt32>& kPlayer)
{
	VeUInt32 au32Buffer[10];
	VeZeroMemory(au32Buffer, sizeof(au32Buffer));
	for(VeUInt32 i(0); i < kBanker.Size(); ++i)
	{
		VE_ASSERT(kBanker[i] <= 9);
		++au32Buffer[kBanker[i]];
	}
	for(VeUInt32 i(1); i < 10; ++i)
	{
		if(au32Buffer[i] > 4)
		{
			return false;
		}
	}
	return true;
}

void CalculateCardType(const VeVector<VeUInt32>& kPoint, VeVector< VePair<CardNum, CardSuit> >& kOut, VeVector< VePair<CardNum, CardSuit> >& kUsed)
{
	kOut.Clear();
	VeVector< VePair<CardNum, CardSuit> > kChoose;
	VePair<CardNum, CardSuit> kPair;
	for(VeUInt32 i(0); i < kPoint.Size(); ++i)
	{
		kChoose.Clear();
		VeUInt32 u32Point = kPoint[i];
		if(u32Point)
		{
			CardNum eNum = (CardNum)(u32Point - 1);
			VE_ASSERT(eNum < CARD_10);
			for(VeUInt32 j(0); j < CardSuit_MAX; ++j)
			{
				kPair.m_tFirst = eNum;
				kPair.m_tSecond = CardSuit(j);
				if(!HasCard(kPair, kUsed)) kChoose.PushBack(kPair);
			}
		}
		else
		{
			for(VeUInt32 j(0); j < CardSuit_MAX; ++j)
			{
				for(VeUInt32 k(CARD_10); k < CardNum_MAX; ++k)
				{
					kPair.m_tFirst = CardNum(k);
					kPair.m_tSecond = CardSuit(j);
					if(!HasCard(kPair, kUsed)) kChoose.PushBack(kPair);
				}
			}
		}
		VE_ASSERT(kChoose.Size());
		VeUInt32 u32Index = VeRand() % kChoose.Size();
		kOut.PushBack(kChoose[u32Index]);
	}
}

void Baccarat::CalculateRes(BaccaratBetType eWinType, VeUInt32 u32Seed)
{
	VeSrand(u32Seed);
	m_eWinType = eWinType;
	VeUInt32 u32Banker, u32Player;
	switch(eWinType)
	{
	case BACC_BANKER:
		u32Player = VeRand() % 9;
		u32Banker = 8 - u32Player;
		u32Banker = (u32Banker ? (VeRand() % u32Banker) : 0) + u32Player + 1;
		VE_ASSERT(u32Banker > u32Player);
		break;
	case BACC_PLAYER:
		u32Banker = VeRand() % 9;
		u32Player = 8 - u32Banker;
		u32Player = (u32Player ? (VeRand() % u32Player) : 0) + u32Banker + 1;
		VE_ASSERT(u32Banker < u32Player);
		break;
	case BACC_TIE:
		u32Banker = VeRand() % 10;
		u32Player = u32Banker;
		break;
	default:
		VE_ASSERT(!"Unknown error");
		break;
	}
	VeVector<VeUInt32> kBanker, kPlayer;
	do 
	{
		CalculateCard(u32Banker, 7, kBanker);
		CalculateCard(u32Player, 6, kPlayer);
	}
	while(!VerifyCard(kBanker, kPlayer));
	
	VeVector< VePair<CardNum, CardSuit> > kUsed;
	CalculateCardType(kBanker, m_kBankerCard, kUsed);
	CalculateCardType(kPlayer, m_kPlayerCard, kUsed);
	m_u32SendCard = 0;
}

void Baccarat::OnPushNotice(const VeChar8* pcNotice)
{
	WIDGET(NoticeBar, notice_bar);
	notice_bar.Notice(pcNotice, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale));
}

BetAreaChip::BetAreaChip(const VeChar8* pcNormal,
	const VeChar8* pcPressed, const VeChar8* pcDisabled)
	: UIChooseButton(pcNormal, pcPressed, pcDisabled)
	, m_bTouch(false)
{

}

UIWidget* BetAreaChip::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(m_bEnabled && m_bVisible)
	{
		UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
		if(pkWidget) return pkWidget;

		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			m_kBeganPos = pkTouch->getLocation();
			SetTouch(true);
			break;
		case UILayer::TOUCH_MOVED:
			if(m_bTouch)
			{
				CCPoint kTemp = pkTouch->getLocation();
				kTemp.x = kTemp.x - m_kBeganPos.x + m_kStart.x;
				kTemp.y = kTemp.y - m_kBeganPos.y + m_kStart.y;
				setPosition(kTemp);
			}
			break;
		case UILayer::TOUCH_ENDED:
			EventRelease();
			break;
		case UILayer::TOUCH_CANCELLED:
			EventRelease();
			break;
		default:
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

bool BetAreaChip::IsIn(VeFloat32 x, VeFloat32 y)
{
	if(m_bTouch) return true;
	else return UIChooseButton::IsIn(x, y);
}

void BetAreaChip::EventRelease()
{
	SetTouch(false);
}

void BetAreaChip::SetTouch(bool bTouch)
{
	if(bTouch != m_bTouch)
	{
		setZOrder(bTouch ? 5 : 3);
		SetChoose(bTouch);
		m_bTouch = bTouch;
		if(m_bTouch)
		{
			m_kStart = getPosition();
			ToTop();
			m_kEvent.Callback(*this, EVENT_TOUCH);
		}
		else
		{
			setPosition(m_kStart);
			m_kEvent.Callback(*this, EVENT_RELEASE);
		}
	}
}

BetArea::BetArea()
{
	UIInitDelegate(BetArea, OnChipClicked0);
	UIInitDelegate(BetArea, OnChipClicked1);
	UIInitDelegate(BetArea, OnChipClicked2);
	UIInitDelegate(BetArea, OnChipClicked3);
	UIInitDelegate(BetArea, OnChipClicked4);
	UIInitDelegate(BetArea, OnChipClicked5);

	CCSpriteBatchNode* pkKenoRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("keno", false) + ".png");
	addChild(pkKenoRoot);

	for(VeUInt32 i(0); i < 6; ++i)
	{
		VeUInt32 u32FileNum = s_au32ChipsMap[i];
		VeChar8 betName_n[64];
		VeSprintf(betName_n, 64, "bacc_%d_n.png", u32FileNum);

		VeChar8 betName_p[64];
		VeSprintf(betName_p, 64, "bacc_%d_p.png", u32FileNum);

		VeChar8 betName_d[64];
		VeSprintf(betName_d, 64, "bacc_%d_d.png", u32FileNum);

		m_apkAreaChips[i] = VE_NEW BetAreaChip(betName_n, betName_p, betName_d);
		m_apkAreaChips[i]->setPosition(ccp(UILayer::Width() * 0.5f + (52.0f * i - 130.0f) * g_f32ScaleHD * ui_main_scale, 100.0f * g_f32ScaleHD * ui_main_scale));
		m_apkAreaChips[i]->setScaleX(ui_main_scale);
		m_apkAreaChips[i]->setScaleY(ui_main_scale);
		AddChild(m_apkAreaChips[i]);
	}
	m_pkTouchedChip = NULL;
	UILinkDelegate(m_apkAreaChips[0], OnChipClicked0);
	UILinkDelegate(m_apkAreaChips[1], OnChipClicked1);
	UILinkDelegate(m_apkAreaChips[2], OnChipClicked2);
	UILinkDelegate(m_apkAreaChips[3], OnChipClicked3);
	UILinkDelegate(m_apkAreaChips[4], OnChipClicked4);
	UILinkDelegate(m_apkAreaChips[5], OnChipClicked5);

	m_pkTieBetAreaL_n = CCSprite::createWithSpriteFrameName("bacc_area_2_n.png");
	m_pkTieBetAreaL_n->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkTieBetAreaL_n->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkTieBetAreaL_n->setScaleX(ui_main_scale);
	m_pkTieBetAreaL_n->setScaleY(ui_main_scale);
	pkKenoRoot->addChild(m_pkTieBetAreaL_n, 3);

	m_pkTieBetAreaR_n = CCSprite::createWithSpriteFrameName("bacc_area_2_n.png");
	m_pkTieBetAreaR_n->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkTieBetAreaR_n->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkTieBetAreaR_n->setScaleX(-ui_main_scale);
	m_pkTieBetAreaR_n->setScaleY(ui_main_scale);
	pkKenoRoot->addChild(m_pkTieBetAreaR_n, 3);

	m_pkTieBetAreaL_p = CCSprite::createWithSpriteFrameName("bacc_area_2_p.png");
	m_pkTieBetAreaL_p->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkTieBetAreaL_p->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkTieBetAreaL_p->setScaleX(ui_main_scale);
	m_pkTieBetAreaL_p->setScaleY(ui_main_scale);
	m_pkTieBetAreaL_p->setVisible(false);
	pkKenoRoot->addChild(m_pkTieBetAreaL_p, 3);

	m_pkTieBetAreaR_p = CCSprite::createWithSpriteFrameName("bacc_area_2_p.png");
	m_pkTieBetAreaR_p->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkTieBetAreaR_p->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkTieBetAreaR_p->setScaleX(-ui_main_scale);
	m_pkTieBetAreaR_p->setScaleY(ui_main_scale);
	m_pkTieBetAreaR_p->setVisible(false);
	pkKenoRoot->addChild(m_pkTieBetAreaR_p, 3);

	m_pkPlayerBetArea_n = CCSprite::createWithSpriteFrameName("bacc_area_1_n.png");
	m_pkPlayerBetArea_n->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 48 * ui_main_scale) * g_f32ScaleHD)));
	m_pkPlayerBetArea_n->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkPlayerBetArea_n->setScaleX(ui_main_scale);
	m_pkPlayerBetArea_n->setScaleY(ui_main_scale);
	pkKenoRoot->addChild(m_pkPlayerBetArea_n, 3);

	m_pkPlayerBetArea_p = CCSprite::createWithSpriteFrameName("bacc_area_1_p.png");
	m_pkPlayerBetArea_p->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 48 * ui_main_scale) * g_f32ScaleHD)));
	m_pkPlayerBetArea_p->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkPlayerBetArea_p->setScaleX(ui_main_scale);
	m_pkPlayerBetArea_p->setScaleY(ui_main_scale);
	m_pkPlayerBetArea_p->setVisible(false);
	pkKenoRoot->addChild(m_pkPlayerBetArea_p, 3);

	m_pkBankerBetArea_n = CCSprite::createWithSpriteFrameName("bacc_area_1_n.png");
	m_pkBankerBetArea_n->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 48 * ui_main_scale) * g_f32ScaleHD)));
	m_pkBankerBetArea_n->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkBankerBetArea_n->setScaleX(-ui_main_scale);
	m_pkBankerBetArea_n->setScaleY(ui_main_scale);
	pkKenoRoot->addChild(m_pkBankerBetArea_n, 3);

	m_pkBankerBetArea_p = CCSprite::createWithSpriteFrameName("bacc_area_1_p.png");
	m_pkBankerBetArea_p->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 48 * ui_main_scale) * g_f32ScaleHD)));
	m_pkBankerBetArea_p->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkBankerBetArea_p->setScaleX(-ui_main_scale);
	m_pkBankerBetArea_p->setScaleY(ui_main_scale);
	m_pkBankerBetArea_p->setVisible(false);
	pkKenoRoot->addChild(m_pkBankerBetArea_p, 3);
	m_eAreaState = AREA_MAX;
	SetChooseArea(AREA_MAX);

	m_akAreas[AREA_BANKER] = m_pkBankerBetArea_n->boundingBox();
	m_akAreas[AREA_PLAYER] = m_pkPlayerBetArea_n->boundingBox();
	m_akAreas[AREA_TIE] = m_pkTieBetAreaL_n->boundingBox();
	m_akAreas[AREA_TIE].size.width *= 2;

	CCSprite* m_pkTieText = CCSprite::createWithSpriteFrameName("bacc_tie.png");
	m_pkTieText->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkTieText->setScaleX(ui_main_scale);
	m_pkTieText->setScaleY(ui_main_scale);
	addChild(m_pkTieText);

	CCSprite* m_pkPays8to1TextL = CCSprite::createWithSpriteFrameName("bacc_pays8.png");
	m_pkPays8to1TextL->setPosition(ccp(UILayer::Width() * 0.5f - 66.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkPays8to1TextL->setScaleX(ui_main_scale);
	m_pkPays8to1TextL->setScaleY(ui_main_scale);
	addChild(m_pkPays8to1TextL);

	CCSprite* m_pkPays8to1TextR = CCSprite::createWithSpriteFrameName("bacc_pays8.png");
	m_pkPays8to1TextR->setPosition(ccp(UILayer::Width() * 0.5f + 66.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkPays8to1TextR->setScaleX(ui_main_scale);
	m_pkPays8to1TextR->setScaleY(ui_main_scale);
	addChild(m_pkPays8to1TextR);

	CCSprite* m_pkPlayerText = CCSprite::createWithSpriteFrameName("bacc_player.png");
	m_pkPlayerText->setPosition(ccp(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkPlayerText->setScaleX(ui_main_scale);
	m_pkPlayerText->setScaleY(ui_main_scale);
	addChild(m_pkPlayerText);

	CCSprite* m_pkBankerText = CCSprite::createWithSpriteFrameName("bacc_banker.png");
	m_pkBankerText->setPosition(ccp(UILayer::Width() * 0.5f + 60.0f * g_f32ScaleHD * ui_main_scale, VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 23 * ui_main_scale) * g_f32ScaleHD)));
	m_pkBankerText->setScaleX(ui_main_scale);
	m_pkBankerText->setScaleY(ui_main_scale);
	addChild(m_pkBankerText);

	m_pkTieAreaChipsIn = VE_NEW GameChips;
	m_pkTieAreaChipsIn->setPosition(ccp(UILayer::Width() * 0.5f,
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale - 21 * ui_main_scale) * g_f32ScaleHD)));
	AddChild(m_pkTieAreaChipsIn);

	m_pkPlayerAreaChipsIn = VE_NEW GameChips;
	m_pkPlayerAreaChipsIn->setPosition(ccp(UILayer::Width() * 0.5f - 60.0f * g_f32ScaleHD * ui_main_scale,
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 47 * ui_main_scale) * g_f32ScaleHD)));
	AddChild(m_pkPlayerAreaChipsIn);

	m_pkBankerAreaChipsIn = VE_NEW GameChips;
	m_pkBankerAreaChipsIn->setPosition(ccp(UILayer::Width() * 0.5f + 60.0f * g_f32ScaleHD * ui_main_scale,
		VeFloorf(UILayer::Height() * 0.5f - (18 * ui_height_scale + 47 * ui_main_scale) * g_f32ScaleHD)));
	AddChild(m_pkBankerAreaChipsIn);

	for(VeUInt32 i(0); i < 3; ++i)
	{
		m_apkGameChipsMove[i] = VE_NEW GameChipsMove;
		AddChild(m_apkGameChipsMove[i]);
	}
	
	m_u32TieNum = 0;
	m_u32PlayerNum = 0;
	m_u32BankerNum = 0;
}

UIWidget* BetArea::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(m_bEnabled && m_bVisible)
	{
		UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
		if(pkWidget)
		{
			if(eType == UILayer::TOUCH_MOVED && pkWidget == m_pkTouchedChip)
			{
				BaccArea eArea;
				if(m_akAreas[AREA_BANKER].containsPoint(pkTouch->getLocation()))
				{
					eArea = AREA_BANKER;
				}
				else if(m_akAreas[AREA_PLAYER].containsPoint(pkTouch->getLocation()))
				{
					eArea = AREA_PLAYER;
				}
				else if(m_akAreas[AREA_TIE].containsPoint(pkTouch->getLocation()))
				{
					eArea = AREA_TIE;
				}
				else
				{
					CCAffineTransform kTransform = pkWidget->nodeToWorldTransform();
					CCRect kRect = pkWidget->boundingBox();
					kRect.origin.x = kTransform.tx - kRect.size.width * m_kAnchorPoint.x;
					kRect.origin.y = kTransform.ty - kRect.size.height * m_kAnchorPoint.y;
					if(m_akAreas[AREA_BANKER].intersectsRect(kRect))
					{
						eArea = AREA_BANKER;
					}
					else if(m_akAreas[AREA_PLAYER].intersectsRect(kRect))
					{
						eArea = AREA_PLAYER;
					}
					else if(m_akAreas[AREA_TIE].intersectsRect(kRect))
					{
						eArea = AREA_TIE;
					}
					else
					{
						eArea = AREA_MAX;
					}
				}
				SetChooseArea(eArea);
			}
			return pkWidget;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void BetArea::EventRelease()
{
	SetTouchedChip(VE_INFINITE);
}

bool BetArea::IsIn(VeFloat32 x, VeFloat32 y)
{
	return true;
}

VeUInt32 BetArea::AllBetFadeOut(BaccaratBetType eType)
{
	VeUInt32 u32Res(0);
	CCPoint kChipsPosWin = ccp(VeFloorf(UILayer::Width() * 0.5f - 40.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD));
	CCPoint kChipsPosLose = ui_ccp(0.5f, 1.0f);

	m_pkTieAreaChipsIn->SetFadePos(kChipsPosLose,kChipsPosLose);
	m_pkPlayerAreaChipsIn->SetFadePos(kChipsPosLose,kChipsPosLose);
	m_pkBankerAreaChipsIn->SetFadePos(kChipsPosLose,kChipsPosLose);

	switch(eType)
	{
	case 0:
		m_apkGameChipsMove[0]->SetFadePos(kChipsPosLose, m_pkBankerAreaChipsIn->getPosition());
		m_apkGameChipsMove[0]->Move(m_u32BankerNum);
		m_pkBankerAreaChipsIn->ChangeNumberDelay(m_u32BankerNum * 2,0.3f);
		m_pkBankerAreaChipsIn->SetFadePos(kChipsPosWin, kChipsPosWin);
		if (m_u32BankerNum)
		{
			g_pSoundSystem->PlayEffect(SE_CHIPS_WIN);
			u32Res = m_u32BankerNum * 2;
		}
		break;
	case 1:
		m_apkGameChipsMove[1]->SetFadePos(kChipsPosLose, m_pkPlayerAreaChipsIn->getPosition());
		m_apkGameChipsMove[1]->Move(m_u32PlayerNum);
		m_pkPlayerAreaChipsIn->ChangeNumberDelay(m_u32PlayerNum * 2,0.3f);
		m_pkPlayerAreaChipsIn->SetFadePos(kChipsPosWin, kChipsPosWin);
		if (m_u32PlayerNum)
		{
			g_pSoundSystem->PlayEffect(SE_CHIPS_WIN);
			u32Res = m_u32PlayerNum * 2;
		}
		break;
	case 2:
		m_apkGameChipsMove[2]->SetFadePos(kChipsPosLose, m_pkTieAreaChipsIn->getPosition());
		m_apkGameChipsMove[2]->Move(m_u32TieNum * 7);
		m_pkTieAreaChipsIn->ChangeNumberDelay(m_u32TieNum * 8,0.3f);
		m_pkTieAreaChipsIn->SetFadePos(kChipsPosWin, kChipsPosWin);
		if (m_u32TieNum)
		{
			g_pSoundSystem->PlayEffect(SE_CHIPS_WIN);
			u32Res = m_u32TieNum * 8;
		}
		break;
	default:
		break;
	}

	if(m_u32TieNum)
	{
		m_pkTieAreaChipsIn->FadeOut(0.6f);
		m_u32TieNum = 0;
	}
	if(m_u32PlayerNum)
	{
		m_pkPlayerAreaChipsIn->FadeOut(0.6f);
		m_u32PlayerNum = 0;
	}
	if(m_u32BankerNum)
	{
		m_pkBankerAreaChipsIn->FadeOut(0.6f);
		m_u32BankerNum = 0;
	}
	return u32Res;
}

void BetArea::SetChooseArea(BaccArea eArea)
{
	if(m_eAreaState != eArea)
	{
		switch(eArea)
		{
		case AREA_BANKER:
			m_pkTieBetAreaL_n->setVisible(true);
			m_pkTieBetAreaR_n->setVisible(true);
			m_pkTieBetAreaL_p->setVisible(false);
			m_pkTieBetAreaR_p->setVisible(false);
			m_pkPlayerBetArea_n->setVisible(true);
			m_pkPlayerBetArea_p->setVisible(false);
			m_pkBankerBetArea_n->setVisible(false);
			m_pkBankerBetArea_p->setVisible(true);
			break;
		case AREA_PLAYER:
			m_pkTieBetAreaL_n->setVisible(true);
			m_pkTieBetAreaR_n->setVisible(true);
			m_pkTieBetAreaL_p->setVisible(false);
			m_pkTieBetAreaR_p->setVisible(false);
			m_pkPlayerBetArea_n->setVisible(false);
			m_pkPlayerBetArea_p->setVisible(true);
			m_pkBankerBetArea_n->setVisible(true);
			m_pkBankerBetArea_p->setVisible(false);
			break;
		case AREA_TIE:
			m_pkTieBetAreaL_n->setVisible(false);
			m_pkTieBetAreaR_n->setVisible(false);
			m_pkTieBetAreaL_p->setVisible(true);
			m_pkTieBetAreaR_p->setVisible(true);
			m_pkPlayerBetArea_n->setVisible(true);
			m_pkPlayerBetArea_p->setVisible(false);
			m_pkBankerBetArea_n->setVisible(true);
			m_pkBankerBetArea_p->setVisible(false);
			break;
		default:
			m_pkTieBetAreaL_n->setVisible(true);
			m_pkTieBetAreaR_n->setVisible(true);
			m_pkTieBetAreaL_p->setVisible(false);
			m_pkTieBetAreaR_p->setVisible(false);
			m_pkPlayerBetArea_n->setVisible(true);
			m_pkPlayerBetArea_p->setVisible(false);
			m_pkBankerBetArea_n->setVisible(true);
			m_pkBankerBetArea_p->setVisible(false);
			break;
		}
		m_eAreaState = eArea;
	}	
}

void BetArea::UpdateCanBet(VeUInt32 u32CanBet)
{
	VeUInt32 u32Enable(0);
	for(VeUInt32 i(0); i < 6; ++i)
	{
		if(s_au32ChipsMap[i] > u32CanBet)
			break;
		++u32Enable;
	}
	for(VeUInt32 i(0); i < u32Enable; ++i)
	{
		m_apkAreaChips[i]->SetEnable(true);
	}
	for(VeUInt32 i(u32Enable); i < 6; ++i)
	{
		m_apkAreaChips[i]->SetEnable(false);
	}
}

void BetArea::UpdateChips(VeUInt32 u32BankerNum, VeUInt32 u32PlayerNum, VeUInt32 u32TieNum, bool bMotion)
{
	if(bMotion)
	{
		CCPoint kChipsPos = ccp(VeFloorf(UILayer::Width() * 0.5f + 64.0f * ui_main_scale * g_f32ScaleHD),
			VeFloorf(25.0f * ui_main_scale * g_f32ScaleHD));

		if(m_u32TieNum != u32TieNum)
		{
			m_pkTieAreaChipsIn->SetFadePos(kChipsPos, m_pkTieAreaChipsIn->getPosition());
			m_pkTieAreaChipsIn->SetNumber(u32TieNum);
			if(!u32TieNum)
			{
				m_pkTieAreaChipsIn->SetVisible(false);
			}
			else if(m_u32TieNum && m_u32TieNum < u32TieNum)
			{
				m_apkGameChipsMove[0]->SetFadePos(kChipsPos, m_pkTieAreaChipsIn->getPosition());
				m_apkGameChipsMove[0]->Move(u32TieNum - m_u32TieNum);
			}
			else
			{
				m_pkTieAreaChipsIn->FadeIn();
			}
			
			m_u32TieNum = u32TieNum;
			g_pSoundSystem->PlayEffect(SE_BET);
		}

		if(m_u32PlayerNum != u32PlayerNum)
		{
			m_pkPlayerAreaChipsIn->SetFadePos(kChipsPos, m_pkPlayerAreaChipsIn->getPosition());
			m_pkPlayerAreaChipsIn->SetNumber(u32PlayerNum);
			if(!u32PlayerNum)
			{
				m_pkPlayerAreaChipsIn->SetVisible(false);
			}
			else if(m_u32PlayerNum && m_u32PlayerNum < u32PlayerNum)
			{
				m_apkGameChipsMove[1]->SetFadePos(kChipsPos, m_pkPlayerAreaChipsIn->getPosition());
				m_apkGameChipsMove[1]->Move(u32PlayerNum - m_u32PlayerNum);
			}
			else
			{
				m_pkPlayerAreaChipsIn->FadeIn();
			}

			m_u32PlayerNum = u32PlayerNum;
			g_pSoundSystem->PlayEffect(SE_BET);
		}

		if(m_u32BankerNum != u32BankerNum)
		{
			m_pkBankerAreaChipsIn->SetFadePos(kChipsPos, m_pkBankerAreaChipsIn->getPosition());
			m_pkBankerAreaChipsIn->SetNumber(u32BankerNum);
			if(!u32BankerNum)
			{
				m_pkBankerAreaChipsIn->SetVisible(false);
			}
			else if(m_u32BankerNum && m_u32BankerNum < u32BankerNum)
			{
				m_apkGameChipsMove[2]->SetFadePos(kChipsPos, m_pkBankerAreaChipsIn->getPosition());
				m_apkGameChipsMove[2]->Move(u32BankerNum - m_u32BankerNum);
			}
			else
			{
				m_pkBankerAreaChipsIn->FadeIn();
			}

			m_u32BankerNum = u32BankerNum;
			g_pSoundSystem->PlayEffect(SE_BET);
		}
	}
	else
	{
		m_pkTieAreaChipsIn->SetNumber(u32TieNum);
		m_pkTieAreaChipsIn->SetVisible(u32TieNum ? true : false);

		m_pkPlayerAreaChipsIn->SetNumber(u32PlayerNum);
		m_pkPlayerAreaChipsIn->SetVisible(u32PlayerNum ? true : false);

		m_pkBankerAreaChipsIn->SetNumber(u32BankerNum);
		m_pkBankerAreaChipsIn->SetVisible(u32BankerNum ? true : false);

		m_u32TieNum = u32TieNum;
		m_u32PlayerNum = u32PlayerNum;
		m_u32BankerNum = u32BankerNum;
	}
}

UIImplementDelegate(BetArea, OnChipClicked0, kWidget, u32State)
{
	if(u32State == BetAreaChip::EVENT_TOUCH)
	{
		SetTouchedChip(0);
	}
	else if(u32State == BetAreaChip::EVENT_RELEASE)
	{
		if(m_eAreaState < AREA_MAX)
		{
			VeUInt32 u32Event = BACC_EVENT_FLAG | (BACC_EVENT_CHIPS_MASK & 0);
			m_kEvent.Callback(*this, u32Event | (m_eAreaState << 8));
		}
		SetTouchedChip(VE_INFINITE);
		SetChooseArea(AREA_MAX);
	}
}

UIImplementDelegate(BetArea, OnChipClicked1, kWidget, u32State)
{
	if(u32State == BetAreaChip::EVENT_TOUCH)
	{
		SetTouchedChip(1);
	}
	else if(u32State == BetAreaChip::EVENT_RELEASE)
	{
		if(m_eAreaState < AREA_MAX)
		{
			VeUInt32 u32Event = BACC_EVENT_FLAG | (BACC_EVENT_CHIPS_MASK & 1);
			m_kEvent.Callback(*this, u32Event | (m_eAreaState << 8));
		}
		SetTouchedChip(VE_INFINITE);
		SetChooseArea(AREA_MAX);
	}
}

UIImplementDelegate(BetArea, OnChipClicked2, kWidget, u32State)
{
	if(u32State == BetAreaChip::EVENT_TOUCH)
	{
		SetTouchedChip(2);
	}
	else if(u32State == BetAreaChip::EVENT_RELEASE)
	{
		if(m_eAreaState < AREA_MAX)
		{
			VeUInt32 u32Event = BACC_EVENT_FLAG | (BACC_EVENT_CHIPS_MASK & 2);
			m_kEvent.Callback(*this, u32Event | (m_eAreaState << 8));
		}
		SetTouchedChip(VE_INFINITE);
		SetChooseArea(AREA_MAX);
	}
}

UIImplementDelegate(BetArea, OnChipClicked3, kWidget, u32State)
{
	if(u32State == BetAreaChip::EVENT_TOUCH)
	{
		SetTouchedChip(3);
	}
	else if(u32State == BetAreaChip::EVENT_RELEASE)
	{
		if(m_eAreaState < AREA_MAX)
		{
			VeUInt32 u32Event = BACC_EVENT_FLAG | (BACC_EVENT_CHIPS_MASK & 3);
			m_kEvent.Callback(*this, u32Event | (m_eAreaState << 8));
		}
		SetTouchedChip(VE_INFINITE);
		SetChooseArea(AREA_MAX);
	}
}

UIImplementDelegate(BetArea, OnChipClicked4, kWidget, u32State)
{
	if(u32State == BetAreaChip::EVENT_TOUCH)
	{
		SetTouchedChip(4);
	}
	else if(u32State == BetAreaChip::EVENT_RELEASE)
	{
		if(m_eAreaState < AREA_MAX)
		{
			VeUInt32 u32Event = BACC_EVENT_FLAG | (BACC_EVENT_CHIPS_MASK & 4);
			m_kEvent.Callback(*this, u32Event | (m_eAreaState << 8));
		}
		SetTouchedChip(VE_INFINITE);
		SetChooseArea(AREA_MAX);
	}
}

UIImplementDelegate(BetArea, OnChipClicked5, kWidget, u32State)
{
	if(u32State == BetAreaChip::EVENT_TOUCH)
	{
		SetTouchedChip(5);
	}
	else if(u32State == BetAreaChip::EVENT_RELEASE)
	{
		if(m_eAreaState < AREA_MAX)
		{
			VeUInt32 u32Event = BACC_EVENT_FLAG | (BACC_EVENT_CHIPS_MASK & 5);
			m_kEvent.Callback(*this, u32Event | (m_eAreaState << 8));
		}
		SetTouchedChip(VE_INFINITE);
		SetChooseArea(AREA_MAX);
	}
}

void BetArea::SetTouchedChip(VeUInt32 u32Index)
{
	if(u32Index < 6)
	{
		if(m_pkTouchedChip && m_apkAreaChips[u32Index] != m_pkTouchedChip)
		{
			m_pkTouchedChip->SetTouch(false);
		}
		m_pkTouchedChip = m_apkAreaChips[u32Index];
	}
	else if(m_pkTouchedChip)
	{
		m_pkTouchedChip->SetTouch(false);
		m_pkTouchedChip = NULL;
	}
}

void BetArea::SetPlayRes(bool bPlay)
{
	if(bPlay)
	{
		SetTouchedChip(VE_INFINITE);
		for(VeUInt32 i(0); i < 6; ++i)
		{
			m_apkAreaChips[i]->SetEnable(!bPlay);
		}
	}
}
