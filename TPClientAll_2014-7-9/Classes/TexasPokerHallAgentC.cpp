#include "TexasPokerHallAgentC.h"
#include "TexasPoker.h"
#include "UIAlert.h"
#include "Login.h"
#include "Hall.h"

///	<CODE-GENERATE>{EntitiesClientInclude}
#include "TexasPokerHallPlayerC.h"
///	</CODE-GENERATE>{EntitiesClientInclude}

//--------------------------------------------------------------------------
TexasPokerHallAgentC* g_pkHallAgent = NULL;
//--------------------------------------------------------------------------
TexasPokerHallAgentC::TexasPokerHallAgentC(Connection* pkParent)
	: ServerAgent(pkParent)
{

}
//--------------------------------------------------------------------------
TexasPokerHallAgentC::~TexasPokerHallAgentC()
{

}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallAgentC::GetName()
{
	return TexasPokerHallClient::GetName();
}
//--------------------------------------------------------------------------
TexasPokerHallClient* TexasPokerHallAgentC::GetClient()
{
	return static_cast<TexasPokerHallClient*>(m_pkParent);
}
//--------------------------------------------------------------------------
void TexasPokerHallAgentC::OnConnect()
{
	/*UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(!VeStrcmp(pkMainLayer->GetName(), Login::GetName()))
	{
	TexasPoker::Language eLan = g_pkGame->GetLanguage();
	switch(eLan)
	{
	case TexasPoker::zh_CN:
	S_ReqNotice(LAN_zh_CN);
	break;
	case TexasPoker::zh_TW:
	S_ReqNotice(LAN_zh_TW);
	break;
	default:
	S_ReqNotice(LAN_en_US);
	break;
	}
	}
	else
	{
		g_pkGame->LoginToHallServer();
	}*/
	g_pkGame->LoginToHallServer();
	g_pkHallAgent = this;
}
//--------------------------------------------------------------------------
void TexasPokerHallAgentC::OnDisconnect()
{

}
//--------------------------------------------------------------------------
EntityC* TexasPokerHallAgentC::NewEntity(const VeChar8* pcName)
{
///	<CODE-GENERATE>{EntitiesClientCreate}
	TexasPokerHallClient::Entity eEnt = GetClient()->EntNameToType(pcName);
	switch(eEnt)
	{
	case TexasPokerHallClient::ENTITY_Player:
		return VE_NEW TexasPokerHallPlayerC(this);
	default:
		return NULL;
	}
///	</CODE-GENERATE>{EntitiesClientCreate}
}
//--------------------------------------------------------------------------
void TexasPokerHallAgentC::OnGlobalRPCCallback(BitStream& kStream)
{
///	<CODE-GENERATE>{GlobalClientRPCCallback}
	StreamErrorClear();
	VeUInt8 u8FuncID;
	kStream >> u8FuncID;
	switch(u8FuncID)
	{
	case CLIENT_RecLogin:
		{
			LoginResult tResult;
			kStream >> tResult;
			if(!IsStreamError()) C_RecLogin(tResult);
		}
		break;
	case CLIENT_RecSyncTime:
		{
			VeUInt64 u64Time;
			kStream >> u64Time;
			if(!IsStreamError()) C_RecSyncTime(u64Time);
		}
		break;
	default:
		break;
	}
///	</CODE-GENERATE>{GlobalClientRPCCallback}
}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{EntitiesClientGetterImpl}
TexasPokerHallPlayerC* TexasPokerHallAgentC::GetPlayer()
{
	EntityC* pkEnt = GetEntity("Player");
	return static_cast<TexasPokerHallPlayerC*>(pkEnt);
}
///	</CODE-GENERATE>{EntitiesClientGetterImpl}
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{GlobalServerFuncSend}
void TexasPokerHallAgentC::S_ReqLogin(const VeChar8* strPlayerUUID, const VeChar8* strChannel, const VeChar8* strDeviceName, const LanguageS& tLanguage, VeUInt8 u8VersionMajor, VeUInt8 u8VersionMinor)
{
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_GLOBAL_RPC);
	kStream << VeUInt8(SERVER_ReqLogin);
	kStream << strPlayerUUID;
	kStream << strChannel;
	kStream << strDeviceName;
	kStream << tLanguage;
	kStream << u8VersionMajor;
	kStream << u8VersionMinor;
	SendPacket(kStream);
}
void TexasPokerHallAgentC::S_ReqSyncTime()
{
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_GLOBAL_RPC);
	kStream << VeUInt8(SERVER_ReqSyncTime);
	SendPacket(kStream);
}
///	</CODE-GENERATE>{GlobalServerFuncSend}
//--------------------------------------------------------------------------
class HallLoginFailed : public UIAlertConfirm::Callback
{
public:
	HallLoginFailed(LoginResult eRes)
	{
		switch(eRes)
		{
		case LOGIN_EXIST:
			m_kTitle = TT("LoginExistTitle");
			m_kContent = TT("LoginExistContent");
			break;
		case LOGIN_WRONG_VERSION:
			m_kTitle = TT("LoginWrongTitle");
			m_kContent = TT("LoginWrongContent");
			break;
		default:
			m_kTitle = TT("LoginTitle");
			m_kContent = TT("LoginContent");
			break;
		}
		m_kEnter = TT("Confirm");
	}

	virtual void OnConfirm()
	{
		g_pkGame->LoginToHallServer();
	}

};
//--------------------------------------------------------------------------
///	<CODE-GENERATE>{GlobalClientFuncImpl}
void TexasPokerHallAgentC::C_RecLogin(const LoginResult& tResult)
{
///	<CODE-IMPLEMENT>{RecLogin}
	if(tResult <= LOGIN_CREATE)
	{
		S_ReqSyncTime();
		UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
		if(!VeStrcmp(pkMainLayer->GetName(), Login::GetName()))
		{
			static_cast<Login*>(pkMainLayer)->IntoHall();
		}
		else
		{
			GetPlayer()->S_ReqInitHall();
			UILayer* pkLayer = g_pLayerManager->GetCurrentMainLayer();
			if(pkLayer)
			{
				pkLayer->OnForceUpdate();
			}
		}
	}
	else
	{
		g_pkGame->ShowAlert(VE_NEW HallLoginFailed(tResult));
	}
///	</CODE-IMPLEMENT>{RecLogin}
}
void TexasPokerHallAgentC::C_RecSyncTime(VeUInt64 u64Time)
{
///	<CODE-IMPLEMENT>{RecSyncTime}
	g_pkGame->UpdateServerTime(u64Time);
///	</CODE-IMPLEMENT>{RecSyncTime}
}
///	</CODE-GENERATE>{GlobalClientFuncImpl}
//--------------------------------------------------------------------------
