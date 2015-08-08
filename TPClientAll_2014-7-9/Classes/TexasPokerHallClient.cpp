#include "TexasPokerHallClient.h"
#include "TexasPokerHallAgentC.h"
#include "TexasPoker.h"
#include "UIAlert.h"
#include "Login.h"

//--------------------------------------------------------------------------
TexasPokerHallClient::TexasPokerHallClient()
	: Connection(TexasPokerHallClient::GetName())
{
///	<CODE-GENERATE>{EntitiesEnumMap}
	m_kEntityNameMap["Player"] = ENTITY_Player;
	m_kEntityNames.PushBack("Player");
///	</CODE-GENERATE>{EntitiesEnumMap}
}
//--------------------------------------------------------------------------
TexasPokerHallClient::~TexasPokerHallClient()
{

}
//--------------------------------------------------------------------------
TexasPokerHallClient::Entity TexasPokerHallClient::EntNameToType(
	const VeChar8* pcName)
{
	VeUInt32 u32Res = _EntNameToType(pcName);
	return u32Res < ENTITY_MAX ? (Entity)u32Res : ENTITY_MAX;
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallClient::EntTypeToName(Entity eType)
{
	return _EntTypeToName(eType);
}
//--------------------------------------------------------------------------
const VeChar8* TexasPokerHallClient::GetName()
{
	return "TexasPokerHall";
}
//--------------------------------------------------------------------------
ServerAgent* TexasPokerHallClient::NewAgent()
{
	return VE_NEW TexasPokerHallAgentC(this);
}
//--------------------------------------------------------------------------
class HallConnectFailed : public UIAlertConfirm::Callback
{
public:
	HallConnectFailed(Connection::ConnectFail eFail)
	{
		switch(eFail)
		{
		case Connection::CONNECT_FAIL_NORMAL:
			m_kTitle = TT("ConnectTitle");
			break;
		case Connection::CONNECT_FAIL_SERVER_FULL:
			m_kTitle = TT("ConnectTitleFull");
			break;
		default:
			m_kTitle = TT("ConnectTitleLost");
			break;
		}
		m_kContent = TT("ConnectContent");
		m_kEnter = TT("Confirm");
	}

	virtual void OnConfirm()
	{
		g_pkGame->ConnectToHallServer();
	}
};
//--------------------------------------------------------------------------
void TexasPokerHallClient::OnConnectFailed(ConnectFail eFail)
{
	g_pkHallAgent = NULL;
	if(eFail != CONNECT_TURN_OFF)
	{
		UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
		if(!VeStrcmp(pkMainLayer->GetName(), Login::GetName()))
		{
			Login* pkLogin = static_cast<Login*>(pkMainLayer);
			pkLogin->HideStart();
		}
		g_pkGame->ShowAlert(VE_NEW HallConnectFailed(eFail));
	}
}
//--------------------------------------------------------------------------
