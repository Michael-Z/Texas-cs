#pragma once

#include "General.h"

class ServerAgent;

void VeClientInit();

void VeClientTerm();

using namespace VENet;

class Connection : public VeMemObject
{
public:
	enum ConnectFail
	{
		CONNECT_FAIL_NORMAL,
		CONNECT_FAIL_SERVER_FULL,
		CONNECT_FAIL_LOST,
		CONNECT_FORCE_OFF,
		CONNECT_TURN_OFF
	};

	enum Command
	{
		COM_CONNECT,
		COM_DISCONNECT
	};

	Connection(const VeChar8* pcName);

	virtual ~Connection();

	void ConnectFailed(ConnectFail eFail);

	virtual void OnConnectFailed(ConnectFail eFail) = 0;

	virtual ServerAgent* NewAgent() = 0;

	ServerAgent* GetAgent();

	void ForceDisconnect(ConnectFail eFail);

protected:
	void Connect(const VeChar8* pcSrvIp, VeUInt32 u32Port, const VeChar8* pcSrvPass);

	void Disconnect();

	void UpdateBackground();

	void UpdateForeground();

	void _Connect();

	void _Disconnect();

	VeUInt32 _EntNameToType(const VeChar8* pcName);

	const VeChar8* _EntTypeToName(VeUInt32 u32Type);

	const VeStringA m_kName;
	VEPeerInterface* m_pkPeer;
	ServerAgent* m_pkServer;
	VeStringA m_kServerIp;
	VeUInt32 m_u32Port;
	VeStringA m_kServerPass;
	VeStringMap<VeUInt32> m_kEntityNameMap;
	VeVector<const VeChar8*> m_kEntityNames;

private:
	friend class Client;
	friend class ServerAgent;
	VeRefNode<Connection*> m_kNode;
	VeVector<Command> m_kCommandList;
	VeVector<Packet*> m_kPacketCache;

};

class Client : public VeSingleton<Client>, public VeTickThread::TickListener
{
public:
	Client();

	~Client();

	bool AddConnection(const VeChar8* pcName, Connection* pkConnect);

	Connection* GetConnection(const VeChar8* pcName);

	void Connect(const VeChar8* pcName, const VeChar8* pcSrvIp, VeUInt32 u32Port, const VeChar8* pcSrvPass);

	void Disconnect(const VeChar8* pcName);

	void Update();
	
	void Start();

	void Stop();	

	virtual void Tick();

protected:
	friend class Connection;

	void Clear();

	VeTickThread m_kThread;
	VeThread::Mutex m_kMutex;
	VeStringMap<Connection*> m_kConnectionMap;
	VeRefList<Connection*> m_kConnectOffList;
	VeRefList<Connection*> m_kConnectOnList;


};

#define g_pClient Client::GetSingletonPtr()
#define CONNECT(type,ip,port,pwd) g_pClient->Connect(type::GetName(), ip, port, pwd)
#define DISCONNECT(type) g_pClient->Disconnect(type::GetName())
#define GET_CONNECTION(type) static_cast<type*>(g_pClient->GetConnection(type::GetName()))
#define GET_AGENT(type) static_cast<type*>(g_pClient->GetConnection(type::GetName())->GetAgent())
