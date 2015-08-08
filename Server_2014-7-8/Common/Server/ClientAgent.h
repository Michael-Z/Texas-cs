#pragma once

#include "Server.h"

class ClientAgent : public VeMemObject
{
public:
	ClientAgent(Server* pkParent, const SystemAddress& kAddress);

	virtual ~ClientAgent();

	virtual void OnConnect() = 0;

	virtual void OnDisconnect() = 0;

	virtual void OnGlobalRPCCallback(BitStream& kStream) = 0;

	LoadEntResult AttachEntity(const VeChar8* pcName, const VeChar8* pcSearchIndex, EntityS** ppkOut = NULL);

	UnLoadEntResult DetachEntity(const VeChar8* pcName);

	EntityS* GetEntity(const VeChar8* pcName);

	void SyncEntityToClient(const VeChar8* pcName);

	void OnEntityRPCCallback(BitStream& kStream);

	void DetachAllEntities();

	void Disconnect();

protected:
	void SendPacket(const BitStream& kStream);

	friend class Server;
	friend class EntityS;
	VeRefNode<ClientAgent*> m_kNode;
	Server* m_pkParent;
	SystemAddress m_kClientID;
	VeStringMap<EntityS*> m_kEntityMap;
	VeRefList<EntityS*> m_kEntityList;

};
