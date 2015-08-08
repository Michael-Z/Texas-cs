#include "Client.h"
#include "MessageIdentifiers.h"
#include "ServerAgent.h"

//--------------------------------------------------------------------------
void VeClientInit()
{
	VeGlobalStringTableA::Create();
	VE_NEW VeAllocatorManager();
	VE_NEW VeStringParser();
	VE_NEW VeTime();
	VeLogger::SetLog(VeLogger::OUTPUT_DEBUGOUTPUT);
	VE_NEW VeResourceManager();
	VE_NEW VeURL();
	VE_NEW Client();
}
//--------------------------------------------------------------------------
void VeClientTerm()
{
	Client::Destory();
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
Connection::Connection(const VeChar8* pcName)
	: m_kName(pcName), m_pkPeer(NULL), m_pkServer(NULL)
{
	m_kNode.m_tContent = this;
	
	g_pClient->AddConnection(pcName, this);
}
//--------------------------------------------------------------------------
Connection::~Connection()
{
	VE_SAFE_DELETE(m_pkServer);
}
//--------------------------------------------------------------------------
void Connection::Connect(const VeChar8* pcSrvIp, VeUInt32 u32Port,
	const VeChar8* pcSrvPass)
{
	VE_AUTO_LOCK_MUTEX(g_pClient->m_kMutex);
	if(!m_pkServer)
	{
		m_pkServer = NewAgent();
		VE_ASSERT(m_pkServer);
	}

	m_kServerIp = pcSrvIp;
	m_u32Port = u32Port;
	m_kServerPass = pcSrvPass;

	VE_ASSERT(m_kNode.IsAttach(g_pClient->m_kConnectOffList));
	g_pClient->m_kConnectOnList.AttachBack(m_kNode);
	m_kCommandList.PushBack(COM_CONNECT);	
}
//--------------------------------------------------------------------------
void Connection::Disconnect()
{
	VE_AUTO_LOCK_MUTEX(g_pClient->m_kMutex);
	VE_ASSERT(m_kNode.IsAttach(g_pClient->m_kConnectOnList));
	m_kCommandList.PushBack(COM_DISCONNECT);
}
//--------------------------------------------------------------------------
void Connection::UpdateBackground()
{
	for(VeUInt32 i(0); i < m_kCommandList.Size(); ++i)
	{
		switch(m_kCommandList[i])
		{
		case COM_CONNECT:
			_Connect();
			break;
		case COM_DISCONNECT:
			_Disconnect();
			break;
		default:
			break;
		}
	}
	m_kCommandList.Clear();

	if(m_pkPeer)
	{
		Packet* pkPacket(NULL);
		for(pkPacket = m_pkPeer->Receive(); pkPacket; pkPacket = m_pkPeer->Receive())
		{
			m_kPacketCache.PushBack(pkPacket);
		}
	}
}
//--------------------------------------------------------------------------
void Connection::UpdateForeground()
{
	for(VeUInt32 i(0); i < m_kPacketCache.Size(); ++i)
	{
		Packet* pkPacket = m_kPacketCache[i];
		BitStream kStream((unsigned char*)pkPacket->data, pkPacket->length, false);
		VeUInt8 u8Event;
		kStream >> u8Event;
		switch(u8Event)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			ConnectFailed(CONNECT_FAIL_NORMAL);
			break;
		case ID_REMOTE_CONNECTION_LOST:
			ConnectFailed(CONNECT_FAIL_NORMAL);
			break;
		case ID_CONNECTION_BANNED:
			ConnectFailed(CONNECT_FAIL_SERVER_FULL);
			break;			
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			ConnectFailed(CONNECT_FAIL_SERVER_FULL);
			break;
		case ID_CONNECTION_LOST:
			ConnectFailed(CONNECT_FAIL_NORMAL);
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			VE_ASSERT(!(m_pkServer->IsServerConnected()));
			m_pkServer->SetServerID(pkPacket->systemAddress);
			m_pkServer->OnConnect();
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			ConnectFailed(CONNECT_FAIL_NORMAL);
			break;
		case ID_CLIENT_GLOBAL_RPC:
			m_pkServer->OnGlobalRPCCallback(kStream);
			break;
		case ID_CLIENT_ENTITY_RPC:
			m_pkServer->OnEntityRPCCallback(kStream);
			break;
		case ID_CLIENT_ENTITY_UPDATE:
			m_pkServer->EntityUpdate(kStream);
			break;
		case ID_CLIENT_ENTITY_DESTORY:
			m_pkServer->EntityDestory(kStream);
			break;
		case ID_CLIENT_FORCE_DISCONNECT:
			ForceDisconnect(CONNECT_FORCE_OFF);
			break;
		default:
			break;
		}
		m_pkPeer->DeallocatePacket(pkPacket);
	}
	m_kPacketCache.Clear();
}
//--------------------------------------------------------------------------
void Connection::ConnectFailed(ConnectFail eFail)
{
	if(m_pkServer && m_pkServer->IsServerConnected())
	{
		m_pkServer->OnDisconnect();
		m_pkServer->SetServerID(UNASSIGNED_SYSTEM_ADDRESS);
	}
	OnConnectFailed(eFail);
	Disconnect();
}
//--------------------------------------------------------------------------
ServerAgent* Connection::GetAgent()
{
	return m_pkServer;
}
//--------------------------------------------------------------------------
void Connection::_Connect()
{
	VE_ASSERT(!m_pkPeer);
	m_pkPeer = VENet::VEPeerInterface::GetInstance();

	VENet::SocketDescriptor kSocketDesc;
	kSocketDesc.socketFamily=AF_INET;

#	if defined(VE_PLATFORM_IOS)
	signal(SIGPIPE,SIG_IGN);
#	endif

	m_pkPeer->Startup(1, &kSocketDesc, 1);
	m_pkPeer->SetOccasionalPing(true);

	VE_ASSERT_EQ(m_pkPeer->Connect(m_kServerIp, m_u32Port, m_kServerPass, m_kServerPass.Length()), VENet::CONNECTION_ATTEMPT_STARTED);
}
//--------------------------------------------------------------------------
void Connection::_Disconnect()
{
	VE_ASSERT(m_pkPeer);
	m_pkPeer->Shutdown(300);
	VENet::VEPeerInterface::DestroyInstance(m_pkPeer);
	m_pkPeer = NULL;
	g_pClient->m_kConnectOffList.AttachBack(m_kNode);
}
//--------------------------------------------------------------------------
VeUInt32 Connection::_EntNameToType(const VeChar8* pcName)
{
	VeStringMap<VeUInt32>::iterator it = m_kEntityNameMap.Find(pcName);
	return it ? *it : VE_INFINITE;
}
//--------------------------------------------------------------------------
const VeChar8* Connection::_EntTypeToName(VeUInt32 u32Type)
{
	return (u32Type < m_kEntityNames.Size()) ? m_kEntityNames[u32Type] : NULL;
}
//--------------------------------------------------------------------------
void Connection::ForceDisconnect(ConnectFail eFail)
{
	ConnectFailed(eFail);
}
//--------------------------------------------------------------------------
Client::Client() : m_kThread(10)
{
	m_kThread.SetListener(this);
}
//--------------------------------------------------------------------------
Client::~Client()
{
	Clear();
}
//--------------------------------------------------------------------------
bool Client::AddConnection(const VeChar8* pcName, Connection* pkConnect)
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	if(!pkConnect) return false;
	VeStringMap<Connection*>::iterator it = m_kConnectionMap.Find(pcName);
	if(it)	return false;
	m_kConnectionMap[pcName] = pkConnect;
	m_kConnectOffList.AttachBack(pkConnect->m_kNode);
	return true;
}
//--------------------------------------------------------------------------
Connection* Client::GetConnection(const VeChar8* pcName)
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	VeStringMap<Connection*>::iterator it = m_kConnectionMap.Find(pcName);
	return it ? *it : NULL;
}
//--------------------------------------------------------------------------
void Client::Clear()
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	m_kConnectionMap.Clear();
	m_kConnectOnList.BeginIterator();
	while(!m_kConnectOnList.IsEnd())
	{
		Connection* pkConnect = m_kConnectOnList.GetIterNode()->m_tContent;
		m_kConnectOnList.Next();
		VE_ASSERT(pkConnect);
		pkConnect->Disconnect();
	}
	VE_ASSERT(m_kConnectOnList.Empty());
	m_kConnectOffList.BeginIterator();
	while(!m_kConnectOffList.IsEnd())
	{
		Connection* pkConnect = m_kConnectOffList.GetIterNode()->m_tContent;
		m_kConnectOffList.Next();
		VE_ASSERT(pkConnect);
		VE_SAFE_DELETE(pkConnect);
	}
	VE_ASSERT(m_kConnectOffList.Empty());
}
//--------------------------------------------------------------------------
void Client::Connect(const VeChar8* pcName, const VeChar8* pcSrvIp,
	VeUInt32 u32Port, const VeChar8* pcSrvPass)
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	Connection* pkConnect = GetConnection(pcName);
	if(pkConnect)
	{
		pkConnect->Connect(pcSrvIp, u32Port, pcSrvPass);
	}
}
//--------------------------------------------------------------------------
void Client::Disconnect(const VeChar8* pcName)
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	Connection* pkConnect = GetConnection(pcName);
	if(pkConnect)
	{
		pkConnect->ForceDisconnect(Connection::CONNECT_TURN_OFF);
	}
}
//--------------------------------------------------------------------------
void Client::Update()
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	VeRefList<Connection*>::iterator it = m_kConnectOnList.GetHeadNode();
	while(!m_kConnectOnList.IsEnd(it))
	{
		Connection* pkConnect = (*it).m_tContent;
		it = VeRefList<Connection*>::Next(it);		
		VE_ASSERT(pkConnect);
		pkConnect->UpdateForeground();
	}
}
//--------------------------------------------------------------------------
void Client::Start()
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	m_kThread.Start();
}
//--------------------------------------------------------------------------
void Client::Stop()
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	m_kThread.Stop();
	m_kConnectOnList.BeginIterator();
	while(!m_kConnectOnList.IsEnd())
	{
		Connection* pkConnect = m_kConnectOnList.GetIterNode()->m_tContent;
		m_kConnectOnList.Next();
		VE_ASSERT(pkConnect);
		pkConnect->_Disconnect();
	}
}
//--------------------------------------------------------------------------
void Client::Tick()
{
	VE_AUTO_LOCK_MUTEX(m_kMutex);
	VeRefList<Connection*>::iterator it = m_kConnectOnList.GetHeadNode();
	while(!m_kConnectOnList.IsEnd(it))
	{
		Connection* pkConnect = (*it).m_tContent;
		it = VeRefList<Connection*>::Next(it);		
		VE_ASSERT(pkConnect);
		pkConnect->UpdateBackground();
	}
}
//--------------------------------------------------------------------------
