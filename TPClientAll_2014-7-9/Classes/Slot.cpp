#include "Slot.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "Knickknacks.h"
#include "Hall.h"
#include "SoundSystem.h"
#include "Keno.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"
#include "NativeInterface.h"

USING_NS_CC;


SlotNumber::SlotNumber(VeFloat32 f32Space)
	: m_f32Space(f32Space), m_u32Number(0)
{
	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("slot", false) + ".png");
	addChild(m_pkRoot);
	Update();
}

SlotNumber::~SlotNumber()
{
	Clear();
}

void SlotNumber::SetNumber(VeUInt32 u32Number)
{
	if(u32Number != m_u32Number)
	{
		m_u32Number = u32Number;
		Update();
	}
}

VeUInt32 SlotNumber::GetNumber()
{
	return m_u32Number;
}

void SlotNumber::Update()
{
	Clear();
	VeChar8 acBuffer[64];
	VeUInt32 i(0);
	VeUInt32 u32Temp = m_u32Number;
	do
	{
		VeSprintf(acBuffer, 64, "slot_%d.png", u32Temp % 10);
		CCSprite* pkSprite = CCSprite::createWithSpriteFrameName(acBuffer);
		pkSprite->setPositionX(-m_f32Space * i);
		m_pkRoot->addChild(pkSprite);
		u32Temp /= 10;
		++i;
	}
	while(u32Temp);
}

void SlotNumber::Clear()
{
	m_pkRoot->removeAllChildren();
}

JpBar::JpBar()
{
	setScale(ui_main_scale);

	m_pkBaceMapL = CCSprite::createWithSpriteFrameName("slot_jp_bar_2.png");
	m_pkBaceMapL->setAnchorPoint(ccp(1.0f, 0.5f));
	addChild(m_pkBaceMapL);

	m_pkBaceMapR = CCSprite::createWithSpriteFrameName("slot_jp_bar_2.png");
	m_pkBaceMapR->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkBaceMapR->setScaleX(-1);
	addChild(m_pkBaceMapR);

	m_apkBar[0] = CCSprite::createWithSpriteFrameName("slot_jp_bar_0.png");
	addChild(m_apkBar[0]);
	m_apkBar[0]->setVisible(true);

	m_apkBar[1] = CCSprite::createWithSpriteFrameName("slot_jp_bar_1.png");
	addChild(m_apkBar[1]);
	m_apkBar[1]->setVisible(false);

	CCSprite* pkJp = CCSprite::createWithSpriteFrameName("slot_jp.png");
	pkJp->setPositionX(-78.0f * g_f32ScaleHD);
	addChild(pkJp);

	m_pkNumber = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	m_pkNumber->setPositionX(82.0f * g_f32ScaleHD);
	AddChild(m_pkNumber);

	m_f32TimePass = 0;
	m_f32Rate = 1.0f;

	GetWell();
}

void JpBar::GetWell()
{
	unschedule(schedule_selector(JpBar::_BigPrizeEffect));
	schedule(schedule_selector(JpBar::Update), 0.0f);
}

void JpBar::Update(VeFloat32 f32Delta)
{
	m_f32TimePass += m_f32Rate * f32Delta;
	if(m_f32TimePass >= 0.3f)
	{
		m_f32TimePass = 0;
		Change();
	}
}

void JpBar::SetRate(VeFloat32 f32Rate)
{
	m_f32Rate = f32Rate;
}

void JpBar::BigPrizeEffect(VeUInt32 m_prizeType)
{
	VeFloat32 m_f32PrizeTag = 0.0f;
	switch (m_prizeType)
	{
	case MAX_PRIZE:
		m_f32PrizeTag = 0.09f;
		break;
	case MID_PRIZE:
		m_f32PrizeTag = 0.18f;
		break;
	case MIN_PRIZE:
		m_f32PrizeTag = 0.27f;
		break;
	}
	unschedule(schedule_selector(JpBar::Update));
	schedule(schedule_selector(JpBar::_BigPrizeEffect),m_f32PrizeTag);
}

void JpBar::Change()
{
	if(m_apkBar[0]->isVisible())
	{
		m_apkBar[0]->setVisible(false);
		m_apkBar[1]->setVisible(true);
	}
	else
	{
		m_apkBar[0]->setVisible(true);
		m_apkBar[1]->setVisible(false);
	}
}

void JpBar::_BigPrizeEffect(VeFloat32 f32Delta)
{
	if (m_apkBar[0]->isVisible() || m_apkBar[1]->isVisible())
	{
		m_apkBar[0]->setVisible(false);
		m_apkBar[1]->setVisible(false);
	}
	else
	{
		m_apkBar[0]->setVisible(true);
		m_apkBar[1]->setVisible(true);
	}
}

void JpBar::SetNumber(VeUInt32 u32Number)
{
	m_pkNumber->SetNumber(u32Number);
}

static VeUInt8 s_au8SlotItemList[] =
{
	0,1,2,3,4,5,6,7,8,9,10,9,4,7,1,8,6,2,5,0,10,3,7,10,3,9,6,5,1,0,4,2,8
};

SlotRoll::SlotRoll()
{
	setContentSize(CCSize(252 * g_f32ScaleHD, 200 * g_f32ScaleHD));
	SetClipByRange(true);
	setScale(ui_main_scale);

	for(VeUInt32 i(0); i < 3; ++i)
	{
		m_apkRollNode[i] = CCNode::create();
		m_apkRollNode[i]->setPositionX(VeFloor((84.0f * i - 84) * g_f32ScaleHD));
		addChild(m_apkRollNode[i]);
		for(VeUInt32 j(0); j < 4; ++j)
		{
			m_apkRollItem[i][j] = CCSprite::createWithSpriteFrameName("slots_item_1.png");
			m_apkRollItem[i][j]->setPositionY(VeFloor((68 - 68.0f * j) * g_f32ScaleHD));
			m_apkRollNode[i]->addChild(m_apkRollItem[i][j]);
		}

		CCSprite* pkShadowT = CCSprite::createWithSpriteFrameName("slot_shadow_t.png");
		pkShadowT->setAnchorPoint(ccp(0.5f, 1.0f));
		pkShadowT->setPosition(ccp(VeFloor((84.0f * i - 84) * g_f32ScaleHD), 100 * g_f32ScaleHD));
		addChild(pkShadowT, 5);

		CCSprite* pkShadowB = CCSprite::createWithSpriteFrameName("slot_shadow_b.png");
		pkShadowB->setAnchorPoint(ccp(0.5f, 0.0f));
		pkShadowB->setPosition(ccp(VeFloor((84.0f * i - 84) * g_f32ScaleHD), -100 * g_f32ScaleHD));
		addChild(pkShadowB, 5);

		m_apkLight[i][0] = CCSprite::createWithSpriteFrameName("slot_light_off.png");
		m_apkLight[i][0]->setPosition(ccp(VeFloor((84.0f * i - 84) * g_f32ScaleHD), 84 * g_f32ScaleHD));
		addChild(m_apkLight[i][0], 5);

		m_apkLight[i][1] = CCSprite::createWithSpriteFrameName("slot_light_on.png");
		m_apkLight[i][1]->setPosition(ccp(VeFloor((84.0f * i - 84) * g_f32ScaleHD), 84 * g_f32ScaleHD));
		addChild(m_apkLight[i][1], 5);
		m_apkLight[i][1]->setVisible(false);

		m_au32Pos[i] = 1;
		m_af32Offset[i] = 0;
	}

	UpdatePos();
	m_bRoll = false;
	m_bStop = false;
}

void SlotRoll::Roll()
{
	if(!m_bRoll)
	{
		g_pSoundSystem->StopMusic();
		g_pSoundSystem->PlayMusic(SLOT_LOOP_EFF);
		unscheduleAllSelectors();
		for(VeUInt32 i(0); i < 3; ++i)
		{
			m_af32Speed[i] = 0;
			m_af32SpeedMax[i] = VeRangeRandomf(6.0f, 12.0f);
		}
		schedule(schedule_selector(SlotRoll::Update), 0.0f);
		m_f32PassTime = 0.0f;
		m_f32LightTime = 0.0f;
		m_bRoll = true;
		m_bStop = false;
	}
}

void SlotRoll::Stop(VeUInt32 u32Left, VeUInt32 u32Middle, VeUInt32 u32Right)
{
	if(m_bRoll && (!m_bStop))
	{
		g_pSoundSystem->PlayEffect(SE_SLOT_ARMRELEASE);
		unscheduleAllSelectors();
		m_au32Dest[0] = u32Left;
		m_au32Dest[1] = u32Middle;
		m_au32Dest[2] = u32Right;
		schedule(schedule_selector(SlotRoll::UpdateStop), 0.0f);
		m_f32StopTime = SLOT_DELAY_TIME;
		m_u32Stop = 0;
		m_bReduceSpeed = false;
		m_bStop = true;
	}
}

void SlotRoll::Update(VeFloat32 f32Delta)
{
	for(VeUInt32 i(0); i < 3; ++i)
	{
		if(m_af32Speed[i] < m_af32SpeedMax[i])
		{
			m_af32Speed[i] += f32Delta * m_af32SpeedMax[i] * 3.0f;
			if(m_af32Speed[i] > m_af32SpeedMax[i])
				m_af32Speed[i] = m_af32SpeedMax[i];
		}

		m_af32Offset[i] -= m_af32Speed[i] * f32Delta;
		if(m_af32Offset[i] < 0.0f)
		{
			m_au32Pos[i] -= 1;
			if(m_au32Pos[i] > sizeof(s_au8SlotItemList) - 1)
			{
				m_au32Pos[i] = sizeof(s_au8SlotItemList) - 1;
			}
			m_af32Offset[i] = m_af32Offset[i] + 1.0f;
			UpdatePos();
		}
		m_apkRollNode[i]->setPositionY(m_af32Offset[i] * 68.0f * g_f32ScaleHD);
	}

	m_f32PassTime += f32Delta;
	UpdateLight(f32Delta);
}

void SlotRoll::UpdateStop(VeFloat32 f32Delta)
{
	if(m_u32Stop < 3)
	{
		VeUInt32 i(m_u32Stop);
		if(m_f32PassTime >= m_f32StopTime)
		{
			if(!m_bReduceSpeed)
			{
				VeUInt32 u32Next = m_au32Pos[i] - 1;
				if(u32Next > sizeof(s_au8SlotItemList) - 1)
				{
					u32Next = sizeof(s_au8SlotItemList) - 1;
				}
				bool bStopNext = s_au8SlotItemList[u32Next] == m_au32Dest[i];

				m_af32Offset[i] -= m_af32Speed[i] * f32Delta;
				if(m_af32Offset[i] < 0.0f)
				{
					m_au32Pos[i] -= 1;
					if(m_au32Pos[i] > sizeof(s_au8SlotItemList) - 1)
					{
						m_au32Pos[i] = sizeof(s_au8SlotItemList) - 1;
					}
					m_af32Offset[i] = m_af32Offset[i] + 1.0f;
					UpdatePos();
					if(bStopNext)
					{
						g_pSoundSystem->PlayEffect(SE_SLOT_REELSTOP);
						m_bReduceSpeed = true;
						m_f32TimeStop = VE_MATH_PI_F * 0.25f / m_af32Speed[i];
					}
				}
				m_apkRollNode[i]->setPositionY(m_af32Offset[i] * 68.0f * g_f32ScaleHD);
			}
			else
			{
				m_f32TimeStop -= f32Delta;
				if(m_f32TimeStop < 0.0f)
				{
					++m_u32Stop;
					m_f32StopTime += SLOT_STOP_DELAY_TIME;
					m_f32TimeStop = 0.0f;
					m_bReduceSpeed = false;
				}

				VeFloat32 f32Time = m_f32TimeStop * m_af32Speed[i] / (VE_MATH_PI_F * 0.25f);
				m_af32Offset[i] = VeSinf(f32Time);
				m_apkRollNode[i]->setPositionY(m_af32Offset[i] * 68.0f * g_f32ScaleHD);
			}
			++i;
		}

		for(; i < 3; ++i)
		{
			if(m_af32Speed[i] < m_af32SpeedMax[i])
			{
				m_af32Speed[i] += f32Delta * m_af32SpeedMax[i] * 3.0f;
				if(m_af32Speed[i] > m_af32SpeedMax[i])
					m_af32Speed[i] = m_af32SpeedMax[i];
			}

			m_af32Offset[i] -= m_af32Speed[i] * f32Delta;
			if(m_af32Offset[i] < 0.0f)
			{
				m_au32Pos[i] -= 1;
				if(m_au32Pos[i] > sizeof(s_au8SlotItemList) - 1)
				{
					m_au32Pos[i] = sizeof(s_au8SlotItemList) - 1;
				}
				m_af32Offset[i] = m_af32Offset[i] + 1.0f;
				UpdatePos();
			}
			m_apkRollNode[i]->setPositionY(m_af32Offset[i] * 68.0f * g_f32ScaleHD);
		}
	}
	else
	{
		unscheduleAllSelectors();
		m_bRoll = false;
		m_bStop = false;
		m_kEvent.Callback(*this, SLOT_ROLL_STOP);
		m_apkLight[0][0]->setVisible(true);
		m_apkLight[1][0]->setVisible(true);
		m_apkLight[2][0]->setVisible(true);
		m_apkLight[0][1]->setVisible(false);
		m_apkLight[1][1]->setVisible(false);
		m_apkLight[2][1]->setVisible(false);
		return;
	}

	m_f32PassTime += f32Delta;
	UpdateLight(f32Delta);
}

void SlotRoll::UpdatePos()
{
	VeChar8 acBuffer[64];
	for(VeUInt32 i(0); i < 3; ++i)
	{
		VE_ASSERT(m_au32Pos[i] < sizeof(s_au8SlotItemList));
		for(VeUInt32 j(0); j < 4; ++j)
		{
			VeInt32 i32Index = m_au32Pos[i] + j;
			i32Index -= 1;
			if(i32Index < 0) i32Index += sizeof(s_au8SlotItemList);
			i32Index = i32Index % sizeof(s_au8SlotItemList);		
			VeSprintf(acBuffer, 64, "slots_item_%d.png", s_au8SlotItemList[i32Index]);
			m_apkRollItem[i][j]->initWithSpriteFrameName(acBuffer);
		}
	}
}

void SlotRoll::visit()
{
	if (!m_bVisible)
	{
		return;
	}
	if(m_bClipByRange)
	{
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
	else
	{
		CCNode::visit();
	}
}

void SlotRoll::UpdateLight(VeFloat32 f32Delta)
{
	m_f32LightTime += f32Delta * 3.0f;
	while(m_f32LightTime > 1.0f)
	{
		m_f32LightTime -= 1.0f;
	}
	if(m_f32LightTime < 0.5f)
	{
		m_apkLight[0][0]->setVisible(false);
		m_apkLight[1][0]->setVisible(true);
		m_apkLight[2][0]->setVisible(false);
		m_apkLight[0][1]->setVisible(true);
		m_apkLight[1][1]->setVisible(false);
		m_apkLight[2][1]->setVisible(true);
	}
	else
	{
		m_apkLight[0][0]->setVisible(true);
		m_apkLight[1][0]->setVisible(false);
		m_apkLight[2][0]->setVisible(true);
		m_apkLight[0][1]->setVisible(false);
		m_apkLight[1][1]->setVisible(true);
		m_apkLight[2][1]->setVisible(false);
	}
}

SlotPollBar::SlotPollBar()
{
	setScale(ui_main_scale);

	CCSprite* pkGroove = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkGroove->setRotation(90);
	addChild(pkGroove);

	m_pkGrooveFill = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("slot_poll_bar.png"));
	m_pkGrooveFill->setType(kCCProgressTimerTypeBar);
	m_pkGrooveFill->setMidpoint(ccp(0,1));
	m_pkGrooveFill->setBarChangeRate(ccp(0, 1));
	addChild(m_pkGrooveFill);

	for(VeUInt32 i(0); i < 3; ++i)
	{
		m_apkArrows[i] = CCSprite::createWithSpriteFrameName("slots_arrows_n.png");
		m_apkArrows[i]->setPositionX(19 * g_f32ScaleHD);
		m_apkArrows[i]->setPositionY((25.0f - 28.0f * i) * g_f32ScaleHD);

		addChild(m_apkArrows[i]);
	}

	m_pkHandle = CCSprite::createWithSpriteFrameName("slot_poll.png");
	addChild(m_pkHandle);

	m_bArrowsMove = false;
	SetArrowsMove(true);

	m_f32Top = 49.0f * g_f32ScaleHD;
	m_f32Bottom = -21.0f * g_f32ScaleHD;
	m_f32Middle = 9.0f * g_f32ScaleHD;
	m_f32Rate = 0.0f;
	UpdateHandle();
	m_bIsTouchHandle = false;

	schedule(schedule_selector(SlotPollBar::Update), 0.0f);
}

void SlotPollBar::SetArrowsMove(bool bMove)
{
	if(m_bArrowsMove != bMove)
	{
		if(bMove)
		{
			schedule(schedule_selector(SlotPollBar::UpdateArrows), 0.0f);
			m_f32PassTime = 0;
		}
		else
		{
			unschedule(schedule_selector(SlotPollBar::UpdateArrows));
			for(VeUInt32 i(0); i < 3; ++i)
			{
				m_apkArrows[i]->initWithSpriteFrameName("slots_arrows_n.png");
			}
		}
		m_bArrowsMove = bMove;
	}
}

void SlotPollBar::UpdateArrows(VeFloat32 f32Delta)
{
	VeFloat32 f32Last = m_f32PassTime;
	m_f32PassTime += f32Delta;
	if(f32Last < 0.5f)
	{
		if(m_f32PassTime >= 0.5f)
		{
			m_apkArrows[0]->initWithSpriteFrameName("slots_arrows_h.png");
			m_apkArrows[1]->initWithSpriteFrameName("slots_arrows_n.png");
			m_apkArrows[2]->initWithSpriteFrameName("slots_arrows_n.png");
		}
	}
	else if(f32Last < 1.0f)
	{
		if(m_f32PassTime >= 1.0f)
		{
			m_apkArrows[0]->initWithSpriteFrameName("slots_arrows_n.png");
			m_apkArrows[1]->initWithSpriteFrameName("slots_arrows_h.png");
			m_apkArrows[2]->initWithSpriteFrameName("slots_arrows_n.png");
		}
	}
	else if(f32Last < 1.5f)
	{
		if(m_f32PassTime >= 1.5f)
		{
			m_apkArrows[0]->initWithSpriteFrameName("slots_arrows_n.png");
			m_apkArrows[1]->initWithSpriteFrameName("slots_arrows_n.png");
			m_apkArrows[2]->initWithSpriteFrameName("slots_arrows_h.png");
		}
	}
	else if(f32Last < 2.0f)
	{
		if(m_f32PassTime >= 2.0f)
		{
			m_apkArrows[0]->initWithSpriteFrameName("slots_arrows_n.png");
			m_apkArrows[1]->initWithSpriteFrameName("slots_arrows_n.png");
			m_apkArrows[2]->initWithSpriteFrameName("slots_arrows_n.png");
			m_f32PassTime -= 2.0f;
		}
	}
}

void SlotPollBar::UpdateHandle()
{
	m_f32Rate = VE_MIN(m_f32Rate, 1.0f);
	m_f32Rate = VE_MAX(m_f32Rate, 0.0f);
	m_pkHandle->setPositionY(VeFloorf(VeLerp(m_f32Rate, m_f32Top, m_f32Bottom)));
	m_pkGrooveFill->setPercentage(VeLerp(m_f32Rate, 1600.0f / 172.0f, 15600.0f / 172.0f));
}

UIWidget* SlotPollBar::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	static CCPoint s_kBeganPos;
	static VeFloat32 s_f32Start;
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			if(m_bArrowsMove)
			{
				s_kBeganPos = pkTouch->getLocation();
				s_f32Start = m_pkHandle->getPositionY();
				m_f32Velocity = 0;
				m_bRollBack = false;
				m_f32RollBackPos = 0;
				m_bTouch = true;
			}
			break;
		case UILayer::TOUCH_MOVED:
			if(m_bTouch)
			{
				VeFloat32 f32DeltaY = pkTouch->getLocation().y - pkTouch->getPreviousLocation().y;
				m_f32Velocity = f32DeltaY * EVENT_RATE;
				m_f32Velocity = VE_MIN(m_f32Velocity, VELOCITYMAX);
				m_f32Velocity = VE_MAX(m_f32Velocity, -VELOCITYMAX);
				s_f32Start += f32DeltaY;
				Move(s_f32Start);
			}
			break;
		case UILayer::TOUCH_ENDED:
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

void SlotPollBar::EventRelease()
{
	m_bTouch = false;
}

bool SlotPollBar::IsIn(VeFloat32 x, VeFloat32 y)
{
	CCAffineTransform kTransform = m_pkHandle->nodeToWorldTransform();
	CCRect kRect = m_pkHandle->boundingBox();
	kRect.origin.x = kTransform.tx - kRect.size.width * 0.5f;
	kRect.origin.y = kTransform.ty - kRect.size.height * 0.5f;
	kRect.size.width *= 2.0f;
	kRect.size.height *= 2.0f;
	return kRect.containsPoint(ccp(x,y));
}

void SlotPollBar::Move(VeFloat32 f32Pos)
{
	if(f32Pos >= m_f32Top || f32Pos <= m_f32Bottom)
	{
		m_f32Velocity = 0;
	}
	f32Pos = VE_MAX(f32Pos, m_f32Bottom);
	f32Pos = VE_MIN(f32Pos, m_f32Top);
	VeFloat32 f32PosPre = m_pkHandle->getPositionY();
	if(f32PosPre >= m_f32Middle && f32Pos < m_f32Middle)
	{
		m_kEvent.Callback(*this, SLOT_POLL_DOWN);
	}
	m_f32Rate = 1.0f - (f32Pos - m_f32Bottom) / (m_f32Top - m_f32Bottom);
	UpdateHandle();
}

void SlotPollBar::Update(VeFloat32 f32Delta)
{
	if(!m_bTouch)
	{
		if(m_f32Velocity == 0)
		{
			VeFloat32 f32Pos = m_pkHandle->getPositionY();
			if(m_bRollBack)
			{
				if(m_f32RollBackTime <= 0)
				{
					m_bRollBack = false;
					m_f32RollBackTime = 0;
				}
				else if(m_f32RollBackPos < m_f32Top)
				{
					m_f32RollBackTime -= f32Delta;
					if(m_f32RollBackTime > 0)
					{
						VeFloat32 f32Offset = VeCosf((m_f32RollBackTime / SLOT_ROLLBACK_TIME) * VE_MATH_PI_2_F);
						Move(VeLerp(f32Offset, m_f32RollBackPos, m_f32Top));
					}
					else
					{
						m_f32RollBackTime = 0;
						m_bRollBack = false;
						Move(m_f32Top);
					}
				}
				else
				{
					m_bRollBack = false;
					m_f32RollBackTime = 0;
				}
			}
			else if(f32Pos < m_f32Top)
			{
				m_bRollBack = true;
				m_f32RollBackTime = SLOT_ROLLBACK_TIME;
				m_f32RollBackPos = f32Pos;
			}			
		}
		else
		{
			VeFloat32 f32Pos = m_pkHandle->getPositionY();
			VeFloat32 f32Acce = FRICTION_PER_SEC * ui_main_scale * 0.5f;
			if(m_f32Velocity > 0)
			{
				m_f32Velocity = VE_MAX(m_f32Velocity - f32Acce * f32Delta, 0);
			}
			else
			{
				m_f32Velocity = VE_MIN(m_f32Velocity + f32Acce * f32Delta, 0);
			}
			Move(f32Pos + m_f32Velocity * f32Delta);
		}
	}
}

Slot::Slot() : UILayerExt<Slot>("slot")
{
	UIInitDelegate(Slot, OnBack);
	UIInitDelegate(Slot, OnPurchase);
	UIInitDelegate(Slot, OnOdds);
	UIInitDelegate(Slot, OnInc);
	UIInitDelegate(Slot, OnDec);
	UIInitDelegate(Slot, OnPollDown);
	UIInitDelegate(Slot, OnRollStop);

	m_bIsRoll = false;
	m_u64WinChips = 0;
	m_u64NowChips = 0;
}

Slot::~Slot()
{
	term();
}

bool Slot::init()
{
	CCSize kSize;

	CCSpriteBatchNode* pkBackRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	addChild(pkBackRoot);

	CCSpriteBatchNode* pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("slot", false) + ".png");
	addChild(pkRoot);

	for(VeUInt32 j(0); j < 4; ++j)
	{
		CCSprite* pkBackground = CCSprite::createWithSpriteFrameName("bg_1.png");
		pkBackground->setAnchorPoint(ccp(1.0f, 0.0f));
		pkBackground->setPosition(ui_ccp(0.5f, 0.5f));
		pkBackground->setBlendFunc(kCCBlendFuncDisable);
		pkBackRoot->addChild(pkBackground, 0);

		CCSprite* pkSlotBg0 = CCSprite::createWithSpriteFrameName("slot_bg_0.png");
		pkSlotBg0->setAnchorPoint(ccp(1.0f, 0.0f));
		pkSlotBg0->setPosition(ccp(ui_w(0.5f),
			VeFloorf((68 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
		pkRoot->addChild(pkSlotBg0);

		CCSprite* pkSlotBg1 = CCSprite::createWithSpriteFrameName("slot_bg_1.png");
		pkSlotBg1->setAnchorPoint(ccp(1.0f, 0.0f));
		pkSlotBg1->setPosition(ccp(ui_w(0.5f),
			VeFloorf((0 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
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

	CCSprite* pkLight = CCSprite::createWithSpriteFrameName("slots_light.png");
	pkLight->setPosition(ccp(ui_w(0.5f),
		VeFloorf((188 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkLight->setScale(ui_main_scale);
	addChild(pkLight, 5);

	CCSprite* pkIcon = CCSprite::createWithSpriteFrameName("nini_slots_n.png");
	pkIcon->setPosition(ccp(ui_w(0.5f),
		VeFloorf((188 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkIcon->setScale(ui_main_scale);
	addChild(pkIcon, 5);

	CCSprite* pkButtomL = CCSprite::createWithSpriteFrameName("slot_bottom.png");
	pkButtomL->setAnchorPoint(ccp(1.0f, 0.0f));
	pkButtomL->setPosition(ui_ccp(0.5f, 0.0f));
	pkButtomL->setScale(ui_main_scale);
	pkRoot->addChild(pkButtomL, 2);
	CCSprite* pkButtomR = CCSprite::createWithSpriteFrameName("slot_bottom.png");
	pkButtomR->setAnchorPoint(ccp(1.0f, 0.0f));
	pkButtomR->setPosition(ui_ccp(0.5f, 0.0f));
	pkButtomR->setScaleX(-ui_main_scale);
	pkButtomR->setScaleY(ui_main_scale);
	pkRoot->addChild(pkButtomR, 2);

	CCSprite* pkBoardL = CCSprite::createWithSpriteFrameName("slot_board.png");
	pkBoardL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBoardL->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 10 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((50 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
	pkBoardL->setScale(ui_main_scale);
	pkRoot->addChild(pkBoardL, 2);
	CCSprite* pkBoardR = CCSprite::createWithSpriteFrameName("slot_board.png");
	pkBoardR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBoardR->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 10 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((50 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
	pkBoardR->setScaleX(-ui_main_scale);
	pkBoardR->setScaleY(ui_main_scale);
	pkRoot->addChild(pkBoardR, 2);

	CCSprite* pkYouWinFrame = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkYouWinFrame->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 50 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((84 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkYouWinFrame->setScale(ui_main_scale);
	pkRoot->addChild(pkYouWinFrame, 2);

	CCSprite* pkCreditFrame = CCSprite::createWithSpriteFrameName("slot_frame.png");
	pkCreditFrame->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 70 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((84 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkCreditFrame->setScale(ui_main_scale);
	pkRoot->addChild(pkCreditFrame, 2);

	CCSprite* pkYouWin = CCSprite::createWithSpriteFrameName("slot_win_text.png");
	pkYouWin->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 50 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((104 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkYouWin->setScale(ui_main_scale);
	addChild(pkYouWin);

	CCSprite* pkCredit = CCSprite::createWithSpriteFrameName("slot_chips_text.png");
	pkCredit->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 70 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((104 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkCredit->setScale(ui_main_scale);
	addChild(pkCredit);

	CCSprite* pkYouWinIcon = CCSprite::createWithSpriteFrameName("slot_win_icon.png");
	pkYouWinIcon->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 108 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((88 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkYouWinIcon->setScale(ui_main_scale);
	pkRoot->addChild(pkYouWinIcon, 2);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chips.png");
	pkChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 16 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((84 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkChips->setScale(ui_main_scale);
	addChild(pkChips);

	m_pkWinChips = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	m_pkWinChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 14 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((84 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	addChild(m_pkWinChips);

	m_pkCredit = CCLabelTTF::create("", TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	m_pkCredit->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 106 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((84 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	addChild(m_pkCredit);

	CCSprite* pkBetLine = CCSprite::createWithSpriteFrameName("slot_bet_line.png");
	pkBetLine->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 10 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((51 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
	pkBetLine->setScaleX(ui_main_scale * g_f32ScaleHD * 247);
	pkBetLine->setScaleY(ui_main_scale);
	addChild(pkBetLine, 5);

	CCSprite* pkBetLineSpot = CCSprite::createWithSpriteFrameName("slot_bet_line_spot.png");
	pkBetLineSpot->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 145 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((54 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
	pkBetLineSpot->setScale(ui_main_scale);
	addChild(pkBetLineSpot, 5);

	NoticeBar* pkNoticeBar = VE_NEW NoticeBar;
	pkNoticeBar->setPositionY(VeFloorf(UILayer::Height() * 0.5f + 230.0f * g_f32ScaleHD * ui_height_scale));
	AddWidget("notice_bar", pkNoticeBar, 0);

	JpBar* pkJpBar = VE_NEW JpBar;
	pkJpBar->setPosition(ccp(ui_w(0.5f),
		VeFloorf((144 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	AddWidget("jp_bar", pkJpBar);
	pkJpBar->SetNumber(0);

	SlotNumber* pkBetChips = VE_NEW SlotNumber(17 * g_f32ScaleHD);
	pkBetChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 64 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(39 * ui_main_scale * g_f32ScaleHD)));
	pkBetChips->setScale(ui_main_scale);
	AddWidget("bet_chips", pkBetChips);

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
	pkPurchase->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 129 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((158 * ui_main_scale + 260 * ui_height_scale) * g_f32ScaleHD)));
	pkPurchase->setScale(ui_main_scale);
	CCSprite* pkPurchaseText = CCSprite::createWithSpriteFrameName("mini_buy_chips.png");
	pkPurchaseText->setPosition(ccp(8 * g_f32ScaleHD, 6.0f * g_f32ScaleHD));
	pkPurchase->addChild(pkPurchaseText, 5);
	AddWidget("purchase", pkPurchase);
	UILinkDelegate(pkPurchase, OnPurchase);

	UIButton* pkOdds = VE_NEW UIButton("slots_button_n.png", "slots_button_p.png");
	pkOdds->setPosition(ccp(ui_w(0.5f),
		VeFloorf(88.0f * ui_main_scale * g_f32ScaleHD)));
	pkOdds->setScale(ui_main_scale);
	CCSprite* pkOddsText = CCSprite::createWithSpriteFrameName("slot_odds.png");
	pkOddsText->setPositionX(1.0f * g_f32ScaleHD);
	pkOdds->addChild(pkOddsText, 5);
	AddWidget("odds", pkOdds);
	UILinkDelegate(pkOdds, OnOdds);

	UIButton* pkBetInc = VE_NEW UIButton("cyc_btn_n.png", "cyc_btn_p.png", "cyc_btn_d.png");
	pkBetInc->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 125 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(36.0f * ui_main_scale * g_f32ScaleHD)));
	pkBetInc->setScale(ui_main_scale);
	CCSize BetIncSize = pkBetInc->getContentSize();
	BetIncSize.width *= 1.5f;
	BetIncSize.height *= 1.5f;
	pkBetInc->setContentSize(BetIncSize);
	m_pkIncOnBetBtn = CCSprite::createWithSpriteFrameName("+.png");
	m_pkIncOnBetBtn->setPositionY(2.0f * g_f32ScaleHD);
	pkBetInc->addChild(m_pkIncOnBetBtn, 5);
	AddWidget("bet_inc", pkBetInc);
	UILinkDelegate(pkBetInc, OnInc);

	UIButton* pkBetDec = VE_NEW UIButton("cyc_btn_n.png", "cyc_btn_p.png", "cyc_btn_d.png");
	pkBetDec->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 125 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(36.0f * ui_main_scale * g_f32ScaleHD)));
	pkBetDec->setScale(ui_main_scale);
	CCSize BetDecSize = pkBetInc->getContentSize();
	BetDecSize.width *= 1.5f;
	BetDecSize.height *= 1.5f;
	pkBetDec->setContentSize(BetDecSize);
	m_pkDecOnBetBtn = CCSprite::createWithSpriteFrameName("-.png");
	m_pkDecOnBetBtn->setPositionY(2.0f * g_f32ScaleHD);
	pkBetDec->addChild(m_pkDecOnBetBtn, 5);
	AddWidget("bet_dec", pkBetDec);
	UILinkDelegate(pkBetDec, OnDec);
	pkBetDec->SetEnable(false);

	SlotRoll* pkSlotRoll = VE_NEW SlotRoll;
	pkSlotRoll->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 10 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((50 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
	AddWidget("slot_roll", pkSlotRoll);
	UILinkDelegate(pkSlotRoll, OnRollStop);

	SlotPollBar* pkSlotPollBar = VE_NEW SlotPollBar;
	pkSlotPollBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 132 * ui_main_scale * g_f32ScaleHD),
		VeFloorf((50 * ui_main_scale + 160 * ui_height_scale) * g_f32ScaleHD)));
	AddWidget("slot_poll_bar", pkSlotPollBar);
	UILinkDelegate(pkSlotPollBar, OnPollDown);

	m_u32Bet = VE_INFINITE;
	SetBet(0);

	return UILayer::init();
}

void Slot::term()
{
	UILayer::term();
}

void Slot::OnActive(const UIUserDataPtr& spData)
{
	VeSrand(g_pTime->GetCurrentCount());
	UpdateSlot();
	g_pSoundSystem->PlayMusic(SLOT_MUSIC);
	schedule(schedule_selector(Slot::Tick), 3.0f);
	NativeOnEventBegin("slot");
}

void Slot::OnDeactive(const UIUserDataPtr& spData)
{
	unschedule(schedule_selector(Slot::Tick));
	NativeOnEventEnd("slot");
}

bool Slot::OnBack()
{
	if(m_bIsRoll)
	{
		g_pkGame->Toast(TT("slot_roll_back"));
		return false;
	}
	else
	{
		return true;
	}
}

void Slot::SetChips(VeUInt64 u64Win, VeUInt64 u64Credit)
{
	CCSize kSize;
	m_pkWinChips->setString(NTL(u64Win, 8));
	kSize = m_pkWinChips->getContentSize();
	m_pkWinChips->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkCredit->setString(NTL(u64Credit, 8));
	kSize = m_pkCredit->getContentSize();
	m_pkCredit->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Slot::SetBet(VeUInt32 u32Bet)
{
	u32Bet = VE_MIN(u32Bet, SLOT_BET_MAX - 1);
	if(m_u32Bet != u32Bet)
	{
		WIDGET(UIButton, bet_inc);
		WIDGET(UIButton, bet_dec);
		WIDGET(SlotNumber, bet_chips);
		if(u32Bet == 0)
		{
			bet_inc.SetEnable(true);
			bet_dec.SetEnable(false);
		}
		else if(u32Bet == (SLOT_BET_MAX - 1))
		{
			bet_inc.SetEnable(false);
			bet_dec.SetEnable(true);
		}
		else
		{
			bet_inc.SetEnable(true);
			bet_dec.SetEnable(true);
		}
		bet_chips.SetNumber((u32Bet + 1) * m_u32SingleBet);
		m_u32Bet = u32Bet;
	}
}

VeUInt32 Slot::GetBet()
{
	WIDGET(SlotNumber, bet_chips);
	return bet_chips.GetNumber();
}

UIImplementDelegate(Slot, OnBack, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Slot, OnPurchase, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("shop");
	}
}

UIImplementDelegate(Slot, OnOdds, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("slot_help");
	}
}

UIImplementDelegate(Slot, OnInc, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkIncOnBetBtn->setScale(1.0f);
	}
	else
	{
		m_pkIncOnBetBtn->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		SetBet(m_u32Bet + 1);
	}
}

UIImplementDelegate(Slot, OnDec, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkDecOnBetBtn->setScale(1.0f);
	}
	else
	{
		m_pkDecOnBetBtn->setScale(0.98f);
	}

	if(u32State == UIWidget::RELEASED)
	{
		SetBet(m_u32Bet - 1);
	}
}

UIImplementDelegate(Slot, OnPollDown, kWidget, u32State)
{
	if(u32State == SlotPollBar::SLOT_POLL_DOWN && m_bIsRoll == false)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		g_pSoundSystem->PlayEffect(SE_SLOT_ARMPULL);
		if(pkPlayer->GetChips() < GetBet())
		{
			g_pkGame->Toast(TT("SlotChipsNotEnough"));
			return;
		}
		pkPlayer->S_ReqSlotRun(m_u32Bet);
		WIDGET(JpBar, jp_bar);
		jp_bar.SetRate(1.0f);
		jp_bar.GetWell();
		WIDGET(SlotRoll, slot_roll);
		slot_roll.Roll();
		WIDGET(SlotPollBar, slot_poll_bar);
		slot_poll_bar.SetArrowsMove(false);
		WIDGET(UIButton, bet_inc);
		bet_inc.SetEnable(false);
		WIDGET(UIButton, bet_dec);
		bet_dec.SetEnable(false);
		m_bIsRoll = true;
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("bet_chips", VeStringA::From((m_u32Bet+1) * pkPlayer->GetSingleBet())));
		NativeOnEvent("ReqSlotRun", kEventValue);
	}
}

UIImplementDelegate(Slot, OnRollStop, kWidget, u32State)
{
	if(u32State == SlotRoll::SLOT_ROLL_STOP)
	{
		m_u64WinChips = m_u64WillWin;
		SetChips(m_u64WinChips, m_u64NowChips);
		if(m_u64WinChips)
		{
			scheduleOnce(schedule_selector(Slot::ScheduleWinChipsUpdate),1.0f);
			VeFloat64 f64Rate = VeFloat64(m_u64WinChips) / VeFloat64(GetBet());
			if(f64Rate <= 7.0)
			{
				g_pSoundSystem->PlayEffect(SE_SLOT_WINTYPE0);
				WIDGET(JpBar, jp_bar);
				jp_bar.BigPrizeEffect(MIN_PRIZE);
			}
			else if(f64Rate <= 25.0)
			{
				g_pSoundSystem->PlayEffect(SE_SLOT_WINTYPE1);
				WIDGET(JpBar, jp_bar);
				jp_bar.BigPrizeEffect(MID_PRIZE);
			}
			else
			{
				g_pSoundSystem->PlayEffect(SE_SLOT_WINTYPE2);
				WIDGET(JpBar, jp_bar);
				jp_bar.BigPrizeEffect(MAX_PRIZE);
			}
		}
		else
		{
			UpdateSlot();
			WIDGET(SlotPollBar, slot_poll_bar);
			slot_poll_bar.SetArrowsMove(true);
			VeUInt32 u32Bet = m_u32Bet;
			m_u32Bet = VE_INFINITE;
			SetBet(u32Bet);
			m_bIsRoll = false;
		}
		g_pSoundSystem->StopMusic();
		g_pSoundSystem->PlayMusic(SLOT_MUSIC);
	}
}

void Slot::ScheduleWinChipsUpdate( VeFloat32 f32Delta )
{
	schedule(schedule_selector(Slot::WinChipsUpdate),0.01f);
}

void Slot::WinChipsUpdate( VeFloat32 f32Delta )
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
			m_u64NowChips += subtractor;
			SetChips(m_u64WinChips,m_u64NowChips);
		}
		else
		{
			m_u64WinChips -= m_u64WinChips;
			SetChips(m_u64WinChips,m_u64NowChips+m_u64WinChips);
		}
		
	}
	else
	{
		unschedule(schedule_selector(Slot::WinChipsUpdate));
		WIDGET(SlotPollBar, slot_poll_bar);
		slot_poll_bar.SetArrowsMove(true);
		VeUInt32 u32Bet = m_u32Bet;
		m_u32Bet = VE_INFINITE;
		SetBet(u32Bet);
		m_bIsRoll = false;
		UpdateSlot();
	}
}

void Slot::Stop(VeUInt64 u64Win, VeUInt8 u8Left, VeUInt8 u8Middle, VeUInt8 u8Right)
{
	WIDGET(SlotRoll, slot_roll);
	slot_roll.Stop(u8Left, u8Middle, u8Right);
	m_u64WillWin = u64Win;
	m_u64WinChips = 0;
	m_u64NowChips -= GetBet();
	SetChips(m_u64WinChips, m_u64NowChips);
}

void Slot::SetJPChips(VeUInt64 u64JPChips)
{
	WIDGET(JpBar, jp_bar);
	jp_bar.SetNumber(u64JPChips);
}

void Slot::SetSingleBet(VeUInt32 u32SingleBet)
{
	m_u32SingleBet = u32SingleBet;
	WIDGET(SlotNumber, bet_chips);
	bet_chips.SetNumber((m_u32Bet + 1) * m_u32SingleBet);
}

void Slot::Tick(VeFloat32 f32Delta)
{
	if(!m_bIsRoll)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqSlotJP();
	}
}

void Slot::UpdateSlot()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_u64WinChips = 0;
	m_u64NowChips = pkPlayer->GetChips();
	SetChips(m_u64WinChips, m_u64NowChips);
	SetSingleBet(pkPlayer->GetSingleBet());
	pkPlayer->S_ReqSlotJP();
}

void Slot::OnPushNotice(const VeChar8* pcNotice)
{
	WIDGET(NoticeBar, notice_bar);
	notice_bar.Notice(pcNotice, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale));
}

SlotHelp::SlotHelp() : UILayerModalExt<SlotHelp>("slot_help")
{

}

SlotHelp::~SlotHelp()
{
	term();
}

bool SlotHelp::init()
{
	CCSize kSize;
	VeChar8 acBuffer[64];

	CCNode* pkRoot = CCNode::create();
	pkRoot->setPosition(ui_ccp(0.5f, 0.5f));
	addChild(pkRoot);

	CCSpriteBatchNode* pkIconRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("slot", false) + ".png");
	pkIconRoot->setPosition(ui_ccp(0.5f, 0.5f));
	addChild(pkIconRoot);

	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 304.0f * ui_main_scale * g_f32ScaleHD + 16 * g_f32ScaleHD, 268.0f * ui_main_scale * g_f32ScaleHD + 16 * g_f32ScaleHD);
	pkRoot->addChild(pkPanel);

	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("slot_odds_title.png");
	pkTitle->setScale(ui_main_scale);
	pkTitle->setPositionY(VeFloorf(116.0f * ui_main_scale * g_f32ScaleHD));
	pkRoot->addChild(pkTitle);

	UIPanel* pkPanelLeft = UIPanel::createWithCltBlBt("alert_board", 146.0f * ui_main_scale * g_f32ScaleHD, 34.0f * ui_main_scale * g_f32ScaleHD);
	pkPanelLeft->setPositionX(VeFloorf(-75 * ui_main_scale * g_f32ScaleHD));
	pkPanelLeft->setPositionY(VeFloorf(82 * ui_main_scale * g_f32ScaleHD));
	pkRoot->addChild(pkPanelLeft);

	for(VeUInt32 k(0); k < 3; ++k)
	{
		CCSprite* pkIcon = CCSprite::createWithSpriteFrameName("slots_small_item_0.png");
		pkIcon->setScale(ui_main_scale);
		pkIcon->setPositionX(VeFloorf((k * 32.0f - 126) * ui_main_scale * g_f32ScaleHD));
		pkIcon->setPositionY(VeFloorf(82 * ui_main_scale * g_f32ScaleHD));
		pkIconRoot->addChild(pkIcon);
	}

	CCSprite* pkJp = CCSprite::createWithSpriteFrameName("slot_jp.png");
	pkJp->setScale(ui_main_scale);
	pkJp->setPositionX(VeFloorf(-24 * ui_main_scale * g_f32ScaleHD));
	pkJp->setPositionY(VeFloorf(82 * ui_main_scale * g_f32ScaleHD));
	pkIconRoot->addChild(pkJp);

	UIPanel* pkPanelRight = UIPanel::createWithCltBlBt("alert_board", 146.0f * ui_main_scale * g_f32ScaleHD, 34.0f * ui_main_scale * g_f32ScaleHD);
	pkPanelRight->setPositionX(VeFloorf(75 * ui_main_scale * g_f32ScaleHD));
	pkPanelRight->setPositionY(VeFloorf(82 * ui_main_scale * g_f32ScaleHD));
	pkRoot->addChild(pkPanelRight);

	VeSprintf(acBuffer, 64, "%s %.0f", TT("PlusSymbol"), 500.0f);

	CCLabelTTF* pkLabel777 = CCLabelTTF::create(acBuffer, TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	kSize = pkLabel777->getContentSize();
	pkLabel777->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkLabel777->setPositionX(VeFloorf(65 * ui_main_scale * g_f32ScaleHD));
	pkLabel777->setColor(ccc3(0,0,0));
	pkPanelRight->addChild(pkLabel777);

	for(VeUInt32 k(0); k < 3; ++k)
	{
		CCSprite* pkIcon = CCSprite::createWithSpriteFrameName("slots_small_item_1.png");
		pkIcon->setScale(ui_main_scale);
		pkIcon->setPositionX(VeFloorf((k * 26.0f + 22) * ui_main_scale * g_f32ScaleHD));
		pkIcon->setPositionY(VeFloorf(82 * ui_main_scale * g_f32ScaleHD));
		pkIconRoot->addChild(pkIcon);
	}

	VeFloat32 af32Rate[8][3] =
	{
		{200, 10, 3},
		{150, 9, 2.5f},
		{100, 8, 2},
		{40, 7, 1.8f},
		{30, 6, 1.6f},
		{25, 5, 1.4f},
		{20, 4, 1.2f},
		{15, 3.5f, 1}
	};

	CCSprite* apkIcon[3][8][3];

	for(VeUInt32 i(0); i < 3; ++i)
	{
		for(VeUInt32 j(0); j < 8; ++j)
		{
			UIPanel* pkPanelSmall = UIPanel::createWithCltBlBt("alert_board", 96.0f * ui_main_scale * g_f32ScaleHD, 20.0f * ui_main_scale * g_f32ScaleHD);
			pkPanelSmall->setPositionX(VeFloorf((100.0f * i - 100.0f) * ui_main_scale * g_f32ScaleHD));
			pkPanelSmall->setPositionY(VeFloorf((50.0f - 24.0f * j) * ui_main_scale * g_f32ScaleHD));
			pkRoot->addChild(pkPanelSmall);

			VeFloat32 f32Rate = af32Rate[j][i];
			if(f32Rate == VeFloorf(f32Rate))
			{
				VeSprintf(acBuffer, 64, "%s %.0f", TT("PlusSymbol"), f32Rate);
			}
			else
			{
				VeSprintf(acBuffer, 64, "%s %.1f", TT("PlusSymbol"), f32Rate);
			}

			CCLabelTTF* pkLabel = CCLabelTTF::create(acBuffer, TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
			kSize = pkLabel->getContentSize();
			pkLabel->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
			pkLabel->setPositionX(VeFloorf(44 * ui_main_scale * g_f32ScaleHD));
			pkLabel->setColor(ccc3(0,0,0));
			pkPanelSmall->addChild(pkLabel);

			for(VeUInt32 k(0); k < 3; ++k)
			{
				apkIcon[i][j][k] = CCSprite::create();
				apkIcon[i][j][k]->setScale(ui_main_scale);
				apkIcon[i][j][k]->setPositionX(VeFloorf((100.0f * i + k * 18.0f - 136) * ui_main_scale * g_f32ScaleHD));
				apkIcon[i][j][k]->setPositionY(VeFloorf((50.0f - 24.0f * j) * ui_main_scale * g_f32ScaleHD));
			}
		}
	}

	apkIcon[0][0][0]->initWithSpriteFrameName("slots_small_item_2.png");
	apkIcon[0][0][1]->initWithSpriteFrameName("slots_small_item_2.png");
	apkIcon[0][0][2]->initWithSpriteFrameName("slots_small_item_2.png");
	apkIcon[0][1][0]->initWithSpriteFrameName("slots_small_item_3.png");
	apkIcon[0][1][1]->initWithSpriteFrameName("slots_small_item_3.png");
	apkIcon[0][1][2]->initWithSpriteFrameName("slots_small_item_3.png");
	apkIcon[0][2][0]->initWithSpriteFrameName("slots_small_item_4.png");
	apkIcon[0][2][1]->initWithSpriteFrameName("slots_small_item_4.png");
	apkIcon[0][2][2]->initWithSpriteFrameName("slots_small_item_4.png");
	apkIcon[0][3][0]->initWithSpriteFrameName("slots_small_item_5.png");
	apkIcon[0][3][1]->initWithSpriteFrameName("slots_small_item_5.png");
	apkIcon[0][3][2]->initWithSpriteFrameName("slots_small_item_5.png");
	apkIcon[0][4][0]->initWithSpriteFrameName("slots_small_item_6.png");
	apkIcon[0][4][1]->initWithSpriteFrameName("slots_small_item_6.png");
	apkIcon[0][4][2]->initWithSpriteFrameName("slots_small_item_6.png");
	apkIcon[0][5][0]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[0][5][1]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[0][5][2]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[0][6][0]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[0][6][1]->initWithSpriteFrameName("slots_small_item_3.png");
	apkIcon[0][6][2]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[0][7][0]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[0][7][1]->initWithSpriteFrameName("slots_small_item_4.png");
	apkIcon[0][7][2]->initWithSpriteFrameName("slots_small_item_7.png");

	apkIcon[1][0][0]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[1][0][1]->initWithSpriteFrameName("slots_small_item_5.png");
	apkIcon[1][0][2]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[1][1][0]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[1][1][1]->initWithSpriteFrameName("slots_small_item_6.png");
	apkIcon[1][1][2]->initWithSpriteFrameName("slots_small_item_7.png");
	apkIcon[1][2][0]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][2][1]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][2][2]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][3][0]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][3][1]->initWithSpriteFrameName("slots_small_item_3.png");
	apkIcon[1][3][2]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][4][0]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][4][1]->initWithSpriteFrameName("slots_small_item_4.png");
	apkIcon[1][4][2]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][5][0]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][5][1]->initWithSpriteFrameName("slots_small_item_5.png");
	apkIcon[1][5][2]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][6][0]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][6][1]->initWithSpriteFrameName("slots_small_item_6.png");
	apkIcon[1][6][2]->initWithSpriteFrameName("slots_small_item_8.png");
	apkIcon[1][7][0]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[1][7][1]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[1][7][2]->initWithSpriteFrameName("slots_small_item_9.png");

	apkIcon[2][0][0]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][0][1]->initWithSpriteFrameName("slots_small_item_3.png");
	apkIcon[2][0][2]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][1][0]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][1][1]->initWithSpriteFrameName("slots_small_item_4.png");
	apkIcon[2][1][2]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][2][0]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][2][1]->initWithSpriteFrameName("slots_small_item_5.png");
	apkIcon[2][2][2]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][3][0]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][3][1]->initWithSpriteFrameName("slots_small_item_6.png");
	apkIcon[2][3][2]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][4][0]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][4][1]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][4][2]->initWithSpriteFrameName("slots_small_item_9.png");
	apkIcon[2][5][0]->initWithSpriteFrameName("slots_small_item_10.png");
	apkIcon[2][5][1]->initWithSpriteFrameName("slots_small_item_10.png");
	apkIcon[2][5][2]->initWithSpriteFrameName("slots_small_item_10.png");
	apkIcon[2][6][0]->initWithSpriteFrameName("slots_small_item_10.png");
	apkIcon[2][6][1]->initWithSpriteFrameName("slots_small_item_10.png");
	apkIcon[2][6][2]->initWithSpriteFrameName("slots_small_item_11.png");
	apkIcon[2][7][0]->initWithSpriteFrameName("slots_small_item_10.png");
	apkIcon[2][7][1]->initWithSpriteFrameName("slots_small_item_11.png");
	apkIcon[2][7][2]->initWithSpriteFrameName("slots_small_item_11.png");

	for(VeUInt32 i(0); i < 3; ++i)
	{
		for(VeUInt32 j(0); j < 8; ++j)
		{
			for(VeUInt32 k(0); k < 3; ++k)
			{
				pkIconRoot->addChild(apkIcon[i][j][k]);
			}
		}
	}

	for(VeUInt32 k(0); k < 3; ++k)
	{
		apkIcon[2][4][k]->setPositionY(VeFloorf((46.0f - 24.0f * 4) * ui_main_scale * g_f32ScaleHD));

		CCSprite* pkIconAdd = CCSprite::createWithSpriteFrameName("slots_small_item_11.png");
		pkIconAdd->setScale(ui_main_scale);
		pkIconAdd->setPositionX(VeFloorf((100.0f * 2 + k * 18.0f - 136) * ui_main_scale * g_f32ScaleHD));
		pkIconAdd->setPositionY(VeFloorf((53.0f - 24.0f * 4) * ui_main_scale * g_f32ScaleHD));
		pkIconRoot->addChild(pkIconAdd);
	}

	return UILayerModalExt<SlotHelp>::init();
}

bool SlotHelp::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(!UILayer::ProcessTouch(eType, pkTouch))
	{
		g_pLayerManager->Back();
	}
	return true;
}
