#pragma once

#include "TexasPokerHallClient.h"

///	<CODE-GENERATE>{EntitiesClientDecl}
class TexasPokerHallPlayerC;
///	</CODE-GENERATE>{EntitiesClientDecl}

class TexasPokerHallAgentC : public ServerAgent
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

	TexasPokerHallAgentC(Connection* pkParent);

	virtual ~TexasPokerHallAgentC();

	static const VeChar8* GetName();

	TexasPokerHallClient* GetClient();

	virtual void OnConnect();

	virtual void OnDisconnect();

	virtual void OnGlobalRPCCallback(BitStream& kStream);

///	<CODE-GENERATE>{EntitiesClientGetterDecl}
	TexasPokerHallPlayerC* GetPlayer();
///	</CODE-GENERATE>{EntitiesClientGetterDecl}

	virtual EntityC* NewEntity(const VeChar8* pcName);

///	<CODE-GENERATE>{GlobalServerFuncDecl}
	void S_ReqLogin(const VeChar8* strPlayerUUID, const VeChar8* strChannel, const VeChar8* strDeviceName, const LanguageS& tLanguage, VeUInt8 u8VersionMajor, VeUInt8 u8VersionMinor);
	void S_ReqSyncTime();
///	</CODE-GENERATE>{GlobalServerFuncDecl}

///	<CODE-GENERATE>{GlobalClientFuncDecl}
	void C_RecLogin(const LoginResult& tResult);
	void C_RecSyncTime(VeUInt64 u64Time);
///	</CODE-GENERATE>{GlobalClientFuncDecl}

};

extern TexasPokerHallAgentC* g_pkHallAgent;
