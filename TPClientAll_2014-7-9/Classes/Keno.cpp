#include "Keno.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "Knickknacks.h"
#include "Hall.h"
#include "SoundSystem.h"
#include "TexasPokerHallPlayerC.h"
#include "TexasPokerHallAgentC.h"
#include "NativeInterface.h"

USING_NS_CC;

KenoMatrix::KenoMatrix(cocos2d::CCSpriteBatchNode* pkRoot)
{
	m_kChoosed.Reserve(10);
	m_pkPressed = NULL;
	VeChar8 acBuffer[64];
	for(VeUInt32 i(0); i < 8; ++i)
	{
		for(VeUInt32 j(0); j < 10; ++j)
		{
			Lattice& kLattice = m_akMatrix[i][j];
			kLattice.m_eState = Lattice::STATE_NORMAL;

			CCPoint kPos(VeFloorf(UILayer::Width() * 0.5f - (j * 32.0f - 144) * g_f32ScaleHD * ui_main_scale),
				VeFloorf(((i * 31.0f + 8) * ui_main_scale + 80 * ui_height_scale) * g_f32ScaleHD));

			kLattice.m_apkStateSprite[0] = CCSprite::createWithSpriteFrameName("keno_btn_s_n_r.png");
			kLattice.m_apkStateSprite[0]->setScale(ui_main_scale);
			kLattice.m_apkStateSprite[0]->setPosition(kPos);
			pkRoot->addChild(kLattice.m_apkStateSprite[0], 3);

			kLattice.m_apkStateSprite[1] = CCSprite::createWithSpriteFrameName("keno_btn_s_p_r.png");
			kLattice.m_apkStateSprite[1]->setScale(ui_main_scale);
			kLattice.m_apkStateSprite[1]->setPosition(kPos);
			pkRoot->addChild(kLattice.m_apkStateSprite[1], 3);
			kLattice.m_apkStateSprite[1]->setVisible(false);

			kLattice.m_apkStateSprite[2] = CCSprite::createWithSpriteFrameName("keno_btn_s_n_b.png");
			kLattice.m_apkStateSprite[2]->setScale(ui_main_scale);
			kLattice.m_apkStateSprite[2]->setPosition(kPos);
			pkRoot->addChild(kLattice.m_apkStateSprite[2], 3);
			kLattice.m_apkStateSprite[2]->setVisible(false);

			kLattice.m_apkStateSprite[3] = CCSprite::createWithSpriteFrameName("keno_btn_s_p_b.png");
			kLattice.m_apkStateSprite[3]->setScale(ui_main_scale);
			kLattice.m_apkStateSprite[3]->setPosition(kPos);
			pkRoot->addChild(kLattice.m_apkStateSprite[3], 3);
			kLattice.m_apkStateSprite[3]->setVisible(false);

			kLattice.m_apkStateSprite[4] = CCSprite::createWithSpriteFrameName("keno_btn_s_d.png");
			kLattice.m_apkStateSprite[4]->setScale(ui_main_scale);
			kLattice.m_apkStateSprite[4]->setPosition(kPos);
			pkRoot->addChild(kLattice.m_apkStateSprite[4], 3);
			kLattice.m_apkStateSprite[4]->setVisible(false);

			VeUInt32 u32Number = 80 - i * 10 - j;
			VeUInt32 u32Unit = u32Number % 10;
			VeUInt32 u32Tens = u32Number / 10;

			if(u32Tens)
			{
				VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
				kLattice.m_apkNumber[0] = CCSprite::createWithSpriteFrameName(acBuffer);
				kLattice.m_apkNumber[0]->setScale(ui_main_scale);
				kLattice.m_apkNumber[0]->setPosition(ccp(VeFloorf(kPos.x + 5 * g_f32ScaleHD * ui_main_scale), kPos.y));
				pkRoot->addChild(kLattice.m_apkNumber[0], 3);
				VeSprintf(acBuffer, 64, "keno_%d.png", u32Tens);
				kLattice.m_apkNumber[1] = CCSprite::createWithSpriteFrameName(acBuffer);
				kLattice.m_apkNumber[1]->setScale(ui_main_scale);
				kLattice.m_apkNumber[1]->setPosition(ccp(VeFloorf(kPos.x - 5 * g_f32ScaleHD * ui_main_scale), kPos.y));
				pkRoot->addChild(kLattice.m_apkNumber[1], 3);
			}
			else
			{
				VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
				kLattice.m_apkNumber[0] = CCSprite::createWithSpriteFrameName(acBuffer);
				kLattice.m_apkNumber[0]->setScale(ui_main_scale);
				kLattice.m_apkNumber[0]->setPosition(kPos);
				pkRoot->addChild(kLattice.m_apkNumber[0], 3);
				kLattice.m_apkNumber[1] = NULL;
			}

			kLattice.m_pkParent = this;
		}
	}

	m_kRange.origin.x = UILayer::Width() * 0.5f - 160 * g_f32ScaleHD * ui_main_scale;
	m_kRange.origin.y = (80 * ui_height_scale - 8 * ui_main_scale) * g_f32ScaleHD;
	m_kRange.size.width = 320.0f * g_f32ScaleHD * ui_main_scale;
	m_kRange.size.height = 249.0f * g_f32ScaleHD * ui_main_scale;
}

UIWidget* KenoMatrix::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		VeUInt32 u32Col = 10 - (pkTouch->getLocation().x - m_kRange.origin.x) * 10 / m_kRange.size.width;
		VeUInt32 u32Row = (pkTouch->getLocation().y - m_kRange.origin.y) * 8 / m_kRange.size.height;
		Lattice* pkCurrent = &m_akMatrix[u32Row][u32Col];

		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
		case UILayer::TOUCH_MOVED:
			if(m_pkPressed && m_pkPressed != pkCurrent)
			{
				m_pkPressed->EventRelease();
			}
			m_pkPressed = pkCurrent;
			m_pkPressed->Press();
			break;
		case UILayer::TOUCH_ENDED:
			if(m_pkPressed)
			{
				m_pkPressed->Release();
				m_pkPressed = NULL;
			}
			break;
		case UILayer::TOUCH_CANCELLED:
			EventRelease();
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void KenoMatrix::EventRelease()
{
	if(m_pkPressed)
	{
		m_pkPressed->EventRelease();
		m_pkPressed = NULL;
	}
}

bool KenoMatrix::IsIn(VeFloat32 x, VeFloat32 y)
{
	return m_kRange.containsPoint(ccp(x,y));
}

void KenoMatrix::Update()
{
	m_kChoosed.Clear();
	for(VeInt32 i(7); i >= 0; --i)
	{
		for(VeInt32 j(9); j >= 0; --j)
		{
			Lattice& kLattice = m_akMatrix[i][j];
			if(kLattice.GetState() == Lattice::STATE_NORMAL_CHOOSED)
			{
				VeUInt32 u32Number = 80 - i * 10 - j;
				m_kChoosed.PushBack(u32Number);
			}
		}
	}
	VE_ASSERT(m_kChoosed.Size() <= 10);
	if(m_kChoosed.Size() == 10)
	{
		for(VeInt32 i(7); i >= 0; --i)
		{
			for(VeInt32 j(9); j >= 0; --j)
			{
				Lattice& kLattice = m_akMatrix[i][j];
				if(kLattice.GetState() != Lattice::STATE_NORMAL_CHOOSED)
				{
					kLattice.SetState(Lattice::STATE_DISABLED);
				}
			}
		}
	}
	else
	{
		for(VeInt32 i(7); i >= 0; --i)
		{
			for(VeInt32 j(9); j >= 0; --j)
			{
				Lattice& kLattice = m_akMatrix[i][j];
				if(kLattice.GetState() == Lattice::STATE_DISABLED)
				{
					kLattice.SetState(Lattice::STATE_NORMAL);
				}
			}
		}
	}
	m_kEvent.Callback(*this, EVENT_UPDATE);
}

void KenoMatrix::Clear()
{
	for(VeInt32 i(7); i >= 0; --i)
	{
		for(VeInt32 j(9); j >= 0; --j)
		{
			Lattice& kLattice = m_akMatrix[i][j];
			kLattice.SetState(Lattice::STATE_NORMAL);
		}
	}
	Update();
}

void KenoMatrix::SetChoose(const VeVector<VeUInt32>& kChoose)
{
	VE_ASSERT(kChoose.Size() <= 10);
	for(VeInt32 i(7); i >= 0; --i)
	{
		for(VeInt32 j(9); j >= 0; --j)
		{
			Lattice& kLattice = m_akMatrix[i][j];
			kLattice.SetState(Lattice::STATE_NORMAL);
		}
	}
	for(VeVector<VeUInt32>::const_iterator it = kChoose.Begin(); it != kChoose.End(); ++it)
	{
		VeUInt32 u32Index = *it;
		VE_ASSERT(u32Index > 0 && u32Index <= 80);
		u32Index = 80 - u32Index;
		VeUInt32 i = u32Index / 10;
		VeUInt32 j = u32Index % 10;
		m_akMatrix[i][j].SetState(Lattice::STATE_NORMAL_CHOOSED);
	}
	Update();
}

void KenoMatrix::SetChoose( const unsigned int* ui32Choose )
{
	m_kChoosed.Clear();
	for (int i(0);i<10;++i)
	{
		m_kChoosed.PushBack(ui32Choose[i]);
	}
}

const VeVector<VeUInt32>& KenoMatrix::GetChoose()
{
	return m_kChoosed;
}

void KenoMatrix::Lattice::SetState(State eState)
{
	if(m_eState != eState)
	{
		m_apkStateSprite[m_eState]->setVisible(false);
		m_eState = eState;
		m_apkStateSprite[m_eState]->setVisible(true);
		if(m_eState == STATE_DISABLED)
		{
			m_apkNumber[0]->setColor(ccc3(128,128,128));
			if(m_apkNumber[1])
				m_apkNumber[1]->setColor(ccc3(128,128,128));
		}
		else
		{
			m_apkNumber[0]->setColor(ccc3(255,255,255));
			if(m_apkNumber[1])
				m_apkNumber[1]->setColor(ccc3(255,255,255));
		}
	}
}

KenoMatrix::Lattice::State KenoMatrix::Lattice::GetState()
{
	return m_eState;
}

void KenoMatrix::Lattice::Press()
{
	if(GetState() == STATE_NORMAL)
	{
		SetState(STATE_PRESSED);
	}
	else if(GetState() == STATE_NORMAL_CHOOSED)
	{
		SetState(STATE_PRESSED_CHOOSED);
	}
}

void KenoMatrix::Lattice::Release()
{
	if(GetState() == STATE_PRESSED)
	{
		SetState(STATE_NORMAL_CHOOSED);
	}
	else if(GetState() == STATE_PRESSED_CHOOSED)
	{
		SetState(STATE_NORMAL);
	}
	m_pkParent->Update();
}

void KenoMatrix::Lattice::EventRelease()
{
	if(GetState() == STATE_PRESSED)
	{
		SetState(STATE_NORMAL);
	}
	else if(GetState() == STATE_PRESSED_CHOOSED)
	{
		SetState(STATE_NORMAL_CHOOSED);
	}
}

KenoCard::KenoCard()
{
	setScale(ui_main_scale);
	setPositionX(ui_w(0.5f));
	m_f32PosYTop = VeFloorf(210 * ui_main_scale + 120 * ui_height_scale) * g_f32ScaleHD;
	m_f32PosYBottom = VeFloorf(132 * ui_main_scale + 120 * ui_height_scale) * g_f32ScaleHD;
	setPositionY(m_f32PosYBottom);

	CCSpriteBatchNode* pkKenoRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("keno", false) + ".png");
	addChild(pkKenoRoot);

	CCSprite* pkTicketL = CCSprite::createWithSpriteFrameName("keno_ticket.png");
	pkTicketL->setAnchorPoint(ccp(1.0f, 42.0f / 196.0f));
	pkKenoRoot->addChild(pkTicketL);

	CCSprite* pkTicketR = CCSprite::createWithSpriteFrameName("keno_ticket.png");
	pkTicketR->setAnchorPoint(ccp(1.0f, 42.0f / 196.0f));
	pkTicketR->setScaleX(-1.0f);
	pkKenoRoot->addChild(pkTicketR);

	for(VeUInt32 i(0); i < 10; ++i)
	{
		m_af32NumberX[i] = VeFloorf((30.0f * i - 135.0f) * g_f32ScaleHD);
		m_apkNumber[i][0] = CCSprite::createWithSpriteFrameName("keno_0.png");
		m_apkNumber[i][0]->setColor(ccc3(0,0,0));
		m_apkNumber[i][0]->setPositionX(m_af32NumberX[i]);
		m_apkNumber[i][0]->setPositionY(VeFloorf(37 * g_f32ScaleHD));
		addChild(m_apkNumber[i][0]);
		m_apkNumber[i][1] = CCSprite::createWithSpriteFrameName("keno_0.png");
		m_apkNumber[i][1]->setColor(ccc3(0,0,0));
		m_apkNumber[i][1]->setPositionX(m_af32NumberX[i]);
		m_apkNumber[i][1]->setPositionY(VeFloorf(37 * g_f32ScaleHD));
		addChild(m_apkNumber[i][1]);
	}

	m_kMaxSize.width = 314.0f * g_f32ScaleHD;
	m_kMaxSize.height = 154.0f * g_f32ScaleHD;
	setContentSize(m_kMaxSize);
}

void KenoCard::visit()
{
	if (!m_bVisible)
	{
		return;
	}
	
	glEnable(GL_SCISSOR_TEST);
	CCRect kRect = boundingBox();
	kRect.origin.x -= kRect.size.width * 0.5f;
	kRect.origin.y -= kRect.size.height * 0.5f;
	CCEGLView::sharedOpenGLView()->setScissorInPoints(kRect.origin.x, kRect.origin.y, kRect.size.width, kRect.size.height);

	VeVector<UIWidget*> kHideList;
	m_kChildren.BeginIterator();
	while(!m_kChildren.IsEnd())
	{
		UIWidget* pkWidget = m_kChildren.GetIterNode()->m_tContent;
		m_kChildren.Next();
		VE_ASSERT(pkWidget);
		if(pkWidget->isVisible() && (!pkWidget->IntersectWith(kRect)))
		{
			pkWidget->setVisible(false);
			kHideList.PushBack(pkWidget);
		}
	}

	CCNode::visit();

	for(VeVector<UIWidget*>::iterator it = kHideList.Begin(); it != kHideList.End(); ++it)
	{
		(*it)->setVisible(true);
	}

	glDisable(GL_SCISSOR_TEST);
	
}

void KenoCard::Show()
{
	SetVisible(true);
	setPositionY(m_f32PosYTop);
	setContentSize(CCSize(m_kMaxSize.width, 0));
	m_f32Delay = 0.0f;
	schedule(schedule_selector(KenoCard::Update), 0.0f);
}

void KenoCard::Update(VeFloat32 f32Delta)
{
	m_f32Delay += f32Delta;
	if(m_f32Delay >= KENO_TICKET_SHOW)
	{
		m_f32Delay = KENO_TICKET_SHOW;
		setPositionY(m_f32PosYBottom);
		unschedule(schedule_selector(KenoCard::Update));
		g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
	}
	else
	{
		VeFloat32 f32Rate = VeSinf(m_f32Delay * VE_MATH_PI_2_F / KENO_TICKET_SHOW);
		setPositionY(VeLerp(f32Rate, m_f32PosYTop, m_f32PosYBottom));
		setContentSize(CCSize(m_kMaxSize.width, VeLerp(f32Rate, 0, m_kMaxSize.height)));
	}
	
}

void KenoCard::SetChoose(const VeVector<VeUInt32>& kChoose)
{
	VeChar8 acBuffer[64];
	VE_ASSERT(kChoose.Size() == 10);
	for(VeUInt32 i(0); i < 10; ++i)
	{
		VeUInt32 u32Number = kChoose[i];
		VE_ASSERT(u32Number > 0 && u32Number <= 80);
		VeUInt32 u32Unit = u32Number % 10;
		VeUInt32 u32Tens = u32Number / 10;

		if(u32Tens)
		{
			VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
			m_apkNumber[i][0]->initWithSpriteFrameName(acBuffer);
			m_apkNumber[i][0]->setPositionX(m_af32NumberX[i] + 5 * g_f32ScaleHD);
			m_apkNumber[i][0]->setColor(ccc3(0,0,0));
			VeSprintf(acBuffer, 64, "keno_%d.png", u32Tens);
			m_apkNumber[i][1]->initWithSpriteFrameName(acBuffer);
			m_apkNumber[i][1]->setPositionX(m_af32NumberX[i] - 5 * g_f32ScaleHD);
			m_apkNumber[i][1]->setColor(ccc3(0,0,0));
			m_apkNumber[i][0]->setVisible(true);
			m_apkNumber[i][1]->setVisible(true);
		}
		else
		{
			VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
			m_apkNumber[i][0]->initWithSpriteFrameName(acBuffer);
			m_apkNumber[i][0]->setPositionX(m_af32NumberX[i]);
			m_apkNumber[i][0]->setColor(ccc3(0,0,0));
			m_apkNumber[i][0]->setVisible(true);
			m_apkNumber[i][1]->setVisible(false);
		}
	}
}

Keno::Keno() : UILayerExt<Keno>("keno")
{
	UIInitDelegate(Keno, OnMatrixUpdate);
	UIInitDelegate(Keno, OnClear);
	UIInitDelegate(Keno, OnMax);
	UIInitDelegate(Keno, OnDec);
	UIInitDelegate(Keno, OnInc);
	UIInitDelegate(Keno, OnAuto);
	UIInitDelegate(Keno, OnBuy);
	UIInitDelegate(Keno, OnBack);
	UIInitDelegate(Keno, OnPurchase);
	m_BetNum = 0;
}

Keno::~Keno()
{
	term();
}

bool Keno::init()
{
	CCSize kSize;

	CCSpriteBatchNode* pkBackRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	addChild(pkBackRoot);

	CCSpriteBatchNode* pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("slot", false) + ".png");
	addChild(pkRoot);

	CCSpriteBatchNode* pkKenoRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("keno", false) + ".png");
	addChild(pkKenoRoot);

	for(VeUInt32 j(0); j < 4; ++j)
	{
		CCSprite* pkBackground = CCSprite::createWithSpriteFrameName("bg_1.png");
		pkBackground->setAnchorPoint(ccp(1.0f, 0.0f));
		pkBackground->setPosition(ui_ccp(0.5f, 0.5f));
		pkBackground->setBlendFunc(kCCBlendFuncDisable);
		pkBackRoot->addChild(pkBackground, 0);

		CCSprite* pkSlotBg0 = CCSprite::createWithSpriteFrameName("slot_bg_0.png");
		pkSlotBg0->setAnchorPoint(ccp(1.0f, 0.0f));
		pkSlotBg0->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 116 * ui_main_scale * g_f32ScaleHD),
			VeFloorf((84 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
		pkRoot->addChild(pkSlotBg0);

		CCSprite* pkSlotBg1 = CCSprite::createWithSpriteFrameName("slot_bg_1.png");
		pkSlotBg1->setAnchorPoint(ccp(1.0f, 0.0f));
		pkSlotBg1->setPosition(ccp(ui_w(0.5f),
			VeFloorf((16 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
		pkRoot->addChild(pkSlotBg1, 1);

		switch(j)
		{
		case 1:
			pkBackground->setScaleX(-ui_main_scale);
			pkBackground->setScaleY(ui_main_scale);
			pkSlotBg0->setScaleX(-ui_main_scale);
			pkSlotBg0->setScaleY(ui_main_scale);
			pkSlotBg1->setScaleX(-ui_main_scale);
			pkSlotBg1->setScaleY(ui_main_scale);
			break;
		case 2:
			pkBackground->setScaleX(ui_main_scale);
			pkBackground->setScaleY(-ui_main_scale);
			pkSlotBg0->setScaleX(ui_main_scale);
			pkSlotBg0->setScaleY(-ui_main_scale);
			pkSlotBg1->setScaleX(ui_main_scale);
			pkSlotBg1->setScaleY(-ui_main_scale);
			break;
		case 3:
			pkBackground->setScaleX(-ui_main_scale);
			pkBackground->setScaleY(-ui_main_scale);
			pkSlotBg0->setScaleX(-ui_main_scale);
			pkSlotBg0->setScaleY(-ui_main_scale);
			pkSlotBg1->setScaleX(-ui_main_scale);
			pkSlotBg1->setScaleY(-ui_main_scale);
			break;
		default:
			pkBackground->setScale(ui_main_scale);
			pkSlotBg0->setScale(ui_main_scale);
			pkSlotBg1->setScale(ui_main_scale);
			break;
		}
	}

	CCLabelTTF* pkHits = CCLabelTTF::create("Hits", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	kSize = pkHits->getContentSize();
	pkHits->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkHits->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 2 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((248 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	addChild(pkHits);

	CCSprite* pkHitsBar = CCSprite::createWithSpriteFrameName("point.png");
	pkHitsBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 2 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((242 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	pkHitsBar->setScaleX(VeFloorf(30.0f * ui_main_scale * g_f32ScaleHD));
	pkHitsBar->setScaleY(VeFloorf(1.5f * ui_main_scale * g_f32ScaleHD));
	addChild(pkHitsBar);

	for(VeUInt32 i(0); i < 7; ++i)
	{
		CCLabelTTF* pkHitNum = CCLabelTTF::create(NTL(i + 4, 5), TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
		kSize = pkHitNum->getContentSize();
		pkHitNum->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkHitNum->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 2 * ui_main_scale * g_f32ScaleHD),
			VeFloorf(((230 - 11 * i) * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
		addChild(pkHitNum);
	}

	CCLabelTTF* pkWin = CCLabelTTF::create("Win", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	kSize = pkWin->getContentSize();
	pkWin->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkWin->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 42 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((248 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	addChild(pkWin);

	CCSprite* pkWinBar = CCSprite::createWithSpriteFrameName("point.png");
	pkWinBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 42 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((242 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	pkWinBar->setScaleX(VeFloorf(30.0f * ui_main_scale * g_f32ScaleHD));
	pkWinBar->setScaleY(VeFloorf(1.5f * ui_main_scale * g_f32ScaleHD));
	addChild(pkWinBar);

	VeUInt32 au32WinRate[7] =
	{
		2,5,12,50,300,2500,20000
	};

	for(VeUInt32 i(0); i < 7; ++i)
	{
		CCLabelTTF* pkWinNum = CCLabelTTF::create(NTL(au32WinRate[i], 8), TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
		kSize = pkWinNum->getContentSize();
		pkWinNum->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkWinNum->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 56 * ui_main_scale * g_f32ScaleHD),
			VeFloorf(((230 - 11 * i) * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
		addChild(pkWinNum);
	}

	CCSprite* pkCreditFrame = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkCreditFrame->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 120 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((162 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	pkCreditFrame->setScale(ui_main_scale * 0.9f);
	addChild(pkCreditFrame);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chips.png");
	pkChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 80 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((162 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	pkChips->setScale(ui_main_scale * 0.9f);
	addChild(pkChips);

	m_pkCredit = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkCredit->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 152 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((162 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	addChild(m_pkCredit);
		
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

	for(VeUInt32 i(0); i < 5; ++i)
	{
		CCSprite* pkBottom = CCSprite::createWithSpriteFrameName("keno_bottom.png");
		pkBottom->setScale(ui_main_scale);
		pkBottom->setAnchorPoint(ccp(0.5f, 0.0f));
		pkBottom->setPosition(ccp(UILayer::Width() * 0.5f + (72.0f * i - 144) * g_f32ScaleHD * ui_main_scale, 0));
		pkKenoRoot->addChild(pkBottom);
	}

	KenoMatrix* pkMatrix = VE_NEW KenoMatrix(pkKenoRoot);
	AddWidget("matrix", pkMatrix);
	UILinkDelegate(pkMatrix, OnMatrixUpdate);

	NoticeBar* pkNoticeBar = VE_NEW NoticeBar;
	pkNoticeBar->setPositionY(VeFloorf(UILayer::Height() * 0.5f + 230.0f * g_f32ScaleHD * ui_height_scale));
	AddWidget("notice_bar", pkNoticeBar, 0);

	CCSprite* pkBarL = CCSprite::createWithSpriteFrameName("keno_bar.png");
	pkBarL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarL->setScale(ui_main_scale);
	pkBarL->setPosition(ccp(ui_w(0.5f), VeFloorf(210 * ui_main_scale + 120 * ui_height_scale) * g_f32ScaleHD));
	pkKenoRoot->addChild(pkBarL, 3);

	CCSprite* pkBarR = CCSprite::createWithSpriteFrameName("keno_bar.png");
	pkBarR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarR->setScaleX(-ui_main_scale);
	pkBarR->setScaleY(ui_main_scale);
	pkBarR->setPosition(ccp(ui_w(0.5f), VeFloorf(210 * ui_main_scale + 120 * ui_height_scale) * g_f32ScaleHD));
	pkKenoRoot->addChild(pkBarR, 3);

	CCSprite* pkKenoNumber = CCSprite::createWithSpriteFrameName("keno_number.png");
	pkKenoNumber->setScale(ui_main_scale);
	pkKenoNumber->setPosition(ccp(ui_w(0.5f), VeFloorf(186 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD));
	addChild(pkKenoNumber, 3);

	CCSprite* pkBetFrameL = CCSprite::createWithSpriteFrameName("keno_bet_frame.png");
	pkBetFrameL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBetFrameL->setScale(ui_main_scale);
	pkBetFrameL->setPosition(ccp(ui_w(0.5f), VeFloorf(23 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBetFrameL);

	CCSprite* pkBetFrameR = CCSprite::createWithSpriteFrameName("keno_bet_frame.png");
	pkBetFrameR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBetFrameR->setScaleX(-ui_main_scale);
	pkBetFrameR->setScaleY(ui_main_scale);
	pkBetFrameR->setPosition(ccp(ui_w(0.5f), VeFloorf(23 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBetFrameR);

	CCSprite* pkBet = CCSprite::createWithSpriteFrameName("keno_bet.png");
	pkBet->setScale(ui_main_scale);
	pkBet->setPosition(ccp(ui_w(0.5f), VeFloorf(23 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBet);

	UIButton* pkMax = VE_NEW UIButton("keno_btn_n_y.png", "keno_btn_p_y.png", "keno_btn_d.png");
	pkMax->setScale(ui_main_scale);
	pkMax->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 92 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(28 * ui_main_scale * g_f32ScaleHD)));
	AddWidget("max_btn", pkMax);
	m_pkMaxText = CCSprite::createWithSpriteFrameName("keno_max.png");
	m_pkMaxText->setPositionY(1.0f * g_f32ScaleHD);
	pkMax->addChild(m_pkMaxText, 5);
	UILinkDelegate(pkMax, OnMax);

	UIButton* pkClear = VE_NEW UIButton("keno_btn_n_r.png", "keno_btn_p_r.png", "keno_btn_d.png");
	pkClear->setScale(ui_main_scale);
	pkClear->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 136 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(28 * ui_main_scale * g_f32ScaleHD)));
	AddWidget("clear_btn", pkClear);
	m_pkClearText = CCSprite::createWithSpriteFrameName("keno_clear.png");
	m_pkClearText->setPositionY(1.0f * g_f32ScaleHD);
	pkClear->addChild(m_pkClearText, 5);
	UILinkDelegate(pkClear, OnClear);

	UIButton* pkBuy = VE_NEW UIButton("keno_btn_n_b.png", "keno_btn_p_b.png", "keno_btn_d.png");
	pkBuy->setScale(ui_main_scale);
	pkBuy->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 136 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(28 * ui_main_scale * g_f32ScaleHD)));
	AddWidget("buy_btn", pkBuy);
	m_pkBuyText = CCSprite::createWithSpriteFrameName("keno_buy.png");
	m_pkBuyText->setPositionY(1.0f * g_f32ScaleHD);
	pkBuy->addChild(m_pkBuyText, 5);
	UILinkDelegate(pkBuy, OnBuy);

	UIButton* pkAuto = VE_NEW UIButton("keno_btn_n_y.png", "keno_btn_p_y.png", "keno_btn_d.png");
	pkAuto->setScale(ui_main_scale);
	pkAuto->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 92 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(28 * ui_main_scale * g_f32ScaleHD)));
	AddWidget("auto_btn", pkAuto);
	m_pkAutoText = CCSprite::createWithSpriteFrameName("keno_auto.png");
	m_pkAutoText->setPositionY(1.0f * g_f32ScaleHD);
	pkAuto->addChild(m_pkAutoText, 5);
	UILinkDelegate(pkAuto, OnAuto);

	UIButton* pkDecBet = VE_NEW UIButton("keno_btn_bet_n.png", "keno_btn_bet_p.png", "keno_btn_bet_d.png");
	pkDecBet->setScale(ui_main_scale);
	pkDecBet->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 44 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(23 * ui_main_scale * g_f32ScaleHD)));
	AddWidget("dec_bet", pkDecBet);
	CCSprite* pkDec = CCSprite::createWithSpriteFrameName("-.png");
	pkDecBet->addChild(pkDec, 5);
	UILinkDelegate(pkDecBet, OnDec);

	UIButton* pkIncBet = VE_NEW UIButton("keno_btn_bet_n.png", "keno_btn_bet_p.png", "keno_btn_bet_d.png");
	pkIncBet->setScale(ui_main_scale);
	pkIncBet->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 44 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(23 * ui_main_scale * g_f32ScaleHD)));
	pkIncBet->Normal()->setScaleX(-1);
	pkIncBet->Pressed()->setScaleX(-1);
	pkIncBet->Disabled()->setScaleX(-1);
	AddWidget("inc_bet", pkIncBet);
	CCSprite* pkInc = CCSprite::createWithSpriteFrameName("+.png");
	pkIncBet->addChild(pkInc, 5);
	UILinkDelegate(pkIncBet, OnInc);

	CCSprite* pkBetNumberFrame = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkBetNumberFrame->setScale(ui_main_scale);
	pkBetNumberFrame->setPosition(ccp(ui_w(0.5f), VeFloorf(49 * ui_main_scale * g_f32ScaleHD)));
	addChild(pkBetNumberFrame);

	m_pkBetNumber = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	m_pkBetNumber->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 36 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(49 * ui_main_scale * g_f32ScaleHD)));
	addChild(m_pkBetNumber);

	CCSprite* pkGirl = CCSprite::createWithSpriteFrameName("table_girl_0.png");
	pkGirl->setScale(ui_main_scale);
	pkGirl->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 108 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((104 * ui_main_scale + 300 * ui_height_scale) * g_f32ScaleHD)));
	pkBackRoot->addChild(pkGirl);

	m_pkGlobal = CCSprite::createWithSpriteFrameName("keno_global_0.png");
	m_pkGlobal->setScale(ui_main_scale);
	m_pkGlobal->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 70 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((136 * ui_main_scale + 300 * ui_height_scale) * g_f32ScaleHD)));
	addChild(m_pkGlobal);

	CCSprite* pkIssue = CCSprite::createWithSpriteFrameName("keno_issue.png");
	pkIssue->setScale(ui_main_scale);
	pkIssue->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 110 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((116 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	addChild(pkIssue);

	m_pkIssueNumber = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkIssueNumber->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 102 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((110 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	addChild(m_pkIssueNumber);

	KenoCard* pkCard = VE_NEW KenoCard;
	AddWidget("card", pkCard);

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

	UIButton* pkPurchase = VE_NEW UIButton("btn_shop_n.png", "btn_shop_p.png");
	pkPurchase->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 110 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((174 * ui_main_scale + 260 * ui_height_scale - ((g_f32ScaleHD == 2.0f) ? 0.5f * ui_main_scale : 0.0f)) * g_f32ScaleHD)));
	pkPurchase->setScale(ui_main_scale);
	CCSprite* pkPurchaseText = CCSprite::createWithSpriteFrameName("mini_buy_chips.png");
	pkPurchaseText->setPosition(ccp(8 * g_f32ScaleHD, 6.0f * g_f32ScaleHD));
	pkPurchase->addChild(pkPurchaseText, 5);
	AddWidget("purchase", pkPurchase);
	UILinkDelegate(pkPurchase, OnPurchase);

	CCSprite* pkKenoCountDown = CCSprite::createWithSpriteFrameName("keno_countdown.png");
	pkKenoCountDown->setScale(ui_main_scale);
	pkKenoCountDown->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 117 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((196 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	addChild(pkKenoCountDown);

	m_pkDrawTime = CCLabelTTF::create("00:00", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	m_pkDrawTime->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 117 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((182 * ui_main_scale + 200 * ui_height_scale) * g_f32ScaleHD)));
	addChild(m_pkDrawTime);

	m_u32Bet = VE_INFINITE;
	SetBet(0);

	return UILayer::init();
}

void Keno::term()
{
	UILayer::term();
}

void Keno::OnActive(const UIUserDataPtr& spData)
{
	VeSrand(g_pTime->GetCurrentCount());
	UpdateKenoResult(false);
	schedule(schedule_selector(Keno::Update), 0.0f);
	NativeOnEventBegin("keno");
}

void Keno::OnDeactive(const UIUserDataPtr& spData)
{
	unscheduleAllSelectors();
	NativeOnEventEnd("keno");
}

bool Keno::OnBack()
{
	return true;
}

void Keno::SetBetNumber(VeUInt32 u32Number)
{
	m_pkBetNumber->setString(NTL(u32Number, 8));
	CCSize kSize = m_pkBetNumber->getContentSize();
	m_pkBetNumber->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_BetNum = u32Number;
}

void Keno::SetIssueNumber(VeUInt32 u32Number)
{
	m_pkIssueNumber->setString(NTL(u32Number, 8));
	CCSize kSize = m_pkIssueNumber->getContentSize();
	m_pkIssueNumber->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Keno::SetBet(VeUInt32 u32Bet)
{
	u32Bet = VE_MIN(u32Bet, KENO_BET_MAX - 1);
	if(m_u32Bet != u32Bet)
	{
		m_u32Bet = u32Bet;
		Update_State();
	}
}

VeUInt32 Keno::GetBet()
{
	return m_u32Bet;
}

void Keno::Update_State()
{
	WIDGET(UIButton, max_btn);
	max_btn.SetEnable(true);
	WIDGET(UIButton, clear_btn);
	clear_btn.SetEnable(true);
	WIDGET(UIButton, auto_btn);
	auto_btn.SetEnable(true);
	WIDGET(KenoMatrix, matrix);
	matrix.SetEnable(true);
	WIDGET(UIButton, buy_btn);
	const VeVector<VeUInt32>& kChoosed = matrix.GetChoose();
	buy_btn.SetEnable(kChoosed.Size() == 10);
	WIDGET(UIButton, inc_bet);
	WIDGET(UIButton, dec_bet);
	inc_bet.SetEnable(m_u32Bet != (KENO_BET_MAX - 1));
	dec_bet.SetEnable(m_u32Bet != 0);
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	SetBetNumber((m_u32Bet + 1) * pkPlayer->GetSingleBet());
}

void Keno::Disable()
{
	WIDGET(UIButton, max_btn);
	max_btn.SetEnable(false);
	WIDGET(UIButton, clear_btn);
	clear_btn.SetEnable(false);
	WIDGET(UIButton, buy_btn);
	buy_btn.SetEnable(false);
	WIDGET(UIButton, auto_btn);
	auto_btn.SetEnable(false);
	WIDGET(UIButton, dec_bet);
	dec_bet.SetEnable(false);
	WIDGET(UIButton, inc_bet);
	inc_bet.SetEnable(false);
	WIDGET(KenoMatrix, matrix);
	matrix.SetEnable(false);
}

void Keno::SetChips(VeUInt32 u32Credit)
{
	CCSize kSize;
	m_pkCredit->setString(NTL(u32Credit, 8));
	kSize = m_pkCredit->getContentSize();
	m_pkCredit->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Keno::UpdateDrawTime()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	SetIssueNumber(pkPlayer->GetKenoIssue());
	VeUInt64 u64DrawTime = pkPlayer->GetKenoDrawTime();
	VeUInt64 u64ServerTime = g_pkGame->GetServerTime();
	VeUInt32 u32Time = u64DrawTime > u64ServerTime ? VeUInt32(u64DrawTime - u64ServerTime) : 0;
	if(!u32Time) pkPlayer->S_ReqKenoUpdate();
	VeChar8 acBuffer[64];
	VeChar8* pcPointer = acBuffer;
	u32Time /= 100;
	VeUInt32 u32Second = u32Time % 60;
	VeUInt32 u32Minute = u32Time / 60;
	if(u32Minute > 9)
		pcPointer += VeSprintf(pcPointer, 8, "%d:", u32Minute);
	else
		pcPointer += VeSprintf(pcPointer, 8, "0%d:", u32Minute);
	if(u32Second > 9)
		pcPointer += VeSprintf(pcPointer, 8, "%d", u32Second);
	else
		pcPointer += VeSprintf(pcPointer, 8, "0%d", u32Second);
	m_pkDrawTime->setString(acBuffer);
	const CCSize& kSize = m_pkDrawTime->getContentSize();
	m_pkDrawTime->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

UIImplementDelegate(Keno, OnMatrixUpdate, kWidget, u32State)
{
	if(u32State == KenoMatrix::EVENT_UPDATE)
	{
		Update_State();
	}
}

UIImplementDelegate(Keno, OnClear, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		WIDGET(KenoMatrix, matrix);
		matrix.Clear();
		Update_State();
	}
}

UIImplementDelegate(Keno, OnMax, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		SetBet(KENO_BET_MAX - 1);
	}
}

UIImplementDelegate(Keno, OnDec, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		SetBet(m_u32Bet - 1);
	}
}

UIImplementDelegate(Keno, OnInc, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		SetBet(m_u32Bet + 1);
	}
}

UIImplementDelegate(Keno, OnAuto, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		WIDGET(KenoMatrix, matrix);
		VeVector<VeUInt32> kChoose;
		while(kChoose.Size() < 10)
		{
			VeUInt32 u32Index = VeRand() % 80 + 1;
			bool bReduplicate = false;
			for(VeVector<VeUInt32>::iterator it = kChoose.Begin(); it != kChoose.End(); ++it)
			{
				if((*it) == u32Index)
				{
					bReduplicate = true;
					break;
				}
			}
			if(!bReduplicate)
			{
				kChoose.PushBack(u32Index);
			}
		}
		matrix.SetChoose(kChoose);
		Update_State();
	}
}

UIImplementDelegate(Keno, OnBuy, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;

		if(pkPlayer->GetChips() >= m_BetNum)
		{
			WIDGET(KenoMatrix, matrix);
			const VeVector<VeUInt32>& kChoose = matrix.GetChoose();
			VeVector<VeUInt8> kChooseSend;
			for(VeUInt32 i(0); i < kChoose.Size(); ++i)
			{
				kChooseSend.PushBack(kChoose[i]);
			}
			pkPlayer->S_ReqBuyKeno(m_u32Bet, kChooseSend);
			Disable();
			VeVector< VePair<VeStringA,VeStringA> > kEventValue;
			kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet", VeStringA::From(VeUInt32((m_u32Bet+1) * pkPlayer->GetSingleBet()))));
			NativeOnEvent("ReqBuyKeno", kEventValue);
		}
		else
		{
			g_pkGame->Toast(TT("KenoFailed"));
		}
	}
}

UIImplementDelegate(Keno, OnBack, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Keno, OnPurchase, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("shop");
	}
}

void Keno::OnBuySucc()
{
	g_pkGame->Toast(TT("KenoResultSucc"));
}

void Keno::OnBuyFailed()
{
	g_pkGame->Toast(TT("KenoResultFailed"));
	Update_State();
}

void Keno::UpdateKenoResult(bool bShow)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	SetChips(pkPlayer->GetChips());
	UpdateDrawTime();
	WIDGET(KenoMatrix, matrix);
	WIDGET(KenoCard, card);
	VeChar8 acBuffer[128];
	VeStrcpy(acBuffer, 128, pkPlayer->GetKenoResult());
	VeChar8* pcContext;
	VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);
	if(pcTemp && (!VeStrcmp(pcTemp, "buy")))
	{
		VeUInt32 u32BetChips(0);
		VeVector<VeUInt32> kChoose;
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		VE_ASSERT(pcTemp);
		VeUInt32 u32IssueNumber = VeAtoi(pcTemp);
		if(u32IssueNumber != pkPlayer->GetKenoIssue())
		{
			pkPlayer->S_ReqKenoUpdate();
			pkPlayer->S_ReqKenoDraw();
		}
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		while(pcTemp)
		{
			if(pcTemp && VeStrcmp(pcTemp, "bet"))
			{
				kChoose.PushBack(VeAtoi(pcTemp));
			}
			else
			{
				break;
			}
			pcTemp = VeStrtok(NULL, ",", &pcContext);
		}
		if(pcTemp && (!VeStrcmp(pcTemp, "bet")))
		{
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			u32BetChips = VeAtoi(pcTemp);
		}
		VE_ASSERT(kChoose.Size() == 10);
		matrix.SetChoose(kChoose);
		card.SetChoose(kChoose);
		if(bShow)
		{
			card.Show();
		}
		else
		{
			card.SetVisible(true);
		}
		Disable();
		if(u32BetChips)
		{
			SetBetNumber(u32BetChips);
		}
	}
	else if(pcTemp && (!VeStrcmp(pcTemp, "res")))
	{
		matrix.Clear();
		card.SetVisible(false);
		Disable();
		g_pkGame->ShowKenoResult();
	}
	else
	{
		matrix.Clear();
		card.SetVisible(false);
		Update_State();
	}
}

void Keno::Update(VeFloat32 f32Delta)
{
	UpdateDrawTime();
}

void Keno::OnForceUpdate()
{
	UpdateKenoResult(true);
}

void Keno::OnPushNotice(const VeChar8* pcNotice)
{
	WIDGET(NoticeBar, notice_bar);
	notice_bar.Notice(pcNotice, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale));
}

KenoBall::KenoBall()
{
	m_pkRoot = CCNode::create();
	addChild(m_pkRoot);

	m_pkBall = CCSprite::createWithSpriteFrameName("keno_ball.png");
	m_pkRoot->addChild(m_pkBall);

	m_apkNumber[0] = CCSprite::createWithSpriteFrameName("keno_0.png");
	m_apkNumber[0]->setColor(ccc3(0,0,0));
	m_apkNumber[0]->setScale(0.7f);
	//m_apkNumber[0]->setPositionX(4.0f * g_f32ScaleHD);
	m_pkRoot->addChild(m_apkNumber[0]);

	m_apkNumber[1] = CCSprite::createWithSpriteFrameName("keno_0.png");
	m_apkNumber[1]->setColor(ccc3(0,0,0));
	m_apkNumber[1]->setScale(0.7f);
	//m_apkNumber[1]->setPositionX(-4.0f * g_f32ScaleHD);
	m_pkRoot->addChild(m_apkNumber[1]);
}

void KenoBall::SetNumber(VeUInt32 u32Number)
{
	VeChar8 acBuffer[64];
	VE_ASSERT(u32Number > 0 && u32Number <= 80);
	VeUInt32 u32Unit = u32Number % 10;
	VeUInt32 u32Tens = u32Number / 10;
	if(u32Tens)
	{
		VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
		m_apkNumber[0]->initWithSpriteFrameName(acBuffer);
		m_apkNumber[0]->setPositionX(4.0f * g_f32ScaleHD);
		m_apkNumber[0]->setColor(ccc3(0,0,0));
		VeSprintf(acBuffer, 64, "keno_%d.png", u32Tens);
		m_apkNumber[1]->initWithSpriteFrameName(acBuffer);
		m_apkNumber[1]->setPositionX(-4.0f * g_f32ScaleHD);
		m_apkNumber[1]->setColor(ccc3(0,0,0));
		m_apkNumber[1]->setVisible(true);
	}
	else
	{
		VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
		m_apkNumber[0]->initWithSpriteFrameName(acBuffer);
		m_apkNumber[0]->setPositionX(0);
		m_apkNumber[0]->setColor(ccc3(0,0,0));
		m_apkNumber[1]->setVisible(false);
	}
}

void KenoBall::Down()
{
	scheduleOnce(schedule_selector(KenoBall::DownSchedule), 1.2f);
	m_pkRoot->setPositionY(30.0f * g_f32ScaleHD);
	m_f32PassTime = 0.0f;
	SetVisible(true);	
	m_pkBall->setOpacity(0);
	m_apkNumber[0]->setOpacity(0);
	m_apkNumber[1]->setOpacity(0);
	m_pkBall->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeIn::create(0.2f), NULL));
	m_apkNumber[0]->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeIn::create(0.2f), NULL));
	m_apkNumber[1]->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeIn::create(0.2f), NULL));
}

void KenoBall::DownSchedule(VeFloat32 f32Delta)
{
	schedule(schedule_selector(KenoBall::Update), 0);
}

void KenoBall::Update(VeFloat32 f32Delta)
{
	m_f32PassTime += f32Delta;
	if(m_f32PassTime >= KENO_BALL_FALL)
	{
		m_pkRoot->setPositionY(0);
		unschedule(schedule_selector(KenoBall::Update));
		m_f32PassTime = KENO_BALL_FALL;
	}
	else
	{
		VeFloat32 f32Rate = (m_f32PassTime * m_f32PassTime) / (KENO_BALL_FALL * KENO_BALL_FALL);
		m_pkRoot->setPositionY(30.0f * g_f32ScaleHD * (1.0f - f32Rate));
	}
}

VeImplementRTTI(RunKeno::Callback, UIUserData);

RunKeno::RunKeno() : UILayerModalExt<RunKeno>("run_keno")
{
	m_bPlaying = false;
}

RunKeno::~RunKeno()
{
	term();
}

bool RunKeno::init()
{
	m_pkRoot = VE_NEW UIWidget;
	AddWidget("root", m_pkRoot);

	CCSpriteBatchNode* pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("slot", false) + ".png");
	m_pkRoot->addChild(pkRoot);

	CCSpriteBatchNode* pkBackRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	m_pkRoot->addChild(pkBackRoot);

	CCSpriteBatchNode* pkKenoRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("keno", false) + ".png");
	m_pkRoot->addChild(pkKenoRoot);	

	CCSprite* pkBgL = CCSprite::createWithSpriteFrameName("slot_bg_1.png");
	pkBgL->setAnchorPoint(ccp(1.0f, 0.0f));
	pkBgL->setPosition(ui_ccp(0.5f, 1.0f));
	pkRoot->addChild(pkBgL, 1);
	pkBgL->setScaleX(ui_main_scale);
	pkBgL->setScaleY(-ui_main_scale);
	CCSprite* pkBgR = CCSprite::createWithSpriteFrameName("slot_bg_1.png");
	pkBgR->setAnchorPoint(ccp(1.0f, 0.0f));
	pkBgR->setPosition(ui_ccp(0.5f, 1.0f));
	pkRoot->addChild(pkBgR, 1);
	pkBgR->setScaleX(-ui_main_scale);
	pkBgR->setScaleY(-ui_main_scale);

	CCSprite* pkGirl = CCSprite::createWithSpriteFrameName("table_girl_0.png");
	pkGirl->setScale(ui_main_scale);
	pkGirl->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 108 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() - 76 * ui_main_scale * g_f32ScaleHD)));
	pkBackRoot->addChild(pkGirl);

	m_pkGlobal = CCSprite::createWithSpriteFrameName("keno_global_0.png");
	m_pkGlobal->setScale(ui_main_scale);
	m_pkGlobal->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 70 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() - 44 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(m_pkGlobal);

	CCSprite* pkIssue = CCSprite::createWithSpriteFrameName("keno_issue.png");
	pkIssue->setScale(ui_main_scale);
	pkIssue->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 20 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() - 20 * ui_main_scale * g_f32ScaleHD)));
	m_pkRoot->addChild(pkIssue);

	m_pkIssueNumber = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkIssueNumber->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 28 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() - 26 * ui_main_scale * g_f32ScaleHD)));
	m_pkRoot->addChild(m_pkIssueNumber);

	CCSprite* pkKenoNumber = CCSprite::createWithSpriteFrameName("keno_number.png");
	pkKenoNumber->setScale(ui_main_scale);
	pkKenoNumber->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - 134 * ui_main_scale * g_f32ScaleHD)));
	m_pkRoot->addChild(pkKenoNumber);

	CCSprite* pkBarL = CCSprite::createWithSpriteFrameName("keno_bar.png");
	pkBarL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarL->setScale(ui_main_scale);
	pkBarL->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - 150 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBarL);

	CCSprite* pkBarR = CCSprite::createWithSpriteFrameName("keno_bar.png");
	pkBarR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarR->setScaleX(-ui_main_scale);
	pkBarR->setScaleY(ui_main_scale);
	pkBarR->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - 150 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkBarR);

	CCSprite* pkTicketL = CCSprite::createWithSpriteFrameName("keno_ticket.png");
	pkTicketL->setAnchorPoint(ccp(1.0f, 42.0f / 196.0f));
	pkTicketL->setScale(ui_main_scale);
	pkTicketL->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - 228 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkTicketL);

	CCSprite* pkTicketR = CCSprite::createWithSpriteFrameName("keno_ticket.png");
	pkTicketR->setAnchorPoint(ccp(1.0f, 42.0f / 196.0f));
	pkTicketR->setScaleX(-ui_main_scale);
	pkTicketR->setScaleY(ui_main_scale);
	pkTicketR->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - 228 * ui_main_scale * g_f32ScaleHD)));
	pkKenoRoot->addChild(pkTicketR);

	m_pkRunLabel = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkRunLabel->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkRunLabel->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 10 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() - 50 * ui_main_scale * g_f32ScaleHD)));
	m_pkRoot->addChild(m_pkRunLabel, 3);

	UIWidget* pkNumberRoot = VE_NEW UIWidget;
	pkNumberRoot->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() - 228 * ui_main_scale * g_f32ScaleHD)));
	pkNumberRoot->setScale(ui_main_scale);
	m_pkRoot->AddChild(pkNumberRoot);

	for(VeUInt32 i(0); i < 10; ++i)
	{
		m_af32NumberX[i] = VeFloorf((30.0f * i - 135.0f) * g_f32ScaleHD);
		m_apkNumber[i][0] = CCSprite::createWithSpriteFrameName("keno_0.png");
		m_apkNumber[i][0]->setColor(ccc3(0,0,0));
		m_apkNumber[i][0]->setPositionX(m_af32NumberX[i]);
		m_apkNumber[i][0]->setPositionY(VeFloorf(37 * g_f32ScaleHD));
		pkNumberRoot->addChild(m_apkNumber[i][0]);
		m_apkNumber[i][1] = CCSprite::createWithSpriteFrameName("keno_0.png");
		m_apkNumber[i][1]->setColor(ccc3(0,0,0));
		m_apkNumber[i][1]->setPositionX(m_af32NumberX[i]);
		m_apkNumber[i][1]->setPositionY(VeFloorf(37 * g_f32ScaleHD));
		pkNumberRoot->addChild(m_apkNumber[i][1]);

		m_apkHitCyc[i] = CCSprite::createWithSpriteFrameName("keno_cycle.png");
		m_apkHitCyc[i]->setPositionX(m_af32NumberX[i]);
		m_apkHitCyc[i]->setPositionY(VeFloorf(37 * g_f32ScaleHD));
		pkNumberRoot->addChild(m_apkHitCyc[i]);

		CCSprite* pkHole = CCSprite::createWithSpriteFrameName("keno_hole.png");
		pkHole->setPositionX(VeFloorf((22.0f * i - 55.0f) * g_f32ScaleHD));
		pkHole->setPositionY(VeFloorf(120 * g_f32ScaleHD));
		pkNumberRoot->addChild(pkHole);

		m_apkBall[i] = VE_NEW KenoBall;
		m_apkBall[i]->setPositionX(VeFloorf((22.0f * i - 55.0f) * g_f32ScaleHD));
		m_apkBall[i]->setPositionY(VeFloorf(128 * g_f32ScaleHD));
		pkNumberRoot->AddChild(m_apkBall[i]);
	}
	
	return UILayerModalExt<RunKeno>::init();
}

void RunKeno::OnActive(const UIUserDataPtr& spData)
{
	m_spData = spData;
	Callback& kCallback = (Callback&)*m_spData;
	for(VeUInt32 i(0); i < 10; ++i)
	{
		m_apkHitCyc[i]->setVisible(false);
		m_apkBall[i]->SetNumber(kCallback.m_au32KenoNumber[i]);
		m_apkBall[i]->SetVisible(false);
	}
	
	SetIssueNumber(kCallback.m_u32IssueNumber);
	SetChoose(kCallback.m_au32PlayerNumber);
	SetLabel(TT("RunKenoStart"));

	m_f32Rate = 0.0f;
	Update();
	schedule(schedule_selector(RunKeno::FadeIn), 0.0f);
	m_u32RunCount = 0;
	scheduleOnce(schedule_selector(RunKeno::Run), 0.7f);
	m_bPlaying = true;
}

void RunKeno::OnDeactive(const UIUserDataPtr& spData)
{
	m_bPlaying = false;
	m_spData = NULL;
}

bool RunKeno::OnBack()
{
	return false;
}

void RunKeno::SetIssueNumber(VeUInt32 u32Number)
{
	m_pkIssueNumber->setString(NTL(u32Number, 8));
	CCSize kSize = m_pkIssueNumber->getContentSize();
	m_pkIssueNumber->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void RunKeno::SetChoose(VeUInt32 au32Choose[10])
{
	VeChar8 acBuffer[64];
	for(VeUInt32 i(0); i < 10; ++i)
	{
		VeUInt32 u32Number = au32Choose[i];
		VE_ASSERT(u32Number > 0 && u32Number <= 80);
		VeUInt32 u32Unit = u32Number % 10;
		VeUInt32 u32Tens = u32Number / 10;

		if(u32Tens)
		{
			VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
			m_apkNumber[i][0]->initWithSpriteFrameName(acBuffer);
			m_apkNumber[i][0]->setPositionX(m_af32NumberX[i] + 5 * g_f32ScaleHD);
			m_apkNumber[i][0]->setColor(ccc3(0,0,0));
			VeSprintf(acBuffer, 64, "keno_%d.png", u32Tens);
			m_apkNumber[i][1]->initWithSpriteFrameName(acBuffer);
			m_apkNumber[i][1]->setPositionX(m_af32NumberX[i] - 5 * g_f32ScaleHD);
			m_apkNumber[i][1]->setColor(ccc3(0,0,0));
			m_apkNumber[i][0]->setVisible(true);
			m_apkNumber[i][1]->setVisible(true);
		}
		else
		{
			VeSprintf(acBuffer, 64, "keno_%d.png", u32Unit);
			m_apkNumber[i][0]->initWithSpriteFrameName(acBuffer);
			m_apkNumber[i][0]->setPositionX(m_af32NumberX[i]);
			m_apkNumber[i][0]->setColor(ccc3(0,0,0));
			m_apkNumber[i][0]->setVisible(true);
			m_apkNumber[i][1]->setVisible(false);
		}
	}
}

void RunKeno::SetLabel(const VeChar8* pcContent)
{
	CCSize kSize;
	m_pkRunLabel->setString(pcContent);
	kSize = m_pkRunLabel->getContentSize();
	m_pkRunLabel->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

bool RunKeno::IsPlaying()
{
	return m_bPlaying;
}

void RunKeno::FadeIn(VeFloat32 f32Delta)
{
	m_f32Rate += f32Delta * 5.0f;
	if(m_f32Rate >= 1.0f)
	{
		m_f32Rate = 1.0f;
		unschedule(schedule_selector(RunKeno::FadeIn));
	}
	Update();
}

void RunKeno::FadeOut(VeFloat32 f32Delta)
{
	m_f32Rate -= f32Delta * 5.0f;
	if(m_f32Rate <= 0.0f)
	{
		m_f32Rate = 0.0f;
		unschedule(schedule_selector(RunKeno::FadeOut));
		g_pLayerManager->PopLayer();
	}
	Update();
}

void RunKeno::Update()
{
	VeFloat32 f32Pos = VeLerp(m_f32Rate, 250.0f, 0);
	m_pkRoot->setPositionY(VeFloorf(f32Pos * ui_main_scale * g_f32ScaleHD));
}

void RunKeno::Run(VeFloat32 f32Delta)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "%s%d%s", TT("RunKenoBefore"), m_u32RunCount + 1, TT("RunKenoAfter"));
	SetLabel(acBuffer);
	RunGlobal();
	m_apkBall[m_u32RunCount]->Down();
	scheduleOnce(schedule_selector(RunKeno::RunNext), 1.6f);
}

void RunKeno::RunGlobal()
{
	CCAnimation* pkAnimation = CCAnimation::create();
	pkAnimation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("keno_global_1.png"));
	pkAnimation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("keno_global_2.png"));
	pkAnimation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("keno_global_1.png"));
	pkAnimation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("keno_global_0.png"));
	pkAnimation->setDelayPerUnit(1.0f / 12.0f);
	pkAnimation->setRestoreOriginalFrame(true);
	m_pkGlobal->runAction(CCRepeat::create(CCAnimate::create(pkAnimation), 3));
	g_pSoundSystem->PlayEffect(SE_GLOBAL);
}

void RunKeno::RunNext(VeFloat32 f32Delta)
{
	Callback& kCallback = (Callback&)*m_spData;
	for(VeUInt32 i(0); i < 10; ++i)
	{
		if(kCallback.m_au32KenoNumber[m_u32RunCount] == kCallback.m_au32PlayerNumber[i])
		{
			m_apkHitCyc[i]->setVisible(true);
			g_pSoundSystem->PlayEffect(SE_RIGHT);
		}
	}
	
	if((++m_u32RunCount) < 10)
	{
		scheduleOnce(schedule_selector(RunKeno::Run), 0.2f);
	}
	else
	{
		scheduleOnce(schedule_selector(RunKeno::WaitFade), 0.5f);
	}
}

void RunKeno::WaitFade(VeFloat32 f32Delta)
{
	unscheduleAllSelectors();
	schedule(schedule_selector(RunKeno::FadeOut), 0.0f);
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	pkPlayer->S_ReqKenoDraw();
}
