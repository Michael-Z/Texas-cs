#include "ServerAgent.h"
#include "EntityC.h"

//--------------------------------------------------------------------------
ServerAgent::ServerAgent(Connection* pkParent) : m_pkParent(pkParent)
{
	Reset();
}
//--------------------------------------------------------------------------
ServerAgent::~ServerAgent()
{
	Reset();
}
//--------------------------------------------------------------------------
void ServerAgent::SetServerID(const SystemAddress& kAddress)
{
	if(m_kServerID != kAddress)
	{
		Reset();
		m_kServerID = kAddress;
		m_bServerConnected = m_kServerID != UNASSIGNED_SYSTEM_ADDRESS;
	}
}
//--------------------------------------------------------------------------
bool ServerAgent::IsServerConnected()
{
	return m_bServerConnected;
}
//--------------------------------------------------------------------------
void ServerAgent::Reset()
{
	m_kServerID = UNASSIGNED_SYSTEM_ADDRESS;
	m_bServerConnected = false;
	m_kEntityMap.Clear();
	m_kEntityList.BeginIterator();
	while(!m_kEntityList.IsEnd())
	{
		EntityC* pkEntity = m_kEntityList.GetIterNode()->m_tContent;
		m_kEntityList.Next();
		VE_SAFE_DELETE(pkEntity);
	}
	VE_ASSERT(m_kEntityList.Empty());
}
//--------------------------------------------------------------------------
bool ServerAgent::SendPacket(const BitStream& kStream)
{
	if(IsServerConnected())
	{
		VE_ASSERT(m_pkParent && m_pkParent->m_pkPeer);
		m_pkParent->m_pkPeer->Send(&kStream,
			IMMEDIATE_PRIORITY,RELIABLE_ORDERED, 0, m_kServerID, false);
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------
void ServerAgent::EntityUpdate(BitStream& kStream)
{
	VEString kName;
	kStream >> kName;
	EntityC* pkEntity(NULL);
	VeStringMap<EntityC*>::iterator it = m_kEntityMap.Find(kName);
	if(it)
	{
		pkEntity = *it;
	}
	else
	{
		pkEntity = NewEntity(kName);
		m_kEntityMap[kName] = pkEntity;
		m_kEntityList.AttachBack(pkEntity->m_kNode);
		
	}
	pkEntity->Update(kStream);
	if(!it) pkEntity->OnLoad();
	VeDebugOutputString("EntityUpdated");
}
//--------------------------------------------------------------------------
void ServerAgent::OnEntityRPCCallback(BitStream& kStream)
{
	VEString kName;
	kStream >> kName;
	VeStringMap<EntityC*>::iterator it = m_kEntityMap.Find(kName);
	if(it)
	{
		(*it)->OnRPCCallback(kStream);
	}
}
//--------------------------------------------------------------------------
void ServerAgent::EntityDestory(BitStream& kStream)
{
	VEString kName;
	kStream >> kName;
	VeStringMap<EntityC*>::iterator it = m_kEntityMap.Find(kName);
	if(it)
	{
		(*it)->OnUnload();
		m_kEntityMap.Remove(kName);
		VE_SAFE_DELETE(*it);
	}
}
//--------------------------------------------------------------------------
EntityC* ServerAgent::GetEntity(const VeChar8* pcName)
{
	VeStringMap<EntityC*>::iterator it = m_kEntityMap.Find(pcName);
	return it ? *it : NULL;
}
//--------------------------------------------------------------------------
