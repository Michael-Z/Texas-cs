#pragma once

#include "General.h"
#include "EntityS.h"
#include "DBBusiness.h"
#include <map>

void VeServerInit();

void VeServerTerm();

void VeServerUpdate();

using namespace VENet;

class ClientAgent;
class ServerAgent;

#ifdef VE_DEBUG
#	define SAVE_DB_DELAY (1000)
#else
#	define SAVE_DB_DELAY (3000)
#endif

enum LoadEntResult
{
	LOAD_ENT_S_OK,
	LOAD_ENT_E_FAIL,
	LOAD_ENT_TYPE_NOT_EXIST,
	LOAD_ENT_EXIST,
	LOAD_ENT_ATTACHED,
	LOAD_ENT_ATTACHED_SELF
};

enum UnLoadEntResult
{
	UNLOAD_ENT_S_OK,
	UNLOAD_ENT_FAIL,
	UNLOAD_ENT_TYPE_NOT_EXIST,
	UNLOAD_ENT_NOT_EXIST
};

class VeTimer;

VeSmartPointer(VeTimeEvent);

class VeTimeEvent : public VeRefObject
{
public:
	typedef VeEvent<const VeTimeEventPtr&> Event;
	typedef VeAbstractDelegate<const VeTimeEventPtr&> Delegate;
	VeTimeEvent();

	virtual ~VeTimeEvent();

	void AddDelegate(Delegate& kDelegate);

	VeUInt64 GetExecuteTime();

	void Detach();

	bool IsAttach()const;

	VeTimer* GetParent();

protected:
	friend class VeTimer;
	VeRefNode<VeTimeEvent*> m_kNode;
	VeTimer* m_pkParent;
	Event m_kEvent;

private:
	VeUInt64 m_u64ExecuteTime;

};

class VeTimer : public VeMemObject
{
public:
	VeTimer();

	virtual ~VeTimer();

	void Attach(const VeTimeEventPtr& spEvent, VeUInt64 u64Expire);

	void Detach(const VeTimeEventPtr& spEvent);

	void Tick(VeUInt64 u64Current);

protected:
	friend class VeTimeEvent;
	std::multimap<VeUInt64,VeTimeEvent*> m_kEventMap;
	VeRefList<VeTimeEvent*> m_kEventList;
	VeVector<VeTimeEvent*> m_kCacheCallback;

};

#define TIMER_DECL(class,name)														\
	typedef VeMemberDelegate<class, const VeTimeEventPtr&> name##Delegate;			\
	name##Delegate m_k##name##Delegate;												\
	VeTimeEventPtr m_sp##name##Event;												\
	void name(const VeTimeEventPtr& spEvent = NULL)

#define TIMER_INIT(class,name)														\
	m_k##name##Delegate.Set(this, &class::name);									\
	m_sp##name##Event = VE_NEW VeTimeEvent;											\
	m_sp##name##Event->AddDelegate(m_k##name##Delegate)

#define TIMER_IMPL(class,name)														\
	void class::name(const VeTimeEventPtr& spEvent)

#define TIMER(name) (m_sp##name##Event)

class Server : public PropertySContainer
{
public:
	typedef VeMemberDelegate<Server, const VeTimeEventPtr&> TimeDelegate;

	struct Version
	{
		VeUInt8 m_au8Version[4];
	};

	Server(const VeChar8* pcName, const VeChar8* pcConfigPath);

	virtual ~Server();

	void Start();

	void Update();

	void DirtyDB(VeRefNode<PropertySContainer*>& kNode);

	DBBusiness* GetDBBusiness();

	void WriteBackDirtyToDB();

	LoadEntResult LoadEntity(const VeChar8* pcName, const VeChar8* pcSearchIndex, EntityS** ppkOut = NULL);

	UnLoadEntResult UnloadEntity(const VeChar8* pcName, const VeChar8* pcSearchIndex);

	EntityS* GetEntity(const VeChar8* pcName, const VeChar8* pcSearchIndex);

	bool IsMatch(VeUInt8 u8VersionMajor, VeUInt8 u8VersionMinor);

	virtual ClientAgent* NewAgent(const SystemAddress& kAddress) = 0;

	virtual EntityS* NewEntity(const VeChar8* pcName, const VeChar8* pcIndex) = 0;

	virtual void OnStart() = 0;

	virtual void OnUpdate() = 0;

	VeUInt32 GetClientNumber();

	void AttachByExpire(const VeTimeEventPtr& spEvent, VeUInt64 u64Expire);

	void AttachByDelay(const VeTimeEventPtr& spEvent, VeUInt32 u32Delay);

	void Detach(const VeTimeEventPtr& spEvent);

	VeUInt64 GetTime();

protected:
	void Disconnect(VeUInt32 u32ClientID);

	VeUInt32 _EntNameToType(const VeChar8* pcName);

	const VeChar8* _EntTypeToName(VeUInt32 u32Type);

	const VeStringA m_kName;
	VENet::VEPeerInterface* m_pkPeer;
	VeXMLDocument m_kConfig;
	Version m_kVersion;
	VeStringMap<VeUInt32> m_kEntityNameMap;
	VeVector<const VeChar8*> m_kEntityNames;

private:
	friend class ServerManager;
	friend class ClientAgent;
	VeRefNode<Server*> m_kNode;
	VeHashMap<VeUInt32,ClientAgent*> m_kAgentMap;
	VeRefList<ClientAgent*> m_kAgentList;
	VeRefList<PropertySContainer*> m_kDBDirty;
	VeVector<EntityCache> m_kEntityCacheArray;
	
private:
	DBBusiness* m_kDatabase;
	TIMER_DECL(Server, _SyncToDatabase);

protected:
	VeStringA_S m_strServerName;
	VeUInt64_S m_u64ServerTime;

protected:
	VeUInt64 m_u64ServerStartTime;
	VeTimer m_kTimer;

};

class ServerManager : public VeSingleton<ServerManager>
{
public:
	enum State
	{
		STATE_NULL,
		STATE_RUNNING,
		STATE_STOP
	};

	ServerManager();

	~ServerManager();

	bool AddServer(const VeChar8* pcName, Server* pkServer);

	Server* GetServer(const VeChar8* pcName);

	void Start();

protected:
	void Clear();

	State m_eState;
	VeStringMap<Server*> m_kServerMap;
	VeRefList<Server*> m_kServerList;

};

#define g_pServerManager ServerManager::GetSingletonPtr()
