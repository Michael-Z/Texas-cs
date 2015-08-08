#pragma once

#include "ClientAgent.h"
#include <time.h>

#define KENO_4HITS_PRIZE_POWER 2
#define KENO_5HITS_PRIZE_POWER 5
#define KENO_6HITS_PRIZE_POWER 12
#define KENO_7HITS_PRIZE_POWER 50
#define KENO_8HITS_PRIZE_POWER 300
#define KENO_9HITS_PRIZE_POWER 2500
#define KENO_10HITS_PRIZE_POWER 20000

class TexasPokerTable;

///	<CODE-GENERATE>{Enums}
enum LanguageS
{
	LAN_zh_CN,
	LAN_zh_TW,
	LAN_en_US,
	LanguageS_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, LanguageS eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, LanguageS& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < LanguageS_MAX ? (LanguageS)u8Data : LanguageS_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum LoginResult
{
	LOGIN_SUCCEED,
	LOGIN_CREATE,
	LOGIN_EXIST,
	LOGIN_WRONG_VERSION,
	LOGIN_FAILED,
	LoginResult_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, LoginResult eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, LoginResult& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < LoginResult_MAX ? (LoginResult)u8Data : LoginResult_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum RoomType
{
	ROOM_TYPE_PRACTICE,
	ROOM_TYPE_NORMAL,
	ROOM_TYPE_EXPERT,
	ROOM_TYPE_VIP,
	ROOM_TYPE_CHAMPIONSHIP,
	ROOM_TYPE_KNOCKOUT,
	RoomType_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, RoomType eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, RoomType& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < RoomType_MAX ? (RoomType)u8Data : RoomType_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum RoomFlag
{
	ROOM_FLAG_VIP = 0x1,
	ROOM_FLAG_HOT = 0x2,
	RoomFlag_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, RoomFlag eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, RoomFlag& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < RoomFlag_MAX ? (RoomFlag)u8Data : RoomFlag_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum EditInfoRes
{
	EDIT_S_OK,
	EDIT_E_FAIL,
	EditInfoRes_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, EditInfoRes eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, EditInfoRes& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < EditInfoRes_MAX ? (EditInfoRes)u8Data : EditInfoRes_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum ExchangeRes
{
	EXCHANGE_S_OK,
	EXCHANGE_E_FAIL,
	ExchangeRes_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, ExchangeRes eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, ExchangeRes& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < ExchangeRes_MAX ? (ExchangeRes)u8Data : ExchangeRes_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum KenoRes
{
	KENO_S_OK,
	KENO_E_FAIL,
	KenoRes_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, KenoRes eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, KenoRes& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < KenoRes_MAX ? (KenoRes)u8Data : KenoRes_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum BaccaratBetType
{
	BACC_BANKER,
	BACC_PLAYER,
	BACC_TIE,
	BaccaratBetType_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, BaccaratBetType eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, BaccaratBetType& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < BaccaratBetType_MAX ? (BaccaratBetType)u8Data : BaccaratBetType_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum BaccaratRes
{
	BACC_S_OK,
	BACC_E_FAIL,
	BaccaratRes_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, BaccaratRes eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, BaccaratRes& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < BaccaratRes_MAX ? (BaccaratRes)u8Data : BaccaratRes_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum EnterTableRes
{
	ENTER_S_OK,
	ENTER_E_FAIL,
	ENTER_NO_CHIPS,
	ENTER_NOT_VIP,
	ENTER_HAS_TABLE,
	EnterTableRes_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, EnterTableRes eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, EnterTableRes& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < EnterTableRes_MAX ? (EnterTableRes)u8Data : EnterTableRes_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum TableState
{
	TABLE_WAIT,
	TABLE_DEAL_0,
	TABLE_DEAL_1,
	TABLE_DEAL_2,
	TABLE_DEAL_3,
	TABLE_PLAY_0,
	TABLE_PLAY_1,
	TABLE_PLAY_2,
	TABLE_PLAY_3,
	TABLE_CLEAR,
	TABLE_ALL_FOLD,
	TableState_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, TableState eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, TableState& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < TableState_MAX ? (TableState)u8Data : TableState_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum TablePlayerState
{
	TABLE_PLAYER_DISABLE,
	TABLE_PLAYER_HOLD,
	TABLE_PLAYER_READY,
	TABLE_PLAYER_PLAY,
	TABLE_PLAYER_FOLD,
	TABLE_PLAYER_RAISE,
	TABLE_PLAYER_CALL,
	TABLE_PLAYER_CHECK,
	TABLE_PLAYER_ALL_IN,
	TABLE_PLAYER_BIG_BLIND,
	TABLE_PLAYER_SMALL_BLIND,
	TablePlayerState_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, TablePlayerState eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, TablePlayerState& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < TablePlayerState_MAX ? (TablePlayerState)u8Data : TablePlayerState_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum CardNum
{
	CARD_A,
	CARD_2,
	CARD_3,
	CARD_4,
	CARD_5,
	CARD_6,
	CARD_7,
	CARD_8,
	CARD_9,
	CARD_10,
	CARD_J,
	CARD_Q,
	CARD_K,
	CardNum_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, CardNum eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, CardNum& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < CardNum_MAX ? (CardNum)u8Data : CardNum_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum CardSuit
{
	CARD_SPADE,
	CARD_HEART,
	CARD_CLUB,
	CARD_DIAMOND,
	CardSuit_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, CardSuit eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, CardSuit& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < CardSuit_MAX ? (CardSuit)u8Data : CardSuit_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum Categories
{
	CATE_ROYAL_FLUSH,
	CATE_STRAIGHT_FLUSH,
	CATE_FOUR_KIND,
	CATE_FULL_HOUSE,
	CATE_FLUSH,
	CATE_STRAIGHT,
	CATE_THREE_KIND,
	CATE_TWO_PAIR,
	CATE_ONE_PAIR,
	CATE_HIGH_CARD,
	Categories_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, Categories eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, Categories& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < Categories_MAX ? (Categories)u8Data : Categories_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum RefreshPurchaseRes
{
	PURCHASE_S_OK,
	PURCHASE_E_FAIL,
	RefreshPurchaseRes_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, RefreshPurchaseRes eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, RefreshPurchaseRes& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < RefreshPurchaseRes_MAX ? (RefreshPurchaseRes)u8Data : RefreshPurchaseRes_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum EventInfo
{
	EVENT_MODIFY,
	EVENT_PURCHASE,
	EVENT_VIP,
	EVENT_PRESENT,
	EVENT_ONLINE,
	EVENT_PLAY_COUNT,
	EVENT_PLAY_SLOT_COUNT,
	EVENT_PLAY_KENO_COUNT,
	EVENT_PLAY_BACC_COUNT,
	EVENT_WIN_COMBO_0,
	EVENT_WIN_COMBO_1,
	EVENT_WIN_COMBO_2,
	EventInfo_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, EventInfo eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, EventInfo& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < EventInfo_MAX ? (EventInfo)u8Data : EventInfo_MAX;
	if(!bRes) StreamError();
	return kIn;
}
enum EventAwardType
{
	EVENT_AWARD_CHIPS,
	EVENT_AWARD_GOLD,
	EVENT_AWARD_CARD,
	EVENT_AWARD_VIP,
	EventAwardType_MAX
};
VE_INLINE BitStream& operator << (BitStream& kOut, EventAwardType eData)
{
	VE_ASSERT(eData <= 0xff);
	kOut.Write(VeUInt8(eData));
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, EventAwardType& eData)
{
	VeUInt8 u8Data;
	bool bRes = kIn.Read(u8Data);
	eData = u8Data < EventAwardType_MAX ? (EventAwardType)u8Data : EventAwardType_MAX;
	if(!bRes) StreamError();
	return kIn;
}
///	</CODE-GENERATE>{Enums}

///	<CODE-GENERATE>{Structs}
struct RoomInfo
{
	RoomType m_tType;
	VeUInt32 m_u32SB;
	VeUInt32 m_u32BB;
	VeUInt32 m_u32ChipsMin;
	VeUInt32 m_u32ChipsMax;
	VeUInt8 m_u8Flags;
};
VE_INLINE BitStream& operator << (BitStream& kOut, const RoomInfo& kData)
{
	kOut << kData.m_tType;
	kOut << kData.m_u32SB;
	kOut << kData.m_u32BB;
	kOut << kData.m_u32ChipsMin;
	kOut << kData.m_u32ChipsMax;
	kOut << kData.m_u8Flags;
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, RoomInfo& kData)
{
	kIn >> kData.m_tType;
	kIn >> kData.m_u32SB;
	kIn >> kData.m_u32BB;
	kIn >> kData.m_u32ChipsMin;
	kIn >> kData.m_u32ChipsMax;
	kIn >> kData.m_u8Flags;
	return kIn;
}
struct CardData
{
	CardNum m_tNumber;
	CardSuit m_tSuit;
};
VE_INLINE BitStream& operator << (BitStream& kOut, const CardData& kData)
{
	kOut << kData.m_tNumber;
	kOut << kData.m_tSuit;
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, CardData& kData)
{
	kIn >> kData.m_tNumber;
	kIn >> kData.m_tSuit;
	return kIn;
}
struct Table
{
	TableState m_tState;
	VeVector<VeUInt8> m_tPlayerIndex;
	VeUInt8 m_u8Dealer;
	VeUInt8 m_u8SmallBlind;
	VeUInt8 m_u8Process;
	VeUInt8 m_u8BetFirst;
	VeUInt32 m_u32BetChips;
	VeVector<VeUInt32> m_tPool;
	VeUInt64 m_u64ProcessExpire;
	VeVector<CardData> m_tCardList;
	VeVector<CardData> m_tHandCardList;
};
VE_INLINE BitStream& operator << (BitStream& kOut, const Table& kData)
{
	kOut << kData.m_tState;
	kOut << kData.m_tPlayerIndex;
	kOut << kData.m_u8Dealer;
	kOut << kData.m_u8SmallBlind;
	kOut << kData.m_u8Process;
	kOut << kData.m_u8BetFirst;
	kOut << kData.m_u32BetChips;
	kOut << kData.m_tPool;
	kOut << kData.m_u64ProcessExpire;
	kOut << kData.m_tCardList;
	kOut << kData.m_tHandCardList;
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, Table& kData)
{
	kIn >> kData.m_tState;
	kIn >> kData.m_tPlayerIndex;
	kIn >> kData.m_u8Dealer;
	kIn >> kData.m_u8SmallBlind;
	kIn >> kData.m_u8Process;
	kIn >> kData.m_u8BetFirst;
	kIn >> kData.m_u32BetChips;
	kIn >> kData.m_tPool;
	kIn >> kData.m_u64ProcessExpire;
	kIn >> kData.m_tCardList;
	kIn >> kData.m_tHandCardList;
	return kIn;
}
struct TablePlayer
{
	VeUInt32 m_u32Index;
	VeStringA m_strName;
	VeUInt32 m_u32Chips;
	VeUInt32 m_u32ChipsOnTable;
	TablePlayerState m_tState;
	VeUInt8 m_u8HeadIcon;
	VeUInt8 m_u8Sex;
	VeUInt8 m_u8Vip;
	VeUInt8 m_u8PoolLevel;
	VeUInt8 m_u8Drink;
};
VE_INLINE BitStream& operator << (BitStream& kOut, const TablePlayer& kData)
{
	kOut << kData.m_u32Index;
	kOut << kData.m_strName;
	kOut << kData.m_u32Chips;
	kOut << kData.m_u32ChipsOnTable;
	kOut << kData.m_tState;
	kOut << kData.m_u8HeadIcon;
	kOut << kData.m_u8Sex;
	kOut << kData.m_u8Vip;
	kOut << kData.m_u8PoolLevel;
	kOut << kData.m_u8Drink;
	return kOut;
}
VE_INLINE BitStream& operator >> (BitStream& kIn, TablePlayer& kData)
{
	kIn >> kData.m_u32Index;
	kIn >> kData.m_strName;
	kIn >> kData.m_u32Chips;
	kIn >> kData.m_u32ChipsOnTable;
	kIn >> kData.m_tState;
	kIn >> kData.m_u8HeadIcon;
	kIn >> kData.m_u8Sex;
	kIn >> kData.m_u8Vip;
	kIn >> kData.m_u8PoolLevel;
	kIn >> kData.m_u8Drink;
	return kIn;
}
///	</CODE-GENERATE>{Structs}

class TexasPokerHallPlayerS;

class TexasPokerHallServer : public Server
{
public:
///	<CODE-GENERATE>{EntitiesEnum}
	enum Entity
	{
		ENTITY_Player,
		ENTITY_MAX
	};
///	</CODE-GENERATE>{EntitiesEnum}

	typedef VeMemberDelegate<TexasPokerHallServer, VeTimeEvent&> TimeDelegate;

	TexasPokerHallServer();

	virtual ~TexasPokerHallServer();

	Entity EntNameToType(const VeChar8* pcName);

	const VeChar8* EntTypeToName(Entity eType);

	virtual ClientAgent* NewAgent(const SystemAddress& kAddress);

	virtual EntityS* NewEntity(const VeChar8* pcName, const VeChar8* pcIndex);

	virtual const VeChar8* GetTableName();

	virtual const VeChar8* GetKeyField();

	virtual const VeChar8* GetKey();

	virtual void OnStart();

	virtual void OnUpdate();

	virtual void OnEnd();

	const VeVector<RoomInfo>& GetRoomList();

	const VeVector<VeUInt32>& GetObjectSellPrice();

	const VeVector<VeUInt32>& GetObjectExchangePrice();

	const VeVector<VeUInt32>& GetDrinkPrice();

	VeUInt32 GetBankCardExpire();

	VeUInt32 GetBonus(VeUInt32 u32Level);

	VeUInt32 GetBonusPurchaseChips(VeUInt32 u32Level);

	VeUInt64 GetJPChips();

	void AddJPChips(VeUInt64 u64Chips);

	void DelJPChips(VeUInt64 u64Chips);

	VeUInt32 GetKenoIssue();

	VeUInt64 GetKenoDrawTime();

	const VeVector<VeUInt32>& GetKenoWin();

	void AddKenoPlayer(TexasPokerHallPlayerS* pkPlayer, VeUInt32 u32BetChips, const VeVector<VeUInt8>& tPlayerNumber);

	VeInt64 GetSlotJackPot();

	void AddSlotJackPot(VeInt64 i64AddNum);

	void ReduceSlotJackPot(VeInt64 i64AddNum);

	void AddBaccJackPot(VeInt64 i64AddNum);

	void ReduceBaccJackPot(VeInt64 i64ReduceNum);

	VeUInt32 GetBaccarat();

	VeUInt64 GetBaccaratPlayTime();

	VeUInt32 GetBaccaratBetPlayerNum();

	VeUInt64 GetBaccaratAllBet();

	void UpdateBaccPlayer(TexasPokerHallPlayerS* pkPlayer, BaccaratBetType eType, VeUInt32 u32BetChips);

	void IntoGameRoom(VeUInt32 u32Index, TexasPokerHallPlayerS* pkPlayer, TexasPokerTable* pkExcept);

	VePair<VeUInt32,VeUInt32> GetPurchaseInfo(const VeChar8* pcChannel, VeUInt32 u32Price);

	const VePair<VeUInt32,VeUInt32>& GetVipPurchase();

	VeUInt32 GetEventTick();

	const VeVector<VeStringA>& GetNameArray();

	VeInt64_S& GetPokerJackPot();

protected:
///	<CODE-GENERATE>{GlobalPropertiesDecl}
	VeUInt64_S m_u64JPChips;
	VeInt64_S m_i64SlotJackPot;
	VeInt64_S m_i64KenoJackPot;
	VeUInt32_S m_u32KenoIssue;
	VeInt64_S m_i64BaccaratJackPot;
	VeUInt32_S m_u32Baccarat;
	VeUInt32_S m_u32EventTick;
	VeInt64_S m_i64PokerJackPot;
///	</CODE-GENERATE>{GlobalPropertiesDecl}

protected:
	void UpdateObjectExchangePrice();

	VeVector<RoomInfo> m_kRoomList;
	VeVector<VeUInt32> m_kObjectPrice;
	VeVector<VeUInt32> m_kObjectExchangePrice;
	VeVector<VeUInt32> m_kDrinkPrice;
	VeUInt32 m_u32BankCardExpire;
	VeVector< VePair<VeUInt32,VeUInt32> > m_kBonus;
	VeUInt32 m_u32KenoIssuePeriod;
	VeVector<VeUInt32> m_kKenoWin;
	VeUInt32 m_u32BaccaratPeriod;
	VeStringMap<VeUInt32> m_kPurchaseChannelMap;
	VeVector< VeHashMap< VeUInt32,VePair<VeUInt32,VeUInt32> > > m_kPurchaseChannelList;
	VePair<VeUInt32,VeUInt32> m_kVipPurchase;

//Keno System
protected:
	void _KenoDraw(VeTimeEvent& kEvent);

	void OnKenoDraw();

	void DrawKeno(VeUInt8 au8Num[10]);

	bool WrongNumCorrect(VeUInt8 au32KenoNum[] ,VeUInt32 u32WrongNum);

	TIMER_DECL(TexasPokerHallServer, _KenoDraw);
	struct Player
	{
		TexasPokerHallPlayerS* m_pkPlayer;
		VeUInt8 m_au8KenoNumbers[10];
		VeUInt32 m_u32BetChips;
	};
	VeVector<Player> m_kKenoPlayerList;

//Baccarat System
protected:
	void _BaccaratPlay(VeTimeEvent& kEvent);

	void OnBaccaratPlay();

	BaccaratBetType DrawBacc();

	TIMER_DECL(TexasPokerHallServer, _BaccaratPlay);
	VeRefList<TexasPokerHallPlayerS*> m_kBaccaratPlayerList;
	VeUInt64 m_au64BaccBetChips[BaccaratBetType_MAX];

//TexasPoker System
protected:
	VeRefList<TexasPokerTable*>* m_pkTableArray;

//Event System
protected:
	TIMER_DECL(TexasPokerHallServer, _TickEvent);
	tm m_kLastLocalTime;

protected:
	VeVector<VeStringA> m_kNameArray;

};
