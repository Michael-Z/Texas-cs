#pragma once

#include "UIWidget.h"
#include "TexasPokerHallClient.h"

class Perspective;

#define CARD_TURN_DELAY (0.3f)

class Card : public UIWidget
{
public:
	enum Event
	{
		TURN_BACK,
		TURN_FACE,
		DEAL_OVER
	};

	Card();

	virtual ~Card();

	void Reset();

	void SetType(VeUInt32 u32Type);

	VeUInt32 GetType();

	void SetContent(CardNum eNum, CardSuit eSuit);

	CardNum GetNumber();

	CardSuit GetSuit();

	void Turn(VeFloat32 f32Delay = 0);

	bool IsFace();

	void SetFace(bool bFace);

	void SetAngle(VeFloat32 f32Angle);

	VeFloat32 GetAngle();

	void SetDealPos(cocos2d::CCPoint kPos);

	void Deal(VeFloat32 f32Delay = 0);

	void SetLight(bool bLight);

protected:
	void Update(VeFloat32 f32Delta);

	void OnDeal(VeFloat32 f32Delta);

	void OnTurn(VeFloat32 f32Delta);

	void UpdateDeal(VeFloat32 f32Delta);

	void UpdateType();

	void UpdateContent();

	void UpdateAngle();

	VeUInt32 m_u32Type;
	CardNum m_eNumber;
	CardSuit m_eSuit;
	VeAngle m_kAngle;
	CCNode* m_pkRoot;
	Perspective* m_pkCardFace;
	Perspective* m_pkCardBack;
	Perspective* m_pkNumber;
	Perspective* m_pkSuitSmall;
	Perspective* m_pkSuitLarge;
	cocos2d::CCSprite* m_pkLight;

	cocos2d::CCPoint m_kNumberOffset;
	cocos2d::CCPoint m_kSuitSmallOffset;
	cocos2d::CCPoint m_kSuitLargeOffset;

	cocos2d::CCPoint m_kDealPos;

	VeFloat32 m_f32TrunTime;
	bool m_bFace;
	
	VeFloat32 m_f32DealTime;
	bool m_bLight;

};

class MiniCard : public UIWidget
{
public:
	MiniCard();

	virtual ~MiniCard();

	void Reset();

	void SetType(VeUInt32 u32Type);

	VeUInt32 GetType();

	void SetContent(CardNum eNum, CardSuit eSuit);

	CardNum GetNumber();

	CardSuit GetSuit();

	bool IsFace();

	void SetFace(bool bFace);

	void SetAngle(VeFloat32 f32Angle);

	VeFloat32 GetAngle();

	void SetDealPos(cocos2d::CCPoint kPos);

	void Deal(VeFloat32 f32Delay = 0);

	void Turn(VeFloat32 f32Delay = 0);

protected:
	void Update(VeFloat32 f32Delta);

	void OnDeal(VeFloat32 f32Delta);

	void OnTurn(VeFloat32 f32Delta);

	void UpdateDeal(VeFloat32 f32Delta);

	void UpdateType();

	void UpdateContent();

	void UpdateAngle();

	VeUInt32 m_u32Type;
	CardNum m_eNumber;
	CardSuit m_eSuit;
	VeAngle m_kAngle;
	CCNode* m_pkRoot;
	Perspective* m_pkCardFace;
	Perspective* m_pkCardBack;
	Perspective* m_pkNumber;
	Perspective* m_pkSuitSmall;

	cocos2d::CCPoint m_kNumberOffset;
	cocos2d::CCPoint m_kSuitSmallOffset;

	cocos2d::CCPoint m_kDealPos;

	VeFloat32 m_f32TrunTime;
	bool m_bFace;

	VeFloat32 m_f32DealTime;

};
