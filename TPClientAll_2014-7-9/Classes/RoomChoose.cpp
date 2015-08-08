#include "RoomChoose.h"
#include "TexasPoker.h"
#include "SoundSystem.h"

USING_NS_CC;

RoomBar::RoomBar(VeUInt32 u32Index)
	: UIButton("room_0_n.png", "room_0_p.png", "room_0_n.png")
	, m_u32Index(u32Index)
{
	CCSpriteBatchNode* pkUi0 = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	addChild(pkUi0, 5);

	SetPressedSound(VE_INFINITE);
	CCSize kSize = getContentSize();
	kSize.height += 24.0f * g_f32ScaleHD;
	setContentSize(kSize);

	m_pkHot = CCSprite::createWithSpriteFrameName("hot_icon.png");
	m_pkHot->setPosition(ccp(-62.0f * g_f32ScaleHD, 34.0f * g_f32ScaleHD));
	pkUi0->addChild(m_pkHot);

	m_pkType = CCSprite::create();
	addChild(m_pkType, 5);

	CCSprite* pkMaxChipsCarry = CCSprite::createWithSpriteFrameName("max_chips_carry.png");
	pkMaxChipsCarry->setAnchorPoint(ccp(0.0f, 0.5f));
	pkMaxChipsCarry->setPosition(ccp(-64.0f * g_f32ScaleHD, -31.0f * g_f32ScaleHD));
	addChild(pkMaxChipsCarry, 5);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chips.png");
	pkChips->setPosition(ccp(2.0f * g_f32ScaleHD, -30.0f * g_f32ScaleHD));
	pkChips->setScale(0.6f);
	pkUi0->addChild(pkChips, 5);

	m_pkMaxNumber = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkMaxNumber->setScale(1.0f / ui_main_scale);
	m_pkMaxNumber->setPosition(ccp(64.0f * g_f32ScaleHD, -31.0f * g_f32ScaleHD));
	addChild(m_pkMaxNumber, 5);

	m_pkSB = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkSB->setScale(1.0f / ui_main_scale);
	m_pkSB->setPosition(ccp(38.0f * g_f32ScaleHD, 9.0f * g_f32ScaleHD));
	addChild(m_pkSB, 5);

	m_pkBB = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkBB->setScale(1.0f / ui_main_scale);
	m_pkBB->setPosition(ccp(38.0f * g_f32ScaleHD, 0.0f * g_f32ScaleHD));
	addChild(m_pkBB, 5);

	m_pkSB->setColor(ccc3(200, 200, 200));
	m_pkBB->setColor(ccc3(200, 200, 200));

	m_pkLock = CCSprite::createWithSpriteFrameName("room_lock.png");
	addChild(m_pkLock, 6);

}

RoomBar::~RoomBar()
{

}

void RoomBar::Init(VeUInt32 u32MaxChips, VeUInt32 u32SB, VeUInt32 u32BB, Type eType, bool bVip, bool bHot)
{
	CCSize kSize;
	m_pkMaxNumber->setString(NTL(u32MaxChips, 6));
	kSize = m_pkMaxNumber->getContentSize();
	m_pkMaxNumber->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkSB->setString(NTL(u32SB, 5));
	kSize = m_pkSB->getContentSize();
	m_pkSB->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkBB->setString(NTL(u32BB, 5));
	kSize = m_pkBB->getContentSize();
	m_pkBB->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));

	VeChar8 acBuffer[64];
	VE_ASSERT(eType < TYPE_MAX);
	VeSprintf(acBuffer, 64, "room_type_%d.png", eType);
	m_pkType->initWithSpriteFrameName(acBuffer);
	m_pkType->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkType->setPosition(ccp(-68.0f * g_f32ScaleHD, 10.0f * g_f32ScaleHD));

	if(bVip)
	{
		((CCSprite*)m_pkNormal)->initWithSpriteFrameName("room_1_n.png");
		((CCSprite*)m_pkPressed)->initWithSpriteFrameName("room_1_p.png");
		((CCSprite*)m_pkDisabled)->initWithSpriteFrameName("room_1_n.png");
	}
	else
	{
		((CCSprite*)m_pkNormal)->initWithSpriteFrameName("room_0_n.png");
		((CCSprite*)m_pkPressed)->initWithSpriteFrameName("room_0_p.png");
		((CCSprite*)m_pkDisabled)->initWithSpriteFrameName("room_0_n.png");
	}

	m_pkHot->setVisible(bHot);
}

VeUInt32 RoomBar::GetIndex()
{
	return m_u32Index;
}

void RoomBar::SetEnable(bool bEnable, bool bHot)
{
	m_pkLock->setVisible(!bEnable);
	m_pkHot->setVisible(bEnable && bHot);
	UIButton::SetEnable(bEnable);
}

RoomChoose::RoomChoose()
	: UIScrollView(320.0f * ui_main_scale * g_f32ScaleHD, 231.0f * ui_main_scale * g_f32ScaleHD,
	"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale)
{
	setPosition(ccp(ui_w(0.5f), VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 240 * g_f32ScaleHD * ui_main_scale)));
	setAnchorPoint(ccp(0.5f,1.0f));
	SetScrollBarColor(ccc3(56, 79, 124));
	SetScrollBarOpacity(128);
	UpdateBounding();
	m_u32Pointer = 0;
	m_kRoomBarArray.Reserve(128);
	UIInitDelegate(RoomChoose, OnFastEnter);

	VeFloat32 f32Width = getContentSize().width;
	m_pkQuickStart = VE_NEW UIButton("room_2_n.png", "room_2_p.png", "room_2_n.png");
	m_pkQuickStart->setScale(ui_main_scale);
	m_pkQuickStart->setPosition(ccp(VeFloorf(f32Width * 0.5f - 76.0f * g_f32ScaleHD * ui_main_scale), 0));
	AddChild(m_pkQuickStart);
	UILinkDelegate(m_pkQuickStart, OnFastEnter);
	m_pkQuickStart->SetPressedSound(VE_INFINITE);
	CCSprite* pkEnterFast = CCSprite::createWithSpriteFrameName("fast_enter.png");
	pkEnterFast->setPositionY(-31 * g_f32ScaleHD);
	m_pkQuickStart->addChild(pkEnterFast, 4);
	{
		CCSprite* pkLeft = CCSprite::createWithSpriteFrameName("fast_0.png");
		pkLeft->setPosition(ccp(-52 * g_f32ScaleHD, -31 * g_f32ScaleHD));
		m_pkQuickStart->addChild(pkLeft, 4);
		CCAnimation* pkAniLeft = CCAnimation::create();
		pkAniLeft->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_0.png"));
		pkAniLeft->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_1.png"));
		pkAniLeft->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_2.png"));
		pkAniLeft->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_3.png"));
		pkAniLeft->setDelayPerUnit(0.2f);
		pkLeft->runAction(CCRepeatForever::create(CCAnimate::create(pkAniLeft)));
		CCSprite* pkRight = CCSprite::createWithSpriteFrameName("fast_0.png");
		pkRight->setPosition(ccp(52 * g_f32ScaleHD, -31 * g_f32ScaleHD));
		pkRight->setScaleX(-1);
		m_pkQuickStart->addChild(pkRight, 4);
		CCAnimation* pkAniRight = CCAnimation::create();
		pkAniRight->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_0.png"));
		pkAniRight->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_1.png"));
		pkAniRight->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_2.png"));
		pkAniRight->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("fast_3.png"));
		pkAniRight->setDelayPerUnit(0.2f);
		pkRight->runAction(CCRepeatForever::create(CCAnimate::create(pkAniRight)));
	}

	m_pkVenusCup = VE_NEW UIButton("room_3_n.png", "room_3_p.png", "room_3_n.png");
	m_pkVenusCup->setScale(ui_main_scale);
	m_pkVenusCup->setPosition(ccp(VeFloorf(f32Width * 0.5f + 76.0f * g_f32ScaleHD * ui_main_scale), 0));
	AddChild(m_pkVenusCup);
	m_pkVenusCup->SetEnable(false);
	CCSprite* pkEnterCup = CCSprite::createWithSpriteFrameName("fast_Match.png");
	pkEnterCup->setPositionY(-31 * g_f32ScaleHD);
	m_pkVenusCup->addChild(pkEnterCup, 4);
	{
		CCSprite* pkLeft = CCSprite::createWithSpriteFrameName("fast_0.png");
		pkLeft->setPosition(ccp(-52 * g_f32ScaleHD, -31 * g_f32ScaleHD));
		m_pkVenusCup->addChild(pkLeft, 4);
		CCSprite* pkRight = CCSprite::createWithSpriteFrameName("fast_0.png");
		pkRight->setPosition(ccp(52 * g_f32ScaleHD, -31 * g_f32ScaleHD));
		pkRight->setScaleX(-1);
		m_pkVenusCup->addChild(pkRight, 4);
		CCSprite* pkLock = CCSprite::createWithSpriteFrameName("room_lock.png");
		m_pkVenusCup->addChild(pkLock, 4);
	}
}

RoomChoose::~RoomChoose()
{

}

VeUInt32 RoomChoose::AddRoomBar(VeUInt32 u32MaxChips, VeUInt32 u32SB, VeUInt32 u32BB, RoomBar::Type eType, bool bVip, bool bHot)
{
	if(m_u32Pointer >= m_kRoomBarArray.Size())
	{
		VE_ASSERT(m_u32Pointer == m_kRoomBarArray.Size());
		RoomBar* pkBar = VE_NEW RoomBar(m_u32Pointer);
		VeFloat32 f32Width = getContentSize().width;
		VeUInt32 u32Flag = m_u32Pointer & 1;
		VeUInt32 u32Row = (m_u32Pointer >> 1) + 1;
		if(u32Flag)
		{
			pkBar->setPosition(ccp(VeFloorf(f32Width * 0.5f + 76.0f * g_f32ScaleHD * ui_main_scale),
				-VeFloorf(VeFloat32(u32Row) * 96 * g_f32ScaleHD * ui_main_scale)));
		}
		else
		{
			pkBar->setPosition(ccp(VeFloorf(f32Width * 0.5f - 76.0f * g_f32ScaleHD * ui_main_scale),
				-VeFloorf(VeFloat32(u32Row) * 96 * g_f32ScaleHD * ui_main_scale)));
		}
		pkBar->setScale(ui_main_scale);
		AddChild(pkBar);
		pkBar->SetVisible(false);
		m_kRoomBarArray.Resize(m_kRoomBarArray.Size() + 1);
		Item& kItem = m_kRoomBarArray.Back();
		kItem.m_pkBar = pkBar;
		kItem.m_kDelegate.Set(this, &RoomChoose::OnClicked);
		kItem.m_pkBar->RegisterListener(kItem.m_kDelegate);
	}
	
	RoomBar* pkBar = m_kRoomBarArray[m_u32Pointer++].m_pkBar;
	pkBar->Init(u32MaxChips, u32SB, u32BB, eType, bVip, bHot);
	pkBar->SetVisible(true);
	UpdateBounding();
	
	return m_u32Pointer;
}

void RoomChoose::SetEnable(VeUInt32 u32Index, bool bEnable, bool bHot)
{
	VE_ASSERT(u32Index < m_u32Pointer);
	RoomBar* pkBar = m_kRoomBarArray[u32Index].m_pkBar;
	pkBar->SetEnable(bEnable, bHot);
}

void RoomChoose::ClearAll()
{
	m_u32Pointer = 0;
	for(VeVector<Item>::iterator it = m_kRoomBarArray.Begin(); it != m_kRoomBarArray.End(); ++it)
	{
		(it)->m_pkBar->SetVisible(false);
	}
	UpdateBounding();
}

void RoomChoose::OnClicked(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == RELEASED)
	{
		g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
		VeUInt32 u32State = ((RoomBar&)kWidget).GetIndex() + 1;
		u32State |= 0x80000000;
		m_kEvent.Callback(*this, u32State);
	}
}

UIImplementDelegate(RoomChoose, OnFastEnter, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
		m_kEvent.Callback(*this, 0x80000000);
	}
}

RoomChoose::Item::Item()
{
	m_pkBar = NULL;
}
