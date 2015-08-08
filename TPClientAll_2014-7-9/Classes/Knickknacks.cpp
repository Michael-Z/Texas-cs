#include "Knickknacks.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "UIScrollView.h"
#include "SoundSystem.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"
#include "NativeInterface.h"
#include "EventList.h"

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
#endif

USING_NS_CC;

AllOnline::AllOnline()
	: m_u64Number(0)
{
	CCSprite* pkAllOL = CCSprite::createWithSpriteFrameName("all_online.png");
	pkAllOL->setAnchorPoint(ccp(0.0f, 0.0f));
	pkAllOL->setScale(ui_main_scale);
	addChild(pkAllOL);

	CCLabelTTF* pkAll = CCLabelTTF::create(TT("AllOnline"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	pkAll->setAnchorPoint(ccp(0.0f, 0.0f));
	pkAll->setPosition(ccp(VeFloorf(24.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(15.0f * g_f32ScaleHD * ui_main_scale)));
	addChild(pkAll, 1);

	m_pkAllOnlineNum = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkAllOnlineNum->setAnchorPoint(ccp(1.0f, 0.0f));
	m_pkAllOnlineNum->setPositionX(VeFloorf((pkAllOL->getContentSize().width - 6.0f * g_f32ScaleHD) * ui_main_scale));
	m_pkAllOnlineNum->setPositionY(VeFloorf(15.0f * g_f32ScaleHD * ui_main_scale));
	addChild(m_pkAllOnlineNum, 1);

	UpdateNumber();
}

AllOnline::~AllOnline()
{

}

void AllOnline::SetNumber(VeUInt64 u64Num)
{
	if(m_u64Number != u64Num)
	{
		m_u64Number = u64Num;
		UpdateNumber();
	}
}

void AllOnline::UpdateNumber()
{
	m_pkAllOnlineNum->setString(NT(m_u64Number, 9999999));
}

NameLabel::NameLabel()
{
	m_pkName = CCLabelTTF::create("", TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
	m_pkName->setAnchorPoint(ccp(0.0f, 0.0f));
	addChild(m_pkName);
}

NameLabel::~NameLabel()
{

}

void NameLabel::SetName(const VeChar8* pcName)
{
	m_pkName->setString(pcName);
	VeFloat32 f32Height = m_pkName->getContentSize().height;
	m_pkName->setPositionY(-VeFloorf(f32Height * 0.5f));
}

HeadArea::HeadArea()
{
	CCSprite* pkHeadArea = CCSprite::createWithSpriteFrameName("head_area.png");
	pkHeadArea->setScale(ui_main_scale);
	addChild(pkHeadArea, 0);
	m_pkIcon = CCSprite::create();
	m_pkIcon->setScale(ui_main_scale);
	addChild(m_pkIcon, 1);

	m_pkSex = CCSprite::create();
	m_pkSex->setPositionX(VeFloorf(22.0f * g_f32ScaleHD * ui_main_scale));
	m_pkSex->setPositionY(VeFloorf(22.0f * g_f32ScaleHD * ui_main_scale));
	m_pkSex->setScale(ui_main_scale);
	addChild(m_pkSex, 1);

	m_pkVip = CCSprite::createWithSpriteFrameName("vip_title.png");
	m_pkVip->setPositionX(VeFloorf(-19.0f * g_f32ScaleHD * ui_main_scale));
	m_pkVip->setPositionY(VeFloorf(25.0f * g_f32ScaleHD * ui_main_scale));
	m_pkVip->setScale(ui_main_scale);
	addChild(m_pkVip, 1);
}

HeadArea::~HeadArea()
{

}

void HeadArea::Set(VeUInt32 u32Index, bool bMale, bool bVip)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "head_%d.png", u32Index);
	m_pkIcon->initWithSpriteFrameName(acBuffer);

	if(bMale)
	{
		m_pkSex->initWithSpriteFrameName("sex_m.png");
	}
	else
	{
		m_pkSex->initWithSpriteFrameName("sex_f.png");
	}

	m_pkVip->setVisible(bVip);
}

ChipsBar::ChipsBar()
{
	CCSprite* pkChipsArea = CCSprite::createWithSpriteFrameName("chips_area.png");
	pkChipsArea->setAnchorPoint(ccp(0.0f, 0.5f));
	pkChipsArea->setScale(ui_main_scale);
	addChild(pkChipsArea, 0);

	m_pkChips = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkChips->setAnchorPoint(ccp(1.0f, 0.0f));
	m_pkChips->setPositionX(VeFloorf((pkChipsArea->getContentSize().width - 6.0f * g_f32ScaleHD) * ui_main_scale));
	addChild(m_pkChips, 0);
    
    m_u32Number = 0;
    UpdateNumber();
}

ChipsBar::~ChipsBar()
{

}

void ChipsBar::SetNumber(VeUInt32 u32Num)
{
	if(m_u32Number != u32Num)
	{
		m_u32Number = u32Num;
		UpdateNumber();
	}
}

void ChipsBar::UpdateNumber()
{
	m_pkChips->setString(NTL(m_u32Number, 7));
	VeFloat32 f32Height = m_pkChips->getContentSize().height;
	m_pkChips->setPositionY(-VeFloorf(f32Height * 0.5f));
}

VeUInt32 ChipsBar::GetNumber()
{
	return atoi(m_pkChips->getString());
}

GoldBar::GoldBar()
{
	CCSprite* pkGoldArea = CCSprite::createWithSpriteFrameName("gold_area.png");
	pkGoldArea->setAnchorPoint(ccp(0.0f, 0.5f));
	pkGoldArea->setScale(ui_main_scale);
	addChild(pkGoldArea, 0);

	m_pkGold = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkGold->setAnchorPoint(ccp(1.0f, 0.0f));
	m_pkGold->setPositionX(VeFloorf((pkGoldArea->getContentSize().width - 6.0f * g_f32ScaleHD) * ui_main_scale));
	addChild(m_pkGold, 0);
    
    m_u32Number = 0;
    UpdateNumber();
}

GoldBar::~GoldBar()
{

}

void GoldBar::SetNumber(VeUInt32 u32Num)
{
	if(m_u32Number != u32Num)
	{
		m_u32Number = u32Num;
		UpdateNumber();
	}
}

void GoldBar::UpdateNumber()
{
	m_pkGold->setString(NTL(m_u32Number, 7));
	VeFloat32 f32Height = m_pkGold->getContentSize().height;
	m_pkGold->setPositionY(-VeFloorf(f32Height * 0.5f));
}

NoticeBar::NoticeBar()
{
	CCSpriteBatchNode* pkUi0 = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	addChild(pkUi0, 0);

	CCSprite* pkBar = CCSprite::createWithSpriteFrameName("notice.png");
	pkBar->setPositionX(ui_w(0.5f));
	pkBar->setScaleX(UILayer::Width());
	pkBar->setScaleY(ui_main_scale);
	pkUi0->addChild(pkBar, 0);
	CCSprite* pkMs = CCSprite::createWithSpriteFrameName("ms.png");
	pkMs->setPositionX(VeFloorf(UILayer::Width() * 1.0f - 14.0f * g_f32ScaleHD * ui_main_scale));
	pkMs->setScale(ui_main_scale);
	pkUi0->addChild(pkMs, 5);
}

NoticeBar::~NoticeBar()
{
	m_kUsedList.BeginIterator();
	while(!m_kUsedList.IsEnd())
	{
		Content* pkContent = m_kUsedList.GetIterNode()->m_tContent;
		m_kUsedList.Next();
		VE_SAFE_DELETE(pkContent);
	}
	m_kFreeList.BeginIterator();
	while(!m_kFreeList.IsEnd())
	{
		Content* pkContent = m_kFreeList.GetIterNode()->m_tContent;
		m_kFreeList.Next();
		VE_SAFE_DELETE(pkContent);
	}
}

void NoticeBar::Notice(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	if(m_kUsedList.Empty())
	{
		schedule(schedule_selector(NoticeBar::Update), 0.0f);
	}
	else
	{
		Content* pkLast = m_kUsedList.GetLastNode()->m_tContent;
		if(m_kCachedTexts.Size() || (!pkLast->IsShown()))
		{
			m_kCachedTexts.PushBack(VePair< VeStringA, VePair<VeFloat32,VeFloat32> >(
				pcContent, VePair<VeFloat32,VeFloat32>(f32Speed, f32Space)));
			return;
		}
	}
	_Notice(pcContent, f32Speed, f32Space);
}

void NoticeBar::Update(VeFloat32 f32Delta)
{
	if(m_kUsedList.Size())
	{
		m_kUsedList.BeginIterator();
		while(!m_kUsedList.IsEnd())
		{
			Content* pkContent = m_kUsedList.GetIterNode()->m_tContent;
			m_kUsedList.Next();
			VE_ASSERT(pkContent);
			if(pkContent->Update(f32Delta))
			{
				m_kFreeList.AttachBack(*pkContent);
			}
		}
	}
	if(m_kUsedList.Empty())
	{
		unschedule(schedule_selector(NoticeBar::Update));
	}
	else if(m_kCachedTexts.Size())
	{
		Content* pkLast = m_kUsedList.GetLastNode()->m_tContent;
		if(pkLast->IsShown())
		{
			VeList< VePair< VeStringA, VePair<VeFloat32,VeFloat32> > >::iterator it = m_kCachedTexts.Begin();
			_Notice(it->m_tFirst, it->m_tSecond.m_tFirst, it->m_tSecond.m_tSecond);
			m_kCachedTexts.PopFront();
		}
	}
}

void NoticeBar::_Notice(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	if(m_kFreeList.Empty())
	{
		Content* pkContent = VE_NEW Content(pcContent, this, f32Speed, f32Space);
		m_kUsedList.AttachBack(*pkContent);
	}
	else
	{
		Content* pkContent = m_kFreeList.GetLastNode()->m_tContent;
		pkContent->Restart(pcContent, f32Speed, f32Space);
		m_kUsedList.AttachBack(*pkContent);
	}
}

NoticeBar::Content::Content(const VeChar8* pcContent, NoticeBar* pkParent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	m_tContent = this;
	m_pkLabel = CCLabelTTF::create(pcContent, TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	m_pkLabel->setAnchorPoint(ccp(0.0f, 0.0f));
	m_pkLabel->setPositionY(-VeFloorf(m_pkLabel->getContentSize().height * 0.5f));
	m_f32Speed = f32Speed;
	m_f32Space = f32Space;
	m_f32PosY = 0;
	pkParent->addChild(m_pkLabel, 0);
	UpdatePos();
}

void NoticeBar::Content::UpdatePos()
{
	m_pkLabel->setPositionX(VeFloorf(UILayer::Width() - m_f32PosY));
}

bool NoticeBar::Content::IsShown()
{
	return m_f32PosY >= (m_pkLabel->getContentSize().width + m_f32Space);
}

void NoticeBar::Content::Restart(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	m_pkLabel->setVisible(true);
	m_pkLabel->setString(pcContent);
	m_pkLabel->setPositionY(-VeFloorf(m_pkLabel->getContentSize().height * 0.5f));
	m_f32Speed = f32Speed;
	m_f32Space = f32Space;
	m_f32PosY = 0;
	UpdatePos();
}

bool NoticeBar::Content::Update(VeFloat32 f32Delta)
{
	m_f32PosY += f32Delta * m_f32Speed;
	if(m_f32PosY >= (UILayer::Width() + m_pkLabel->getContentSize().width))
	{
		m_pkLabel->setVisible(false);
		return true;
	}
	else
	{
		UpdatePos();
		return false;
	}
}

HeadChatBar::HeadChatBar()
{
}

HeadChatBar::~HeadChatBar()
{
	m_kUsedList.BeginIterator();
	while(!m_kUsedList.IsEnd())
	{
		ChatContent* pkContent = m_kUsedList.GetIterNode()->m_tContent;
		m_kUsedList.Next();
		VE_SAFE_DELETE(pkContent);
	}
	m_kFreeList.BeginIterator();
	while(!m_kFreeList.IsEnd())
	{
		ChatContent* pkContent = m_kFreeList.GetIterNode()->m_tContent;
		m_kFreeList.Next();
		VE_SAFE_DELETE(pkContent);
	}
}

void HeadChatBar::Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	if(m_kUsedList.Empty())
	{
		schedule(schedule_selector(HeadChatBar::Update), 0.0f);
	}
	else
	{
		ChatContent* pkLast = m_kUsedList.GetLastNode()->m_tContent;
		if(pkLast != NULL)
		{
			pkLast->Restart(pcContent, f32Speed, f32Space);
			return;
		}
		//if(m_kCachedTexts.Size() || (!pkLast->IsShown()))
		//{
		//	m_kCachedTexts.PushBack(VePair< VeStringA, VePair<VeFloat32,VeFloat32> >(
		//		pcContent, VePair<VeFloat32,VeFloat32>(f32Speed, f32Space)));
		//	return;
		//}
	}
	_Chat(pcContent, f32Speed, f32Space);
}

void HeadChatBar::Update(VeFloat32 f32Delta)
{
	if(m_kUsedList.Size())
	{
		m_kUsedList.BeginIterator();
		while(!m_kUsedList.IsEnd())
		{
			ChatContent* pkContent = m_kUsedList.GetIterNode()->m_tContent;
			m_kUsedList.Next();
			VE_ASSERT(pkContent);
			if(pkContent->Update(f32Delta))
			{
				m_kFreeList.AttachBack(*pkContent);
			}
		}
	}
	if(m_kUsedList.Empty())
	{
		unschedule(schedule_selector(HeadChatBar::Update));
	}
	else if(m_kCachedTexts.Size())
	{
		ChatContent* pkLast = m_kUsedList.GetLastNode()->m_tContent;
		if(pkLast->IsShown())
		{
			VeList< VePair< VeStringA, VePair<VeFloat32,VeFloat32> > >::iterator it = m_kCachedTexts.Begin();
			_Chat(it->m_tFirst, it->m_tSecond.m_tFirst, it->m_tSecond.m_tSecond);
			m_kCachedTexts.PopFront();
		}
	}
}

void HeadChatBar::_Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	if(m_kFreeList.Empty())
	{
		ChatContent* pkContent = VE_NEW ChatContent(pcContent, this, f32Speed, f32Space);
		m_kUsedList.AttachBack(*pkContent);
	}
	else
	{
		ChatContent* pkContent = m_kFreeList.GetLastNode()->m_tContent;
		pkContent->Restart(pcContent, f32Speed, f32Space);
		m_kUsedList.AttachBack(*pkContent);
	}
}

HeadChatBar::ChatContent::ChatContent(const VeChar8* pcContent, HeadChatBar* pkParent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	m_tContent = this;

	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 128.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
	pkParent->addChild(m_pkPanel);
	m_pkPanel->setPosition(ui_w(0.0f), ui_h(0.0f));

	m_pkLabel = CCLabelTTF::create(pcContent, TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale), CCSize(110.0f * g_f32ScaleHD * ui_main_scale, 50.0f * g_f32ScaleHD * ui_main_scale), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
	//m_pkLabel->setAnchorPoint(ccp(0.0f, 0.0f));
	//m_pkLabel->setPosition(ccp(ui_w(0.0f), -VeFloorf(m_pkLabel->getContentSize().height * 0.5f)));

	m_pkIcon = CCSprite::create();
	m_pkIcon->setScale(ui_main_scale);
	pkParent->addChild(m_pkIcon, 0);

	m_f32Speed = f32Speed;
	m_f32Space = f32Space;
	m_f32Time = 0;
	m_f32TimeDuration = f32Space * 0.1f;
	pkParent->addChild(m_pkLabel, 0);
	UpdatePos();
	
	ResolveContent(pcContent);
}

void HeadChatBar::ChatContent::UpdatePos()
{
	//m_pkLabel->setPositionX(VeFloorf(UILayer::Width() - m_f32PosY));
}

bool HeadChatBar::ChatContent::IsShown()
{
	return m_f32Time >= m_f32TimeDuration;
}

void HeadChatBar::ChatContent::Restart(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	//m_pkLabel->setPositionY(-VeFloorf(m_pkLabel->getContentSize().height * 0.5f));
	m_f32Speed = f32Speed;
	m_f32Space = f32Space;
	m_f32Time = 0;
	m_f32TimeDuration = f32Space * 0.1f;
	UpdatePos();

	ResolveContent(pcContent);
}

bool HeadChatBar::ChatContent::Update(VeFloat32 f32Delta)
{
	m_f32Time += f32Delta;
	if(m_f32Time >= m_f32TimeDuration)
	{
		m_pkLabel->setVisible(false);
		m_pkPanel->setVisible(false);
		m_pkIcon->setVisible(false);
		return true;
	}
	else
	{
		UpdatePos();
		return false;
	}
}

void HeadChatBar::ChatContent::ResolveContent(const VeChar8* pcContent)
{
	VeString strContent = pcContent;
	if(strContent.GetAt(0) == '#')
	{
		VeString strContentTemp = strContent.GetSubstring(1, strContent.Length());
		VeUInt32 u32Index = 0;
		strContentTemp.ToUInt(u32Index);
		SetIcon(u32Index);
	}
	else
	{
		SetText(pcContent);
	}
}

void HeadChatBar::ChatContent::SetIcon(VeUInt32 u32Index)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "expression_%d.png", u32Index);
	m_pkIcon->initWithSpriteFrameName(acBuffer);

	m_pkIcon->setVisible(true);
	m_pkLabel->setVisible(false);
	m_pkPanel->setVisible(false);
}

void HeadChatBar::ChatContent::SetText(const VeChar8* pcContent)
{
	m_pkLabel->setString(pcContent);

	m_pkIcon->setVisible(false);
	m_pkLabel->setVisible(true);
	m_pkPanel->setVisible(true);
}

Slider::Slider() : UISlider("handle.png", 20.0f * g_f32ScaleHD, "bar.png", "bar_light.png")
{
	setScale(ui_main_scale);

	CCSpriteBatchNode* pkUi0 = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	addChild(pkUi0, 0);

	CCSprite* pkMinus = CCSprite::createWithSpriteFrameName("text_-.png");
	pkMinus->setPositionX(-42.0f * g_f32ScaleHD);
	pkUi0->addChild(pkMinus, 0);

	CCSprite* pkPlus = CCSprite::createWithSpriteFrameName("text_+.png");
	pkPlus->setPositionX(42.0f * g_f32ScaleHD);
	pkUi0->addChild(pkPlus, 0);
}

FillSlider::FillSlider() : UISliderV("bet_slider_bean.png", 50.0f * g_f32ScaleHD)
{
	setScale(ui_main_scale);
	CCSprite* pkBarL = CCSprite::createWithSpriteFrameName("bet_slider.png");
	pkBarL->setAnchorPoint(ccp(1.0f, 170.0f / 344.0f));
	addChild(pkBarL, 0);
	CCSprite* pkBarR = CCSprite::createWithSpriteFrameName("bet_slider.png");
	pkBarR->setAnchorPoint(ccp(1.0f, 170.0f / 344.0f));
	pkBarR->setScaleX(-1.0f);
	addChild(pkBarR, 0);
	m_pkBarLight = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("bet_slider_light.png"));
	m_pkBarLight->setType(kCCProgressTimerTypeBar);
	m_pkBarLight->setMidpoint(ccp(0,0));
	m_pkBarLight->setBarChangeRate(ccp(0, 1));
	addChild(m_pkBarLight, 0);
	m_f32BarHeight = m_pkBarLight->getContentSize().height;
	Update();
}

ChipsCarrySlider::ChipsCarrySlider() : UISlider("chips_carry_bean.png", 72.0f * g_f32ScaleHD)
{
	setScale(ui_main_scale);
	if(g_f32ScaleHD > 1.0f)
	{
		m_pkHandle->setAnchorPoint(ccp(0.5f, 25.0f / 64.0f));
	}
	else
	{
		m_pkHandle->setAnchorPoint(ccp(0.5f, 12.0f / 32.0f));
	}
	
	UIBarH* pkBar = UIBarH::createWithEl("chips_carry_bar", 188.0f * g_f32ScaleHD);
	addChild(pkBar);
	pkBar->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkBarLight = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("chips_carry_bar_light.png"));
	m_pkBarLight->setType(kCCProgressTimerTypeBar);
	m_pkBarLight->setMidpoint(ccp(0,0));
	m_pkBarLight->setBarChangeRate(ccp(1, 0));
	addChild(m_pkBarLight, 0);
	m_f32BarWidth = m_pkBarLight->getContentSize().width;
	
	m_pkBubble = UIBarH::createWithEl("chips_carry", 100 * g_f32ScaleHD);
	m_pkBubble->setPositionY(18 * g_f32ScaleHD);
	addChild(m_pkBubble, 5);

	m_pkCurrent = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkCurrent->setPositionY(9 * g_f32ScaleHD);
	m_pkCurrent->setScale(1.0f / ui_main_scale);
	m_pkBubble->addChild(m_pkCurrent);

	m_u32Min = 0;
	m_u32Max = 0;
	m_u32Unit = 1;

	Update();
}

void ChipsCarrySlider::UpdateData()
{
	m_u32Current = (VeUInt32)VeLerp(GetValue(), VeFloat64(m_u32Min), VeFloat64(m_u32Max));
	if(m_u32Unit) m_u32Current -= m_u32Current % m_u32Unit;
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "%d", m_u32Current);
	m_pkCurrent->setString(acBuffer);
	const CCSize& kSize = m_pkCurrent->getContentSize();
	m_pkCurrent->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkBubble->SetLength(kSize.width + 24 * g_f32ScaleHD);
	m_pkBubble->setPositionX(m_pkHandle->getPositionX());
}

void ChipsCarrySlider::Update()
{
	UISlider::Update();
	UpdateData();
}

void ChipsCarrySlider::SetRange(VeUInt32 u32Min, VeUInt32 u32Max, VeUInt32 u32Unit)
{
	m_u32Min = u32Min;
	m_u32Max = u32Max;
	m_u32Unit = u32Unit;
	UpdateData();
}

VeUInt32 ChipsCarrySlider::GetCurrent()
{
	return m_u32Current;
}

Perspective::Perspective()
{
	CCSize kScreenSize = g_pkDirector->getVisibleSize();
	m_kTrans.z = 500.0f * ui_main_scale * g_f32ScaleHD;
	m_kTrans.w = kScreenSize.height / kScreenSize.width;
	m_kRotate.FromAngleAxis(0, VeVector3D::UNIT_Y);
}

Perspective::~Perspective()
{

}

Perspective* Perspective::create(const char *pszFileName)
{
	Perspective* pRet = new Perspective();
	if(pRet && pRet->initWithSpriteFrameName(pszFileName))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

bool Perspective::initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated)
{
	if(CCSprite::initWithTexture(pTexture, rect, rotated)) 
	{
		setShaderProgram(g_pkGame->GetShader(SHADER_PERSPECTIVE_SPRITE));
		return true;
	}

	return false;
}

void Perspective::draw()
{
	ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex );
	ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

	getShaderProgram()->use();
	getShaderProgram()->setUniformsForBuiltins();

	getShaderProgram()->setUniformLocationWith4fv(
		g_pkGame->GetShaderParams(SHADER_PERSPECTIVE_SPRITE), m_kTrans, 2);

	ccGLBindTexture2D(  getTexture()->getName() );

	//
	// Attributes
	//
#define kQuadSize sizeof(m_sQuad.bl)
	long offset = (long)&m_sQuad;

	// vertex
	int diff = offsetof( ccV3F_C4B_T2F, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

	// texCoods
	diff = offsetof( ccV3F_C4B_T2F, texCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color
	diff = offsetof( ccV3F_C4B_T2F, colors);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CC_INCREMENT_GL_DRAWS(1);
}

void Perspective::SetRotate(VeQuaternion& kQuat)
{
	m_kRotate = kQuat;
}

const VeQuaternion& Perspective::GetRotate()
{
	return m_kRotate;
}

void Perspective::SetRotateCenter(VeFloat32 x, VeFloat32 y)
{
	m_kTrans.x = x;
	m_kTrans.y = y;
}

void Perspective::SetPerspectiveRate(VeFloat32 f32Rate)
{
	m_kTrans.z = f32Rate;
}

PlayerCard::PlayerCard()
{
	m_pkRoot = VE_NEW UIWidget;
	AddChild(m_pkRoot);

	m_pkBoard = CCSprite::createWithSpriteFrameName("player_frame.png");
	m_pkRoot->addChild(m_pkBoard, 1);
	m_pkCdBar = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("cd_frame.png"));
	m_pkCdBar->setType(kCCProgressTimerTypeRadial);
	m_pkCdBar->setReverseProgress(true);
	m_pkRoot->addChild(m_pkCdBar, 0);

	m_pkWinFrame = CCSprite::createWithSpriteFrameName("player_win.png");
	m_pkRoot->addChild(m_pkWinFrame, 2);
	m_pkWinFrame->setVisible(false);

	m_pkHead = CCSprite::create();
	m_pkHead->setPositionX(VeFloorf(-19.0f * g_f32ScaleHD));
	m_pkHead->setScale(0.6f);
	m_pkRoot->addChild(m_pkHead, 1);

	m_pkSex = CCSprite::create();
	m_pkSex->setPositionX(VeFloorf(-6.0f * g_f32ScaleHD));
	m_pkSex->setPositionY(VeFloorf(14.0f * g_f32ScaleHD));
	m_pkSex->setScale(0.6f);
	m_pkRoot->addChild(m_pkSex, 1);

	m_pkVip = CCSprite::createWithSpriteFrameName("vip_title.png");
	m_pkVip->setPositionX(VeFloorf(-28.0f * g_f32ScaleHD));
	m_pkVip->setPositionY(VeFloorf(14.0f * g_f32ScaleHD));
	m_pkRoot->addChild(m_pkVip, 1);

	m_pkRoot->setContentSize(m_pkCdBar->getContentSize());

	m_pkRoot->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * 332) * g_f32ScaleHD)));
	m_pkRoot->setScale(ui_main_scale);

	CCSpriteBatchNode* pkBatchNode = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	addChild(pkBatchNode);

	m_apkBoardHalo[0] = CCSprite::createWithSpriteFrameName("board_halo.png");
	m_apkBoardHalo[0]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkBoardHalo[0]->setPosition(ui_ccp(0.5f, 0.0f));
	m_apkBoardHalo[0]->setScale(ui_main_scale);
	pkBatchNode->addChild(m_apkBoardHalo[0]);

	m_apkBoardHalo[1] = CCSprite::createWithSpriteFrameName("board_halo.png");
	m_apkBoardHalo[1]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkBoardHalo[1]->setPosition(ui_ccp(0.5f, 0.0f));
	m_apkBoardHalo[1]->setScaleX(-ui_main_scale);
	m_apkBoardHalo[1]->setScaleY(ui_main_scale);
	pkBatchNode->addChild(m_apkBoardHalo[1]);

	m_apkBoardHalo[0]->setPositionY(VeFloorf((ui_height_scale * 98.0f - 116.0f * ui_main_scale) * g_f32ScaleHD));
	m_apkBoardHalo[1]->setPositionY(VeFloorf((ui_height_scale * 98.0f - 116.0f * ui_main_scale) * g_f32ScaleHD));

	m_pkDealer = CCSprite::createWithSpriteFrameName("dealer.png");
	m_pkDealer->setPosition(ccp(33.0f * g_f32ScaleHD, 23.0f * g_f32ScaleHD));
	m_pkRoot->addChild(m_pkDealer, 1);
	m_pkDealer->setVisible(false);

	setContentSize(m_pkCdBar->getContentSize());
	SetRate(1.0f);
	SetHeadIcon(0, true, true);

	CCNode* pkTextNode = CCNode::create();
	pkTextNode->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 1.0f - ((ui_height_scale - ui_main_scale) * 250.0f + ui_main_scale * 332) * g_f32ScaleHD)));
	addChild(pkTextNode, 5);

	m_eProcess = PROCESS_NULL;

	m_pkProcess = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkProcess->setPositionY(VeFloorf(26.0f * g_f32ScaleHD * ui_main_scale));
	pkTextNode->addChild(m_pkProcess, 5);
	m_pkProcess->setVisible(false);

	m_pkName = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkName->setPositionY(VeFloorf(26.0f * g_f32ScaleHD * ui_main_scale));
	pkTextNode->addChild(m_pkName, 5);

	CCSize kSize;

	CCLabelTTF* pkWinBet = CCLabelTTF::create(TT("CardMaxWinBet"), TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	pkWinBet->setColor(ccc3(150, 150, 150));
	pkWinBet->setPositionY(VeFloorf(2.0f * g_f32ScaleHD * ui_main_scale));
	kSize = pkWinBet->getContentSize();
	pkWinBet->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkTextNode->addChild(pkWinBet, 5);

	CCLabelTTF* pkWinRate = CCLabelTTF::create(TT("CardWinRate"), TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	pkWinRate->setColor(ccc3(150, 150, 150));
	pkWinRate->setPositionY(-VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	kSize = pkWinRate->getContentSize();
	pkWinRate->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkTextNode->addChild(pkWinRate, 5);

	m_pkTitle = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkTitle->setPositionX(VeFloorf(19.0f * g_f32ScaleHD * ui_main_scale));
	m_pkTitle->setPositionY(VeFloorf(14.0f * g_f32ScaleHD * ui_main_scale));
	pkTextNode->addChild(m_pkTitle, 5);

	m_pkWinBet = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkWinBet->setPositionX(VeFloorf(38.0f * g_f32ScaleHD * ui_main_scale));
	m_pkWinBet->setPositionY(VeFloorf(2.0f * g_f32ScaleHD * ui_main_scale));
	pkTextNode->addChild(m_pkWinBet, 5);

	m_pkWinRate = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkWinRate->setPositionX(VeFloorf(38.0f * g_f32ScaleHD * ui_main_scale));
	m_pkWinRate->setPositionY(-VeFloorf(10.0f * g_f32ScaleHD * ui_main_scale));
	pkTextNode->addChild(m_pkWinRate, 5);

	m_pkHandStrength = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkHandStrength->setPositionY(-VeFloorf(25.0f * g_f32ScaleHD * ui_main_scale));
	pkTextNode->addChild(m_pkHandStrength, 5);

	m_f32Delay = 0.0f;
}

PlayerCard::~PlayerCard()
{

}

void PlayerCard::SetWin(bool bWin)
{
	m_pkWinFrame->setVisible(bWin);
}

void PlayerCard::SetRate(VeFloat32 f32Rate)
{
	f32Rate = VE_MIN(f32Rate, 1.0f);
	f32Rate = VE_MAX(f32Rate, 0.0f);
	if(f32Rate < 1.0f)
	{
		m_pkCdBar->setPercentage((1.0f - f32Rate) * 100.0f);
		VeColor kColor;
		if(f32Rate < 0.4f)
		{
			kColor = VeLerp(f32Rate * 10.0f / 4.0f, VeColor(0.0f, 1.0f, 0.0f), VeColor(1.0f, 1.0f, 0.0f));
		}
		else if(f32Rate < 0.8f)
		{
			kColor = VeLerp((f32Rate - 0.4f) * 10.0f / 4.0f, VeColor(1.0f, 1.0f, 0.0f), VeColor(1.0f, 0.0f, 0.0f));
		}
		else
		{
			kColor = VeColor(1.0f, 0.0f, 0.0f);
		}
		m_pkCdBar->setColor(ccc3(kColor.r * 255, kColor.g * 255, kColor.b * 255));
		m_apkBoardHalo[0]->setColor(ccc3(kColor.r * 255, kColor.g * 255, kColor.b * 255));
		m_apkBoardHalo[1]->setColor(ccc3(kColor.r * 255, kColor.g * 255, kColor.b * 255));
		m_pkCdBar->setVisible(true);
		m_apkBoardHalo[0]->setVisible(true);
		m_apkBoardHalo[1]->setVisible(true);
	}
	else
	{
		m_pkCdBar->setVisible(false);
		m_apkBoardHalo[0]->setVisible(false);
		m_apkBoardHalo[1]->setVisible(false);
	}
}

void PlayerCard::SetHeadIcon(VeUInt32 u32Index, bool bMale, bool bVip)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "head_%d.png", u32Index);
	m_pkHead->initWithSpriteFrameName(acBuffer);
	if(bMale)
	{
		m_pkSex->initWithSpriteFrameName("sex_m.png");
	}
	else
	{
		m_pkSex->initWithSpriteFrameName("sex_f.png");
	}
	m_pkVip->setVisible(bVip);
}

void PlayerCard::SetName(const VeChar8* pcName)
{
	m_pkName->setString(pcName);
	const CCSize& kSize = m_pkName->getContentSize();
	m_pkName->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void PlayerCard::SetTitle(const VeChar8* pcTitle)
{
	m_pkTitle->setString(pcTitle);
	const CCSize& kSize = m_pkTitle->getContentSize();
	m_pkTitle->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void PlayerCard::SetGameData(VeUInt32 u32MaxWinBet, VeFloat32 f32WinRate)
{
	CCSize kSize;
	m_pkWinBet->setString(NTL(u32MaxWinBet, 4));
	kSize = m_pkWinBet->getContentSize();
	m_pkWinBet->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "%.0f%%", f32WinRate * 100.0f);
	m_pkWinRate->setString(acBuffer);
	kSize = m_pkWinRate->getContentSize();
	m_pkWinRate->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void PlayerCard::SetStrength(VeFloat32 f32Rate, bool bShow)
{
	m_pkHandStrength->setString("");

	/*if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	if(pkPlayer->GetVip())
	{
		if(bShow)
		{
			VeChar8 acBuffer[64];
			VeSprintf(acBuffer, 64, "%s %.0f%%%s", TT("HandStrengthName"), f32Rate, TT("HandStrengthWin"));
			m_pkHandStrength->setString(acBuffer);
		}
		else
		{
			m_pkHandStrength->setString("");
		}
	}
	else
	{
		m_pkHandStrength->setString(TT("HandStrengthLocked"));
	}
	CCSize kSize = m_pkHandStrength->getContentSize();
	m_pkHandStrength->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));*/
}

void PlayerCard::SetSeatIndex(VeUInt32 u32Index)
{
	m_u32SeatIndex = u32Index;
}

VeUInt32 PlayerCard::GetSeatIndex()
{
	return m_u32SeatIndex;
}

void PlayerCard::UpdateTurnTime( VeFloat32 f32Delta )
{
	f32Delta = g_pTime->GetDeltaTime();
	m_f32Delay += f32Delta;
	if(m_f32Delay >= PLAYER_TURN_TIME)
	{
		m_f32Delay = PLAYER_TURN_TIME;
		SetRate(m_f32Delay);
		m_f32Delay = 0.0f;
		unschedule(schedule_selector(PlayerCard::UpdateTurnTime));

	}
	else
	{
		VeFloat32 f32Rate = VeSinf(m_f32Delay * VE_MATH_PI_2_F / PLAYER_TURN_TIME);
		SetRate(f32Rate);
	}
}

void PlayerCard::StopUpdateRate()
{
	unschedule(schedule_selector(PlayerCard::UpdateTurnTime));
	m_f32Delay = PLAYER_TURN_TIME;
	SetRate(m_f32Delay);
	m_f32Delay = 0.0f;
}

void PlayerCard::SetDealer(bool bDealer)
{
	m_pkDealer->setVisible(bDealer);
}

bool PlayerCard::GetDealer()
{
	return m_pkDealer->isVisible();
}

void PlayerCard::SetProcess(Process eProcess)
{
	m_eProcess = eProcess;
	if(m_eProcess)
	{
		m_pkName->setVisible(false);
		m_pkProcess->setVisible(true);
		VeChar8 acBuffer[64];
		if(eProcess < PROCESS_CATE_ROYAL_FLUSH)
		{
			VeSprintf(acBuffer, 64, "GameProcess%d", m_eProcess);
		}
		else if(eProcess < PROCESS_MAX)
		{
			VeSprintf(acBuffer, 64, "Categories_%d", m_eProcess - PROCESS_CATE_ROYAL_FLUSH);
		}
		m_pkProcess->setString(TT(acBuffer));
		const CCSize& kSize = m_pkProcess->getContentSize();
		m_pkProcess->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	else
	{
		m_pkName->setVisible(true);
		m_pkProcess->setVisible(false);
	}
}

PlayerCard::Process PlayerCard::GetProcess()
{
	return m_eProcess;
}

const cocos2d::CCPoint PlayerCard::GetPosition()
{
	return m_pkRoot->getPosition();
}

NameCard::NameCard(VeUInt32 u32Index)
{
	CCSize kSize;

	m_spCanteenData = VE_NEW UILayerChangeParams(u32Index, u32Index);
	UIInitDelegate(NameCard, OnPresent);
	UIInitDelegate(NameCard, OnPresentDrink);
	UIInitDelegate(NameCard, OnSeat);
	setScale(ui_main_scale);
	m_pkSeat = VE_NEW UIButton("seat_n.png", "seat_p.png");
	AddChild(m_pkSeat);
	UILinkDelegate(m_pkSeat, OnSeat);

	m_pkRoot = VE_NEW UIWidget;
	AddChild(m_pkRoot);

	m_pkBoard = CCSprite::createWithSpriteFrameName("opponent_frame.png");
	m_pkRoot->addChild(m_pkBoard, 1);
	m_pkCdBar = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("cd_frame.png"));
	m_pkCdBar->setType(kCCProgressTimerTypeRadial);
	m_pkCdBar->setReverseProgress(true);
	m_pkRoot->addChild(m_pkCdBar, 0);

	m_pkWinFrame = CCSprite::createWithSpriteFrameName("player_win.png");
	m_pkRoot->addChild(m_pkWinFrame, 2);
	m_pkWinFrame->setVisible(false);

	m_pkOverlayText = CCSprite::create();
	m_pkOverlayText->setPositionX(VeFloorf(20.0f * g_f32ScaleHD));
	m_pkRoot->addChild(m_pkOverlayText, 6);
	m_pkOverlayText->setVisible(false);

	m_pkOverlay = CCSprite::createWithSpriteFrameName("opponent_frame_d.png");
	m_pkRoot->addChild(m_pkOverlay, 6);
	m_pkOverlay->setVisible(false);

	m_pkHead = CCSprite::create();
	m_pkHead->setPositionX(VeFloorf(-19.0f * g_f32ScaleHD));
	m_pkHead->setScale(0.6f);
	m_pkRoot->addChild(m_pkHead, 1);

	m_pkSex = CCSprite::create();
	m_pkSex->setPositionX(VeFloorf(-6.0f * g_f32ScaleHD));
	m_pkSex->setPositionY(VeFloorf(14.0f * g_f32ScaleHD));
	m_pkSex->setScale(0.6f);
	m_pkRoot->addChild(m_pkSex, 1);

	m_pkVip = CCSprite::createWithSpriteFrameName("vip_title.png");
	m_pkVip->setPositionX(VeFloorf(-28.0f * g_f32ScaleHD));
	m_pkVip->setPositionY(VeFloorf(14.0f * g_f32ScaleHD));
	m_pkRoot->addChild(m_pkVip, 1);

	m_pkDealer = CCSprite::createWithSpriteFrameName("dealer.png");
	m_pkDealer->setPosition(ccp(33.0f * g_f32ScaleHD, 23.0f * g_f32ScaleHD));
	m_pkRoot->addChild(m_pkDealer, 8);
	m_pkDealer->setVisible(false);

	m_pkGiveDrink = VE_NEW UIButton("drink_s_n.png", "drink_s_p.png");
	m_pkRoot->AddChild(m_pkGiveDrink, 8);
	m_pkGiveDrink->setPosition(ccp(-35.0f * g_f32ScaleHD, 27.0f * g_f32ScaleHD));
	UILinkDelegate(m_pkGiveDrink, OnPresent);
	kSize = m_pkGiveDrink->getContentSize();
	kSize.width *= 2.0f;
	kSize.height *= 2.0f;
	m_pkGiveDrink->setContentSize(kSize);

	m_pkDrink = VE_NEW UIButton("drink_0.png", "drink_0.png");
	m_pkDrink->setPosition(ccp(-30.0f * g_f32ScaleHD, 34.0f * g_f32ScaleHD));
	m_pkDrink->setScale(0.6f);
	m_pkRoot->AddChild(m_pkDrink, 8);
	m_pkDrink->SetVisible(false);
	kSize = m_pkDrink->getContentSize();
	kSize.width *= 2.0f;
	m_pkDrink->setContentSize(kSize);
	UILinkDelegate(m_pkDrink, OnPresentDrink);

	kSize = m_pkCdBar->getContentSize();
	kSize.width *= 1.2f;
	kSize.height *= 1.2f;
	m_pkRoot->setContentSize(kSize);
	setContentSize(kSize);
	SetRate(1.0f);
	ShowBoard();
	SetDrink(0);
	SetHeadIcon(0, true, true);

	m_eProcess = PlayerCard::PROCESS_NULL;

	m_pkProcess = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkProcess->setPositionY(VeFloorf(26.0f * g_f32ScaleHD * ui_main_scale) / ui_main_scale);
	m_pkProcess->setScale(1.0f / ui_main_scale);
	m_pkRoot->addChild(m_pkProcess, 5);
	m_pkProcess->setVisible(false);

	m_pkName = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkName->setPositionY(VeFloorf(26.0f * g_f32ScaleHD * ui_main_scale) / ui_main_scale);
	m_pkName->setScale(1.0f / ui_main_scale);
	m_pkRoot->addChild(m_pkName, 5);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chip_1.png");
	pkChips->setPosition(ccp(-30.0f * g_f32ScaleHD, -26.0f * g_f32ScaleHD));
	m_pkRoot->addChild(pkChips, 5);

	m_pkChips = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkChips->setPositionX(VeFloorf(33.0f * g_f32ScaleHD * ui_main_scale) / ui_main_scale);
	m_pkChips->setPositionY(VeFloorf(-26.0f * g_f32ScaleHD * ui_main_scale) / ui_main_scale);
	m_pkChips->setScale(1.0f / ui_main_scale);
	m_pkRoot->addChild(m_pkChips, 5);

	m_pkWinChips = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkWinChips->setPositionX(VeFloorf(33.0f * g_f32ScaleHD * ui_main_scale) / ui_main_scale);
	m_pkWinChips->setPositionY(VeFloorf(-26.0f * g_f32ScaleHD * ui_main_scale) / ui_main_scale);
	m_pkWinChips->setScale(1.0f / ui_main_scale);
	m_pkRoot->addChild(m_pkWinChips, 5);
	m_pkWinChips->setColor(ccc3(20, 200, 0));
	m_pkWinChips->setVisible(false);

	m_apkCard[0] = VE_NEW MiniCard;
	m_apkCard[0]->setPositionX(13.0f * g_f32ScaleHD);
	m_apkCard[0]->setScale(1.0f / ui_main_scale);
	m_pkRoot->AddChild(m_apkCard[0]);

	m_apkCard[1] = VE_NEW MiniCard;
	m_apkCard[1]->setPositionX(27.0f * g_f32ScaleHD);
	m_apkCard[1]->setScale(1.0f / ui_main_scale);
	m_pkRoot->AddChild(m_apkCard[1]);

	m_apkCard[0]->setZOrder(4);
	m_apkCard[1]->setZOrder(5);

	m_f32Delay = 0.0f;
}

NameCard::~NameCard()
{

}

void NameCard::SetWin(VeUInt32 u32WinChips)
{
	if(u32WinChips)
	{
		m_pkWinFrame->setVisible(true);
		m_pkWinChips->setVisible(true);
		m_pkChips->setVisible(false);
		VeChar8 acBuffer[64];
		VeSprintf(acBuffer, 64, "+ %s", NTL(u32WinChips, 5));
		m_pkWinChips->setString(acBuffer);
		const CCSize& kSize = m_pkChips->getContentSize();
		m_pkWinChips->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	else
	{
		m_pkWinFrame->setVisible(false);
		m_pkWinChips->setVisible(false);
		m_pkChips->setVisible(true);
	}
}

void NameCard::SetRate(VeFloat32 f32Rate)
{
	f32Rate = VE_MIN(f32Rate, 1.0f);
	f32Rate = VE_MAX(f32Rate, 0.0f);
	if(f32Rate < 1.0f)
	{
		m_pkCdBar->setPercentage((1.0f - f32Rate) * 100.0f);
		VeColor kColor;
		if(f32Rate < 0.4f)
		{
			kColor = VeLerp(f32Rate * 10.0f / 4.0f, VeColor(0.0f, 1.0f, 0.0f), VeColor(1.0f, 1.0f, 0.0f));
		}
		else if(f32Rate < 0.8f)
		{
			kColor = VeLerp((f32Rate - 0.4f) * 10.0f / 4.0f, VeColor(1.0f, 1.0f, 0.0f), VeColor(1.0f, 0.0f, 0.0f));
		}
		else
		{
			kColor = VeColor(1.0f, 0.0f, 0.0f);
		}
		m_pkCdBar->setColor(ccc3(kColor.r * 255, kColor.g * 255, kColor.b * 255));
		m_pkCdBar->setVisible(true);
	}
	else
	{
		m_pkCdBar->setVisible(false);
	}
}

void NameCard::ShowSeat()
{
	m_pkSeat->SetVisible(true);
	m_pkRoot->SetVisible(false);
}

void NameCard::ShowBoard()
{
	m_pkSeat->SetVisible(false);
	m_pkRoot->SetVisible(true);
}

void NameCard::SetDrink(VeInt32 i32Index)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	if(pkPlayer->GetRoomInfo().m_tType == ROOM_TYPE_PRACTICE)
	{
		m_pkGiveDrink->SetVisible(false);
		m_pkDrink->SetVisible(false);
	}
	else
	{
		if(i32Index < 0)
		{
			m_pkGiveDrink->SetVisible(true);
			m_pkDrink->SetVisible(false);
		}
		else
		{
			m_pkGiveDrink->SetVisible(false);
			m_pkDrink->SetVisible(true);
			VeChar8 acBuffer[64];
			VeSprintf(acBuffer, 64, "drink_%d.png", i32Index);
			((CCSprite*)(m_pkDrink->Normal()))->initWithSpriteFrameName(acBuffer);
			((CCSprite*)(m_pkDrink->Pressed()))->initWithSpriteFrameName(acBuffer);
			((CCSprite*)(m_pkDrink->Pressed()))->setColor(ccc3(200,200,200));
		}
	}
}

void NameCard::SetHeadIcon(VeUInt32 u32Index, bool bMale, bool bVip)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "head_%d.png", u32Index);
	m_pkHead->initWithSpriteFrameName(acBuffer);
	if(bMale)
	{
		m_pkSex->initWithSpriteFrameName("sex_m.png");
	}
	else
	{
		m_pkSex->initWithSpriteFrameName("sex_f.png");
	}
	m_pkVip->setVisible(bVip);
}

void NameCard::SetRight()
{
	m_pkGiveDrink->setPositionX(35.0f * g_f32ScaleHD);
	m_pkDrink->setPositionX(30.0f * g_f32ScaleHD);
	m_pkDealer->setPositionX(-33.0f * g_f32ScaleHD);
}

void NameCard::SetName(const VeChar8* pcName)
{
	m_pkName->setString(pcName);
	const CCSize& kSize = m_pkName->getContentSize();
	m_pkName->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void NameCard::SetChips(VeUInt32 u32Chips)
{
	m_pkChips->setString(NTL(u32Chips, 5));
	const CCSize& kSize = m_pkChips->getContentSize();
	m_pkChips->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

UIImplementDelegate(NameCard, OnPresent, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		VeDebugOutput("ButForSitId[%d]",m_u32SeatIndex);
		g_pLayerManager->PushLayer("canteen", VE_NEW UILayerChangeParams(1, m_u32SeatIndex));
	}
};

UIImplementDelegate(NameCard, OnPresentDrink, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		VeDebugOutput("ButForSitId[%d]",m_u32SeatIndex);
		g_pLayerManager->PushLayer("canteen", VE_NEW UILayerChangeParams(1, m_u32SeatIndex));
	}
};

class ChangeSeatAlertCallback : public UIAlert::Callback
{
public:
	ChangeSeatAlertCallback(VeUInt32 u32Index)
	{
		m_kTitle = TT("ChangeSeatConfirmTitle");
		m_kContent = TT("ChangeSeatConfirmContent");
		m_kEnter = TT("Confirm");
		m_kCancel = TT("Cancel");
		m_u32SeatIndex = u32Index;
	}

	virtual void OnConfirm()
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqChangeSeat(m_u32SeatIndex);
		g_pLayerManager->Back();
	}

protected:
	VeUInt32 m_u32SeatIndex;

};

UIImplementDelegate(NameCard, OnSeat, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		const Table& kTable = pkPlayer->GetTableState();
		const TablePlayer& kSelf = *pkPlayer->GetTablePlayer(pkPlayer->GetSeatIndex());
		if(kTable.m_tState && kSelf.m_tState >= TABLE_PLAYER_PLAY && kSelf.m_tState != TABLE_PLAYER_FOLD)
		{
			g_pLayerManager->PushLayer("alert", VE_NEW ChangeSeatAlertCallback(m_u32SeatIndex));
		}
		else
		{
			pkPlayer->S_ReqChangeSeat(m_u32SeatIndex);
		}
	}
};

MiniCard* NameCard::GetLeft()
{
	return m_apkCard[0];
}

MiniCard* NameCard::GetRight()
{
	return m_apkCard[1];
}

void NameCard::SetMiniCardVisible(bool mbVisible)
{
	 m_apkCard[0]->setVisible(mbVisible);
	 m_apkCard[1]->setVisible(mbVisible);
	 m_apkCard[0]->SetFace(false);
	 m_apkCard[1]->SetFace(false);
}

void NameCard::SetSeatIndex(VeUInt32 u32Index)
{
	m_u32SeatIndex = u32Index;
}

VeUInt32 NameCard::GetSeatIndex()
{
	return m_u32SeatIndex;
}

void NameCard::UpdateTurnTime( VeFloat32 f32Delta )
{
	f32Delta = g_pTime->GetDeltaTime();
	m_f32Delay += f32Delta;
	if(m_f32Delay >= PLAYER_TURN_TIME)
	{
		m_f32Delay = PLAYER_TURN_TIME;
		SetRate(m_f32Delay);
		m_f32Delay = 0.0f;
		unschedule(schedule_selector(NameCard::UpdateTurnTime));

	}
	else
	{
		VeFloat32 f32Rate = VeSinf(m_f32Delay * VE_MATH_PI_2_F / PLAYER_TURN_TIME);
		SetRate(f32Rate);
	}
}

void NameCard::StopUpdateRate()
{
	m_f32Delay = PLAYER_TURN_TIME;
	SetRate(m_f32Delay);
	unschedule(schedule_selector(NameCard::UpdateTurnTime));
	m_f32Delay = 0.0f;
}

void NameCard::SetDealer(bool bDealer)
{
	m_pkDealer->setVisible(bDealer);
}

bool NameCard::GetDealer()
{
	return m_pkDealer->isVisible();
}

void NameCard::SetProcess(PlayerCard::Process eProcess)
{
	m_eProcess = eProcess;
	if(m_eProcess)
	{
		m_pkName->setVisible(false);
		m_pkProcess->setVisible(true);
		VeChar8 acBuffer[64];
		if(eProcess < PlayerCard::PROCESS_CATE_ROYAL_FLUSH)
		{
			VeSprintf(acBuffer, 64, "GameProcess%d", m_eProcess);
		}
		else if(eProcess < PlayerCard::PROCESS_MAX)
		{
			VeSprintf(acBuffer, 64, "Categories_%d", m_eProcess - PlayerCard::PROCESS_CATE_ROYAL_FLUSH);
		}
		m_pkProcess->setString(TT(acBuffer));
		const CCSize& kSize = m_pkProcess->getContentSize();
		m_pkProcess->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	else
	{
		m_pkName->setVisible(true);
		m_pkProcess->setVisible(false);
	}
}

PlayerCard::Process NameCard::GetProcess()
{
	return m_eProcess;
}

void NameCard::SetOverlay(Overlay eOverlay)
{
	switch(eOverlay)
	{
	case OVERLAY_HOLD:
		m_pkOverlayText->initWithSpriteFrameName("btn_hold.png");
		m_pkOverlay->setVisible(true);
		m_pkOverlayText->setVisible(true);
		break;
	case OVERLAY_READY:
		m_pkOverlayText->initWithSpriteFrameName("btn_reday.png");
		m_pkOverlay->setVisible(true);
		m_pkOverlayText->setVisible(true);
		break;
	case OVERLAY_FOLD:
		m_pkOverlayText->initWithSpriteFrameName("btn_discard_text.png");
		m_pkOverlay->setVisible(true);
		m_pkOverlayText->setVisible(true);
		break;
	default:
		m_pkOverlay->setVisible(false);
		m_pkOverlayText->setVisible(false);
		break;
	}
}

Number::Number(const VeChar8* pcType, VeFloat32 f32Space)
	: m_kType(pcType), m_f32Space(f32Space), m_u32Number(0)
{
	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	addChild(m_pkRoot);
	Update();
}

Number::Number(const VeChar8* pcImage, const VeChar8* pcType, VeFloat32 f32Space)
	: m_kType(pcType), m_f32Space(f32Space), m_u32Number(0)
{
	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath(pcImage, false) + ".png");
	addChild(m_pkRoot);
	Update();
}

Number::~Number()
{
	Clear();
}

void Number::SetNumber(VeUInt32 u32Number)
{
	if(u32Number != m_u32Number)
	{
		m_u32Number = u32Number;
		Update();
	}
}

VeUInt32 Number::GetNumber()
{
	return m_u32Number;
}

void Number::Update()
{
	Clear();
	VeChar8 acBuffer[64];
	VeUInt32 i(0);
	VeUInt32 u32Temp = m_u32Number;
	do
	{
		VeSprintf(acBuffer, 64, "%s_%d.png", m_kType.GetString(), u32Temp % 10);
		CCSprite* pkSprite = CCSprite::createWithSpriteFrameName(acBuffer);
		pkSprite->setPositionX(-m_f32Space * i);
		m_pkRoot->addChild(pkSprite);
		u32Temp /= 10;
		++i;
	}
	while(u32Temp);
}

void Number::Clear()
{
	m_pkRoot->removeAllChildren();
}

EditInfo::EditInfo() : UILayerModalExt<EditInfo>("edit_info")
{
	UIInitDelegate(EditInfo, OnConfirm);
	UIInitDelegate(EditInfo, OnCancel);
	UIInitDelegate(EditInfo, OnSex);
	UIInitDelegate(EditInfo, OnChoose);
}

EditInfo::~EditInfo()
{
	term();
}

bool EditInfo::init()
{
	VeChar8 acBuffer[64];
	CCSize kSize;

	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 280.0f * ui_main_scale * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	UIBarH* pkTitle = UIBarH::createWithEl("alert_title", 280.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD);
	pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 205.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	CCLabelTTF* pkTitleText = CCLabelTTF::create(TT("EditInfoTitle"), TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
	addChild(pkTitleText, 1);
	pkTitleText->setAnchorPoint(ccp(VeFloorf(pkTitleText->getContentSize().width * 0.5f) / pkTitleText->getContentSize().width, 0));
	pkTitleText->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 175.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));
	
	UIButton* pkConfirm = VE_NEW UIButton("confirm_n.png", "confirm_p.png");
	pkConfirm->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 70.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 172.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkConfirm->setScale(ui_main_scale);
	AddWidget("confirm", pkConfirm);
	UILinkDelegate(pkConfirm, OnConfirm);

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 70.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 172.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	m_pkHeadArea = VE_NEW HeadArea;
	m_pkHeadArea->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 89.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 114.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	AddWidget("head_area", m_pkHeadArea, 0);

	CCLabelTTF* pkNameText = CCLabelTTF::create(TT("Nickname"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkNameText->getContentSize();
	pkNameText->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkNameText->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 48.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 139.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(pkNameText);
	pkNameText->setColor(ccc3(150, 150, 150));

	CCLabelTTF* pkSex = CCLabelTTF::create(TT("Sex"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkSex->getContentSize();
	pkSex->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkSex->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 48.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 114.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(pkSex);
	pkSex->setColor(ccc3(150, 150, 150));

	CCLabelTTF* pkMail = CCLabelTTF::create(TT("Tell"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkMail->getContentSize();
	pkMail->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkMail->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 48.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 89.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(pkMail);
	pkMail->setColor(ccc3(150, 150, 150));

	UIChooseButton* pkCheckMale = VE_NEW UIChooseButton("check_n.png", "check_p.png");
	pkCheckMale->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 18.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 114.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCheckMale->setScale(ui_main_scale);
	CCSprite* pkIconM = CCSprite::createWithSpriteFrameName("sex_m.png");
	pkIconM->setPositionX(VeFloorf(25.0f * g_f32ScaleHD));
	pkCheckMale->addChild(pkIconM);

	UIChooseButton* pkCheckFemale = VE_NEW UIChooseButton("check_n.png", "check_p.png");
	pkCheckFemale->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 75.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 114.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCheckFemale->setScale(ui_main_scale);
	CCSprite* pkIconF = CCSprite::createWithSpriteFrameName("sex_f.png");
	pkIconF->setPositionX(VeFloorf(25.0f * g_f32ScaleHD));
	pkCheckFemale->addChild(pkIconF);
	m_pkSexArea = VE_NEW UIChooseButtonArea();
	m_pkSexArea->AddButton(pkCheckMale);
	m_pkSexArea->AddButton(pkCheckFemale);
	AddWidget("sex_area", m_pkSexArea, 0);
	UILinkDelegate(m_pkSexArea, OnSex);

	m_pkInputName = VE_NEW UIInputBox(TT("InputName"), TTF_NAME, 12 * ui_main_scale * g_f32ScaleHD, 15);
	m_pkInputName->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 65.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 139.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	AddWidget("input_name", m_pkInputName);
	UIBarH* pkInputNameBar = UIBarH::createWithElEr("alert_input", 120.0f * ui_main_scale * g_f32ScaleHD);
	pkInputNameBar->setScaleY(ui_main_scale);
	m_pkInputName->addChild(pkInputNameBar);
	CCSize kNameSize = pkInputNameBar->getContentSize();
	kNameSize.height *= 2.0f * ui_main_scale;
	m_pkInputName->setContentSize(kNameSize);

	m_pkInputTell = VE_NEW UIInputBox(TT("InputTell"), TTF_NAME, 12 * ui_main_scale * g_f32ScaleHD, 15);
	m_pkInputTell->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 65.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 89.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	AddWidget("input_tell", m_pkInputTell);
	UIBarH* pkInputTellBar = UIBarH::createWithElEr("alert_input", 120.0f * ui_main_scale * g_f32ScaleHD);
	pkInputTellBar->setScaleY(ui_main_scale);
	m_pkInputTell->addChild(pkInputTellBar);
	CCSize kTellSize = pkInputTellBar->getContentSize();
	kTellSize.height *= 2.0f * ui_main_scale;
	m_pkInputTell->setContentSize(kTellSize);

	CCLabelTTF* pkHeadChoose = CCLabelTTF::create(TT("HeadChoose"), TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	kSize = pkHeadChoose->getContentSize();
	pkHeadChoose->setAnchorPoint(ccp(0, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkHeadChoose->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 120.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 70.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(pkHeadChoose);
	pkHeadChoose->setColor(ccc3(150, 150, 150));

	CCSpriteBatchNode* pkBatchNode = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	pkBatchNode->setPosition(ui_ccp(0.5f, 0.5f));
	addChild(pkBatchNode, 0);

	CCSpriteBatchNode* pkHeadBatchNode = CCSpriteBatchNode::create(g_pkGame->GetImagePath("head_icon", false) + ".png");
	pkHeadBatchNode->setPosition(ui_ccp(0.5f, 0.5f));
	addChild(pkHeadBatchNode, 0);
	
	CCSprite* apkBoard[4][4];
	CCSprite* apkHead[4][4];

	m_pkChooseArea = VE_NEW UIChooseButtonArea();
	AddWidget("choose_area", m_pkChooseArea, 0);
	UILinkDelegate(m_pkChooseArea, OnChoose);

	for(VeUInt32 i(0); i < 4; ++i)
	{
		for(VeUInt32 j(0); j < 4; ++j)
		{
			apkBoard[i][j] = CCSprite::createWithSpriteFrameName("head_area.png");
			apkBoard[i][j]->setScale(0.7f * ui_main_scale);
			apkBoard[i][j]->setPositionX(VeFloorf((-90.0f + 60.0f * j) * g_f32ScaleHD * ui_main_scale));
			apkBoard[i][j]->setPositionY(VeFloorf((43.0f - 52.0f * i) * g_f32ScaleHD * ui_main_scale));
			pkBatchNode->addChild(apkBoard[i][j]);

			VeSprintf(acBuffer, 64, "head_%d.png", i * 4 + j);
			apkHead[i][j] = CCSprite::createWithSpriteFrameName(acBuffer);
			apkHead[i][j]->setScale(0.7f * ui_main_scale);
			apkHead[i][j]->setPositionX(VeFloorf((-90.0f + 60.0f * j) * g_f32ScaleHD * ui_main_scale));
			apkHead[i][j]->setPositionY(VeFloorf((43.0f - 52.0f * i) * g_f32ScaleHD * ui_main_scale));
			pkHeadBatchNode->addChild(apkHead[i][j]);

			UIChooseButton* pkHeadHighlight = VE_NEW UIChooseButton(NULL, "head_area_hl.png");
			pkHeadHighlight->setScale(0.7f * ui_main_scale);
			pkHeadHighlight->setPositionX(VeFloorf(UILayer::Width() * 0.5f + (-90.0f + 60.0f * j) * g_f32ScaleHD * ui_main_scale));
			pkHeadHighlight->setPositionY(VeFloorf(UILayer::Height() * 0.5f + (43.0f - 52.0f * i) * g_f32ScaleHD * ui_main_scale));
			kSize = apkBoard[i][j]->getContentSize();
			pkHeadHighlight->setContentSize(kSize);
			m_pkChooseArea->AddButton(pkHeadHighlight);
		}
	}

	return UILayerModalExt<EditInfo>::init();
}

void EditInfo::OnActive(const UIUserDataPtr& spData)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_bEvent = false;
	UIButton::SetPressedSoundGlobal(SE_NULL);
	m_pkHeadArea->Set(pkPlayer->GetHeadIcon(), pkPlayer->GetSex() == 0, pkPlayer->GetVip() > 0);
	m_pkInputName->SetText(pkPlayer->GetNickname());
	m_pkInputName->SetActive(false);
	m_pkInputTell->SetText(pkPlayer->GetPhone());
	m_pkInputTell->SetActive(false);
	m_pkSexArea->Choose(pkPlayer->GetSex() ? 1 : 0);
	m_pkChooseArea->Choose(pkPlayer->GetHeadIcon());
	UIButton::SetPressedSoundGlobal(SE_BTN_CLICK);
	m_bEvent = true;
}

class EditInfoAlertCallback : public UIAlert::Callback
{
public:
	EditInfoAlertCallback()
	{
		m_kTitle = TT("EditInfoConfirmTitle");
		m_kContent = TT("EditInfoConfirmContent");
		m_kEnter = TT("Confirm");
		m_kCancel = TT("Cancel");
	}

	virtual void OnConfirm()
	{
		g_pLayerManager->PopAllLayers();
	}
};

bool EditInfo::OnBack()
{
	if(IsChange())
	{
		g_pLayerManager->PushLayer("alert", VE_NEW EditInfoAlertCallback);
		return false;
	}
	else
	{
		return true;
	}
}

void EditInfo::UpdateHead()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_pkHeadArea->Set(m_pkChooseArea->GetCurrentIndex(), m_pkSexArea->GetCurrentIndex() ? false : true, pkPlayer->GetVip() > 0);
}

bool EditInfo::IsChange()
{
	if(!g_pkHallAgent) return false;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return false;
	VeString kName = m_pkInputName->GetText();
	VeString kTell = m_pkInputTell->GetText();
	if(kName != pkPlayer->GetNickname())
	{
		return true;
	}
	if(kTell != pkPlayer->GetPhone())
	{
		return true;
	}
	if(m_pkChooseArea->GetCurrentIndex() != pkPlayer->GetHeadIcon())
	{
		return true;
	}
	if((m_pkSexArea->GetCurrentIndex() ? false : true) != (pkPlayer->GetSex() ? false : true))
	{
		return true;
	}
	return false;
}

UIImplementDelegate(EditInfo, OnConfirm, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		if(IsChange())
		{
			pkPlayer->S_ReqEditInfo(m_pkInputName->GetText(), m_pkSexArea->GetCurrentIndex(),
				m_pkChooseArea->GetCurrentIndex(), m_pkInputTell->GetText());
			g_pkGame->ShowLoading(false);
		}
		else
		{
			g_pLayerManager->Back();
		}
	}
}

UIImplementDelegate(EditInfo, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(EditInfo, OnSex, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE && m_bEvent)
	{
		UpdateHead();
	}
}

UIImplementDelegate(EditInfo, OnChoose, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE && m_bEvent)
	{
		UpdateHead();
	}
}

ChatWindow::ChatWindow() : UILayerModalExt<ChatWindow>("chat_window")
{
	UIInitDelegate(ChatWindow, OnClose);
	UIInitDelegate(ChatWindow, OnSend);
	UIInitDelegate(ChatWindow, OnSex);
	UIInitDelegate(ChatWindow, OnChoose);

	m_pkChatArea = NULL;
}

ChatWindow::~ChatWindow()
{
	term();
}

bool ChatWindow::init()
{
	VeChar8 acBuffer[64];
	CCSize kSize;

	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 320.0f * ui_main_scale * g_f32ScaleHD, 260.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.0f + 130.0f * ui_main_scale * g_f32ScaleHD)));

	UIBarH* pkTitle = UIBarH::createWithEl("alert_title", 320.0f * ui_main_scale * g_f32ScaleHD - 0 * g_f32ScaleHD);
	//pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.0f + 260.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	UIButton* pkClose = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkClose->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 1.0f - 25.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkClose->setScale(ui_main_scale);
	AddWidget("confirm", pkClose);
	UILinkDelegate(pkClose, OnClose);

	UIChooseButton* pkCheckMale = VE_NEW UIChooseButton("sex_m.png", "sex_m.png");
	pkCheckMale->setPosition(ccp(VeFloorf(UILayer::Width() * 0.0f + 18.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.0f + 260.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCheckMale->setScale(ui_main_scale);
	pkCheckMale->SetVisible( false );

	UIChooseButton* pkCheckFemale = VE_NEW UIChooseButton("sex_f.png", "sex_f.png");
	pkCheckFemale->setPosition(ccp(VeFloorf(UILayer::Width() * 0.0f + 55.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.0f + 260.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCheckFemale->setScale(ui_main_scale);
	pkCheckFemale->SetVisible( false );

	m_pkSexArea = VE_NEW UIChooseButtonArea();
	m_pkSexArea->AddButton(pkCheckMale);
	m_pkSexArea->AddButton(pkCheckFemale);
	AddWidget("sex_area", m_pkSexArea, 0);
	UILinkDelegate(m_pkSexArea, OnSex);

	m_pkInputName = VE_NEW UIInputBox(TT("InputChat"), TTF_NAME, 16 * ui_main_scale * g_f32ScaleHD, 90);
	m_pkInputName->setPosition(ccp(VeFloorf(UILayer::Width() * 0.0f + 142.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.0f + 260.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	AddWidget("input_name", m_pkInputName);
	UIBarH* pkInputNameBar = UIBarH::createWithElEr("alert_input", 280.0f * ui_main_scale * g_f32ScaleHD);
	pkInputNameBar->setScaleY(ui_main_scale * 1.25f);
	m_pkInputName->addChild(pkInputNameBar);
	CCSize kNameSize = pkInputNameBar->getContentSize();
	kNameSize.height *= 2.0f * ui_main_scale;
	m_pkInputName->setContentSize(kNameSize);

	UIButton* pkSend = VE_NEW UIButton("confirm_n.png", "confirm_p.png");
	pkSend->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.0f + 260.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkSend->setScale(ui_main_scale);
	AddWidget("cancel", pkSend);
	UILinkDelegate(pkSend, OnSend);

	CCSpriteBatchNode* pkBatchNode = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	pkBatchNode->setPosition(ui_ccp(0.5f, 0.25f));
	addChild(pkBatchNode, 0);

	CCSpriteBatchNode* pkHeadBatchNode = CCSpriteBatchNode::create(g_pkGame->GetImagePath("expression_icon", false) + ".png");
	pkHeadBatchNode->setPosition(ui_ccp(0.5f, 0.25f));
	addChild(pkHeadBatchNode, 0);
	
	//CCSprite* apkBoard[4][4];
	//CCSprite* apkHead[4][4];

	m_pkChooseArea = VE_NEW UIChooseButtonArea();
	m_pkChooseArea->setPosition(ui_ccp(0.5f, 0.25f));
	AddWidget("choose_area", m_pkChooseArea, 0);
	UILinkDelegate(m_pkChooseArea, OnChoose);

	for(VeUInt32 i(0); i < 4; ++i)
	{
		for(VeUInt32 j(0); j < 4; ++j)
		{
			m_apkBoard[i][j] = CCSprite::createWithSpriteFrameName("head_area.png");
			m_apkBoard[i][j]->setScale(0.7f * ui_main_scale);
			m_apkBoard[i][j]->setPositionX(VeFloorf((-90.0f + 60.0f * j) * g_f32ScaleHD * ui_main_scale));
			m_apkBoard[i][j]->setPositionY(VeFloorf((76.0f - 58.0f * i) * g_f32ScaleHD * ui_main_scale));
			m_apkBoard[i][j]->setVisible(false);
			pkBatchNode->addChild(m_apkBoard[i][j]);

			VeSprintf(acBuffer, 64, "expression_%d.png", i * 4 + j);
			m_apkHead[i][j] = CCSprite::createWithSpriteFrameName(acBuffer);
			m_apkHead[i][j]->setScaleX(0.8f * ui_main_scale);
			m_apkHead[i][j]->setScaleY(0.8f * ui_main_scale);
			m_apkHead[i][j]->setPositionX(VeFloorf((-90.0f + 60.0f * j) * g_f32ScaleHD * ui_main_scale));
			m_apkHead[i][j]->setPositionY(VeFloorf((76.0f - 58.0f * i) * g_f32ScaleHD * ui_main_scale));
			pkHeadBatchNode->addChild(m_apkHead[i][j]);

			UIChooseButton* pkHeadHighlight = VE_NEW UIChooseButton(NULL, NULL);
			pkHeadHighlight->setScale(0.7f * ui_main_scale);
			pkHeadHighlight->setPositionX(VeFloorf((-90.0f + 60.0f * j) * g_f32ScaleHD * ui_main_scale));
			pkHeadHighlight->setPositionY(VeFloorf((76.0f - 58.0f * i) * g_f32ScaleHD * ui_main_scale));
			kSize = m_apkBoard[i][j]->getContentSize();
			pkHeadHighlight->setContentSize(kSize);
			m_pkChooseArea->AddButton(pkHeadHighlight);
			UILinkDelegate(pkHeadHighlight, OnChooseHead);
		}
	}

	m_pkChatArea = VE_NEW ChatInfoChoose();
	m_pkChatArea->setPosition(ui_ccp(0.250f, 0.425f));
	m_pkChatArea->SetVisible(false);
	AddWidget("choose_chat", m_pkChatArea, 0);
	UILinkDelegate(m_pkChatArea, OnChooseChat);

	//for(int i = 0; i < 10; ++i)
	//{
	//	VeChar8 acChatBuffer[256];
	//	VeSprintf(acChatBuffer, 256, "ChatList%d", i);
	//	m_pkChatArea->AddChatInfoBar(TT(acChatBuffer), i & 1);
	//}
	/*
	m_pkChatArea->AddChatInfoBar("11dddddd");
	m_pkChatArea->AddChatInfoBar("22ddddddddd");
	m_pkChatArea->AddChatInfoBar("33ddddddssssss");
	m_pkChatArea->AddChatInfoBar("44ddddddfffffffffff");
	m_pkChatArea->AddChatInfoBar("55dddddddd");
	m_pkChatArea->AddChatInfoBar("66ddddddffffffffff");
	m_pkChatArea->AddChatInfoBar("77ddddddsssssss");
	m_pkChatArea->AddChatInfoBar("88ddddddssssssssssssssssssssssssdddddddddddd");
	m_pkChatArea->AddChatInfoBar("99ddddddff");
	m_pkChatArea->AddChatInfoBar("aadddddd");
	*/
	m_pkChatArea->ScrollToTop();

	m_pkSexArea->Choose(0);
	bool bShowHeadIcon = m_pkSexArea->GetCurrentIndex() > 0;
	ShowHeadIcon(bShowHeadIcon);
	ShowChatInfo(!bShowHeadIcon);

	return UILayerModalExt<ChatWindow>::init();
}

void ChatWindow::OnActive(const UIUserDataPtr& spData)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_bEvent = false;
	UIButton::SetPressedSoundGlobal(SE_NULL);
	//m_pkHeadArea->Set(pkPlayer->GetHeadIcon(), pkPlayer->GetSex() == 0, pkPlayer->GetVip() > 0);
	m_pkInputName->SetText("");
	m_pkInputName->SetActive(false);
	//m_pkInputTell->SetText(pkPlayer->GetPhone());
	//m_pkInputTell->SetActive(false);
	m_pkSexArea->Choose(pkPlayer->GetSex() ? 1 : 0);
	//m_pkChooseArea->Choose(pkPlayer->GetHeadIcon());
	bool bShowHeadIcon = m_pkSexArea->GetCurrentIndex() > 0;
	bShowHeadIcon = true;
	ShowHeadIcon(bShowHeadIcon);
	ShowChatInfo(!bShowHeadIcon);
	UIButton::SetPressedSoundGlobal(SE_BTN_CLICK);
	m_bEvent = true;
}

class ChatWindowAlertCallback : public UIAlert::Callback
{
public:
	ChatWindowAlertCallback()
	{
		m_kTitle = TT("EditInfoConfirmTitle");
		m_kContent = TT("EditInfoConfirmContent");
		m_kEnter = TT("Confirm");
		m_kCancel = TT("Cancel");
	}

	virtual void OnConfirm()
	{
		g_pLayerManager->PopAllLayers();
	}
};

bool ChatWindow::OnBack()
{
	//if(IsChange())
	//{
	//	g_pLayerManager->PushLayer("alert", VE_NEW ChatWindowAlertCallback);
	//	return false;
	//}
	//else
	//{
	//	return true;
	//}
	return true;
}

void ChatWindow::UpdateHead()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	//m_pkHeadArea->Set(m_pkChooseArea->GetCurrentIndex(), m_pkSexArea->GetCurrentIndex() ? false : true, pkPlayer->GetVip() > 0);
}

bool ChatWindow::IsChange()
{
	if(!g_pkHallAgent) return false;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return false;
	VeString kName = m_pkInputName->GetText();
	//VeString kTell = m_pkInputTell->GetText();
	if(kName != pkPlayer->GetNickname())
	{
		return true;
	}
	//if(kTell != pkPlayer->GetPhone())
	//{
	//	return true;
	//}
	if(m_pkChooseArea->GetCurrentIndex() != pkPlayer->GetHeadIcon())
	{
		return true;
	}
	//if((m_pkSexArea->GetCurrentIndex() ? false : true) != (pkPlayer->GetSex() ? false : true))
	//{
	//	return true;
	//}
	return false;
}

void ChatWindow::ShowHeadIcon(bool bShow)
{
	m_pkChooseArea->SetVisible(bShow);

	for(VeUInt32 i(0); i < 4; ++i)
	{
		for(VeUInt32 j(0); j < 4; ++j)
		{
			m_apkBoard[i][j]->setVisible(false);
			m_apkHead[i][j]->setVisible(bShow);
		}
	}
}

void ChatWindow::ShowChatInfo(bool bShow)
{
	if(m_pkChatArea != NULL)
	{
		m_pkChatArea->SetVisible(bShow);
	}
}

UIImplementDelegate(ChatWindow, OnClose, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(ChatWindow, OnSend, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		if(strlen(m_pkInputName->GetText()) > 0)
		{
			pkPlayer->S_ReqChatAll(4, m_pkInputName->GetText());			
		}
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(ChatWindow, OnSex, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE && m_bEvent)
	{
		//UpdateHead();
		bool bShowHeadIcon = m_pkSexArea->GetCurrentIndex() > 0;
		ShowHeadIcon(bShowHeadIcon);
		ShowChatInfo(!bShowHeadIcon);
	}
}

UIImplementDelegate(ChatWindow, OnChoose, kWidget, u32State)
{
	if(u32State == UIWidget::PRESSED)
	{
		UpdateHead();

		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;

		int index = m_pkChooseArea->GetCurrentIndex();
		m_pkChooseArea->GetCurrent()->SetChoose(false);
		char szChat[32] = "";
		sprintf(szChat, "#%d", index); 
		pkPlayer->S_ReqChatAll(4, szChat);
		g_pLayerManager->Back();		
	}
}

UIImplementDelegate(ChatWindow, OnChooseHead, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;

		int index = m_pkChooseArea->GetCurrentIndex();
		char szChat[32] = "";
		sprintf(szChat, "#%d", index); 
		pkPlayer->S_ReqChatAll(4, szChat);
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(ChatWindow, OnChooseChat, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{

	}
}

ChatInfoBar::ChatInfoBar(VeUInt32 index)
{
	m_u32Index = index;
}

ChatInfoBar::~ChatInfoBar()
{
	m_kUsedList.BeginIterator();
	while(!m_kUsedList.IsEnd())
	{
		ChatContent* pkContent = m_kUsedList.GetIterNode()->m_tContent;
		m_kUsedList.Next();
		VE_SAFE_DELETE(pkContent);
	}
	m_kFreeList.BeginIterator();
	while(!m_kFreeList.IsEnd())
	{
		ChatContent* pkContent = m_kFreeList.GetIterNode()->m_tContent;
		m_kFreeList.Next();
		VE_SAFE_DELETE(pkContent);
	}
}

void ChatInfoBar::Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space, bool bMove/* = false*/)
{
	if(m_kUsedList.Empty())
	{
		schedule(schedule_selector(ChatInfoBar::Update), 0.0f);
	}
	else
	{
		ChatContent* pkLast = m_kUsedList.GetLastNode()->m_tContent;
		if(pkLast != NULL)
		{
			pkLast->Restart(pcContent, f32Speed, f32Space);
			return;
		}
		//if(m_kCachedTexts.Size() || (!pkLast->IsShown()))
		//{
		//	m_kCachedTexts.PushBack(VePair< VeStringA, VePair<VeFloat32,VeFloat32> >(
		//		pcContent, VePair<VeFloat32,VeFloat32>(f32Speed, f32Space)));
		//	return;
		//}
	}
	_Chat(pcContent, f32Speed, f32Space, bMove);
}

void ChatInfoBar::Update(VeFloat32 f32Delta)
{
	if(m_kUsedList.Size())
	{
		m_kUsedList.BeginIterator();
		while(!m_kUsedList.IsEnd())
		{
			ChatContent* pkContent = m_kUsedList.GetIterNode()->m_tContent;
			m_kUsedList.Next();
			VE_ASSERT(pkContent);
			if(pkContent->Update(f32Delta))
			{
				m_kFreeList.AttachBack(*pkContent);
			}
		}
	}
	if(m_kUsedList.Empty())
	{
		unschedule(schedule_selector(ChatInfoBar::Update));
	}
	else if(m_kCachedTexts.Size())
	{
		ChatContent* pkLast = m_kUsedList.GetLastNode()->m_tContent;
		if(pkLast->IsShown())
		{
			VeList< VePair< VeStringA, VePair<VeFloat32,VeFloat32> > >::iterator it = m_kCachedTexts.Begin();
			_Chat(it->m_tFirst, it->m_tSecond.m_tFirst, it->m_tSecond.m_tSecond);
			m_kCachedTexts.PopFront();
		}
	}
}

void ChatInfoBar::_Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space, bool bMove )
{
	if(m_kFreeList.Empty())
	{
		ChatContent* pkContent = VE_NEW ChatContent(pcContent, this, f32Speed, f32Space, bMove);
		m_kUsedList.AttachBack(*pkContent);
	}
	else
	{
		ChatContent* pkContent = m_kFreeList.GetLastNode()->m_tContent;
		pkContent->Restart(pcContent, f32Speed, f32Space);
		m_kUsedList.AttachBack(*pkContent);
	}
}

ChatInfoBar::ChatContent::ChatContent(const VeChar8* pcContent, ChatInfoBar* pkParent, VeFloat32 f32Speed, VeFloat32 f32Space, bool bMove)
{
	m_tContent = this;
	m_bMove = bMove;
	//m_pkPanel = UIPanel::createWithCltBlBt("alert_event", 300.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
	//pkParent->addChild(m_pkPanel);
	//m_pkPanel->setAnchorPoint(ccp(0.0f, 0.0f));
	//m_pkPanel->setPosition(ui_w(0.25f), ui_h(-0.035f));
	//m_pkPanel->setVisible(false);

	m_pkLabel = CCLabelTTF::create(pcContent, TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale), CCSize(410.0f * g_f32ScaleHD * ui_main_scale, 50.0f * g_f32ScaleHD * ui_main_scale), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
	m_pkLabel->setAnchorPoint(ccp(0.0f, 0.0f));
	m_pkLabel->setPosition(ccp(ui_w(-0.6f), -VeFloorf(m_pkLabel->getContentSize().height * (0.45f))));

	m_pkIcon = CCSprite::create();
	m_pkIcon->setScale(ui_main_scale);
	pkParent->addChild(m_pkIcon, 0);

	m_f32Speed = f32Speed;
	m_f32Space = f32Space;
	m_f32PosY = UILayer::Width() * 1.60f;
	m_f32Time = 0;
	m_f32TimeDuration = f32Space * 0.1f;
	pkParent->addChild(m_pkLabel, 0);
	UpdatePos();
	
	ResolveContent(pcContent);
}

void ChatInfoBar::ChatContent::UpdatePos()
{
	m_pkLabel->setPositionX(VeFloorf(UILayer::Width() - m_f32PosY));
}

bool ChatInfoBar::ChatContent::IsShown()
{
	return m_f32Time >= m_f32TimeDuration;
}

void ChatInfoBar::ChatContent::Restart(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space)
{
	//m_pkLabel->setPositionY(-VeFloorf(m_pkLabel->getContentSize().height * 0.5f));
	m_f32Speed = f32Speed;
	m_f32Space = f32Space;
	m_f32PosY = UILayer::Width();
	m_f32Time = 0;
	m_f32TimeDuration = f32Space * 0.1f;
	UpdatePos();

	ResolveContent(pcContent);
}

bool ChatInfoBar::ChatContent::Update(VeFloat32 f32Delta)
{
	if(!m_bMove)
	{
		return true;
	}
	m_f32PosY += f32Delta * m_f32Speed;
	if(m_f32PosY >= (UILayer::Width() + m_pkLabel->getContentSize().width))
	{
		//m_pkLabel->setVisible(false);
		m_f32PosY = 0;
		return false;
	}
	else
	{
		UpdatePos();
		return false;
	}
	//m_f32Time += f32Delta;
	//if(m_f32Time >= m_f32TimeDuration)
	//{
	//	//m_pkLabel->setVisible(false);
	//	//m_pkPanel->setVisible(false);
	//	//m_pkIcon->setVisible(false);
	//	return true;
	//}
	//else
	//{
	//	UpdatePos();
	//	return false;
	//}
}

void ChatInfoBar::ChatContent::ResolveContent(const VeChar8* pcContent)
{
	VeString strContent = pcContent;
	if(strContent.GetAt(0) == '#')
	{
		VeString strContentTemp = strContent.GetSubstring(1, strContent.Length());
		VeUInt32 u32Index = 0;
		strContentTemp.ToUInt(u32Index);
		SetIcon(u32Index);
	}
	else
	{
		SetText(pcContent);
	}
}

void ChatInfoBar::ChatContent::SetIcon(VeUInt32 u32Index)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "head_%d.png", u32Index);
	m_pkIcon->initWithSpriteFrameName(acBuffer);

	m_pkIcon->setVisible(true);
	m_pkLabel->setVisible(false);
	//m_pkPanel->setVisible(false);
}

void ChatInfoBar::ChatContent::SetText(const VeChar8* pcContent)
{
	m_pkLabel->setString(pcContent);

	m_pkIcon->setVisible(false);
	m_pkLabel->setVisible(true);
	//m_pkPanel->setVisible(true);
}

ChatInfoChoose::ChatInfoChoose()
	: UIScrollView(650.0f * ui_main_scale * g_f32ScaleHD, 230.0f * ui_main_scale * g_f32ScaleHD,
	"scroll_bean.png", "scroll_bean_ext.png", 10.0f * g_f32ScaleHD * ui_main_scale)
{
	setPosition(ccp(ui_w(0.5f), VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 240 * g_f32ScaleHD * ui_main_scale)));
	setAnchorPoint(ccp(0.5f,1.0f));
	SetScrollBarColor(ccc3(56, 79, 124));
	SetScrollBarOpacity(128);
	SetClipByRange(true);
	UpdateBounding();
	m_u32Pointer = 0;
	m_kChatInfoBarArray.Reserve(32);
}

ChatInfoChoose::~ChatInfoChoose()
{

}

VeUInt32 ChatInfoChoose::AddChatInfoBar(const VeChar8* szChat, bool bMove)
{
	if(m_u32Pointer >= m_kChatInfoBarArray.Size())
	{
		VE_ASSERT(m_u32Pointer == m_kChatInfoBarArray.Size());
		UIPanel* pkNormal = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 408 * g_f32ScaleHD, 42 * g_f32ScaleHD);
		pkNormal->setScaleX(-ui_main_scale);
		pkNormal->setScaleY(-ui_main_scale);
		pkNormal->SetMiddleVisible(false);

		UIPanel* pkPressed = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 408 * g_f32ScaleHD, 42 * g_f32ScaleHD);
		pkPressed->setScale(ui_main_scale);
		pkPressed->SetMiddleVisible(false);

		EventButton* pkEvent = VE_NEW EventButton(m_u32Pointer, pkNormal, pkPressed);
		VeFloat32 f32Width = getContentSize().width;
		
		VeUInt32 index = m_kChatInfoBarArray.Size();
		
		pkEvent->setPosition(ccp(VeFloorf(f32Width * 0.5f + 80.50f * g_f32ScaleHD * ui_main_scale),
			- VeFloat32(index) * 32 * g_f32ScaleHD * ui_main_scale));
		pkEvent->setScale(ui_main_scale);
		AddChild(pkEvent);

		ChatInfoBar* pkBar = VE_NEW ChatInfoBar(m_kChatInfoBarArray.Size());
		//VeFloat32 f32Width = getContentSize().width;		
		pkBar->setPosition(ccp(VeFloorf(f32Width * 0.5f + 80.50f * g_f32ScaleHD * ui_main_scale),
			- VeFloat32(index) * 32 * g_f32ScaleHD * ui_main_scale));

		pkBar->setScale(ui_main_scale);
		AddChild(pkBar);
		pkBar->SetVisible(false);
		m_kChatInfoBarArray.Resize(m_kChatInfoBarArray.Size() + 1);
		Item& kItem = m_kChatInfoBarArray.Back();
		kItem.m_pkBar = pkBar;
		kItem.m_pkButton = pkEvent;
		kItem.m_kDelegate.Set(this, &ChatInfoChoose::OnClicked);
		kItem.m_pkButton->RegisterListener(kItem.m_kDelegate);
		//kItem.m_kDelegate.Set(this, &ChatInfoChoose::OnClicked);
		//kItem.m_pkBar->RegisterListener(kItem.m_kDelegate);

		//CCSprite* m_apkBoard = CCSprite::createWithSpriteFrameName("head_area.png");
		//m_apkBoard->setScale(0.7f * ui_main_scale);
		//m_apkBoard->setPositionX(VeFloorf((-90.0f + 60.0f) * g_f32ScaleHD * ui_main_scale));
		//m_apkBoard->setPositionY(VeFloorf((76.0f - 58.0f) * g_f32ScaleHD * ui_main_scale));
		//m_apkBoard->setVisible(true);
		//addChild(m_apkBoard);
	}
	
	ChatInfoBar* pkBar = m_kChatInfoBarArray[m_u32Pointer++].m_pkBar;
	pkBar->Chat(szChat, VeFloorf(50.0f * g_f32ScaleHD * ui_main_scale), VeFloorf(20.0f * g_f32ScaleHD * ui_main_scale), bMove);
	pkBar->SetVisible(true);
	UpdateBounding();
	
	return m_u32Pointer;
}

void ChatInfoChoose::SetEnable(VeUInt32 u32Index, bool bEnable, bool bHot)
{
	VE_ASSERT(u32Index < m_u32Pointer);
	ChatInfoBar* pkBar = m_kChatInfoBarArray[u32Index].m_pkBar;
	//pkBar->SetEnable(bEnable, bHot);
}

void ChatInfoChoose::ClearAll()
{
	m_u32Pointer = 0;
	for(VeVector<Item>::iterator it = m_kChatInfoBarArray.Begin(); it != m_kChatInfoBarArray.End(); ++it)
	{
		(it)->m_pkBar->SetVisible(false);
	}
	UpdateBounding();
}

void ChatInfoChoose::OnClicked(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == RELEASED)
	{
		g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
		VeUInt32 u32Index = ((EventButton&)kWidget).GetIndex();

		m_kEvent.Callback(*this, u32Index);

		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;

		VeChar8 acBuffer[256];
		VeSprintf(acBuffer, 256, "ChatList%d", u32Index);
		pkPlayer->S_ReqChatAll(4, TT(acBuffer));

		g_pLayerManager->Back();
	}
}

ChatInfoChoose::Item::Item()
{
	m_pkBar = NULL;
}
// added end

BonusCard::BonusCard() : m_bTouch(false), m_bRollBack(false)
{
	setScale(ui_main_scale);
	m_pkCard = CCSprite::createWithSpriteFrameName("bonus_card_n.png");
	addChild(m_pkCard);
	m_pkNumber = VE_NEW Number("bank", 8.0f * g_f32ScaleHD);
	AddChild(m_pkNumber);
	m_pkNumber->setPosition(ccp(-18.0f * g_f32ScaleHD, -12.0f * g_f32ScaleHD));
	setContentSize(m_pkCard->getContentSize());
	setPositionY(VeFloorf(UILayer::Height() * 0.5f - 45.0f * ui_main_scale * g_f32ScaleHD));

	m_f32Left = VeFloorf(UILayer::Width() * 0.5f - 90.0f * ui_main_scale * g_f32ScaleHD);
	m_f32Right = VeFloorf(UILayer::Width() * 0.5f + 90.0f * ui_main_scale * g_f32ScaleHD);
	m_f32Middle = VeFloorf(UILayer::Width() * 0.5f + 40.0f * ui_main_scale * g_f32ScaleHD);
	setPositionX(m_f32Left);

	schedule(schedule_selector(BonusCard::Update), 0.0f);
}

BonusCard::~BonusCard()
{

}

void BonusCard::SetLevel(VeUInt32 u32Level)
{
	m_pkNumber->SetNumber(u32Level);
}

VeUInt32 BonusCard::GetLevel()
{
	return m_pkNumber->GetNumber();
}

void BonusCard::SetHighlight(bool bEnable)
{
	if(bEnable)
	{
		m_pkCard->initWithSpriteFrameName("bonus_card_p.png");
	}
	else
	{
		m_pkCard->initWithSpriteFrameName("bonus_card_n.png");
	}
}

UIWidget* BonusCard::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
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
			s_kBeganPos = pkTouch->getLocation();
			s_f32Start = getPositionX();
			m_f32Velocity = 0;
			m_bRollBack = false;
			m_f32RollBackPos = 0;
			m_bTouch = true;
			SetHighlight(true);
			break;
		case UILayer::TOUCH_MOVED:
			if(m_bTouch)
			{
				VeFloat32 f32DeltaX = pkTouch->getLocation().x - pkTouch->getPreviousLocation().x;
				m_f32Velocity = f32DeltaX * EVENT_RATE;
				m_f32Velocity = VE_MIN(m_f32Velocity, VELOCITYMAX);
				m_f32Velocity = VE_MAX(m_f32Velocity, -VELOCITYMAX);
				s_f32Start += f32DeltaX;
				Move(s_f32Start);
			}
			break;
		case UILayer::TOUCH_ENDED:
		case UILayer::TOUCH_CANCELLED:
			m_bTouch = false;
			SetHighlight(false);
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void BonusCard::Move(VeFloat32 f32Pos)
{
	if(f32Pos >= m_f32Right || f32Pos <= m_f32Left)
	{
		m_f32Velocity = 0;
	}
	f32Pos = VE_MAX(f32Pos, m_f32Left);
	f32Pos = VE_MIN(f32Pos, m_f32Right);
	VeFloat32 f32PosPre = getPositionX();
	if(f32PosPre < m_f32Middle && f32Pos > m_f32Middle)
	{
		g_pSoundSystem->PlayEffect(SE_SLOT_CARD);
		m_kEvent.Callback(*this, CARD_EVENT_SLOT);
	}
	setPositionX(f32Pos);
}

void BonusCard::EventRelease()
{
	m_bTouch = false;
	SetHighlight(false);
}

void BonusCard::Update(VeFloat32 f32Delta)
{
	if(!m_bTouch)
	{
		if(m_f32Velocity == 0)
		{
			VeFloat32 f32Pos = getPositionX();
			if(m_bRollBack)
			{
				if(m_f32RollBackTime <= 0)
				{
					m_bRollBack = false;
					m_f32RollBackTime = 0;
				}
				else if(m_f32RollBackPos > m_f32Left)
				{
					m_f32RollBackTime -= f32Delta;
					if(m_f32RollBackTime > 0)
					{
						VeFloat32 f32Offset = VeCosf((m_f32RollBackTime / ROLLBACK_TIME) * VE_MATH_PI_2_F);
						Move(VeLerp(f32Offset, m_f32RollBackPos, m_f32Left));
					}
					else
					{
						m_f32RollBackTime = 0;
						m_bRollBack = false;
						Move(m_f32Left);
					}
				}
				else
				{
					m_bRollBack = false;
					m_f32RollBackTime = 0;
				}
			}
			else if(f32Pos > m_f32Left)
			{
				m_bRollBack = true;
				m_f32RollBackTime = ROLLBACK_TIME;
				m_f32RollBackPos = f32Pos;
			}			
		}
		else
		{
			VeFloat32 f32Pos = getPositionX();
			VeFloat32 f32Acce = FRICTION_PER_SEC * ui_main_scale * 2.0f;
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

Bank::Bank() : UILayerModalExt<Bank>("bank"), m_bLock(false), m_bCardLock(false)
{
	UIInitDelegate(Bank, OnCancel);
	UIInitDelegate(Bank, OnBonusCard);
	UIInitDelegate(Bank, OnExchangeCard);
	UIInitDelegate(Bank, OnExchangeDiamodClicked);
	UIInitDelegate(Bank, OnExchangeAntiqueClicked);
	UIInitDelegate(Bank, OnExchangeCarClicked);
	UIInitDelegate(Bank, OnExchangePlaneClicked);
	UIInitDelegate(Bank, OnCardSlot);
}

Bank::~Bank()
{
	term();
}

bool Bank::init()
{
	CCSize kSize;

	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 320.0f * ui_main_scale * g_f32ScaleHD, 280.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	CCSprite* pkBarL = CCSprite::createWithSpriteFrameName("option_title_bar.png");
	pkBarL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarL->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkBarL->setScale(ui_main_scale);
	CCSprite* pkBarR = CCSprite::createWithSpriteFrameName("option_title_bar.png");
	pkBarR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarR->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkBarR->setScaleX(-ui_main_scale);
	pkBarR->setScaleY(ui_main_scale);
	addChild(pkBarL);
	addChild(pkBarR);

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 132.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	m_pkPage = VE_NEW UIPageView;
	AddWidget("bank_view", m_pkPage);
	UIPage* pkMenu = VE_NEW UIPage;
	UIPage* pkBonus = VE_NEW UIPage;
	UIPage* pkExchange = VE_NEW UIPage;
	m_pkPage->AddPage(pkMenu);
	m_pkPage->AddPage(pkBonus);
	m_pkPage->AddPage(pkExchange);
	m_pkPage->Change(0);

	CreateMenu(pkMenu);
	CreateBonus(pkBonus);
	CreateExchange(pkExchange);

	return UILayerModalExt<Bank>::init();
}

void Bank::CreateMenu(UIWidget* pkParent)
{
	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("bank_title.png");
	pkParent->addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkTitle->setScale(ui_main_scale);

	UIPanel* pkNormalL = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 135 * g_f32ScaleHD, 180 * g_f32ScaleHD);
	pkNormalL->setScaleX(-1.0f);
	pkNormalL->setScaleY(-1.0f);
	pkNormalL->SetMiddleVisible(false);

	UIPanel* pkPressedL = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 135 * g_f32ScaleHD, 180 * g_f32ScaleHD);
	pkPressedL->setScale(1.0f);
	pkPressedL->SetMiddleVisible(false);

	UIButton* pkBonus = VE_NEW UIButton(pkNormalL, pkPressedL);
	pkBonus->setScale(ui_main_scale);
	pkBonus->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 72.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 10.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkParent->AddChild(pkBonus);

	UIPanel* pkNormalR = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 135 * g_f32ScaleHD, 180 * g_f32ScaleHD);
	pkNormalR->setScaleX(-1.0f);
	pkNormalR->setScaleY(-1.0f);
	pkNormalR->SetMiddleVisible(false);

	UIPanel* pkPressedR = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 135 * g_f32ScaleHD, 180 * g_f32ScaleHD);
	pkPressedR->setScale(1.0f);
	pkPressedR->SetMiddleVisible(false);

	UIButton* pkExchange = VE_NEW UIButton(pkNormalR, pkPressedR);
	pkExchange->setScale(ui_main_scale);
	pkExchange->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 72.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 10.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkParent->AddChild(pkExchange);

	m_pkNumber = VE_NEW UIWidget;
	pkBonus->AddChild(m_pkNumber);
	m_pkCardLevel = VE_NEW Number("bank", 8 * g_f32ScaleHD);
	m_pkCardLevel->setPosition(ccp(-18.0f * g_f32ScaleHD, -42.0f * g_f32ScaleHD));
	m_pkNumber->AddChild(m_pkCardLevel);
	m_pkNodeL = CCSpriteBatchNode::create(g_pkGame->GetImagePath("text_0", true) + ".png");
	pkBonus->addChild(m_pkNodeL, 0);
	CCSprite* pkBonusCard = CCSprite::createWithSpriteFrameName("bonus_card_n.png");
	pkBonusCard->setPositionY(VeFloorf(-30.0f * g_f32ScaleHD));
	m_pkNodeL->addChild(pkBonusCard);
	CCSprite* pkBonusTitle = CCSprite::createWithSpriteFrameName("free_bonus.png");
	pkBonusTitle->setPositionY(VeFloorf(40.0f * g_f32ScaleHD));
	m_pkNodeL->addChild(pkBonusTitle);
	UILinkDelegate(pkBonus, OnBonusCard);

	m_pkNodeR = CCSpriteBatchNode::create(g_pkGame->GetImagePath("text_0", true) + ".png");
	pkExchange->addChild(m_pkNodeR, 0);
	CCSprite* pkExchangeCard = CCSprite::createWithSpriteFrameName("exchange_card.png");
	pkExchangeCard->setPositionY(VeFloorf(-30.0f * g_f32ScaleHD));
	m_pkNodeR->addChild(pkExchangeCard);
	CCSprite* pkExchangeTitle = CCSprite::createWithSpriteFrameName("object_change.png");
	pkExchangeTitle->setPositionY(VeFloorf(40.0f * g_f32ScaleHD));
	m_pkNodeR->addChild(pkExchangeTitle);
	UILinkDelegate(pkExchange, OnExchangeCard);
}

void Bank::CreateBonus(UIWidget* pkParent)
{
	CCSpriteBatchNode* pkTextNode= CCSpriteBatchNode::create(g_pkGame->GetImagePath("text_0", true) + ".png");
	pkParent->addChild(pkTextNode, 4);

	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("free_bonus.png");
	pkTextNode->addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkTitle->setScale(ui_main_scale);

	CCSprite* pkSlot0L = CCSprite::createWithSpriteFrameName("slot_bar_0.png");
	pkSlot0L->setAnchorPoint(ccp(1.0f, 0.5f));
	pkSlot0L->setScale(ui_main_scale);
	CCSprite* pkSlot0R = CCSprite::createWithSpriteFrameName("slot_bar_0.png");
	pkSlot0R->setAnchorPoint(ccp(1.0f, 0.5f));
	pkSlot0R->setScaleX(-ui_main_scale);
	pkSlot0R->setScaleY(ui_main_scale);
	CCSpriteBatchNode* pkSlotNode0 = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	pkParent->addChild(pkSlotNode0, 1);
	pkSlotNode0->addChild(pkSlot0L);
	pkSlotNode0->addChild(pkSlot0R);
	pkSlotNode0->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - 57.0f * ui_main_scale * g_f32ScaleHD)));

	CCSprite* pkSlot1L = CCSprite::createWithSpriteFrameName("slot_bar_1.png");
	pkSlot1L->setAnchorPoint(ccp(1.0f, 0.5f));
	pkSlot1L->setScale(ui_main_scale);
	CCSprite* pkSlot1R = CCSprite::createWithSpriteFrameName("slot_bar_1.png");
	pkSlot1R->setAnchorPoint(ccp(1.0f, 0.5f));
	pkSlot1R->setScaleX(-ui_main_scale);
	pkSlot1R->setScaleY(ui_main_scale);
	CCSpriteBatchNode* pkSlotNode1 = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_0", false) + ".png");
	pkParent->addChild(pkSlotNode1, 3);
	pkSlotNode1->addChild(pkSlot1L);
	pkSlotNode1->addChild(pkSlot1R);
	pkSlotNode1->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - 80.0f * ui_main_scale * g_f32ScaleHD)));

	CCSprite* pkSlotText = CCSprite::createWithSpriteFrameName("slot_text.png");
	pkSlotText->setScale(ui_main_scale);
	pkTextNode->addChild(pkSlotText);
	pkSlotText->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - 80.0f * ui_main_scale * g_f32ScaleHD)));

	CCSprite* pkInfo = CCSprite::createWithSpriteFrameName("bank_info.png");
	pkInfo->setScale(ui_main_scale);
	pkTextNode->addChild(pkInfo);
	pkInfo->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 60.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 60.0f * ui_main_scale * g_f32ScaleHD)));

	CCSprite* pkBonusToday = CCSprite::createWithSpriteFrameName("bonus_today.png");
	pkBonusToday->setScale(ui_main_scale);
	pkTextNode->addChild(pkBonusToday);
	pkBonusToday->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 90.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 40.0f * ui_main_scale * g_f32ScaleHD)));

	m_apkState[0] = CCSprite::createWithSpriteFrameName("event_state_2.png");
	m_apkState[0]->setScale(ui_main_scale);
	pkTextNode->addChild(m_apkState[0]);
	m_apkState[0]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 90.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 70.0f * ui_main_scale * g_f32ScaleHD)));

	m_apkState[1] = CCSprite::createWithSpriteFrameName("event_state_3.png");
	m_apkState[1]->setScale(ui_main_scale);
	pkTextNode->addChild(m_apkState[1]);
	m_apkState[1]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 90.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 70.0f * ui_main_scale * g_f32ScaleHD)));

	m_apkState[2] = CCSprite::createWithSpriteFrameName("event_state_0.png");
	m_apkState[2]->setScale(ui_main_scale);
	pkTextNode->addChild(m_apkState[2]);
	m_apkState[2]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 90.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 70.0f * ui_main_scale * g_f32ScaleHD)));

	m_pkMovdCard = VE_NEW BonusCard();
	pkParent->AddChild(m_pkMovdCard, 2);
	UILinkDelegate(m_pkMovdCard, OnCardSlot);
}

void Bank::CreateExchange(UIWidget* pkParent)
{
	VeChar8 acBuffer[64];

	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("object_change.png");
	pkParent->addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 140.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkTitle->setScale(ui_main_scale);

	VeFloat32 af32PosX[4] = { -90.0f, -40.0f, 26.0f, 90.0f };

	for(VeUInt32 i(0); i < 4; ++i)
	{
		VeSprintf(acBuffer, 64, "exchange_col_%d.png", i);
		CCSprite* pkCol = CCSprite::createWithSpriteFrameName(acBuffer);
		pkCol->setScale(ui_main_scale);
		pkCol->setPositionX(VeFloorf(UILayer::Width() * 0.5f  + af32PosX[i] * ui_main_scale * g_f32ScaleHD));
		pkCol->setPositionY(VeFloorf(UILayer::Height() * 0.5f + 85.0f * ui_main_scale * g_f32ScaleHD));
		pkParent->addChild(pkCol);
	}

	for(VeUInt32 i(0); i < 4; ++i)
	{
		CCNode* pkNode = CCNode::create();
		pkParent->addChild(pkNode);
		pkNode->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + (55.0f - 40.0f * i) * ui_main_scale * g_f32ScaleHD)));
		UIBarH* pkBar = UIBarH::createWithEl("notice", 300.0f * ui_main_scale * g_f32ScaleHD);
		pkBar->setAnchorPoint(ccp(0, 0.5f));
		pkBar->setScaleY(ui_main_scale);
		pkNode->addChild(pkBar);
		VeSprintf(acBuffer, 64, "object_%d.png", i);
		CCSprite* pkObject = CCSprite::createWithSpriteFrameName(acBuffer);
		pkObject->setScale(ui_main_scale);
		pkObject->setPositionX(VeFloorf(af32PosX[0] * ui_main_scale * g_f32ScaleHD));
		pkNode->addChild(pkObject);
		m_apkObjectNum[i] = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
		m_apkObjectNum[i]->setPositionX(VeFloorf(af32PosX[1] * ui_main_scale * g_f32ScaleHD));
		pkNode->addChild(m_apkObjectNum[i]);

		CCSprite* pkChips = CCSprite::createWithSpriteFrameName("chips.png");
		pkChips->setScale(ui_main_scale);
		pkNode->addChild(pkChips);

		m_apkChangeNum[i] = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
		m_apkChangeNum[i]->setPositionX(VeFloorf(63 * ui_main_scale * g_f32ScaleHD));
		pkNode->addChild(m_apkChangeNum[i]);

		m_apkExchange[i] = VE_NEW UIButton("exchange_n.png", "exchange_p.png", "exchange_d.png");
		m_apkExchange[i]->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + af32PosX[3] * ui_main_scale * g_f32ScaleHD),
			VeFloorf(UILayer::Height() * 0.5f + (55.0f - 40.0f * i) * ui_main_scale * g_f32ScaleHD)));
		m_apkExchange[i]->setScale(ui_main_scale);
		pkParent->AddChild(m_apkExchange[i]);
	}
	UILinkDelegate(m_apkExchange[0], OnExchangeDiamodClicked);
	UILinkDelegate(m_apkExchange[1], OnExchangeAntiqueClicked);
	UILinkDelegate(m_apkExchange[2], OnExchangeCarClicked);
	UILinkDelegate(m_apkExchange[3], OnExchangePlaneClicked);
}

void Bank::SetObject(VeUInt32 u32Diamond, VeUInt32 u32Antique, VeUInt32 u32Car, VeUInt32 u32Plane)
{
	VeUInt32 au32Number[4] = { u32Diamond, u32Antique, u32Car, u32Plane };
	VeChar8 acBuffer[64];

	for(VeUInt32 i(0); i < 4; ++i)
	{
		VeSprintf(acBuffer, 64, "%d", au32Number[i]);
		m_apkObjectNum[i]->setString(acBuffer);
		const CCSize& kSize = m_apkObjectNum[i]->getContentSize();
		m_apkObjectNum[i]->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		m_apkExchange[i]->SetEnable(au32Number[i] ? true : false);
	}
}

void Bank::DisableExchange()
{
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_apkExchange[i]->SetEnable(false);
	}
}

void Bank::SetChange(VeUInt32 u32Diamond, VeUInt32 u32Antique, VeUInt32 u32Car, VeUInt32 u32Plane)
{
	VeUInt32 au32Number[4] = { u32Diamond, u32Antique, u32Car, u32Plane };
	VeChar8 acBuffer[64];

	for(VeUInt32 i(0); i < 4; ++i)
	{
		VeSprintf(acBuffer, 64, "%d", au32Number[i]);
		m_apkChangeNum[i]->setString(acBuffer);
		const CCSize& kSize = m_apkChangeNum[i]->getContentSize();
		m_apkChangeNum[i]->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
}

void Bank::Lock()
{
	m_bLock = true;
}

void Bank::Unlock()
{
	m_bLock = false;
}

void Bank::UpdateObject()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	SetObject(pkPlayer->GetObj0(), pkPlayer->GetObj1(), pkPlayer->GetObj2(), pkPlayer->GetObj3());
}

void Bank::UpdateChange(const VeVector<VeUInt32>& tObjectPriceList)
{
	VeUInt32 u32Price[4] = {0,0,0,0};
	for(VeUInt32 i(0); i < 4; ++i)
	{
		if(i < tObjectPriceList.Size())
		{
			u32Price[i] = tObjectPriceList[i];
		}
		else
		{
			break;
		}
	}
	SetChange(u32Price[0], u32Price[1], u32Price[2], u32Price[3]);
}

void Bank::UpdateBonusCard()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeUInt32 u32Level = pkPlayer->GetBankCard();
	m_pkCardLevel->SetNumber(u32Level);
	m_pkMovdCard->SetLevel(u32Level);
	m_pkMovdCard->SetVisible(u32Level ? true : false);
	if(u32Level)
	{
		bool bState = pkPlayer->GetBankCardExpire() > g_pkGame->GetServerTime();
		if(bState)
		{
			m_apkState[0]->setVisible(false);
			m_apkState[1]->setVisible(true);
			m_apkState[2]->setVisible(false);
		}
		else
		{
			m_apkState[0]->setVisible(true);
			m_apkState[1]->setVisible(false);
			m_apkState[2]->setVisible(false);
		}
	}
	else
	{
		m_apkState[0]->setVisible(false);
		m_apkState[1]->setVisible(false);
		m_apkState[2]->setVisible(true);
	}
}

void Bank::OnActive(const UIUserDataPtr& spData)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	pkPlayer->S_ReqGetObjExchangePrice();
	m_pkPage->Change(0);
	UpdateObject();
	UpdateBonusCard();
	schedule(schedule_selector(Bank::TickBonusCard), 1.0f);
}

void Bank::OnDeactive(const UIUserDataPtr& spData)
{
	unschedule(schedule_selector(Bank::TickBonusCard));
}

bool Bank::OnBack()
{
	if(m_bLock)
	{
		return false;
	}
	else if(m_pkPage->GetCurrent())
	{
		m_pkPage->Change(0);
		return false;
	}
	else
	{
		return true;
	}
}

void Bank::TickBonusCard(VeFloat32 f32Delta)
{
	UpdateBonusCard();
}

UIImplementDelegate(Bank, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Bank, OnBonusCard, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkNodeL->setScale(1.0f);
		m_pkNumber->setScale(1.0f);
	}
	else
	{
		m_pkNumber->setScale(0.99f);
		m_pkNodeL->setScale(0.99f);
	}
	if(u32State == UIWidget::RELEASED)
	{
		m_pkPage->Change(1);
	}
}

UIImplementDelegate(Bank, OnExchangeCard, kWidget, u32State)
{
	UIButton& kButton = (UIButton&)kWidget;
	if(kButton.Normal()->isVisible())
	{
		m_pkNodeR->setScale(1.0f);
	}
	else
	{
		m_pkNodeR->setScale(0.99f);
	}
	if(u32State == UIWidget::RELEASED)
	{
		m_pkPage->Change(2);
	}
}

UIImplementDelegate(Bank, OnExchangeDiamodClicked, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		DisableExchange();
		pkPlayer->S_ReqExchangeObj(0);
	}
}

UIImplementDelegate(Bank, OnExchangeAntiqueClicked, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		DisableExchange();
		pkPlayer->S_ReqExchangeObj(1);
	}
}

UIImplementDelegate(Bank, OnExchangeCarClicked, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		DisableExchange();
		pkPlayer->S_ReqExchangeObj(2);
	}
}

UIImplementDelegate(Bank, OnExchangePlaneClicked, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		DisableExchange();
		pkPlayer->S_ReqExchangeObj(3);
	}
}

UIImplementDelegate(Bank, OnCardSlot, kWidget, u32State)
{
	if(u32State == BonusCard::CARD_EVENT_SLOT)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		VeUInt32 u32Level = pkPlayer->GetBankCard();
		if(u32Level && (!m_bCardLock))
		{
			if(pkPlayer->GetBankCardExpire() < g_pkGame->GetServerTime())
			{
				m_bCardLock = true;
				pkPlayer->S_ReqSlotBonusCard();
			}
		}
	}
}

void Bank::OnBonus(VeUInt32 u32Bonus)
{
	m_bCardLock = false;
	UpdateBonusCard();
	if(u32Bonus)
	{
		g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
		VeChar8 acBuffer[64];
		VeSprintf(acBuffer, 64, "%s%d", TT("PosCardSucceed"), u32Bonus);
		g_pkGame->Toast(acBuffer);
	}
	else
	{
		g_pkGame->Toast(TT("PosCardFailed"));
	}
}

GiftItem::GiftItem(VeUInt32 u32Index, Type eType, VeUInt32 u32Kind, bool bHot)
	: UIChooseButton("drink_frame_n.png", "drink_frame_p.png")
	, m_u32Index(u32Index), m_eType(eType), m_bInTouch(false)
{
	CCSize kSize;
	VeChar8 acBuffer[64];
	if(eType == TYPE_DRINK)
	{
		VeSprintf(acBuffer, 64, "drink_%d.png", u32Kind);
		CCSprite* pkItem = CCSprite::createWithSpriteFrameName(acBuffer);
		pkItem->setPositionY(-2 * g_f32ScaleHD);
		addChild(pkItem, 5);
		VeSprintf(acBuffer, 64, "drink_%d", u32Kind);
	}
	else
	{
		VeSprintf(acBuffer, 64, "object_%d.png", u32Kind);
		CCSprite* pkItem = CCSprite::createWithSpriteFrameName(acBuffer);
		pkItem->setPositionY(-2 * g_f32ScaleHD);
		addChild(pkItem, 5);
		VeSprintf(acBuffer, 64, "object_%d", u32Kind);
	}

	CCLabelTTF* pkName = CCLabelTTF::create(TT(acBuffer), TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	pkName->setScale(1.0f / ui_main_scale);
	kSize = pkName->getContentSize();
	pkName->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkName->setPositionY(28 * g_f32ScaleHD);
	addChild(pkName, 6);

	if(bHot)
	{
		CCSprite* pkHot = CCSprite::createWithSpriteFrameName("hot_icon.png");
		pkHot->setPosition(ccp(-30 * g_f32ScaleHD, 30 * g_f32ScaleHD));
		addChild(pkHot, 5);

		pkName->setPositionX(8 * g_f32ScaleHD);
	}

	CCSprite* pkChipsIcon = CCSprite::createWithSpriteFrameName("chips.png");
	pkChipsIcon->setPosition(ccp(-26 * g_f32ScaleHD, -28 * g_f32ScaleHD));
	pkChipsIcon->setScale(0.6f);
	addChild(pkChipsIcon, 5);

	m_pkPrice = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkPrice->setScale(1.0f / ui_main_scale);
	m_pkPrice->setPosition(ccp(34 * g_f32ScaleHD, -28 * g_f32ScaleHD));
	addChild(m_pkPrice, 5);
	
	kSize = getContentSize();
	kSize.height *= 1.18f;
	setContentSize(kSize);
}

GiftItem::~GiftItem()
{

}

void GiftItem::SetPrice(VeUInt32 u32Price)
{
	CCSize kSize;
	m_pkPrice->setString(NTL(u32Price, 6));
	kSize = m_pkPrice->getContentSize();
	m_pkPrice->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

VeUInt32 GiftItem::GetIndex()
{
	return m_u32Index;
}

GiftItem::Type GiftItem::GetType()
{
	return m_eType;
}

UIWidget* GiftItem::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			m_bInTouch = true;
			break;
		case UILayer::TOUCH_ENDED:
			if(m_bInTouch)
			{
				SetChoose(true);
				m_bInTouch = false;
			}
			break;
		case UILayer::TOUCH_CANCELLED:
			m_bInTouch = false;
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

void GiftItem::EventRelease()
{
	m_bInTouch = false;
}

GameTableBoard::GameTableBoard()
{
	CCSize kSize;
	CCSprite* pkBack = CCSprite::createWithSpriteFrameName("table_board.png");
	pkBack->setScale(ui_main_scale);
	addChild(pkBack);
	CCLabelTTF* pkSBText = CCLabelTTF::create(TT("SB"), TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	kSize = pkSBText->getContentSize();
	pkSBText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkSBText->setPosition(ccp(VeFloorf(14.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(9.0f * ui_main_scale * g_f32ScaleHD)));
	addChild(pkSBText);
	CCLabelTTF* pkBBText = CCLabelTTF::create(TT("BB"), TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	kSize = pkBBText->getContentSize();
	pkBBText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkBBText->setPosition(ccp(VeFloorf(14.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(-4.0f * ui_main_scale * g_f32ScaleHD)));
	addChild(pkBBText);
	m_pkSmallBill = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkSmallBill->setPosition(ccp(VeFloorf(0.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(9.0f * ui_main_scale * g_f32ScaleHD)));
	addChild(m_pkSmallBill);
	m_pkBigBill = CCLabelTTF::create("", TTF_NAME, VeFloorf(8 * g_f32ScaleHD * ui_main_scale));
	m_pkBigBill->setPosition(ccp(VeFloorf(0.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(-4.0f * ui_main_scale * g_f32ScaleHD)));
	addChild(m_pkBigBill);
}

void GameTableBoard::Set(VeUInt32 u32SmallBill, VeUInt32 u32BigBill)
{
	CCSize kSize;
	m_pkSmallBill->setString(NTL(u32SmallBill, 5));
	kSize = m_pkSmallBill->getContentSize();
	m_pkSmallBill->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkBigBill->setString(NTL(u32BigBill, 5));
	kSize = m_pkBigBill->getContentSize();
	m_pkBigBill->setAnchorPoint(ccp(1.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

ChipsCarry::ChipsCarry() : UILayerModalExt<ChipsCarry>("chips_carry")
{
	UIInitDelegate(ChipsCarry, OnConfirm);
	UIInitDelegate(ChipsCarry, OnCancel);
	UIInitDelegate(ChipsCarry, OnChipsChange);
	m_bIsShow = false;
}

ChipsCarry::~ChipsCarry()
{
	term();
}

bool ChipsCarry::init()
{
	CCSize kSize;

	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 320.0f * ui_main_scale * g_f32ScaleHD, 156.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	CCSprite* pkBarL = CCSprite::createWithSpriteFrameName("option_title_bar.png");
	pkBarL->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarL->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 78.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkBarL->setScale(ui_main_scale);
	CCSprite* pkBarR = CCSprite::createWithSpriteFrameName("option_title_bar.png");
	pkBarR->setAnchorPoint(ccp(1.0f, 0.5f));
	pkBarR->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 78.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkBarR->setScaleX(-ui_main_scale);
	pkBarR->setScaleY(ui_main_scale);
	addChild(pkBarL);
	addChild(pkBarR);

	CCSprite* pkTitle = CCSprite::createWithSpriteFrameName("chips_carry.png");
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 78.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	pkTitle->setScale(ui_main_scale);
	addChild(pkTitle);

	UIButton* pkConfirm = VE_NEW UIButton("confirm_n.png", "confirm_p.png");
	pkConfirm->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 70.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 70.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkConfirm->setScale(ui_main_scale);
	AddWidget("confirm", pkConfirm);
	UILinkDelegate(pkConfirm, OnConfirm);

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 70.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 70.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	m_pkChipsBar = VE_NEW ChipsBar;
	m_pkChipsBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 43.0f * g_f32ScaleHD * ui_main_scale),
		VeFloorf(UILayer::Height() * 0.5f - 34.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	AddWidget("chips_bar", m_pkChipsBar);

	m_pkSlider = VE_NEW ChipsCarrySlider;
	m_pkSlider->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 4.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	AddWidget("slider", m_pkSlider);
	UILinkDelegate(m_pkSlider, OnChipsChange);
	kSize = m_pkSlider->getContentSize();
	kSize.height *= 2.0f;
	m_pkSlider->setContentSize(kSize);

	CCLabelTTF* pkMin = CCLabelTTF::create(TT("ChipsCarryMin"), TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	kSize = pkMin->getContentSize();
	pkMin->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkMin->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 120 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 2.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(pkMin, 5);

	m_pkMinNum = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkMinNum->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 120 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 18.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(m_pkMinNum, 5);

	CCLabelTTF* pkMax = CCLabelTTF::create(TT("ChipsCarryMax"), TTF_NAME, VeFloorf(14 * g_f32ScaleHD * ui_main_scale));
	kSize = pkMax->getContentSize();
	pkMax->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	pkMax->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 120 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 2.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(pkMax, 5);

	m_pkMaxNum = CCLabelTTF::create("", TTF_NAME, VeFloorf(12 * g_f32ScaleHD * ui_main_scale));
	m_pkMaxNum->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 120 * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 18.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	addChild(m_pkMaxNum, 5);

	return UILayerModalExt<ChipsCarry>::init();
}

void ChipsCarry::SetRange(VeUInt32 u32Min, VeUInt32 u32Max, VeUInt32 u32Unit)
{
	u32Min += u32Unit - 1;
	u32Min -= u32Min % u32Unit;
	u32Max -= u32Max % u32Unit;

	CCSize kSize;
	VeChar8 acBuffer[64];

	VeSprintf(acBuffer, 64, "%d", u32Min);
	m_pkMinNum->setString(acBuffer);
	kSize = m_pkMinNum->getContentSize();
	m_pkMinNum->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	VeSprintf(acBuffer, 64, "%d", u32Max);
	m_pkMaxNum->setString(acBuffer);
	kSize = m_pkMaxNum->getContentSize();
	m_pkMaxNum->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkSlider->SetRange(u32Min, u32Max, u32Unit);
}

void ChipsCarry::OnActive(const UIUserDataPtr& spData)
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	const RoomInfo& kInfo = pkPlayer->GetRoomInfo();
	const TablePlayer& kPlayer = *pkPlayer->GetTablePlayer(pkPlayer->GetSeatIndex());
	m_pkSlider->SetValue(1.0f);
	if(kInfo.m_tType == ROOM_TYPE_PRACTICE)
	{
		SetRange(kInfo.m_u32ChipsMax, kInfo.m_u32ChipsMax, kInfo.m_u32BB);
		m_pkSlider->SetEnable(false);
		m_pkChipsBar->SetVisible(false);
	}
	else
	{
		VeUInt64 u64Chips = pkPlayer->GetChips();
		VeUInt32 u32ChipsCarryNow = kPlayer.m_u32Chips;
		m_pkChipsBar->SetVisible(true);
		VeUInt32 u32Min = kInfo.m_u32ChipsMin;
		VeUInt32 u32Max = VE_MIN(kInfo.m_u32ChipsMax, u64Chips + u32ChipsCarryNow);
		VE_ASSERT(u32ChipsCarryNow <= u32Max);
		SetRange(u32Min, u32Max, kInfo.m_u32SB);
		m_pkSlider->SetEnable(true);
		m_pkChipsBar->SetNumber(u64Chips - m_pkSlider->GetCurrent());
	}
	m_bIsShow = true;
}

void ChipsCarry::OnDeactive(const UIUserDataPtr& spData)
{
	m_bIsShow = false;
}

bool ChipsCarry::OnBack()
{
	if(!g_pkHallAgent) return false;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return false;
	pkPlayer->S_ReqLeavePokerTable();
	g_pkGame->ShowLoading();
	return true;
}

UIImplementDelegate(ChipsCarry, OnConfirm, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PopAllLayers();
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		const TablePlayer& kPlayer = *pkPlayer->GetTablePlayer(pkPlayer->GetSeatIndex());
		if(kPlayer.m_tState == TABLE_PLAYER_HOLD)
		{
			pkPlayer->S_ReqReady(m_pkSlider->GetCurrent());
		}
	}
}

UIImplementDelegate(ChipsCarry, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
};

UIImplementDelegate(ChipsCarry, OnChipsChange, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		const TablePlayer& kPlayer = *pkPlayer->GetTablePlayer(pkPlayer->GetSeatIndex());
		VeUInt64 u64Chips = pkPlayer->GetChips();
		m_pkChipsBar->SetNumber(u64Chips - m_pkSlider->GetCurrent() + kPlayer.m_u32Chips);
	}
}

bool ChipsCarry::IsShow()
{
	return m_bIsShow;
}

GiftChoose::GiftChoose()
	: UIScrollView(258.0f * ui_main_scale * g_f32ScaleHD, 256.0f * ui_main_scale * g_f32ScaleHD,
	"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale)
{
	setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 134.0f * ui_main_scale * g_f32ScaleHD)));
	setAnchorPoint(ccp(0.5f,1.0f));
	SetScrollBarColor(ccc3(56, 79, 124));
	SetScrollBarOpacity(128);

	UpdateBounding();
	m_kGiftArray.Reserve(32);

	m_u32Current = VE_INFINITE;
}

GiftChoose::~GiftChoose()
{

}

VeUInt32 GiftChoose::AddGiftItem(GiftItem::Type eType, VeUInt32 u32Kind, bool bHot)
{
	VeUInt32 u32Res = m_kGiftArray.Size();
	GiftItem* pkGift = VE_NEW GiftItem(u32Res, eType, u32Kind, bHot);
	VeFloat32 f32Width = getContentSize().width;
	VeUInt32 u32Flag = u32Res % 3;
	VeUInt32 u32Row = u32Res / 3;
	pkGift->setPosition(ccp(VeFloorf(f32Width * 0.5f + (-86.0f + 83 * u32Flag) * g_f32ScaleHD * ui_main_scale),
		-VeFloorf(VeFloat32(u32Row) * 82 * g_f32ScaleHD * ui_main_scale)));

	pkGift->setScale(ui_main_scale);
	
	AddChild(pkGift);
	m_kGiftArray.Resize(m_kGiftArray.Size() + 1);
	Item& kItem = m_kGiftArray.Back();
	kItem.m_pkGift = pkGift;
	kItem.m_kDelegate.Set(this, &GiftChoose::OnClicked);
	kItem.m_pkGift->RegisterListener(kItem.m_kDelegate);
	return u32Res;
}

void GiftChoose::ClearAll()
{
	m_kChildren.BeginIterator();
	while(!m_kChildren.IsEnd())
	{
		UIWidget* pkWidget = m_kChildren.GetIterNode()->m_tContent;
		m_kChildren.Next();
		VE_ASSERT(pkWidget);
		VE_DELETE(pkWidget);
	}
	m_kChildren.Clear();
	m_kGiftArray.Clear();
}

void GiftChoose::OnClicked(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == PRESSED)
	{
		VeUInt32 u32Index = ((GiftItem&)kWidget).GetIndex();
		if(u32Index != m_u32Current)
		{
			if(m_u32Current < m_kGiftArray.Size())
			{
				m_kGiftArray[m_u32Current].m_pkGift->SetChoose(false);
			}
			m_u32Current = u32Index;
			m_kEvent.Callback(*this, VALUE_CHANGE);
		}
	}
}

void GiftChoose::Reset()
{
	if(m_u32Current < m_kGiftArray.Size())
	{
		m_kGiftArray[m_u32Current].m_pkGift->SetChoose(false);
	}
	m_u32Current = VE_INFINITE;
}

GiftItem* GiftChoose::GetCurrent()
{
	if(m_u32Current < m_kGiftArray.Size())
	{
		return m_kGiftArray[m_u32Current].m_pkGift;
	}
	else
	{
		return NULL;
	}
}

void GiftChoose::UpdateData()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VE_ASSERT(m_kGiftArray.Size() == 24);
	for(VeUInt32 i(0); i < 20; ++i)
	{
		m_kGiftArray[i].m_u32Price = pkPlayer->GetDrinkPrice()[i] * pkPlayer->GetRoomInfo().m_u32SB;
		m_kGiftArray[i].m_pkGift->SetPrice(m_kGiftArray[i].m_u32Price);
	}
	for(VeUInt32 i(20); i < 24; ++i)
	{
		m_kGiftArray[i].m_u32Price = pkPlayer->GetObjectPrice()[i - 20];
		m_kGiftArray[i].m_pkGift->SetPrice(m_kGiftArray[i].m_u32Price);
	}
}

VeUInt32 GiftChoose::GetChoosePrice()
{
	if(m_u32Current < m_kGiftArray.Size())
	{
		return m_kGiftArray[m_u32Current].m_u32Price;
	}
	else
	{
		return 0;
	}
}

GiftChoose::Item::Item()
{
	m_pkGift = NULL;
	m_u32Price = 0;
}

Canteen::Canteen() : UILayerModalExt<Canteen>("canteen")
{
	UIInitDelegate(Canteen, OnCancel);
	UIInitDelegate(Canteen, OnBuy);
	UIInitDelegate(Canteen, OnBuyAll);
	UIInitDelegate(Canteen, OnChoose);
}

Canteen::~Canteen()
{
	term();
}

bool Canteen::init()
{
	CCSize kSize;

	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 280.0f * ui_main_scale * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanel);
	m_pkPanel->setPosition(ui_ccp(0.5f, 0.5f));

	m_pkTitle = UIBarH::createWithEl("alert_title", 280.0f * ui_main_scale * g_f32ScaleHD - 16 * g_f32ScaleHD);
	m_pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(m_pkTitle);
	m_pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 180.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	m_pkTitleText = CCLabelTTF::create(TT("Gift"), TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
	addChild(m_pkTitleText, 1);
	m_pkTitleText->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 130.0f * ui_main_scale * g_f32ScaleHD + 8.0f * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 150.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));
	m_pkTitleText->setAnchorPoint(ccp(0,0));

	m_pkChipsBar = VE_NEW ChipsBar;
	m_pkChipsBar->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 50.0f * g_f32ScaleHD * ui_main_scale - 8.0f * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f + 160.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));
	AddWidget("chips_bar", m_pkChipsBar);
	//m_pkChipsBar->SetNumber(g_pkClientData->GetChips());

	UIPanel* pkCancel_n = UIPanel::createWithCltClbBtBbBl("alert_btn_1", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkCancel_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkCancel = VE_NEW UIButton(pkCancel_n, pkCancel_p);
	pkCancel->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 85.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 148.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);
	CCLabelTTF* pkCancelText = CCLabelTTF::create(TT("Cancel"), TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkCancel->addChild(pkCancelText, 5);
	kSize = pkCancelText->getContentSize();
	pkCancelText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	UIPanel* pkBuy_n = UIPanel::createWithCltClbBtBbBl("alert_btn_0", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkBuy_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkBuy_d = UIPanel::createWithCltClbBtBbBl("alert_btn_3", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkBuy = VE_NEW UIButton(pkBuy_n, pkBuy_p, pkBuy_d);
	pkBuy->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 0.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 148.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("buy", pkBuy);
	UILinkDelegate(pkBuy, OnBuy);
	pkBuy->SetEnable(false);

	m_pkBuyText = CCLabelTTF::create("", TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkBuy->addChild(m_pkBuyText, 5);

	UIPanel* pkBuyAll_n = UIPanel::createWithCltClbBtBbBl("alert_btn_0", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkBuyAll_p = UIPanel::createWithCltClbBtBbBl("alert_btn_2", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIPanel* pkBuyAll_d = UIPanel::createWithCltClbBtBbBl("alert_btn_3", 85.0f * ui_main_scale * g_f32ScaleHD, 40.0f * ui_main_scale * g_f32ScaleHD);
	UIButton* pkBuyAll = VE_NEW UIButton(pkBuyAll_n, pkBuyAll_p, pkBuyAll_d);
	pkBuyAll->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 85.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 148.0f * ui_main_scale * g_f32ScaleHD)));
	AddWidget("buy_all", pkBuyAll);
	UILinkDelegate(pkBuyAll, OnBuyAll);
	pkBuyAll->SetEnable(false);

	m_pkBuyAllText = CCLabelTTF::create("", TTF_NAME, VeFloorf(16 * g_f32ScaleHD * ui_main_scale));
	pkBuyAll->addChild(m_pkBuyAllText, 5);

	m_pkPanelInner = UIPanel::createWithCltBlBt("alert_board", 258.0f * ui_main_scale * g_f32ScaleHD, 260.0f * ui_main_scale * g_f32ScaleHD);
	addChild(m_pkPanelInner);
	m_pkPanelInner->setAnchorPoint(ccp(0.0f, 0.5f));
	m_pkPanelInner->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 136.0f * ui_main_scale * g_f32ScaleHD)));

	m_pkGiftChoose = VE_NEW GiftChoose;
	AddWidget("choose", m_pkGiftChoose);

	for(VeUInt32 i(0); i < 20; ++i)
	{
		m_pkGiftChoose->AddGiftItem(GiftItem::TYPE_DRINK, i, false);
	}
	for(VeUInt32 i(0); i < 4; ++i)
	{
		m_pkGiftChoose->AddGiftItem(GiftItem::TYPE_OBJECT, i, false);
	}
	UILinkDelegate(m_pkGiftChoose, OnChoose);
	m_pkGiftChoose->ScrollToTop();
	return UILayerModalExt<Canteen>::init();
}

void Canteen::OnActive(const UIUserDataPtr& spData)
{
	UILayerChangeParams* pkLayer = VeDynamicCast(UILayerChangeParams, spData);
	VE_ASSERT(pkLayer);
	if(pkLayer->m_u32ActiveParam == 0)
	{
		SetText(TT("Buy"), TT("BuyAll"));
	}
	else
	{
		SetText(TT("Present"), TT("BuyAll"));
	}
	m_spData = spData;
	UpdateData();
}

bool Canteen::OnBack()
{
	m_spData = NULL;
	return true;
}

void Canteen::SetText(const VeChar8* pcBuy, const VeChar8* pcBuyAll)
{
	CCSize kSize;

	m_pkBuyText->setString(pcBuy);
	kSize = m_pkBuyText->getContentSize();
	m_pkBuyText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

	m_pkBuyAllText->setString(pcBuyAll);
	kSize = m_pkBuyAllText->getContentSize();
	m_pkBuyAllText->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
}

void Canteen::UpdateButton()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	WIDGET(UIButton, buy);
	WIDGET(UIButton, buy_all);
	VeUInt32 u32Chips = m_pkGiftChoose->GetChoosePrice();
	GiftItem* pkItem = m_pkGiftChoose->GetCurrent();
	if(u32Chips)
	{
		buy_all.SetEnable(u32Chips * 10 <= pkPlayer->GetChips());
		buy.SetEnable(u32Chips <= pkPlayer->GetChips());
		if(VeDynamicCast(UILayerChangeParams, m_spData)->m_u32ActiveParam == 0)
		{
			if(pkItem->GetType() == GiftItem::TYPE_OBJECT)
			{
				buy.SetEnable(false);
			}
		}
	}
	else
	{
		buy_all.SetEnable(false);
		buy.SetEnable(false);
	}
}

UIImplementDelegate(Canteen, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Canteen, OnBuy, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		GiftItem* pkItem = m_pkGiftChoose->GetCurrent();
		VeUInt32 u32Index = pkItem->GetIndex();
		if(u32Index >= 20)
			u32Index = (u32Index - 20) | 0x80;
		pkPlayer->S_ReqBuy(u32Index, VeDynamicCast(UILayerChangeParams, m_spData)->m_u32DeactiveParam);
		// fixed me temp for test
		//pkPlayer->S_ReqChat(u32Index, VeDynamicCast(UILayerChangeParams, m_spData)->m_u32DeactiveParam, "Test chat");
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Canteen, OnBuyAll, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		GiftItem* pkItem = m_pkGiftChoose->GetCurrent();
		VeUInt32 u32Index = pkItem->GetIndex();
		if(u32Index >= 20)
			u32Index = (u32Index - 20) | 0x80;
		pkPlayer->S_ReqBuyAll(u32Index);
		// fixed me temp for test
		//pkPlayer->S_ReqChatAll(u32Index, "Test chat all");
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Canteen, OnChoose, kWidget, u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		UpdateButton();
	}
}

void Canteen::UpdateData()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	m_pkChipsBar->SetNumber(pkPlayer->GetChips());
	m_pkGiftChoose->UpdateData();
	UpdateButton();
}

GameMenu::GameMenu() : UILayerModalExt<GameMenu>("game_menu")
{
	UIInitDelegate(GameMenu, OnBackToHall);
	UIInitDelegate(GameMenu, OnChangeRoom);
	UIInitDelegate(GameMenu, OnReSit);
	UIInitDelegate(GameMenu, OnClose);
}

GameMenu::~GameMenu()
{
	term();
}

bool GameMenu::init()
{
	m_pkRoot = VE_NEW UIPage;
	m_pkRoot->setScale(ui_main_scale);
	m_pkRoot->setPosition(ccp(62.0f * ui_main_scale * g_f32ScaleHD, ui_h(1.0f)));
	AddWidget("root", m_pkRoot);

	CCSize kSize(124 * g_f32ScaleHD, 32 * g_f32ScaleHD);

	UIButton* pkBackToHall = VE_NEW UIButton(NULL, "menu_h.png");
	CCSprite* pkBackToHallBack = CCSprite::createWithSpriteFrameName("menu_t.png");
	pkBackToHallBack->setAnchorPoint(ccp(0.5f, 16.0f / 34.0f));
	pkBackToHall->addChild(pkBackToHallBack);
	CCSprite* pkBackToHallText = CCSprite::createWithSpriteFrameName("menu_0.png");
	pkBackToHall->addChild(pkBackToHallText);
	pkBackToHall->setContentSize(kSize);
	pkBackToHall->setPositionY(-16.0f * g_f32ScaleHD);
	m_pkRoot->AddChild(pkBackToHall);
	UILinkDelegate(pkBackToHall, OnBackToHall);

	UIButton* pkChangeRoom = VE_NEW UIButton(NULL, "menu_h.png");
	CCSprite* pkChangeRoomBack = CCSprite::createWithSpriteFrameName("menu_m.png");
	pkChangeRoom->addChild(pkChangeRoomBack);
	CCSprite* pkChangeRoomText = CCSprite::createWithSpriteFrameName("menu_1.png");
	pkChangeRoom->addChild(pkChangeRoomText);
	pkChangeRoom->setContentSize(kSize);
	pkChangeRoom->setPositionY(-48.0f * g_f32ScaleHD);
	m_pkRoot->AddChild(pkChangeRoom);
	UILinkDelegate(pkChangeRoom, OnChangeRoom);

	/*UIButton* pkReSit = VE_NEW UIButton(NULL, "menu_h.png");
	CCSprite* pkReSitBack = CCSprite::createWithSpriteFrameName("menu_m.png");
	pkReSit->addChild(pkReSitBack);

	CCSprite* pkReSitText = CCSprite::createWithSpriteFrameName("menu_2.png");
	pkReSit->addChild(pkReSitText);

	pkReSit->setContentSize(kSize);
	pkReSit->setPositionY(-80.0f * g_f32ScaleHD);
	m_pkRoot->AddChild(pkReSit);
	UILinkDelegate(pkReSit, OnReSit);*/

	UIButton* pkClose = VE_NEW UIButton(NULL, "menu_h.png");
	CCSprite* pkCloseBack = CCSprite::createWithSpriteFrameName("menu_t.png");
	pkCloseBack->setAnchorPoint(ccp(0.5f, 16.0f / 34.0f));
	pkCloseBack->setScaleY(-1.0f);
	pkClose->addChild(pkCloseBack);
	CCSprite* pkCloseText = CCSprite::createWithSpriteFrameName("menu_close.png");
	pkClose->addChild(pkCloseText);
	pkClose->setContentSize(kSize);
	pkClose->setPositionY(-80.0f * g_f32ScaleHD);
	m_pkRoot->AddChild(pkClose);
	UILinkDelegate(pkClose, OnClose);

	return UILayerModalExt<GameMenu>::init();
}

void GameMenu::OnActive(const UIUserDataPtr& spData)
{
	m_f32Rate = 0.0f;
	Update();
	schedule(schedule_selector(GameMenu::FadeIn), 0.0f);
}

bool GameMenu::OnBack()
{
	Close();
	return false;
}

void GameMenu::Close()
{
	unscheduleAllSelectors();
	schedule(schedule_selector(GameMenu::FadeOut), 0.0f);
}

bool GameMenu::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(m_f32Rate == 1.0f)
	{
		if(!UILayer::ProcessTouch(eType, pkTouch))
		{
			Close();
		}
	}
	return true;
}

void GameMenu::FadeIn(VeFloat32 f32Delta)
{
	m_f32Rate += f32Delta * 5.0f;
	if(m_f32Rate >= 1.0f)
	{
		m_f32Rate = 1.0f;
		unschedule(schedule_selector(GameMenu::FadeIn));
	}
	Update();
}

void GameMenu::FadeOut(VeFloat32 f32Delta)
{
	m_f32Rate -= f32Delta * 5.0f;
	if(m_f32Rate <= 0.0f)
	{
		m_f32Rate = 0.0f;
		unschedule(schedule_selector(GameMenu::FadeOut));
		g_pLayerManager->PopLayer();
	}
	Update();
}

void GameMenu::Update()
{
	VeFloat32 f32Pos = VeLerp(m_f32Rate, 130.0f, 0);
	m_pkRoot->setPositionY(VeFloorf(UILayer::Height() + f32Pos * ui_main_scale * g_f32ScaleHD));
}

UIImplementDelegate(GameMenu, OnBackToHall, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PopLayer();
		g_pLayerManager->Back();
	}
}

class ChangeRoomCallback : public UIAlert::Callback
{
public:
	ChangeRoomCallback()
	{
		m_kTitle = TT("ChangeRoomConfirmTitle");
		m_kContent = TT("ChangeRoomConfirmContent");
		m_kEnter = TT("Confirm");
		m_kCancel = TT("Cancel");
	}

	virtual void OnConfirm()
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		pkPlayer->S_ReqChangeRoom();
		g_pLayerManager->PopAllLayers();
		g_pkGame->ShowLoading();
	}
};

UIImplementDelegate(GameMenu, OnChangeRoom, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->PushLayer("alert", VE_NEW ChangeRoomCallback);
	}
}

UIImplementDelegate(GameMenu, OnReSit, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if(!g_pkHallAgent) return;
		TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
		if(!pkPlayer) return;
		const RoomInfo& kInfo = pkPlayer->GetRoomInfo();
		const TablePlayer& kPlayer = *pkPlayer->GetTablePlayer(pkPlayer->GetSeatIndex());
		if(kInfo.m_tType == ROOM_TYPE_PRACTICE) return;
		if(kPlayer.m_u32Chips >= kInfo.m_u32ChipsMax) return;
		if((kPlayer.m_u32Chips + pkPlayer->GetChips()) < kInfo.m_u32ChipsMin) return;
		pkPlayer->S_ReqAddChips();
		g_pLayerManager->PopLayer();
	}
}

UIImplementDelegate(GameMenu, OnClose, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		Close();
	}
}

GameHelp::GameHelp() : UILayerModalExt<GameHelp>("game_help")
{

}

GameHelp::~GameHelp()
{
	term();
}

bool GameHelp::init()
{
	VeChar8 acBuffer[64];

	m_pkRoot = VE_NEW UIWidget;
	m_pkRoot->setPosition(ccp(0.0f, ui_h(0.5f)));
	AddWidget("root", m_pkRoot);

	m_pkPanel = UIPanel::createWithCltBlBt("alert_frame", 200.0f * ui_main_scale * g_f32ScaleHD + 16 * g_f32ScaleHD, 350.0f * ui_main_scale * g_f32ScaleHD + 16 * g_f32ScaleHD);
	m_pkPanel->setAnchorPoint(ccp(-0.5f, 0.0f));
	m_pkRoot->addChild(m_pkPanel);

	VeFloat32 f32TextWidthMax(0);

	for(VeUInt32 i(0); i < 10; ++i)
	{
		VeSprintf(acBuffer, 64, "Categories_%d.png", i);
		CCSprite* pkCategories = CCSprite::createWithSpriteFrameName(acBuffer);
		pkCategories->setScale(ui_main_scale);
		pkCategories->setPosition(ccp(VeFloorf(48.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD),
			VeFloorf((153.0f - 34.0f * i) * ui_main_scale * g_f32ScaleHD)));
		m_pkRoot->addChild(pkCategories);

		VeSprintf(acBuffer, 64, "Categories_%d", i);
		CCLabelTTF* pkName = CCLabelTTF::create(TT(acBuffer), TTF_NAME, VeFloorf(18 * g_f32ScaleHD * ui_main_scale));
		CCSize kSize = pkName->getContentSize();
		pkName->setAnchorPoint(ccp(0.0f, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkName->setPosition(ccp(VeFloorf(102.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD),
			VeFloorf((153.0f - 34.0f * i) * ui_main_scale * g_f32ScaleHD)));
		m_pkRoot->addChild(pkName);

		f32TextWidthMax = VE_MAX(f32TextWidthMax, kSize.width);
	}

	m_pkPanel->SetSize(108.0f * ui_main_scale * g_f32ScaleHD + 16 * g_f32ScaleHD + f32TextWidthMax, 350.0f * ui_main_scale * g_f32ScaleHD + 16 * g_f32ScaleHD);

	return UILayerModalExt<GameHelp>::init();
}

void GameHelp::OnActive(const UIUserDataPtr& spData)
{
	m_f32Rate = 0.0f;
	Update();
	schedule(schedule_selector(GameHelp::FadeIn), 0.0f);
}

bool GameHelp::OnBack()
{
	Close();
	return false;
}

void GameHelp::Close()
{
	unscheduleAllSelectors();
	schedule(schedule_selector(GameHelp::FadeOut), 0.0f);
}

bool GameHelp::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(m_f32Rate == 1.0f)
	{
		if(!UILayer::ProcessTouch(eType, pkTouch))
		{
			Close();
		}
	}
	return true;
}

void GameHelp::FadeIn(VeFloat32 f32Delta)
{
	m_f32Rate += f32Delta * 5.0f;
	if(m_f32Rate >= 1.0f)
	{
		m_f32Rate = 1.0f;
		unschedule(schedule_selector(GameHelp::FadeIn));
	}
	Update();
}

void GameHelp::FadeOut(VeFloat32 f32Delta)
{
	m_f32Rate -= f32Delta * 5.0f;
	if(m_f32Rate <= 0.0f)
	{
		m_f32Rate = 0.0f;
		unschedule(schedule_selector(GameHelp::FadeOut));
		g_pLayerManager->PopLayer();
	}
	Update();
}

void GameHelp::Update()
{
	VeFloat32 f32Pos = VeLerp(m_f32Rate, -m_pkPanel->getContentSize().width, 0);
	m_pkRoot->setPositionX(f32Pos);
}

static const VeUInt32 s_au32ChipsLevel[14] =
{
	1,5,10,50,100,500.1000,5000
};

GameChips::GameChips()
{
	m_pkChipsAlert = CCSprite::createWithSpriteFrameName("board_chips_frame2.png");
	m_pkChipsAlert->setScale(ui_main_scale);
	m_pkChipsAlert->setPositionY(- 10.0f * g_f32ScaleHD * ui_main_scale);
	m_pkChipsAlert->setVisible(false);
	addChild(m_pkChipsAlert);

	m_pkNumber = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale));
	m_pkNumber->setPositionY(- 10.0f * g_f32ScaleHD * ui_main_scale);
	addChild(m_pkNumber);
	m_u32Number = 0;
	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	addChild(m_pkRoot);
}

VeUInt32 GetDigitNum(VeUInt64 u64Number);

void GameChips::SetNumber(VeUInt32 u32Number)
{
	if(m_u32Number == 0 || u32Number != m_u32Number)
	{
		ClearNumber();
		VeChar8 acBuffer[64];
		m_pkChipsAlert->setVisible(true);
		m_pkNumber->setString(NTL(u32Number, 5));
		CCSize kSize = m_pkNumber->getContentSize();
		m_pkNumber->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		VeVector<VeUInt32> kChips;
		VeUInt32 u32NumberCopy = u32Number;
		while(u32NumberCopy)
		{
			VeUInt32 u32DigitNum = GetDigitNum(u32NumberCopy);
			VeUInt32 u32Digit = VePowf(10, u32DigitNum - 1);
			VeUInt32 u32Num = u32NumberCopy / u32Digit;
			if(u32Num >= 5)
			{
				kChips.PushBack(u32Digit * 5);
				u32Num -= 5;
			}
			while(u32Num)
			{
				kChips.PushBack(u32Digit);
				--u32Num;
			}
			u32NumberCopy %= u32Digit;
		}
		VeUInt32 u32ShowChips = kChips.Size();
		for(VeUInt32 i(0); i < u32ShowChips; ++i)
		{
			VeSprintf(acBuffer, 64, "chip_%d.png", kChips[i]);
			CCSprite* pkChips = CCSprite::createWithSpriteFrameName(acBuffer);
			pkChips->setScale(ui_main_scale);
			pkChips->setPositionY((2.0f * i) * ui_main_scale * g_f32ScaleHD);
			m_pkRoot->addChild(pkChips);
			m_kChips.PushBack(pkChips);
		}
	}
}

void GameChips::SetFadePos(cocos2d::CCPoint kFadeIn, cocos2d::CCPoint kFadeOut)
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	m_kFadeInPos.x = kFadeIn.x - kTransform.tx;
	m_kFadeInPos.y = kFadeIn.y - kTransform.ty;
	m_kFadeOutPos.x = kFadeOut.x - kTransform.tx;
	m_kFadeOutPos.y = kFadeOut.y - kTransform.ty;
}

cocos2d::CCPoint GameChips::FadeInPos()
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	return ccp(m_kFadeInPos.x + kTransform.tx, m_kFadeInPos.y + kTransform.ty);
}

cocos2d::CCPoint GameChips::FadeOutPos()
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	return ccp(m_kFadeOutPos.x + kTransform.tx, m_kFadeOutPos.y + kTransform.ty);
}

void GameChips::FadeIn(VeFloat32 f32Delay)
{
	SetVisible(false);
	scheduleOnce(schedule_selector(GameChips::_FadeIn), f32Delay);
}

void GameChips::_FadeIn(VeFloat32 f32Delta)
{
	VeUInt32 u32ShowChips = m_kChips.Size();
	SetVisible(true);
	for(VeUInt32 i(0); i < u32ShowChips; ++i)
	{
		m_kChips[i]->stopAllActions();
		m_kChips[i]->setPosition(m_kFadeInPos);
		m_kChips[i]->setOpacity(0);
		m_kChips[i]->runAction(CCSequence::create(CCDelayTime::create(0.1f * i), CCFadeIn::create(0.1f), NULL));
		m_kChips[i]->runAction(CCSequence::create(CCDelayTime::create(0.1f * i), CCMoveTo::create(0.3f, ccp(0, (2.0f * i) * ui_main_scale * g_f32ScaleHD)), NULL));
	}
	m_pkChipsAlert->stopAllActions();
	m_pkChipsAlert->setOpacity(0);
	m_pkChipsAlert->setColor(ccc3(255, 255, 255));
	m_pkChipsAlert->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32ShowChips + 0.3f), CCFadeIn::create(0.1f), NULL));
	m_pkNumber->stopAllActions();
	m_pkNumber->setOpacity(0);
	m_pkNumber->setColor(ccc3(255, 255, 255));
	m_pkNumber->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32ShowChips + 0.3f), CCFadeIn::create(0.1f), NULL));
}

void GameChips::FadeOut(VeFloat32 f32Delay)
{
	scheduleOnce(schedule_selector(GameChips::_FadeOut), f32Delay);
}

void GameChips::_FadeOut(VeFloat32 f32Delta)
{
	VeUInt32 u32ShowChips = m_kChips.Size();
	SetVisible(true);
	for(VeUInt32 i(0); i < u32ShowChips; ++i)
	{
		VeUInt32 u32Inv = u32ShowChips - 1 - i;
		m_kChips[i]->stopAllActions();
		m_kChips[i]->setPosition(ccp(0, (2.0f * i) * ui_main_scale * g_f32ScaleHD));
		m_kChips[i]->setOpacity(255);
		m_kChips[i]->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32Inv + 0.2f), CCFadeOut::create(0.1f), NULL));
		m_kChips[i]->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32Inv), CCMoveTo::create(0.3f, m_kFadeOutPos), NULL));
	}
	m_pkChipsAlert->stopAllActions();
	m_pkChipsAlert->setOpacity(255);
	m_pkChipsAlert->setColor(ccc3(255, 255, 255));
	m_pkChipsAlert->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32ShowChips + 0.3f), CCFadeOut::create(0.1f), NULL));
	m_pkNumber->stopAllActions();
	m_pkNumber->setOpacity(255);
	m_pkNumber->setColor(ccc3(255, 255, 255));
	m_pkNumber->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32ShowChips + 0.3f), CCFadeOut::create(0.1f), NULL));
	scheduleOnce(schedule_selector(GameChips::_FadeOutHide), 0.1f * u32ShowChips + 0.4f);
}

void GameChips::_FadeOutHide(VeFloat32 f32Delta)
{
	SetVisible(false);
}

VeUInt32 GameChips::GetNumber()
{
	return m_u32Number;
}

void GameChips::ClearNumber()
{
	m_u32Number = 0;
	m_pkChipsAlert->setVisible(false);
	m_pkNumber->setString(NTL(0, 5));
	CCSize kSize = m_pkNumber->getContentSize();
	m_pkNumber->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_pkRoot->removeAllChildren();
	m_kChips.Clear();
}

void GameChips::ChangeNumberDelay(VeUInt32 u32Number, VeFloat32 f32Delay)
{
	m_u32NumberWaitChange = u32Number;
	scheduleOnce(schedule_selector(GameChips::_ChangeNumber), f32Delay);
}

void GameChips::_ChangeNumber(VeFloat32 f32Delta)
{
	SetNumber(m_u32NumberWaitChange);
}

void GameChips::Reset()
{
	SetNumber(0);
	SetVisible(false);
}

GameChipsMove::GameChipsMove()
{
	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	addChild(m_pkRoot);
	SetVisible(false);
}

void GameChipsMove::SetFadePos(cocos2d::CCPoint kFadeIn, cocos2d::CCPoint kFadeOut)
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	m_kFadeInPos.x = kFadeIn.x - kTransform.tx;
	m_kFadeInPos.y = kFadeIn.y - kTransform.ty;
	m_kFadeOutPos.x = kFadeOut.x - kTransform.tx;
	m_kFadeOutPos.y = kFadeOut.y - kTransform.ty;
}

void GameChipsMove::Move(VeUInt32 u32Number, VeFloat32 f32Delay)
{
	Clear();
	VeChar8 acBuffer[64];
	VeVector<VeUInt32> kChips;
	VeUInt32 u32NumberCopy = u32Number;
	while(u32NumberCopy)
	{
		VeUInt32 u32DigitNum = GetDigitNum(u32NumberCopy);
		VeUInt32 u32Digit = VePowf(10, u32DigitNum - 1);
		VeUInt32 u32Num = u32NumberCopy / u32Digit;
		if(u32Num >= 5)
		{
			kChips.PushBack(u32Digit * 5);
			u32Num -= 5;
		}
		while(u32Num)
		{
			kChips.PushBack(u32Digit);
			--u32Num;
		}
		u32NumberCopy %= u32Digit;
	}
	VeUInt32 u32ShowChips = kChips.Size();
	for(VeUInt32 i(0); i < u32ShowChips; ++i)
	{
		VeSprintf(acBuffer, 64, "chip_%d.png", kChips[i]);
		CCSprite* pkChips = CCSprite::createWithSpriteFrameName(acBuffer);
		pkChips->setScale(ui_main_scale);
		pkChips->setPositionY((2.0f * i) * ui_main_scale * g_f32ScaleHD);
		addChild(pkChips);
		m_kChips.PushBack(pkChips);
	}
	SetVisible(false);
	scheduleOnce(schedule_selector(GameChipsMove::_Move), f32Delay);
}

void GameChipsMove::_Move(VeFloat32 f32Delta)
{
	VeUInt32 u32ShowChips = m_kChips.Size();
	SetVisible(true);
	for(VeUInt32 i(0); i < u32ShowChips; ++i)
	{
		VeUInt32 u32Inv = u32ShowChips - 1 - i;
		m_kChips[i]->stopAllActions();
		m_kChips[i]->setPosition(m_kFadeInPos);
		m_kChips[i]->setOpacity(0);
		m_kChips[i]->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32Inv), CCFadeIn::create(0.1f), CCDelayTime::create(0.2f), CCFadeOut::create(0.1f), NULL));
		m_kChips[i]->runAction(CCSequence::create(CCDelayTime::create(0.1f * u32Inv), CCMoveTo::create(0.3f, m_kFadeOutPos), NULL));
	}
	scheduleOnce(schedule_selector(GameChipsMove::_MoveHide), 0.1f * u32ShowChips + 0.4f);
}

void GameChipsMove::_MoveHide(VeFloat32 f32Delta)
{
	SetVisible(false);
}

void GameChipsMove::Clear()
{
	m_pkRoot->removeAllChildren();
	m_kChips.Clear();
}

GameDrinksMove::GameDrinksMove()
{
	m_kDrinkAndObject = CCSprite::create();
	m_kDrinkAndObject->setScale(ui_main_scale);
	addChild(m_kDrinkAndObject, 1);
}

void GameDrinksMove::SetFadeInPos(cocos2d::CCPoint kFadeIn)
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	m_kFadeInPos.x = kFadeIn.x - kTransform.tx;
	m_kFadeInPos.y = kFadeIn.y - kTransform.ty;
}

void GameDrinksMove::SetFadeOutPos(cocos2d::CCPoint kFadeOut)
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	m_kFadeOutPos.x = kFadeOut.x - kTransform.tx;
	m_kFadeOutPos.y = kFadeOut.y - kTransform.ty;
}

void GameDrinksMove::Move(GiftItem::Type m_type, VeUInt32 u32Number, VeFloat32 f32Delay)
{
	VeChar8 acBuffer[64];
	unscheduleAllSelectors();
	if (m_type == GiftItem::TYPE_DRINK)
	{
		VeSprintf(acBuffer, 64, "drink_%d.png", u32Number);
		m_kDrinkAndObject->initWithSpriteFrameName(acBuffer);
	}
	else if (m_type == GiftItem::TYPE_OBJECT)
	{
		VeSprintf(acBuffer, 64, "object_%d.png", u32Number);
		m_kDrinkAndObject->initWithSpriteFrameName(acBuffer);
	}
	SetVisible(false);
	scheduleOnce(schedule_selector(GameDrinksMove::_Move), f32Delay);
}

void GameDrinksMove::_Move(VeFloat32 f32Delta)
{
	SetVisible(true);
	m_kDrinkAndObject->stopAllActions();
	m_kDrinkAndObject->setPosition(m_kFadeInPos);
	m_kDrinkAndObject->setOpacity(0);
	m_kDrinkAndObject->runAction(CCSequence::create(CCDelayTime::create(0.1f), CCFadeIn::create(0.2f), CCDelayTime::create(0.2f), CCFadeOut::create(0.2f), NULL));
	m_kDrinkAndObject->runAction(CCSequence::create(CCDelayTime::create(0.1f), CCMoveTo::create(0.6f, m_kFadeOutPos), NULL));
	scheduleOnce(schedule_selector(GameDrinksMove::_MoveHide),0.7f);
}

void GameDrinksMove::_MoveHide(VeFloat32 f32Delta)
{
	SetVisible(false);
}

const cocos2d::CCPoint& GameDrinksMove::GetFadeInPos()
{
	return m_kFadeInPos;
}

const cocos2d::CCPoint& GameDrinksMove::GetFadeOutPos()
{
	return m_kFadeOutPos;
}

ShopNumber::ShopNumber()
{
	m_f32Space = 17.0f * g_f32ScaleHD;
	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_shop", false) + ".png");
	addChild(m_pkRoot);
	Update();
}

ShopNumber::~ShopNumber()
{
	Clear();
}

void ShopNumber::SetNumber(VeUInt32 u32Number, bool bFrac)
{
	m_u32Number = u32Number;
	m_bFrac = bFrac;
	Update();
}

VeUInt32 ShopNumber::GetNumber()
{
	return m_u32Number;
}

void ShopNumber::Update()
{
	Clear();
	VeChar8 acBuffer[64];
	VeUInt32 i(0);
	VeUInt32 u32Temp = m_bFrac ? m_u32Number / 100 : m_u32Number;
	do
	{
		VeSprintf(acBuffer, 64, "shop_%d.png", u32Temp % 10);
		CCSprite* pkSprite = CCSprite::createWithSpriteFrameName(acBuffer);
		pkSprite->setPositionX(-m_f32Space * i);
		m_pkRoot->addChild(pkSprite);
		u32Temp /= 10;
		++i;
	}
	while(u32Temp);
	if(m_bFrac)
	{
		CCSprite* pkPoint = CCSprite::createWithSpriteFrameName("shop_point.png");
		pkPoint->setPosition(ccp(11.0f * g_f32ScaleHD, -6.0f * g_f32ScaleHD));
		m_pkRoot->addChild(pkPoint);

		VeUInt32 u32Frac = m_u32Number % 100;
		VeSprintf(acBuffer, 64, "shop_%d.png", u32Frac / 10);
		CCSprite* pkNum0 = CCSprite::createWithSpriteFrameName(acBuffer);
		pkNum0->setPosition(ccp(20.0f * g_f32ScaleHD, 2.0f * g_f32ScaleHD));
		pkNum0->setScale(0.8f);
		m_pkRoot->addChild(pkNum0);

		VeSprintf(acBuffer, 64, "shop_%d.png", u32Frac % 10);
		CCSprite* pkNum1 = CCSprite::createWithSpriteFrameName(acBuffer);
		pkNum1->setPosition(ccp(33.0f * g_f32ScaleHD, 2.0f * g_f32ScaleHD));
		pkNum1->setScale(0.8f);
		m_pkRoot->addChild(pkNum1);

	}
}

void ShopNumber::Clear()
{
	m_pkRoot->removeAllChildren();
}

PurchaseItem::PurchaseItem(cocos2d::CCNode* pkNormal, cocos2d::CCNode* pkPressed, VeUInt32 u32Index)
	: UIButton(pkNormal, pkPressed), m_u32Index(u32Index)
{
	SetPressedSound(VE_INFINITE);

	CCSprite* pkFlag = CCSprite::createWithSpriteFrameName("shop_flag.png");
	pkFlag->setPosition(ccp(-80.0f * g_f32ScaleHD, 8.0f * g_f32ScaleHD));
	addChild(pkFlag, 4);

	m_pkCurrency = CCSprite::create();
	m_pkCurrency->setPosition(ccp(-120.0f * g_f32ScaleHD, 16.0f * g_f32ScaleHD));
	addChild(m_pkCurrency, 4);

	m_pkIcon = CCSprite::create();
	m_pkIcon->setPosition(ccp(-96.0f * g_f32ScaleHD, -15.0f * g_f32ScaleHD));
	addChild(m_pkIcon, 4);

	m_pkVip = CCSprite::createWithSpriteFrameName("shop_vip.png");
	m_pkVip->setPosition(ccp(-96.0f * g_f32ScaleHD, -26.0f * g_f32ScaleHD));
	addChild(m_pkVip, 4);

	CCSprite* pkBuy = CCSprite::createWithSpriteFrameName("shop_buy.png");
	pkBuy->setPosition(ccp(-40.0f * g_f32ScaleHD, -18.0f * g_f32ScaleHD));
	addChild(pkBuy, 4);

	CCSprite* pkChips = CCSprite::createWithSpriteFrameName("shop_chips.png");
	pkChips->setPosition(ccp(104.0f * g_f32ScaleHD, -22.0f * g_f32ScaleHD));
	addChild(pkChips, 4);

	m_pkPrice = VE_NEW ShopNumber();
	AddChild(m_pkPrice, 4);
	m_pkPrice->setPosition(ccp(-87.0f * g_f32ScaleHD, 15.0f * g_f32ScaleHD));

	m_pkChips = VE_NEW ShopNumber();
	AddChild(m_pkChips, 4);
	m_pkChips->setPosition(ccp(80.0f * g_f32ScaleHD, -19.0f * g_f32ScaleHD));
	m_pkChips->setScale(0.8f);

	m_pkInfo = CCLabelTTF::create("", TTF_NAME, VeFloorf(12.0f * g_f32ScaleHD * ui_main_scale), CCSize(140 * ui_main_scale * g_f32ScaleHD, 36 * ui_main_scale * g_f32ScaleHD), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
	addChild(m_pkInfo, 5);
	m_pkInfo->setPosition(ccp(-20.0f * g_f32ScaleHD, 18.0f * g_f32ScaleHD));
	m_pkInfo->setScale(1.0f / ui_main_scale);
	m_pkInfo->setAnchorPoint(ccp(0.0f, 0.5f));
}

PurchaseItem::~PurchaseItem()
{

}

void PurchaseItem::Init(const VeChar8* pcInfo, VeUInt32 u32Price, PurchaseItem::Type eType, VeUInt32 u32Chips, VeUInt32 u32Icon, bool bVip)
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "shop_currency_%d.png", eType);
	m_pkCurrency->initWithSpriteFrameName(acBuffer);
	VeSprintf(acBuffer, 64, "shop_chips_%d.png", u32Icon);
	m_pkIcon->initWithSpriteFrameName(acBuffer);
	m_pkVip->setVisible(bVip);
    if(u32Price < 10000)
    {
        m_pkPrice->SetNumber(u32Price, true);
        m_pkPrice->setPositionX(-87.0f * g_f32ScaleHD);
    }
    else
    {
        m_pkPrice->SetNumber(u32Price / 100, false);
        m_pkPrice->setPositionX(-57.0f * g_f32ScaleHD);
    }
	m_pkChips->SetNumber(u32Chips, false);
	m_pkInfo->setString(pcInfo);
}

VeUInt32 PurchaseItem::GetIndex()
{
	return m_u32Index;
}

PurchaseChoose::PurchaseChoose()
	: UIScrollView(276.0f * ui_main_scale * g_f32ScaleHD, 296.0f * ui_main_scale * g_f32ScaleHD,
	"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale)
{
	setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 158.0f * ui_main_scale * g_f32ScaleHD)));
	setAnchorPoint(ccp(0.5f,1.0f));
	SetScrollBarColor(ccc3(40, 40, 40));
	SetScrollBarOpacity(128);

	UpdateBounding();
	m_u32Pointer = 0;
	m_kItemArray.Reserve(64);
}

PurchaseChoose::~PurchaseChoose()
{

}

PurchaseChoose::Item::Item()
{
	m_pkItem = NULL;
}

VeUInt32 PurchaseChoose::AddItem(const VeChar8* pcInfo, VeUInt32 u32Price, PurchaseItem::Type eType, VeUInt32 u32Chips, VeUInt32 u32Icon, bool bVip)
{
	if(m_u32Pointer >= m_kItemArray.Size())
	{
		VE_ASSERT(m_u32Pointer == m_kItemArray.Size());
		CCSpriteBatchNode* pkNormal = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_shop", false) + ".png");
		CCSprite* pkNormalL = CCSprite::createWithSpriteFrameName("shop_bar_n.png");
		pkNormalL->setAnchorPoint(ccp(1.0f, 0.5f));
		CCSprite* pkNormalR = CCSprite::createWithSpriteFrameName("shop_bar_n.png");
		pkNormalR->setAnchorPoint(ccp(1.0f, 0.5f));
		pkNormalR->setScaleX(-1.0f);
		pkNormal->addChild(pkNormalL);
		pkNormal->addChild(pkNormalR);
		pkNormal->setContentSize(CCSize(260.0f * g_f32ScaleHD, 90.0f * g_f32ScaleHD));

		CCSpriteBatchNode* pkPressed = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_shop", false) + ".png");
		CCSprite* pkPressedL = CCSprite::createWithSpriteFrameName("shop_bar_p.png");
		pkPressedL->setAnchorPoint(ccp(1.0f, 0.5f));
		CCSprite* pkPressedR = CCSprite::createWithSpriteFrameName("shop_bar_p.png");
		pkPressedR->setAnchorPoint(ccp(1.0f, 0.5f));
		pkPressedR->setScaleX(-1.0f);
		pkPressed->addChild(pkPressedL);
		pkPressed->addChild(pkPressedR);
		pkPressed->setContentSize(CCSize(260.0f * g_f32ScaleHD, 90.0f * g_f32ScaleHD));

		PurchaseItem* pkItem = VE_NEW PurchaseItem(pkNormal, pkPressed, m_u32Pointer);
		VeFloat32 f32Width = getContentSize().width;
		VeUInt32 u32Row = m_u32Pointer;
		pkItem->setPosition(ccp(VeFloorf(f32Width * 0.5f - 2.0f * g_f32ScaleHD * ui_main_scale),
			-VeFloorf(VeFloat32(u32Row) * 82 * g_f32ScaleHD * ui_main_scale)));
		pkItem->setScale(ui_main_scale);
		AddChild(pkItem);

		pkItem->SetVisible(false);
		m_kItemArray.Resize(m_kItemArray.Size() + 1);
		Item& kItem = m_kItemArray.Back();
		kItem.m_pkItem = pkItem;
		kItem.m_kDelegate.Set(this, &PurchaseChoose::OnClicked);
		kItem.m_pkItem->RegisterListener(kItem.m_kDelegate);
	}

	PurchaseItem* pkItem = m_kItemArray[m_u32Pointer++].m_pkItem;
	pkItem->Init(pcInfo, u32Price, eType, u32Chips, u32Icon, bVip);
	pkItem->SetVisible(true);

	UpdateBounding();

	return m_u32Pointer;
}

void PurchaseChoose::ClearAll()
{
	m_u32Pointer = 0;
	for(VeVector<Item>::iterator it = m_kItemArray.Begin(); it != m_kItemArray.End(); ++it)
	{
		(it)->m_pkItem->SetVisible(false);
	}
	UpdateBounding();
}

void PurchaseChoose::OnClicked(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == RELEASED)
	{
		g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
		VeUInt32 u32State = ((PurchaseItem&)kWidget).GetIndex();
		u32State |= 0x80000000;
		m_kEvent.Callback(*this, u32State);
	}
}

PlayerWin::PlayerWin()
{
	setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 1.0f - (ui_height_scale * 250 - ui_main_scale * 74) * g_f32ScaleHD)));
	setScale(ui_main_scale);

	m_pkYouWin = CCSprite::createWithSpriteFrameName("win_logo.png");
	addChild(m_pkYouWin);

	m_pkBarLeft = CCSprite::createWithSpriteFrameName("win_title.png");
	m_pkBarLeft->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkBarLeft->setPositionY(-64.0f * g_f32ScaleHD);
	addChild(m_pkBarLeft);
	m_pkBarRight = CCSprite::createWithSpriteFrameName("win_title.png");
	m_pkBarRight->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pkBarRight->setPositionY(-64.0f * g_f32ScaleHD);
	m_pkBarRight->setScaleX(-1);
	addChild(m_pkBarRight);

	m_pkChipsIcon = CCSprite::createWithSpriteFrameName("chips.png");
	m_pkChipsIcon->setPosition(ccp(-80.0f * g_f32ScaleHD, -64.0f * g_f32ScaleHD));
	addChild(m_pkChipsIcon);

	m_pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_shop", false) + ".png");
	m_pkRoot->setPosition(ccp(84.0f * g_f32ScaleHD, -64.0f * g_f32ScaleHD));
	addChild(m_pkRoot);
}

PlayerWin::~PlayerWin()
{

}

void PlayerWin::Show(VeUInt32 u32WinChips, bool bMotion)
{
	SetVisible(true);
	Reset();
	VeChar8 acBuffer[64];
	VeUInt32 i(0);
	VeUInt32 u32Temp = u32WinChips;
	do
	{
		VeSprintf(acBuffer, 64, "shop_%d.png", u32Temp % 10);
		CCSprite* pkSprite = CCSprite::createWithSpriteFrameName(acBuffer);
		pkSprite->setPositionX(-17 * g_f32ScaleHD * i);
		m_pkRoot->addChild(pkSprite);
		m_kNumber.PushBack(pkSprite);
		u32Temp /= 10;
		++i;
	}
	while(u32Temp);
	CCSprite* pkSprite = CCSprite::createWithSpriteFrameName("win_add.png");
	pkSprite->setPositionX(-17 * g_f32ScaleHD * i);
	m_pkRoot->addChild(pkSprite);
	m_kNumber.PushBack(pkSprite);
	if(bMotion)
	{
		m_pkYouWin->setPosition(ccp(0, 10 * g_f32ScaleHD));
		m_pkYouWin->setOpacity(0);
		m_pkYouWin->runAction(CCFadeIn::create(0.3f));
		m_pkYouWin->runAction(CCMoveTo::create(0.3f, ccp(0, 0)));
		m_pkBarLeft->setPosition(ccp(0, -54.0f * g_f32ScaleHD));
		m_pkBarLeft->setOpacity(0);
		m_pkBarLeft->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.3f), NULL));
		m_pkBarLeft->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCMoveTo::create(0.3f, ccp(0, -64.0f * g_f32ScaleHD)), NULL));
		m_pkBarRight->setPosition(ccp(0, -54.0f * g_f32ScaleHD));
		m_pkBarRight->setOpacity(0);
		m_pkBarRight->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.3f), NULL));
		m_pkBarRight->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCMoveTo::create(0.3f, ccp(0, -64.0f * g_f32ScaleHD)), NULL));
		m_pkChipsIcon->setPosition(ccp(-80.0f * g_f32ScaleHD, -54.0f * g_f32ScaleHD));
		m_pkChipsIcon->setOpacity(0);
		m_pkChipsIcon->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.3f), NULL));
		m_pkChipsIcon->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCMoveTo::create(0.3f, ccp(-80.0f * g_f32ScaleHD, -64.0f * g_f32ScaleHD)), NULL));
		m_pkRoot->setPosition(ccp(84.0f * g_f32ScaleHD, -54.0f * g_f32ScaleHD));;
		m_pkRoot->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCMoveTo::create(0.3f, ccp(84.0f * g_f32ScaleHD, -64.0f * g_f32ScaleHD)), NULL));
		for(VeUInt32 i(0); i < m_kNumber.Size(); ++i)
		{
			cocos2d::CCSprite* pkNumber = m_kNumber[i];
			pkNumber->setOpacity(0);
			pkNumber->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.3f), NULL));
		}
	}
	else
	{
		m_pkYouWin->setPosition(ccp(0, 0));
		m_pkBarLeft->setPosition(ccp(0, -64.0f * g_f32ScaleHD));
		m_pkBarRight->setPosition(ccp(0, -64.0f * g_f32ScaleHD));
		m_pkChipsIcon->setPosition(ccp(-80.0f * g_f32ScaleHD, -64.0f * g_f32ScaleHD));
		m_pkRoot->setPosition(ccp(84.0f * g_f32ScaleHD, -64.0f * g_f32ScaleHD));
	}
}

void PlayerWin::Reset()
{
	m_pkRoot->removeAllChildren();
	m_pkRoot->stopAllActions();
	m_kNumber.Clear();
	m_pkYouWin->stopAllActions();
	m_pkBarLeft->stopAllActions();
	m_pkBarRight->stopAllActions();
	m_pkChipsIcon->stopAllActions();
}

Shop::Shop() : UILayerModalExt<Shop>("shop")
{
	UIInitDelegate(Shop, OnCancel);
	UIInitDelegate(Shop, OnPurchase);
	UIInitDelegate(Shop, OnFreeChips);
}

Shop::~Shop()
{
	term();
}

bool Shop::init()
{
	CCSprite* pkShade = CCSprite::createWithSpriteFrameName("point.png");
	pkShade->setScaleX(UILayer::Width());
	pkShade->setScaleY(UILayer::Height());
	pkShade->setPosition(ui_ccp(0.5f, 0.5f));
	pkShade->setColor(ccc3(0, 0, 0));
	pkShade->setOpacity(128);
	addChild(pkShade);

	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 278.0f * ui_main_scale * g_f32ScaleHD + 16.0f * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	UIBarH* pkTitle = UIBarH::createWithEl("alert_title", 278.0f * ui_main_scale * g_f32ScaleHD);
	pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 205.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	CCSprite* pkShopName = CCSprite::createWithSpriteFrameName("shop_name.png");
	pkShopName->setAnchorPoint(ccp(0.5f, 0.0f));
	pkShopName->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 175.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));
	pkShopName->setScale(ui_main_scale);
	addChild(pkShopName);

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f - 100.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 172.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel, 4);
	UILinkDelegate(pkCancel, OnCancel);

	UIButton* pkFreeChips = VE_NEW UIButton("free_chips_n.png", "free_chips_p.png");
	pkFreeChips->setPosition(ccp(VeFloorf(UILayer::Width() * 0.5f + 36.0f * ui_main_scale * g_f32ScaleHD),
		VeFloorf(UILayer::Height() * 0.5f - 164.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkFreeChips->setScale(ui_main_scale);
	AddWidget("free_chips", pkFreeChips, 4);
	UILinkDelegate(pkFreeChips, OnFreeChips);

	CCSprite* pkFreeText = CCSprite::createWithSpriteFrameName("free_chips.png");
	pkFreeText->setPositionX(17.0f * g_f32ScaleHD);
	pkFreeChips->addChild(pkFreeText, 4);

	UIPanel* pkPanelInner = UIPanel::createWithCltBlBt("alert_board2", 276.0f * ui_main_scale * g_f32ScaleHD, 300.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanelInner);
	pkPanelInner->setAnchorPoint(ccp(0.0f, 0.5f));
	pkPanelInner->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 160.0f * ui_main_scale * g_f32ScaleHD)));

	PurchaseChoose* pkChoose = VE_NEW PurchaseChoose;
	AddWidget("choose", pkChoose);
	UILinkDelegate(pkChoose, OnPurchase);

	return UILayerModalExt<Shop>::init();
}

bool ShowWall();

void Shop::OnActive(const UIUserDataPtr& spData)
{
	NativeOnEventBegin("ShowShop");
	WIDGET(UIButton, free_chips);
	WIDGET(UIButton, cancel);
	if(ShowWall())
	{
		cancel.setPositionX(VeFloorf(UILayer::Width() * 0.5f - 100.0f * ui_main_scale * g_f32ScaleHD));
		free_chips.SetVisible(true);
	}
	else
	{
		cancel.setPositionX(VeFloorf(UILayer::Width() * 0.5f));
		free_chips.SetVisible(false);
	}
}

void Shop::OnDeactive(const UIUserDataPtr& spData)
{
	NativeOnEventEnd("ShowShop");
}

bool Shop::OnBack()
{
	return true;
}

PurchaseChoose& Shop::GetChoose()
{
	WIDGET(PurchaseChoose, choose);
	return choose;
}

UIImplementDelegate(Shop, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

UIImplementDelegate(Shop, OnPurchase, kWidget, u32State)
{
	if(u32State & 0x80000000)
	{
		VeUInt32 u32Index = u32State & 0x7FFFFFFF;
#		if defined(VE_PLATFORM_IOS)
		g_pLayerManager->PopAllLayers();
        g_pkGame->ShowLoading();
        CocoaHelper::OnPurchase(u32Index);
#		elif defined(VE_PLATFORM_ANDROID)
		g_pLayerManager->PopAllLayers();
		JniMethodInfo t;
		if(JniHelper::getStaticMethodInfo(t,
			"cn/VenusIE/TexasPoker/TexasPokerActivity",
			"Purchase",
			"(I)V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, u32Index);
		}
#		else
		g_pkGame->OnPurchaseEnd(-1);
#		endif
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("index", VeStringA::From(u32Index)));
		NativeOnEvent("ClickPurchase", kEventValue);
	}
}

UIImplementDelegate(Shop, OnFreeChips, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pkGame->StartRefreshPurchase();
#		if defined(VE_PLATFORM_IOS)
        CocoaHelper::ShowWall();
#		elif defined(VE_PLATFORM_ANDROID)
		g_pLayerManager->PopAllLayers();
		g_pkGame->ShowLoading();
		JniMethodInfo t;
		if(JniHelper::getStaticMethodInfo(t,
			"cn/VenusIE/TexasPoker/TexasPokerActivity",
			"jni_ShowOffersWall",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
#		else
        
#		endif
		NativeOnEvent("ShowFreeChips");
	}
}

TexasHelp::TexasHelp() : UILayerModalExt<TexasHelp>("texas_help")
{
	UIInitDelegate(TexasHelp, OnCancel);
}

TexasHelp::~TexasHelp()
{
	term();
}

bool TexasHelp::init()
{
	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 278.0f * ui_main_scale * g_f32ScaleHD + 16.0f * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	UIBarH* pkTitle = UIBarH::createWithEl("alert_title", 278.0f * ui_main_scale * g_f32ScaleHD);
	pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 205.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	UIPanel* pkPanelInner = UIPanel::createWithCltBlBt("alert_board", 276.0f * ui_main_scale * g_f32ScaleHD, 300.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanelInner);
	pkPanelInner->setAnchorPoint(ccp(0.0f, 0.5f));
	pkPanelInner->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 160.0f * ui_main_scale * g_f32ScaleHD)));

	pkHelpScrollView = VE_NEW UIScrollView(276.0f * ui_main_scale * g_f32ScaleHD, 296.0f * ui_main_scale * g_f32ScaleHD,
		"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale);
	pkHelpScrollView->setPosition(ccp(ui_w(0.5f),VeFloorf(UILayer::Height() * 0.5f + 158.0f * ui_main_scale * g_f32ScaleHD)));
	pkHelpScrollView->setAnchorPoint(ccp(0.5f,1.0f));
	pkHelpScrollView->SetScrollBarColor(ccc3(40, 40, 40));
	pkHelpScrollView->SetScrollBarOpacity(128);
	pkHelpScrollView->UpdateBounding();
	AddWidget("help_scroll_view",pkHelpScrollView);

	VeFloat32 f32PosX = VeFloorf(pkHelpScrollView->getContentSize().width * 0.5f);

	UIWidget* pkNode = VE_NEW UIWidget;
	pkNode->setScale(ui_main_scale);
	CCSprite* pkHelpContent = CCSprite::createWithSpriteFrameName("help_content.png");
	pkNode->addChild(pkHelpContent);
	CCSize m_SpriteSize = pkHelpContent->getContentSize();
	m_SpriteSize.height += 50.0f * g_f32ScaleHD;
	pkNode->setContentSize(m_SpriteSize);
	pkNode->setPositionX(f32PosX);
	pkHelpScrollView->AddChild(pkNode);
	pkHelpScrollView->ScrollToTop();

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 172.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	CCSprite* pkShopName = CCSprite::createWithSpriteFrameName("game_logo_mini.png");
	pkShopName->setAnchorPoint(ccp(0.5f, 0.0f));
	pkShopName->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 167.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));
	pkShopName->setScale(ui_main_scale * 0.9f);
	addChild(pkShopName);

	return UILayerModalExt<TexasHelp>::init();

}

void TexasHelp::OnActive(const UIUserDataPtr& spData)
{
	pkHelpScrollView->ScrollToTop();
}

bool TexasHelp::OnBack()
{
	return true;
}

UIImplementDelegate(TexasHelp, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

void TexasHelp::visit( void )
{
	UILayerModalExt<TexasHelp>::visit();
}

LottoHelp::LottoHelp() : UILayerModalExt<LottoHelp>("lotto_help")
{
	UIInitDelegate(LottoHelp, OnCancel);
}

LottoHelp::~LottoHelp()
{
	term();
}

bool LottoHelp::init()
{
	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 278.0f * ui_main_scale * g_f32ScaleHD + 16.0f * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	UIBarH* pkTitle = UIBarH::createWithEl("alert_title", 278.0f * ui_main_scale * g_f32ScaleHD);
	pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 205.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	UIPanel* pkPanelInner = UIPanel::createWithCltBlBt("alert_board", 276.0f * ui_main_scale * g_f32ScaleHD, 300.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanelInner);
	pkPanelInner->setAnchorPoint(ccp(0.0f, 0.5f));
	pkPanelInner->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 160.0f * ui_main_scale * g_f32ScaleHD)));

	pkHelpScrollView = VE_NEW UIScrollView(276.0f * ui_main_scale * g_f32ScaleHD, 296.0f * ui_main_scale * g_f32ScaleHD,
		"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale);
	pkHelpScrollView->setPosition(ccp(ui_w(0.5f),VeFloorf(UILayer::Height() * 0.5f + 158.0f * ui_main_scale * g_f32ScaleHD)));
	pkHelpScrollView->setAnchorPoint(ccp(0.5f,1.0f));
	pkHelpScrollView->SetScrollBarColor(ccc3(40, 40, 40));
	pkHelpScrollView->SetScrollBarOpacity(128);
	pkHelpScrollView->UpdateBounding();
	AddWidget("help_scroll_view",pkHelpScrollView);

	VeFloat32 f32PosX = VeFloorf(pkHelpScrollView->getContentSize().width * 0.5f);

	UIWidget* pkNode = VE_NEW UIWidget;
	pkNode->setScale(ui_main_scale);
	CCSprite* pkHelpContent = CCSprite::createWithSpriteFrameName("help_keno.png");
	pkNode->addChild(pkHelpContent);
	CCSize m_SpriteSize = pkHelpContent->getContentSize();
	m_SpriteSize.height += 60.0f * g_f32ScaleHD;
	pkNode->setContentSize(m_SpriteSize);
	pkNode->setPositionX(f32PosX);
	pkHelpScrollView->AddChild(pkNode);
	pkHelpScrollView->ScrollToTop();

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 172.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	CCSprite* pkShopName = CCSprite::createWithSpriteFrameName("nini_lotto_n.png");
	pkShopName->setAnchorPoint(ccp(0.5f, 0.0f));
	pkShopName->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 167.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));
	pkShopName->setScale(ui_main_scale * 0.9f);
	addChild(pkShopName);

	return UILayerModalExt<LottoHelp>::init();
}

void LottoHelp::OnActive(const UIUserDataPtr& spData)
{
	pkHelpScrollView->ScrollToTop();
}

bool LottoHelp::OnBack()
{
	return true;
}

UIImplementDelegate(LottoHelp, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

BaccHelp::BaccHelp() : UILayerModalExt<BaccHelp>("bacc_help")
{
	UIInitDelegate(BaccHelp, OnCancel);
}

BaccHelp::~BaccHelp()
{
	term();
}

bool BaccHelp::init()
{
	UIPanel* pkPanel = UIPanel::createWithCltBlBt("alert_frame", 278.0f * ui_main_scale * g_f32ScaleHD + 16.0f * g_f32ScaleHD, 360.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanel);
	pkPanel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + 25.0f * ui_main_scale * g_f32ScaleHD)));

	UIBarH* pkTitle = UIBarH::createWithEl("alert_title", 278.0f * ui_main_scale * g_f32ScaleHD);
	pkTitle->setAnchorPoint(ccp(0.0f, 1.0f));
	addChild(pkTitle);
	pkTitle->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 205.0f * ui_main_scale * g_f32ScaleHD - 8 * g_f32ScaleHD)));

	UIPanel* pkPanelInner = UIPanel::createWithCltBlBt("alert_board", 276.0f * ui_main_scale * g_f32ScaleHD, 300.0f * ui_main_scale * g_f32ScaleHD);
	addChild(pkPanelInner);
	pkPanelInner->setAnchorPoint(ccp(0.0f, 0.5f));
	pkPanelInner->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 160.0f * ui_main_scale * g_f32ScaleHD)));

	pkHelpScrollView = VE_NEW UIScrollView(276.0f * ui_main_scale * g_f32ScaleHD, 296.0f * ui_main_scale * g_f32ScaleHD,
		"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale);
	pkHelpScrollView->setPosition(ccp(ui_w(0.5f),VeFloorf(UILayer::Height() * 0.5f + 158.0f * ui_main_scale * g_f32ScaleHD)));
	pkHelpScrollView->setAnchorPoint(ccp(0.5f,1.0f));
	pkHelpScrollView->SetScrollBarColor(ccc3(40, 40, 40));
	pkHelpScrollView->SetScrollBarOpacity(128);
	pkHelpScrollView->UpdateBounding();
	AddWidget("help_scroll_view",pkHelpScrollView);

	VeFloat32 f32PosX = VeFloorf(pkHelpScrollView->getContentSize().width * 0.5f);

	UIWidget* pkNode = VE_NEW UIWidget;
	pkNode->setScale(ui_main_scale);
	CCSprite* pkHelpContent = CCSprite::createWithSpriteFrameName("help_baccarat.png");
	pkNode->addChild(pkHelpContent);
	CCSize m_SpriteSize = pkHelpContent->getContentSize();
	m_SpriteSize.height += 60.0f * g_f32ScaleHD;
	pkNode->setContentSize(m_SpriteSize);
	pkNode->setPositionX(f32PosX);
	pkHelpScrollView->AddChild(pkNode);
	pkHelpScrollView->ScrollToTop();

	UIButton* pkCancel = VE_NEW UIButton("cancel_n.png", "cancel_p.png");
	pkCancel->setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f - 172.0f * ui_main_scale * g_f32ScaleHD + 8 * g_f32ScaleHD)));
	pkCancel->setScale(ui_main_scale);
	AddWidget("cancel", pkCancel);
	UILinkDelegate(pkCancel, OnCancel);

	CCSprite* pkShopName = CCSprite::createWithSpriteFrameName("mini_baccarat_n.png");
	pkShopName->setAnchorPoint(ccp(0.5f, 0.0f));
	pkShopName->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f + 167.0f * ui_main_scale * g_f32ScaleHD - 8.0f * g_f32ScaleHD)));
	pkShopName->setScale(ui_main_scale * 0.9f);
	addChild(pkShopName);

	return UILayerModalExt<BaccHelp>::init();

}

void BaccHelp::OnActive(const UIUserDataPtr& spData)
{
	pkHelpScrollView->ScrollToTop();
}

bool BaccHelp::OnBack()
{
	return true;
}

UIImplementDelegate(BaccHelp, OnCancel, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		g_pLayerManager->Back();
	}
}

RankList::RankList()
{
	setPosition(ccp(ui_w(0.5f),
		VeFloorf(UILayer::Height() * 0.5f + (72.0f * ui_height_scale + 80.0f * ui_main_scale) * g_f32ScaleHD)));
	setContentSize(CCSize(290 * g_f32ScaleHD * ui_main_scale, 25 * g_f32ScaleHD * ui_main_scale));
	SetClipByRange(true);
	CCSize kSize;
	for (VeUInt32 i(0);i < 3;++i)
	{
		m_apkRankName[i] = CCLabelTTF::create("", TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_apkRankName[i]->setPosition(ccp(VeFloorf(-92.0f * g_f32ScaleHD * ui_main_scale),
			VeFloorf((3.0f - 13.0f * i) * ui_main_scale * g_f32ScaleHD)));
		addChild(m_apkRankName[i]);
		kSize = m_apkRankName[i]->getContentSize();
		m_apkRankName[i]->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

		m_apkChipList[i] = CCLabelTTF::create("", TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_apkChipList[i]->setPosition(ccp(VeFloorf(94.0f * g_f32ScaleHD * ui_main_scale),
			VeFloorf((3.0f - 13.0f * i) * ui_main_scale * g_f32ScaleHD)));
		addChild(m_apkChipList[i]);
		kSize = m_apkChipList[i]->getContentSize();
		m_apkChipList[i]->setAnchorPoint(ccp(VeFloorf(kSize.width * 1.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	}
	m_f32Move = 0;
	m_u32Pointer = 0;
}

void RankList::visit()
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
		kRect.origin.y -= VeFloorf(3.0f * g_f32ScaleHD * ui_main_scale); 
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

void RankList::UpdatePos(VeFloat32 f32Delta)
{
	m_f32Move += f32Delta * 0.5f;
	if(m_f32Move >= 1.0f)
	{
		m_f32Move -= 1.0f;
		m_u32Pointer = (m_u32Pointer + 1) % m_kLeaderboard.Size();
	}
	UpdateMove();
	UpdateInfo();
}

void RankList::GetRankData( const VeVector< VePair<VeStringA,VeStringA> >& kLeaderboard )
{
	m_kLeaderboard = kLeaderboard;
	m_f32Move = 0;
	m_u32Pointer = 0;
	if(m_kLeaderboard.Size() < 3)
	{
		unscheduleAllSelectors();
	}
	else
	{
		schedule(schedule_selector(RankList::UpdatePos), 0);
	}
	UpdateInfo();
}

void RankList::UpdateRankInfo(VeUInt32 u32TextIndex ,VeUInt32 u32LeaderboardIndex)
{
	CCSize kSize;
	VeChar8 acTestRankNameInfo[1024];
	VeChar8 acTestRankChipInfo[1024];
	VeStringA kName = m_kLeaderboard[u32LeaderboardIndex].m_tFirst;
	VeStringA kChip = m_kLeaderboard[u32LeaderboardIndex].m_tSecond;
	VeSprintf(acTestRankNameInfo, 1024, "%d. %s", u32LeaderboardIndex + 1, (const VeChar8*)kName);
	VeSprintf(acTestRankChipInfo, 1024, "%s", (const VeChar8*)kChip);
	m_apkRankName[u32TextIndex]->setString(acTestRankNameInfo);
	kSize = m_apkRankName[u32TextIndex]->getContentSize();
	m_apkRankName[u32TextIndex]->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
	m_apkChipList[u32TextIndex]->setString(acTestRankChipInfo);
	kSize = m_apkChipList[u32TextIndex]->getContentSize();
	m_apkChipList[u32TextIndex]->setAnchorPoint(ccp(VeFloorf(kSize.width * 1.0f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));

}

void RankList::UpdateMove()
{
	for (VeUInt32 i(0);i < 3;++i)
	{
		VeFloat32 f32PosY = VeFloorf((3.0f - 13.0f * (VeFloat32(i) - m_f32Move)) * ui_main_scale * g_f32ScaleHD);
		m_apkRankName[i]->setPositionY(f32PosY);
		m_apkChipList[i]->setPositionY(f32PosY);
	}
}

void RankList::UpdateInfo()
{
	for(VeUInt32 i(0); i < 3 && i < m_kLeaderboard.Size(); ++i)
	{
		UpdateRankInfo(i, (m_u32Pointer + i) % m_kLeaderboard.Size());
	}
}
