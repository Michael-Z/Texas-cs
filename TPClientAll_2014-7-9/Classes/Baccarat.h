#pragma once

#include "UILayer.h"
#include "UIWidget.h"

enum BetLayerTouch
{
	BACC_EVENT_FLAG = 0x80000000,
	BACC_EVENT_BANKER = 0x00000000,
	BACC_EVENT_PLAYER = 0x00000100,
	BACC_EVENT_TIE = 0x00000200,
	BACC_EVENT_TYPE_MASK = 0x00000F00,
	BACC_EVENT_CHIPS_MASK = 0x000000FF
};

class Baccarat : public UILayerExt<Baccarat>
{
public:

	Baccarat();

	virtual ~Baccarat();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	void SetChips(VeUInt64 u64Win, VeUInt64 u64Credit);

	void SetWin(VeUInt64 u64Win);

	void SetCountDownTime(VeUInt32 u32Second);

	void SetBetChips(VeUInt32 u32Player, VeUInt32 u32Banker, VeUInt32 u32Tie);

	virtual void SetEnable(bool bEnable);

	void Update(VeFloat32 f32Delta = 0);

	void UpdateCountDownTime();

	void UpdateServer(VeFloat32 f32Delta = 0);

	void UpdateAllPlayers();

	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	void SendCard();

	void OnSendCardStopFirst(VeFloat32 f32Delta);

	void OnSendCardStopSecond(VeFloat32 f32Delta);

	void WinLosMusicEffect();

	void BankerWinMusicCallfunc();

	void PlayerWinMusicCallfunc();

	void DrawGameMusicCallfunc();

	void DoubleBetCallFunc();

	void AllBetFadeOut();

	void AfterAllBetFadeOut(VeFloat32 f32Delta);

	void ScheduleWinChipsUpdate(VeFloat32 f32Delta);

	void WinChipsUpdate(VeFloat32 f32Delta);

	VE_CREATE_FUNC(Baccarat);

	void UpdateBaccResult(bool bMotion);

	virtual void OnForceUpdate();

	void CalculateRes(BaccaratBetType eWinType, VeUInt32 u32Seed);

	virtual void OnPushNotice(const VeChar8* pcNotice);

protected:
	UIDeclareDelegate(OnPurchase);
	UIDeclareDelegate(RepeatBet);
	UIDeclareDelegate(DoubleBet);
	UIDeclareDelegate(OnBack);
	UIDeclareDelegate(OnAreaEvent);

	UIButton* m_pkRepeatBet;
	UIButton* m_pkDoubleBet;

	SlotNumber* m_kBetPlayerNum;
	SlotNumber* m_kAllBetChips;

	MiniCard* m_apkPlayerCard[3];
	MiniCard* m_apkBankerCard[3];

	cocos2d::CCSprite* m_pkDrawGameLeft;
	cocos2d::CCSprite* m_pkDrawGameRight;
	cocos2d::CCSprite* m_pkWin;
	cocos2d::CCSprite* m_pkLos;

	SlotNumber* m_pkPlayerScore;
	SlotNumber* m_pkBankerScore;

	cocos2d::CCLabelTTF* m_pkBetChipsPlayer;
	cocos2d::CCLabelTTF* m_pkBetChipsTie;
	cocos2d::CCLabelTTF* m_pkBetChipsBanker;

	cocos2d::CCLabelTTF* m_pkWinChips;
	cocos2d::CCLabelTTF* m_pkCredit;
	
	VeUInt32 m_u32CountDownTime;

	SlotNumber* m_kCountDownTimes;

	VeUInt32 m_u32TieNum;
	VeUInt32 m_u32PlayerNum;
	VeUInt32 m_u32BankerNum;

	VeUInt32 m_u32TieNumNow;
	VeUInt32 m_u32PlayerNumNow;
	VeUInt32 m_u32BankerNumNow;

	GameChips* m_pkTieAreaChipsIn;
	GameChips* m_pkPlayerAreaChipsIn;
	GameChips* m_pkBankerAreaChipsIn;

	BaccaratBetType m_eWinType;
	VeVector< VePair<CardNum, CardSuit> > m_kBankerCard;
	VeVector< VePair<CardNum, CardSuit> > m_kPlayerCard;
	VeUInt32 m_u32SendCard;
	VeUInt64 m_u64WinChips;
	VeUInt64 m_u64Credit;

	bool m_bPlayRes;

};

class BetArea;

class BetAreaChip : public UIChooseButton
{
public:
	enum Event
	{
		EVENT_TOUCH				= 0x8000,
		EVENT_RELEASE
	};

	BetAreaChip(const VeChar8* pcNormal, const VeChar8* pcPressed, const VeChar8* pcDisabled = NULL);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual bool IsIn(VeFloat32 x, VeFloat32 y);

	virtual void EventRelease();

	void SetTouch(bool bTouch);

protected:
	cocos2d::CCPoint m_kStart;
	cocos2d::CCPoint m_kBeganPos;
	bool m_bTouch;

};

class BetArea : public UIWidget
{
public:
	enum BaccArea
	{
		AREA_BANKER,
		AREA_PLAYER,
		AREA_TIE,
		AREA_MAX
	};

	enum BetNPD
	{
		BET_N,
		BET_P,
		BET_D
	};

	typedef VeMemberDelegate<BetArea, UIWidget&, VeUInt32> UIDelegate;

	BetArea();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	virtual bool IsIn(VeFloat32 x, VeFloat32 y);

	VeUInt32 AllBetFadeOut(BaccaratBetType eType);

	void SetChooseArea(BaccArea eArea);

	void UpdateChips(VeUInt32 u32BankerNum, VeUInt32 u32PlayerNum, VeUInt32 u32TieNum, bool bMotion = true);

	void UpdateCanBet(VeUInt32 u32CanBet);

	void SetPlayRes(bool bPlay);

protected:
	void SetTouchedChip(VeUInt32 u32Index);

	friend class Baccarat;
	UIDeclareDelegate(OnChipClicked0);
	UIDeclareDelegate(OnChipClicked1);
	UIDeclareDelegate(OnChipClicked2);
	UIDeclareDelegate(OnChipClicked3);
	UIDeclareDelegate(OnChipClicked4);
	UIDeclareDelegate(OnChipClicked5);
	
	BetAreaChip* m_apkAreaChips[6];
	cocos2d::CCSprite* m_pkTieBetAreaL_n;
	cocos2d::CCSprite* m_pkTieBetAreaR_n;
	cocos2d::CCSprite* m_pkTieBetAreaL_p;
	cocos2d::CCSprite* m_pkTieBetAreaR_p;
	cocos2d::CCSprite* m_pkPlayerBetArea_n;
	cocos2d::CCSprite* m_pkPlayerBetArea_p;
	cocos2d::CCSprite* m_pkBankerBetArea_n;
	cocos2d::CCSprite* m_pkBankerBetArea_p;
	cocos2d::CCRect m_akAreas[AREA_MAX];
	BetAreaChip* m_pkTouchedChip;
	BaccArea m_eAreaState;

	GameChips* m_pkTieAreaChipsIn;
	GameChips* m_pkPlayerAreaChipsIn;
	GameChips* m_pkBankerAreaChipsIn;

	GameChipsMove* m_apkGameChipsMove[3];

	VeUInt32 m_u32TieNum;
	VeUInt32 m_u32PlayerNum;
	VeUInt32 m_u32BankerNum;

};
