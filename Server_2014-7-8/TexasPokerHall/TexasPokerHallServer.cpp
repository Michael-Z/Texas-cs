#include "TexasPokerHallServer.h"
#include "TexasPokerHallAgentS.h"
#include "TexasPokerTable.h"

///	<CODE-GENERATE>{EntitiesServerInclude}
#include "TexasPokerHallPlayerS.h"
///	</CODE-GENERATE>{EntitiesServerInclude}

//--------------------------------------------------------------------------
TexasPokerHallServer::TexasPokerHallServer()
	: Server("TexasPokerHall", "file#../Configs")
///	<CODE-GENERATE>{GlobalPropertiesImpl}
	, m_u64JPChips(this, FLAG_DATABASE | FLAG_CLIENT, "JPChips", 800000000)
	, m_i64SlotJackPot(this, FLAG_DATABASE | FLAG_CLIENT, "SlotJackPot")
	, m_i64KenoJackPot(this, FLAG_DATABASE | FLAG_CLIENT, "KenoJackPot")
	, m_u32KenoIssue(this, FLAG_DATABASE | FLAG_CLIENT, "KenoIssue", 1)
	, m_i64BaccaratJackPot(this, FLAG_DATABASE | FLAG_CLIENT, "BaccaratJackPot")
	, m_u32Baccarat(this, FLAG_DATABASE | FLAG_CLIENT, "Baccarat")
	, m_u32EventTick(this, FLAG_DATABASE | FLAG_CLIENT, "EventTick")
	, m_i64PokerJackPot(this, FLAG_DATABASE | FLAG_CLIENT, "PokerJackPot")
///	</CODE-GENERATE>{GlobalPropertiesImpl}
{
///	<CODE-GENERATE>{EntitiesEnumMap}
	m_kEntityNameMap["Player"] = ENTITY_Player;
	m_kEntityNames.PushBack("Player");
///	</CODE-GENERATE>{EntitiesEnumMap}
	TIMER_INIT(TexasPokerHallServer, _TickEvent);
	TIMER_INIT(TexasPokerHallServer, _KenoDraw);
	TIMER_INIT(TexasPokerHallServer, _BaccaratPlay);
	VeZeroMemory(m_au64BaccBetChips, sizeof(m_au64BaccBetChips));
	m_pkTableArray = NULL;
}
//--------------------------------------------------------------------------
TexasPokerHallServer::~TexasPokerHallServer()
{
	OnEnd();
}
//--------------------------------------------------------------------------
TexasPokerHallServer::Entity TexasPokerHallServer::EntNameToType(
	const VeChar8* pcName)
{
	VeUInt32 u32Res = _EntNameToType(pcName);
	return u32Res < ENTITY_MAX ? (Entity)u32Res : ENTITY_MAX;
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallServer::EntTypeToName(Entity eType)
{
	return _EntTypeToName(eType);
}
//--------------------------------------------------------------------------
ClientAgent* TexasPokerHallServer::NewAgent(const SystemAddress& kAddress)
{
	return VE_NEW TexasPokerHallAgentS(this, kAddress);
}
//--------------------------------------------------------------------------
EntityS* TexasPokerHallServer::NewEntity(
	const VeChar8* pcName, const VeChar8* pcIndex)
{
///	<CODE-GENERATE>{EntitiesServerCreate}
	Entity eEnt = EntNameToType(pcName);
	switch(eEnt)
	{
	case ENTITY_Player:
		return VE_NEW TexasPokerHallPlayerS(this, pcIndex);
	default:
		return NULL;
	}
///	</CODE-GENERATE>{EntitiesServerCreate}
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallServer::GetTableName()
{
///	<CODE-GENERATE>{GlobalName}
	return "HallGlobal";
///	</CODE-GENERATE>{GlobalName}
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallServer::GetKeyField()
{
	return "";
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallServer::GetKey()
{
	return "";
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::OnStart()
{
	m_i64PokerJackPot = 0;
	VeDirectory* pkDir = g_pResourceManager->CreateDir("file#../Configs");
	VeBinaryIStreamPtr spConfig = pkDir->OpenSync("AINames.txt");
	VeInt32 i32Len = spConfig->RemainingLength();
	VeChar8* pcBuffer = VeAlloc(VeChar8, i32Len + 1);
	spConfig->Read(pcBuffer, i32Len);
	pcBuffer[i32Len] = 0;
	VeChar8* pcContent;
	VeChar8* pcTemp = VeStrtok(pcBuffer, "\r\n", &pcContent);
	while(pcTemp)
	{
		m_kNameArray.PushBack(pcTemp);
		pcTemp = VeStrtok(NULL, "\r\n", &pcContent);
	}
	VeFree(pcBuffer);
	g_pResourceManager->DestoryDir(pkDir);

	VeXMLElement* pkTemp;

	pkTemp = m_kConfig.RootElement()->FirstChildElement("Rooms")->FirstChildElement();
	while(pkTemp)
	{
		m_kRoomList.Resize(m_kRoomList.Size() + 1);
		RoomInfo& kInfo = m_kRoomList.Back();
		VeUInt32 u32Type = pkTemp->Attribute("type", VeUInt32(RoomType_MAX));
		u32Type = u32Type < RoomType_MAX ? u32Type : RoomType_MAX;
		kInfo.m_tType = (RoomType)u32Type;
		kInfo.m_u32SB = pkTemp->Attribute("sb", 0);
		kInfo.m_u32BB = pkTemp->Attribute("bb", 0);
		kInfo.m_u32ChipsMax = pkTemp->Attribute("max", 0);
		kInfo.m_u32ChipsMin = pkTemp->Attribute("min", kInfo.m_u32ChipsMax);
		kInfo.m_u8Flags = 0;
		if(pkTemp->Attribute("hot", false))
		{
			kInfo.m_u8Flags |= ROOM_FLAG_HOT;
		}
		if(pkTemp->Attribute("vip", false))
		{
			kInfo.m_u8Flags |= ROOM_FLAG_VIP;
		}
		pkTemp = pkTemp->NextSiblingElement();
	}
	m_pkTableArray = VE_NEW VeRefList<TexasPokerTable*>[m_kRoomList.Size()];

	pkTemp = m_kConfig.RootElement()->FirstChildElement("ObjPrice")->FirstChildElement();
	while(pkTemp)
	{
		m_kObjectPrice.PushBack(pkTemp->Attribute("price", 0));
		pkTemp = pkTemp->NextSiblingElement();
	}
	UpdateObjectExchangePrice();

	pkTemp = m_kConfig.RootElement()->FirstChildElement("DrinkPrice")->FirstChildElement();
	while(pkTemp)
	{
		m_kDrinkPrice.PushBack(pkTemp->Attribute("price", 0));
		pkTemp = pkTemp->NextSiblingElement();
	}

	pkTemp = m_kConfig.RootElement()->FirstChildElement("BankCardExpire");
	m_u32BankCardExpire = pkTemp->Attribute("value", 0);

	pkTemp = m_kConfig.RootElement()->FirstChildElement("BonusCard")->FirstChildElement();
	while(pkTemp)
	{
		VeUInt32 u32Value = pkTemp->Attribute("value", 0);
		VeUInt32 u32Purchase = pkTemp->Attribute("purchase", 0);
		m_kBonus.PushBack(VePair<VeUInt32,VeUInt32>(u32Value, u32Purchase));
		pkTemp = pkTemp->NextSiblingElement();
	}

	pkTemp = m_kConfig.RootElement()->FirstChildElement("KenoIssuePeriod");
	m_u32KenoIssuePeriod = pkTemp->Attribute("value", 0);
	AttachByDelay(TIMER(_KenoDraw), m_u32KenoIssuePeriod);

	pkTemp = m_kConfig.RootElement()->FirstChildElement("KenoWin")->FirstChildElement();
	while(pkTemp)
	{
		m_kKenoWin.PushBack(pkTemp->Attribute("rate", 0));
		pkTemp = pkTemp->NextSiblingElement();
	}
	VE_ASSERT(m_kKenoWin.Size() == 11);

	pkTemp = m_kConfig.RootElement()->FirstChildElement("BaccaratPeriod");
	m_u32BaccaratPeriod = pkTemp->Attribute("value", 0);
	AttachByDelay(TIMER(_BaccaratPlay), m_u32BaccaratPeriod);

	pkTemp = m_kConfig.RootElement()->FirstChildElement("PurchaseChannel")->FirstChildElement();
	while(pkTemp)
	{
		m_kPurchaseChannelMap[pkTemp->Value()] = m_kPurchaseChannelList.Size();
		m_kPurchaseChannelList.Resize(m_kPurchaseChannelList.Size() + 1);
		VeHashMap< VeUInt32,VePair<VeUInt32,VeUInt32> >& kMap = m_kPurchaseChannelList.Back();
		VeXMLElement* pkIter = pkTemp->FirstChildElement();
		while(pkIter)
		{
			VeUInt32 u32Price = pkIter->Attribute("value", 0);
			VeUInt32 u32Purchase = pkIter->Attribute("purchase", 0);
			VeUInt32 u32Chips = pkIter->Attribute("chips", 0);
			VE_ASSERT(u32Price && u32Purchase && u32Chips);
			kMap.Insert(u32Price, VePair<VeUInt32,VeUInt32>(u32Purchase, u32Chips));
			pkIter = pkIter->NextSiblingElement();
		}
		pkTemp = pkTemp->NextSiblingElement();
	}

	pkTemp = m_kConfig.RootElement()->FirstChildElement("Vip");
	m_kVipPurchase.m_tFirst = pkTemp->Attribute("once_purchase", 0);
	m_kVipPurchase.m_tSecond = pkTemp->Attribute("total_purchase", 0);

	time_t tTime;
	time(&tTime);
	localtime_s(&m_kLastLocalTime, &tTime);
	AttachByDelay(TIMER(_TickEvent), 100);
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::OnEnd()
{
	for(VeUInt32 i(0); i < m_kRoomList.Size(); ++i)
	{
		VeRefList<TexasPokerTable*>& kList = m_pkTableArray[i];
		kList.BeginIterator();
		while(!kList.IsEnd())
		{
			TexasPokerTable* pkTable = kList.GetIterNode()->m_tContent;
			kList.Next();
			VE_SAFE_DELETE(pkTable);
		}
		VE_ASSERT(kList.Empty());
	}
	VE_SAFE_DELETE_ARRAY(m_pkTableArray);
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::OnUpdate()
{
	
}
//--------------------------------------------------------------------------
const VeVector<RoomInfo>& TexasPokerHallServer::GetRoomList()
{
	return m_kRoomList;
}
//--------------------------------------------------------------------------
const VeVector<VeUInt32>& TexasPokerHallServer::GetObjectSellPrice()
{
	return m_kObjectPrice;
}
//--------------------------------------------------------------------------
const VeVector<VeUInt32>& TexasPokerHallServer::GetObjectExchangePrice()
{
	return m_kObjectExchangePrice;
}
//--------------------------------------------------------------------------
const VeVector<VeUInt32>& TexasPokerHallServer::GetDrinkPrice()
{
	return m_kDrinkPrice;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetBankCardExpire()
{
	return m_u32BankCardExpire;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetBonus(VeUInt32 u32Level)
{
	VE_ASSERT(u32Level > 0 && u32Level <= m_kBonus.Size());
	return m_kBonus[u32Level - 1].m_tFirst;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetBonusPurchaseChips(VeUInt32 u32Level)
{
	VE_ASSERT(u32Level > 0 && u32Level <= m_kBonus.Size());
	return m_kBonus[u32Level - 1].m_tSecond;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::UpdateObjectExchangePrice()
{
	m_kObjectExchangePrice.Resize(m_kObjectPrice.Size());
	for(VeUInt32 i(0); i < m_kObjectExchangePrice.Size(); ++i)
	{
		m_kObjectExchangePrice[i] = m_kObjectPrice[i] >> 1;
	}
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallServer::GetJPChips()
{
	return m_u64JPChips;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::AddJPChips(VeUInt64 u64Chips)
{
	m_u64JPChips += u64Chips;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::DelJPChips(VeUInt64 u64Chips)
{
	if(m_u64JPChips >= u64Chips) m_u64JPChips -= u64Chips;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetKenoIssue()
{
	return m_u32KenoIssue;
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallServer::GetKenoDrawTime()
{
	return TIMER(_KenoDraw)->GetExecuteTime();
}
//--------------------------------------------------------------------------
const VeVector<VeUInt32>& TexasPokerHallServer::GetKenoWin()
{
	return m_kKenoWin;
}
//--------------------------------------------------------------------------
TIMER_IMPL(TexasPokerHallServer, _KenoDraw)
{
	OnKenoDraw();
	AttachByDelay(spEvent, m_u32KenoIssuePeriod);
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::OnKenoDraw()
{
	if(m_kKenoPlayerList.Size())
	{
		VeUInt8 au8Num[10];
		DrawKeno(au8Num);
		VeStringA kKenoResult = "res,";
		for(VeUInt32 i(0); i < 10; ++i)
		{
			kKenoResult += VeStringA::From(au8Num[i]) + ",";
		}
		for(VeUInt32 i(0); i < m_kKenoPlayerList.Size(); ++i)
		{
			Player& kPlayer = m_kKenoPlayerList[i];
			VeStringA kResult = kKenoResult + kPlayer.m_pkPlayer->GetKenoResult();
			kPlayer.m_pkPlayer->GetKenoResult() = kResult;
			kPlayer.m_pkPlayer->SyncToClient();
			kPlayer.m_pkPlayer->ToWild();
		}
		m_kKenoPlayerList.Clear();
	}
	m_u32KenoIssue += VeUInt32(1);
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::AddKenoPlayer(TexasPokerHallPlayerS* pkPlayer,
	VeUInt32 u32BetChips, const VeVector<VeUInt8>& tPlayerNumber)
{
	VeChar8 acBuffer[64];
	VE_ASSERT(pkPlayer && (tPlayerNumber.Size() == 10));
	m_kKenoPlayerList.Resize(m_kKenoPlayerList.Size() + 1);
	Player& kPlayer = m_kKenoPlayerList.Back();
	pkPlayer->ToHome();
	kPlayer.m_pkPlayer = pkPlayer;
	VeStringA kKenoResult = "buy";
	VeSprintf(acBuffer, 64, ",%d", GetKenoIssue());
	kKenoResult += acBuffer;
	for(VeUInt32 i(0); i < 10; ++i)
	{
		kPlayer.m_au8KenoNumbers[i] = tPlayerNumber[i];
		VeSprintf(acBuffer, 64, ",%d", tPlayerNumber[i]);
		kKenoResult += acBuffer;
	}
	VeSprintf(acBuffer, 64, ",bet,%d", u32BetChips);
	kKenoResult += acBuffer;
	pkPlayer->GetKenoResult() = kKenoResult;
	kPlayer.m_u32BetChips = u32BetChips;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::DrawKeno(VeUInt8 au8Num[10])
{
	VeUInt32 au32NotesTag[81] = {0};
	for (VeUInt32 i(0);i < 81;++i)
	{
		au32NotesTag[i] = 0;
	}
	VeUInt32 au32GetAllNum[80] = {0};
	for (VeUInt32 i(0);i < 80;++i)
	{
		au32GetAllNum[i] = i + 1;
	}
	VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
	for (VeUInt32 i(0);i < 80;++i)
	{
		VeUInt32 u32RanNum = i+VeRand()%(80-i);
		VeUInt32 temp = au32GetAllNum[i];
		au32GetAllNum[i] = au32GetAllNum[u32RanNum];
		au32GetAllNum[u32RanNum] = temp;
	}
	for (VeUInt32 i(0);i < 10;++i)
	{
		au8Num[i] = au32GetAllNum[i];
	}
	for(VeUInt32 i(0); i < m_kKenoPlayerList.Size(); ++i)
	{
		VeUInt32 u32Index = 0;
		Player& kPlayer = m_kKenoPlayerList[i];
		for (VeUInt32 j(0);j < 10;++j)
		{
			u32Index = kPlayer.m_au8KenoNumbers[j];
			au32NotesTag[u32Index] += 1;
		}
	}
	VeUInt32 u32Index1 = VeRand()%(4);
	VeUInt32 u32Index2 = 4 + VeRand()%(4);
	VeUInt32 u32Index3 = 8 + VeRand()%(3);
	for (VeUInt32 i(1);i < 81;++i)
	{
		VeUInt32 u32PrizeNumTag = au32NotesTag[i];
		if (u32PrizeNumTag >= m_kKenoPlayerList.Size() * 0.8)
		{
			if (WrongNumCorrect(au8Num,i))
			{
				continue;
			}
			else
			{
				au8Num[u32Index1] = i;
				break;
			}
		}
		else
		{
			continue;
		}
	}
	for (VeUInt32 i(1);i < 81;++i)
	{
		VeUInt32 u32PrizeNumTag = au32NotesTag[i];
		if (u32PrizeNumTag < m_kKenoPlayerList.Size() * 0.8
			&& u32PrizeNumTag >= m_kKenoPlayerList.Size() * 0.5)
		{
			if (WrongNumCorrect(au8Num,i))
			{
				continue;
			}
			else
			{
				au8Num[u32Index2] = i;
				break;
			}
		}
		else
		{
			continue;
		}
	}
	for (VeUInt32 i(1);i < 81;++i)
	{
		VeUInt32 u32PrizeNumTag = au32NotesTag[i];
		if (u32PrizeNumTag < m_kKenoPlayerList.Size() * 0.5 
			&& u32PrizeNumTag >= m_kKenoPlayerList.Size() * 0.3)
		{
			if (WrongNumCorrect(au8Num,i))
			{
				continue;
			}
			else
			{
				au8Num[u32Index3] = i;
				break;
			}
		}
		else
		{
			continue;
		}
	}

	VeUInt32 u32PrizesNumTag = 0;

	for (VeUInt32 i(0);i < m_kKenoPlayerList.Size();++i)
	{
		Player& kPlayer = m_kKenoPlayerList[i];

		m_i64KenoJackPot += VeInt64(kPlayer.m_u32BetChips);
		kPlayer.m_pkPlayer->AddKenoJackPot(VeInt64(kPlayer.m_u32BetChips));
		kPlayer.m_pkPlayer->RecordKenoCount();
		for (VeUInt32 j(0);j < 10;++j)
		{
			for (VeUInt32 k(0);k < 10;++k)
			{
				if (kPlayer.m_au8KenoNumbers[j] == au8Num[k])
				{
					u32PrizesNumTag += 1;
				}
			}
		}
		switch (u32PrizesNumTag)
		{
		case 4:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_4HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_4HITS_PRIZE_POWER));
			break;
		case 5:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_5HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_5HITS_PRIZE_POWER));
			break;
		case 6:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_6HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_6HITS_PRIZE_POWER));
			break;
		case 7:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_7HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_7HITS_PRIZE_POWER));
			break;
		case 8:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_8HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_8HITS_PRIZE_POWER));
			break;
		case 9:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_9HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_9HITS_PRIZE_POWER));
			break;
		case 10:
			m_i64KenoJackPot -= VeInt64(kPlayer.m_u32BetChips * KENO_10HITS_PRIZE_POWER);
			kPlayer.m_pkPlayer->ReduceKenoJackPot(VeInt64(kPlayer.m_u32BetChips * KENO_10HITS_PRIZE_POWER));
			break;
		default:
			break;
		}
		u32PrizesNumTag = 0;
	}
}
//--------------------------------------------------------------------------
bool TexasPokerHallServer::WrongNumCorrect(VeUInt8 au32KenoNum[] ,VeUInt32 u32WrongNum)
{
	for (VeUInt32 i(0);i < 10;++i)
	{
		if (au32KenoNum[i] == u32WrongNum)
		{
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------
VeInt64 TexasPokerHallServer::GetSlotJackPot()
{
	return m_i64SlotJackPot;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::AddSlotJackPot(VeInt64 i64AddNum)
{
	m_i64SlotJackPot += i64AddNum;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::ReduceSlotJackPot(VeInt64 i64AddNum)
{
	m_i64SlotJackPot -= i64AddNum;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::AddBaccJackPot(VeInt64 i64AddNum)
{
	m_i64BaccaratJackPot += i64AddNum;
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::ReduceBaccJackPot(VeInt64 i64ReduceNum)
{
	m_i64BaccaratJackPot -= i64ReduceNum;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetBaccarat()
{
	return m_u32Baccarat;
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallServer::GetBaccaratPlayTime()
{
	return TIMER(_BaccaratPlay)->GetExecuteTime();
}
//--------------------------------------------------------------------------
TIMER_IMPL(TexasPokerHallServer, _BaccaratPlay)
{
	OnBaccaratPlay();
	AttachByDelay(spEvent, m_u32BaccaratPeriod);
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::OnBaccaratPlay()
{
	if(m_kBaccaratPlayerList.Size())
	{
		VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
		BaccaratBetType eRes = DrawBacc();
		VE_ASSERT(eRes < BaccaratBetType_MAX);
		VeUInt32 u32Seed = g_pTime->GetCurrentCount() % 100000;
		VeStringA kKenoResult = "res,";
		kKenoResult += VeStringA::From(VeUInt32(eRes)) + ",";
		kKenoResult += VeStringA::From(u32Seed) + ",";
		m_kBaccaratPlayerList.BeginIterator();
		while(!m_kBaccaratPlayerList.IsEnd())
		{
			TexasPokerHallPlayerS* pkPlayer = m_kBaccaratPlayerList.GetIterNode()->m_tContent;
			m_kBaccaratPlayerList.Next();
			VE_ASSERT(pkPlayer);
			pkPlayer->GetBaccaratResult() = kKenoResult + pkPlayer->GetBaccaratResult();
			pkPlayer->SyncToClient();
			pkPlayer->ToWild();
		}
		m_kBaccaratPlayerList.Clear();
	}
	m_u32Baccarat += VeUInt32(1);
	VeZeroMemory(m_au64BaccBetChips, sizeof(m_au64BaccBetChips));
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::UpdateBaccPlayer(TexasPokerHallPlayerS* pkPlayer,
	BaccaratBetType eType, VeUInt32 u32BetChips)
{
	VE_ASSERT(pkPlayer && eType < BaccaratBetType_MAX);
	if(!(pkPlayer->GetBaccNode().IsAttach(m_kBaccaratPlayerList)))
	{
		pkPlayer->ToHome();
		pkPlayer->GetTickPlayBaccCount() += VeUInt32(1);
		m_kBaccaratPlayerList.AttachBack(pkPlayer->GetBaccNode());
	}
	m_au64BaccBetChips[eType] += u32BetChips;
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetBaccaratBetPlayerNum()
{
	return m_kBaccaratPlayerList.Size();
}
//--------------------------------------------------------------------------
VeUInt64 TexasPokerHallServer::GetBaccaratAllBet()
{
	return m_au64BaccBetChips[BACC_BANKER]
		+ m_au64BaccBetChips[BACC_PLAYER]
		+ m_au64BaccBetChips[BACC_TIE];
}
//--------------------------------------------------------------------------
BaccaratBetType TexasPokerHallServer::DrawBacc()
{
	VeSrand(VeUInt32(g_pTime->GetCurrentCount()));
	VeUInt32 u32CheatRandNum;
	if (m_kBaccaratPlayerList.Size() <= 5)
	{
		u32CheatRandNum = 2;
	}
	else if (m_kBaccaratPlayerList.Size() > 5
	&& m_kBaccaratPlayerList.Size() <= 100)
	{
		u32CheatRandNum = 4;
	}
	else
	{
		u32CheatRandNum = 5;
	}
	VeUInt32 u32CheatRate = VeRand()%(u32CheatRandNum);
	VeUInt32 u32TieRate = VeRand()%(10);
	VeUInt32 u32Rate = VeRand()%(2);
	if (!u32TieRate && m_i64BaccaratJackPot >= 0)
	{
		return BaccaratBetType(BACC_TIE);
	}
	else
	{
		if (u32CheatRate || m_i64BaccaratJackPot <= 0)
		{
			if (m_au64BaccBetChips[BACC_BANKER] > m_au64BaccBetChips[BACC_PLAYER])
			{
				return BaccaratBetType(BACC_PLAYER);
			}
			else if (m_au64BaccBetChips[BACC_BANKER] < m_au64BaccBetChips[BACC_PLAYER])
			{
				return BaccaratBetType(BACC_BANKER);
			}
			else
			{
				if (u32Rate)
				{
					return BaccaratBetType(BACC_BANKER);
				}
				else
				{
					return BaccaratBetType(BACC_PLAYER);
				}
			}
		}
		else
		{
			if (u32Rate)
			{
				return BaccaratBetType(BACC_BANKER);
			}
			else
			{
				return BaccaratBetType(BACC_PLAYER);
			}
		}
	}
}
//--------------------------------------------------------------------------
void TexasPokerHallServer::IntoGameRoom(VeUInt32 u32Index,
	TexasPokerHallPlayerS* pkPlayer, TexasPokerTable* pkExcept)
{
	VE_ASSERT(pkPlayer);
	VE_ASSERT(u32Index < m_kRoomList.Size());
	VeRefList<TexasPokerTable*>& kTableList = m_pkTableArray[u32Index];
	TexasPokerTable* pkTableWanted(NULL);
	kTableList.BeginIterator();
	while(!kTableList.IsEnd())
	{
		TexasPokerTable* pkTable = kTableList.GetIterNode()->m_tContent;
		kTableList.Next();
		if(pkTable != pkExcept && pkTable->GetEmptySeat())
		{
			pkTableWanted = pkTable;
			break;
		}
	}
	if(!pkTableWanted)
	{
		pkTableWanted = VE_NEW TexasPokerTable(u32Index, m_kRoomList[u32Index], this);
		kTableList.AttachFront(*pkTableWanted);
	}
	VE_ASSERT(pkTableWanted && pkTableWanted->GetEmptySeat());
	pkTableWanted->AddPlayer(pkPlayer);
}
//--------------------------------------------------------------------------
VePair<VeUInt32,VeUInt32> TexasPokerHallServer::GetPurchaseInfo(
	const VeChar8* pcChannel, VeUInt32 u32Price)
{
	VeStringMap<VeUInt32>::iterator it = m_kPurchaseChannelMap.Find(pcChannel);
	if(it)
	{
		VeHashMap< VeUInt32,VePair<VeUInt32,VeUInt32> >& kMap = m_kPurchaseChannelList[*it];
		VePair<VeUInt32,VeUInt32>* pkRes = kMap.Find(u32Price);
		if(pkRes) return *pkRes;
	}
	return VePair<VeUInt32,VeUInt32>(0, 0);
}
//--------------------------------------------------------------------------
const VePair<VeUInt32,VeUInt32>& TexasPokerHallServer::GetVipPurchase()
{
	return m_kVipPurchase;
}
//--------------------------------------------------------------------------
TIMER_IMPL(TexasPokerHallServer, _TickEvent)
{
	time_t tTime;
	time(&tTime);
	tm kCurrent;
	localtime_s(&kCurrent, &tTime);
	if(kCurrent.tm_mday != m_kLastLocalTime.tm_mday)
	{
		m_u32EventTick += VeUInt32(1);
	}
	VeMemcpy(&m_kLastLocalTime, &kCurrent, sizeof(tm));
	AttachByDelay(TIMER(_TickEvent), 100);
}
//--------------------------------------------------------------------------
VeUInt32 TexasPokerHallServer::GetEventTick()
{
	return m_u32EventTick;
}
//--------------------------------------------------------------------------
const VeVector<VeStringA>& TexasPokerHallServer::GetNameArray()
{
	return m_kNameArray;
}
//--------------------------------------------------------------------------
VeInt64_S& TexasPokerHallServer::GetPokerJackPot()
{
	return m_i64PokerJackPot;
}
//--------------------------------------------------------------------------
