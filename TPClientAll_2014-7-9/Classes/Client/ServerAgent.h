#pragma once

#include "Client.h"

class EntityC;

class ServerAgent : public VeMemObject
{
public:
	ServerAgent(Connection* pkParent);

	virtual ~ServerAgent();

	void SetServerID(const SystemAddress& kAddress);

	bool IsServerConnected();

	void EntityUpdate(BitStream& kStream);

	void OnEntityRPCCallback(BitStream& kStream);

	void EntityDestory(BitStream& kStream);

	EntityC* GetEntity(const VeChar8* pcName);

	virtual void OnConnect() = 0;

	virtual void OnDisconnect() = 0;

	virtual void OnGlobalRPCCallback(BitStream& kStream) = 0;

	virtual EntityC* NewEntity(const VeChar8* pcName) = 0;

protected:
	void Reset();

	bool SendPacket(const BitStream& kStream);

	friend class Client;
	friend class EntityC;
	VeStringMap<EntityC*> m_kEntityMap;
	VeRefList<EntityC*> m_kEntityList;
	Connection* m_pkParent;
	SystemAddress m_kServerID;
	bool m_bServerConnected;

};
