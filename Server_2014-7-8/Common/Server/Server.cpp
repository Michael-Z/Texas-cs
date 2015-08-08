#include "Server.h"
#include "PacketFileLogger.h"
#include "MessageIdentifiers.h"
#include "ClientAgent.h"
#include "EntityS.h"

//--------------------------------------------------------------------------
void VeServerInit()
{
	VeGlobalStringTableA::Create();
	VE_NEW VeAllocatorManager();
	VE_NEW VeStringParser();
	VE_NEW VeTime();
	VeLogger::SetLog(VeLogger::OUTPUT_DEBUGOUTPUT);
	VE_NEW VeResourceManager();
	VE_NEW VeURL();
	VE_NEW ServerManager();
}
//--------------------------------------------------------------------------
void VeServerTerm()
{
	ServerManager::Destory();
	VeURL::Destory();
	VeResourceManager::Destory();
	VeLogger::SetLog(VeLogger::OUTPUT_MAX);
	VeTime::Destory();
	VeStringParser::Destory();
	VeAllocatorManager::Destory();
	VeGlobalStringTableA::Destory();
#ifdef VE_MEM_DEBUG
	_VeMemoryExit();
#endif
}
//--------------------------------------------------------------------------
void VeServerUpdate()
{
	g_pTime->Update();
	g_pURL->Update();
	g_pResourceManager->Update();
}
//--------------------------------------------------------------------------
VeTimeEvent::VeTimeEvent()
{
	m_kNode.m_tContent = this;
	m_pkParent = NULL;
	m_u64ExecuteTime = 0;
}
//--------------------------------------------------------------------------
VeTimeEvent::~VeTimeEvent()
{

}
//--------------------------------------------------------------------------
void VeTimeEvent::AddDelegate(Delegate& kDelegate)
{
	m_kEvent.AddDelegate(kDelegate);
}
//--------------------------------------------------------------------------
VeUInt64 VeTimeEvent::GetExecuteTime()
{
	return m_u64ExecuteTime;
}
//--------------------------------------------------------------------------
void VeTimeEvent::Detach()
{
	if(m_pkParent)
	{
		m_pkParent->Detach(this);
	}
}
//--------------------------------------------------------------------------
bool VeTimeEvent::IsAttach() const
{
	return m_pkParent ? true : false;
}
//--------------------------------------------------------------------------
VeTimer* VeTimeEvent::GetParent()
{
	return m_pkParent;
}
//--------------------------------------------------------------------------
VeTimer::VeTimer()
{

}
//--------------------------------------------------------------------------
VeTimer::~VeTimer()
{

}
//--------------------------------------------------------------------------
void VeTimer::Attach(const VeTimeEventPtr& spEvent, VeUInt64 u64Expire)
{
	spEvent->Detach();
	spEvent->m_pkParent = this;
	m_kEventList.AttachBack(spEvent->m_kNode);
	m_kEventMap.insert(std::make_pair(u64Expire, spEvent));
	spEvent->m_u64ExecuteTime = u64Expire;
	spEvent->IncRefCount();
}
//--------------------------------------------------------------------------
void VeTimer::Detach(const VeTimeEventPtr& spEvent)
{
	VE_ASSERT(spEvent);
	VE_ASSERT(spEvent->m_pkParent == this);
	VE_ASSERT(spEvent->m_kNode.IsAttach(m_kEventList));
	std::multimap<VeUInt64,VeTimeEvent*>::iterator it = m_kEventMap.lower_bound(spEvent->m_u64ExecuteTime);
	while(it != m_kEventMap.upper_bound(spEvent->m_u64ExecuteTime))
	{
		if(it->second == spEvent)
		{
			m_kEventMap.erase(it);
			break;
		}
		++it;
	}
	spEvent->m_pkParent = NULL;
	spEvent->m_kNode.Detach();
	spEvent->DecRefCount();
}
//--------------------------------------------------------------------------
void VeTimer::Tick(VeUInt64 u64Current)
{
	m_kCacheCallback.Clear();
	std::multimap<VeUInt64,VeTimeEvent*>::iterator it = m_kEventMap.begin();
	while(true)
	{
		if((it != m_kEventMap.end()) && (it->first <= u64Current))
		{
			m_kCacheCallback.PushBack(it->second);
		}
		else
		{
			m_kEventMap.erase(m_kEventMap.begin(), it);
			break;
		}
		++it;
	}
	for(VeUInt32 i(0); i < m_kCacheCallback.Size(); ++i)
	{
		m_kCacheCallback[i]->m_kNode.Detach();
		m_kCacheCallback[i]->m_pkParent = NULL;
		m_kCacheCallback[i]->m_kEvent.Callback(m_kCacheCallback[i]);
		m_kCacheCallback[i]->DecRefCount();
	}
}
//--------------------------------------------------------------------------
Server::Server(const VeChar8* pcName, const VeChar8* pcConfigPath)
	: PropertySContainer(this), m_kName(pcName), m_pkPeer(NULL), m_kAgentMap(1024), m_kDatabase(NULL)
	, m_strServerName(this, FLAG_DATABASE, "ServerName", pcName)
	, m_u64ServerTime(this, FLAG_DATABASE, "ServerTime", 0)
{
	m_kNode.m_tContent = this;
	g_pServerManager->AddServer(pcName, this);
	m_pkPeer = VEPeerInterface::GetInstance();

	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "%s.xml", pcName);
	VeDirectory* pkDir = g_pResourceManager->CreateDir(pcConfigPath);
	VeBinaryIStreamPtr spConfig = pkDir->OpenSync(acBuffer);
	(*spConfig) >> m_kConfig;
	g_pResourceManager->DestoryDir(pkDir);

	TIMER_INIT(Server, _SyncToDatabase);
}
//--------------------------------------------------------------------------
Server::~Server()
{
	VEPeerInterface::DestroyInstance(m_pkPeer);
	m_pkPeer = NULL;
}
//--------------------------------------------------------------------------
void Server::Start()
{
	VeXMLElement* pkServer = m_kConfig.RootElement()->FirstChildElement("Server");
	const VeChar8* pcVersion = pkServer->FirstChildElement("Version")->GetText();
	{
		VeZeroMemory(&m_kVersion, sizeof(Version));
		VeChar8 acBuffer[64];
		VeStrcpy(acBuffer, 64, pcVersion);
		VeChar8* pcContext;
		const VeChar8* pcTemp = VeStrtok(acBuffer, ".", &pcContext);
		VeUInt32 i(0);
		while(pcTemp && i < 4)
		{
			m_kVersion.m_au8Version[i] = VeAtoi(pcTemp);
			pcTemp = VeStrtok(NULL, ".", &pcContext);
			++i;
		}
	}
	VeUInt16 u16Port;
	VE_ASSERT_EQ(VeStringA(pkServer->FirstChildElement("ServerPort")->GetText()).To(u16Port), true);
	VeUInt16 u16MaxConnections;
	VE_ASSERT_EQ(VeStringA(pkServer->FirstChildElement("MaxConnections")->GetText()).To(u16MaxConnections), true);
	VeUInt32 u32TimeOut;
	VE_ASSERT_EQ(VeStringA(pkServer->FirstChildElement("TimeOut")->GetText()).To(u32TimeOut), true);
	const VeChar8* pcPassword = pkServer->FirstChildElement("Password")->GetText();

	m_pkPeer->SetIncomingPassword(pcPassword, (VeInt32)VeStrlen(pcPassword));
	m_pkPeer->SetTimeoutTime(u32TimeOut, UNASSIGNED_SYSTEM_ADDRESS);

	SocketDescriptor akSocketDesc[2];
	akSocketDesc[0].port = u16Port;
	akSocketDesc[0].socketFamily = AF_INET;
	akSocketDesc[1].port = u16Port;
	akSocketDesc[1].socketFamily = AF_INET6;

	bool bRes = m_pkPeer->Startup(u16MaxConnections, akSocketDesc, 2, 30) == VENET_STARTED;
	m_pkPeer->SetMaximumIncomingConnections(u16MaxConnections);

	if(bRes)
	{
		printf("%s Server Started, Waiting For Connections.\n", m_kName);
	}	
	else
	{
		bRes = m_pkPeer->Startup(u16MaxConnections, akSocketDesc, 1, 30) == VENET_STARTED;
		if(!bRes)
		{
			VeChar8 acBuffer[256];
			VeSprintf(acBuffer, 256, "%s Server failed to start. Terminating.", m_kName);
			puts(acBuffer);
			exit(1);
		}
		printf("%s Server Started, Waiting For Connections.\n", m_kName);
	}

	m_pkPeer->SetOccasionalPing(true);
	m_pkPeer->SetUnreliableTimeout(1000);

	DataStructures::List< VENetSmartPtr<VENetSocket> > kSockets;
	m_pkPeer->GetSockets(kSockets);
	for(VeUInt32 i(0); i < kSockets.Size(); ++i)
	{
		printf("Ports Used By :%i. %i\n", i+1, kSockets[i]->boundAddress.ToString(true));
	}
	for(VeUInt32 i(0); i < m_pkPeer->GetNumberOfAddresses(); ++i)
	{
		printf("\n本机可提供服务的IP :\n%i. %s\n", i+1, m_pkPeer->GetLocalIP(i));
	}
	printf("%s Server GUID is %s\n", m_kName, m_pkPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString());
	
	{
		VeXMLElement* pkDBLink = m_kConfig.RootElement()->FirstChildElement("DBLink");
		VeStringA kAccount = pkDBLink->FirstChildElement("Account")->GetText();
		VeStringA kPassword = pkDBLink->FirstChildElement("Password")->GetText();
		VeStringA kDBSource = pkDBLink->FirstChildElement("DBSource")->GetText();
        printf("\nDBSource :%s\n", pkDBLink->FirstChildElement("DBSource")->GetText());
		m_kDatabase = VE_NEW DBBusiness(kAccount,kPassword,kDBSource);
	}
	
	LoadProperties();
	m_u64ServerStartTime = m_u64ServerTime;
	AttachByDelay(TIMER(_SyncToDatabase), SAVE_DB_DELAY);
	OnStart();
}
//--------------------------------------------------------------------------
void Server::Update()
{
	Packet* pkPacket = m_pkPeer->Receive();
	while(pkPacket)
	{
		VeUInt32 u32ClientID = SystemAddress::ToInteger(pkPacket->systemAddress);
		BitStream kStream((unsigned char*)pkPacket->data, pkPacket->length, false);
		VeUInt8 u8Event;
		kStream >> u8Event;
		switch(u8Event)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			printf("ID_DISCONNECTION_NOTIFICATION from %s\n", pkPacket->systemAddress.ToString(true));
			Disconnect(u32ClientID);
			break;
		case ID_NEW_INCOMING_CONNECTION:
			{
				printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", pkPacket->systemAddress.ToString(true), pkPacket->guid.ToString());
				
				if(!m_kAgentMap.Find(u32ClientID))
				{
					ClientAgent* pkAgent = NewAgent(pkPacket->systemAddress);
					if(pkAgent)
					{
						m_kAgentMap[u32ClientID] = pkAgent;
						m_kAgentList.AttachBack(pkAgent->m_kNode);
						pkAgent->OnConnect();
					}
				}
			}
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;
		case ID_CONNECTION_LOST:
			printf("ID_CONNECTION_LOST from %s\n", pkPacket->systemAddress.ToString(true));
			Disconnect(u32ClientID);
			break;
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", pkPacket->systemAddress.ToString(true));
			break;
		case ID_SERVER_GLOBAL_RPC:
			{
				ClientAgent** it = m_kAgentMap.Find(u32ClientID);
				if(it)
				{
					(*it)->OnGlobalRPCCallback(kStream);
				}
			}
			break;
		case ID_SERVER_ENTITY_RPC:
			{
				ClientAgent** it = m_kAgentMap.Find(u32ClientID);
				if(it)
				{
					(*it)->OnEntityRPCCallback(kStream);
				}
			}
			break;
		case ID_SERVER_REQ_ENTITY_UPDATE:
			{
				ClientAgent** it = m_kAgentMap.Find(u32ClientID);
				if(it)
				{
					VEString kName;
					kStream >> kName;
					(*it)->SyncEntityToClient(kName);
				}
			}
			break;
		default:
			break;
		}
		m_pkPeer->DeallocatePacket(pkPacket);
		pkPacket = m_pkPeer->Receive();
	}

	m_u64ServerTime = m_u64ServerStartTime + VeUInt64(g_pTime->GetTime() * 100);
	m_kTimer.Tick(m_u64ServerTime);
	for(VeUInt32 i(0); i < m_kEntityCacheArray.Size(); ++i)
	{
		EntityCache& kCache = m_kEntityCacheArray[i];
		kCache.m_kWildEntities.BeginIterator();
		while(!kCache.m_kWildEntities.IsEnd())
		{
			EntityS* pkEntity = kCache.m_kWildEntities.GetIterNode()->m_tContent;
			kCache.m_kWildEntities.Next();
			VE_ASSERT(pkEntity);
			if(!(pkEntity->IsLocked()))
			{
				pkEntity->Unload();
			}
		}
	}
	OnUpdate();
}
//--------------------------------------------------------------------------
void Server::Disconnect(VeUInt32 u32ClientID)
{
	ClientAgent** it = m_kAgentMap.Find(u32ClientID);
	if(it)
	{
		(*it)->DetachAllEntities();
		(*it)->OnDisconnect();
		m_kAgentMap.Remove(u32ClientID);
		VE_SAFE_DELETE(*it);
	}
}
//--------------------------------------------------------------------------
void Server::DirtyDB(VeRefNode<PropertySContainer*>& kNode)
{
	m_kDBDirty.AttachBack(kNode);
}
//--------------------------------------------------------------------------
DBBusiness* Server::GetDBBusiness()
{
	VE_ASSERT(m_kDatabase);
	return m_kDatabase;
}
//--------------------------------------------------------------------------
void Server::WriteBackDirtyToDB()
{
	m_kDBDirty.BeginIterator();
	while(!m_kDBDirty.IsEnd())
	{
		PropertySContainer* pkContainer = m_kDBDirty.GetIterNode()->m_tContent;
		m_kDBDirty.Next();
		VE_ASSERT(pkContainer);
		pkContainer->UpdateProperties();
	}
}
//--------------------------------------------------------------------------
LoadEntResult Server::LoadEntity(const VeChar8* pcName,
	const VeChar8* pcSearchIndex, EntityS** ppkOut)
{
	VeUInt32 u32Type = _EntNameToType(pcName);
	if(u32Type == VE_INFINITE)
	{
		if(ppkOut) *ppkOut = NULL;
		return LOAD_ENT_TYPE_NOT_EXIST;
	}
	else
	{
		if(u32Type >= m_kEntityCacheArray.Size())
		{
			m_kEntityCacheArray.Resize(u32Type + 1);
		}
		EntityCache& kCache = m_kEntityCacheArray[u32Type];
		VeStringMap<EntityS*>::iterator it = kCache.m_kIndexMap.Find(pcSearchIndex);
		if(it)
		{
			if(ppkOut) *ppkOut = *it;
			return LOAD_ENT_EXIST;
		}
		else
		{
			EntityS* pkEnt = NewEntity(pcName, pcSearchIndex);
			if(pkEnt)
			{
				if(pkEnt->OnLoad())
				{
					pkEnt->m_pkCache = &kCache;
					kCache.m_kIndexMap[pcSearchIndex] = pkEnt;
					kCache.m_kWildEntities.AttachBack(pkEnt->m_kServerNode);
					if(ppkOut) *ppkOut = pkEnt;
					return LOAD_ENT_S_OK;
				}
				else
				{
					VE_SAFE_DELETE(pkEnt);
					if(ppkOut) *ppkOut = NULL;
					return LOAD_ENT_E_FAIL;
				}
			}
			else
			{
				if(ppkOut) *ppkOut = NULL;
				return LOAD_ENT_E_FAIL;
			}
		}
	}
}
//--------------------------------------------------------------------------
UnLoadEntResult Server::UnloadEntity(const VeChar8* pcName,
	const VeChar8* pcSearchIndex)
{
	VeUInt32 u32Type = _EntNameToType(pcName);
	if(u32Type == VE_INFINITE)
	{
		return UNLOAD_ENT_TYPE_NOT_EXIST;
	}
	else
	{
		if(u32Type >= m_kEntityCacheArray.Size())
		{
			m_kEntityCacheArray.Resize(u32Type + 1);
		}
		EntityCache& kCache = m_kEntityCacheArray[u32Type];
		VeStringMap<EntityS*>::iterator it = kCache.m_kIndexMap.Find(pcSearchIndex);
		if(it)
		{
			EntityS* pkEnt = *it;
			if(pkEnt->Unload())
			{
				return UNLOAD_ENT_S_OK;
			}
			else
			{
				return UNLOAD_ENT_FAIL;
			}
		}
		else
		{
			return UNLOAD_ENT_NOT_EXIST;
		}
	}
}
//--------------------------------------------------------------------------
EntityS* Server::GetEntity(const VeChar8* pcName, const VeChar8* pcSearchIndex)
{
	VeUInt32 u32Type = _EntNameToType(pcName);
	if(u32Type < m_kEntityCacheArray.Size())
	{
		EntityCache& kCache = m_kEntityCacheArray[u32Type];
		VeStringMap<EntityS*>::iterator it = kCache.m_kIndexMap.Find(pcSearchIndex);
		if(it)
		{
			return *it;
		}
	}
	return NULL;
}
//--------------------------------------------------------------------------
VeUInt32 Server::_EntNameToType(const VeChar8* pcName)
{
	VeStringMap<VeUInt32>::iterator it = m_kEntityNameMap.Find(pcName);
	return it ? *it : VE_INFINITE;
}
//--------------------------------------------------------------------------
const VeChar8* Server::_EntTypeToName(VeUInt32 u32Type)
{
	return (u32Type < m_kEntityNames.Size()) ? m_kEntityNames[u32Type] : NULL;
}
//--------------------------------------------------------------------------
bool Server::IsMatch(VeUInt8 u8VersionMajor, VeUInt8 u8VersionMinor)
{
	return m_kVersion.m_au8Version[0] == u8VersionMajor
		&& m_kVersion.m_au8Version[1] == u8VersionMinor;
}
//--------------------------------------------------------------------------
VeUInt32 Server::GetClientNumber()
{
	return m_kAgentList.Size() + 587;
}
//--------------------------------------------------------------------------
void Server::AttachByExpire(const VeTimeEventPtr& spEvent, VeUInt64 u64Expire)
{
	m_kTimer.Attach(spEvent, u64Expire);
}
//--------------------------------------------------------------------------
void Server::AttachByDelay(const VeTimeEventPtr& spEvent, VeUInt32 u32Delay)
{
	m_kTimer.Attach(spEvent, m_u64ServerTime + VeUInt64(u32Delay));
}
//--------------------------------------------------------------------------
void Server::Detach(const VeTimeEventPtr& spEvent)
{
	m_kTimer.Detach(spEvent);
}
//--------------------------------------------------------------------------
TIMER_IMPL(Server, _SyncToDatabase)
{
	WriteBackDirtyToDB();
	AttachByDelay(spEvent, SAVE_DB_DELAY);
}
//--------------------------------------------------------------------------
VeUInt64 Server::GetTime()
{
	return m_u64ServerTime;
}
//--------------------------------------------------------------------------
ServerManager::ServerManager()
	: m_eState(STATE_NULL)
{

}
//--------------------------------------------------------------------------
ServerManager::~ServerManager()
{
	Clear();
}
//--------------------------------------------------------------------------
bool ServerManager::AddServer(const VeChar8* pcName, Server* pkServer)
{
	if(!pkServer) return false;
	VeStringMap<Server*>::iterator it = m_kServerMap.Find(pcName);
	if(it)	return false;
	m_kServerMap[pcName] = pkServer;
	m_kServerList.AttachBack(pkServer->m_kNode);
	return true;
}
//--------------------------------------------------------------------------
Server* ServerManager::GetServer(const VeChar8* pcName)
{
	VeStringMap<Server*>::iterator it = m_kServerMap.Find(pcName);
	return it ? *it : NULL;
}
//--------------------------------------------------------------------------
void ServerManager::Start()
{
	m_kServerList.BeginIterator();
	while(!m_kServerList.IsEnd())
	{
		Server* pkServer = m_kServerList.GetIterNode()->m_tContent;
		m_kServerList.Next();
		VE_ASSERT(pkServer);
		pkServer->Start();
	}

	m_eState = STATE_RUNNING;
	while(m_eState == STATE_RUNNING)
	{
		VeServerUpdate();
		m_kServerList.BeginIterator();
		while(!m_kServerList.IsEnd())
		{
			Server* pkServer = m_kServerList.GetIterNode()->m_tContent;
			m_kServerList.Next();
			VE_ASSERT(pkServer);
			pkServer->Update();
		}
		VeSleep(1);
	}
}
//--------------------------------------------------------------------------
void ServerManager::Clear()
{
	m_kServerMap.Clear();
	m_kServerList.BeginIterator();
	while(!m_kServerList.IsEnd())
	{
		Server* pkServer = m_kServerList.GetIterNode()->m_tContent;
		m_kServerList.Next();
		VE_ASSERT(pkServer);
		VE_SAFE_DELETE(pkServer);
	}
	m_kServerList.Clear();
}
//--------------------------------------------------------------------------
