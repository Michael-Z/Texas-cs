#pragma once

#include "UILayer.h"
#include "UIWidget.h"

#define SLOT_BET_MAX 10
#define SLOT_DELAY_TIME (2.0f)
#define SLOT_STOP_DELAY_TIME (0.3f)

#define SLOT_ROLLBACK_TIME (0.3f)

class SlotNumber : public UIWidget
{
public:
	SlotNumber(VeFloat32 f32Space);

	virtual ~SlotNumber();

	void SetNumber(VeUInt32 u32Number);

	VeUInt32 GetNumber();

protected:
	void Update();

	void Clear();

	VeFloat32 m_f32Space;
	VeUInt32 m_u32Number;
	cocos2d::CCSpriteBatchNode* m_pkRoot;

};

enum PrizeType
{
	MAX_PRIZE,
	MID_PRIZE,
	MIN_PRIZE
};

class JpBar : public UIWidget
{
public:
	JpBar();

	void GetWell();

	void Update(VeFloat32 f32Delta);

	void SetRate(VeFloat32 f32Rate);

	void SetNumber(VeUInt32 u32Number);

	void BigPrizeEffect(VeUInt32 m_prizeType);

protected:
	void Change();

	void _BigPrizeEffect(VeFloat32 f32Delta);

	cocos2d::CCSprite* m_pkBaceMapL;
	cocos2d::CCSprite* m_pkBaceMapR;
	cocos2d::CCSprite* m_apkBar[2];
	VeFloat32 m_f32TimePass;
	VeFloat32 m_f32Rate;
	SlotNumber* m_pkNumber;

};

class SlotRoll : public UIWidget
{
public:
	enum Event
	{
		SLOT_ROLL_STOP = 1001
	};

	SlotRoll();

	void Roll();

	void Stop(VeUInt32 u32Left, VeUInt32 u32Middle, VeUInt32 u32Right);

	void Update(VeFloat32 f32Delta);

	void UpdateStop(VeFloat32 f32Delta);

protected:
	void UpdatePos();

	void UpdateLight(VeFloat32 f32Delta);

	virtual void visit();

	cocos2d::CCSprite* m_apkRollItem[3][4];
	cocos2d::CCNode* m_apkRollNode[3];
	cocos2d::CCSprite* m_apkLight[3][2];
	VeUInt32 m_au32Pos[3];
	VeFloat32 m_af32Offset[3];
	VeFloat32 m_af32Speed[3];
	VeFloat32 m_af32SpeedMax[3];
	VeFloat32 m_f32PassTime;
	VeFloat32 m_f32LightTime;

	VeUInt32 m_au32Dest[3];
	VeFloat32 m_f32StopTime;
	VeUInt32 m_u32Stop;
	VeFloat32 m_f32TimeStop;
	bool m_bReduceSpeed;
	bool m_bRoll;
	bool m_bStop;

};

class SlotPollBar : public UIWidget
{
public:
	enum Event
	{
		SLOT_POLL_DOWN = 1100
	};

	SlotPollBar();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	virtual bool IsIn(VeFloat32 x, VeFloat32 y);

	void SetArrowsMove(bool bMove);

protected:
	void UpdateArrows(VeFloat32 f32Delta);

	void UpdateHandle();

	void Move(VeFloat32 f32Pos);

	void Update(VeFloat32 f32Delta);

	cocos2d::CCProgressTimer* m_pkGrooveFill;
	cocos2d::CCSprite* m_apkArrows[3];
	cocos2d::CCSprite* m_pkHandle;
	VeFloat32 m_f32PassTime;
	VeFloat32 m_f32Rate;
	bool m_bIsTouchHandle;


	VeFloat32 m_f32Top;
	VeFloat32 m_f32Bottom;
	VeFloat32 m_f32Middle;
	VeFloat32 m_f32Velocity;
	VeFloat32 m_f32RollBackTime;
	VeFloat32 m_f32RollBackPos;
	bool m_bTouch;
	bool m_bRollBack;
	bool m_bArrowsMove;
};

class Slot : public UILayerExt<Slot>
{
public:
	Slot();

	virtual ~Slot();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(Slot);

	void SetChips(VeUInt64 u64Win, VeUInt64 u64Credit);

	void SetBet(VeUInt32 u32Bet);

	VeUInt32 GetBet();

	void Stop(VeUInt64 u64Win, VeUInt8 u8Left, VeUInt8 u8Middle, VeUInt8 u8Right);

	void SetJPChips(VeUInt64 u64JPChips);

	void SetSingleBet(VeUInt32 u32SingleBet);

	void Tick(VeFloat32 f32Delta);

	void UpdateSlot();

	virtual void OnPushNotice(const VeChar8* pcNotice);

protected:
	void WinChipsUpdate(VeFloat32 f32Delta);

	void ScheduleWinChipsUpdate(VeFloat32 f32Delta);

	UIDeclareDelegate(OnBack);
	UIDeclareDelegate(OnPurchase);
	UIDeclareDelegate(OnOdds);
	UIDeclareDelegate(OnInc);
	UIDeclareDelegate(OnDec);
	UIDeclareDelegate(OnPollDown);
	UIDeclareDelegate(OnRollStop);

	cocos2d::CCSprite* m_pkIncOnBetBtn;
	cocos2d::CCSprite* m_pkDecOnBetBtn;
	cocos2d::CCLabelTTF* m_pkWinChips;
	cocos2d::CCLabelTTF* m_pkCredit;
	VeUInt32 m_u32Bet;
	VeUInt32 m_u32SingleBet;
	bool m_bIsRoll;

	VeUInt64 m_u64WinChips;
	VeUInt64 m_u64NowChips;
	VeUInt64 m_u64WillWin;

};

class SlotHelp : public UILayerModalExt<SlotHelp>
{
public:
	SlotHelp();

	virtual ~SlotHelp();

	virtual bool init();

	VE_CREATE_FUNC(SlotHelp);

	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

};
