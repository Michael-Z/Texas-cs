#include "TexasPokerTable.h"
#include "TexasPokerHallPlayerS.h"
#include "TexasPokerHallAgentS.h"
#include <map>

//--------------------------------------------------------------------------
VeUInt32 GetNumberPower(CardNum eNum)
{
	VE_ASSERT(eNum < CardNum_MAX);
	return (eNum + CardNum_MAX - 1) % CardNum_MAX;
}
//--------------------------------------------------------------------------
VeUInt32 GetCardPower(const CardData& kCard)
{
	return GetNumberPower(kCard.m_tNumber);
}
//--------------------------------------------------------------------------
void SortCard(const VeVector<CardData>& kIn, VeVector<CardData>& kOut)
{
	std::multimap<VeUInt32,CardData> kCardSort;
	for(VeUInt32 i(0);i < kIn.Size();++i)
	{
		VeUInt32 u32Key = CardNum_MAX - GetCardPower(kIn[i]);
		kCardSort.insert(std::make_pair(u32Key, kIn[i]));
	}
	kOut.Reserve(kIn.Size());
	kOut.Clear();
	for(std::multimap<VeUInt32,CardData>::iterator it = kCardSort.begin(); it != kCardSort.end(); ++it)
	{
		kOut.PushBack(it->second);
	}
}
//--------------------------------------------------------------------------
VeUInt32 GetCategoriesMask(Categories eCategories)
{
	VE_ASSERT(eCategories < Categories_MAX);
	return (Categories_MAX - eCategories) << 20;
}
//--------------------------------------------------------------------------
void SplitFlush(const VeVector<CardData>& kHand,
	const VeVector<CardData>& kPublic, VeVector<CardData>& kFlush,
	VeVector<CardData>& kElse)
{
	VeUInt32 au32Count[CardSuit_MAX];
	VeZeroMemory(au32Count, sizeof(au32Count));
	for(VeUInt32 i(0); i < kHand.Size(); ++i)
	{
		VE_ASSERT(kHand[i].m_tSuit < CardSuit_MAX);
		++au32Count[kHand[i].m_tSuit];
	}
	for(VeUInt32 i(0); i < kPublic.Size(); ++i)
	{
		VE_ASSERT(kPublic[i].m_tSuit < CardSuit_MAX);
		++au32Count[kPublic[i].m_tSuit];
	}
	VeUInt32 u32CountMax(0);
	CardSuit eMainSuit = CardSuit_MAX;
	for(VeUInt32 i(0); i < CardSuit_MAX; ++i)
	{
		if(au32Count[i] > u32CountMax)
		{
			u32CountMax = au32Count[i];
			eMainSuit = CardSuit(i);
		}
	}
	if(u32CountMax < 5) eMainSuit = CardSuit_MAX;
	kFlush.Clear();
	kElse.Clear();
	for(VeUInt32 i(0); i < kHand.Size(); ++i)
	{
		if(kHand[i].m_tSuit == eMainSuit)
		{
			kFlush.PushBack(kHand[i]);
		}
		else
		{
			kElse.PushBack(kHand[i]);
		}
	}
	for(VeUInt32 i(0); i < kPublic.Size(); ++i)
	{
		if(kPublic[i].m_tSuit == eMainSuit)
		{
			kFlush.PushBack(kPublic[i]);
		}
		else
		{
			kElse.PushBack(kPublic[i]);
		}
	}
	SortCard(kFlush, kFlush);
	SortCard(kElse, kElse);
}
//--------------------------------------------------------------------------
VeUInt32 GetMaxStraightIndex(const VeVector<CardData>& kIn)
{
	VE_ASSERT(kIn.Size() >= 5);
	VeUInt32 u32Count(0), u32Index(0);
	VeUInt32 u32Start = GetCardPower(kIn.Front());
	for(VeUInt32 i(0); i < kIn.Size(); ++i)
	{
		VeUInt32 u32Iter = (i + 1) % kIn.Size();
		VeUInt32 u32Current = GetCardPower(kIn[u32Iter]);
		if((u32Start - u32Current) == (u32Count + 1))
		{
			++u32Count;
		}
		else if(u32Current == (CardNum_MAX - 1)
			&& (u32Start == u32Count) && (u32Count == 3))
		{
			++u32Count;
		}
		else if((kIn.Size() - (i + 1)) >= 4)
		{
			u32Count = 0;
			u32Index = u32Iter;
			u32Start = u32Current;
		}
		else
		{
			break;
		}
	}
	if(u32Count >= 4) return u32Index;
	else return VE_INFINITE;
}
//--------------------------------------------------------------------------
void GetFourKind(const VeVector<CardData>& kCards, CardNum& eFour, CardNum& eElse,
	VeUInt32 pu32Count[CardNum_MAX])
{
	eFour = CardNum_MAX;
	eElse = CardNum_MAX;
	VeZeroMemory(pu32Count, sizeof(VeUInt32) * CardNum_MAX);
	for(VeUInt32 i(0); i < kCards.Size(); ++i)
	{
		VE_ASSERT(kCards[i].m_tNumber < CardNum_MAX);
		if((++pu32Count[kCards[i].m_tNumber]) >= 4)
		{
			eFour = kCards[i].m_tNumber;
			break;
		}
	}
	if(eFour < CardNum_MAX)
	{
		for(VeUInt32 i(0); i < kCards.Size(); ++i)
		{
			VE_ASSERT(kCards[i].m_tNumber < CardNum_MAX);
			if(kCards[i].m_tNumber != eFour)
			{
				if(eElse < CardNum_MAX
					&& (GetCardPower(kCards[i]) <= GetNumberPower(eElse)))
					continue;
				eElse = kCards[i].m_tNumber;
			}
		}
	}
}
//--------------------------------------------------------------------------
CardNum GetStraight(const VeVector<CardData>& kIn)
{
	std::map<VeUInt32,CardNum> kCardSort;
	for(VeUInt32 i(0);i < kIn.Size();++i)
	{
		VeUInt32 u32Key = CardNum_MAX - GetCardPower(kIn[i]);
		kCardSort.insert(std::make_pair(u32Key, kIn[i].m_tNumber));
	}
	if(kCardSort.size() >= 5)
	{
		VeVector<CardData> kTemp;
		for(std::map<VeUInt32,CardNum>::iterator it = kCardSort.begin(); it != kCardSort.end(); ++it)
		{
			CardData kCard;
			kCard.m_tNumber = it->second;
			kCard.m_tSuit = CARD_SPADE;
			kTemp.PushBack(kCard);
		}
		VeUInt32 u32Index = GetMaxStraightIndex(kTemp);
		if(u32Index < kTemp.Size())
		{
			return kTemp[u32Index].m_tNumber;
		}
	}
	return CardNum_MAX;
}
//--------------------------------------------------------------------------
VePair<Categories, VeUInt32> CalculateCardPower(
	const VeVector<CardData>& kHand, const VeVector<CardData>& kPublic, 
	VeVector<CardData>* pkOut = NULL)
{
	VE_ASSERT(kHand.Size() == 2);
	VE_ASSERT(kPublic.Size() == 0 || (kPublic.Size() >= 3 && kPublic.Size() <= 5));
	VePair<Categories, VeUInt32> kRes(Categories_MAX, 0);
	VeVector<CardData> kPoweredCards;
	kPoweredCards.Reserve(5);
	if (kPublic.Size() == 0)
	{
		SortCard(kHand, kPoweredCards);
		VE_ASSERT(kPoweredCards.Size() == 2);
		kRes.m_tFirst = (kPoweredCards[0].m_tNumber == kPoweredCards[1].m_tNumber) ? CATE_ONE_PAIR : CATE_HIGH_CARD;
		kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
		kRes.m_tSecond |= GetCardPower(kPoweredCards[0]) << 16;
		kRes.m_tSecond |= GetCardPower(kPoweredCards[1]) << 12;
	}
	else
	{
		VeVector<CardData> kFlush, kElse;
		SplitFlush(kHand, kPublic, kFlush, kElse);
		if(kFlush.Size())
		{
			VE_ASSERT(kFlush.Size() >= 5);
			VeUInt32 u32Index = GetMaxStraightIndex(kFlush);
			if(u32Index < kFlush.Size())
			{
				kRes.m_tFirst = kFlush[u32Index].m_tNumber == CARD_A ? CATE_ROYAL_FLUSH : CATE_STRAIGHT_FLUSH;
				kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
				kRes.m_tSecond |= GetCardPower(kFlush[u32Index]) << 16;
				if(pkOut)
				{
					(*pkOut).Clear();
					for(VeUInt32 i(0); i < 5; ++i)
					{
						(*pkOut).PushBack(kFlush[(u32Index+i) % kFlush.Size()]);
					}
				}
			}
			else
			{
				kRes.m_tFirst = CATE_FLUSH;
				kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
				for(VeUInt32 i(0); i < 5; ++i)
				{
					kRes.m_tSecond |= GetCardPower(kFlush[i]) << ((4 - i) * 4);
				}
				if(pkOut)
				{
					(*pkOut).Clear();
					for(VeUInt32 i(0); i < 5; ++i)
					{
						(*pkOut).PushBack(kFlush[i]);
					}
				}
			}
		}
		else
		{
			CardNum eFour, eOne;
			VeUInt32 au32Count[CardNum_MAX];
			GetFourKind(kElse, eFour, eOne, au32Count);
			if(eFour < CardNum_MAX)
			{
				kRes.m_tFirst = CATE_FOUR_KIND;
				kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
				kRes.m_tSecond |= GetNumberPower(eFour) << 16;
				kRes.m_tSecond |= GetNumberPower(eFour) << 12;
				kRes.m_tSecond |= GetNumberPower(eFour) << 8;
				kRes.m_tSecond |= GetNumberPower(eFour) << 4;
				kRes.m_tSecond |= GetNumberPower(eOne);
				if(pkOut)
				{
					(*pkOut).Clear();
					for(VeUInt32 i(0); i < kElse.Size(); ++i)
					{
						if(kElse[i].m_tNumber == eFour)
						{
							(*pkOut).PushBack(kElse[i]);
							if((*pkOut).Size() == 4) break;
						}
					}
					for(VeUInt32 i(0); i < kElse.Size(); ++i)
					{
						if(kElse[i].m_tNumber == eOne)
						{
							(*pkOut).PushBack(kElse[i]);
							break;
						}
					}
				}
			}
			else
			{
				CardNum eThree(CardNum_MAX), ePairMax(CardNum_MAX);
				for(VeUInt32 i(0); i < CardNum_MAX; ++i)
				{
					CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
					if(au32Count[eNum] == 3)
					{
						eThree = eNum;
						break;
					}
				}
				for(VeUInt32 i(0); i < CardNum_MAX; ++i)
				{
					CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
					if(au32Count[eNum] >= 2 && eNum != eThree)
					{
						ePairMax = eNum;
						break;
					}
				}
				if(eThree < CardNum_MAX && ePairMax < CardNum_MAX)
				{
					kRes.m_tFirst = CATE_FULL_HOUSE;
					kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
					kRes.m_tSecond |= GetNumberPower(eThree) << 16;
					kRes.m_tSecond |= GetNumberPower(eThree) << 12;
					kRes.m_tSecond |= GetNumberPower(eThree) << 8;
					kRes.m_tSecond |= GetNumberPower(ePairMax) << 4;
					kRes.m_tSecond |= GetNumberPower(ePairMax);
					if(pkOut)
					{
						(*pkOut).Clear();
						for(VeUInt32 i(0); i < kElse.Size(); ++i)
						{
							if(kElse[i].m_tNumber == eThree)
							{
								(*pkOut).PushBack(kElse[i]);
								if((*pkOut).Size() == 3) break;
							}
						}
						for(VeUInt32 i(0); i < kElse.Size(); ++i)
						{
							if(kElse[i].m_tNumber == ePairMax)
							{
								(*pkOut).PushBack(kElse[i]);
								if((*pkOut).Size() == 5) break;
							}
						}
					}
				}
				else
				{
					CardNum eNum = GetStraight(kElse);
					if(eNum < CardNum_MAX)
					{
						kRes.m_tFirst = CATE_STRAIGHT;
						kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
						kRes.m_tSecond |= GetNumberPower(eNum) << 16;
						if(pkOut)
						{
							(*pkOut).Clear();
							for(VeUInt32 i(0); i < 5; ++i)
							{
								CardNum iter = CardNum((eNum + CardNum_MAX - i) % CardNum_MAX);
								for(VeUInt32 j(0); j < kElse.Size(); ++j)
								{
									if(kElse[j].m_tNumber == iter)
									{
										(*pkOut).PushBack(kElse[j]);
										break;
									}
								}
							}
						}
					}
					else if(eThree < CardNum_MAX)
					{
						kRes.m_tFirst = CATE_THREE_KIND;
						kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
						kRes.m_tSecond |= GetNumberPower(eThree) << 16;
						kRes.m_tSecond |= GetNumberPower(eThree) << 12;
						kRes.m_tSecond |= GetNumberPower(eThree) << 8;
						VeUInt32 u32Count(0);
						for(VeUInt32 i(0); i < CardNum_MAX; ++i)
						{
							CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
							if(au32Count[eNum] && eNum != eThree)
							{
								kRes.m_tSecond |= GetNumberPower(eNum) << (u32Count ? 0 : 4);
								if((++u32Count) >= 2)
								{
									break;
								}
							}
						}
						if(pkOut)
						{
							(*pkOut).Clear();
							for(VeUInt32 i(0); i < kElse.Size(); ++i)
							{
								if(kElse[i].m_tNumber == eThree)
								{
									(*pkOut).PushBack(kElse[i]);
									if((*pkOut).Size() == 3) break;
								}
							}
							for(VeUInt32 i(0); i < kElse.Size(); ++i)
							{
								if(kElse[i].m_tNumber != eThree)
								{
									(*pkOut).PushBack(kElse[i]);
									if((*pkOut).Size() == 5) break;
								}
							}
						}
					}
					else if(ePairMax < CardNum_MAX)
					{
						CardNum ePairSecond(CardNum_MAX);
						for(VeUInt32 i(0); i < CardNum_MAX; ++i)
						{
							CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
							if(au32Count[eNum] >= 2 && eNum != ePairMax)
							{
								ePairSecond = eNum;
								break;
							}
						}
						if(ePairSecond < CardNum_MAX)
						{
							kRes.m_tFirst = CATE_TWO_PAIR;
							kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
							kRes.m_tSecond |= GetNumberPower(ePairMax) << 16;
							kRes.m_tSecond |= GetNumberPower(ePairMax) << 12;
							kRes.m_tSecond |= GetNumberPower(ePairSecond) << 8;
							kRes.m_tSecond |= GetNumberPower(ePairSecond) << 4;
							for(VeUInt32 i(0); i < CardNum_MAX; ++i)
							{
								CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
								if(au32Count[eNum] && au32Count[eNum] != ePairMax && au32Count[eNum] != ePairSecond)
								{
									kRes.m_tSecond |= GetNumberPower(eNum);
									break;
								}
							}
							if(pkOut)
							{
								(*pkOut).Clear();
								for(VeUInt32 i(0); i < kElse.Size(); ++i)
								{
									if(kElse[i].m_tNumber == ePairMax)
									{
										(*pkOut).PushBack(kElse[i]);
										if((*pkOut).Size() == 2) break;
									}
								}
								for(VeUInt32 i(0); i < kElse.Size(); ++i)
								{
									if(kElse[i].m_tNumber == ePairSecond)
									{
										(*pkOut).PushBack(kElse[i]);
										if((*pkOut).Size() == 2) break;
									}
								}
								for(VeUInt32 i(0); i < kElse.Size(); ++i)
								{
									if(kElse[i].m_tNumber != ePairMax && kElse[i].m_tNumber != ePairSecond)
									{
										(*pkOut).PushBack(kElse[i]);
										if((*pkOut).Size() == 5) break;
									}
								}
							}
						}
						else
						{
							kRes.m_tFirst = CATE_ONE_PAIR;
							kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
							kRes.m_tSecond |= GetNumberPower(ePairMax) << 16;
							kRes.m_tSecond |= GetNumberPower(ePairMax) << 12;
							VeUInt32 u32Count(0);
							for(VeUInt32 i(0); i < CardNum_MAX; ++i)
							{
								CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
								if(au32Count[eNum] == 1)
								{
									kRes.m_tSecond |= GetNumberPower(eNum) << (8 - (u32Count << 2));
									if((++u32Count) > 2)
									{
										break;
									}
								}
							}
							if(pkOut)
							{
								(*pkOut).Clear();
								for(VeUInt32 i(0); i < kElse.Size(); ++i)
								{
									if(kElse[i].m_tNumber == ePairMax)
									{
										(*pkOut).PushBack(kElse[i]);
										if((*pkOut).Size() == 2) break;
									}
								}
								for(VeUInt32 i(0); i < kElse.Size(); ++i)
								{
									if(kElse[i].m_tNumber != ePairMax)
									{
										(*pkOut).PushBack(kElse[i]);
										if((*pkOut).Size() == 5) break;
									}
								}
							}
						}
					}
					else
					{
						kRes.m_tFirst = CATE_HIGH_CARD;
						kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
						kRes.m_tSecond |= GetCardPower(kElse[0]) << 16;
						kRes.m_tSecond |= GetCardPower(kElse[1]) << 12;
						kRes.m_tSecond |= GetCardPower(kElse[2]) << 8;
						kRes.m_tSecond |= GetCardPower(kElse[3]) << 4;
						kRes.m_tSecond |= GetCardPower(kElse[4]);
						if(pkOut)
						{
							(*pkOut).Clear();
							for(VeUInt32 i(0); i < kElse.Size(); ++i)
							{
								(*pkOut).PushBack(kElse[i]);
								if((*pkOut).Size() == 5) break;
							}
						}
					}
				}
			}
		}
	}
	return kRes;
}
//--------------------------------------------------------------------------
TexasPokerTable::TexasPokerTable(VeUInt32 u32Index, const RoomInfo& kInfo,
	TexasPokerHallServer* pkServer) : m_u32Index(u32Index), m_kRoomInfo(kInfo)
	, m_pkServer(pkServer), m_u32EmptySeat(TEXAS_POKER_SEAT_NUM)
{
	m_tContent = this;
	m_kTableState.m_tState = TABLE_WAIT;
	m_kTableState.m_u8Dealer = TEXAS_POKER_SEAT_NUM;
	m_kTableState.m_u8SmallBlind = TEXAS_POKER_SEAT_NUM;
	m_kTableState.m_u8Process = TEXAS_POKER_SEAT_NUM;
	m_kTableState.m_u8BetFirst = TEXAS_POKER_SEAT_NUM;
	m_kTableState.m_u32BetChips = 0;
	m_kTableState.m_u64ProcessExpire = 0;
	InitSeat();
	TIMER_INIT(TexasPokerTable, OnStart);
	TIMER_INIT(TexasPokerTable, OnRun);
	TIMER_INIT(TexasPokerTable, OnAIProcess);
	m_pkServer->AttachByDelay(TIMER(OnStart), 300);
	u32RateAIConduct = VeRand() % 5;
	u32CheatTag = 0;
}
//--------------------------------------------------------------------------
TexasPokerTable::~TexasPokerTable()
{
	for(VeUInt32 i(0); i < m_kAIPlayerIndex.Size(); ++i)
	{
		VE_ASSERT(m_kAIPlayerIndex[i] < TEXAS_POKER_SEAT_NUM);
		Seat& kSeat = m_akSeatArray[m_kAIPlayerIndex[i]];
		m_pkServer->GetPokerJackPot() += VeInt64(kSeat.m_kPlayerInfo.m_u32Chips);
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::InitSeat()
{
	for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
	{
		m_akSeatArray[i].m_u32Index = i;
		ClearSeat(m_akSeatArray[i]);
	}
	m_u32EmptySeat = TEXAS_POKER_SEAT_NUM;
}
//--------------------------------------------------------------------------
void TexasPokerTable::ClearSeat(Seat& kSeat)
{
	kSeat.m_eState = SEAT_EMPTY;
	kSeat.m_kPlayerInfo.m_u32Index = kSeat.m_u32Index;
	kSeat.m_kPlayerInfo.m_strName = "";
	kSeat.m_kPlayerInfo.m_u32Chips = 0;
	kSeat.m_kPlayerInfo.m_u32ChipsOnTable = 0;
	kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_DISABLE;
	kSeat.m_kPlayerInfo.m_u8Drink = 0xFF;
	kSeat.m_pkTouchedPlayer = NULL;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerTable::GetEmptySeat()
{
	return m_u32EmptySeat;
}
//--------------------------------------------------------------------------
const RoomInfo& TexasPokerTable::GetInfo()
{
	return m_kRoomInfo;
}
//--------------------------------------------------------------------------
void TexasPokerTable::AddPlayer(TexasPokerHallPlayerS* pkPlayer)
{
	VE_ASSERT(pkPlayer && m_u32EmptySeat);
	VE_ASSERT(!pkPlayer->m_pkTargetTable);
	//VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
	/*if(m_u32EmptySeat == TEXAS_POKER_SEAT_NUM)
	{
		for(VeUInt32 i(0); i< TEXAS_POKER_AI_MAX; ++i)
		{
			Seat& kSeat = m_akSeatArray[i];
			VE_ASSERT(kSeat.m_eState == SEAT_EMPTY && kSeat.m_pkTouchedPlayer == NULL);
			kSeat.m_eState = SEAT_AI;
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
			kSeat.m_kPlayerInfo.m_strName = "TestAI" + VeString::From(i);
			kSeat.m_kPlayerInfo.m_u8HeadIcon = VeRand() % 16;
			kSeat.m_kPlayerInfo.m_u8Sex = VeRand() % 2;
			if(VE_MASK_HAS_ANY(m_kRoomInfo.m_u8Flags, ROOM_FLAG_VIP))
				kSeat.m_kPlayerInfo.m_u8Vip = 1;
			else
				kSeat.m_kPlayerInfo.m_u8Vip = (VeRand() % 10) > 7 ? 1 : 0;
			kSeat.m_kPlayerInfo.m_u32Chips = m_kRoomInfo.m_u32ChipsMax;
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable = 0;
			--m_u32EmptySeat;
		}
	}
	VE_ASSERT(m_u32EmptySeat);*/
	for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
	{
		Seat& kSeat = m_akSeatArray[i];
		if(kSeat.m_eState == SEAT_EMPTY)
		{
			VE_ASSERT(!(kSeat.m_pkTouchedPlayer));
			m_kPlayerList.AttachBack(pkPlayer->m_kTableNode);
			pkPlayer->m_pkTargetTable = this;
			pkPlayer->m_u32IndexOnTable = i;
			pkPlayer->ToHome();
			kSeat.m_eState = SEAT_PLAYER;
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_HOLD;
			kSeat.m_kPlayerInfo.m_strName = pkPlayer->GetNickname().GetString();
			kSeat.m_kPlayerInfo.m_u8HeadIcon = pkPlayer->GetHeadIcon();
			kSeat.m_kPlayerInfo.m_u8Sex = pkPlayer->GetSex();
			kSeat.m_kPlayerInfo.m_u8Vip = pkPlayer->GetVip();
			kSeat.m_kPlayerInfo.m_u32Chips = 0;
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable = 0;
			kSeat.m_pkTouchedPlayer = pkPlayer;
			pkPlayer->UpdateRoomInfo();
			pkPlayer->UpdateTable();
			--m_u32EmptySeat;
			break;
		}
	}
	m_kPlayerList.BeginIterator();
	while(!m_kPlayerList.IsEnd())
	{
		TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
		m_kPlayerList.Next();
		VE_ASSERT(pkIter);
		pkIter->UpdateTableSeats();
		if(pkIter == pkPlayer)
		{
			pkIter->UpdateTable();
		}
		else
		{
			pkIter->C_OnTableUpdate();
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::RemovePlayer(TexasPokerHallPlayerS* pkPlayer)
{
	_RemovePlayer(pkPlayer);
	pkPlayer->C_RecInitHall();
	if(m_kPlayerList.Empty())
	{
		VE_DELETE(this);
	}
	else
	{
		m_kPlayerList.BeginIterator();
		while(!m_kPlayerList.IsEnd())
		{
			TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
			m_kPlayerList.Next();
			VE_ASSERT(pkIter);
			pkIter->UpdateTableSeats();
			pkIter->C_OnTableUpdate();
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::_RemovePlayer(TexasPokerHallPlayerS* pkPlayer)
{
	VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
	Seat& kSeat = m_akSeatArray[pkPlayer->m_u32IndexOnTable];
	VE_ASSERT(kSeat.m_pkTouchedPlayer == pkPlayer);
	if(kSeat.m_kPlayerInfo.m_u32Chips)
	{
		if(m_kRoomInfo.m_tType != ROOM_TYPE_PRACTICE)
		{
			pkPlayer->GetChips() += VeUInt64(kSeat.m_kPlayerInfo.m_u32Chips);
			pkPlayer->GetPlayChips() += VeInt64(kSeat.m_kPlayerInfo.m_u32Chips);
			pkPlayer->SyncToClient();
		}
	}
	pkPlayer->m_kTableNode.Detach();
	pkPlayer->m_pkTargetTable = NULL;
	pkPlayer->m_u32IndexOnTable = VE_INFINITE;
	pkPlayer->ToWild();
	if(m_kTableState.m_tState == TABLE_WAIT || kSeat.m_kPlayerInfo.m_tState < TABLE_PLAYER_PLAY)
	{
		ClearSeat(kSeat);
		++m_u32EmptySeat;
	}
	else
	{
		kSeat.m_eState = SEAT_AI_FOLD;
		kSeat.m_kPlayerInfo.m_u32Index = kSeat.m_u32Index;
		kSeat.m_kPlayerInfo.m_strName = "FoldAgent";
		kSeat.m_pkTouchedPlayer = NULL;
	}
}
//--------------------------------------------------------------------------
const TexasPokerTable::Seat& TexasPokerTable::GetSeat(VeUInt32 u32Index)
{
	VE_ASSERT(u32Index < TEXAS_POKER_SEAT_NUM);
	return m_akSeatArray[u32Index];
}
//--------------------------------------------------------------------------
const Table& TexasPokerTable::GetState()
{
	return m_kTableState;
}
//--------------------------------------------------------------------------
void TexasPokerTable::ChangeSeat(TexasPokerHallPlayerS* pkPlayer,
	VeUInt32 u32Target)
{
	if(u32Target < TEXAS_POKER_SEAT_NUM)
	{
		Seat& kSeatTarget = m_akSeatArray[u32Target];
		if(kSeatTarget.m_eState == SEAT_EMPTY)
		{
			VE_ASSERT(m_u32EmptySeat);
			VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
			Seat& kSeat = m_akSeatArray[pkPlayer->m_u32IndexOnTable];
			VE_ASSERT(kSeat.m_pkTouchedPlayer == pkPlayer);
			pkPlayer->m_u32IndexOnTable = u32Target;
			kSeatTarget.m_eState = kSeat.m_eState;
			--m_u32EmptySeat;
			kSeatTarget.m_kPlayerInfo = kSeat.m_kPlayerInfo;
			kSeatTarget.m_kPlayerInfo.m_u32Index = kSeatTarget.m_u32Index;
			kSeatTarget.m_pkTouchedPlayer = kSeat.m_pkTouchedPlayer;
			kSeatTarget.m_kHandCard.Clear();
			if(kSeatTarget.m_kPlayerInfo.m_u32Chips >= m_kRoomInfo.m_u32ChipsMin)
			{
				kSeatTarget.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
			}
			else
			{
				kSeatTarget.m_pkTouchedPlayer->GetChips() += VeUInt64(kSeatTarget.m_kPlayerInfo.m_u32Chips);
				kSeatTarget.m_pkTouchedPlayer->GetPlayChips() += VeInt64(kSeatTarget.m_kPlayerInfo.m_u32Chips);
				kSeatTarget.m_kPlayerInfo.m_u32Chips = 0;
				kSeatTarget.m_kPlayerInfo.m_tState = TABLE_PLAYER_HOLD;
			}
			if(m_kTableState.m_tState == TABLE_WAIT || kSeat.m_kPlayerInfo.m_tState < TABLE_PLAYER_PLAY)
			{
				ClearSeat(kSeat);
				++m_u32EmptySeat;
			}
			else
			{
				kSeat.m_eState = SEAT_AI_FOLD;
				kSeat.m_kPlayerInfo.m_u32Index = kSeat.m_u32Index;
				kSeat.m_kPlayerInfo.m_strName = "FoldAgent";
				kSeat.m_pkTouchedPlayer = NULL;
				
			}
			m_kPlayerList.BeginIterator();
			while(!m_kPlayerList.IsEnd())
			{
				TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
				m_kPlayerList.Next();
				VE_ASSERT(pkIter);
				pkIter->UpdateTableSeats();
				if(pkIter == pkPlayer)
				{
					pkIter->C_OnTableUpdateNoMotion();
				}
				else
				{
					pkIter->C_OnTableUpdate();
				}
			}
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::AddChips(TexasPokerHallPlayerS* pkPlayer)
{
	
}
//--------------------------------------------------------------------------
void TexasPokerTable::Buy(TexasPokerHallPlayerS* pkPlayer,
	VeUInt8 u8Type, VeUInt8 u8Index)
{
	VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
	if(m_kRoomInfo.m_tType == ROOM_TYPE_PRACTICE) return;
	Seat& kSeat = m_akSeatArray[u8Index];
	bool bObj = (u8Type & 0x80) ? true : false;
	u8Type &= 0x7F;
	bool bSelf = pkPlayer->m_u32IndexOnTable == u8Index;
	if(bSelf && bObj) return;
	if(bObj && u8Type >= 4) return;
	if((!bObj) && u8Type >= 20) return;
	VeUInt32 u32Price(0);
	if(bObj) u32Price = pkPlayer->GetAgent()->GetServer()->GetObjectSellPrice()[u8Type];
	else u32Price = pkPlayer->GetAgent()->GetServer()->GetDrinkPrice()[u8Type] * m_kRoomInfo.m_u32SB;
	if((!u32Price) || u32Price > pkPlayer->GetChips()) return;
	pkPlayer->GetChips() -= VeUInt64(u32Price);
	pkPlayer->GetPresentSend() += u32Price;
	pkPlayer->SyncToClient();
	if(bObj)
	{
		if(kSeat.m_pkTouchedPlayer)
		{
			switch(u8Type)
			{
			case 0:
				kSeat.m_pkTouchedPlayer->GetObj0() += VeUInt32(1);
				break;
			case 1:
				kSeat.m_pkTouchedPlayer->GetObj1() += VeUInt32(1);
				break;
			case 2:
				kSeat.m_pkTouchedPlayer->GetObj2() += VeUInt32(1);
				break;
			default:
				kSeat.m_pkTouchedPlayer->GetObj3() += VeUInt32(1);
				break;
			}
			kSeat.m_pkTouchedPlayer->SyncToClient();
		}
	}
	else
	{
		kSeat.m_kPlayerInfo.m_u8Drink = u8Type;
	}
	m_kPlayerList.BeginIterator();
	while(!m_kPlayerList.IsEnd())
	{
		TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
		m_kPlayerList.Next();
		VE_ASSERT(pkIter);
		pkIter->C_OnBuy(u8Type | (bObj ? 0x80 : 0), pkPlayer->m_u32IndexOnTable, u8Index);
		pkIter->UpdateTable();
		pkIter->UpdateTableSeats();
		pkIter->C_OnTableUpdate();
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::BuyAll(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type)
{
	VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
	if(m_kRoomInfo.m_tType == ROOM_TYPE_PRACTICE) return;
	bool bObj = (u8Type & 0x80) ? true : false;
	u8Type &= 0x7F;
	if(bObj && u8Type >= 4) return;
	if((!bObj) && u8Type >= 20) return;
	VeVector<Seat*> kSeats;
	for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
	{
		Seat& kSeat = m_akSeatArray[i];
		if(bObj && pkPlayer->m_u32IndexOnTable == i) continue;
		if(kSeat.m_eState == SEAT_EMPTY) continue;
		kSeats.PushBack(&kSeat);
	}
	if(kSeats.Empty()) return;
	VeUInt32 u32Price(0);
	if(bObj) u32Price = pkPlayer->GetAgent()->GetServer()->GetObjectSellPrice()[u8Type];
	else u32Price = pkPlayer->GetAgent()->GetServer()->GetDrinkPrice()[u8Type] * m_kRoomInfo.m_u32SB;
	u32Price *= kSeats.Size();
	if((!u32Price) || u32Price > pkPlayer->GetChips()) return;
	pkPlayer->GetChips() -= VeUInt64(u32Price);
	pkPlayer->GetPresentSend() += u32Price;
	pkPlayer->SyncToClient();
	for(VeUInt32 i(0); i < kSeats.Size(); ++i)
	{
		Seat& kSeat = *kSeats[i];
		if(bObj)
		{
			if(kSeat.m_pkTouchedPlayer)
			{
				switch(u8Type)
				{
				case 0:
					kSeat.m_pkTouchedPlayer->GetObj0() += VeUInt32(1);
					break;
				case 1:
					kSeat.m_pkTouchedPlayer->GetObj1() += VeUInt32(1);
					break;
				case 2:
					kSeat.m_pkTouchedPlayer->GetObj2() += VeUInt32(1);
					break;
				default:
					kSeat.m_pkTouchedPlayer->GetObj3() += VeUInt32(1);
					break;
				}
				kSeat.m_pkTouchedPlayer->SyncToClient();
			}
		}
		else
		{
			kSeat.m_kPlayerInfo.m_u8Drink = u8Type;
		}
	}	
	m_kPlayerList.BeginIterator();
	while(!m_kPlayerList.IsEnd())
	{
		TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
		m_kPlayerList.Next();
		VE_ASSERT(pkIter);
		pkIter->C_OnBuyAll(u8Type | (bObj ? 0x80 : 0), pkPlayer->m_u32IndexOnTable);
		pkIter->UpdateTable();
		pkIter->UpdateTableSeats();
		pkIter->C_OnTableUpdate();
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::Chat(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type, VeUInt8 u8Index, const VeChar8* strChat)
{
	VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
	if(m_kRoomInfo.m_tType == ROOM_TYPE_PRACTICE) return;
	Seat& kSeat = m_akSeatArray[u8Index];
	bool bObj = (u8Type & 0x80) ? true : false;
	u8Type &= 0x7F;
	bool bSelf = pkPlayer->m_u32IndexOnTable == u8Index;
	if(bSelf && bObj) return;
	if(bObj && u8Type >= 4) return;
	if((!bObj) && u8Type >= 20) return;

	m_kPlayerList.BeginIterator();
	while(!m_kPlayerList.IsEnd())
	{
		TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
		m_kPlayerList.Next();
		VE_ASSERT(pkIter);
		pkIter->C_OnChat(u8Type | (bObj ? 0x80 : 0), pkPlayer->m_u32IndexOnTable, u8Index, strChat);
	}
}

void TexasPokerTable::ChatAll(TexasPokerHallPlayerS* pkPlayer, VeUInt8 u8Type, const VeChar8* strChat)
{
	VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
	if(m_kRoomInfo.m_tType == ROOM_TYPE_PRACTICE) return;
	bool bObj = (u8Type & 0x80) ? true : false;
	u8Type &= 0x7F;
	if(bObj && u8Type >= 4) return;
	if((!bObj) && u8Type >= 20) return;
	VeVector<Seat*> kSeats;
	for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
	{
		Seat& kSeat = m_akSeatArray[i];
		if(bObj && pkPlayer->m_u32IndexOnTable == i) continue;
		if(kSeat.m_eState == SEAT_EMPTY) continue;
		kSeats.PushBack(&kSeat);
	}
	if(kSeats.Empty()) return;

	m_kPlayerList.BeginIterator();
	while(!m_kPlayerList.IsEnd())
	{
		TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
		m_kPlayerList.Next();
		VE_ASSERT(pkIter);
		pkIter->C_OnChatAll(u8Type | (bObj ? 0x80 : 0), pkPlayer->m_u32IndexOnTable, strChat);
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::Ready(TexasPokerHallPlayerS* pkPlayer,
	VeUInt32 u32Chips)
{
	VE_ASSERT(pkPlayer && pkPlayer->m_pkTargetTable == this);
	Seat& kSeat = m_akSeatArray[pkPlayer->m_u32IndexOnTable];
	VE_ASSERT(kSeat.m_pkTouchedPlayer == pkPlayer);
	if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_HOLD)
	{
		if(m_kRoomInfo.m_tType == ROOM_TYPE_PRACTICE)
		{
			kSeat.m_kPlayerInfo.m_u32Chips = m_kRoomInfo.m_u32ChipsMax;
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
		}
		else
		{
			if(u32Chips && (u32Chips <= pkPlayer->GetChips()))
			{
				VeUInt32 u32Min = m_kRoomInfo.m_u32ChipsMin;
				VeUInt32 u32Max = m_kRoomInfo.m_u32ChipsMax;
				if(u32Chips >= u32Min && u32Chips <= u32Max && ((u32Chips - u32Min) % m_kRoomInfo.m_u32SB) == 0)
				{
					pkPlayer->GetChips() -= VeUInt64(u32Chips);
					pkPlayer->GetPlayChips() -= VeInt64(u32Chips);
					pkPlayer->SyncToClient();
					kSeat.m_kPlayerInfo.m_u32Chips = u32Chips;
					kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
				}
			}
		}
	}
	OnSync();
}
//--------------------------------------------------------------------------
void TexasPokerTable::Fold(TexasPokerHallPlayerS* pkPlayer)
{
	if(m_kTableState.m_tState >= TABLE_PLAY_0
		&& m_kTableState.m_tState < TABLE_CLEAR
		&& m_kTableState.m_u64ProcessExpire)
	{
		VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
		if(u32Index == pkPlayer->m_u32IndexOnTable)
		{
			FoldCurrent();
			OnProcess();
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::Call(TexasPokerHallPlayerS* pkPlayer)
{
	if(m_kTableState.m_tState >= TABLE_PLAY_0
		&& m_kTableState.m_tState < TABLE_CLEAR
		&& m_kTableState.m_u64ProcessExpire)
	{
		VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
		if(u32Index == pkPlayer->m_u32IndexOnTable)
		{
			CallCurrent();
			OnProcess();
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::Raise(TexasPokerHallPlayerS* pkPlayer, VeUInt32 u32Chips)
{
	if(m_kTableState.m_tState >= TABLE_PLAY_0
		&& m_kTableState.m_tState < TABLE_CLEAR
		&& m_kTableState.m_u64ProcessExpire)
	{
		VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
		if(u32Index == pkPlayer->m_u32IndexOnTable && (u32Chips >= m_kRoomInfo.m_u32BB))
		{
			Seat& kSeat = m_akSeatArray[u32Index];
			VeUInt32 u32NewBet = m_kTableState.m_u32BetChips + u32Chips;
			if(kSeat.m_kPlayerInfo.m_u32Chips
				&& ((kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) > u32NewBet)
				&& (!(u32Chips % m_kRoomInfo.m_u32BB)))
			{
				RaiseCurrent(u32Chips);
				OnProcess();
			}
			else if(kSeat.m_kPlayerInfo.m_u32Chips
				&& ((kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) == u32NewBet))
			{
				RaiseCurrent(u32Chips);
				OnProcess();
			}
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::BlindBet()
{
	VE_ASSERT(m_kTableState.m_tPlayerIndex.Size() >= 2);
	{
		VeUInt32 u32SbIndex = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8SmallBlind];
		TablePlayer& kSbPlayer = m_akSeatArray[u32SbIndex].m_kPlayerInfo;
		if(kSbPlayer.m_u32Chips > m_kRoomInfo.m_u32SB)
		{
			kSbPlayer.m_u32ChipsOnTable = m_kRoomInfo.m_u32SB;
			kSbPlayer.m_u32Chips -= m_kRoomInfo.m_u32SB;
			kSbPlayer.m_tState = TABLE_PLAYER_SMALL_BLIND;
		}
		else
		{
			kSbPlayer.m_u32ChipsOnTable = kSbPlayer.m_u32Chips;
			kSbPlayer.m_u32Chips = 0;
			kSbPlayer.m_tState = TABLE_PLAYER_ALL_IN;
		}
	}
	{
		VeUInt32 u32BbIndex = m_kTableState.m_tPlayerIndex[(m_kTableState.m_u8SmallBlind + 1) % m_kTableState.m_tPlayerIndex.Size()];
		TablePlayer& kBbPlayer = m_akSeatArray[u32BbIndex].m_kPlayerInfo;
		if(kBbPlayer.m_u32Chips > m_kRoomInfo.m_u32BB)
		{
			kBbPlayer.m_u32ChipsOnTable = m_kRoomInfo.m_u32BB;
			kBbPlayer.m_u32Chips -= m_kRoomInfo.m_u32BB;
			kBbPlayer.m_tState = TABLE_PLAYER_BIG_BLIND;
		}
		else
		{
			kBbPlayer.m_u32ChipsOnTable = kBbPlayer.m_u32Chips;
			kBbPlayer.m_u32Chips = 0;
			kBbPlayer.m_tState = TABLE_PLAYER_ALL_IN;
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::DealHand()
{
	VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
	u32RateAIConduct = VeRand() % 5;
	VeUInt32 u32HasGetNumTag = 0;
	if (m_kAIPlayerIndex.Size())
	{
		while (true)
		{
			for (VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
			{
				Seat& kSeat = m_akSeatArray[m_kTableState.m_tPlayerIndex[i]];
				if (kSeat.m_eState == SEAT_AI)
				{
					VeUInt32 u32RateGetNum = VeRand() % 2;
					if (u32RateGetNum)
					{
						u32CheatTag = i;
						u32HasGetNumTag = 1;
						break;
					}
					else
					{
						continue;
					}				
				}
			}
			if (u32HasGetNumTag)
			{
				break;
			}
			else
			{
				continue;
			}
		}
	}
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		Seat& kSeat = m_akSeatArray[m_kTableState.m_tPlayerIndex[i]];
		if(kSeat.m_eState == SEAT_PLAYER || kSeat.m_eState == SEAT_ROBOT)
		{
			kSeat.m_kHandCard.Resize(2);
			VePair<CardNum, CardSuit> kCard;
			kCard = RandomCard();
			kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
			kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
			kCard = RandomCard();
			kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
			kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
			if(kSeat.m_eState == SEAT_PLAYER)
			{
				VE_ASSERT(kSeat.m_pkTouchedPlayer);
				kSeat.m_pkTouchedPlayer->C_UpdateHandCards(kSeat.m_kHandCard);
			}
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::CheatAIHand()
{
	VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
	VeUInt32 u32RateChange;
	if (m_kTableState.m_tPlayerIndex.Size() < 4)
	{
		u32RateChange = 3;
	}
	else
	{
		u32RateChange = 4;
	}
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		Seat& kSeat = m_akSeatArray[m_kTableState.m_tPlayerIndex[i]];
		if(kSeat.m_eState >= SEAT_AI)
		{
			kSeat.m_kHandCard.Resize(2);
			if (kSeat.m_eState == SEAT_AI)
			{
				if (i == u32CheatTag && m_kAIPlayerIndex.Size() != 0)
				{
					VeUInt32 u32Rate = VeRand() % u32RateChange;
					if (!u32Rate)
					{
						VePair<CardNum, CardSuit> kCard;
						kCard = kAIWinRes[0];
						kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
						kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
						kCard = kAIWinRes[1];
						kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
						kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
					}
					else
					{
						if (u32RateAIConduct == 0 || u32RateAIConduct == 1)
						{
							VePair<CardNum, CardSuit> kCard;
							kCard = kAIWinRes[0];
							kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
							kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
							kCard = kAIWinRes[1];
							kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
							kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
						}
						else if (u32RateAIConduct == 2)
						{
							VePair<CardNum, CardSuit> kCard;
							kCard = kAILosRes[0];
							kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
							kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
							kCard = kAILosRes[1];
							kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
							kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
						}
						else
						{
							VePair<CardNum, CardSuit> kCard;
							kCard = kAIRandomRes[0];
							kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
							kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
							kCard = kAIRandomRes[1];
							kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
							kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
						}
					}
				}
				else
				{
					VePair<CardNum, CardSuit> kCard;
					kCard = RandomCard();
					kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
					kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
					kCard = RandomCard();
					kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
					kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
				}
			}
			else
			{
				VePair<CardNum, CardSuit> kCard;
				kCard = RandomCard();
				kSeat.m_kHandCard[0].m_tNumber = kCard.m_tFirst;
				kSeat.m_kHandCard[0].m_tSuit = kCard.m_tSecond;
				kCard = RandomCard();
				kSeat.m_kHandCard[1].m_tNumber = kCard.m_tFirst;
				kSeat.m_kHandCard[1].m_tSuit = kCard.m_tSecond;
			}
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::DealThree()
{
	VE_ASSERT(m_kTableState.m_tCardList.Size() == 0);
	m_kTableState.m_tCardList.Resize(3);
	for(VeUInt32 i(0); i < 3; ++i)
	{
		VePair<CardNum, CardSuit> kCard;
		kCard = RandomCard();
		m_kTableState.m_tCardList[i].m_tNumber = kCard.m_tFirst;
		m_kTableState.m_tCardList[i].m_tSuit = kCard.m_tSecond;
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::DealFourth()
{
	VE_ASSERT(m_kTableState.m_tCardList.Size() == 3);
	m_kTableState.m_tCardList.Resize(4);
	VePair<CardNum, CardSuit> kCard;
	kCard = RandomCard();
	m_kTableState.m_tCardList[3].m_tNumber = kCard.m_tFirst;
	m_kTableState.m_tCardList[3].m_tSuit = kCard.m_tSecond;
}
//--------------------------------------------------------------------------
void TexasPokerTable::DealFifth()
{
	VE_ASSERT(m_kTableState.m_tCardList.Size() == 4);
	m_kTableState.m_tCardList.Resize(5);
	VePair<CardNum, CardSuit> kCard;
	kCard = RandomCard();
	m_kTableState.m_tCardList[4].m_tNumber = kCard.m_tFirst;
	m_kTableState.m_tCardList[4].m_tSuit = kCard.m_tSecond;

	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
		Seat& kSeat = m_akSeatArray[u32Index];
		if(kSeat.m_pkTouchedPlayer)
		{
			VE_ASSERT(kSeat.m_kHandCard.Size() == 2);
			VeVector<CardData> kOut;
			VePair<Categories, VeUInt32> kNow = CalculateCardPower(kSeat.m_kHandCard, m_kTableState.m_tCardList, &kOut);
			const VeChar8* pcBest = VeString(kSeat.m_pkTouchedPlayer->GetBestType());
			bool bWrite = false;
			if(VeStrlen(pcBest) < 5)
			{
				bWrite = true;
			}
			else
			{
				VeVector<CardData> kBestTypeHand;
				kBestTypeHand.Resize(2);
				VeVector<CardData> kBestTypeTable;
				kBestTypeTable.Resize(3);
				for(VeUInt32 i(0); i < 2; ++i)
				{
					kBestTypeHand[i].m_tNumber = CardNum((pcBest[i] & 0xF) - 1);
					kBestTypeHand[i].m_tSuit = CardSuit((pcBest[i] & 0xF0) >> 4);
				}
				for(VeUInt32 i(0); i < 3; ++i)
				{
					kBestTypeTable[i].m_tNumber = CardNum((pcBest[i+2] & 0xF) - 1);
					kBestTypeTable[i].m_tSuit = CardSuit((pcBest[i+2] & 0xF0) >> 4);
				}
				VePair<Categories, VeUInt32> kBest = CalculateCardPower(kBestTypeHand, kBestTypeTable);
				if(kNow.m_tSecond >= kBest.m_tSecond) bWrite = true;
			}
			if(bWrite && kOut.Size() == 5)
			{
				VeChar8 acBuffer[6];
				for(VeUInt32 j(0); j < 5; ++j)
				{
					acBuffer[j] = (kOut[j].m_tNumber + 1) | (kOut[j].m_tSuit << 4);
				}
				acBuffer[5] = 0;
				kSeat.m_pkTouchedPlayer->GetBestType() = acBuffer;
				kSeat.m_pkTouchedPlayer->SyncToClient();
			}
		}
	}
}
//--------------------------------------------------------------------------
bool TexasPokerTable::HasCard(CardNum tNumber, CardSuit tSuit)
{
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		Seat& kSeat = m_akSeatArray[m_kTableState.m_tPlayerIndex[i]];
		for(VeUInt32 j(0); j < kSeat.m_kHandCard.Size(); ++j)
		{
			CardData& kCard = kSeat.m_kHandCard[j];
			if(kCard.m_tNumber == tNumber && kCard.m_tSuit == tSuit)
			{
				return true;
			}
		}
	}
	for(VeUInt32 i(0); i < m_kTableState.m_tCardList.Size(); ++i)
	{
		CardData& kCard = m_kTableState.m_tCardList[i];
		if(kCard.m_tNumber == tNumber && kCard.m_tSuit == tSuit)
		{
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------
VePair<CardNum, CardSuit> TexasPokerTable::RandomCard()
{
	while(true)
	{
		CardNum	eNum = CardNum(VeRand() % CardNum_MAX);
		CardSuit eSuit = CardSuit(VeRand() % CardSuit_MAX);
		if(!HasCard(eNum, eSuit))
		{
			return VePair<CardNum, CardSuit>(eNum, eSuit);
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::ProcessedAICards()
{
	VeUInt32 u32CardHighTag = 0;
	VeVector<CardData> kHand,kPublic;
	VePair<Categories, VeUInt32> kRes(Categories_MAX, 0);
	VeUInt32 u32CardLowTag = kRes.m_tSecond;
	kHand.Resize(2),kPublic.Resize(5);
	for(VeUInt32 i(0); i < 5; ++i)
	{
		kPublic[i].m_tNumber = m_kTableState.m_tCardList[i].m_tNumber;
		kPublic[i].m_tSuit = m_kTableState.m_tCardList[i].m_tSuit;
	}
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		Seat& kSeat = m_akSeatArray[m_kTableState.m_tPlayerIndex[i]];
		if(kSeat.m_eState == SEAT_PLAYER)
		{
			kHand[0].m_tNumber = kSeat.m_kHandCard[0].m_tNumber;
			kHand[0].m_tSuit = kSeat.m_kHandCard[0].m_tSuit;
			kHand[1].m_tNumber = kSeat.m_kHandCard[1].m_tNumber;
			kHand[1].m_tSuit = kSeat.m_kHandCard[1].m_tSuit;
			kRes = CalculateCardPower(kHand,kPublic);
			if (kRes.m_tSecond > u32CardHighTag)
			{
				u32CardHighTag = kRes.m_tSecond;
			}
			else if (kRes.m_tSecond < u32CardLowTag)
			{
				u32CardLowTag = kRes.m_tSecond;
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}
	}
	VeUInt32 u32BeWinCountIter = 0;
	VeUInt32 u32BeLosCountIter = 0;
	VePair<Categories, VeUInt32> kNewRes(Categories_MAX, 0);
	VeVector<CardData> kNewHand;
	kNewHand.Resize(2);
	while(true)
	{
		u32BeWinCountIter += 1;
		kAIWinRes[0].m_tFirst = CardNum(VeRand() % CardNum_MAX);
		kAIWinRes[0].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
		kAIWinRes[1].m_tFirst = CardNum(VeRand() % CardNum_MAX);
		kAIWinRes[1].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
		kNewHand[0].m_tNumber = kAIWinRes[0].m_tFirst;
		kNewHand[0].m_tSuit = kAIWinRes[0].m_tSecond;
		kNewHand[1].m_tNumber = kAIWinRes[1].m_tFirst;
		kNewHand[1].m_tSuit = kAIWinRes[1].m_tSecond;
		kNewRes = CalculateCardPower(kNewHand,kPublic);
		if (u32BeWinCountIter < 30)
		{
			if (!HasCard(kAIWinRes[0].m_tFirst, kAIWinRes[0].m_tSecond)
			&& !HasCard(kAIWinRes[1].m_tFirst, kAIWinRes[1].m_tSecond))
			{
				if (kNewRes.m_tSecond > u32CardHighTag)
				{
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			kAIWinRes[0].m_tFirst = CardNum(VeRand() % CardNum_MAX);
			kAIWinRes[0].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
			kAIWinRes[1].m_tFirst = CardNum(VeRand() % CardNum_MAX);
			kAIWinRes[1].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
			if (!HasCard(kAIWinRes[0].m_tFirst, kAIWinRes[0].m_tSecond)
			&& !HasCard(kAIWinRes[1].m_tFirst, kAIWinRes[1].m_tSecond))
			{
				break;
			}
		}
	}
	while(true)
	{
		u32BeLosCountIter += 1;
		kAILosRes[0].m_tFirst = CardNum(VeRand() % CardNum_MAX);
		kAILosRes[0].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
		kAILosRes[1].m_tFirst = CardNum(VeRand() % CardNum_MAX);
		kAILosRes[1].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
		kNewHand[0].m_tNumber = kAILosRes[0].m_tFirst;
		kNewHand[0].m_tSuit = kAILosRes[0].m_tSecond;
		kNewHand[1].m_tNumber = kAILosRes[1].m_tFirst;
		kNewHand[1].m_tSuit = kAILosRes[1].m_tSecond;
		kNewRes = CalculateCardPower(kNewHand,kPublic);
		if (u32BeLosCountIter < 30)
		{
			if (!HasCard(kAILosRes[0].m_tFirst, kAILosRes[0].m_tSecond)
			&& !HasCard(kAILosRes[1].m_tFirst, kAILosRes[1].m_tSecond))
			{
				if (kNewRes.m_tSecond < u32CardLowTag)
				{
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			kAILosRes[0].m_tFirst = CardNum(VeRand() % CardNum_MAX);
			kAILosRes[0].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
			kAILosRes[1].m_tFirst = CardNum(VeRand() % CardNum_MAX);
			kAILosRes[1].m_tSecond = CardSuit(VeRand() % CardSuit_MAX);
			if (!HasCard(kAILosRes[0].m_tFirst, kAILosRes[0].m_tSecond)
			&& !HasCard(kAILosRes[1].m_tFirst, kAILosRes[1].m_tSecond))
			{
				break;
			}
		}
	}
	kAIRandomRes[0] = RandomCard();
	kAIRandomRes[1] = RandomCard();
}
//--------------------------------------------------------------------------
const VeChar8* GetPlayerStateName(TablePlayerState eState)
{
	switch(eState)
	{
	case TABLE_PLAYER_PLAY:
		return "play";
	case TABLE_PLAYER_FOLD:
		return "fold";
	case TABLE_PLAYER_RAISE:
		return "raise";
	case TABLE_PLAYER_CALL:
		return "call";
	case TABLE_PLAYER_CHECK:
		return "check";
	case TABLE_PLAYER_ALL_IN:
		return "all_in";
	case TABLE_PLAYER_BIG_BLIND:
		return "big_blind";
	case TABLE_PLAYER_SMALL_BLIND:
		return "small_blind";
	default:
		return "error";
	}
}
//--------------------------------------------------------------------------
void OutputSeat(VeUInt32 u32Index, TexasPokerTable::Seat& kSeat)
{
	VeDebugOutput("seat\tindex:%d,id:%d,name:%s,chips:%d,bet:%d,state:%s,level:%d,bet_current:%d",
		u32Index,
		kSeat.m_kPlayerInfo.m_u32Index,
		kSeat.m_kPlayerInfo.m_strName,
		kSeat.m_kPlayerInfo.m_u32Chips,
		kSeat.m_kPlayerInfo.m_u32ChipsOnTable,
		GetPlayerStateName(kSeat.m_kPlayerInfo.m_tState),
		kSeat.m_kPlayerInfo.m_u8PoolLevel,
		kSeat.m_u32BetCurrent);
}
//--------------------------------------------------------------------------
void TexasPokerTable::Output()
{
	VeDebugOutputString("output table");
	VeDebugOutput("table\tdealer:%d,bet_first:%d,bet_chips:%d",
		m_kTableState.m_u8Dealer,
		m_kTableState.m_u8BetFirst,
		m_kTableState.m_u32BetChips);
	VeStringA kPool("pool\t");
	for(VeUInt32 i(0); i < m_kTableState.m_tPool.Size(); ++i)
	{
		if(i) kPool += ",";
		kPool += VeStringA::From(m_kTableState.m_tPool[i]);
	}
	VeDebugOutputString(kPool);
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		OutputSeat(i, m_akSeatArray[m_kTableState.m_tPlayerIndex[i]]);
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::FoldCurrent()
{
	VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
	Seat& kSeat = m_akSeatArray[u32Index];
	if(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY
		&& kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD
		&& kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_ALL_IN)
	{
		kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_FOLD;
#		ifdef VE_DEBUG
		VeDebugOutput("%s fold", kSeat.m_kPlayerInfo.m_strName);
		Output();
#		endif
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::CallCurrent()
{
	VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
	Seat& kSeat = m_akSeatArray[u32Index];
	if(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY
		&& kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD
		&& kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_ALL_IN)
	{
		if(m_kTableState.m_u32BetChips > kSeat.m_kPlayerInfo.m_u32ChipsOnTable)
		{
			if((kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) > m_kTableState.m_u32BetChips)
			{
				kSeat.m_kPlayerInfo.m_u32Chips = (kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) - m_kTableState.m_u32BetChips;
				kSeat.m_kPlayerInfo.m_u32ChipsOnTable = m_kTableState.m_u32BetChips;
				kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_CALL;
			}
			else
			{
				kSeat.m_kPlayerInfo.m_u32ChipsOnTable += kSeat.m_kPlayerInfo.m_u32Chips;
				kSeat.m_kPlayerInfo.m_u32Chips = 0;
				kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_ALL_IN;
			}
		}
		else
		{
			VE_ASSERT(m_kTableState.m_u32BetChips == kSeat.m_kPlayerInfo.m_u32ChipsOnTable);
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_CHECK;
		}
#		ifdef VE_DEBUG
		VeDebugOutput("%s call", kSeat.m_kPlayerInfo.m_strName);
		Output();
#		endif
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::RaiseCurrent(VeUInt32 u32Chips)
{
	VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
	Seat& kSeat = m_akSeatArray[u32Index];
	if(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY
		&& kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD
		&& kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_ALL_IN)
	{
		VE_ASSERT(u32Chips >= m_kRoomInfo.m_u32BB);
		VeUInt32 u32NewBet = m_kTableState.m_u32BetChips + u32Chips;
		VE_ASSERT(kSeat.m_kPlayerInfo.m_u32Chips);
		if((kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) > u32NewBet)
		{
			VE_ASSERT(!(u32Chips % m_kRoomInfo.m_u32BB));
			kSeat.m_kPlayerInfo.m_u32Chips = (kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) - u32NewBet;
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable = u32NewBet;
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_RAISE;
			m_kTableState.m_u32BetChips = u32NewBet;
			m_kTableState.m_u8BetFirst = m_kTableState.m_u8Process;
		}
		else if((kSeat.m_kPlayerInfo.m_u32Chips + kSeat.m_kPlayerInfo.m_u32ChipsOnTable) == u32NewBet)
		{
			kSeat.m_kPlayerInfo.m_u32Chips = 0;
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable = u32NewBet;
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_ALL_IN;
			m_kTableState.m_u32BetChips = u32NewBet;
			m_kTableState.m_u8BetFirst = m_kTableState.m_u8Process;
		}
		else
		{
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable += kSeat.m_kPlayerInfo.m_u32Chips;
			kSeat.m_kPlayerInfo.m_u32Chips = 0;
			kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_ALL_IN;
			if(kSeat.m_kPlayerInfo.m_u32ChipsOnTable > m_kTableState.m_u32BetChips)
			{
				m_kTableState.m_u32BetChips = kSeat.m_kPlayerInfo.m_u32ChipsOnTable;
				m_kTableState.m_u8BetFirst = m_kTableState.m_u8Process;
			}
		}
		
#ifdef VE_DEBUG
		VeDebugOutput("%s raise:%d", kSeat.m_kPlayerInfo.m_strName, u32Chips);
		Output();
#endif
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::OnProcess()
{
	DeatchTimers();
	m_kTableState.m_u64ProcessExpire = 0;
	OnSync();
	m_pkServer->AttachByDelay(TIMER(OnRun), 100);
}
//--------------------------------------------------------------------------
void TexasPokerTable::OnWaitProcess()
{
	VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
	Seat& kSeat = m_akSeatArray[u32Index];
	VE_ASSERT(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY);
	if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_FOLD || kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_ALL_IN)
	{
		OnRun();
		return;
	}
	else
	{
		m_kTableState.m_u64ProcessExpire = m_pkServer->GetTime() + 1700;
		m_pkServer->AttachByExpire(TIMER(OnRun), m_kTableState.m_u64ProcessExpire);
		if(kSeat.m_eState == SEAT_AI)
		{
			VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
			VeFloat32 f32Scale = VeUnitRandomf();
			m_pkServer->AttachByExpire(TIMER(OnAIProcess), m_pkServer->GetTime() + 100 + VeUInt64(300 * f32Scale));
		}
		else if(kSeat.m_eState == SEAT_AI_FOLD)
		{
			m_pkServer->AttachByDelay(TIMER(OnAIProcess), 10);
		}
		else if(kSeat.m_eState && kSeat.m_eState < SEAT_MAX)
		{
			m_pkServer->AttachByExpire(TIMER(OnAIProcess), m_kTableState.m_u64ProcessExpire - 20);
		}
		
	}
	OnSync();
}
//--------------------------------------------------------------------------
void TexasPokerTable::OnSync()
{
	m_kPlayerList.BeginIterator();
	while(!m_kPlayerList.IsEnd())
	{
		TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
		m_kPlayerList.Next();
		VE_ASSERT(pkIter);
		pkIter->UpdateTable();
		pkIter->UpdateTableSeats();
		pkIter->C_OnTableUpdate();
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::TableChipsToPool()
{
	std::multimap<VeUInt32,Seat*> kSeats;
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
		Seat& kSeat = m_akSeatArray[u32Index];
		kSeat.m_u32BetCurrent += kSeat.m_kPlayerInfo.m_u32ChipsOnTable;
		kSeats.insert(std::make_pair(kSeat.m_kPlayerInfo.m_u32ChipsOnTable, &kSeat));
	}
	VeVector<VeUInt32> kFission;
	VeUInt32 u32MaxBet(0);
	VE_ASSERT(m_kTableState.m_tPool.Size());
	VeUInt32 u32LastPointer = m_kTableState.m_tPool.Size() - 1;
	for(std::multimap<VeUInt32,Seat*>::iterator it = kSeats.begin(); it != kSeats.end(); ++it)
	{
		Seat& kSeat = *(it->second);
		bool bBet = kSeat.m_kPlayerInfo.m_u32ChipsOnTable ? true : false;
		if(kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD)
		{
			if(kFission.Empty())
			{
				kFission.PushBack(kSeat.m_kPlayerInfo.m_u32ChipsOnTable);
				u32MaxBet = kSeat.m_kPlayerInfo.m_u32ChipsOnTable;
			}
			else if(u32MaxBet < kSeat.m_kPlayerInfo.m_u32ChipsOnTable)
			{
				kFission.PushBack(kSeat.m_kPlayerInfo.m_u32ChipsOnTable - u32MaxBet);
				u32MaxBet = kSeat.m_kPlayerInfo.m_u32ChipsOnTable;
			}
			if(bBet)
			{
				VE_ASSERT(kFission.Size());
				if((m_kTableState.m_tPool.Size() - u32LastPointer) < kFission.Size())
				{
					VE_ASSERT((kFission.Size() -(m_kTableState.m_tPool.Size() - u32LastPointer)) == 1);
					m_kTableState.m_tPool.PushBack(0);
				}
				for(VeUInt32 i(0); i < kFission.Size(); ++i)
				{
					VE_ASSERT(kFission[i] <= kSeat.m_kPlayerInfo.m_u32ChipsOnTable);
					kSeat.m_kPlayerInfo.m_u32ChipsOnTable -= kFission[i];
					m_kTableState.m_tPool[u32LastPointer + i] += kFission[i];
				}
				VE_ASSERT(!kSeat.m_kPlayerInfo.m_u32ChipsOnTable);
				kSeat.m_kPlayerInfo.m_u8PoolLevel = m_kTableState.m_tPool.Size() - 1;
			}
		}
		else if(bBet)
		{
			m_kTableState.m_tPool[u32LastPointer] += kSeat.m_kPlayerInfo.m_u32ChipsOnTable;
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable = 0;
			kSeat.m_kPlayerInfo.m_u8PoolLevel = u32LastPointer;
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerTable::UpdateAIPlayers()
{
	for(VeUInt32 i(0); i< TEXAS_POKER_SEAT_NUM; ++i)
	{
		Seat& kSeat = m_akSeatArray[i];
		if(kSeat.m_eState == SEAT_AI)
		{
			VeUInt32 u32Rand = VeRand() % 10;
			if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_HOLD)
			{
				kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
				kSeat.m_kPlayerInfo.m_u32Chips = m_kRoomInfo.m_u32ChipsMax;
				m_pkServer->GetPokerJackPot() -= VeInt64(m_kRoomInfo.m_u32ChipsMax);
				return;
			}
			else if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_READY)
			{
				if((kSeat.m_kPlayerInfo.m_u32Chips >= (m_kRoomInfo.m_u32ChipsMax * 5) && u32Rand < 5) || u32Rand == 0)
				{
					m_pkServer->GetPokerJackPot() += VeInt64(kSeat.m_kPlayerInfo.m_u32Chips);
					for(VeVector<VeUInt8>::iterator it = m_kAIPlayerIndex.Begin(); it != m_kAIPlayerIndex.End(); ++it)
					{
						if((*it) == i)
						{
							m_kAIPlayerIndex.Erase(it);
							break;
						}
					}
					ClearSeat(kSeat);
					++m_u32EmptySeat;
					return;
				}
			}
		}
	}
	if(m_u32EmptySeat > TEXAS_POKER_EMPTY_MAX)
	{
		const VeVector<VeStringA>& kNameArray = m_pkServer->GetNameArray();
		if(m_kAIPlayerIndex.Size() < TEXAS_POKER_AI_MAX)
		{
			for(VeUInt32 i(0); i< TEXAS_POKER_SEAT_NUM; ++i)
			{
				Seat& kSeat = m_akSeatArray[i];
				if(kSeat.m_eState == SEAT_EMPTY)
				{
					kSeat.m_eState = SEAT_AI;
					kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_HOLD;
					kSeat.m_kPlayerInfo.m_strName = kNameArray[VeRand() % kNameArray.Size()];
					kSeat.m_kPlayerInfo.m_u8HeadIcon = VeRand() % 16;
					kSeat.m_kPlayerInfo.m_u8Sex = (kSeat.m_kPlayerInfo.m_u8HeadIcon < 8) ? 0 : 1;
					if(VE_MASK_HAS_ANY(m_kRoomInfo.m_u8Flags, ROOM_FLAG_VIP))
						kSeat.m_kPlayerInfo.m_u8Vip = 1;
					else
						kSeat.m_kPlayerInfo.m_u8Vip = (VeRand() % 10) > 7 ? 1 : 0;
					kSeat.m_kPlayerInfo.m_u32Chips = 0;
					kSeat.m_kPlayerInfo.m_u32ChipsOnTable = 0;
					--m_u32EmptySeat;
					m_kAIPlayerIndex.PushBack(i);
					break;
				}
			}
		}
	}
}
//--------------------------------------------------------------------------
TIMER_IMPL(TexasPokerTable, OnStart)
{
	DeatchTimers();
	if(m_kTableState.m_tState == TABLE_WAIT)
	{
		for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
		{
			Seat& kSeat = m_akSeatArray[i];
			if(kSeat.m_eState)
			{
				VE_ASSERT(kSeat.m_kPlayerInfo.m_tState < TABLE_PLAYER_PLAY);
				if(kSeat.m_eState == SEAT_AI_FOLD)
				{
					ClearSeat(kSeat);
					++m_u32EmptySeat;
				}
				else if(kSeat.m_pkTouchedPlayer)
				{
					if((kSeat.m_kPlayerInfo.m_u32Chips == 0 && kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_READY)
						|| kSeat.m_pkTouchedPlayer->GetAgent() == NULL)
					{
						TexasPokerHallPlayerS* pkPlayer = kSeat.m_pkTouchedPlayer;
						_RemovePlayer(pkPlayer);
						pkPlayer->C_RecInitHall();
					}
				} 
			}
		}
		if(m_kPlayerList.Empty())
		{
			VE_DELETE(this);
			return;
		}
		VeUInt32 u32ReadyPlayerNum(0);
		for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
		{
			Seat& kSeat = m_akSeatArray[i];
			kSeat.m_kHandCard.Clear();
			if(kSeat.m_eState && kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_READY)
			{
				++u32ReadyPlayerNum;
			}
		}
		
		if(u32ReadyPlayerNum >= 2)
		{
			m_kTableState.m_tState = TABLE_DEAL_0;
			m_kTableState.m_tPool.Resize(1);
			m_kTableState.m_tPool[0] = 0;
			m_kTableState.m_tPlayerIndex.Clear();
			m_kTableState.m_tCardList.Clear();
			m_kTableState.m_tHandCardList.Clear();
			if(m_kTableState.m_u8Dealer >= TEXAS_POKER_SEAT_NUM)
			{
				VeVector<VeUInt32> kReady;
				for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
				{
					Seat& kSeat = m_akSeatArray[i];
					if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_READY)
					{
						kReady.PushBack(i);
					}
				}
				VE_ASSERT(kReady.Size());
				m_kTableState.m_u8Dealer = kReady[VeRand() % kReady.Size()];
			}
			else
			{
				m_kTableState.m_u8Dealer = (m_kTableState.m_u8Dealer + 1) % TEXAS_POKER_SEAT_NUM;
			}
			for(VeUInt32 i(m_kTableState.m_u8Dealer); i < TEXAS_POKER_SEAT_NUM; ++i)
			{
				Seat& kSeat = m_akSeatArray[i];
				if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_READY)
				{
					kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_PLAY;
					kSeat.m_kPlayerInfo.m_u8PoolLevel = 0;
					kSeat.m_u32BetCurrent = 0;
					m_kTableState.m_tPlayerIndex.PushBack(i);
					if(kSeat.m_pkTouchedPlayer)
					{
						kSeat.m_pkTouchedPlayer->GetPlayCount() += VeUInt32(1);
						kSeat.m_pkTouchedPlayer->GetTickPlayCount() += VeUInt32(1);
						kSeat.m_pkTouchedPlayer->SyncToClient();
					}
				}
			}
			for(VeUInt32 i(0); i < m_kTableState.m_u8Dealer; ++i)
			{
				Seat& kSeat = m_akSeatArray[i];
				if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_READY)
				{
					kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_PLAY;
					kSeat.m_kPlayerInfo.m_u8PoolLevel = 0;
					kSeat.m_u32BetCurrent = 0;
					m_kTableState.m_tPlayerIndex.PushBack(i);
					if(kSeat.m_pkTouchedPlayer)
					{
						kSeat.m_pkTouchedPlayer->GetPlayCount() += VeUInt32(1);
						kSeat.m_pkTouchedPlayer->GetTickPlayCount() += VeUInt32(1);
						kSeat.m_pkTouchedPlayer->SyncToClient();
					}
				}
			}
			m_kTableState.m_u8Dealer = m_kTableState.m_tPlayerIndex[0];
			m_kTableState.m_u8SmallBlind = m_kTableState.m_tPlayerIndex.Size() > 2 ? 1 : 0;
			BlindBet();
			DealHand();
			m_kTableState.m_u8BetFirst = (m_kTableState.m_u8SmallBlind + 1) % m_kTableState.m_tPlayerIndex.Size();
			m_kTableState.m_u32BetChips = m_kRoomInfo.m_u32BB;
			m_kTableState.m_u8Process = m_kTableState.m_u8BetFirst;
			m_kTableState.m_u64ProcessExpire = TIMER(OnRun)->GetExecuteTime();
			m_bFirstBlood = true;
			VeUInt32 u32Delay = m_kTableState.m_tPlayerIndex.Size() * 60;
			u32Delay += 300;
			m_pkServer->AttachByDelay(TIMER(OnRun), u32Delay);
		}
		else
		{
			m_pkServer->AttachByDelay(TIMER(OnStart), 300);
		}
		UpdateAIPlayers();
		OnSync();
	}
}
//--------------------------------------------------------------------------
TIMER_IMPL(TexasPokerTable, OnRun)
{
	DeatchTimers();
	VE_ASSERT(m_kTableState.m_tState);
	if(m_kTableState.m_tState == TABLE_DEAL_0) m_kTableState.m_tState = TABLE_PLAY_0;
	if(m_kTableState.m_tState < TABLE_CLEAR)
	{
		bool bDeal = m_kTableState.m_tState < TABLE_PLAY_0;
		VeUInt32 u32Round = (m_kTableState.m_tState - 1) % 4;
		if(bDeal)
		{
			VE_ASSERT(u32Round);
			VeUInt32 u32BetFirstIndex = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8BetFirst];
			VeVector<VeUInt8> kNewPlayerIndex;
			for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
			{
				VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
				Seat& kSeat = m_akSeatArray[u32Index];
				if(kSeat.m_kPlayerInfo.m_tState == TABLE_PLAYER_FOLD)
				{
					kSeat.m_kHandCard.Clear();
					kSeat.m_kPlayerInfo.m_u8PoolLevel = 0;
				}
				else
				{
					kNewPlayerIndex.PushBack(u32Index);
					if(u32Index == u32BetFirstIndex)
					{
						m_kTableState.m_u8BetFirst = kNewPlayerIndex.Size() - 1;
						m_kTableState.m_u8Process = m_kTableState.m_u8BetFirst;
					}
					if(kSeat.m_kPlayerInfo.m_u32Chips)
					{
						kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_PLAY;
					}
					else
					{
						kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_ALL_IN;
					}
				}
			}
			m_kTableState.m_tPlayerIndex = kNewPlayerIndex;
			m_kTableState.m_u32BetChips = 0;
			m_kTableState.m_tState = TableState(TABLE_PLAY_0 + u32Round);
			OnWaitProcess();
		}
		else
		{
			VeUInt32 u32PlayerNum(0);
			for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
			{
				VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
				Seat& kSeat = m_akSeatArray[u32Index];
				if(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY && (kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD))
				{
					++u32PlayerNum;
				}
			}
			if(u32PlayerNum >= 2)
			{
				m_kTableState.m_u8Process = (m_kTableState.m_u8Process + 1) % m_kTableState.m_tPlayerIndex.Size();
				if(m_kTableState.m_u8Process == m_kTableState.m_u8BetFirst)
				{
					VE_ASSERT(m_kTableState.m_tPlayerIndex.Size() >= 2);
					TableChipsToPool();
					m_kTableState.m_u8BetFirst = 0;
					m_kTableState.m_u8Process = 0;
					for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
					{
						VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
						if(u32Index != m_kTableState.m_u8Dealer
							&& m_akSeatArray[u32Index].m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD)
						{
							m_kTableState.m_u8BetFirst = i;
							m_kTableState.m_u8Process = i;
							break;
						}
					}
					switch(u32Round)
					{
					case 0:
						DealThree();
						m_kTableState.m_tState = TABLE_DEAL_1;
						m_pkServer->AttachByDelay(TIMER(OnRun), 260);
						break;
					case 1:
						DealFourth();
						m_kTableState.m_tState = TABLE_DEAL_2;
						m_pkServer->AttachByDelay(TIMER(OnRun), 200);
						break;
					case 2:
						DealFifth();
						m_kTableState.m_tState = TABLE_DEAL_3;
						m_pkServer->AttachByDelay(TIMER(OnRun), 200);
						break;
					default:
						VE_ASSERT(u32Round == 3);
						OnCompare();
						break;
					}
					OnSync();
				}
				else
				{
					if(m_bFirstBlood)
					{
						m_kTableState.m_u8BetFirst = (m_kTableState.m_u8BetFirst + 1) % m_kTableState.m_tPlayerIndex.Size();
						m_bFirstBlood = false;
					}
					OnWaitProcess();
				}
			}
			else
			{
				TableChipsToPool();
				VE_ASSERT(u32PlayerNum && m_kTableState.m_tPool.Size());
				VeUInt32 u32PoolAll(0);
				for(VeUInt32 i(0); i < m_kTableState.m_tPool.Size(); ++i)
				{
					u32PoolAll += m_kTableState.m_tPool[i];
				}
				for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
				{
					VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
					Seat& kSeat = m_akSeatArray[u32Index];
					if(kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY && (kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD))
					{
						kSeat.m_kPlayerInfo.m_u32ChipsOnTable = u32PoolAll;
						kSeat.m_kPlayerInfo.m_u32Chips += kSeat.m_kPlayerInfo.m_u32ChipsOnTable;
						m_kTableState.m_tPool.Clear();
						if(kSeat.m_pkTouchedPlayer)
						{
							kSeat.m_pkTouchedPlayer->OnWin(kSeat.m_u32BetCurrent);
						}
						break;
					}
				}
				VE_ASSERT(m_kTableState.m_tPool.Empty());
				m_kTableState.m_tState = TABLE_ALL_FOLD;
				m_pkServer->AttachByDelay(TIMER(OnRun), 500);
				OnSync();
			}
		}
	}
	else
	{
		for(VeUInt32 i(0); i < TEXAS_POKER_SEAT_NUM; ++i)
		{
			Seat& kSeat = m_akSeatArray[i];
			if(kSeat.m_eState && kSeat.m_kPlayerInfo.m_tState >= TABLE_PLAYER_PLAY)
			{
				kSeat.m_kPlayerInfo.m_u32ChipsOnTable = 0;
				if(kSeat.m_kPlayerInfo.m_u32Chips >= m_kRoomInfo.m_u32BB)
				{
					kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
				}
				else if(kSeat.m_pkTouchedPlayer)
				{
					kSeat.m_pkTouchedPlayer->GetChips() += VeUInt64(kSeat.m_kPlayerInfo.m_u32Chips);
					kSeat.m_pkTouchedPlayer->GetPlayChips() += VeInt64(kSeat.m_kPlayerInfo.m_u32Chips);
					kSeat.m_kPlayerInfo.m_u32Chips = 0;
					if((kSeat.m_pkTouchedPlayer->GetChips() >= m_kRoomInfo.m_u32ChipsMin) || m_kRoomInfo.m_tType == ROOM_TYPE_PRACTICE)
					{
						kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_HOLD;
					}
					else
					{
						kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_READY;
					}
				}
				else
				{
					kSeat.m_kPlayerInfo.m_tState = TABLE_PLAYER_HOLD;
					m_pkServer->GetPokerJackPot() += VeInt64(kSeat.m_kPlayerInfo.m_u32Chips);
					kSeat.m_kPlayerInfo.m_u32Chips = 0;
				}
			}
		}
		UpdateAIPlayers();
		m_kTableState.m_tState = TABLE_WAIT;
		m_kPlayerList.BeginIterator();
		while(!m_kPlayerList.IsEnd())
		{
			TexasPokerHallPlayerS* pkIter = m_kPlayerList.GetIterNode()->m_tContent;
			m_kPlayerList.Next();
			VE_ASSERT(pkIter);
			pkIter->UpdateTable();
			pkIter->UpdateTableSeats();
			pkIter->C_OnTableUpdateNoMotion();
		}
		m_pkServer->AttachByDelay(TIMER(OnStart), 100);
	}
}
//--------------------------------------------------------------------------
TIMER_IMPL(TexasPokerTable, OnAIProcess)
{
	VeUInt32 u32RandNumTag = 1 + VeRand() % 5;
	DeatchTimers();
	VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[m_kTableState.m_u8Process];
	Seat& kSeat = m_akSeatArray[u32Index];
	if(kSeat.m_kPlayerInfo.m_tState != TABLE_PLAYER_FOLD)
	{
		if(kSeat.m_eState == SEAT_AI)
		{
			if (u32RateAIConduct == 0 || u32RateAIConduct == 1)
			{
				if (m_kTableState.m_tCardList.Size() == 0)
				{
					CallCurrent();
				}
				else if (m_kTableState.m_tCardList.Size() == 3)
				{
					VeUInt32 u32Rate = VeRand() % 7;
					if (!u32Rate)
					{
						FoldCurrent();
					}
					else if (u32Rate == 1)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB);
					}
					else
					{
						CallCurrent();
					}
				}
				else if (m_kTableState.m_tCardList.Size() == 4)
				{
					VeUInt32 u32Rate = VeRand() % 4;
					if (!u32Rate)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB * u32RandNumTag);
					}
					else
					{
						CallCurrent();
					}
				}
				else
				{
					VeUInt32 u32Rate = VeRand() % 3;
					if (u32Rate)
					{
						CallCurrent();
					}
					else
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB * u32RandNumTag * (3 + (VeRand() % 2)));
					}
				}
			}
			else if (u32RateAIConduct == 2)
			{
				if (m_kTableState.m_tCardList.Size() == 0)
				{
					CallCurrent();
				}
				else if (m_kTableState.m_tCardList.Size() == 3)
				{
					VeUInt32 u32Rate = VeRand() % 7;
					if (!u32Rate)
					{
						FoldCurrent();
					}
					else if (u32Rate == 1)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB);
					}
					else
					{
						CallCurrent();
					}
				}
				else if (m_kTableState.m_tCardList.Size() == 4)
				{
					VeUInt32 u32Rate = VeRand() % 7;
					if (!u32Rate)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB);
					}
					else
					{
						CallCurrent();
					}
				}
				else
				{
					VeUInt32 u32Rate = VeRand() % 6;
					if (!u32Rate)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB);
					}
					else
					{
						CallCurrent();
					}
				}
			}
			else
			{
				if (m_kTableState.m_tCardList.Size() == 0)
				{
					CallCurrent();
				}
				else if (m_kTableState.m_tCardList.Size() == 3)
				{
					VeUInt32 u32Rate = VeRand() % 7;
					if (!u32Rate)
					{
						FoldCurrent();
					}
					else if (u32Rate == 1)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB);
					}
					else
					{
						CallCurrent();
					}
				}
				else if (m_kTableState.m_tCardList.Size() == 4)
				{
					VeUInt32 u32Rate = VeRand() % 6;
					if (!u32Rate)
					{
						FoldCurrent();
					}
					else if (u32Rate == 2)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB);
					}
					else
					{
						CallCurrent();
					}
				}
				else
				{
					VeUInt32 u32Rate = VeRand() % 5;
					if (!u32Rate)
					{
						RaiseCurrent(m_kRoomInfo.m_u32BB * u32RandNumTag);
					}
					else
					{
						CallCurrent();
					}
				}
			}
		}
		else if(kSeat.m_eState == SEAT_AI_FOLD)
		{
			FoldCurrent();
		}
		else if(kSeat.m_eState && kSeat.m_eState < SEAT_MAX)
		{
			FoldCurrent();
		}
	}
	OnProcess();
}
//--------------------------------------------------------------------------
void TexasPokerTable::OnCompare()
{
	ProcessedAICards();
	CheatAIHand();
	m_kTableState.m_tState = TABLE_CLEAR;
	VE_ASSERT(m_kTableState.m_tPlayerIndex.Size() >= 2);
	VE_ASSERT(m_kTableState.m_tHandCardList.Empty());
	VE_ASSERT(m_kTableState.m_tCardList.Size() == 5);
	for(VeUInt32 i(0); i < m_kTableState.m_tPlayerIndex.Size(); ++i)
	{
		VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[i];
		Seat& kSeat = m_akSeatArray[u32Index];
		VE_ASSERT(kSeat.m_kHandCard.Size() == 2);
		m_kTableState.m_tHandCardList.PushBack(kSeat.m_kHandCard[0]);
		m_kTableState.m_tHandCardList.PushBack(kSeat.m_kHandCard[1]);
	}
	TableChipsToPool();
	for(VeUInt32 i(0); i < m_kTableState.m_tPool.Size(); ++i)
	{
		VeUInt32 u32MaxPower(0);
		VeVector<Seat*> kWinPlayers;
		for(VeUInt32 j(0); j < m_kTableState.m_tPlayerIndex.Size(); ++j)
		{
			VeUInt32 u32Index = m_kTableState.m_tPlayerIndex[j];
			Seat& kSeat = m_akSeatArray[u32Index];
			if(kSeat.m_kPlayerInfo.m_tState > TABLE_PLAYER_FOLD && kSeat.m_kPlayerInfo.m_u8PoolLevel >= i)
			{
				VE_ASSERT(kSeat.m_kHandCard.Size() == 2);
				VePair<Categories, VeUInt32> kPower = CalculateCardPower(kSeat.m_kHandCard, m_kTableState.m_tCardList);
				if(kPower.m_tSecond > u32MaxPower)
				{
					kWinPlayers.Clear();
					kWinPlayers.PushBack(&kSeat);
					u32MaxPower = kPower.m_tSecond;
				}
				else if(kPower.m_tSecond == u32MaxPower)
				{
					kWinPlayers.PushBack(&kSeat);
				}
			}
		}
		if(kWinPlayers.Empty()) continue;
		VeUInt32 u32PerPlayer = m_kTableState.m_tPool[i];
		if(kWinPlayers.Size() > 1)
		{
			u32PerPlayer -= (u32PerPlayer / kWinPlayers.Size()) % m_kRoomInfo.m_u32SB;
			VE_ASSERT(m_kTableState.m_tPool[i] >= u32PerPlayer);
			VeUInt32 u32Frac = m_kTableState.m_tPool[i] - u32PerPlayer;
			u32PerPlayer /= kWinPlayers.Size();
			m_kTableState.m_tPool[i] = 0;
			if(u32Frac)
			{
				Seat* pkOrder(NULL);
				for(VeUInt32 j(0); j < kWinPlayers.Size(); ++j)
				{
					if(pkOrder)
					{
						VeUInt32 u32SetpOrder = (pkOrder->m_u32Index + TEXAS_POKER_SEAT_NUM - m_kTableState.m_u8Dealer - 1) % TEXAS_POKER_SEAT_NUM;
						VeUInt32 u32SetpSeat = (kWinPlayers[j]->m_u32Index + TEXAS_POKER_SEAT_NUM - m_kTableState.m_u8Dealer - 1) % TEXAS_POKER_SEAT_NUM;
						if(u32SetpSeat < u32SetpOrder)
						{
							pkOrder = kWinPlayers[j];
						}
					}
					else
					{
						pkOrder = kWinPlayers[j];
					}
				}
				VE_ASSERT(pkOrder);
				pkOrder->m_kPlayerInfo.m_u32ChipsOnTable += u32Frac;
				pkOrder->m_kPlayerInfo.m_u32Chips += u32Frac;
			}
		}
		for(VeUInt32 j(0); j < kWinPlayers.Size(); ++j)
		{
			Seat& kSeat = *kWinPlayers[j];
			kSeat.m_kPlayerInfo.m_u32ChipsOnTable += u32PerPlayer;
			kSeat.m_kPlayerInfo.m_u32Chips += u32PerPlayer;
			if(i == 0 && kSeat.m_pkTouchedPlayer)
			{
				kSeat.m_pkTouchedPlayer->OnWin(kSeat.m_u32BetCurrent);
			}
		}
	}
	m_pkServer->AttachByDelay(TIMER(OnRun), 700);
}
//--------------------------------------------------------------------------
void TexasPokerTable::DeatchTimers()
{
	TIMER(OnStart)->Detach();
	TIMER(OnRun)->Detach();
	TIMER(OnAIProcess)->Detach();
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerTable::GetIndex()
{
	return m_u32Index;
}
//--------------------------------------------------------------------------
