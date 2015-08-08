#pragma once

#include "UIScrollView.h"
#include "UIButton.h"

class RoomBar : public UIButton
{
public:
	enum Type
	{
		TYPE_BASE,
		TYPE_ADVANCE,
		TYPE_EXPERT,
		TYPE_MATCH,
		TYPE_KNOCKOUT,
		TYPE_MAX
	};

	RoomBar(VeUInt32 u32Index);

	virtual ~RoomBar();

	void Init(VeUInt32 u32MaxChips, VeUInt32 u32SB, VeUInt32 u32BB, Type eType, bool bVip, bool bHot);

	VeUInt32 GetIndex();

	void SetEnable(bool bEnable, bool bHot);

protected:
	const VeUInt32 m_u32Index;
	cocos2d::CCSprite* m_pkHot;
	cocos2d::CCSprite* m_pkType;
	cocos2d::CCLabelTTF* m_pkMaxNumber;
	cocos2d::CCLabelTTF* m_pkSB;
	cocos2d::CCLabelTTF* m_pkBB;
	cocos2d::CCSprite* m_pkLock;

};

class RoomChoose : public UIScrollView
{
public:
	typedef VeMemberDelegate<RoomChoose, UIWidget&, VeUInt32> UIDelegate;

	RoomChoose();

	virtual ~RoomChoose();

	VeUInt32 AddRoomBar(VeUInt32 u32MaxChips, VeUInt32 u32SB, VeUInt32 u32BB, RoomBar::Type eType, bool bVip, bool bHot);

	void SetEnable(VeUInt32 u32Index, bool bEnable, bool bHot);

	void ClearAll();

protected:
	void OnClicked(UIWidget& kWidget, VeUInt32 u32State);

	UIDeclareDelegate(OnFastEnter);

	struct Item
	{
		Item();
		RoomBar* m_pkBar;
		UIDelegate m_kDelegate;
	};

	VeVector<Item> m_kRoomBarArray;
	VeUInt32 m_u32Pointer;
	UIButton* m_pkQuickStart;
	UIButton* m_pkVenusCup;

};
