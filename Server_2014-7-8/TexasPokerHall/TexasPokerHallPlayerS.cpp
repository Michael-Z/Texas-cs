#include "TexasPokerHallPlayerS.h"
#include "TexasPokerHallAgentS.h"
#include "TexasPokerTable.h"

//--------------------------------------------------------------------------
TexasPokerHallPlayerS::TexasPokerHallPlayerS(Server* pkParent,
	const VeChar8* pcIndex)
	: EntityS(pkParent, TexasPokerHallPlayerS::GetName(), pcIndex)
///	<CODE-GENERATE>{EntityServerPropertiesImpl Player}
	, m_strPlayerUUID(this, FLAG_DATABASE, "PlayerUUID")
	, m_strChannel(this, FLAG_DATABASE, "Channel")
	, m_strNickname(this, FLAG_DATABASE | FLAG_CLIENT, "Nickname")
	, m_u64Chips(this, FLAG_DATABASE | FLAG_CLIENT, "Chips", 5000)
	, m_u64GoldBricks(this, FLAG_DATABASE | FLAG_CLIENT, "GoldBricks")
	, m_u32HeadIcon(this, FLAG_DATABASE | FLAG_CLIENT, "HeadIcon")
	, m_u32Sex(this, FLAG_DATABASE | FLAG_CLIENT, "Sex")
	, m_u32Title(this, FLAG_DATABASE | FLAG_CLIENT, "Title")
	, m_u32Vip(this, FLAG_DATABASE | FLAG_CLIENT, "Vip")
	, m_u32BankCard(this, FLAG_DATABASE | FLAG_CLIENT, "BankCard")
	, m_u64BankCardExpire(this, FLAG_DATABASE | FLAG_CLIENT, "BankCardExpire")
	, m_u32Obj0(this, FLAG_DATABASE | FLAG_CLIENT, "Obj0")
	, m_u32Obj1(this, FLAG_DATABASE | FLAG_CLIENT, "Obj1")
	, m_u32Obj2(this, FLAG_DATABASE | FLAG_CLIENT, "Obj2")
	, m_u32Obj3(this, FLAG_DATABASE | FLAG_CLIENT, "Obj3")
	, m_u32Medal0(this, FLAG_DATABASE | FLAG_CLIENT, "Medal0")
	, m_u32Medal1(this, FLAG_DATABASE | FLAG_CLIENT, "Medal1")
	, m_u32Medal2(this, FLAG_DATABASE | FLAG_CLIENT, "Medal2")
	, m_i64PlayChips(this, FLAG_DATABASE, "PlayChips")
	, m_u32PlayCount(this, FLAG_DATABASE | FLAG_CLIENT, "PlayCount")
	, m_u32WinCount(this, FLAG_DATABASE | FLAG_CLIENT, "WinCount")
	, m_u32WinMaxBet(this, FLAG_DATABASE | FLAG_CLIENT, "WinMaxBet")
	, m_strBestType(this, FLAG_DATABASE | FLAG_CLIENT, "BestType")
	, m_strPhone(this, FLAG_DATABASE | FLAG_CLIENT, "Phone")
	, m_i64SlotJackPot(this, FLAG_DATABASE, "SlotJackPot")
	, m_u32SlotCount(this, FLAG_DATABASE | FLAG_CLIENT, "SlotCount")
	, m_i64KenoJackPot(this, FLAG_DATABASE, "KenoJackPot")
	, m_u32KenoCount(this, FLAG_DATABASE | FLAG_CLIENT, "KenoCount")
	, m_strKenoResult(this, FLAG_DATABASE | FLAG_CLIENT, "KenoResult", "0")
	, m_i64BaccaratJackPot(this, FLAG_DATABASE, "BaccaratJackPot")
	, m_u32BaccaratCount(this, FLAG_DATABASE | FLAG_CLIENT, "BaccaratCount")
	, m_strBaccaratResult(this, FLAG_DATABASE | FLAG_CLIENT, "BaccaratResult", "0")
	, m_u64Purchase(this, FLAG_DATABASE | FLAG_CLIENT, "Purchase")
	, m_u64PurchaseChips(this, FLAG_DATABASE | FLAG_CLIENT, "PurchaseChips")
	, m_u32PurchaseCount(this, FLAG_DATABASE | FLAG_CLIENT, "PurchaseCount")
	, m_u32EventModifyInfo(this, FLAG_DATABASE | FLAG_CLIENT, "EventModifyInfo")
	, m_u32EventPurchase(this, FLAG_DATABASE | FLAG_CLIENT, "EventPurchase")
	, m_u32EventVip(this, FLAG_DATABASE | FLAG_CLIENT, "EventVip")
	, m_u32EventPresent(this, FLAG_DATABASE | FLAG_CLIENT, "EventPresent")
	, m_u32EventTickCurrent(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickCurrent")
	, m_u32EventTickOnline(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickOnline")
	, m_u32EventTickPlayCount(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlayCount")
	, m_u32EventTickPlaySlotCount(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlaySlotCount")
	, m_u32EventTickPlayKenoCount(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlayKenoCount")
	, m_u32EventTickPlayBaccCount(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlayBaccCount")
	, m_u32EventTickPlayWinCombo0(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlayWinCombo0")
	, m_u32EventTickPlayWinCombo1(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlayWinCombo1")
	, m_u32EventTickPlayWinCombo2(this, FLAG_DATABASE | FLAG_CLIENT, "EventTickPlayWinCombo2")
	, m_u32TickPlayCount(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlayCount")
	, m_u32TickPlaySlotCount(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlaySlotCount")
	, m_u32TickPlayKenoCount(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlayKenoCount")
	, m_u32TickPlayBaccCount(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlayBaccCount")
	, m_u32TickPlayWinCombo0(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlayWinCombo0")
	, m_u32TickPlayWinCombo1(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlayWinCombo1")
	, m_u32TickPlayWinCombo2(this, FLAG_DATABASE | FLAG_CLIENT, "TickPlayWinCombo2")
	, m_u32PresentSend(this, FLAG_DATABASE | FLAG_CLIENT, "PresentSend")
///	</CODE-GENERATE>{EntityServerPropertiesImpl Player}
	, m_bNeedName(false)
{
	m_kBaccNode.m_tContent = this;
	m_kTableNode.m_tContent = this;
	m_pkTargetTable = NULL;
	m_u32IndexOnTable = VE_INFINITE;
	m_strPlayerUUID = pcIndex;
}
//--------------------------------------------------------------------------
TexasPokerHallPlayerS::~TexasPokerHallPlayerS()
{

}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallPlayerS::GetName()
{
///	<CODE-GENERATE>{EntityName Player}
	return "Player";
///	</CODE-GENERATE>{EntityName Player}
}
//--------------------------------------------------------------------------
TexasPokerHallAgentS* TexasPokerHallPlayerS::GetAgent()
{
	return static_cast<TexasPokerHallAgentS*>(m_pkAgent);
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallPlayerS::GetTableName()
{
	return GetName();
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallPlayerS::GetKeyField()
{
///	<CODE-GENERATE>{EntityKeyField Player}
	return "PlayerUUID";
///	</CODE-GENERATE>{EntityKeyField Player}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnAfterLoad(bool bCreate)
{
	m_bNeedName = bCreate;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnBeforeUnload()
{

}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnAfterAttach()
{

}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnBeforeDetach()
{

}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnRPCCallback(BitStream& kStream)
{
///	<CODE-GENERATE>{EntityServerRPCCallback Player}
	StreamErrorClear();
	VeUInt8 u8FuncID;
	kStream >> u8FuncID;
	switch(u8FuncID)
	{
	case SERVER_ReqInitHall:
		{
			if(!IsStreamError()) S_ReqInitHall();
		}
		break;
	case SERVER_ReqTickHall:
		{
			if(!IsStreamError()) S_ReqTickHall();
		}
		break;
	case SERVER_ReqEditInfo:
		{
			VEString strNickname;
			kStream >> strNickname;
			VeUInt8 u8Sex;
			kStream >> u8Sex;
			VeUInt8 u8HeadIcon;
			kStream >> u8HeadIcon;
			VEString strPhone;
			kStream >> strPhone;
			if(!IsStreamError()) S_ReqEditInfo(strNickname, u8Sex, u8HeadIcon, strPhone);
		}
		break;
	case SERVER_ReqGetObjExchangePrice:
		{
			if(!IsStreamError()) S_ReqGetObjExchangePrice();
		}
		break;
	case SERVER_ReqExchangeObj:
		{
			VeUInt32 u32Index;
			kStream >> u32Index;
			if(!IsStreamError()) S_ReqExchangeObj(u32Index);
		}
		break;
	case SERVER_ReqSlotBonusCard:
		{
			if(!IsStreamError()) S_ReqSlotBonusCard();
		}
		break;
	case SERVER_ReqSlotJP:
		{
			if(!IsStreamError()) S_ReqSlotJP();
		}
		break;
	case SERVER_ReqSlotRun:
		{
			VeUInt32 u32BetCount;
			kStream >> u32BetCount;
			if(!IsStreamError()) S_ReqSlotRun(u32BetCount);
		}
		break;
	case SERVER_ReqKenoUpdate:
		{
			if(!IsStreamError()) S_ReqKenoUpdate();
		}
		break;
	case SERVER_ReqBuyKeno:
		{
			VeUInt8 u8Bet;
			kStream >> u8Bet;
			VeVector<VeUInt8> tPlayerNumber;
			kStream >> tPlayerNumber;
			if(!IsStreamError()) S_ReqBuyKeno(u8Bet, tPlayerNumber);
		}
		break;
	case SERVER_ReqKenoDraw:
		{
			if(!IsStreamError()) S_ReqKenoDraw();
		}
		break;
	case SERVER_ReqBaccaratUpdate:
		{
			if(!IsStreamError()) S_ReqBaccaratUpdate();
		}
		break;
	case SERVER_ReqBaccaratBet:
		{
			BaccaratBetType tBetType;
			kStream >> tBetType;
			VeUInt32 u32Chips;
			kStream >> u32Chips;
			if(!IsStreamError()) S_ReqBaccaratBet(tBetType, u32Chips);
		}
		break;
	case SERVER_ReqBaccaratBetAll:
		{
			VeUInt32 u32Banker;
			kStream >> u32Banker;
			VeUInt32 u32Player;
			kStream >> u32Player;
			VeUInt32 u32Tie;
			kStream >> u32Tie;
			if(!IsStreamError()) S_ReqBaccaratBetAll(u32Banker, u32Player, u32Tie);
		}
		break;
	case SERVER_ReqBaccaratPlay:
		{
			if(!IsStreamError()) S_ReqBaccaratPlay();
		}
		break;
	case SERVER_ReqEnterPokerTable:
		{
			VeUInt32 u32Index;
			kStream >> u32Index;
			if(!IsStreamError()) S_ReqEnterPokerTable(u32Index);
		}
		break;
	case SERVER_ReqLeavePokerTable:
		{
			if(!IsStreamError()) S_ReqLeavePokerTable();
		}
		break;
	case SERVER_ReqChangeSeat:
		{
			VeUInt32 u32Index;
			kStream >> u32Index;
			if(!IsStreamError()) S_ReqChangeSeat(u32Index);
		}
		break;
	case SERVER_ReqAddChips:
		{
			if(!IsStreamError()) S_ReqAddChips();
		}
		break;
	case SERVER_ReqChangeRoom:
		{
			if(!IsStreamError()) S_ReqChangeRoom();
		}
		break;
	case SERVER_ReqReady:
		{
			VeUInt32 u32Chips;
			kStream >> u32Chips;
			if(!IsStreamError()) S_ReqReady(u32Chips);
		}
		break;
	case SERVER_ReqFold:
		{
			if(!IsStreamError()) S_ReqFold();
		}
		break;
	case SERVER_ReqCall:
		{
			if(!IsStreamError()) S_ReqCall();
		}
		break;
	case SERVER_ReqFill:
		{
			VeUInt32 u32Chips;
			kStream >> u32Chips;
			if(!IsStreamError()) S_ReqFill(u32Chips);
		}
		break;
	case SERVER_ReqBuy:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VeUInt8 u8Index;
			kStream >> u8Index;
			if(!IsStreamError()) S_ReqBuy(u8Type, u8Index);
		}
		break;
	case SERVER_ReqBuyAll:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			if(!IsStreamError()) S_ReqBuyAll(u8Type);
		}
		break;
	case SERVER_ReqChat:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VeUInt8 u8Index;
			kStream >> u8Index;
			VEString strChat;
			kStream >> strChat;
			if(!IsStreamError()) S_ReqChat(u8Type, u8Index, strChat);
		}
		break;
	case SERVER_ReqChatAll:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VEString strChat;
			kStream >> strChat;
			if(!IsStreamError()) S_ReqChatAll(u8Type, strChat);
		}
		break;
	case SERVER_ReqRefreshPurchase:
		{
			if(!IsStreamError()) S_ReqRefreshPurchase();
		}
		break;
	case SERVER_ReqFinishEvent:
		{
			EventInfo tInfo;
			kStream >> tInfo;
			if(!IsStreamError()) S_ReqFinishEvent(tInfo);
		}
		break;
	default:
		break;
	}
///	</CODE-GENERATE>{EntityServerRPCCallback Player}
}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{EntityClientFuncSend Player}
void TexasPokerHallPlayerS::C_RecInitHall()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecInitHall);
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_UpdateRoomList(const VeVector<RoomInfo>& tRoomList, const VeVector<VeUInt32>& tDrinkPrice, const VeVector<VeUInt32>& tObjPrice)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_UpdateRoomList);
	kStream << tRoomList;
	kStream << tDrinkPrice;
	kStream << tObjPrice;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_UpdateOnline(VeUInt32 u32Number)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_UpdateOnline);
	kStream << u32Number;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecEditInfo(const EditInfoRes& tResult)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecEditInfo);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecGetObjExchangePrice(const VeVector<VeUInt32>& tExchangePriceList)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecGetObjExchangePrice);
	kStream << tExchangePriceList;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecExchangeObj(const EditInfoRes& tResult)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecExchangeObj);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecSlotBonusCard(VeUInt32 u32Bonus)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecSlotBonusCard);
	kStream << u32Bonus;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecSlotJP(VeUInt64 u64JPChips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecSlotJP);
	kStream << u64JPChips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecSlotRun(VeUInt64 u64WinChips, VeUInt8 u8Result0, VeUInt8 u8Result1, VeUInt8 u8Result2)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecSlotRun);
	kStream << u64WinChips;
	kStream << u8Result0;
	kStream << u8Result1;
	kStream << u8Result2;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecKenoUpdate(VeUInt32 u32IssueNumber, VeUInt64 u64ExpireTime)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecKenoUpdate);
	kStream << u32IssueNumber;
	kStream << u64ExpireTime;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecBuyKeno(const KenoRes& tResult)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecBuyKeno);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecKenoDraw(const KenoRes& tResult, VeUInt8 u8Hit, VeUInt32 u32Issue, VeUInt32 u32Bet, VeUInt64 u64Win)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecKenoDraw);
	kStream << tResult;
	kStream << u8Hit;
	kStream << u32Issue;
	kStream << u32Bet;
	kStream << u64Win;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecBaccaratUpdate(VeUInt32 u32Index, VeUInt64 u64ExpireTime, VeUInt32 u32AllBetPlayer, VeUInt64 u64AllBetChips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecBaccaratUpdate);
	kStream << u32Index;
	kStream << u64ExpireTime;
	kStream << u32AllBetPlayer;
	kStream << u64AllBetChips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecBaccaratBet(const BaccaratRes& tResult)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecBaccaratBet);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecBaccaratPlay(const BaccaratRes& tResult, VeUInt32 u32WinChips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecBaccaratPlay);
	kStream << tResult;
	kStream << u32WinChips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_UpdateRoomInfo(const RoomInfo& tInfo)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_UpdateRoomInfo);
	kStream << tInfo;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_UpdateTableSeats(VeUInt8 u8SeatIndex, const VeVector<TablePlayer>& tPlayers)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_UpdateTableSeats);
	kStream << u8SeatIndex;
	kStream << tPlayers;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_UpdateTable(const Table& tTabel)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_UpdateTable);
	kStream << tTabel;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_UpdateHandCards(const VeVector<CardData>& tHandCards)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_UpdateHandCards);
	kStream << tHandCards;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecEnterPokerTable(const EnterTableRes& tResult)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecEnterPokerTable);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_OnTableUpdate()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnTableUpdate);
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_OnTableUpdateNoMotion()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnTableUpdateNoMotion);
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_OnBuy(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnBuy);
	kStream << u8Type;
	kStream << u8From;
	kStream << u8To;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_OnBuyAll(VeUInt8 u8Type, VeUInt8 u8From)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnBuyAll);
	kStream << u8Type;
	kStream << u8From;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_OnChat(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To, const VeChar8* strChat)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnChat);
	kStream << u8Type;
	kStream << u8From;
	kStream << u8To;
	kStream << strChat;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_OnChatAll(VeUInt8 u8Type, VeUInt8 u8From, const VeChar8* strChat)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnChatAll);
	kStream << u8Type;
	kStream << u8From;
	kStream << strChat;
	SendPacket(kStream);
}
// added end
void TexasPokerHallPlayerS::C_OnPurchaseCompleted(const VeChar8* strChannel, VeUInt32 u32Chips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_OnPurchaseCompleted);
	kStream << strChannel;
	kStream << u32Chips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecRefreshPurchase(const RefreshPurchaseRes& tResult)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecRefreshPurchase);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecFinishEvent(const EventAwardType& tType, VeUInt32 u32Value)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecFinishEvent);
	kStream << tType;
	kStream << u32Value;
	SendPacket(kStream);
}
void TexasPokerHallPlayerS::C_RecNotice(const VeChar8* strNotice, VeUInt16 u16Show, VeUInt16 u16Interval)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(CLIENT_RecNotice);
	kStream << strNotice;
	kStream << u16Show;
	kStream << u16Interval;
	SendPacket(kStream);
}
///	</CODE-GENERATE>{EntityClientFuncSend Player}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{EntityServerFuncImpl Player}
void TexasPokerHallPlayerS::S_ReqInitHall()
{
///	<CODE-IMPLEMENT>{Player ReqInitHall}
	C_UpdateOnline(GetAgent()->GetServer()->GetClientNumber());
	C_UpdateRoomList(GetAgent()->GetServer()->GetRoomList(),
		GetAgent()->GetServer()->GetDrinkPrice(),
		GetAgent()->GetServer()->GetObjectSellPrice());
	if(m_pkTargetTable)
	{
		UpdateRoomInfo();
		UpdateTable();
		UpdateTableSeats();
		UpdateHandCards();
		C_RecEnterPokerTable(ENTER_S_OK);
	}
	else
	{
		OnEventUpdate();
		C_RecInitHall();
	}
///	</CODE-IMPLEMENT>{Player ReqInitHall}
}
void TexasPokerHallPlayerS::S_ReqTickHall()
{
///	<CODE-IMPLEMENT>{Player ReqTickHall}
	OnEventUpdate();
	C_UpdateOnline(GetAgent()->GetServer()->GetClientNumber());
///	</CODE-IMPLEMENT>{Player ReqTickHall}
}
void TexasPokerHallPlayerS::S_ReqEditInfo(const VeChar8* strNickname, VeUInt8 u8Sex, VeUInt8 u8HeadIcon, const VeChar8* strPhone)
{
///	<CODE-IMPLEMENT>{Player ReqEditInfo}
	if(VeStrlen(strNickname) <= 18 && u8Sex < 2 && u8HeadIcon < 16)
	{
		m_strNickname = strNickname;
		m_u32Sex = u8Sex;
		m_u32HeadIcon = u8HeadIcon;
		m_strPhone = strPhone;
		SyncToClient();
		C_RecEditInfo(EDIT_S_OK);
	}
	else
	{
		C_RecEditInfo(EDIT_E_FAIL);
	}
///	</CODE-IMPLEMENT>{Player ReqEditInfo}
}
void TexasPokerHallPlayerS::S_ReqGetObjExchangePrice()
{
///	<CODE-IMPLEMENT>{Player ReqGetObjExchangePrice}
	C_RecGetObjExchangePrice(GetAgent()->GetServer()->GetObjectExchangePrice());
///	</CODE-IMPLEMENT>{Player ReqGetObjExchangePrice}
}
void TexasPokerHallPlayerS::S_ReqExchangeObj(VeUInt32 u32Index)
{
///	<CODE-IMPLEMENT>{Player ReqExchangeObj}
	const VeVector<VeUInt32>& kExchange = GetAgent()->GetServer()->GetObjectExchangePrice();
	if(u32Index < kExchange.Size() && u32Index < 4)
	{
		VeUInt32_S& u32Obj = (&m_u32Obj0)[u32Index];
		if(u32Obj > 0)
		{
			u32Obj -= VeUInt32(1);
			m_u64Chips += VeUInt64(kExchange[u32Index]);
			SyncToClient();
			C_RecExchangeObj(EDIT_S_OK);
			return;
		}
	}
	C_RecExchangeObj(EDIT_E_FAIL);
///	</CODE-IMPLEMENT>{Player ReqExchangeObj}
}
void TexasPokerHallPlayerS::S_ReqSlotBonusCard()
{
///	<CODE-IMPLEMENT>{Player ReqSlotBonusCard}
	TexasPokerHallServer* pkServer = GetAgent()->GetServer();
	VeUInt64 u64ServerTime = pkServer->GetTime();
	if(GetBankCard() && (GetBankCardExpire() <= u64ServerTime))
	{
		VeUInt32 u32Bonus = pkServer->GetBonus(GetBankCard());
		m_u64BankCardExpire = u64ServerTime + pkServer->GetBankCardExpire();
		m_u64Chips += VeUInt64(u32Bonus);
		SyncToClient();
		C_RecSlotBonusCard(u32Bonus);
	}
	else
	{
		C_RecSlotBonusCard(0);
	}
///	</CODE-IMPLEMENT>{Player ReqSlotBonusCard}
}
void TexasPokerHallPlayerS::S_ReqSlotJP()
{
///	<CODE-IMPLEMENT>{Player ReqSlotJP}
	C_RecSlotJP(GetAgent()->GetServer()->GetJPChips());
///	</CODE-IMPLEMENT>{Player ReqSlotJP}
}
void TexasPokerHallPlayerS::S_ReqSlotRun(VeUInt32 u32BetCount)
{
///	<CODE-IMPLEMENT>{Player ReqSlotRun}
	if(u32BetCount < 10)
	{
		VeUInt32 u32BetChips = (u32BetCount + 1) * GetSingleBet();
		if(VeUInt64(m_u64Chips) >= u32BetChips)
		{
			TexasPokerHallServer* pkPokerHallServer = GetAgent()->GetServer();
			pkPokerHallServer->AddSlotJackPot(VeUInt64(u32BetChips * DISCOUNT));
			m_i64SlotJackPot += VeInt64(u32BetChips * DISCOUNT);
			m_u64Chips -= VeUInt64(u32BetChips);
			pkPokerHallServer->AddJPChips(u32BetChips);
			SlotResult kRes;
			GetSlotRunPro(u32BetChips,kRes);
			VE_ASSERT(kRes.m_au8Result[0] >= 0 && kRes.m_au8Result[0] <= 10);
			VE_ASSERT(kRes.m_au8Result[1] >= 0 && kRes.m_au8Result[1] <= 10);
			VE_ASSERT(kRes.m_au8Result[2] >= 0 && kRes.m_au8Result[2] <= 10);
			VE_ASSERT(kRes.m_u64Win < VE_UINT32_MAX);
			if(kRes.m_u64Win)
			{
				m_u64Chips += kRes.m_u64Win;
			}
			m_u32TickPlaySlotCount += VeUInt32(1);
			SyncToClient();
			C_RecSlotRun(kRes.m_u64Win, kRes.m_au8Result[0], kRes.m_au8Result[1], kRes.m_au8Result[2]);
			return;
		}
	}
	C_RecSlotRun(0, 0, 0, 0);
///	</CODE-IMPLEMENT>{Player ReqSlotRun}
}
void TexasPokerHallPlayerS::S_ReqKenoUpdate()
{
///	<CODE-IMPLEMENT>{Player ReqKenoUpdate}
	TexasPokerHallServer* pkServer = GetAgent()->GetServer();
	C_RecKenoUpdate(pkServer->GetKenoIssue(), pkServer->GetKenoDrawTime());
///	</CODE-IMPLEMENT>{Player ReqKenoUpdate}
}
void TexasPokerHallPlayerS::S_ReqBuyKeno(VeUInt8 u8Bet, const VeVector<VeUInt8>& tPlayerNumber)
{
///	<CODE-IMPLEMENT>{Player ReqBuyKeno}
	if((u8Bet < 10) && (tPlayerNumber.Size() == 10) && m_strKenoResult == "0")
	{
		bool bRight = true;
		for(VeUInt32 i(0); i < tPlayerNumber.Size(); ++i)
		{
			if(tPlayerNumber[i] < 1 || tPlayerNumber[i] > 80)
			{
				bRight = false;
				break;
			}
		}
		if(bRight)
		{
			VeUInt32 u32BetChips = (u8Bet + 1) * GetSingleBet();
			if(m_u64Chips >= u32BetChips)
			{
				m_u64Chips -= VeUInt64(u32BetChips);
				GetAgent()->GetServer()->AddKenoPlayer(this, u32BetChips, tPlayerNumber);
				m_u32TickPlayKenoCount += VeUInt32(1);
				SyncToClient();
				C_RecBuyKeno(KENO_S_OK);
				return;
			}
		}
	}
	C_RecBuyKeno(KENO_E_FAIL);
///	</CODE-IMPLEMENT>{Player ReqBuyKeno}
}
void TexasPokerHallPlayerS::S_ReqKenoDraw()
{
///	<CODE-IMPLEMENT>{Player ReqKenoDraw}
	VeStringA kResult = GetKenoResult();
	VeChar8 acBuffer[128];
	VeStrcpy(acBuffer, 128, kResult);
	VeChar8* pcContext;
	VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);
	if(pcTemp && (!VeStrcmp(pcTemp, "res")))
	{
		VeUInt32 u32BetChips;
		VeUInt32 u32IssueNumber;
		VeVector<VeUInt32> kKenoNumber;
		VeVector<VeUInt32> kPlayerNumber;
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		while(pcTemp)
		{
			if(pcTemp && VeStrcmp(pcTemp, "buy"))
			{
				kKenoNumber.PushBack(VeAtoi(pcTemp));
			}
			else
			{
				break;
			}
			pcTemp = VeStrtok(NULL, ",", &pcContext);
		}
		if(pcTemp && (!VeStrcmp(pcTemp, "buy")))
		{
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			u32IssueNumber = VeAtoi(pcTemp);
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			while(pcTemp)
			{
				if(pcTemp && VeStrcmp(pcTemp, "bet"))
				{
					kPlayerNumber.PushBack(VeAtoi(pcTemp));
				}
				else
				{
					break;
				}
				pcTemp = VeStrtok(NULL, ",", &pcContext);
			}
		}
		if(pcTemp && (!VeStrcmp(pcTemp, "bet")))
		{
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			u32BetChips = VeAtoi(pcTemp);
		}
		if(u32BetChips && kKenoNumber.Size() == 10 && kPlayerNumber.Size() == 10)
		{
			VeUInt8 u8Hit(0);
			for(VeUInt32 i(0); i < kKenoNumber.Size(); ++i)
			{
				for(VeUInt32 j(0); j < kPlayerNumber.Size(); ++j)
				{
					if(kKenoNumber[i] == kPlayerNumber[j])
					{
						++u8Hit;
						break;
					}
				}
			}
			u8Hit = VE_MIN(u8Hit, 10);
			VeUInt64 u64Win = GetAgent()->GetServer()->GetKenoWin()[u8Hit] * u32BetChips;
			if(u64Win) m_u64Chips += u64Win;
			m_strKenoResult = "0";
			SyncToClient();
			C_RecKenoDraw(KENO_S_OK, u8Hit, u32IssueNumber, u32BetChips, u64Win);
			return;
		}
	}
	else if(pcTemp && (!VeStrcmp(pcTemp, "buy")))
	{
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		VeUInt32 u32IssueNumber = VeAtoi(pcTemp);
		if(u32IssueNumber == GetAgent()->GetServer()->GetKenoIssue())
		{
			return;
		}
	}
	m_strKenoResult = "0";
	SyncToClient();
	C_RecKenoDraw(KENO_E_FAIL);
///	</CODE-IMPLEMENT>{Player ReqKenoDraw}
}
void TexasPokerHallPlayerS::S_ReqBaccaratUpdate()
{
///	<CODE-IMPLEMENT>{Player ReqBaccaratUpdate}
	TexasPokerHallServer* pkServer = GetAgent()->GetServer();
	C_RecBaccaratUpdate(pkServer->GetBaccarat(), pkServer->GetBaccaratPlayTime(),
		pkServer->GetBaccaratBetPlayerNum(), pkServer->GetBaccaratAllBet());
///	</CODE-IMPLEMENT>{Player ReqBaccaratUpdate}
}
void TexasPokerHallPlayerS::S_ReqBaccaratBet(const BaccaratBetType& tBetType, VeUInt32 u32Chips)
{
///	<CODE-IMPLEMENT>{Player ReqBaccaratBet}
	VeUInt32 u32BetLimit = GetBaccBetLimit();
	if(tBetType < BaccaratBetType_MAX && u32Chips <= VE_MIN(u32BetLimit, m_u64Chips))
	{
		TexasPokerHallServer* pkServer = GetAgent()->GetServer();
		VeChar8 acBuffer[128];
		VeStrcpy(acBuffer, 128, VeStringA(m_strBaccaratResult));
		VeChar8* pcContext;
		VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);
		if(pcTemp && VeStrcmp(pcTemp, "res"))
		{
			if(pcTemp && (!VeStrcmp(pcTemp, "0")))
			{
				VeZeroMemory(m_au32BaccBetChips, sizeof(m_au32BaccBetChips));
			}
			VeUInt32 u32AllBet = m_au32BaccBetChips[BACC_BANKER] + m_au32BaccBetChips[BACC_PLAYER] + m_au32BaccBetChips[BACC_TIE];
			if((u32Chips + u32AllBet) <= u32BetLimit)
			{
				m_au32BaccBetChips[tBetType] += u32Chips;
				m_u64Chips -= VeUInt64(u32Chips);
				VeSprintf(acBuffer, 128, "buy,%d,%d,%d", m_au32BaccBetChips[BACC_BANKER], m_au32BaccBetChips[BACC_PLAYER], m_au32BaccBetChips[BACC_TIE]);
				m_strBaccaratResult = acBuffer;
				pkServer->UpdateBaccPlayer(this, tBetType, u32Chips);
				SyncToClient();
				C_RecBaccaratBet(BACC_S_OK);
				return;
			}
		}
	}
	SyncToClient();
	C_RecBaccaratBet(BACC_E_FAIL);
///	</CODE-IMPLEMENT>{Player ReqBaccaratBet}
}
void TexasPokerHallPlayerS::S_ReqBaccaratBetAll(VeUInt32 u32Banker, VeUInt32 u32Player, VeUInt32 u32Tie)
{
///	<CODE-IMPLEMENT>{Player ReqBaccaratBetAll}
	TexasPokerHallServer* pkPokerHallServer = GetAgent()->GetServer();
	VeUInt32 u32BetLimit = GetBaccBetLimit();
	u32Banker = VE_MIN(u32Banker, u32BetLimit);
	u32Player = VE_MIN(u32Player, u32BetLimit);
	u32Tie = VE_MIN(u32Tie, u32BetLimit);
	VeUInt32 u32All = u32Banker + u32Player + u32Tie;
	m_i64BaccaratJackPot += VeInt64(u32All);
	VeUInt32 u32Count = 1;
	m_u32BaccaratCount = m_u32BaccaratCount + u32Count;
	pkPokerHallServer->AddBaccJackPot(VeInt64(u32All));
	if(u32All <= VE_MIN(u32BetLimit, m_u64Chips))
	{
		TexasPokerHallServer* pkServer = GetAgent()->GetServer();
		VeChar8 acBuffer[128];
		VeStrcpy(acBuffer, 128, VeStringA(m_strBaccaratResult));
		VeChar8* pcContext;
		VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);
		if(pcTemp && VeStrcmp(pcTemp, "res"))
		{
			if(pcTemp && (!VeStrcmp(pcTemp, "0")))
			{
				VeZeroMemory(m_au32BaccBetChips, sizeof(m_au32BaccBetChips));
			}
			VeUInt32 u32AllBet = m_au32BaccBetChips[BACC_BANKER] + m_au32BaccBetChips[BACC_PLAYER] + m_au32BaccBetChips[BACC_TIE];
			if((u32All + u32AllBet) <= u32BetLimit)
			{
				m_au32BaccBetChips[BACC_BANKER] += u32Banker;
				m_au32BaccBetChips[BACC_PLAYER] += u32Player;
				m_au32BaccBetChips[BACC_TIE] += u32Tie;
				m_u64Chips -= VeUInt64(u32All);
				VeSprintf(acBuffer, 128, "buy,%d,%d,%d", m_au32BaccBetChips[BACC_BANKER], m_au32BaccBetChips[BACC_PLAYER], m_au32BaccBetChips[BACC_TIE]);
				m_strBaccaratResult = acBuffer;
				pkServer->UpdateBaccPlayer(this, BACC_BANKER, u32Banker);
				pkServer->UpdateBaccPlayer(this, BACC_PLAYER, u32Player);
				pkServer->UpdateBaccPlayer(this, BACC_TIE, u32Tie);
				SyncToClient();
				C_RecBaccaratBet(BACC_S_OK);
				return;
			}
		}
	}
	SyncToClient();
	C_RecBaccaratBet(BACC_E_FAIL);
///	</CODE-IMPLEMENT>{Player ReqBaccaratBetAll}
}
void TexasPokerHallPlayerS::S_ReqBaccaratPlay()
{
///	<CODE-IMPLEMENT>{Player ReqBaccaratPlay}
	TexasPokerHallServer* pkPokerHallServer = GetAgent()->GetServer();
	VeChar8 acBuffer[128];
	VeStrcpy(acBuffer, 128, VeStringA(m_strBaccaratResult));
	VeChar8* pcContext;
	VeChar8* pcTemp = VeStrtok(acBuffer, ",", &pcContext);
	if(pcTemp && (!VeStrcmp(pcTemp, "res")))
	{
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		BaccaratBetType eWinType = (BaccaratBetType)VeAtoi(pcTemp);
		VE_ASSERT(eWinType < BaccaratBetType_MAX);
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		pcTemp = VeStrtok(NULL, ",", &pcContext);
		VeUInt32 au32Bet[BaccaratBetType_MAX];
		VeZeroMemory(au32Bet, sizeof(au32Bet));
		if(pcTemp && (!VeStrcmp(pcTemp, "buy")))
		{
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			if(pcTemp) au32Bet[BACC_BANKER] = VeAtoi(pcTemp);
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			if(pcTemp) au32Bet[BACC_PLAYER] = VeAtoi(pcTemp);
			pcTemp = VeStrtok(NULL, ",", &pcContext);
			if(pcTemp) au32Bet[BACC_TIE] = VeAtoi(pcTemp);
			VeUInt32 u32Win(0);
			if(eWinType == BACC_TIE)
			{
				u32Win = au32Bet[eWinType] * 8;
			}
			else
			{
				u32Win = au32Bet[eWinType] * 2;
			}
			m_strBaccaratResult = "0";
			if(u32Win) m_u64Chips += VeUInt64(u32Win);
			m_i64BaccaratJackPot -= VeInt64(u32Win);
			pkPokerHallServer->ReduceBaccJackPot(VeInt64(u32Win));
			SyncToClient();
			C_RecBaccaratPlay(BACC_S_OK, u32Win);
			return;
		}
	}
	C_RecBaccaratPlay(BACC_E_FAIL, 0);
///	</CODE-IMPLEMENT>{Player ReqBaccaratPlay}
}
void TexasPokerHallPlayerS::S_ReqEnterPokerTable(VeUInt32 u32Index)
{
///	<CODE-IMPLEMENT>{Player ReqEnterPokerTable}
	if(m_pkTargetTable)
	{
		C_RecEnterPokerTable(ENTER_HAS_TABLE); 
	}
	else
	{
		TexasPokerHallServer* pkServer = GetAgent()->GetServer();
		const VeVector<RoomInfo>& kRoomList = pkServer->GetRoomList();
		if(u32Index < kRoomList.Size())
		{
			const RoomInfo& kInfo = kRoomList[u32Index];
			if(kInfo.m_tType == ROOM_TYPE_PRACTICE || m_u64Chips >= kInfo.m_u32ChipsMin)
			{
				if((!VE_MASK_HAS_ALL(kInfo.m_u8Flags, ROOM_FLAG_VIP)) || (m_u32Vip > 0))
				{
					pkServer->IntoGameRoom(u32Index, this, NULL);
					C_RecEnterPokerTable(ENTER_S_OK);
				}
				else
				{
					C_RecEnterPokerTable(ENTER_NOT_VIP);
				}
			}
			else
			{
				C_RecEnterPokerTable(ENTER_NO_CHIPS);
			}
		}
		else
		{
			C_RecEnterPokerTable(ENTER_E_FAIL);
		}
	}
///	</CODE-IMPLEMENT>{Player ReqEnterPokerTable}
}
void TexasPokerHallPlayerS::S_ReqLeavePokerTable()
{
///	<CODE-IMPLEMENT>{Player ReqLeavePokerTable}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->RemovePlayer(this);
	}
///	</CODE-IMPLEMENT>{Player ReqLeavePokerTable}
}
void TexasPokerHallPlayerS::S_ReqChangeSeat(VeUInt32 u32Index)
{
///	<CODE-IMPLEMENT>{Player ReqChangeSeat}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->ChangeSeat(this, u32Index);
	}
///	</CODE-IMPLEMENT>{Player ReqChangeSeat}
}
void TexasPokerHallPlayerS::S_ReqAddChips()
{
///	<CODE-IMPLEMENT>{Player ReqAddChips}

///	</CODE-IMPLEMENT>{Player ReqAddChips}
}
void TexasPokerHallPlayerS::S_ReqChangeRoom()
{
///	<CODE-IMPLEMENT>{Player ReqChangeRoom}
	if(m_pkTargetTable)
	{
		TexasPokerTable* pkTable = m_pkTargetTable;
		VeUInt32 u32Index = pkTable->GetIndex();
		m_pkTargetTable->RemovePlayer(this);
		GetAgent()->GetServer()->IntoGameRoom(u32Index, this, pkTable);
		C_OnTableUpdateNoMotion();
	}
///	</CODE-IMPLEMENT>{Player ReqChangeRoom}
}
void TexasPokerHallPlayerS::S_ReqReady(VeUInt32 u32Chips)
{
///	<CODE-IMPLEMENT>{Player ReqReady}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->Ready(this, u32Chips);
	}
///	</CODE-IMPLEMENT>{Player ReqReady}
}
void TexasPokerHallPlayerS::S_ReqFold()
{
///	<CODE-IMPLEMENT>{Player ReqFold}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->Fold(this);
	}
///	</CODE-IMPLEMENT>{Player ReqFold}
}
void TexasPokerHallPlayerS::S_ReqCall()
{
///	<CODE-IMPLEMENT>{Player ReqCall}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->Call(this);
	}
///	</CODE-IMPLEMENT>{Player ReqCall}
}
void TexasPokerHallPlayerS::S_ReqFill(VeUInt32 u32Chips)
{
///	<CODE-IMPLEMENT>{Player ReqFill}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->Raise(this, u32Chips);
	}
///	</CODE-IMPLEMENT>{Player ReqFill}
}
void TexasPokerHallPlayerS::S_ReqBuy(VeUInt8 u8Type, VeUInt8 u8Index)
{
///	<CODE-IMPLEMENT>{Player ReqBuy}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->Buy(this, u8Type, u8Index);
	}
///	</CODE-IMPLEMENT>{Player ReqBuy}
}
void TexasPokerHallPlayerS::S_ReqBuyAll(VeUInt8 u8Type)
{
///	<CODE-IMPLEMENT>{Player ReqBuyAll}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->BuyAll(this, u8Type);
	}
///	</CODE-IMPLEMENT>{Player ReqBuyAll}
}
void TexasPokerHallPlayerS::S_ReqChat(VeUInt8 u8Type, VeUInt8 u8Index, const VeChar8* strChat)
{
///	<CODE-IMPLEMENT>{Player ReqChat}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->Chat(this, u8Type, u8Index, strChat);
	}
///	</CODE-IMPLEMENT>{Player ReqChat}
}
void TexasPokerHallPlayerS::S_ReqChatAll(VeUInt8 u8Type, const VeChar8* strChat)
{
///	<CODE-IMPLEMENT>{Player ReqChatAll}
	if(m_pkTargetTable)
	{
		m_pkTargetTable->ChatAll(this, u8Type, strChat);
	}
///	</CODE-IMPLEMENT>{Player ReqChatAll}
}
void TexasPokerHallPlayerS::S_ReqRefreshPurchase()
{
///	<CODE-IMPLEMENT>{Player ReqRefreshPurchase}
	C_RecRefreshPurchase(OnRefreshPurchase() ? PURCHASE_S_OK : PURCHASE_E_FAIL);
///	</CODE-IMPLEMENT>{Player ReqRefreshPurchase}
}
void TexasPokerHallPlayerS::S_ReqFinishEvent(const EventInfo& tInfo)
{
///	<CODE-IMPLEMENT>{Player ReqFinishEvent}
	switch(tInfo)
	{
	case EVENT_MODIFY:
		FinishBindingPhone();
		break;
	case EVENT_PURCHASE:
		FinishPurchase();
		break;
	case EVENT_VIP:
		FinishVip();
		break;
	case EVENT_PRESENT:
		FinishPresent();
		break;
	case EVENT_ONLINE:
		FinishOnline();
		break;
	case EVENT_PLAY_COUNT:
		FinishPokerCount();
		break;
	case EVENT_PLAY_SLOT_COUNT:
		FinishSlotCount();
		break;
	case EVENT_PLAY_KENO_COUNT:
		FinishKenoCount();
		break;
	case EVENT_PLAY_BACC_COUNT:
		FinishBaccCount();
		break;
	default:
		break;
	}
///	</CODE-IMPLEMENT>{Player ReqFinishEvent}
}
///	</CODE-GENERATE>{EntityServerFuncImpl Player}
//--------------------------------------------------------------------------
bool TexasPokerHallPlayerS::NeedName()
{
	bool bRes = m_bNeedName;
	m_bNeedName = false;
	return bRes;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerS::GetSingleBet()
{
	return VeUInt32(10 * VE_MIN(200, VE_MAX(1, (VeUInt64(m_u64Chips) / 10000))));
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::GetSlotRunPro(VeUInt32 u32WinChips,
	SlotResult& kWinResultOut)
{
	VeUInt32 u32Count = 1;
	m_u32SlotCount = m_u32SlotCount + u32Count;

	TexasPokerHallServer* pkPokerHallServer = GetAgent()->GetServer();

	VeUInt32 u32_1TimesTag = u32WinChips * 1;
	VeUInt32 u32_8TimesTag = u32WinChips * 8;
	VeUInt32 u32_30TimesTag = u32WinChips * 30;

	VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
	VeUInt32 u32RandNum = VeRand()%(8);
	VeUInt32 u8RanNum0 = VeRand()%(18);
	VeUInt32 u8RanNum1 = VeRand()%(10);
	VeUInt32 u8RanNum2 = VeRand()%(10);
	VeUInt32 u8RanNum3 = VeRand()%(150000);
	VeUInt32 u8RanNum4 =  VeRand()%(1250);
	VeInt64 m_64GlobalJackPot = pkPokerHallServer->GetSlotJackPot();

	if (u8RanNum3 == 88888
		&& u32WinChips >= 50
		&& u32WinChips <= 1000
		&& m_64GlobalJackPot >= u32WinChips * JP_LEVEL * 5
		&& pkPokerHallServer->GetJPChips() >= u32WinChips * JP_LEVEL * 2)
	{
		kWinResultOut.m_au8Result[0] = 0;
		kWinResultOut.m_au8Result[1] = 0;
		kWinResultOut.m_au8Result[2] = 0;
		pkPokerHallServer->DelJPChips(u32WinChips);
		kWinResultOut.m_u64Win = VeUInt64(u32WinChips * JP_LEVEL);
		pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
		m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
	}
	else
	{
		if (m_i64SlotJackPot >= u32_1TimesTag
			&& m_i64SlotJackPot <= u32_8TimesTag)
		{
			if (u32RandNum == 1 || u32RandNum == 2 || u32RandNum == 3)
			{
				if (u8RanNum0 == 1)
				{
					kWinResultOut.m_au8Result[0] = 10;
					kWinResultOut.m_au8Result[1] = 10;
					kWinResultOut.m_au8Result[2] = VeRand()%(10);
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL2);
				}
				else if (u8RanNum0 == 2)
				{
					kWinResultOut.m_au8Result[0] = 10;
					kWinResultOut.m_au8Result[1] = 10;
					kWinResultOut.m_au8Result[2] = 10;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL3);
				}
				else if (u8RanNum0 == 3)
				{
					kWinResultOut.m_au8Result[0] = 7 + VeRand()%(3);
					kWinResultOut.m_au8Result[1] = 9;
					kWinResultOut.m_au8Result[2] = 8 + VeRand()%(2);
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL4);
				}
				else if (u8RanNum0 == 4)
				{
					kWinResultOut.m_au8Result[0] = 9;
					kWinResultOut.m_au8Result[1] = 6;
					kWinResultOut.m_au8Result[2] = 9;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL5);
				}
				else if (u8RanNum0 == 5)
				{
					kWinResultOut.m_au8Result[0] = 9;
					kWinResultOut.m_au8Result[1] = 5;
					kWinResultOut.m_au8Result[2] = 9;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL6);
				}
				else if (u8RanNum0 == 6)
				{
					kWinResultOut.m_au8Result[0] = 9;
					kWinResultOut.m_au8Result[1] = 4;
					kWinResultOut.m_au8Result[2] = 9;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL7);
				}
				else if (u8RanNum0 == 7)
				{
					kWinResultOut.m_au8Result[0] = 9;
					kWinResultOut.m_au8Result[1] = 3;
					kWinResultOut.m_au8Result[2] = 9;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL8);
				}
				else if (u8RanNum0 == 8)
				{
					kWinResultOut.m_au8Result[0] = 9;
					kWinResultOut.m_au8Result[1] = 9;
					kWinResultOut.m_au8Result[2] = 9;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL9);
				}
				else if (u8RanNum0 == 9)
				{
					kWinResultOut.m_au8Result[0] = 8;
					kWinResultOut.m_au8Result[1] = 6;
					kWinResultOut.m_au8Result[2] = 8;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL10);
				}
				else if (u8RanNum0 == 10)
				{
					kWinResultOut.m_au8Result[0] = 8;
					kWinResultOut.m_au8Result[1] = 5;
					kWinResultOut.m_au8Result[2] = 8;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL11);
				}
				else if (u8RanNum0 == 11)
				{
					kWinResultOut.m_au8Result[0] = 8;
					kWinResultOut.m_au8Result[1] = 4;
					kWinResultOut.m_au8Result[2] = 8;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL12);
				}
				else if (u8RanNum0 == 12)
				{
					kWinResultOut.m_au8Result[0] = 8;
					kWinResultOut.m_au8Result[1] = 3;
					kWinResultOut.m_au8Result[2] = 8;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL13);
				}
				else
				{
					kWinResultOut.m_au8Result[0] = 10;
					kWinResultOut.m_au8Result[1] = VeRand()%(10);
					kWinResultOut.m_au8Result[2] = VeRand()%(10);
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL1);
				}
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else
			{
				GetFailSlotRun(kWinResultOut);
			}
		}
		else if (m_i64SlotJackPot > u32_8TimesTag
			&& m_i64SlotJackPot <= u32_30TimesTag)
		{
			if (u32RandNum == 1 || u32RandNum == 2 || u32RandNum == 3)
			{
				if (u8RanNum1 == 0)
				{
					kWinResultOut.m_au8Result[0] = 8;
					kWinResultOut.m_au8Result[1] = 8;
					kWinResultOut.m_au8Result[2] = 8;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL14);
				}
				else if (u8RanNum1 == 1)
				{
					kWinResultOut.m_au8Result[0] = 7;
					kWinResultOut.m_au8Result[1] = 6;
					kWinResultOut.m_au8Result[2] = 7;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL15);
				}
				else if (u8RanNum1 == 2)
				{
					kWinResultOut.m_au8Result[0] = 7;
					kWinResultOut.m_au8Result[1] = 5;
					kWinResultOut.m_au8Result[2] = 7;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL16);
				}
				else if (u8RanNum1 == 3)
				{
					kWinResultOut.m_au8Result[0] = 7;
					kWinResultOut.m_au8Result[1] = 4;
					kWinResultOut.m_au8Result[2] = 7;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL17);
				}
				else if (u8RanNum1 == 4)
				{
					kWinResultOut.m_au8Result[0] = 7;
					kWinResultOut.m_au8Result[1] = 3;
					kWinResultOut.m_au8Result[2] = 7;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL18);
				}
				else if (u8RanNum1 == 5)
				{
					kWinResultOut.m_au8Result[0] = 7;
					kWinResultOut.m_au8Result[1] = 7;
					kWinResultOut.m_au8Result[2] = 7;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL19);
				}
				else
				{
					kWinResultOut.m_au8Result[0] = 10;
					kWinResultOut.m_au8Result[1] = 10;
					kWinResultOut.m_au8Result[2] = VeRand()%(10);
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL2);
				}
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else
			{
				GetFailSlotRun(kWinResultOut);
			}
		}
		else if (m_i64SlotJackPot > u32_30TimesTag)
		{
			if (u32RandNum == 1 || u32RandNum == 2 || u32RandNum == 3)
			{
				if (u8RanNum2 == 0)
				{
					kWinResultOut.m_au8Result[0] = 6;
					kWinResultOut.m_au8Result[1] = 6;
					kWinResultOut.m_au8Result[2] = 6;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL20);
				}
				else if (u8RanNum2 == 1)
				{
					kWinResultOut.m_au8Result[0] = 5;
					kWinResultOut.m_au8Result[1] = 5;
					kWinResultOut.m_au8Result[2] = 5;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL21);
				}
				else if (u8RanNum2 == 2)
				{
					kWinResultOut.m_au8Result[0] = 4;
					kWinResultOut.m_au8Result[1] = 4;
					kWinResultOut.m_au8Result[2] = 4;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL22);
				}
				else if (u8RanNum2 == 3)
				{
					kWinResultOut.m_au8Result[0] = 3;
					kWinResultOut.m_au8Result[1] = 3;
					kWinResultOut.m_au8Result[2] = 3;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL23);
				}
				else if (u8RanNum2 == 4)
				{
					kWinResultOut.m_au8Result[0] = 2;
					kWinResultOut.m_au8Result[1] = 2;
					kWinResultOut.m_au8Result[2] = 2;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL24);
				}
				else if (u8RanNum2 == 5)
				{
					kWinResultOut.m_au8Result[0] = 1;
					kWinResultOut.m_au8Result[1] = 1;
					kWinResultOut.m_au8Result[2] = 1;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL25);
				}
				else
				{
					kWinResultOut.m_au8Result[0] = VeRand()%(10);
					kWinResultOut.m_au8Result[1] = 10;
					kWinResultOut.m_au8Result[2] = 10;
					kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL2);
				}
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else
			{
				GetFailSlotRun(kWinResultOut);
			}
		}
		else
		{
			if (u8RanNum4 > 200 && u8RanNum4 <= 240)
			{
				kWinResultOut.m_au8Result[0] = 10;
				kWinResultOut.m_au8Result[1] = VeRand()%(10);
				kWinResultOut.m_au8Result[2] = VeRand()%(10);
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL1);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 > 240 && u8RanNum4 <= 270)
			{
				kWinResultOut.m_au8Result[0] = 10;
				kWinResultOut.m_au8Result[1] = 10;
				kWinResultOut.m_au8Result[2] = VeRand()%(10);
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL2);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 > 270 && u8RanNum4 <= 300)
			{
				kWinResultOut.m_au8Result[0] = 10;
				kWinResultOut.m_au8Result[1] = 10;
				kWinResultOut.m_au8Result[2] = 10;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL3);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 51 || u8RanNum4 == 52 || u8RanNum4 == 101 
				|| u8RanNum4 == 501 || u8RanNum4 == 502 || u8RanNum4 == 503)
			{
				kWinResultOut.m_au8Result[0] = 7 + VeRand()%(3);
				kWinResultOut.m_au8Result[1] = 9;
				kWinResultOut.m_au8Result[2] = 8 + VeRand()%(2);
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL4);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 53 || u8RanNum4 == 54 || u8RanNum4 == 102 
				|| u8RanNum4 == 504 || u8RanNum4 == 505 || u8RanNum4 == 506)
			{
				kWinResultOut.m_au8Result[0] = 9;
				kWinResultOut.m_au8Result[1] = 6;
				kWinResultOut.m_au8Result[2] = 9;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL5);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 55 || u8RanNum4 == 56 || u8RanNum4 == 103
				|| u8RanNum4 == 507 || u8RanNum4 == 508 || u8RanNum4 == 509)
			{
				kWinResultOut.m_au8Result[0] = 9;
				kWinResultOut.m_au8Result[1] = 5;
				kWinResultOut.m_au8Result[2] = 9;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL6);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 57 || u8RanNum4 == 58 || u8RanNum4 == 104
				|| u8RanNum4 == 510 || u8RanNum4 == 511 || u8RanNum4 == 512)
			{
				kWinResultOut.m_au8Result[0] = 9;
				kWinResultOut.m_au8Result[1] = 4;
				kWinResultOut.m_au8Result[2] = 9;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL7);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 59 || u8RanNum4 == 60 || u8RanNum4 == 105
				|| u8RanNum4 == 513 || u8RanNum4 == 514 || u8RanNum4 == 515)
			{
				kWinResultOut.m_au8Result[0] = 9;
				kWinResultOut.m_au8Result[1] = 3;
				kWinResultOut.m_au8Result[2] = 9;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL8);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 61 || u8RanNum4 == 62 || u8RanNum4 == 106
				|| u8RanNum4 == 516 || u8RanNum4 == 517 || u8RanNum4 == 518)
			{
				kWinResultOut.m_au8Result[0] = 9;
				kWinResultOut.m_au8Result[1] = 9;
				kWinResultOut.m_au8Result[2] = 9;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL9);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 63 || u8RanNum4 == 64 || u8RanNum4 == 107
				|| u8RanNum4 == 519 || u8RanNum4 == 520 || u8RanNum4 == 521)
			{
				kWinResultOut.m_au8Result[0] = 8;
				kWinResultOut.m_au8Result[1] = 6;
				kWinResultOut.m_au8Result[2] = 8;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL10);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 65 || u8RanNum4 == 66 || u8RanNum4 == 108
				|| u8RanNum4 == 522 || u8RanNum4 == 523 || u8RanNum4 == 524)
			{
				kWinResultOut.m_au8Result[0] = 8;
				kWinResultOut.m_au8Result[1] = 5;
				kWinResultOut.m_au8Result[2] = 8;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL11);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 67 || u8RanNum4 == 68
				|| u8RanNum4 == 525 || u8RanNum4 == 526)
			{
				kWinResultOut.m_au8Result[0] = 8;
				kWinResultOut.m_au8Result[1] = 4;
				kWinResultOut.m_au8Result[2] = 8;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL12);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 69 || u8RanNum4 == 70
				|| u8RanNum4 == 527 || u8RanNum4 == 528)
			{
				kWinResultOut.m_au8Result[0] = 8;
				kWinResultOut.m_au8Result[1] = 3;
				kWinResultOut.m_au8Result[2] = 8;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL13);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 71 || u8RanNum4 == 72
				|| u8RanNum4 == 529 || u8RanNum4 == 530)
			{
				kWinResultOut.m_au8Result[0] = 8;
				kWinResultOut.m_au8Result[1] = 8;
				kWinResultOut.m_au8Result[2] = 8;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL14);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 73 || u8RanNum4 == 74
				|| u8RanNum4 == 531 || u8RanNum4 == 532)
			{
				kWinResultOut.m_au8Result[0] = 7;
				kWinResultOut.m_au8Result[1] = 6;
				kWinResultOut.m_au8Result[2] = 7;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL15);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 75 || u8RanNum4 == 76)
			{
				kWinResultOut.m_au8Result[0] = 7;
				kWinResultOut.m_au8Result[1] = 5;
				kWinResultOut.m_au8Result[2] = 7;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL16);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 77)
			{
				kWinResultOut.m_au8Result[0] = 7;
				kWinResultOut.m_au8Result[1] = 4;
				kWinResultOut.m_au8Result[2] = 7;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL17);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 79)
			{
				kWinResultOut.m_au8Result[0] = 7;
				kWinResultOut.m_au8Result[1] = 3;
				kWinResultOut.m_au8Result[2] = 7;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL18);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 81)
			{
				kWinResultOut.m_au8Result[0] = 7;
				kWinResultOut.m_au8Result[1] = 7;
				kWinResultOut.m_au8Result[2] = 7;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL19);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 82)
			{
				kWinResultOut.m_au8Result[0] = 6;
				kWinResultOut.m_au8Result[1] = 6;
				kWinResultOut.m_au8Result[2] = 6;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL20);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 83)
			{
				kWinResultOut.m_au8Result[0] = 5;
				kWinResultOut.m_au8Result[1] = 5;
				kWinResultOut.m_au8Result[2] = 5;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL21);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 84)
			{
				kWinResultOut.m_au8Result[0] = 4;
				kWinResultOut.m_au8Result[1] = 4;
				kWinResultOut.m_au8Result[2] = 4;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL22);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else if (u8RanNum4 == 85)
			{
				kWinResultOut.m_au8Result[0] = 3;
				kWinResultOut.m_au8Result[1] = 3;
				kWinResultOut.m_au8Result[2] = 3;
				kWinResultOut.m_u64Win = VeUInt64(u32WinChips * PRIZE_LEVEL23);
				pkPokerHallServer->DelJPChips(kWinResultOut.m_u64Win);
				pkPokerHallServer->ReduceSlotJackPot(kWinResultOut.m_u64Win);
				m_i64SlotJackPot -= VeInt64(kWinResultOut.m_u64Win);
			}
			else
			{
				GetFailSlotRun(kWinResultOut);
			}
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::GetFailSlotRun(SlotResult& kResultOut)
{
	VeUInt32 u8RanNum0 = VeRand()%(10);
	VeUInt32 u8RanNum1 = VeRand()%(10);
	VeUInt32 u32RandNum = VeRand()%(5);
	if (u8RanNum0 == 7 || u8RanNum0 == 8 || u8RanNum0 == 9)
	{
		u8RanNum0 = 5;
	}
	if (u8RanNum1 == 7 || u8RanNum1 == 8 || u8RanNum1 == 9)
	{
		u8RanNum1 = 4;
	}

	if (u32RandNum == 4)
	{
		kResultOut.m_au8Result[0] = u8RanNum0;
		kResultOut.m_au8Result[1] = u8RanNum0;
		kResultOut.m_au8Result[2] = GetRandNumExceptParameter(kResultOut.m_au8Result[0]);
		kResultOut.m_u64Win = 0;
	}
	else if (u32RandNum == 0 || u32RandNum == 1)
	{
		kResultOut.m_au8Result[0] = u8RanNum0;
		kResultOut.m_au8Result[1] = u8RanNum1;
		kResultOut.m_au8Result[2] = GetRandNumExceptParameter(kResultOut.m_au8Result[0]);
		kResultOut.m_u64Win = 0;
	}
	else
	{
		kResultOut.m_au8Result[1] = u8RanNum0;
		kResultOut.m_au8Result[2] = u8RanNum1;
		kResultOut.m_au8Result[0] = GetRandNumExceptParameter(kResultOut.m_au8Result[2]);;
		kResultOut.m_u64Win = 0;
	}
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerS::GetRandNumExceptParameter(VeUInt32 m_u32ExceptNum)
{
	VeUInt32 m_f32RandNum = VeRand()%(10);
	if (m_f32RandNum == m_u32ExceptNum)
	{
		switch(m_u32ExceptNum)
		{
		case 0:
			return 3;
		case 1:
			return 5;
		case 2:
			return 4;
		case 3:
			return 5;
		case 4:
			return 0;
		case 5:
			return 1;
		case 6:
			return 2;
		case 7:
			return 6;
		case 8:
			return 3;
		case 9:
			return 6;
		default:
			return 2;
		}
	}
	else
	{
		return m_f32RandNum;
	}
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerS::GetBaccBetLimit()
{
	return GetSingleBet() * 30;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerS::GetBaccBetChips(BaccaratBetType eType)
{
	if(eType < BaccaratBetType_MAX)
		return m_au32BaccBetChips[eType];
	else
		return 0;
}
//--------------------------------------------------------------------------
VeRefNode<TexasPokerHallPlayerS*>& TexasPokerHallPlayerS::GetBaccNode()
{
	return m_kBaccNode;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::AddKenoJackPot(VeInt64 i64AddNum)
{
	m_i64KenoJackPot += i64AddNum;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::ReduceKenoJackPot(VeInt64 i64ReduceNum)
{
	m_i64KenoJackPot -= i64ReduceNum;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::RecordKenoCount()
{
	VeUInt32 u32Count = 1;
	m_u32KenoCount = m_u32KenoCount + u32Count;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::UpdateRoomInfo()
{
	VE_ASSERT(m_pkTargetTable);
	C_UpdateRoomInfo(m_pkTargetTable->GetInfo());
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::UpdateTable()
{
	VE_ASSERT(m_pkTargetTable);
	C_UpdateTable(m_pkTargetTable->GetState());
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::UpdateTableSeats()
{
	VE_ASSERT(m_pkTargetTable);
	VE_ASSERT(m_u32IndexOnTable < TEXAS_POKER_SEAT_NUM);
	VeVector<TablePlayer> kPlayers;
	for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
	{
		const TexasPokerTable::Seat& kSeat = m_pkTargetTable->GetSeat(i);
		if(kSeat.m_eState)
		{
			kPlayers.PushBack(kSeat.m_kPlayerInfo);
		}
	}
	C_UpdateTableSeats(VeUInt8(m_u32IndexOnTable), kPlayers);
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::UpdateHandCards()
{
	VE_ASSERT(m_pkTargetTable);
	const TexasPokerTable::Seat& kSeat = m_pkTargetTable->GetSeat(m_u32IndexOnTable);
	if(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY)
	{
		C_UpdateHandCards(kSeat.m_kHandCard);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnWin(VeUInt32 u32Bet)
{
	if(u32Bet > m_u32WinMaxBet)
	{
		m_u32WinMaxBet = u32Bet;
	}
	m_u32WinCount += VeUInt32(1);
	VeUInt32 u32WinCount = m_u32WinCount;
	if(u32WinCount < 5)
	{
		m_u32Title = VeUInt32(0);
	}
	else if(u32WinCount < 10)
	{
		m_u32Title = VeUInt32(1);
	}
	else if(u32WinCount < 20)
	{
		m_u32Title = VeUInt32(2);
	}
	else if(u32WinCount < 40)
	{
		m_u32Title = VeUInt32(3);
	}
	else if(u32WinCount < 80)
	{
		m_u32Title = VeUInt32(4);
	}
	else if(u32WinCount < 160)
	{
		m_u32Title = VeUInt32(5);
	}
	else if(u32WinCount < 320)
	{
		m_u32Title = VeUInt32(6);
	}
	else if(u32WinCount < 640)
	{
		m_u32Title = VeUInt32(7);
	}
	else if(u32WinCount < 1280)
	{
		m_u32Title = VeUInt32(8);
	}
	else if(u32WinCount < 2560)
	{
		m_u32Title = VeUInt32(9);
	}
	SyncToClient();
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerS::OnRefreshPurchase()
{
	VeUInt32 u32Res(0);
	TexasPokerHallServer* pkServer = GetAgent()->GetServer();
	VeVector< VePair<VeString, VeInt32> > kPurchase = pkServer->GetDBBusiness()->checkPurchase(m_strPlayerUUID.GetString());
	for(VeUInt32 i(0); i < kPurchase.Size(); ++i)
	{
		if(!VeStrncmp(kPurchase[i].m_tFirst, "YOUMI", 5))
		{
			VeUInt32 u32AddChips = kPurchase[i].m_tSecond;
			m_u64Purchase += VeUInt64(u32AddChips / 200);
			m_u64PurchaseChips += VeUInt64(u32AddChips);
			m_u32PurchaseCount += VeUInt32(1);
			m_u64Chips += VeUInt64(u32AddChips);
			C_OnPurchaseCompleted("YOUMI", u32AddChips);
			++u32Res;
		}
		else
		{
			VePair<VeUInt32,VeUInt32> kRes = pkServer->GetPurchaseInfo(kPurchase[i].m_tFirst, kPurchase[i].m_tSecond);
			if(kRes.m_tFirst && kRes.m_tSecond)
			{
				m_u64Purchase += VeUInt64(kRes.m_tFirst);
				m_u64PurchaseChips += VeUInt64(kRes.m_tSecond);
				m_u32PurchaseCount += VeUInt32(1);
				m_u64Chips += VeUInt64(kRes.m_tSecond);
				if(m_u32Vip == 0 && kRes.m_tFirst >= pkServer->GetVipPurchase().m_tFirst)
				{
					m_u32EventVip = 1;
				}
				C_OnPurchaseCompleted(kPurchase[i].m_tFirst, kRes.m_tSecond);
				++u32Res;
			}
		}
	}
	if(u32Res && m_u32Vip == 0 && m_u64Purchase >= pkServer->GetVipPurchase().m_tSecond)
	{
		m_u32EventVip = 1;
	}
	SyncToClient();
	return u32Res;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::OnEventUpdate()
{
	TexasPokerHallServer* pkServer = GetAgent()->GetServer();
	if(m_u32EventTickCurrent != pkServer->GetEventTick())
	{
		m_u32EventTickCurrent = pkServer->GetEventTick();
		m_u32EventTickOnline = 0;
		m_u32EventTickPlayCount = 0;
		m_u32EventTickPlaySlotCount = 0;
		m_u32EventTickPlayKenoCount = 0;
		m_u32EventTickPlayBaccCount = 0;
		m_u32TickPlayCount = 0;
		m_u32TickPlaySlotCount = 0;
		m_u32TickPlayKenoCount = 0;
		m_u32TickPlayBaccCount = 0;
		SyncToClient();
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishBindingPhone()
{
	if(m_u32EventModifyInfo == 0 && m_strPhone != "")
	{
		m_u32EventModifyInfo += VeUInt32(1);
		m_u64Chips += VeUInt64(2000);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_CHIPS, 2000);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishPurchase()
{
	TexasPokerHallServer* pkServer = GetAgent()->GetServer();
	if(m_u32EventPurchase < 5)
	{
		VeUInt32 u32PurchaseChips = pkServer->GetBonusPurchaseChips(m_u32EventPurchase + VeUInt32(1));
		if(m_u64PurchaseChips >= u32PurchaseChips)
		{
			m_u32EventPurchase += VeUInt32(1);
			m_u32BankCard = m_u32EventPurchase;
			SyncToClient();
			C_RecFinishEvent(EVENT_AWARD_CARD, m_u32BankCard);
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishVip()
{
	if(m_u32Vip == 0 && m_u32EventVip)
	{
		m_u32Vip = 1;
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_VIP, 0);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishPresent()
{
	if(m_u32PresentSend >= 100000)
	{
		m_u32PresentSend -= VeUInt32(100000);
		m_u64GoldBricks += VeUInt64(1);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_GOLD, 1);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishOnline()
{
	if(m_u32EventTickOnline == 0)
	{
		m_u32EventTickOnline = 1;
		VeUInt32 u32Award = m_u32Vip ? 4000 : 2000;
		m_u64Chips += VeUInt64(u32Award);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_CHIPS, u32Award);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishPokerCount()
{
	if(m_u32EventTickPlayCount == 0 && m_u32TickPlayCount >= 10)
	{
		m_u32EventTickPlayCount = 1;
		VeUInt32 u32Award = m_u32Vip ? 4000 : 2000;
		m_u64Chips += VeUInt64(u32Award);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_CHIPS, u32Award);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishSlotCount()
{
	if(m_u32EventTickPlaySlotCount == 0 && m_u32TickPlaySlotCount >= 30)
	{
		m_u32EventTickPlaySlotCount = 1;
		VeUInt32 u32Award = m_u32Vip ? 4000 : 2000;
		m_u64Chips += VeUInt64(u32Award);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_CHIPS, u32Award);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishKenoCount()
{
	if(m_u32EventTickPlayKenoCount == 0 && m_u32TickPlayKenoCount >= 3)
	{
		m_u32EventTickPlayKenoCount = 1;
		VeUInt32 u32Award = m_u32Vip ? 4000 : 2000;
		m_u64Chips += VeUInt64(u32Award);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_CHIPS, u32Award);
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerS::FinishBaccCount()
{
	if(m_u32EventTickPlayBaccCount == 0 && m_u32TickPlayBaccCount >= 10)
	{
		m_u32EventTickPlayBaccCount = 1;
		VeUInt32 u32Award = m_u32Vip ? 4000 : 2000;
		m_u64Chips += VeUInt64(u32Award);
		SyncToClient();
		C_RecFinishEvent(EVENT_AWARD_CHIPS, u32Award);
	}
}
//--------------------------------------------------------------------------
