#include "TexasPokerHallPlayerC.h"
#include "TexasPokerHallAgentC.h"
#include "SoundSystem.h"
#include "TexasPoker.h"
#include "Knickknacks.h"
#include "Login.h"
#include "Hall.h"
#include "Slot.h"
#include "Keno.h"
#include "Baccarat.h"
#include "Game.h"
#include "NativeInterface.h"
#include <map>
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "CocoaHelper.h"
#endif

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
VeUInt32 GetCardIndex(const VeVector<CardData>& kHand,
	const VeVector<CardData>& kPublic, CardData& kData)
{
	for(VeUInt32 i(0); i < kHand.Size(); ++i)
	{
		if(kHand[i].m_tNumber == kData.m_tNumber
			&& kHand[i].m_tSuit == kData.m_tSuit)
		{
			return i;
		}
	}
	for(VeUInt32 i(0); i < kPublic.Size(); ++i)
	{
		if(kPublic[i].m_tNumber == kData.m_tNumber
			&& kPublic[i].m_tSuit == kData.m_tSuit)
		{
			return i + 2;
		}
	}
	return VE_INFINITE;
}
//--------------------------------------------------------------------------
void GetCardIndex(const VeVector<CardData>& kHand,
	const VeVector<CardData>& kPublic, CardNum eNum, VeVector<VeUInt32>& kOut)
{
	kOut.Clear();
	for(VeUInt32 i(0); i < kHand.Size(); ++i)
	{
		if(kHand[i].m_tNumber == eNum)
		{
			kOut.PushBack(i);
		}
	}
	for(VeUInt32 i(0); i < kPublic.Size(); ++i)
	{
		if(kPublic[i].m_tNumber == eNum)
		{
			kOut.PushBack(i+2);
		}
	}
}
//--------------------------------------------------------------------------
VePair<Categories, VeUInt32> CalculateCardPower(
	const VeVector<CardData>& kHand, const VeVector<CardData>& kPublic,
	bool abHightLight[7])
{
	VeZeroMemory(abHightLight, sizeof(bool) * 7);
	VE_ASSERT(kHand.Size() == 2);
	VE_ASSERT(kPublic.Size() == 0 || kPublic.Size() >= 3 || kPublic.Size() <= 5);
	VePair<Categories, VeUInt32> kRes(Categories_MAX, 0);
	VeVector<CardData> kPoweredCards;
	VeVector<VeUInt32> kHightLightCard;
	kPoweredCards.Reserve(5);
	if(kPublic.Size() == 0)
	{
		SortCard(kHand, kPoweredCards);
		VE_ASSERT(kPoweredCards.Size() == 2);
		kRes.m_tFirst = (kPoweredCards[0].m_tNumber == kPoweredCards[1].m_tNumber) ? CATE_ONE_PAIR : CATE_HIGH_CARD;
		kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
		kRes.m_tSecond |= GetCardPower(kPoweredCards[0]) << 16;
		kRes.m_tSecond |= GetCardPower(kPoweredCards[1]) << 12;
		if(kRes.m_tFirst == CATE_ONE_PAIR)
		{
			abHightLight[0] = true;
			abHightLight[1] = true;
		}
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
				for(VeUInt32 i(0); i < 5; ++i)
				{
					VeUInt32 u32Card = GetCardIndex(kHand, kPublic, kFlush[i + u32Index]);
					VE_ASSERT(u32Card < 7);
					abHightLight[u32Card] = true;
				}
			}
			else
			{
				kRes.m_tFirst = CATE_FLUSH;
				kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
				for(VeUInt32 i(0); i < 5; ++i)
				{
					kRes.m_tSecond |= GetCardPower(kFlush[i]) << ((4 - i) * 4);
					VeUInt32 u32Card = GetCardIndex(kHand, kPublic, kFlush[i]);
					VE_ASSERT(u32Card < 7);
					abHightLight[u32Card] = true;
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
				GetCardIndex(kHand, kPublic, eFour, kHightLightCard);
				VE_ASSERT(kHightLightCard.Size() >= 4);
				for(VeUInt32 i(0); i < 4; ++i)
				{
					VeUInt32 u32Card = kHightLightCard[i];
					VE_ASSERT(u32Card < 7);
					abHightLight[u32Card] = true;
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
					GetCardIndex(kHand, kPublic, eThree, kHightLightCard);
					VE_ASSERT(kHightLightCard.Size() >= 3);
					for(VeUInt32 i(0); i < 3; ++i)
					{
						VeUInt32 u32Card = kHightLightCard[i];
						VE_ASSERT(u32Card < 7);
						abHightLight[u32Card] = true;
					}
					GetCardIndex(kHand, kPublic, ePairMax, kHightLightCard);
					VE_ASSERT(kHightLightCard.Size() >= 2);
					for(VeUInt32 i(0); i < 2; ++i)
					{
						VeUInt32 u32Card = kHightLightCard[i];
						VE_ASSERT(u32Card < 7);
						abHightLight[u32Card] = true;
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
						for(VeUInt32 i(0); i < 5; ++i)
						{
							CardNum eIter = CardNum((eNum + CardNum_MAX - i) % CardNum_MAX);
							GetCardIndex(kHand, kPublic, eIter, kHightLightCard);
							VE_ASSERT(kHightLightCard.Size());
							VeUInt32 u32Card = kHightLightCard.Front();
							VE_ASSERT(u32Card < 7);
							abHightLight[u32Card] = true;
						}
					}
					else if(eThree < CardNum_MAX)
					{
						kRes.m_tFirst = CATE_THREE_KIND;
						kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
						kRes.m_tSecond |= GetNumberPower(eThree) << 16;
						kRes.m_tSecond |= GetNumberPower(eThree) << 12;
						kRes.m_tSecond |= GetNumberPower(eThree) << 8;
						GetCardIndex(kHand, kPublic, eThree, kHightLightCard);
						VE_ASSERT(kHightLightCard.Size() >= 3);
						for(VeUInt32 i(0); i < 3; ++i)
						{
							VeUInt32 u32Card = kHightLightCard[i];
							VE_ASSERT(u32Card < 7);
							abHightLight[u32Card] = true;
						}
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
							GetCardIndex(kHand, kPublic, ePairMax, kHightLightCard);
							VE_ASSERT(kHightLightCard.Size() >= 2);
							for(VeUInt32 i(0); i < 2; ++i)
							{
								VeUInt32 u32Card = kHightLightCard[i];
								VE_ASSERT(u32Card < 7);
								abHightLight[u32Card] = true;
							}
							GetCardIndex(kHand, kPublic, ePairSecond, kHightLightCard);
							VE_ASSERT(kHightLightCard.Size() >= 2);
							for(VeUInt32 i(0); i < 2; ++i)
							{
								VeUInt32 u32Card = kHightLightCard[i];
								VE_ASSERT(u32Card < 7);
								abHightLight[u32Card] = true;
							}
							for(VeUInt32 i(0); i < CardNum_MAX; ++i)
							{
								CardNum eNum = i ? CardNum(CardNum_MAX - i) : CARD_A;
								if(au32Count[eNum] == 1)
								{
									kRes.m_tSecond |= GetNumberPower(eNum);
									break;
								}
							}
						}
						else
						{
							kRes.m_tFirst = CATE_ONE_PAIR;
							kRes.m_tSecond |= GetCategoriesMask(kRes.m_tFirst);
							kRes.m_tSecond |= GetNumberPower(ePairMax) << 16;
							kRes.m_tSecond |= GetNumberPower(ePairMax) << 12;
							GetCardIndex(kHand, kPublic, ePairMax, kHightLightCard);
							VE_ASSERT(kHightLightCard.Size() >= 2);
							for(VeUInt32 i(0); i < 2; ++i)
							{
								VeUInt32 u32Card = kHightLightCard[i];
								VE_ASSERT(u32Card < 7);
								abHightLight[u32Card] = true;
							}
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
					}
				}
			}
		}
	}
	return kRes;
}
//--------------------------------------------------------------------------
TexasPokerHallPlayerC::TexasPokerHallPlayerC(ServerAgent* pkParent)
	: EntityC(pkParent, GetName())
///	<CODE-GENERATE>{EntityClientPropertiesImpl Player}
	, m_strNickname(this, "Nickname")
	, m_u64Chips(this, "Chips")
	, m_u64GoldBricks(this, "GoldBricks")
	, m_u32HeadIcon(this, "HeadIcon")
	, m_u32Sex(this, "Sex")
	, m_u32Title(this, "Title")
	, m_u32Vip(this, "Vip")
	, m_u32BankCard(this, "BankCard")
	, m_u64BankCardExpire(this, "BankCardExpire")
	, m_u32Obj0(this, "Obj0")
	, m_u32Obj1(this, "Obj1")
	, m_u32Obj2(this, "Obj2")
	, m_u32Obj3(this, "Obj3")
	, m_u32Medal0(this, "Medal0")
	, m_u32Medal1(this, "Medal1")
	, m_u32Medal2(this, "Medal2")
	, m_u32PlayCount(this, "PlayCount")
	, m_u32WinCount(this, "WinCount")
	, m_u32WinMaxBet(this, "WinMaxBet")
	, m_strBestType(this, "BestType")
	, m_strPhone(this, "Phone")
	, m_u32SlotCount(this, "SlotCount")
	, m_u32KenoCount(this, "KenoCount")
	, m_strKenoResult(this, "KenoResult")
	, m_u32BaccaratCount(this, "BaccaratCount")
	, m_strBaccaratResult(this, "BaccaratResult")
	, m_u64Purchase(this, "Purchase")
	, m_u64PurchaseChips(this, "PurchaseChips")
	, m_u32PurchaseCount(this, "PurchaseCount")
	, m_u32EventModifyInfo(this, "EventModifyInfo")
	, m_u32EventPurchase(this, "EventPurchase")
	, m_u32EventVip(this, "EventVip")
	, m_u32EventPresent(this, "EventPresent")
	, m_u32EventTickCurrent(this, "EventTickCurrent")
	, m_u32EventTickOnline(this, "EventTickOnline")
	, m_u32EventTickPlayCount(this, "EventTickPlayCount")
	, m_u32EventTickPlaySlotCount(this, "EventTickPlaySlotCount")
	, m_u32EventTickPlayKenoCount(this, "EventTickPlayKenoCount")
	, m_u32EventTickPlayBaccCount(this, "EventTickPlayBaccCount")
	, m_u32EventTickPlayWinCombo0(this, "EventTickPlayWinCombo0")
	, m_u32EventTickPlayWinCombo1(this, "EventTickPlayWinCombo1")
	, m_u32EventTickPlayWinCombo2(this, "EventTickPlayWinCombo2")
	, m_u32TickPlayCount(this, "TickPlayCount")
	, m_u32TickPlaySlotCount(this, "TickPlaySlotCount")
	, m_u32TickPlayKenoCount(this, "TickPlayKenoCount")
	, m_u32TickPlayBaccCount(this, "TickPlayBaccCount")
	, m_u32TickPlayWinCombo0(this, "TickPlayWinCombo0")
	, m_u32TickPlayWinCombo1(this, "TickPlayWinCombo1")
	, m_u32TickPlayWinCombo2(this, "TickPlayWinCombo2")
	, m_u32PresentSend(this, "PresentSend")
///	</CODE-GENERATE>{EntityClientPropertiesImpl Player}
{
	m_u32KenoIssue = 0;
	m_u64KenoDrawTime = 0;

	m_u32BaccIndex = 0;
	m_u64BaccExpireTime = 0;
	m_u32BaccBetPlayer = 0;
	m_u64BaccAllBet = 0;

	m_u32SeatIndex = 0;
	m_u32TablePool = 0;

	m_eCategories = Categories_MAX;
	VeZeroMemory(m_abHighLight, sizeof(m_abHighLight));
}
//--------------------------------------------------------------------------
TexasPokerHallPlayerC::~TexasPokerHallPlayerC()
{

}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallPlayerC::GetName()
{
///	<CODE-GENERATE>{EntityName Player}
	return "Player";
///	</CODE-GENERATE>{EntityName Player}
}
//--------------------------------------------------------------------------
TexasPokerHallAgentC* TexasPokerHallPlayerC::GetAgent()
{
	return static_cast<TexasPokerHallAgentC*>(m_pkAgent);
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerC::OnLoad()
{

}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerC::OnUnload()
{
	GetAgent()->GetClient()->ForceDisconnect(Connection::CONNECT_FORCE_OFF);
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerC::OnUpdate()
{
	UILayer* pkLayer = g_pLayerManager->GetCurrentMainLayer();
	if(pkLayer)
	{
		pkLayer->OnForceUpdate();
	}
#   if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CocoaHelper::OnUpdateChips(m_u64Chips);
#   endif
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerC::OnRPCCallback(BitStream& kStream)
{
///	<CODE-GENERATE>{EntityClientRPCCallback Player}
	StreamErrorClear();
	VeUInt8 u8FuncID;
	kStream >> u8FuncID;
	switch(u8FuncID)
	{
	case CLIENT_RecInitHall:
		{
			if(!IsStreamError()) C_RecInitHall();
		}
		break;
	case CLIENT_UpdateRoomList:
		{
			VeVector<RoomInfo> tRoomList;
			kStream >> tRoomList;
			VeVector<VeUInt32> tDrinkPrice;
			kStream >> tDrinkPrice;
			VeVector<VeUInt32> tObjPrice;
			kStream >> tObjPrice;
			if(!IsStreamError()) C_UpdateRoomList(tRoomList, tDrinkPrice, tObjPrice);
		}
		break;
	case CLIENT_UpdateOnline:
		{
			VeUInt32 u32Number;
			kStream >> u32Number;
			if(!IsStreamError()) C_UpdateOnline(u32Number);
		}
		break;
	case CLIENT_RecEditInfo:
		{
			EditInfoRes tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecEditInfo(tResult);
		}
		break;
	case CLIENT_RecGetObjExchangePrice:
		{
			VeVector<VeUInt32> tExchangePriceList;
			kStream >> tExchangePriceList;
			if(!IsStreamError()) C_RecGetObjExchangePrice(tExchangePriceList);
		}
		break;
	case CLIENT_RecExchangeObj:
		{
			EditInfoRes tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecExchangeObj(tResult);
		}
		break;
	case CLIENT_RecSlotBonusCard:
		{
			VeUInt32 u32Bonus;
			kStream >> u32Bonus;
			if(!IsStreamError()) C_RecSlotBonusCard(u32Bonus);
		}
		break;
	case CLIENT_RecSlotJP:
		{
			VeUInt64 u64JPChips;
			kStream >> u64JPChips;
			if(!IsStreamError()) C_RecSlotJP(u64JPChips);
		}
		break;
	case CLIENT_RecSlotRun:
		{
			VeUInt64 u64WinChips;
			kStream >> u64WinChips;
			VeUInt8 u8Result0;
			kStream >> u8Result0;
			VeUInt8 u8Result1;
			kStream >> u8Result1;
			VeUInt8 u8Result2;
			kStream >> u8Result2;
			if(!IsStreamError()) C_RecSlotRun(u64WinChips, u8Result0, u8Result1, u8Result2);
		}
		break;
	case CLIENT_RecKenoUpdate:
		{
			VeUInt32 u32IssueNumber;
			kStream >> u32IssueNumber;
			VeUInt64 u64ExpireTime;
			kStream >> u64ExpireTime;
			if(!IsStreamError()) C_RecKenoUpdate(u32IssueNumber, u64ExpireTime);
		}
		break;
	case CLIENT_RecBuyKeno:
		{
			KenoRes tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecBuyKeno(tResult);
		}
		break;
	case CLIENT_RecKenoDraw:
		{
			KenoRes tResult;
			kStream >> tResult;
			VeUInt8 u8Hit;
			kStream >> u8Hit;
			VeUInt32 u32Issue;
			kStream >> u32Issue;
			VeUInt32 u32Bet;
			kStream >> u32Bet;
			VeUInt64 u64Win;
			kStream >> u64Win;
			if(!IsStreamError()) C_RecKenoDraw(tResult, u8Hit, u32Issue, u32Bet, u64Win);
		}
		break;
	case CLIENT_RecBaccaratUpdate:
		{
			VeUInt32 u32Index;
			kStream >> u32Index;
			VeUInt64 u64ExpireTime;
			kStream >> u64ExpireTime;
			VeUInt32 u32AllBetPlayer;
			kStream >> u32AllBetPlayer;
			VeUInt64 u64AllBetChips;
			kStream >> u64AllBetChips;
			if(!IsStreamError()) C_RecBaccaratUpdate(u32Index, u64ExpireTime, u32AllBetPlayer, u64AllBetChips);
		}
		break;
	case CLIENT_RecBaccaratBet:
		{
			BaccaratRes tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecBaccaratBet(tResult);
		}
		break;
	case CLIENT_RecBaccaratPlay:
		{
			BaccaratRes tResult;
			kStream >> tResult;
			VeUInt32 u32WinChips;
			kStream >> u32WinChips;
			if(!IsStreamError()) C_RecBaccaratPlay(tResult, u32WinChips);
		}
		break;
	case CLIENT_UpdateRoomInfo:
		{
			RoomInfo tInfo;
			kStream >> tInfo;
			if(!IsStreamError()) C_UpdateRoomInfo(tInfo);
		}
		break;
	case CLIENT_UpdateTableSeats:
		{
			VeUInt8 u8SeatIndex;
			kStream >> u8SeatIndex;
			VeVector<TablePlayer> tPlayers;
			kStream >> tPlayers;
			if(!IsStreamError()) C_UpdateTableSeats(u8SeatIndex, tPlayers);
		}
		break;
	case CLIENT_UpdateTable:
		{
			Table tTabel;
			kStream >> tTabel;
			if(!IsStreamError()) C_UpdateTable(tTabel);
		}
		break;
	case CLIENT_UpdateHandCards:
		{
			VeVector<CardData> tHandCards;
			kStream >> tHandCards;
			if(!IsStreamError()) C_UpdateHandCards(tHandCards);
		}
		break;
	case CLIENT_RecEnterPokerTable:
		{
			EnterTableRes tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecEnterPokerTable(tResult);
		}
		break;
	case CLIENT_OnTableUpdate:
		{
			if(!IsStreamError()) C_OnTableUpdate();
		}
		break;
	case CLIENT_OnTableUpdateNoMotion:
		{
			if(!IsStreamError()) C_OnTableUpdateNoMotion();
		}
		break;
	case CLIENT_OnBuy:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VeUInt8 u8From;
			kStream >> u8From;
			VeUInt8 u8To;
			kStream >> u8To;
			if(!IsStreamError()) C_OnBuy(u8Type, u8From, u8To);
		}
		break;
	case CLIENT_OnBuyAll:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VeUInt8 u8From;
			kStream >> u8From;
			if(!IsStreamError()) C_OnBuyAll(u8Type, u8From);
		}
		break;
	case CLIENT_OnChat:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VeUInt8 u8From;
			kStream >> u8From;
			VeUInt8 u8To;
			kStream >> u8To;
			VEString strChat;
			kStream >> strChat;
			if(!IsStreamError()) C_OnChat(u8Type, u8From, u8To, strChat);
		}
		break;
	case CLIENT_OnChatAll:
		{
			VeUInt8 u8Type;
			kStream >> u8Type;
			VeUInt8 u8From;
			kStream >> u8From;
			VEString strChat;
			kStream >> strChat;
			if(!IsStreamError()) C_OnChatAll(u8Type, u8From, strChat);
		}
		break;
	case CLIENT_OnPurchaseCompleted:
		{
			VEString strChannel;
			kStream >> strChannel;
			VeUInt32 u32Chips;
			kStream >> u32Chips;
			if(!IsStreamError()) C_OnPurchaseCompleted(strChannel, u32Chips);
		}
		break;
	case CLIENT_RecRefreshPurchase:
		{
			RefreshPurchaseRes tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecRefreshPurchase(tResult);
		}
		break;
	case CLIENT_RecFinishEvent:
		{
			EventAwardType tType;
			kStream >> tType;
			VeUInt32 u32Value;
			kStream >> u32Value;
			if(!IsStreamError()) C_RecFinishEvent(tType, u32Value);
		}
		break;
	case CLIENT_RecNotice:
		{
			VEString strNotice;
			kStream >> strNotice;
			VeUInt16 u16Show;
			kStream >> u16Show;
			VeUInt16 u16Interval;
			kStream >> u16Interval;
			if(!IsStreamError()) C_RecNotice(strNotice, u16Show, u16Interval);
		}
		break;
	default:
		break;
	}
///	</CODE-GENERATE>{EntityClientRPCCallback Player}
}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{EntityServerFuncSend Player}
void TexasPokerHallPlayerC::S_ReqInitHall()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqInitHall);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqTickHall()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqTickHall);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqEditInfo(const VeChar8* strNickname, VeUInt8 u8Sex, VeUInt8 u8HeadIcon, const VeChar8* strPhone)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqEditInfo);
	kStream << strNickname;
	kStream << u8Sex;
	kStream << u8HeadIcon;
	kStream << strPhone;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqGetObjExchangePrice()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqGetObjExchangePrice);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqExchangeObj(VeUInt32 u32Index)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqExchangeObj);
	kStream << u32Index;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqSlotBonusCard()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqSlotBonusCard);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqSlotJP()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqSlotJP);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqSlotRun(VeUInt32 u32BetCount)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqSlotRun);
	kStream << u32BetCount;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqKenoUpdate()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqKenoUpdate);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBuyKeno(VeUInt8 u8Bet, const VeVector<VeUInt8>& tPlayerNumber)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBuyKeno);
	kStream << u8Bet;
	kStream << tPlayerNumber;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqKenoDraw()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqKenoDraw);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBaccaratUpdate()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBaccaratUpdate);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBaccaratBet(const BaccaratBetType& tBetType, VeUInt32 u32Chips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBaccaratBet);
	kStream << tBetType;
	kStream << u32Chips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBaccaratBetAll(VeUInt32 u32Banker, VeUInt32 u32Player, VeUInt32 u32Tie)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBaccaratBetAll);
	kStream << u32Banker;
	kStream << u32Player;
	kStream << u32Tie;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBaccaratPlay()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBaccaratPlay);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqEnterPokerTable(VeUInt32 u32Index)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqEnterPokerTable);
	kStream << u32Index;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqLeavePokerTable()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqLeavePokerTable);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqChangeSeat(VeUInt32 u32Index)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqChangeSeat);
	kStream << u32Index;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqAddChips()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqAddChips);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqChangeRoom()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqChangeRoom);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqReady(VeUInt32 u32Chips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqReady);
	kStream << u32Chips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqFold()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqFold);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqCall()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqCall);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqFill(VeUInt32 u32Chips)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqFill);
	kStream << u32Chips;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBuy(VeUInt8 u8Type, VeUInt8 u8Index)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBuy);
	kStream << u8Type;
	kStream << u8Index;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqBuyAll(VeUInt8 u8Type)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqBuyAll);
	kStream << u8Type;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqRefreshPurchase()
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqRefreshPurchase);
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqFinishEvent(const EventInfo& tInfo)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqFinishEvent);
	kStream << tInfo;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqChat(VeUInt8 u8Type, VeUInt8 u8Index, const VeChar8* strChat)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqChat);
	kStream << u8Type;
	kStream << u8Index;
	kStream << strChat;
	SendPacket(kStream);
}
void TexasPokerHallPlayerC::S_ReqChatAll(VeUInt8 u8Type, const VeChar8* strChat)
{
	if(!m_pkAgent) return;
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_ENTITY_RPC);
	kStream << GetName();
	kStream << VeUInt8(SERVER_ReqChatAll);
	kStream << u8Type;
	kStream << strChat;
	SendPacket(kStream);
}
///	</CODE-GENERATE>{EntityServerFuncSend Player}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{EntityClientFuncImpl Player}
void TexasPokerHallPlayerC::C_RecInitHall()
{
///	<CODE-IMPLEMENT>{Player RecInitHall}
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	g_pkGame->Toast(TT("ReLoginSucc"));
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->ToHall();
	}
	else
	{
		g_pkGame->ShowKenoResult();
	}
///	</CODE-IMPLEMENT>{Player RecInitHall}
}
void TexasPokerHallPlayerC::C_UpdateRoomList(const VeVector<RoomInfo>& tRoomList, const VeVector<VeUInt32>& tDrinkPrice, const VeVector<VeUInt32>& tObjPrice)
{
///	<CODE-IMPLEMENT>{Player UpdateRoomList}
	GetUILayer(Hall,"hall")->UpdateRoomList(tRoomList);
	m_kDrinkPrice = tDrinkPrice;
	m_kObjectPrice = tObjPrice;
///	</CODE-IMPLEMENT>{Player UpdateRoomList}
}
void TexasPokerHallPlayerC::C_UpdateOnline(VeUInt32 u32Number)
{
///	<CODE-IMPLEMENT>{Player UpdateOnline}
	GetUILayer(Hall,"hall")->UpdateOnline(u32Number);
///	</CODE-IMPLEMENT>{Player UpdateOnline}
}
void TexasPokerHallPlayerC::C_RecEditInfo(const EditInfoRes& tResult)
{
///	<CODE-IMPLEMENT>{Player RecEditInfo}
	GetUILayer(Hall,"hall")->UpdateProperties();
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	if(VE_SUCCEEDED(tResult))
	{
		g_pkGame->Toast(TT("EditInfoSucceed"));
	}
	else
	{
		g_pkGame->Toast(TT("EditInfoFaild"));
	}
///	</CODE-IMPLEMENT>{Player RecEditInfo}
}
void TexasPokerHallPlayerC::C_RecGetObjExchangePrice(const VeVector<VeUInt32>& tExchangePriceList)
{
///	<CODE-IMPLEMENT>{Player RecGetObjExchangePrice}
	GetUILayer(Bank, "bank")->UpdateChange(tExchangePriceList);
///	</CODE-IMPLEMENT>{Player RecGetObjExchangePrice}
}
void TexasPokerHallPlayerC::C_RecExchangeObj(const EditInfoRes& tResult)
{
///	<CODE-IMPLEMENT>{Player RecExchangeObj}
	GetUILayer(Hall,"hall")->UpdateProperties();
	GetUILayer(Bank, "bank")->UpdateObject();
	if(VE_SUCCEEDED(tResult))
	{
		g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
		g_pkGame->Toast(TT("ItmeExChangeSucceed"));
	}
	else
	{
		g_pkGame->Toast(TT("ItmeExChangeFaild"));
	}
///	</CODE-IMPLEMENT>{Player RecExchangeObj}
}
void TexasPokerHallPlayerC::C_RecSlotBonusCard(VeUInt32 u32Bonus)
{
///	<CODE-IMPLEMENT>{Player RecSlotBonusCard}
	GetUILayer(Bank, "bank")->OnBonus(u32Bonus);
///	</CODE-IMPLEMENT>{Player RecSlotBonusCard}
}
void TexasPokerHallPlayerC::C_RecSlotJP(VeUInt64 u64JPChips)
{
///	<CODE-IMPLEMENT>{Player RecSlotJP}
	GetUILayer(Slot,"slot")->SetJPChips(u64JPChips);
///	</CODE-IMPLEMENT>{Player RecSlotJP}
}
void TexasPokerHallPlayerC::C_RecSlotRun(VeUInt64 u64WinChips, VeUInt8 u8Result0, VeUInt8 u8Result1, VeUInt8 u8Result2)
{
///	<CODE-IMPLEMENT>{Player RecSlotRun}
	GetUILayer(Slot,"slot")->Stop(u64WinChips, u8Result0, u8Result1, u8Result2);
	VeVector< VePair<VeStringA,VeStringA> > kEventValue;
	kEventValue.PushBack(VePair<VeStringA,VeStringA>("win_chips", VeStringA::From(VeUInt32(u64WinChips))));
	NativeOnEvent("RecSlotRun", kEventValue);
///	</CODE-IMPLEMENT>{Player RecSlotRun}
}
void TexasPokerHallPlayerC::C_RecKenoUpdate(VeUInt32 u32IssueNumber, VeUInt64 u64ExpireTime)
{
///	<CODE-IMPLEMENT>{Player RecKenoUpdate}
	m_u32KenoIssue = u32IssueNumber;
	m_u64KenoDrawTime = u64ExpireTime;
///	</CODE-IMPLEMENT>{Player RecKenoUpdate}
}
void TexasPokerHallPlayerC::C_RecBuyKeno(const KenoRes& tResult)
{
///	<CODE-IMPLEMENT>{Player RecBuyKeno}
	if(VE_SUCCEEDED(tResult))
	{
		GetUILayer(Keno,"keno")->OnBuySucc();
	}
	else
	{
		GetUILayer(Keno,"keno")->OnBuyFailed();
	}
///	</CODE-IMPLEMENT>{Player RecBuyKeno}
}
void TexasPokerHallPlayerC::C_RecKenoDraw(const KenoRes& tResult, VeUInt8 u8Hit, VeUInt32 u32Issue, VeUInt32 u32Bet, VeUInt64 u64Win)
{
///	<CODE-IMPLEMENT>{Player RecKenoDraw}
	if(VE_SUCCEEDED(tResult))
	{
		VeChar8 acBuffer[128];
		if(u64Win)
		{
			g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
			VeSprintf(acBuffer, 128, "%s%d%s%d%s", TT("KenoWin0"), u32Issue, TT("KenoWin1"), u8Hit, TT("KenoWin2"));
			g_pkGame->Toast(acBuffer);
			VeSprintf(acBuffer, 128, "%s%d%s%llu", TT("KenoWin3"), u32Bet, TT("KenoWin4"), u64Win);
			g_pkGame->Toast(acBuffer, false);
		}
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("win_chips", VeStringA::From(VeUInt32(u64Win))));
		NativeOnEvent("RecKenoDraw", kEventValue);
	}
///	</CODE-IMPLEMENT>{Player RecKenoDraw}
}
void TexasPokerHallPlayerC::C_RecBaccaratUpdate(VeUInt32 u32Index, VeUInt64 u64ExpireTime, VeUInt32 u32AllBetPlayer, VeUInt64 u64AllBetChips)
{
///	<CODE-IMPLEMENT>{Player RecBaccaratUpdate}
	m_u32BaccIndex = u32Index;
	m_u64BaccExpireTime = u64ExpireTime;
	m_u32BaccBetPlayer = u32AllBetPlayer;
	m_u64BaccAllBet = u64AllBetChips;
	GetUILayer(Baccarat,"baccarat")->UpdateAllPlayers();
///	</CODE-IMPLEMENT>{Player RecBaccaratUpdate}
}
void TexasPokerHallPlayerC::C_RecBaccaratBet(const BaccaratRes& tResult)
{
///	<CODE-IMPLEMENT>{Player RecBaccaratBet}
	if(VE_FAILED(tResult))
	{
		g_pkGame->Toast(TT("BaccBetFailed"));
	}
///	</CODE-IMPLEMENT>{Player RecBaccaratBet}
}
void TexasPokerHallPlayerC::C_RecBaccaratPlay(const BaccaratRes& tResult, VeUInt32 u32WinChips)
{
///	<CODE-IMPLEMENT>{Player RecBaccaratPlay}
	if(VE_SUCCEEDED(tResult))
	{
		if(u32WinChips)
		{
			g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
		}
		VeVector< VePair<VeStringA,VeStringA> > kEventValue;
		kEventValue.PushBack(VePair<VeStringA,VeStringA>("win_chips", VeStringA::From(VeUInt32(u32WinChips))));
		NativeOnEvent("RecBaccaratPlay", kEventValue);
	}
///	</CODE-IMPLEMENT>{Player RecBaccaratPlay}
}
void TexasPokerHallPlayerC::C_UpdateRoomInfo(const RoomInfo& tInfo)
{
///	<CODE-IMPLEMENT>{Player UpdateRoomInfo}
	m_kRoomInfo = tInfo;
///	</CODE-IMPLEMENT>{Player UpdateRoomInfo}
}
void TexasPokerHallPlayerC::C_UpdateTableSeats(VeUInt8 u8SeatIndex, const VeVector<TablePlayer>& tPlayers)
{
///	<CODE-IMPLEMENT>{Player UpdateTableSeats}
	m_u32SeatIndex = u8SeatIndex;
	m_kPlayers.Resize(tPlayers.Size());
	for(VeUInt32 i(0); i < tPlayers.Size(); ++i)
	{
		m_kPlayers[i].m_u32Index = tPlayers[i].m_u32Index;
		m_kPlayers[i].m_strName = tPlayers[i].m_strName;
		m_kPlayers[i].m_u32Chips = tPlayers[i].m_u32Chips;
		m_kPlayers[i].m_u32ChipsOnTable = tPlayers[i].m_u32ChipsOnTable;
		m_kPlayers[i].m_tState = tPlayers[i].m_tState;
		m_kPlayers[i].m_u8HeadIcon = tPlayers[i].m_u8HeadIcon;
		m_kPlayers[i].m_u8Sex = tPlayers[i].m_u8Sex;
		m_kPlayers[i].m_u8Vip = tPlayers[i].m_u8Vip;
		m_kPlayers[i].m_u8PoolLevel = tPlayers[i].m_u8PoolLevel;
		m_kPlayers[i].m_u8Drink = tPlayers[i].m_u8Drink;
	}
///	</CODE-IMPLEMENT>{Player UpdateTableSeats}
}
void TexasPokerHallPlayerC::C_UpdateTable(const Table& tTabel)
{
///	<CODE-IMPLEMENT>{Player UpdateTable}
	m_kTable = tTabel;
	m_u32TablePool = 0;
	for(VeUInt32 i(0); i < m_kTable.m_tPool.Size(); ++i)
	{
		m_u32TablePool += m_kTable.m_tPool[i];
	}
	UpdateCategories();
///	</CODE-IMPLEMENT>{Player UpdateTable}
}
void TexasPokerHallPlayerC::C_UpdateHandCards(const VeVector<CardData>& tHandCards)
{
///	<CODE-IMPLEMENT>{Player UpdateHandCards}
	m_kHandCards = tHandCards;
	UpdateCategories();
///	</CODE-IMPLEMENT>{Player UpdateHandCards}
}
void TexasPokerHallPlayerC::C_RecEnterPokerTable(const EnterTableRes& tResult)
{
///	<CODE-IMPLEMENT>{Player RecEnterPokerTable}
	if(VE_SUCCEEDED(tResult))
	{
		UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
		if(!VeStrcmp(pkMainLayer->GetName(), "hall"))
		{
			static_cast<Hall*>(pkMainLayer)->TurnGame();
		}
		else if(!VeStrcmp(pkMainLayer->GetName(), "game"))
		{
			g_pLayerManager->PopAllLayers();
			g_pLayerManager->PopAllMiddleLayers();
			static_cast<Game*>(pkMainLayer)->UpdateTable(false);
		}
	}
	else
	{
		g_pLayerManager->PopAllLayers();
		g_pLayerManager->PopAllMiddleLayers();
		g_pkGame->Toast(TT("EnterGameFailed"));
	}
///	</CODE-IMPLEMENT>{Player RecEnterPokerTable}
}
void TexasPokerHallPlayerC::C_OnTableUpdate()
{
///	<CODE-IMPLEMENT>{Player OnTableUpdate}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->UpdateTable(true);
	}
///	</CODE-IMPLEMENT>{Player OnTableUpdate}
}
void TexasPokerHallPlayerC::C_OnTableUpdateNoMotion()
{
///	<CODE-IMPLEMENT>{Player OnTableUpdateNoMotion}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->UpdateTable(false);
	}
///	</CODE-IMPLEMENT>{Player OnTableUpdateNoMotion}
}
void TexasPokerHallPlayerC::C_OnBuy(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To)
{
///	<CODE-IMPLEMENT>{Player OnBuy}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->OnBuy(u8Type, u8From, u8To);
	}
///	</CODE-IMPLEMENT>{Player OnBuy}
}
void TexasPokerHallPlayerC::C_OnBuyAll(VeUInt8 u8Type, VeUInt8 u8From)
{
///	<CODE-IMPLEMENT>{Player OnBuyAll}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->OnBuyAll(u8Type, u8From);
	}
///	</CODE-IMPLEMENT>{Player OnBuyAll}
}
void TexasPokerHallPlayerC::C_OnChat(VeUInt8 u8Type, VeUInt8 u8From, VeUInt8 u8To, const VeChar8* strChat)
{
///	<CODE-IMPLEMENT>{Player OnBuy}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->OnChat(u8Type, u8From, u8To, strChat);
	}
///	</CODE-IMPLEMENT>{Player OnBuy}
}
void TexasPokerHallPlayerC::C_OnChatAll(VeUInt8 u8Type, VeUInt8 u8From, const VeChar8* strChat)
{
///	<CODE-IMPLEMENT>{Player OnBuyAll}
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), "game"))
	{
		static_cast<Game*>(pkMainLayer)->OnChatAll(u8Type, u8From, strChat);
	}
///	</CODE-IMPLEMENT>{Player OnBuyAll}
}
void TexasPokerHallPlayerC::C_OnPurchaseCompleted(const VeChar8* strChannel, VeUInt32 u32Chips)
{
///	<CODE-IMPLEMENT>{Player OnPurchaseCompleted}
	VeString kContent = TT("Purchase0");
	kContent += TT(VeStringA(strChannel) + "_Purchase");
	kContent += TT("Purchase1");
	kContent += VeStringA::From(u32Chips);
	kContent += TT("Purchase2");
	g_pkGame->Toast(kContent, false);
	m_kLastPurchase = kContent;
	VeVector< VePair<VeStringA,VeStringA> > kValue;
	kValue.PushBack(VePair<VeStringA,VeStringA>("channel", strChannel));
	kValue.PushBack(VePair<VeStringA,VeStringA>("chips", VeStringA::From(u32Chips)));
	NativeOnEvent("EventPurchase", kValue);
///	</CODE-IMPLEMENT>{Player OnPurchaseCompleted}
}
void TexasPokerHallPlayerC::C_RecRefreshPurchase(const RefreshPurchaseRes& tResult)
{
///	<CODE-IMPLEMENT>{Player RecRefreshPurchase}
	if(VE_SUCCEEDED(tResult))
	{
		g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
		g_pkGame->ShowPurchaseAlert(true, m_kLastPurchase);
		g_pkGame->EndRefreshPurchase();
	}
///	</CODE-IMPLEMENT>{Player RecRefreshPurchase}
}
void TexasPokerHallPlayerC::C_RecFinishEvent(const EventAwardType& tType, VeUInt32 u32Value)
{
///	<CODE-IMPLEMENT>{Player RecFinishEvent}
	VeVector< VePair<VeStringA,VeStringA> > kValue;
	kValue.Resize(2);
	kValue[0].m_tFirst = "type";
	kValue[1].m_tFirst = "value";
	g_pLayerManager->PopAllLayers();
	g_pLayerManager->PopAllMiddleLayers();
	g_pkGame->Toast(TT("EventFinish"), false);
	switch(tType)
	{
	case EVENT_AWARD_CHIPS:
		kValue[0].m_tSecond = "chips";
		kValue[1].m_tSecond = VeStringA::From(u32Value);
		g_pSoundSystem->PlayEffect(SE_GET_CHIPS);
		g_pkGame->Toast(VeStringA(TT("EventAwardChips")) + VeStringA::From(u32Value), false);
		break;
	case EVENT_AWARD_CARD:
		kValue[0].m_tSecond = "card";
		kValue[1].m_tSecond = "TestBankDesc" + VeStringA::From(u32Value);
		if(u32Value > 1)
		{
			g_pkGame->Toast(VeStringA(TT("EventAwardCardUp")) + TT("TestBankDesc" + VeStringA::From(u32Value)), false);
		}
		else
		{
			g_pkGame->Toast(VeStringA(TT("EventAwardCardNew")) + TT("TestBankDesc1"), false);
		}
		g_pSoundSystem->PlayEffect(SE_BANK_UP);
		break;
	case EVENT_AWARD_VIP:
		kValue[0].m_tSecond = "vip";
		kValue[1].m_tSecond = VeStringA::From(u32Value);
		g_pkGame->Toast(TT("EventAwardVip"), false);
		g_pSoundSystem->PlayEffect(SE_POKER_WIN_2);
		break;
	case EVENT_AWARD_GOLD:
		kValue[0].m_tSecond = "gold";
		kValue[1].m_tSecond = VeStringA::From(u32Value);
		g_pkGame->Toast(TT("EventAwardGold"), false);
		g_pSoundSystem->PlayEffect(SE_GOLD);
		break;
	default:
		break;
	}
	NativeOnEvent("EventFinish", kValue);
///	</CODE-IMPLEMENT>{Player RecFinishEvent}
}
void TexasPokerHallPlayerC::C_RecNotice(const VeChar8* strNotice, VeUInt16 u16Show, VeUInt16 u16Interval)
{
///	<CODE-IMPLEMENT>{Player RecNotice}
	g_pkGame->ShowNotice(strNotice, u16Show, u16Interval);
///	</CODE-IMPLEMENT>{Player RecNotice}
}
///	</CODE-GENERATE>{EntityClientFuncImpl Player}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerC::GetSingleBet()
{
	return VeUInt32(10 * VE_MIN(200, VE_MAX(1, (VeUInt64(m_u64Chips) / 10000))));
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerC::GetKenoIssue()
{
	return m_u32KenoIssue;
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallPlayerC::GetKenoDrawTime()
{
	return m_u64KenoDrawTime;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerC::GetBaccIndex()
{
	return m_u32BaccIndex;
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallPlayerC::GetBaccExpireTime()
{
	return m_u64BaccExpireTime;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerC::GetBaccBetPlayer()
{
	return m_u32BaccBetPlayer;
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallPlayerC::GetBaccAllBet()
{
	return m_u64BaccAllBet;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerC::GetBaccBetLimit()
{
	return GetSingleBet() * 30;
}
//--------------------------------------------------------------------------
const RoomInfo& TexasPokerHallPlayerC::GetRoomInfo()
{
	return m_kRoomInfo;
}
//--------------------------------------------------------------------------
VeUInt8 TexasPokerHallPlayerC::GetSeatIndex()
{
	VE_ASSERT(m_u32SeatIndex < 9);
	return VeUInt8(m_u32SeatIndex);
}
//--------------------------------------------------------------------------
const Table& TexasPokerHallPlayerC::GetTableState()
{
	return m_kTable;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallPlayerC::GetTablePool()
{
	return m_u32TablePool;
}
//--------------------------------------------------------------------------
const VeVector<TablePlayer>& TexasPokerHallPlayerC::GetTablePlayers()
{
	return m_kPlayers;
}
//--------------------------------------------------------------------------
const TablePlayer* TexasPokerHallPlayerC::GetTablePlayer(VeUInt32 u32Index)
{
	for(VeUInt32 i(0); i < m_kPlayers.Size(); ++i)
	{
		if(m_kPlayers[i].m_u32Index == u32Index)
		{
			return &(m_kPlayers[i]);
		}
	}
	return NULL;
}
//--------------------------------------------------------------------------
const VeVector<CardData>& TexasPokerHallPlayerC::GetHandCards()
{
	return m_kHandCards;
}
//--------------------------------------------------------------------------
Categories TexasPokerHallPlayerC::GetCategories()
{
	return m_eCategories;
}
//--------------------------------------------------------------------------
Categories TexasPokerHallPlayerC::GetCategories(VeUInt32 u32Index)
{
	if(u32Index < m_kPlayerCategories.Size())
	{
		return m_kPlayerCategories[u32Index];
	}
	return Categories_MAX;
}
//--------------------------------------------------------------------------
void TexasPokerHallPlayerC::UpdateCategories()
{
	m_eCategories = Categories_MAX;
	VeZeroMemory(m_abHighLight, sizeof(m_abHighLight));
	m_kPlayerCategories.Clear();
	if(m_kTable.m_tState > TABLE_DEAL_0)
	{
		const TablePlayer* kPlayer = GetTablePlayer(GetSeatIndex());
		if(kPlayer)
		{
			if(kPlayer->m_tState >= TABLE_PLAYER_PLAY &&  kPlayer->m_tState != TABLE_PLAYER_FOLD)
			{
				m_eCategories = CalculateCardPower(m_kHandCards, m_kTable.m_tCardList, m_abHighLight).m_tFirst;
			}
		}
	}
	VeVector<CardData> kHand;
	kHand.Resize(2);
	bool abHighLight[7];
	if(m_kTable.m_tHandCardList.Size())
	{
		for(VeUInt32 i(0); i < (m_kTable.m_tHandCardList.Size() - 1); i += 2)
		{
			kHand[0] = m_kTable.m_tHandCardList[i];
			kHand[1] = m_kTable.m_tHandCardList[i + 1];
			m_kPlayerCategories.PushBack(CalculateCardPower(kHand, m_kTable.m_tCardList, abHighLight).m_tFirst);
		}
	}
}
//--------------------------------------------------------------------------
bool TexasPokerHallPlayerC::IsHighLight(VeUInt32 u32Index)
{
	if(u32Index < 7)
	{
		return m_abHighLight[u32Index];
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------
const VeVector<VeUInt32>& TexasPokerHallPlayerC::GetDrinkPrice()
{
	return m_kDrinkPrice;
}
//--------------------------------------------------------------------------
const VeVector<VeUInt32>& TexasPokerHallPlayerC::GetObjectPrice()
{
	return m_kObjectPrice;
}
//--------------------------------------------------------------------------
