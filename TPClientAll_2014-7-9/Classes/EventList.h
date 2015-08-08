#pragma once

#include "UIScrollView.h"
#include "UIButton.h"
#include "TexasPokerHallClient.h"

class UIPanel;

class EventButton : public UIButton
{
public:
	enum Type
	{
		TYPE_DAILY,
		TYPE_SPECIAL,
		TYPE_VIP,
		TYPE_MAX
	};

	enum State
	{
		STATE_CLOSED,
		STATE_OPENING,
		STATE_COMPLETED,
		STATE_FINISHED,
		STATE_MAX
	};
	typedef VeMemberDelegate<EventButton, UIWidget&, VeUInt32> UIDelegate;

	EventButton(VeUInt32 u32Index, UIPanel* pkNormal, UIPanel* pkPressed);

	virtual ~EventButton();

	void Init(Type eType, State eState, const VeChar8* pcInfo);

	VeUInt32 GetIndex();	

protected:
	virtual void OnEvent(UIWidget& kWidget, VeUInt32 u32State);

	const VeUInt32 m_u32Index;
	cocos2d::CCNode* m_pkRoot;
	cocos2d::CCSprite* m_pkIcon;
	cocos2d::CCSprite* m_pkTitle;
	cocos2d::CCSprite* m_pkState;
	cocos2d::CCLabelTTF* m_pkInfo;

};

class EventList : public UIScrollView
{
public:
	typedef VeMemberDelegate<EventList, UIWidget&, VeUInt32> UIDelegate;

	enum EventAction
	{
		ACT_NULL,
		ACT_EDIT_INFO,
		ACT_OPEN_SHOP,
		ACT_PLAY_POKER,
		ACT_PLAY_SLOT,
		ACT_PLAY_KENO,
		ACT_PLAY_BACC,
		ACT_MAX
	};

	struct Event
	{
		EventButton::Type m_eType;
		EventButton::State m_eState;
		EventAction m_eAction;
		VeString m_kInfo;
		EventInfo m_eInfo;
	};

	EventList();

	virtual ~EventList();

	VeUInt32 AddEventButton(EventButton::Type eType, EventButton::State eState, const VeChar8* pcInfo);

	void ClearAll();

	void UpdateEvents();

protected:
	void OnClicked(UIWidget& kWidget, VeUInt32 u32State);

	struct Item
	{
		Item();
		EventButton* m_pkButton;
		UIDelegate m_kDelegate;
	};

	VeVector<Item> m_kEventButtonArray;
	VeUInt32 m_u32Pointer;
	VeVector<Event> m_kEventList;

};
