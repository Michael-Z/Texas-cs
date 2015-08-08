#pragma once

#include "TexasPokerHallServer.h"

///	<CODE-GENERATE>{EntitiesServerDecl}
class TexasPokerHallPlayerS;
///	</CODE-GENERATE>{EntitiesServerDecl}

class TexasPokerHallAgentS : public ClientAgent
{
public:
///	<CODE-GENERATE>{GlobalFuncEnum}
	enum ServerRPCFunc
	{
		SERVER_ReqLogin,
		SERVER_ReqSyncTime,
		SERVER_MAX
	};
	enum ClientRPCFunc
	{
		CLIENT_RecLogin,
		CLIENT_RecSyncTime,
		CLIENT_MAX
	};
///	</CODE-GENERATE>{GlobalFuncEnum}

	TexasPokerHallAgentS(Server* pkParent, const SystemAddress& kAddress);

	virtual ~TexasPokerHallAgentS();

	TexasPokerHallServer* GetServer();

	virtual void OnConnect();

	virtual void OnDisconnect();

	virtual void OnGlobalRPCCallback(BitStream& kStream);

///	<CODE-GENERATE>{EntitiesServerGetterDecl}
	TexasPokerHallPlayerS* GetPlayer();
///	</CODE-GENERATE>{EntitiesServerGetterDecl}

///	<CODE-GENERATE>{GlobalClientFuncDecl}
	void C_RecLogin(const LoginResult& tResult);
	void C_RecSyncTime(VeUInt64 u64Time);
///	</CODE-GENERATE>{GlobalClientFuncDecl}

///	<CODE-GENERATE>{GlobalServerFuncDecl}
	void S_ReqLogin(const VeChar8* strPlayerUUID, const VeChar8* strChannel, const VeChar8* strDeviceName, const LanguageS& tLanguage, VeUInt8 u8VersionMajor, VeUInt8 u8VersionMinor);
	void S_ReqSyncTime();
///	</CODE-GENERATE>{GlobalServerFuncDecl}

protected:
	LanguageS m_eLoginLanguage;

};
