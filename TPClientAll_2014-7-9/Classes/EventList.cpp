#include "EventList.h"
#include "TexasPoker.h"
#include "UIAlert.h"
#include "SoundSystem.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPokerHallPlayerC.h"
#include "Hall.h"
#include "NativeInterface.h"

USING_NS_CC;

EventButton::EventButton(VeUInt32 u32Index, UIPanel* pkNormal, UIPanel* pkPressed)
	: UIButton(pkNormal, pkPressed), m_u32Index(u32Index)
{
	SetPressedSound(VE_INFINITE);

	m_pkRoot = CCNode::create();
	addChild(m_pkRoot);

	m_pkIcon = CCSprite::create();
	m_pkIcon->setScale(ui_main_scale);
	m_pkIcon->setPositionX(-VeFloorf(125 * g_f32ScaleHD * ui_main_scale));
	m_pkRoot->addChild(m_pkIcon);

	m_pkTitle = CCSprite::create();
	m_pkTitle->setScale(ui_main_scale);
	m_pkTitle->setPosition(ccp(-VeFloorf(90 * g_f32ScaleHD * ui_main_scale), VeFloorf(18 * g_f32ScaleHD * ui_main_scale)));
	m_pkRoot->addChild(m_pkTitle);

	m_pkState = CCSprite::create();
	m_pkState->setScale(ui_main_scale);
	m_pkState->setPositionX(VeFloorf(120 * g_f32ScaleHD * ui_main_scale));
	m_pkRoot->addChild(m_pkState);

	CCSize kSize(UILayer::Width(), 66 * g_f32ScaleHD * ui_main_scale);
	setContentSize(kSize);

	m_pkInfo = CCLabelTTF::create("", TTF_NAME, VeFloorf(10 * g_f32ScaleHD * ui_main_scale),
		CCSize(172.0f * g_f32ScaleHD * ui_main_scale, 40.0f * g_f32ScaleHD * ui_main_scale), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
	m_pkInfo->setAnchorPoint(ccp(0.0f, 1.0f));
	m_pkInfo->setPosition(ccp(-VeFloorf(90 * g_f32ScaleHD * ui_main_scale), VeFloorf(11 * g_f32ScaleHD * ui_main_scale)));
	m_pkRoot->addChild(m_pkInfo);
}

EventButton::~EventButton()
{

}

void EventButton::Init(Type eType, State eState, const VeChar8* pcInfo)
{
	CCSize kSize;
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "event_icon_%d.png", eType);
	m_pkIcon->initWithSpriteFrameName(acBuffer);
	VeSprintf(acBuffer, 64, "event_title_%d.png", eType);
	m_pkTitle->initWithSpriteFrameName(acBuffer);
	m_pkTitle->setAnchorPoint(ccp(0.0f, 0.5f));
	VeSprintf(acBuffer, 64, "event_state_%d.png", eState);
	m_pkState->initWithSpriteFrameName(acBuffer);
	m_pkInfo->setString(pcInfo);
	if(eState == STATE_COMPLETED || eState == STATE_OPENING)
	{
		SetEnable(true);
	}
	else
	{
		SetEnable(false);
	}
}

VeUInt32 EventButton::GetIndex()
{
	return m_u32Index;
}

void EventButton::OnEvent(UIWidget& kWidget, VeUInt32 u32State)
{
	UIButton::OnEvent(kWidget, u32State);
	if(m_pkPressed->isVisible())
	{
		m_pkRoot->setScale(0.99f);
	}
	else
	{
		m_pkRoot->setScale(1.0f);
	}
}

EventList::EventList()
	: UIScrollView(320.0f * ui_main_scale * g_f32ScaleHD, 231.0f * ui_main_scale * g_f32ScaleHD,
	"scroll_bean.png", "scroll_bean_ext.png", 3.0f * g_f32ScaleHD * ui_main_scale)
{
	setPosition(ccp(ui_w(0.5f), VeFloorf(104.0f * g_f32ScaleHD * ui_height_scale + 240 * g_f32ScaleHD * ui_main_scale)));
	setAnchorPoint(ccp(0.5f,1.0f));
	SetScrollBarColor(ccc3(56, 79, 124));
	SetScrollBarOpacity(128);
	UpdateBounding();
	m_u32Pointer = 0;
	m_kEventButtonArray.Reserve(128);
}

EventList::~EventList()
{

}

VeUInt32 EventList::AddEventButton(EventButton::Type eType, EventButton::State eState, const VeChar8* pcInfo)
{
	if(m_u32Pointer >= m_kEventButtonArray.Size())
	{
		VE_ASSERT(m_u32Pointer == m_kEventButtonArray.Size());

		UIPanel* pkNormal = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 316 * g_f32ScaleHD, 64 * g_f32ScaleHD);
		pkNormal->setScaleX(-ui_main_scale);
		pkNormal->setScaleY(-ui_main_scale);
		pkNormal->SetMiddleVisible(false);

		UIPanel* pkPressed = UIPanel::createWithCltClbCrtCrbBtBbBlBr("alert_event", 316 * g_f32ScaleHD, 64 * g_f32ScaleHD);
		pkPressed->setScale(ui_main_scale);
		pkPressed->SetMiddleVisible(false);

		EventButton* pkEvent = VE_NEW EventButton(m_u32Pointer, pkNormal, pkPressed);
		VeFloat32 f32Width = getContentSize().width;
		VeUInt32 u32Row = m_u32Pointer;
		
		pkEvent->setPosition(ccp(VeFloorf(f32Width * 0.5f),
			-VeFloorf(VeFloat32(u32Row) * 66 * g_f32ScaleHD * ui_main_scale)));
		AddChild(pkEvent);
		pkEvent->SetVisible(false);
		m_kEventButtonArray.Resize(m_kEventButtonArray.Size() + 1);
		Item& kItem = m_kEventButtonArray.Back();
		kItem.m_pkButton = pkEvent;
		kItem.m_kDelegate.Set(this, &EventList::OnClicked);
		kItem.m_pkButton->RegisterListener(kItem.m_kDelegate);
	}

	EventButton* pkEvent = m_kEventButtonArray[m_u32Pointer++].m_pkButton;
	pkEvent->Init(eType, eState, pcInfo);
	pkEvent->SetVisible(true);
	UpdateBounding();

	return m_u32Pointer;
}

void EventList::ClearAll()
{
	m_u32Pointer = 0;
	for(VeVector<Item>::iterator it = m_kEventButtonArray.Begin(); it != m_kEventButtonArray.End(); ++it)
	{
		(it)->m_pkButton->SetVisible(false);
	}
	UpdateBounding();
}

void EventList::OnClicked(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == RELEASED)
	{
		g_pSoundSystem->PlayEffect(SE_BTN_CLICK);
		VeUInt32 u32Index = ((EventButton&)kWidget).GetIndex();
		if(u32Index < m_kEventList.Size())
		{
			if(m_kEventList[u32Index].m_eState == EventButton::STATE_OPENING)
			{
				switch(m_kEventList[u32Index].m_eAction)
				{
				case ACT_EDIT_INFO:
					g_pLayerManager->PushLayer("edit_info");
					break;
				case ACT_OPEN_SHOP:
					g_pLayerManager->PushLayer("shop");
					break;
				case ACT_PLAY_POKER:
					m_kEvent.Callback(kWidget, 1000);
					break;
				case ACT_PLAY_SLOT:
					m_kEvent.Callback(kWidget, 1001);
					break;
				case ACT_PLAY_KENO:
					m_kEvent.Callback(kWidget, 1002);
					break;
				case ACT_PLAY_BACC:
					m_kEvent.Callback(kWidget, 1003);
					break;
				default:
					break;
				}
			}
			else if(m_kEventList[u32Index].m_eState == EventButton::STATE_COMPLETED)
			{
				if(!g_pkHallAgent) return;
				TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
				if(!pkPlayer) return;
				g_pkGame->ShowLoading();
				pkPlayer->S_ReqFinishEvent(m_kEventList[u32Index].m_eInfo);
				VeVector< VePair<VeStringA,VeStringA> > kValue;
				kValue.Resize(1);
				kValue[0].m_tFirst = "type";
				switch(m_kEventList[u32Index].m_eInfo)
				{
				case EVENT_MODIFY:
					kValue[0].m_tSecond = "EVENT_MODIFY";
					break;
				case EVENT_PURCHASE:
					kValue[0].m_tSecond = "EVENT_PURCHASE";
					break;
				case EVENT_VIP:
					kValue[0].m_tSecond = "EVENT_VIP";
					break;
				case EVENT_PRESENT:
					kValue[0].m_tSecond = "EVENT_PRESENT";
					break;
				case EVENT_ONLINE:
					kValue[0].m_tSecond = "EVENT_ONLINE";
					break;
				case EVENT_PLAY_COUNT:
					kValue[0].m_tSecond = "EVENT_PLAY_COUNT";
					break;
				case EVENT_PLAY_SLOT_COUNT:
					kValue[0].m_tSecond = "EVENT_PLAY_SLOT_COUNT";
					break;
				case EVENT_PLAY_KENO_COUNT:
					kValue[0].m_tSecond = "EVENT_PLAY_KENO_COUNT";
					break;
				case EVENT_PLAY_BACC_COUNT:
					kValue[0].m_tSecond = "EVENT_PLAY_BACC_COUNT";
					break;
				default:
					kValue[0].m_tSecond = "EventInfo_MAX";
					break;
				}
				NativeOnEvent("ReqFinishEvent", kValue);
			}
		}
	}
}

VeUInt32 SortEventList(VeVector<EventList::Event>& kDst, VeVector<EventList::Event>& kSrc)
{
	VeUInt32 u32Res(0);
	kDst.Clear();
	for(VeVector<EventList::Event>::iterator it = kSrc.Begin(); it != kSrc.End(); ++it)
	{
		if(it->m_eState == EventButton::STATE_COMPLETED)
		{
			kDst.PushBack(*it);
			++u32Res;
		}
	}
	for(VeVector<EventList::Event>::iterator it = kSrc.Begin(); it != kSrc.End(); ++it)
	{
		if(it->m_eState == EventButton::STATE_OPENING)
		{
			kDst.PushBack(*it);
		}
	}
	for(VeVector<EventList::Event>::iterator it = kSrc.Begin(); it != kSrc.End(); ++it)
	{
		if(it->m_eState == EventButton::STATE_CLOSED)
		{
			kDst.PushBack(*it);
		}
	}
	for(VeVector<EventList::Event>::iterator it = kSrc.Begin(); it != kSrc.End(); ++it)
	{
		if(it->m_eState == EventButton::STATE_FINISHED)
		{
			kDst.PushBack(*it);
		}
	}
	return u32Res;
}

void UpdateModifyEvent(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = EventButton::TYPE_SPECIAL;
	kEvent.m_eState = EventButton::STATE_FINISHED;
	if(kPlayer.GetEventModifyInfo() == 0)
	{
		if(kPlayer.GetPhone() == "")
		{
			kEvent.m_eState = EventButton::STATE_OPENING;
		}
		else
		{
			kEvent.m_eState = EventButton::STATE_COMPLETED;
		}
	}
	kEvent.m_eAction = EventList::ACT_EDIT_INFO;
	kEvent.m_kInfo += TT("EventInfoTarget0");
	kEvent.m_kInfo += "\n";
	kEvent.m_kInfo += TT("EventInfoAward0");
	kEvent.m_eInfo = EVENT_MODIFY;
}

void UpdateEventPurchase(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	VeUInt32 u32Level = kPlayer.GetEventPurchase();
	VeUInt32 u32PurchaseChips(0);
	switch(u32Level)
	{
	case 0:
		u32PurchaseChips = 10;
		break;
	case 1:
		u32PurchaseChips = 50;
		break;
	case 2:
		u32PurchaseChips = 250;
		break;
	case 3:
		u32PurchaseChips = 1250;
		break;
	case 4:
		u32PurchaseChips = 6250;
		break;
	default:
		return;
	}
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = EventButton::TYPE_SPECIAL;
	kEvent.m_eState = (kPlayer.GetPurchaseChips() >= (u32PurchaseChips * 1000)) ?
		EventButton::STATE_COMPLETED : EventButton::STATE_OPENING;
	kEvent.m_eAction = EventList::ACT_OPEN_SHOP;
	kEvent.m_kInfo = TT("EventInfoTarget1")
		+ VeStringA::From(u32PurchaseChips)
		+ "k(" + VeStringA::From(VeUInt32(kPlayer.GetPurchaseChips() / 1000)) + "k)\n";
	kEvent.m_kInfo += TT("EventInfoAward1");
	kEvent.m_kInfo += TT(VeStringA("TestBankDesc") + VeStringA::From(u32Level+1));
	kEvent.m_eInfo = EVENT_PURCHASE;
}

void UpdateEventVip(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = EventButton::TYPE_VIP;
	if(kPlayer.GetVip() == 0)
	{
		kEvent.m_eState = kPlayer.GetEventVip() ? EventButton::STATE_COMPLETED : EventButton::STATE_OPENING;
	}
	else
	{
		kEvent.m_eState = EventButton::STATE_FINISHED;
	}
	kEvent.m_eAction = EventList::ACT_OPEN_SHOP;
	kEvent.m_kInfo += TT("EventInfoTarget2");
	kEvent.m_kInfo += "\n";
	kEvent.m_kInfo += TT("EventInfoAward2");
	kEvent.m_eInfo = EVENT_VIP;
}

void UpdateEventPresent(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = EventButton::TYPE_SPECIAL;
	if(kPlayer.GetPresentSend() >= 100000)
	{
		kEvent.m_eState = EventButton::STATE_COMPLETED;
	}
	else
	{
		kEvent.m_eState = EventButton::STATE_CLOSED;
	}
	kEvent.m_eAction = EventList::ACT_NULL;
	kEvent.m_kInfo += TT("EventInfoTarget3");
	kEvent.m_kInfo += "100k(" + VeStringA::From(VeUInt32(kPlayer.GetPresentSend() / 1000)) + "k)\n";
	kEvent.m_kInfo += TT("EventInfoAward3");
	kEvent.m_eInfo = EVENT_PRESENT;
}

void UpdateEventTickOnline(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = kPlayer.GetVip() ? EventButton::TYPE_VIP : EventButton::TYPE_DAILY;
	kEvent.m_eState = kPlayer.GetEventTickOnline() ? EventButton::STATE_FINISHED : EventButton::STATE_COMPLETED;
	kEvent.m_eAction = EventList::ACT_NULL;
	kEvent.m_kInfo += TT("EventInfoTarget4");
	kEvent.m_kInfo += "\n";
	kEvent.m_kInfo += kPlayer.GetVip() ? TT("EventInfoAward4_1") : TT("EventInfoAward4_0");
	kEvent.m_eInfo = EVENT_ONLINE;
}

void UpdateEventTickPlayCount(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = kPlayer.GetVip() ? EventButton::TYPE_VIP : EventButton::TYPE_DAILY;
	if(kPlayer.GetEventTickPlayCount())
	{
		kEvent.m_eState = EventButton::STATE_FINISHED;
	}
	else
	{
		kEvent.m_eState = kPlayer.GetTickPlayCount() >= 10 ? EventButton::STATE_COMPLETED : EventButton::STATE_OPENING;
	}
	kEvent.m_eAction = EventList::ACT_PLAY_POKER;
	kEvent.m_kInfo += TT("EventInfoTarget5");
	kEvent.m_kInfo += "(" + VeStringA::From(kPlayer.GetTickPlayCount()) + ")\n";
	kEvent.m_kInfo += kPlayer.GetVip() ? TT("EventInfoAward5_1") : TT("EventInfoAward5_0");
	kEvent.m_eInfo = EVENT_PLAY_COUNT;
}

void UpdateEventTickSlotCount(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = kPlayer.GetVip() ? EventButton::TYPE_VIP : EventButton::TYPE_DAILY;
	if(kPlayer.GetEventTickPlaySlotCount())
	{
		kEvent.m_eState = EventButton::STATE_FINISHED;
	}
	else
	{
		kEvent.m_eState = kPlayer.GetTickPlaySlotCount() >= 30 ? EventButton::STATE_COMPLETED : EventButton::STATE_OPENING;
	}
	kEvent.m_eAction = EventList::ACT_PLAY_SLOT;
	kEvent.m_kInfo += TT("EventInfoTarget6");
	kEvent.m_kInfo += "(" + VeStringA::From(kPlayer.GetTickPlaySlotCount()) + ")\n";
	kEvent.m_kInfo += kPlayer.GetVip() ? TT("EventInfoAward6_1") : TT("EventInfoAward6_0");
	kEvent.m_eInfo = EVENT_PLAY_SLOT_COUNT;
}

void UpdateEventTickKenoCount(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = kPlayer.GetVip() ? EventButton::TYPE_VIP : EventButton::TYPE_DAILY;
	if(kPlayer.GetEventTickPlayKenoCount())
	{
		kEvent.m_eState = EventButton::STATE_FINISHED;
	}
	else
	{
		kEvent.m_eState = kPlayer.GetTickPlayKenoCount() >= 3 ? EventButton::STATE_COMPLETED : EventButton::STATE_OPENING;
	}
	kEvent.m_eAction = EventList::ACT_PLAY_KENO;
	kEvent.m_kInfo += TT("EventInfoTarget7");
	kEvent.m_kInfo += "(" + VeStringA::From(kPlayer.GetTickPlayKenoCount()) + ")\n";
	kEvent.m_kInfo += kPlayer.GetVip() ? TT("EventInfoAward7_1") : TT("EventInfoAward7_0");
	kEvent.m_eInfo = EVENT_PLAY_KENO_COUNT;
}

void UpdateEventTickBaccCount(TexasPokerHallPlayerC& kPlayer, VeVector<EventList::Event>& kList)
{
	kList.Resize(kList.Size() + 1);
	EventList::Event& kEvent = kList.Back();
	kEvent.m_eType = kPlayer.GetVip() ? EventButton::TYPE_VIP : EventButton::TYPE_DAILY;
	if(kPlayer.GetEventTickPlayBaccCount())
	{
		kEvent.m_eState = EventButton::STATE_FINISHED;
	}
	else
	{
		kEvent.m_eState = kPlayer.GetTickPlayBaccCount() >= 10 ? EventButton::STATE_COMPLETED : EventButton::STATE_OPENING;
	}
	kEvent.m_eAction = EventList::ACT_PLAY_BACC;
	kEvent.m_kInfo += TT("EventInfoTarget8");
	kEvent.m_kInfo += "(" + VeStringA::From(kPlayer.GetTickPlayBaccCount()) + ")\n";
	kEvent.m_kInfo += kPlayer.GetVip() ? TT("EventInfoAward8_1") : TT("EventInfoAward8_0");
	kEvent.m_eInfo = EVENT_PLAY_BACC_COUNT;
}

void EventList::UpdateEvents()
{
	if(!g_pkHallAgent) return;
	TexasPokerHallPlayerC* pkPlayer = g_pkHallAgent->GetPlayer();
	if(!pkPlayer) return;
	VeVector<Event> kEventListTemp;
	UpdateModifyEvent(*pkPlayer, kEventListTemp);
	UpdateEventPurchase(*pkPlayer, kEventListTemp);
	UpdateEventVip(*pkPlayer, kEventListTemp);
	UpdateEventPresent(*pkPlayer, kEventListTemp);
	UpdateEventTickOnline(*pkPlayer, kEventListTemp);
	UpdateEventTickPlayCount(*pkPlayer, kEventListTemp);
	UpdateEventTickSlotCount(*pkPlayer, kEventListTemp);
	UpdateEventTickKenoCount(*pkPlayer, kEventListTemp);
	UpdateEventTickBaccCount(*pkPlayer, kEventListTemp);
	VeUInt32 u32Number = SortEventList(m_kEventList, kEventListTemp);
	GetUILayer(Hall, "hall")->SetEventNumber(u32Number);
	ClearAll();
	for(VeUInt32 i(0); i < m_kEventList.Size(); ++i)
	{
		Event& kEvent = m_kEventList[i];
		AddEventButton(kEvent.m_eType, kEvent.m_eState, kEvent.m_kInfo);
	}
	ScrollToTop();
}

EventList::Item::Item()
{
	m_pkButton = NULL;
}
