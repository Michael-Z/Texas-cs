#pragma once

#include "EntityC.h"
#include "TexasPokerHallClient.h"

class TexasPokerHallAgentC;

class TexasPokerHallPlayerC : public EntityC
{
public:
///	<CODE-GENERATE>{EntityFuncEnum Player}
	enum ServerRPCFunc
	{
		SERVER_ReqInitHall,
		SERVER_ReqTickHall,
		SERVER_ReqEditInfo,
		SERVER_ReqGetObjExchangePrice,
		SERVER_ReqExchangeObj,
		SERVER_ReqSlotBonusCard,
		SERVER_ReqSlotJP,
		SERVER_ReqSlotRun,
		SERVER_ReqKenoUpdate,
		SERVER_ReqBuyKeno,
		SERVER_ReqKenoDraw,
		SERVER_ReqBaccaratUpdate,
		SERVER_ReqBaccaratBet,
		SERVER_ReqBaccaratBetAll,
		SERVER_ReqBaccaratPlay,
		SERVER_ReqEnterPokerTable,
		SERVER_ReqLeavePokerTable,
		SERVER_ReqChangeSeat,
		SERVER_ReqAddChips,
		SERVER_ReqChangeRoom,
		SERVER_ReqReady,
		SERVER_ReqFold,
		SERVER_ReqCall,
		SERVER_ReqFill,
		SERVER_ReqBuy,
		SERVER_ReqBuyAll,
		SERVER_ReqRefreshPurchase,
		SERVER_ReqFinishEvent,
		SERVER_ReqChat,
		SERVER_ReqChatAll,
		SERVER_MAX
	};
	enum ClientRPCFunc
	{
		CLIENT_RecInitHall,
		CLIENT_UpdateRoomList,
		CLIENT_UpdateOnline,
		CLIENT_RecEditInfo,
		CLIENT_RecGetObjExchangePrice,
		CLIENT_RecExchangeObj,
		CLIENT_RecSlotBonusCard,
		CLIENT_RecSlotJP,
		CLIENT_RecSlotRun,
		CLIENT_RecKenoUpdate,
		CLIENT_RecBuyKeno,
		CLIENT_RecKenoDraw,
		CLIENT_RecBaccaratUpdate,
		CLIENT_RecBaccaratBet,
		CLIENT_RecBaccaratPlay,
		CLIENT_UpdateRoomInfo,
		CLIENT_UpdateTableSeats,
		CLIENT_UpdateTable,
		CLIENT_UpdateHandCards,
		CLIENT_RecEnterPokerTable,
		CLIENT_OnTableUpdate,
		CLIENT_OnTableUpdateNoMotion,
		CLIENT_OnBuy,
		CLIENT_OnBuyAll,
		CLIENT_OnPurchaseCompleted,
		CLIENT_RecRefreshPurchase,
		CLIENT_RecFinishEvent,
		CLIENT_RecNotice,
		CLIENT_OnChat,
		CLIENT_OnChatAll,
		CLIENT_MAX
	};
///	</CODE-GENERATE>{EntityFuncEnum Player}

	TexasPokerHallPlayerC(ServerAgent* pkAgent);

	virtual ~TexasPokerHallPlayerC();

	static const VeChar8* GetName();

	TexasPokerHallAgentC* GetAgent();

	virtual void OnLoad();

	virtual void OnUnload();

	virtual void OnUpdate();

	virtual void OnRPCCallback(BitStream& kStream);

///	<CODE-GENERATE>{EntityServerFuncDecl Player}
	void S_ReqInitHall();
	void S_ReqTickHall();
	void S_ReqEditInfo(const VeChar8* strNickname, VeUInt8 u8Sex, VeUInt8 u8HeadIcon, const VeChar8* strPhone);
	void S_ReqGetObjExchangePrice();
	void S_ReqExchangeObj(VeUInt32 u32Index);
	void S_ReqSlotBonusCard();
	void S_ReqSlotJP();
	void S_ReqSlotRun(VeUInt32 u32BetCount);
	void S_ReqKenoUpdate();
	void S_ReqBuyKeno(VeUInt8 u8Bet, const VeVector<VeUInt8>& tPlayerNumber);
	void S_ReqKenoDraw();
	void S_ReqBaccaratUpdate();
	void S_ReqBaccaratBet(const BaccaratBetType& tBetType, VeUInt32 u32Chips);
	void S_ReqBaccaratBetAll(VeUInt32 u32Banker, VeUInt32 u32Player, VeUInt32 u32Tie);
	void S_ReqBaccaratPlay();
	void S_ReqEnterPokerTable(VeUInt32 u32Index);
	void S_ReqLeavePokerTable();
	void S_ReqChangeSeat(VeUInt32 u32Index);
	void S_ReqAddChips();
	void S_ReqChangeRoom();
	void S_ReqReady(VeUInt32 u32Chips);
	void S_ReqFold();
	void S_ReqCall();
	void S_ReqFill(VeUInt32 u32Chips);
	void S_ReqBuy(VeUInt8 u8Type, VeUInt8 u8Index);
	void S_ReqBuyAll(VeUInt8 u8Type);
	void S_ReqRefreshPurchase();
	void S_ReqFinishEvent(const EventInfo& tInfo);
	void S_ReqChat(VeUInt8 u8Type, VeUInt8 u8Index, const VeChar8* strChat);
	void S_ReqChatAll(VeUInt8 u8Type, const VeChar8* strChat);
///	</CODE-GENERATE>{EntityServerFuncDecl Player}

///	<CODE-GENERATE>{EntityClientFuncDecl Player}
	void C_RecInitHall();
	void C_UpdateRoomList(const VeVector<RoomInfo>& tRoomList, const VeVector<VeUInt32>& tDrinkPrice, const VeVector<VeUInt32>& tObjPrice);
	void C_UpdateOnline(VeUInt32 u32Number);
	void C_RecEditInfo(const EditInfoRes& tResult);
	void C_RecGetObjExchangePrice(const VeVector<VeUInt32>& tExchangePriceList);
	void C_RecExchangeObj(const EditInfoRes& tResult);
	void C_RecSlotBonusCard(VeUInt32 u32Bonus);
	void C_RecSlotJP(VeUInt64 u64JPChips);
	void C_RecSlotRun(VeUInt64 u64WinChips, VeUInt8 u8Result0, VeUInt8 u8Result1, VeUInt8 u8Result2);
	void C_RecKenoUpdate(VeUInt32 u32IssueNumber, VeUInt64 u64ExpireTime);
	void C_RecBuyKeno(const KenoRes& tResult);
	void C_RecKenoDraw(const KenoRes& tResult, VeUInt8 u8Hit = 0, VeUInt32 u32Issue = 0, VeUInt32 u32Bet = 0, VeUInt64 u64Win = 0);
	void C_RecBaccaratUpdate(VeUInt32 u32Index, VeUInt64 u64ExpireTime, VeUInt32 u32AllBetPlayer, VeUInt64 u64AllBetChips);
	void C_RecBaccaratBet(const BaccaratRes& tResult);
	void C_RecBaccaratPlay(const BaccaratRes& tResult, VeUInt32 u32WinChips);
	void C_UpdateRoomInfo(const RoomInfo& tInfo);
	void C_UpdateTableSeats(VeUInt8 u8SeatIndex, const VeVector<TablePlayer>& tPlayers);
	void C_UpdateTable(const Table& tTabel);
	void C_UpdateHandCards(const VeVector<CardData>& tHandCards);
	void C_RecEnterPokerTable(const EnterTableRes& tResult);
	void C_OnTableUpdate();
	void C_OnTableUpdateNoMotion();
	void C_OnBuy(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To);
	void C_OnBuyAll(VeUInt8 u8Type, VeUInt8 u8From);
	void C_OnPurchaseCompleted(const VeChar8* strChannel, VeUInt32 u32Chips);
	void C_RecRefreshPurchase(const RefreshPurchaseRes& tResult);
	void C_RecFinishEvent(const EventAwardType& tType, VeUInt32 u32Value);
	void C_RecNotice(const VeChar8* strNotice, VeUInt16 u16Show, VeUInt16 u16Interval);
	void C_OnChat(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To, const VeChar8* strChat);
	void C_OnChatAll(VeUInt8 u8Type, VeUInt8 u8From, const VeChar8* strChat);
///	</CODE-GENERATE>{EntityClientFuncDecl Player}

///	<CODE-GENERATE>{EntityClientPropertiesGetter Player}
	const VeStringA& GetNickname()
	{
		return m_strNickname;
	}
	const VeUInt64& GetChips()
	{
		return m_u64Chips;
	}
	const VeUInt64& GetGoldBricks()
	{
		return m_u64GoldBricks;
	}
	const VeUInt32& GetHeadIcon()
	{
		return m_u32HeadIcon;
	}
	const VeUInt32& GetSex()
	{
		return m_u32Sex;
	}
	const VeUInt32& GetTitle()
	{
		return m_u32Title;
	}
	const VeUInt32& GetVip()
	{
		return m_u32Vip;
	}
	const VeUInt32& GetBankCard()
	{
		return m_u32BankCard;
	}
	const VeUInt64& GetBankCardExpire()
	{
		return m_u64BankCardExpire;
	}
	const VeUInt32& GetObj0()
	{
		return m_u32Obj0;
	}
	const VeUInt32& GetObj1()
	{
		return m_u32Obj1;
	}
	const VeUInt32& GetObj2()
	{
		return m_u32Obj2;
	}
	const VeUInt32& GetObj3()
	{
		return m_u32Obj3;
	}
	const VeUInt32& GetMedal0()
	{
		return m_u32Medal0;
	}
	const VeUInt32& GetMedal1()
	{
		return m_u32Medal1;
	}
	const VeUInt32& GetMedal2()
	{
		return m_u32Medal2;
	}
	const VeUInt32& GetPlayCount()
	{
		return m_u32PlayCount;
	}
	const VeUInt32& GetWinCount()
	{
		return m_u32WinCount;
	}
	const VeUInt32& GetWinMaxBet()
	{
		return m_u32WinMaxBet;
	}
	const VeStringA& GetBestType()
	{
		return m_strBestType;
	}
	const VeStringA& GetPhone()
	{
		return m_strPhone;
	}
	const VeUInt32& GetSlotCount()
	{
		return m_u32SlotCount;
	}
	const VeUInt32& GetKenoCount()
	{
		return m_u32KenoCount;
	}
	const VeStringA& GetKenoResult()
	{
		return m_strKenoResult;
	}
	const VeUInt32& GetBaccaratCount()
	{
		return m_u32BaccaratCount;
	}
	const VeStringA& GetBaccaratResult()
	{
		return m_strBaccaratResult;
	}
	const VeUInt64& GetPurchase()
	{
		return m_u64Purchase;
	}
	const VeUInt64& GetPurchaseChips()
	{
		return m_u64PurchaseChips;
	}
	const VeUInt32& GetPurchaseCount()
	{
		return m_u32PurchaseCount;
	}
	const VeUInt32& GetEventModifyInfo()
	{
		return m_u32EventModifyInfo;
	}
	const VeUInt32& GetEventPurchase()
	{
		return m_u32EventPurchase;
	}
	const VeUInt32& GetEventVip()
	{
		return m_u32EventVip;
	}
	const VeUInt32& GetEventPresent()
	{
		return m_u32EventPresent;
	}
	const VeUInt32& GetEventTickCurrent()
	{
		return m_u32EventTickCurrent;
	}
	const VeUInt32& GetEventTickOnline()
	{
		return m_u32EventTickOnline;
	}
	const VeUInt32& GetEventTickPlayCount()
	{
		return m_u32EventTickPlayCount;
	}
	const VeUInt32& GetEventTickPlaySlotCount()
	{
		return m_u32EventTickPlaySlotCount;
	}
	const VeUInt32& GetEventTickPlayKenoCount()
	{
		return m_u32EventTickPlayKenoCount;
	}
	const VeUInt32& GetEventTickPlayBaccCount()
	{
		return m_u32EventTickPlayBaccCount;
	}
	const VeUInt32& GetEventTickPlayWinCombo0()
	{
		return m_u32EventTickPlayWinCombo0;
	}
	const VeUInt32& GetEventTickPlayWinCombo1()
	{
		return m_u32EventTickPlayWinCombo1;
	}
	const VeUInt32& GetEventTickPlayWinCombo2()
	{
		return m_u32EventTickPlayWinCombo2;
	}
	const VeUInt32& GetTickPlayCount()
	{
		return m_u32TickPlayCount;
	}
	const VeUInt32& GetTickPlaySlotCount()
	{
		return m_u32TickPlaySlotCount;
	}
	const VeUInt32& GetTickPlayKenoCount()
	{
		return m_u32TickPlayKenoCount;
	}
	const VeUInt32& GetTickPlayBaccCount()
	{
		return m_u32TickPlayBaccCount;
	}
	const VeUInt32& GetTickPlayWinCombo0()
	{
		return m_u32TickPlayWinCombo0;
	}
	const VeUInt32& GetTickPlayWinCombo1()
	{
		return m_u32TickPlayWinCombo1;
	}
	const VeUInt32& GetTickPlayWinCombo2()
	{
		return m_u32TickPlayWinCombo2;
	}
	const VeUInt32& GetPresentSend()
	{
		return m_u32PresentSend;
	}
///	</CODE-GENERATE>{EntityClientPropertiesGetter Player}

	VeUInt32 GetSingleBet();

	VeUInt32 GetKenoIssue();

	VeUInt64 GetKenoDrawTime();

	VeUInt32 GetBaccIndex();

	VeUInt64 GetBaccExpireTime();

	VeUInt32 GetBaccBetPlayer();

	VeUInt64 GetBaccAllBet();

	VeUInt32 GetBaccBetLimit();

	const RoomInfo& GetRoomInfo();

	VeUInt8 GetSeatIndex();

	const Table& GetTableState();

	VeUInt32 GetTablePool();

	const VeVector<TablePlayer>& GetTablePlayers();

	const TablePlayer* GetTablePlayer(VeUInt32 u32Index);

	const VeVector<CardData>& GetHandCards();

	Categories GetCategories();

	Categories GetCategories(VeUInt32 u32Index);

	void UpdateCategories();

	bool IsHighLight(VeUInt32 u32Index);

	const VeVector<VeUInt32>& GetDrinkPrice();

	const VeVector<VeUInt32>& GetObjectPrice();

protected:
///	<CODE-GENERATE>{EntityClientPropertiesDecl Player}
	VeStringA_C m_strNickname;
	VeUInt64_C m_u64Chips;
	VeUInt64_C m_u64GoldBricks;
	VeUInt32_C m_u32HeadIcon;
	VeUInt32_C m_u32Sex;
	VeUInt32_C m_u32Title;
	VeUInt32_C m_u32Vip;
	VeUInt32_C m_u32BankCard;
	VeUInt64_C m_u64BankCardExpire;
	VeUInt32_C m_u32Obj0;
	VeUInt32_C m_u32Obj1;
	VeUInt32_C m_u32Obj2;
	VeUInt32_C m_u32Obj3;
	VeUInt32_C m_u32Medal0;
	VeUInt32_C m_u32Medal1;
	VeUInt32_C m_u32Medal2;
	VeUInt32_C m_u32PlayCount;
	VeUInt32_C m_u32WinCount;
	VeUInt32_C m_u32WinMaxBet;
	VeStringA_C m_strBestType;
	VeStringA_C m_strPhone;
	VeUInt32_C m_u32SlotCount;
	VeUInt32_C m_u32KenoCount;
	VeStringA_C m_strKenoResult;
	VeUInt32_C m_u32BaccaratCount;
	VeStringA_C m_strBaccaratResult;
	VeUInt64_C m_u64Purchase;
	VeUInt64_C m_u64PurchaseChips;
	VeUInt32_C m_u32PurchaseCount;
	VeUInt32_C m_u32EventModifyInfo;
	VeUInt32_C m_u32EventPurchase;
	VeUInt32_C m_u32EventVip;
	VeUInt32_C m_u32EventPresent;
	VeUInt32_C m_u32EventTickCurrent;
	VeUInt32_C m_u32EventTickOnline;
	VeUInt32_C m_u32EventTickPlayCount;
	VeUInt32_C m_u32EventTickPlaySlotCount;
	VeUInt32_C m_u32EventTickPlayKenoCount;
	VeUInt32_C m_u32EventTickPlayBaccCount;
	VeUInt32_C m_u32EventTickPlayWinCombo0;
	VeUInt32_C m_u32EventTickPlayWinCombo1;
	VeUInt32_C m_u32EventTickPlayWinCombo2;
	VeUInt32_C m_u32TickPlayCount;
	VeUInt32_C m_u32TickPlaySlotCount;
	VeUInt32_C m_u32TickPlayKenoCount;
	VeUInt32_C m_u32TickPlayBaccCount;
	VeUInt32_C m_u32TickPlayWinCombo0;
	VeUInt32_C m_u32TickPlayWinCombo1;
	VeUInt32_C m_u32TickPlayWinCombo2;
	VeUInt32_C m_u32PresentSend;
///	</CODE-GENERATE>{EntityClientPropertiesDecl Player}

	VeUInt32 m_u32KenoIssue;
	VeUInt64 m_u64KenoDrawTime;

	VeUInt32 m_u32BaccIndex;
	VeUInt64 m_u64BaccExpireTime;
	VeUInt32 m_u32BaccBetPlayer;
	VeUInt64 m_u64BaccAllBet;

	VeVector<VeUInt32> m_kDrinkPrice;
	VeVector<VeUInt32> m_kObjectPrice;

	VeStringA m_kLastPurchase;

	RoomInfo m_kRoomInfo;
	VeUInt32 m_u32SeatIndex;
	VeUInt32 m_u32TablePool;
	Table m_kTable;
	VeVector<TablePlayer> m_kPlayers;
	VeVector<CardData> m_kHandCards;
	Categories m_eCategories;
	VeVector<Categories> m_kPlayerCategories;
	bool m_abHighLight[7];

};
