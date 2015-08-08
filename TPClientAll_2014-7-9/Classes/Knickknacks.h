#pragma once

#include "UIWidget.h"
#include "UISlider.h"
#include "UIAlert.h"
#include "UIInputBox.h"
#include "UIButton.h"
#include "UITableView.h"
#include "UIScrollView.h"
#include "Card.h"

#define PLAYER_TURN_TIME 15.0f

class AllOnline : public UIWidget
{
public:
	AllOnline();

	virtual ~AllOnline();

	void SetNumber(VeUInt64 u64Num);

protected:
	void UpdateNumber();

	cocos2d::CCLabelTTF* m_pkAllOnlineNum;
	VeUInt64 m_u64Number;

};

class NameLabel : public UIWidget
{
public:
	NameLabel();

	virtual ~NameLabel();

	void SetName(const VeChar8* pcName);

protected:
	cocos2d::CCLabelTTF* m_pkName;

};

class HeadArea : public UIWidget
{
public:
	HeadArea();

	virtual ~HeadArea();

	void Set(VeUInt32 u32Index, bool bMale, bool bVip);

protected:
	cocos2d::CCSprite* m_pkIcon;
	cocos2d::CCSprite* m_pkSex;
	cocos2d::CCSprite* m_pkVip;

};

class ChipsBar : public UIWidget
{
public:
	ChipsBar();

	virtual ~ChipsBar();

	void SetNumber(VeUInt32 u32Num);

	VeUInt32 GetNumber();

protected:
	void UpdateNumber();

	cocos2d::CCLabelTTF* m_pkChips;
	VeUInt32 m_u32Number;
};

class GoldBar : public UIWidget
{
public:
	GoldBar();

	virtual ~GoldBar();

	void SetNumber(VeUInt32 u32Num);

protected:
	void UpdateNumber();

	cocos2d::CCLabelTTF* m_pkGold;
	VeUInt32 m_u32Number;
};

class NoticeBar : public UIWidget
{
public:
	NoticeBar();

	virtual ~NoticeBar();

	void Notice(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

protected:
	void Update(VeFloat32 f32Delta);

	void _Notice(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

	class Content : public VeRefNode<Content*>
	{
	public:
		Content(const VeChar8* pcContent, NoticeBar* pkParent, VeFloat32 f32Speed, VeFloat32 f32Space);

		bool IsShown();

		void Restart(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

		bool Update(VeFloat32 f32Delta);

	protected:
		void UpdatePos();

		cocos2d::CCLabelTTF* m_pkLabel;
		VeFloat32 m_f32Speed;
		VeFloat32 m_f32Space;
		VeFloat32 m_f32PosY;

	};

	VeRefList<Content*> m_kUsedList;
	VeRefList<Content*> m_kFreeList;
	VeList< VePair< VeStringA, VePair<VeFloat32,VeFloat32> > > m_kCachedTexts;

};

class HeadChatBar : public UIWidget
{
public:
	HeadChatBar();

	virtual ~HeadChatBar();

	void Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

protected:
	void Update(VeFloat32 f32Delta);

	void _Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

	class ChatContent : public VeRefNode<ChatContent*>
	{
	public:
		ChatContent(const VeChar8* pcContent, HeadChatBar* pkParent, VeFloat32 f32Speed, VeFloat32 f32Space);

		bool IsShown();

		void Restart(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

		bool Update(VeFloat32 f32Delta);

	protected:
		void ResolveContent(const VeChar8* pcContent);
		void SetIcon(VeUInt32 u32Index);
		void SetText(const VeChar8* pcContent);
		void UpdatePos();
		UIPanel* m_pkPanel;
		cocos2d::CCLabelTTF* m_pkLabel;
		cocos2d::CCSprite* m_pkIcon;
		VeFloat32 m_f32Speed;
		VeFloat32 m_f32Space;
		VeFloat32 m_f32Time;
		VeFloat32 m_f32TimeDuration;

	};

	VeRefList<ChatContent*> m_kUsedList;
	VeRefList<ChatContent*> m_kFreeList;
	VeList< VePair< VeStringA, VePair<VeFloat32,VeFloat32> > > m_kCachedTexts;

};

class Slider : public UISlider
{
public:
	Slider();
};

class FillSlider : public UISliderV
{
public:
	FillSlider();
};

class ChipsCarrySlider : public UISlider
{
public:
	ChipsCarrySlider();

	void SetRange(VeUInt32 u32Min, VeUInt32 u32Max, VeUInt32 u32Unit);

	VeUInt32 GetCurrent();

protected:
	void UpdateData();

	virtual void Update();

	UIBarH* m_pkBubble;
	cocos2d::CCLabelTTF* m_pkCurrent;

	VeUInt32 m_u32Min;
	VeUInt32 m_u32Max;
	VeUInt32 m_u32Unit;
	VeUInt32 m_u32Current;

};

class Perspective : public cocos2d::CCSprite
{
public:
	Perspective();

	virtual ~Perspective();

	static Perspective* create(const char *pszFileName);

	virtual bool initWithTexture(cocos2d::CCTexture2D *pTexture, const cocos2d::CCRect& rect, bool rotated);

	virtual void draw();

	void SetRotate(VeQuaternion& kQuat);

	const VeQuaternion& GetRotate();

	void SetRotateCenter(VeFloat32 x, VeFloat32 y);

	void SetPerspectiveRate(VeFloat32 f32Rate);

protected:
	VeVector4D m_kTrans;
	VeQuaternion m_kRotate;

};

class PlayerCard : public UIWidget
{
public:
	enum Process
	{
		PROCESS_NULL,
		PROCESS_FOLD,
		PROCESS_RAISE,
		PROCESS_CALL,
		PROCESS_CHECK,
		PROCESS_ALL_IN,
		PROCESS_BIG_BLIND,
		PROCESS_SMALL_BLIND,
		PROCESS_CATE_ROYAL_FLUSH,
		PROCESS_CATE_STRAIGHT_FLUSH,
		PROCESS_CATE_FOUR_KIND,
		PROCESS_CATE_FULL_HOUSE,
		PROCESS_CATE_FLUSH,
		PROCESS_CATE_STRAIGHT,
		PROCESS_CATE_THREE_KIND,
		PROCESS_CATE_TWO_PAIR,
		PROCESS_CATE_ONE_PAIR,
		PROCESS_CATE_HIGH_CARD,
		PROCESS_MAX
	};

	PlayerCard();

	virtual ~PlayerCard();

	void SetWin(bool bWin);

	void SetRate(VeFloat32 f32Rate);

	void SetHeadIcon(VeUInt32 u32Index, bool bMale, bool bVip);

	void SetName(const VeChar8* pcName);

	void SetTitle(const VeChar8* pcTitle);

	void SetGameData(VeUInt32 u32MaxWinBet, VeFloat32 f32WinRate);

	void SetStrength(VeFloat32 f32Rate, bool bShow);

	void SetSeatIndex(VeUInt32 u32Index);

	VeUInt32 GetSeatIndex();

	void UpdateTurnTime(VeFloat32 f32Delta);

	void StopUpdateRate();

	void SetDealer(bool bDealer);

	bool GetDealer();

	void SetProcess(Process eProcess);

	Process GetProcess();

	const cocos2d::CCPoint GetPosition();

protected:
	UIWidget* m_pkRoot;
	cocos2d::CCSprite* m_pkBoard;
	cocos2d::CCProgressTimer* m_pkCdBar;
	cocos2d::CCSprite* m_pkWinFrame;
	cocos2d::CCSprite* m_pkHead;
	cocos2d::CCSprite* m_pkSex;
	cocos2d::CCSprite* m_pkVip;
	cocos2d::CCSprite* m_apkBoardHalo[2];
	cocos2d::CCSprite* m_pkDealer;
	cocos2d::CCLabelTTF* m_pkProcess;
	cocos2d::CCLabelTTF* m_pkName;
	cocos2d::CCLabelTTF* m_pkTitle;
	cocos2d::CCLabelTTF* m_pkWinBet;
	cocos2d::CCLabelTTF* m_pkWinRate;
	cocos2d::CCLabelTTF* m_pkHandStrength;
	VeUInt32 m_u32SeatIndex;
	VeFloat32 m_f32Delay;
	Process m_eProcess;

};

class NameCard : public UIWidget
{
public:
	typedef VeMemberDelegate<NameCard, UIWidget&, VeUInt32> UIDelegate;

	enum Overlay
	{
		OVERLAY_NULL,
		OVERLAY_HOLD,
		OVERLAY_READY,
		OVERLAY_FOLD,
		OVERLAY_MAX
	};

	NameCard(VeUInt32 u32Index);

	virtual ~NameCard();

	void SetWin(VeUInt32 u32WinChips);

	void SetRate(VeFloat32 f32Rate);

	void ShowSeat();

	void ShowBoard();

	void SetDrink(VeInt32 i32Index);

	void SetHeadIcon(VeUInt32 u32Index, bool bMale, bool bVip);

	void SetRight();

	void SetName(const VeChar8* pcName);

	MiniCard* GetLeft();

	MiniCard* GetRight();

	void SetMiniCardVisible(bool mbVisible);

	void SetSeatIndex(VeUInt32 u32Index);

	VeUInt32 GetSeatIndex();

	void SetChips(VeUInt32 u32Chips);

	void UpdateTurnTime(VeFloat32 f32Delta);

	void StopUpdateRate();

	void SetDealer(bool bDealer);

	bool GetDealer();

	void SetProcess(PlayerCard::Process eProcess);

	PlayerCard::Process GetProcess();

	void SetOverlay(Overlay eOverlay);

protected:
	friend class Game;
	UIDeclareDelegate(OnPresent);
	UIDeclareDelegate(OnPresentDrink);
	UIDeclareDelegate(OnSeat);

	UIUserDataPtr m_spCanteenData;
	UIButton* m_pkSeat;
	UIWidget* m_pkRoot;
	cocos2d::CCSprite* m_pkBoard;
	cocos2d::CCProgressTimer* m_pkCdBar;
	cocos2d::CCSprite* m_pkWinFrame;
	cocos2d::CCSprite* m_pkOverlay;
	cocos2d::CCSprite* m_pkOverlayText;
	cocos2d::CCSprite* m_pkHead;
	cocos2d::CCSprite* m_pkSex;
	cocos2d::CCSprite* m_pkVip;
	cocos2d::CCSprite* m_pkDealer;
	UIButton* m_pkGiveDrink;
	UIButton* m_pkDrink;
	//cocos2d::CCSprite* m_pkDrink;
	cocos2d::CCLabelTTF* m_pkProcess;
	cocos2d::CCLabelTTF* m_pkName;
	cocos2d::CCLabelTTF* m_pkChips;
	cocos2d::CCLabelTTF* m_pkWinChips;
	MiniCard* m_apkCard[2];
	VeUInt32 m_u32SeatIndex;
	VeFloat32 m_f32Delay;
	PlayerCard::Process m_eProcess;
	
};

class Number : public UIWidget
{
public:
	Number(const VeChar8* pcType, VeFloat32 f32Space);

	Number(const VeChar8* pcImage, const VeChar8* pcType, VeFloat32 f32Space);

	virtual ~Number();

	void SetNumber(VeUInt32 u32Number);

	VeUInt32 GetNumber();

protected:
	void Update();

	void Clear();

	VeStringA m_kType;
	VeFloat32 m_f32Space;
	VeUInt32 m_u32Number;
	cocos2d::CCSpriteBatchNode* m_pkRoot;

};

class EditInfo : public UILayerModalExt<EditInfo>
{
public:
	EditInfo();

	virtual ~EditInfo();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(EditInfo);

protected:
	void UpdateHead();

	bool IsChange();

	UIDeclareDelegate(OnConfirm);
	UIDeclareDelegate(OnCancel);
	UIDeclareDelegate(OnSex);
	UIDeclareDelegate(OnChoose);

	HeadArea* m_pkHeadArea;
	UIInputBox* m_pkInputName;
	UIInputBox* m_pkInputTell;
	UIChooseButtonArea* m_pkSexArea;
	UIChooseButtonArea* m_pkChooseArea;
	bool m_bEvent;

};

class ChatInfoBar : public UIWidget
{
public:
	ChatInfoBar(VeUInt32 index);

	virtual ~ChatInfoBar();

	void Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space, bool bMove = false);

	VeUInt32 GetIndex() const { return m_u32Index; }

protected:
	void Update(VeFloat32 f32Delta);

	void _Chat(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space, bool bMove = false);

	class ChatContent : public VeRefNode<ChatContent*>
	{
	public:
		ChatContent(const VeChar8* pcContent, ChatInfoBar* pkParent, VeFloat32 f32Speed, VeFloat32 f32Space, bool bMove);

		bool IsShown();

		void Restart(const VeChar8* pcContent, VeFloat32 f32Speed, VeFloat32 f32Space);

		bool Update(VeFloat32 f32Delta);

	protected:
		void ResolveContent(const VeChar8* pcContent);
		void SetIcon(VeUInt32 u32Index);
		void SetText(const VeChar8* pcContent);
		void UpdatePos();
		//UIPanel* m_pkPanel;
		bool m_bMove;
		cocos2d::CCLabelTTF* m_pkLabel;
		cocos2d::CCSprite* m_pkIcon;
		VeFloat32 m_f32Speed;
		VeFloat32 m_f32Space;
		VeFloat32 m_f32PosY;
		VeFloat32 m_f32Time;
		VeFloat32 m_f32TimeDuration;		

	};

	VeRefList<ChatContent*> m_kUsedList;
	VeRefList<ChatContent*> m_kFreeList;
	VeList< VePair< VeStringA, VePair<VeFloat32,VeFloat32> > > m_kCachedTexts;
	VeUInt32 m_u32Index;
};

class EventButton;
class ChatInfoChoose : public UIScrollView
{
public:
	typedef VeMemberDelegate<ChatInfoChoose, UIWidget&, VeUInt32> UIDelegate;

	ChatInfoChoose();

	virtual ~ChatInfoChoose();

	VeUInt32 AddChatInfoBar(const VeChar8* szChat, bool bMove = false);

	void SetEnable(VeUInt32 u32Index, bool bEnable, bool bHot);

	void ClearAll();

protected:
	void OnClicked(UIWidget& kWidget, VeUInt32 u32State);

	struct Item
	{
		Item();
		EventButton* m_pkButton;
		ChatInfoBar* m_pkBar;
		UIDelegate m_kDelegate;
	};

	VeVector<Item> m_kChatInfoBarArray;
	VeUInt32 m_u32Pointer;
	UIButton* m_pkQuickStart;
	UIButton* m_pkVenusCup;

};

class EventList;
class ChatWindow : public UILayerModalExt<ChatWindow>
{
public:
	ChatWindow();

	virtual ~ChatWindow();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(ChatWindow);

protected:
	void UpdateHead();

	bool IsChange();

	void ShowHeadIcon(bool bShow);
	void ShowChatInfo(bool bShow);

	UIDeclareDelegate(OnClose);
	UIDeclareDelegate(OnSend);
	UIDeclareDelegate(OnSex);
	UIDeclareDelegate(OnChoose);
	UIDeclareDelegate(OnChooseHead);
	UIDeclareDelegate(OnChooseChat);

	HeadArea* m_pkHeadArea;
	UIInputBox* m_pkInputName;
	UIInputBox* m_pkInputTell;
	UIChooseButtonArea* m_pkSexArea;
	UIChooseButtonArea* m_pkChooseArea;
	ChatInfoChoose* m_pkChatArea;
	EventList* m_pkChatList;
	bool m_bEvent;

	cocos2d::CCSprite* m_apkBoard[4][4];
	cocos2d::CCSprite* m_apkHead[4][4];

};

class BonusCard : public UIWidget
{
public:
	enum Event
	{
		CARD_EVENT_SLOT,
		CARD_EVENT_MAX
	};

	BonusCard();

	virtual ~BonusCard();

	void SetLevel(VeUInt32 u32Level);

	VeUInt32 GetLevel();

	void SetHighlight(bool bEnable);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	void Update(VeFloat32 f32Delta);

	void Move(VeFloat32 f32Pos);

protected:
	cocos2d::CCSprite* m_pkCard;
	Number* m_pkNumber;
	VeFloat32 m_f32Left;
	VeFloat32 m_f32Right;
	VeFloat32 m_f32Middle;
	VeFloat32 m_f32Velocity;
	VeFloat32 m_f32RollBackTime;
	VeFloat32 m_f32RollBackPos;
	bool m_bTouch;
	bool m_bRollBack;

};

class Bank : public UILayerModalExt<Bank>
{
public:
	Bank();

	virtual ~Bank();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(Bank);

	void Lock();

	void Unlock();

	void UpdateObject();

	void UpdateChange(const VeVector<VeUInt32>& tObjectPriceList);

	void UpdateBonusCard();

	void OnBonus(VeUInt32 u32Bonus);

protected:
	void SetObject(VeUInt32 u32Diamond, VeUInt32 u32Antique, VeUInt32 u32Car, VeUInt32 u32Plane);

	void SetChange(VeUInt32 u32Diamond, VeUInt32 u32Antique, VeUInt32 u32Car, VeUInt32 u32Plane);

	void DisableExchange();

	void CreateMenu(UIWidget* pkParent);

	void CreateBonus(UIWidget* pkParent);

	void CreateExchange(UIWidget* pkParent);

	void TickBonusCard(VeFloat32 f32Delta);

	UIDeclareDelegate(OnCancel);
	UIDeclareDelegate(OnBonusCard);
	UIDeclareDelegate(OnExchangeCard);
	UIDeclareDelegate(OnExchangeDiamodClicked);
	UIDeclareDelegate(OnExchangeAntiqueClicked);
	UIDeclareDelegate(OnExchangeCarClicked);
	UIDeclareDelegate(OnExchangePlaneClicked);
	UIDeclareDelegate(OnCardSlot);
	UIPageView* m_pkPage;
	Number* m_pkCardLevel;
	BonusCard* m_pkMovdCard;
	UIWidget* m_pkNumber;
	cocos2d::CCSprite* m_apkState[3];
	cocos2d::CCSpriteBatchNode* m_pkNodeL;
	cocos2d::CCSpriteBatchNode* m_pkNodeR;
	cocos2d::CCLabelTTF* m_apkObjectNum[4];
	cocos2d::CCLabelTTF* m_apkChangeNum[4];
	UIButton* m_apkExchange[4];
	bool m_bLock;
	bool m_bCardLock;

};

class GameTableBoard : public UIWidget
{
public:
	GameTableBoard();

	void Set(VeUInt32 u32SmallBill, VeUInt32 u32BigBill);

protected:
	cocos2d::CCLabelTTF* m_pkSmallBill;
	cocos2d::CCLabelTTF* m_pkBigBill;

};

class ChipsCarry : public UILayerModalExt<ChipsCarry>
{
public:
	ChipsCarry();

	virtual ~ChipsCarry();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	bool IsShow();

	VE_CREATE_FUNC(ChipsCarry);

	void SetRange(VeUInt32 u32Min, VeUInt32 u32Max, VeUInt32 u32Unit);

protected:
	UIDeclareDelegate(OnConfirm);
	UIDeclareDelegate(OnCancel);
	UIDeclareDelegate(OnChipsChange);	

	ChipsBar* m_pkChipsBar;
	ChipsCarrySlider* m_pkSlider;
	cocos2d::CCLabelTTF* m_pkMinNum;
	cocos2d::CCLabelTTF* m_pkMaxNum;
	bool m_bIsShow;

};

class GiftChoose;

class GiftItem : public UIChooseButton
{
public:
	enum Type
	{
		TYPE_DRINK,
		TYPE_OBJECT,
		TYPE_MAX
	};

	GiftItem(VeUInt32 u32Index, Type eType, VeUInt32 u32Kind, bool bHot);

	virtual ~GiftItem();

	VeUInt32 GetIndex();

	Type GetType();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	void SetPrice(VeUInt32 u32Price);

protected:
	const VeUInt32 m_u32Index;
	const Type m_eType;
	cocos2d::CCLabelTTF* m_pkPrice;
	bool m_bInTouch;

};

class GiftChoose : public UIScrollView
{
public:
	typedef VeMemberDelegate<GiftChoose, UIWidget&, VeUInt32> UIDelegate;

	GiftChoose();

	virtual ~GiftChoose();

	VeUInt32 AddGiftItem(GiftItem::Type eType, VeUInt32 u32Kind, bool bHot);

	void ClearAll();

	void Reset();

	GiftItem* GetCurrent();

	void UpdateData();

	VeUInt32 GetChoosePrice();

protected:
	void OnClicked(UIWidget& kWidget, VeUInt32 u32State);

	struct Item
	{
		Item();
		GiftItem* m_pkGift;
		VeUInt32 m_u32Price;
		UIDelegate m_kDelegate;
	};

	VeVector<Item> m_kGiftArray;
	VeUInt32 m_u32Current;

};

class Canteen : public UILayerModalExt<Canteen>
{
public:
	Canteen();

	virtual ~Canteen();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	void UpdateData();

	VE_CREATE_FUNC(Canteen);

protected:
	void SetText(const VeChar8* pcBuy, const VeChar8* pcBuyAll);

	void UpdateButton();

	UIDeclareDelegate(OnCancel);
	UIDeclareDelegate(OnBuy);
	UIDeclareDelegate(OnBuyAll);
	UIDeclareDelegate(OnChoose);

	UIUserDataPtr m_spData;
	UIPanel* m_pkPanel;
	UIPanel* m_pkPanelInner;
	UIBarH* m_pkTitle;
	cocos2d::CCLabelTTF* m_pkTitleText;
	cocos2d::CCLabelTTF* m_pkBuyText;
	cocos2d::CCLabelTTF* m_pkBuyAllText;
	GiftChoose* m_pkGiftChoose;
	ChipsBar* m_pkChipsBar;
	VeUInt32 m_u32Index;

};

class GameMenu : public UILayerModalExt<GameMenu>
{
public:
	GameMenu();

	virtual ~GameMenu();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(GameMenu);

	void Close();

protected:
	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	void FadeIn(VeFloat32 f32Delta);

	void FadeOut(VeFloat32 f32Delta);

	void Update();

	UIDeclareDelegate(OnBackToHall);
	UIDeclareDelegate(OnChangeRoom);
	UIDeclareDelegate(OnReSit);
	UIDeclareDelegate(OnClose);

	UIPage* m_pkRoot;
	VeFloat32 m_f32Rate;

};

class GameHelp : public UILayerModalExt<GameHelp>
{
public:
	GameHelp();

	virtual ~GameHelp();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(GameHelp);

	void Close();

protected:
	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	void FadeIn(VeFloat32 f32Delta);

	void FadeOut(VeFloat32 f32Delta);

	void Update();

	UIWidget* m_pkRoot;
	UIPanel* m_pkPanel;
	VeFloat32 m_f32Rate;

};

class GameChips : public UIWidget
{
public:
	GameChips();

	void SetNumber(VeUInt32 u32Number);

	void SetFadePos(cocos2d::CCPoint kFadeIn, cocos2d::CCPoint kFadeOut);

	cocos2d::CCPoint FadeInPos();

	cocos2d::CCPoint FadeOutPos();

	void FadeIn(VeFloat32 f32Delay = 0);

	void FadeOut(VeFloat32 f32Delay = 0);

	VeUInt32 GetNumber();

	void ClearNumber();

	void ChangeNumberDelay(VeUInt32 u32Number, VeFloat32 f32Delay = 0);

	void Reset();

protected:
	void _FadeIn(VeFloat32 f32Delta);

	void _FadeOut(VeFloat32 f32Delta);

	void _FadeOutHide(VeFloat32 f32Delta);

	void _ChangeNumber(VeFloat32 f32Delta);

	VeUInt32 m_u32Number;
	cocos2d::CCLabelTTF* m_pkNumber;
	cocos2d::CCSpriteBatchNode* m_pkRoot;
	VeVector<cocos2d::CCSprite*> m_kChips;
	cocos2d::CCPoint m_kFadeInPos;
	cocos2d::CCPoint m_kFadeOutPos;
	VeUInt32 m_u32NumberWaitChange;
	cocos2d::CCSprite* m_pkChipsAlert;
};

class GameChipsMove : public UIWidget
{
public:
	GameChipsMove();

	void SetFadePos(cocos2d::CCPoint kFadeIn, cocos2d::CCPoint kFadeOut);

	void Move(VeUInt32 u32Number, VeFloat32 f32Delay = 0);

protected:
	void _Move(VeFloat32 f32Delta);

	void _MoveHide(VeFloat32 f32Delta);

	void Clear();

	cocos2d::CCSpriteBatchNode* m_pkRoot;
	VeVector<cocos2d::CCSprite*> m_kChips;
	cocos2d::CCPoint m_kFadeInPos;
	cocos2d::CCPoint m_kFadeOutPos;

};

class GameDrinksMove : public UIWidget
{
public:

	GameDrinksMove();

	void SetFadeInPos(cocos2d::CCPoint kFadeIn);

	void SetFadeOutPos(cocos2d::CCPoint kFadeOut);

	const cocos2d::CCPoint& GetFadeInPos();

	const cocos2d::CCPoint& GetFadeOutPos();

	void Move(GiftItem::Type m_type, VeUInt32 u32Number, VeFloat32 f32Delay = 0);

protected:
	void _Move(VeFloat32 f32Delta);

	void _MoveHide(VeFloat32 f32Delta);

	cocos2d::CCSprite* m_kDrinkAndObject;

	cocos2d::CCPoint m_kFadeInPos;
	cocos2d::CCPoint m_kFadeOutPos;

};

class ShopNumber : public UIWidget
{
public:
	ShopNumber();

	virtual ~ShopNumber();

	void SetNumber(VeUInt32 u32Number, bool bFrac);

	VeUInt32 GetNumber();

protected:
	void Update();

	void Clear();

	VeFloat32 m_f32Space;
	VeUInt32 m_u32Number;
	bool m_bFrac;
	cocos2d::CCSpriteBatchNode* m_pkRoot;

};

class PlayerWin : public UIWidget
{
public:
	PlayerWin();

	virtual ~PlayerWin();

	void Show(VeUInt32 u32WinChips, bool bMotion);

protected:
	void Reset();

	cocos2d::CCSprite* m_pkYouWin;
	cocos2d::CCSprite* m_pkBarLeft;
	cocos2d::CCSprite* m_pkBarRight;
	cocos2d::CCSprite* m_pkChipsIcon;
	cocos2d::CCSpriteBatchNode* m_pkRoot;
	VeVector<cocos2d::CCSprite*> m_kNumber;

};

class PurchaseItem : public UIButton
{
public:
	enum Type
	{
		TYPE_DOLLAR,
		TYPE_RMB,
		TYPE_MAX
	};

	PurchaseItem(cocos2d::CCNode* pkNormal, cocos2d::CCNode* pkPressed, VeUInt32 u32Index);

	virtual ~PurchaseItem();

	void Init(const VeChar8* pcInfo, VeUInt32 u32Price, Type eType, VeUInt32 u32Chips, VeUInt32 u32Icon, bool bVip);

	VeUInt32 GetIndex();

protected:
	const VeUInt32 m_u32Index;
	cocos2d::CCSprite* m_pkCurrency;
	cocos2d::CCSprite* m_pkIcon;
	cocos2d::CCSprite* m_pkVip;
	cocos2d::CCLabelTTF* m_pkInfo;
	ShopNumber* m_pkPrice;
	ShopNumber* m_pkChips;

};

class PurchaseChoose : public UIScrollView
{
public:
	typedef VeMemberDelegate<PurchaseChoose, UIWidget&, VeUInt32> UIDelegate;

	PurchaseChoose();

	virtual ~PurchaseChoose();

	VeUInt32 AddItem(const VeChar8* pcInfo, VeUInt32 u32Price, PurchaseItem::Type eType, VeUInt32 u32Chips, VeUInt32 u32Icon, bool bVip);

	void ClearAll();

	void Reset();

protected:
	void OnClicked(UIWidget& kWidget, VeUInt32 u32State);

	struct Item
	{
		Item();
		PurchaseItem* m_pkItem;
		UIDelegate m_kDelegate;
	};

	VeVector<Item> m_kItemArray;
	VeUInt32 m_u32Pointer;

};

class Shop : public UILayerModalExt<Shop>
{
public:
	Shop();

	virtual ~Shop();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	PurchaseChoose& GetChoose();

	VE_CREATE_FUNC(Shop);

protected:
	UIDeclareDelegate(OnCancel);
	UIDeclareDelegate(OnPurchase);
	UIDeclareDelegate(OnFreeChips);

};

class TexasHelp : public UILayerModalExt<TexasHelp>
{
public:
	TexasHelp();

	virtual ~TexasHelp();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(TexasHelp);

	virtual void visit(void);


protected:
	UIDeclareDelegate(OnCancel);

	UIScrollView* pkHelpScrollView;
};

class LottoHelp : public UILayerModalExt<LottoHelp>
{
public:
	LottoHelp();

	virtual ~LottoHelp();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(LottoHelp);


protected:
	UIDeclareDelegate(OnCancel);

	UIScrollView* pkHelpScrollView;

};

class BaccHelp : public UILayerModalExt<BaccHelp>
{
public:
	BaccHelp();

	virtual ~BaccHelp();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(BaccHelp);


protected:
	UIDeclareDelegate(OnCancel);

	UIScrollView* pkHelpScrollView;

};

class RankList : public UIWidget
{
public:
	RankList();

	virtual void visit();

	void UpdatePos(VeFloat32 f32Delta);

	void GetRankData(const VeVector< VePair<VeStringA,VeStringA> >& kLeaderboard);

	void UpdateRankInfo(VeUInt32 u32TextIndex ,VeUInt32 u32LeaderboardIndex);

protected:
	void UpdateMove();

	void UpdateInfo();

	cocos2d::CCLabelTTF* m_apkRankName[3];
	cocos2d::CCLabelTTF* m_apkChipList[3];
	VeVector< VePair<VeStringA,VeStringA> > m_kLeaderboard;
	VeFloat32 m_f32Move;
	VeUInt32 m_u32Pointer;
};
