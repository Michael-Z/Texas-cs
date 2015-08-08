#pragma once

#include "TexasPokerHallServer.h"

#define TEXAS_POKER_SEAT_NUM (9)
#define TEXAS_POKER_AI_MAX (3)
#define TEXAS_POKER_EMPTY_MAX (3)

class TexasPokerHallPlayerS;

class TexasPokerTable : public VeRefNode<TexasPokerTable*>
{
public:
	typedef VeMemberDelegate<TexasPokerTable, VeTimeEvent&> TimeDelegate;
	enum SeatState
	{
		SEAT_EMPTY,
		SEAT_PLAYER,
		SEAT_ROBOT,
		SEAT_AI,
		SEAT_AI_FOLD,
		SEAT_MAX
	};

	struct Seat
	{
		VeUInt32 m_u32Index;
		SeatState m_eState;
		TablePlayer m_kPlayerInfo;
		VeUInt32 m_u32BetCurrent;
		VeVector<CardData> m_kHandCard;
		TexasPokerHallPlayerS* m_pkTouchedPlayer;
	};

	TexasPokerTable(VeUInt32 u32Index, const RoomInfo& kInfo, TexasPokerHallServer* pkServer);

	virtual ~TexasPokerTable();

	VeUInt32 GetEmptySeat();

	const RoomInfo& GetInfo();

	void AddPlayer(TexasPokerHallPlayerS* pkPlayer);

	void RemovePlayer(TexasPokerHallPlayerS* pkPlayer);

	void ChangeSeat(TexasPokerHallPlayerS* pkPlayer, VeUInt32 u32Target);

	void AddChips(TexasPokerHallPlayerS* pkPlayer);

	void Ready(TexasPokerHallPlayerS* pkPlayer, VeUInt32 u32Chips);

	void Fold(TexasPokerHallPlayerS* pkPlayer);

	void Call(TexasPokerHallPlayerS* pkPlayer);

	void Raise(TexasPokerHallPlayerS* pkPlayer, VeUInt32 u32Chips);

	void Buy(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type, VeUInt8 u8Index);

	void BuyAll(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type);

	void Chat(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type, VeUInt8 u8Index, const VeChar8* strChat);

	void ChatAll(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type, const VeChar8* strChat);

	const Seat& GetSeat(VeUInt32 u32Index);

	const Table& GetState();

	VeUInt32 GetIndex();

protected:
	void InitSeat();

	void ClearSeat(Seat& kSeat);

	void BlindBet();

	void DealHand();

	void CheatAIHand();

	void DealThree();

	void DealFourth();

	void DealFifth();

	void ProcessedAICards();

	VePair<CardNum, CardSuit> RandomCard();

	bool HasCard(CardNum tNumber, CardSuit tSuit);

	void FoldCurrent();

	void CallCurrent();

	void RaiseCurrent(VeUInt32 u32Chips);

	void OnProcess();

	void OnWaitProcess();

	void OnSync();

	void OnCompare();

	void TableChipsToPool();

	void _RemovePlayer(TexasPokerHallPlayerS* pkPlayer);

	void DeatchTimers();

	void Output();

	void UpdateAIPlayers();

	const VeUInt32 m_u32Index;
	const RoomInfo& m_kRoomInfo;
	TexasPokerHallServer* m_pkServer;
	Table m_kTableState;
	VeUInt32 m_u32EmptySeat;
	Seat m_akSeatArray[TEXAS_POKER_SEAT_NUM];
	VeRefList<TexasPokerHallPlayerS*> m_kPlayerList;
	TIMER_DECL(TexasPokerTable, OnStart);
	TIMER_DECL(TexasPokerTable, OnRun);
	TIMER_DECL(TexasPokerTable, OnAIProcess);
	bool m_bFirstBlood;

	VeVector<VeUInt8> m_kAIPlayerIndex;

	VePair<CardNum, CardSuit> kAIWinRes[2];
	VePair<CardNum, CardSuit> kAILosRes[2];
	VePair<CardNum, CardSuit> kAIRandomRes[2];
	VeUInt32 u32RateAIConduct;
	VeUInt32 u32CheatTag;
};
