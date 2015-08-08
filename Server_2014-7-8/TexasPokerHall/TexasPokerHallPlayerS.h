#pragma once

#include "EntityS.h"
#include "TexasPokerHallServer.h"

#define DISCOUNT 0.8f

#define PRIZE_LEVEL1 1
#define PRIZE_LEVEL2 1.2f
#define PRIZE_LEVEL3 1.4f
#define PRIZE_LEVEL4 1.6f
#define PRIZE_LEVEL5 1.8f
#define PRIZE_LEVEL6 2
#define PRIZE_LEVEL7 2.5f
#define PRIZE_LEVEL8 3
#define PRIZE_LEVEL9 3.5f
#define PRIZE_LEVEL10 4
#define PRIZE_LEVEL11 5
#define PRIZE_LEVEL12 6
#define PRIZE_LEVEL13 7
#define PRIZE_LEVEL14 8
#define PRIZE_LEVEL15 9
#define PRIZE_LEVEL16 10
#define PRIZE_LEVEL17 15
#define PRIZE_LEVEL18 20
#define PRIZE_LEVEL19 25
#define PRIZE_LEVEL20 30
#define PRIZE_LEVEL21 40
#define PRIZE_LEVEL22 100
#define PRIZE_LEVEL23 150
#define PRIZE_LEVEL24 200
#define PRIZE_LEVEL25 500
#define JP_LEVEL 3000

class TexasPokerHallAgentS;

class TexasPokerHallPlayerS : public EntityS
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

	TexasPokerHallPlayerS(Server* pkParent, const VeChar8* pcIndex);

	virtual ~TexasPokerHallPlayerS();

	static const VeChar8* GetName();

	TexasPokerHallAgentS* GetAgent();

	virtual const VeChar8* GetTableName();

	virtual const VeChar8* GetKeyField();

	virtual void OnAfterLoad(bool bCreate);

	virtual void OnBeforeUnload();

	virtual void OnAfterAttach();

	virtual void OnBeforeDetach();

	virtual void OnRPCCallback(BitStream& kStream);

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

///	<CODE-GENERATE>{EntityServerPropertiesGetter Player}
	VeStringA_S& GetPlayerUUID()
	{
		return m_strPlayerUUID;
	}
	VeStringA_S& GetChannel()
	{
		return m_strChannel;
	}
	VeStringA_S& GetNickname()
	{
		return m_strNickname;
	}
	VeUInt64_S& GetChips()
	{
		return m_u64Chips;
	}
	VeUInt64_S& GetGoldBricks()
	{
		return m_u64GoldBricks;
	}
	VeUInt32_S& GetHeadIcon()
	{
		return m_u32HeadIcon;
	}
	VeUInt32_S& GetSex()
	{
		return m_u32Sex;
	}
	VeUInt32_S& GetTitle()
	{
		return m_u32Title;
	}
	VeUInt32_S& GetVip()
	{
		return m_u32Vip;
	}
	VeUInt32_S& GetBankCard()
	{
		return m_u32BankCard;
	}
	VeUInt64_S& GetBankCardExpire()
	{
		return m_u64BankCardExpire;
	}
	VeUInt32_S& GetObj0()
	{
		return m_u32Obj0;
	}
	VeUInt32_S& GetObj1()
	{
		return m_u32Obj1;
	}
	VeUInt32_S& GetObj2()
	{
		return m_u32Obj2;
	}
	VeUInt32_S& GetObj3()
	{
		return m_u32Obj3;
	}
	VeUInt32_S& GetMedal0()
	{
		return m_u32Medal0;
	}
	VeUInt32_S& GetMedal1()
	{
		return m_u32Medal1;
	}
	VeUInt32_S& GetMedal2()
	{
		return m_u32Medal2;
	}
	VeInt64_S& GetPlayChips()
	{
		return m_i64PlayChips;
	}
	VeUInt32_S& GetPlayCount()
	{
		return m_u32PlayCount;
	}
	VeUInt32_S& GetWinCount()
	{
		return m_u32WinCount;
	}
	VeUInt32_S& GetWinMaxBet()
	{
		return m_u32WinMaxBet;
	}
	VeStringA_S& GetBestType()
	{
		return m_strBestType;
	}
	VeStringA_S& GetPhone()
	{
		return m_strPhone;
	}
	VeInt64_S& GetSlotJackPot()
	{
		return m_i64SlotJackPot;
	}
	VeUInt32_S& GetSlotCount()
	{
		return m_u32SlotCount;
	}
	VeInt64_S& GetKenoJackPot()
	{
		return m_i64KenoJackPot;
	}
	VeUInt32_S& GetKenoCount()
	{
		return m_u32KenoCount;
	}
	VeStringA_S& GetKenoResult()
	{
		return m_strKenoResult;
	}
	VeInt64_S& GetBaccaratJackPot()
	{
		return m_i64BaccaratJackPot;
	}
	VeUInt32_S& GetBaccaratCount()
	{
		return m_u32BaccaratCount;
	}
	VeStringA_S& GetBaccaratResult()
	{
		return m_strBaccaratResult;
	}
	VeUInt64_S& GetPurchase()
	{
		return m_u64Purchase;
	}
	VeUInt64_S& GetPurchaseChips()
	{
		return m_u64PurchaseChips;
	}
	VeUInt32_S& GetPurchaseCount()
	{
		return m_u32PurchaseCount;
	}
	VeUInt32_S& GetEventModifyInfo()
	{
		return m_u32EventModifyInfo;
	}
	VeUInt32_S& GetEventPurchase()
	{
		return m_u32EventPurchase;
	}
	VeUInt32_S& GetEventVip()
	{
		return m_u32EventVip;
	}
	VeUInt32_S& GetEventPresent()
	{
		return m_u32EventPresent;
	}
	VeUInt32_S& GetEventTickCurrent()
	{
		return m_u32EventTickCurrent;
	}
	VeUInt32_S& GetEventTickOnline()
	{
		return m_u32EventTickOnline;
	}
	VeUInt32_S& GetEventTickPlayCount()
	{
		return m_u32EventTickPlayCount;
	}
	VeUInt32_S& GetEventTickPlaySlotCount()
	{
		return m_u32EventTickPlaySlotCount;
	}
	VeUInt32_S& GetEventTickPlayKenoCount()
	{
		return m_u32EventTickPlayKenoCount;
	}
	VeUInt32_S& GetEventTickPlayBaccCount()
	{
		return m_u32EventTickPlayBaccCount;
	}
	VeUInt32_S& GetEventTickPlayWinCombo0()
	{
		return m_u32EventTickPlayWinCombo0;
	}
	VeUInt32_S& GetEventTickPlayWinCombo1()
	{
		return m_u32EventTickPlayWinCombo1;
	}
	VeUInt32_S& GetEventTickPlayWinCombo2()
	{
		return m_u32EventTickPlayWinCombo2;
	}
	VeUInt32_S& GetTickPlayCount()
	{
		return m_u32TickPlayCount;
	}
	VeUInt32_S& GetTickPlaySlotCount()
	{
		return m_u32TickPlaySlotCount;
	}
	VeUInt32_S& GetTickPlayKenoCount()
	{
		return m_u32TickPlayKenoCount;
	}
	VeUInt32_S& GetTickPlayBaccCount()
	{
		return m_u32TickPlayBaccCount;
	}
	VeUInt32_S& GetTickPlayWinCombo0()
	{
		return m_u32TickPlayWinCombo0;
	}
	VeUInt32_S& GetTickPlayWinCombo1()
	{
		return m_u32TickPlayWinCombo1;
	}
	VeUInt32_S& GetTickPlayWinCombo2()
	{
		return m_u32TickPlayWinCombo2;
	}
	VeUInt32_S& GetPresentSend()
	{
		return m_u32PresentSend;
	}
///	</CODE-GENERATE>{EntityServerPropertiesGetter Player}

	bool NeedName();

	struct SlotResult
	{
		VeUInt64 m_u64Win;
		VeUInt8 m_au8Result[3];
	};

	VeUInt32 GetSingleBet();

	void GetSlotRunPro(VeUInt32 u32WinChips,SlotResult& kWinResultOut);

	void GetFailSlotRun(SlotResult& kResultOut);

	VeUInt32 GetRandNumExceptParameter(VeUInt32 m_u32ExceptNum);

	VeUInt32 GetBaccBetLimit();

	VeUInt32 GetBaccBetChips(BaccaratBetType eType);

	VeRefNode<TexasPokerHallPlayerS*>& GetBaccNode();

	void AddKenoJackPot(VeInt64 i64AddNum);

	void ReduceKenoJackPot(VeInt64 i64ReduceNum);

	void RecordKenoCount();

	void OnWin(VeUInt32 u32Bet);

	VeUInt32 OnRefreshPurchase();

	void OnEventUpdate();

protected:
	friend class TexasPokerHallAgentS;
	friend class TexasPokerTable;
	friend class TexasPokerHallServer;
///	<CODE-GENERATE>{EntityServerPropertiesDecl Player}
	VeStringA_S m_strPlayerUUID;
	VeStringA_S m_strChannel;
	VeStringA_S m_strNickname;
	VeUInt64_S m_u64Chips;
	VeUInt64_S m_u64GoldBricks;
	VeUInt32_S m_u32HeadIcon;
	VeUInt32_S m_u32Sex;
	VeUInt32_S m_u32Title;
	VeUInt32_S m_u32Vip;
	VeUInt32_S m_u32BankCard;
	VeUInt64_S m_u64BankCardExpire;
	VeUInt32_S m_u32Obj0;
	VeUInt32_S m_u32Obj1;
	VeUInt32_S m_u32Obj2;
	VeUInt32_S m_u32Obj3;
	VeUInt32_S m_u32Medal0;
	VeUInt32_S m_u32Medal1;
	VeUInt32_S m_u32Medal2;
	VeInt64_S m_i64PlayChips;
	VeUInt32_S m_u32PlayCount;
	VeUInt32_S m_u32WinCount;
	VeUInt32_S m_u32WinMaxBet;
	VeStringA_S m_strBestType;
	VeStringA_S m_strPhone;
	VeInt64_S m_i64SlotJackPot;
	VeUInt32_S m_u32SlotCount;
	VeInt64_S m_i64KenoJackPot;
	VeUInt32_S m_u32KenoCount;
	VeStringA_S m_strKenoResult;
	VeInt64_S m_i64BaccaratJackPot;
	VeUInt32_S m_u32BaccaratCount;
	VeStringA_S m_strBaccaratResult;
	VeUInt64_S m_u64Purchase;
	VeUInt64_S m_u64PurchaseChips;
	VeUInt32_S m_u32PurchaseCount;
	VeUInt32_S m_u32EventModifyInfo;
	VeUInt32_S m_u32EventPurchase;
	VeUInt32_S m_u32EventVip;
	VeUInt32_S m_u32EventPresent;
	VeUInt32_S m_u32EventTickCurrent;
	VeUInt32_S m_u32EventTickOnline;
	VeUInt32_S m_u32EventTickPlayCount;
	VeUInt32_S m_u32EventTickPlaySlotCount;
	VeUInt32_S m_u32EventTickPlayKenoCount;
	VeUInt32_S m_u32EventTickPlayBaccCount;
	VeUInt32_S m_u32EventTickPlayWinCombo0;
	VeUInt32_S m_u32EventTickPlayWinCombo1;
	VeUInt32_S m_u32EventTickPlayWinCombo2;
	VeUInt32_S m_u32TickPlayCount;
	VeUInt32_S m_u32TickPlaySlotCount;
	VeUInt32_S m_u32TickPlayKenoCount;
	VeUInt32_S m_u32TickPlayBaccCount;
	VeUInt32_S m_u32TickPlayWinCombo0;
	VeUInt32_S m_u32TickPlayWinCombo1;
	VeUInt32_S m_u32TickPlayWinCombo2;
	VeUInt32_S m_u32PresentSend;
///	</CODE-GENERATE>{EntityServerPropertiesDecl Player}

	void UpdateRoomInfo();

	void UpdateTable();

	void UpdateTableSeats();

	void UpdateHandCards();

	void FinishBindingPhone();

	void FinishPurchase();

	void FinishVip();

	void FinishPresent();

	void FinishOnline();

	void FinishPokerCount();

	void FinishSlotCount();

	void FinishKenoCount();

	void FinishBaccCount();

	VeRefNode<TexasPokerHallPlayerS*> m_kBaccNode;
	VeUInt32 m_au32BaccBetChips[BaccaratBetType_MAX];
	VeRefNode<TexasPokerHallPlayerS*> m_kTableNode;
	TexasPokerTable* m_pkTargetTable;
	VeUInt32 m_u32IndexOnTable;
	bool m_bNeedName;

};
