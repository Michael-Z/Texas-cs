#include "ClientAgent.h"
#include "EntityS.h"

//--------------------------------------------------------------------------
ClientAgent::ClientAgent(Server* pkParent, const SystemAddress& kAddress)
{
	m_kNode.m_tContent = this;
	m_pkParent = pkParent;
	m_kClientID = kAddress;
}
//--------------------------------------------------------------------------
ClientAgent::~ClientAgent()
{

}
//--------------------------------------------------------------------------
void ClientAgent::SendPacket(const BitStream& kStream)
{
	m_pkParent->m_pkPeer->Send(&kStream, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0, m_kClientID, false);
}
//--------------------------------------------------------------------------
LoadEntResult ClientAgent::AttachEntity(const VeChar8* pcName,
	const VeChar8* pcSearchIndex, EntityS** ppkOut)
{
	VeStringMap<EntityS*>::iterator itEnt = m_kEntityMap.Find(pcName);
	if(itEnt)
	{
		if(ppkOut) *ppkOut = *itEnt;
		return LOAD_ENT_ATTACHED_SELF;
	}
	EntityS* pkEnt(NULL);
	LoadEntResult eRes = m_pkParent->LoadEntity(pcName, pcSearchIndex, &pkEnt);
	if(eRes == LOAD_ENT_S_OK)
	{
		if(pkEnt)
		{
			pkEnt->AttachAgent(this);
		}
		else
		{
			if(ppkOut) *ppkOut = pkEnt;
			return LOAD_ENT_E_FAIL;
		}
	}
	else if(eRes == LOAD_ENT_EXIST)
	{
		if(pkEnt)
		{
			if(pkEnt->HasAgent())
			{
				if(ppkOut) *ppkOut = pkEnt;
				return LOAD_ENT_ATTACHED;
			}
			else
			{
				pkEnt->AttachAgent(this);
			}
		}
		else
		{
			if(ppkOut) *ppkOut = pkEnt;
			return LOAD_ENT_E_FAIL;
		}
	}
	if(ppkOut) *ppkOut = pkEnt;
	return eRes;
}
//--------------------------------------------------------------------------
UnLoadEntResult ClientAgent::DetachEntity(const VeChar8* pcName)
{
	VeStringMap<EntityS*>::iterator it = m_kEntityMap.Find(pcName);
	if(it)
	{
		(*it)->DetachAgent();
		return UNLOAD_ENT_S_OK;
	}
	else
	{
		return UNLOAD_ENT_NOT_EXIST;
	}
	return UNLOAD_ENT_NOT_EXIST;
}
//--------------------------------------------------------------------------
EntityS* ClientAgent::GetEntity(const VeChar8* pcName)
{
	VeStringMap<EntityS*>::iterator it = m_kEntityMap.Find(pcName);
	return it ? *it : NULL;
}
//--------------------------------------------------------------------------
void ClientAgent::SyncEntityToClient(const VeChar8* pcName)
{
	VeStringMap<EntityS*>::iterator it = m_kEntityMap.Find(pcName);
	if(it)
	{
		(*it)->SyncToClient();
	}
}
//--------------------------------------------------------------------------
void ClientAgent::OnEntityRPCCallback(BitStream& kStream)
{
	VEString kName;
	kStream >> kName;
	VeStringMap<EntityS*>::iterator it = m_kEntityMap.Find(kName);
	if(it)
	{
		(*it)->OnRPCCallback(kStream);
	}
}
//--------------------------------------------------------------------------
void ClientAgent::DetachAllEntities()
{
	m_kEntityList.BeginIterator();
	while(!m_kEntityList.IsEnd())
	{
		EntityS* pkEntity = m_kEntityList.GetIterNode()->m_tContent;
		m_kEntityList.Next();
		VE_ASSERT(pkEntity);
		pkEntity->DetachAgent();
	}
	m_kEntityList.Clear();
	m_kEntityMap.Clear();
}
//--------------------------------------------------------------------------
void ClientAgent::Disconnect()
{
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_CLIENT_FORCE_DISCONNECT);
	SendPacket(kStream);
}
//--------------------------------------------------------------------------
