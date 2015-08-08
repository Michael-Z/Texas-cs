#include "TexasPokerHallAgentS.h"

///	<CODE-GENERATE>{EntitiesServerInclude}
#include "TexasPokerHallPlayerS.h"
///	</CODE-GENERATE>{EntitiesServerInclude}

//--------------------------------------------------------------------------
TexasPokerHallAgentS::TexasPokerHallAgentS(Server* pkParent,
	const SystemAddress& kAddress)
	: ClientAgent(pkParent, kAddress)
{
	m_eLoginLanguage = LanguageS_MAX;
}
//--------------------------------------------------------------------------
TexasPokerHallAgentS::~TexasPokerHallAgentS()
{

}
//--------------------------------------------------------------------------
TexasPokerHallServer* TexasPokerHallAgentS::GetServer()
{
	return static_cast<TexasPokerHallServer*>(m_pkParent);
}
//--------------------------------------------------------------------------
void TexasPokerHallAgentS::OnConnect()
{

}
//--------------------------------------------------------------------------
void TexasPokerHallAgentS::OnDisconnect()
{

}
//--------------------------------------------------------------------------
void TexasPokerHallAgentS::OnGlobalRPCCallback(BitStream& kStream)
{
///	<CODE-GENERATE>{GlobalServerRPCCallback}
	StreamErrorClear();
	VeUInt8 u8FuncID;
	kStream >> u8FuncID;
	switch(u8FuncID)
	{
	case SERVER_ReqLogin:
		{
			VEString strPlayerUUID;
			kStream >> strPlayerUUID;
			VEString strChannel;
			kStream >> strChannel;
			VEString strDeviceName;
			kStream >> strDeviceName;
			LanguageS tLanguage;
			kStream >> tLanguage;
			VeUInt8 u8VersionMajor;
			kStream >> u8VersionMajor;
			VeUInt8 u8VersionMinor;
			kStream >> u8VersionMinor;
			if(!IsStreamError()) S_ReqLogin(strPlayerUUID, strChannel, strDeviceName, tLanguage, u8VersionMajor, u8VersionMinor);
		}
		break;
	case SERVER_ReqSyncTime:
		{
			if(!IsStreamError()) S_ReqSyncTime();
		}
		break;
	default:
		break;
	}
///	</CODE-GENERATE>{GlobalServerRPCCallback}
}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{EntitiesServerGetterImpl}
TexasPokerHallPlayerS* TexasPokerHallAgentS::GetPlayer()
{
	EntityS* pkEnt = GetEntity("Player");
	return static_cast<TexasPokerHallPlayerS*>(pkEnt);
}
///	</CODE-GENERATE>{EntitiesServerGetterImpl}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{GlobalClientFuncSend}
void TexasPokerHallAgentS::C_RecLogin(const LoginResult& tResult)
{
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_GLOBAL_RPC);
	kStream << VeUInt8(CLIENT_RecLogin);
	kStream << tResult;
	SendPacket(kStream);
}
void TexasPokerHallAgentS::C_RecSyncTime(VeUInt64 u64Time)
{
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_GLOBAL_RPC);
	kStream << VeUInt8(CLIENT_RecSyncTime);
	kStream << u64Time;
	SendPacket(kStream);
}
///	</CODE-GENERATE>{GlobalClientFuncSend}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{GlobalServerFuncImpl}
void TexasPokerHallAgentS::S_ReqLogin(const VeChar8* strPlayerUUID, const VeChar8* strChannel, const VeChar8* strDeviceName, const LanguageS& tLanguage, VeUInt8 u8VersionMajor, VeUInt8 u8VersionMinor)
{
///	<CODE-IMPLEMENT>{ReqLogin}
	m_eLoginLanguage = tLanguage;
	if(GetServer()->IsMatch(u8VersionMajor, u8VersionMinor))
	{
		union
		{
			EntityS* pkEntity;
			TexasPokerHallPlayerS* pkPlayer;
		};
		pkEntity = NULL;
		LoadEntResult eRes = AttachEntity("Player", strPlayerUUID, &pkEntity);
		switch(eRes)
		{
		case LOAD_ENT_S_OK:
			if(pkPlayer->NeedName())
			{
				pkPlayer->m_strNickname = strDeviceName;
				pkPlayer->m_strChannel = strChannel;
				pkPlayer->SyncToClient();
				C_RecLogin(LOGIN_CREATE);
			}
			else
			{
				pkPlayer->m_strChannel = strChannel;
				pkPlayer->SyncToClient();
				C_RecLogin(LOGIN_SUCCEED);
			}
			break;
		case LOAD_ENT_EXIST:
			C_RecLogin(LOGIN_SUCCEED);
			break;
		case LOAD_ENT_ATTACHED:
			C_RecLogin(LOGIN_EXIST);
			break;
		default:
			C_RecLogin(LOGIN_FAILED);
			break;
		}
	}
	else
	{
		C_RecLogin(LOGIN_WRONG_VERSION);
	}
///	</CODE-IMPLEMENT>{ReqLogin}
}
void TexasPokerHallAgentS::S_ReqSyncTime()
{
///	<CODE-IMPLEMENT>{ReqSyncTime}
	C_RecSyncTime(GetServer()->GetTime());
///	</CODE-IMPLEMENT>{ReqSyncTime}
}
///	</CODE-GENERATE>{GlobalServerFuncImpl}
//--------------------------------------------------------------------------
