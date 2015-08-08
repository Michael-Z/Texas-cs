#pragma once

#include "UILayer.h"
#include "TexasPokerHallClient.h"

class Card;
class MiniCard;
class NameCard;
class HeadChatBar;
class PlayerCard;
class UIPage;
class GameChips;
class GameChipsMove;
class GameDrinksMove;
class FillSlider;
class PlayerWin;

#define SEND_CARD_DELAY 0.3f
#define NORMAL_DELAY 1.0f

class Game : public UILayerExt<Game>
{
public:
	enum Grade
	{
		GRADE_NORMAL,
		GRADE_VIP,
		GRADE_MAX
	};

	enum ControlBtnState
	{
		CONTROL_CALL,
		CONTROL_CHECK,
		CONTROL_AUTO,
		STATE_MAX
	};

	enum TableCardType
	{
		DRAW_CARD,
		TURN_CARD,
		RIVER_CARD,
		ALL_TABLE_CARD,
		TURN_AND_RIVER_CARD,
		CARD_MAX
	};

	enum AutoState
	{
		AUTO_NULL,
		AUTO_CHECK,
		AUTO_CHECK_OR_DISCARD,
		AUTO_CALL_ANY,
		AUTO_MAX
	};

	Game();

	virtual ~Game();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	virtual void OnMenu();

	VE_CREATE_FUNC(Game);

	void SetSelfDrink(VeInt32 i32Index);

	void SetCategories(VeUInt32 u32Categories);

	void SetSelfSeatIndex(VeUInt32 u32Index);

	NameCard* GetCard(VeInt32 i32Target);

	HeadChatBar* GetHeadChatBar(VeInt32 i32Target);

	GameDrinksMove* GetDrink(VeInt32 i32Target);

	GameChips* GetChips(VeInt32 i32Target);

	void UpdateTurnCard(VeFloat32 f32Delta);

	void SetCotrolBtnState(ControlBtnState state, bool bEnable);

	void DisableSetCotrolBtn();

	void TurnTableCard(CCNode* pSender,void* TargetId);

	void UpdateTable(bool bMotion);

	void ToHall();

	void OnBuy(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To);

	void OnBuyAll(VeUInt8 u8Type, VeUInt8 u8From);

	void OnChat(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To, const VeChar8* strChat);

	void OnChatAll(VeUInt8 u8Type, VeUInt8 u8From, const VeChar8* strChat);

	virtual void OnPushNotice(const VeChar8* pcNotice);

protected:

	void CreateGradePage();

	void SetPlayerChipsNumberEffect(VeUInt32 u32Number);

	void UpdateTablePlayer(bool bMotion);

	void ResetAllCards();

	void SetAllCardsType(VeUInt32 u32Type);

	void ResetAllChips();

	void ResetAllNameCards();

	void SetChipsOnTable(VeInt32 i32Target, VeUInt32 u32Chips, bool bMotion);

	void ShowSB(VeFloat32 f32Delta);

	void ShowBB(VeFloat32 f32Delta);

	void WinChips(VeFloat32 f32Delta);

	void OpenCards(VeFloat32 f32Delta);

	void UpdateProcessTime(VeFloat32 f32Delta);

	void UpdateSlider();

	Card* m_apkSelfCard[2];
	Card* m_apkTableCard[5];

	NameCard* m_apkLeft[4];
	NameCard* m_apkRight[4];
	PlayerCard* m_pkPlayerCard;

	HeadChatBar* m_chatBarLeft[4];
	HeadChatBar* m_chatBarRight[4];
	HeadChatBar* m_pkPlayerChatBar;

	GameDrinksMove* m_apkDrinkLeft[4];
	GameDrinksMove* m_apkDrinkRight[4];
	GameDrinksMove* m_pkPlayerDrink;

	GameChips* m_apkLeftChips[4];
	GameChips* m_apkRightChips[4];
	GameChips* m_pkPlayerChips;
	VeUInt32 m_au32Chips[9];
	GameChipsMove* m_apkChipsMove[9];
	GameChips* m_pkShowPlayerChips;

	cocos2d::CCSprite* m_pkFillText;
	cocos2d::CCSprite* m_pkDiscardText;
	cocos2d::CCSprite* m_pkCheckText;
	cocos2d::CCSprite* m_pkCallText;

	UIPage* m_pkFillBar;
	FillSlider* m_pkSlider;
	cocos2d::CCSprite* m_pkFillBarConfirmText;
	cocos2d::CCSprite* m_pkBetNumber;
	cocos2d::CCLabelTTF* m_pkBetNumberText;
	cocos2d::CCSprite* m_pkBetAllin;
	cocos2d::CCSprite* m_pkAllinText;

	void SetBetNumber(VeUInt32 u32BetNumber);

	cocos2d::CCNode* m_pkAutoCheckNode;
	cocos2d::CCSprite* m_pkAutoCheckState[2];

	cocos2d::CCNode* m_pkCheckDiscardNode;
	cocos2d::CCSprite* m_pkCheckDiscardState[2];

	cocos2d::CCNode* m_pkCallAnyNode;
	cocos2d::CCSprite* m_pkCallAnyState[2];

	cocos2d::CCSprite* m_pkSelfDrink;

	cocos2d::CCLabelTTF* m_pkCategories;

	PlayerWin* m_pkPlayWin;

	VeInt32 m_MyTurnSoundPlayCount;

	VeUInt32 m_u32SelfBetNumber;

	UIDeclareDelegate(OnFill);
	UIDeclareDelegate(OnFillConfirm);
	UIDeclareDelegate(OnDiscard);
	UIDeclareDelegate(OnCheck);
	UIDeclareDelegate(OnCall);

	UIDeclareDelegate(OnFillValue);

	UIDeclareDelegate(OnAutoCheck);
	UIDeclareDelegate(OnCheckDiscard);
	UIDeclareDelegate(OnCallAny);

	UIDeclareDelegate(OnHelp);
	UIDeclareDelegate(OnDrink);
	UIDeclareDelegate(OnChat);
	UIDeclareDelegate(OnMenu);

	TableState m_eStateLastUpdate;
	VeUInt32 m_u32ProcessLastUpdate;
	VeUInt64 m_u64ProcessLastExpireUpdate;
	AutoState m_eAutoState;

};
