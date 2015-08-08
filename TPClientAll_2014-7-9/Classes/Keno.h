#pragma once

#include "UILayer.h"
#include "UIWidget.h"
#include "Knickknacks.h"

#define KENO_BET_MAX 10
#define KENO_TICKET_SHOW (0.5f)

class KenoMatrix : public UIWidget
{
public:
	enum Event
	{
		EVENT_UPDATE
	};

	KenoMatrix(cocos2d::CCSpriteBatchNode* pkRoot);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	virtual bool IsIn(VeFloat32 x, VeFloat32 y);

	void Clear();

	void SetChoose(const VeVector<VeUInt32>& kChoose);

	void SetChoose(const unsigned int* ui32Choose);

	const VeVector<VeUInt32>& GetChoose();

protected:
	struct Lattice
	{
		enum State
		{
			STATE_NORMAL,
			STATE_PRESSED,
			STATE_NORMAL_CHOOSED,
			STATE_PRESSED_CHOOSED,
			STATE_DISABLED
		};
		cocos2d::CCSprite* m_apkStateSprite[5];
		cocos2d::CCSprite* m_apkNumber[2];
		State m_eState;
		KenoMatrix* m_pkParent;

		void Press();

		void Release();

		void EventRelease();

		void SetState(State eState);

		State GetState();

	};

	void Update();

	Lattice m_akMatrix[8][10];
	cocos2d::CCRect m_kRange;
	Lattice* m_pkPressed;
	VeVector<VeUInt32> m_kChoosed;

};

class KenoCard : public UIWidget
{
public:
	KenoCard();

	virtual void visit();

	void Show();

	void Update(VeFloat32 f32Delta);

	void SetChoose(const VeVector<VeUInt32>& kChoose);

protected:
	VeFloat32 m_f32PosYTop;
	VeFloat32 m_f32PosYBottom;
	cocos2d::CCSize m_kMaxSize;
	VeFloat32 m_f32Delay;
	cocos2d::CCSprite* m_apkNumber[10][2];
	VeFloat32 m_af32NumberX[10];

};

class Keno : public UILayerExt<Keno>
{
public:
	Keno();

	virtual ~Keno();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(Keno);

	void SetBetNumber(VeUInt32 u32Number);

	void SetIssueNumber(VeUInt32 u32Number);

	void SetBet(VeUInt32 u32Bet);

	VeUInt32 GetBet();
	
	void SetChips(VeUInt32 u32Credit);

	void UpdateDrawTime();

	void OnBuySucc();

	void OnBuyFailed();

	void UpdateKenoResult(bool bShow);

	virtual void OnForceUpdate();

	virtual void OnPushNotice(const VeChar8* pcNotice);

protected:
	void Update_State();

	void Disable();	

	void Update(VeFloat32 f32Delta);

	UIDeclareDelegate(OnMatrixUpdate);
	UIDeclareDelegate(OnClear);
	UIDeclareDelegate(OnMax);
	UIDeclareDelegate(OnDec);
	UIDeclareDelegate(OnInc);
	UIDeclareDelegate(OnAuto);
	UIDeclareDelegate(OnBuy);
	UIDeclareDelegate(OnBack);
	UIDeclareDelegate(OnPurchase);

	cocos2d::CCLabelTTF* m_pkCredit;
	cocos2d::CCSprite* m_pkMaxText;
	cocos2d::CCSprite* m_pkClearText;
	cocos2d::CCSprite* m_pkBuyText;
	cocos2d::CCSprite* m_pkAutoText;
	cocos2d::CCLabelTTF* m_pkBetNumber;
	cocos2d::CCSprite* m_pkGlobal;
	cocos2d::CCLabelTTF* m_pkIssueNumber;
	cocos2d::CCLabelTTF* m_pkDrawTime;
	VeUInt32 m_u32Bet;
	VeUInt32 m_BetNum;

};

#define KENO_BALL_FALL (0.15f)

class KenoBall : public UIWidget
{
public:
	KenoBall();

	void SetNumber(VeUInt32 u32Number);

	void Down();

protected:
	void DownSchedule(VeFloat32 f32Delta);

	void Update(VeFloat32 f32Delta);

	cocos2d::CCNode* m_pkRoot;
	cocos2d::CCSprite* m_pkBall;
	cocos2d::CCSprite* m_apkNumber[2];
	VeFloat32 m_f32PassTime;

};

class RunKeno : public UILayerModalExt<RunKeno>
{
public:
	class Callback : public UIUserData
	{
		VeDeclareRTTI;
	public:
		VeUInt32 m_u32IssueNumber;
		VeUInt32 m_au32KenoNumber[10];
		VeUInt32 m_au32PlayerNumber[10];

	};

	RunKeno();

	virtual ~RunKeno();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(RunKeno);

	void SetIssueNumber(VeUInt32 u32Number);

	void SetChoose(VeUInt32 au32Choose[10]);

	void SetLabel(const VeChar8* pcContent);

	bool IsPlaying();

protected:
	void FadeIn(VeFloat32 f32Delta);

	void FadeOut(VeFloat32 f32Delta);

	void Update();

	void Run(VeFloat32 f32Delta);

	void RunGlobal();

	void RunNext(VeFloat32 f32Delta);

	void WaitFade(VeFloat32 f32Delta);

	UIWidget* m_pkRoot;
	cocos2d::CCSprite* m_pkGlobal;
	cocos2d::CCLabelTTF* m_pkIssueNumber;
	cocos2d::CCSprite* m_apkNumber[10][2];
	VeFloat32 m_af32NumberX[10];
	cocos2d::CCSprite* m_apkHitCyc[10];
	KenoBall* m_apkBall[10];
	cocos2d::CCLabelTTF* m_pkRunLabel;
	VeFloat32 m_f32Rate;
	VeUInt32 m_u32RunCount;
	UIUserDataPtr m_spData;
	bool m_bPlaying;

};
