#pragma once

#include "General.h"
#include "DBBusiness.h"

class PropertySContainer;
class Server;
class ClientAgent;
class DBBusiness;

enum PropertyFlag
{
	FLAG_DATABASE = 0x1,
	FLAG_CLIENT = 0x2
};

class PropertyS : public VeMemObject
{
public:
	PropertyS(PropertySContainer* pkParent, VeUInt32 u32Attribute);

	virtual ~PropertyS();

	bool IsSaveDatabase();

	bool IsSynToClient();

protected:
	PropertySContainer* m_pkParent;
	const VeUInt32 m_u32Attribute;

};

class PropertySValue : public PropertyS
{
public:
	PropertySValue(PropertySContainer* pkParent, VeUInt32 u32Attribute, const VeChar8* pcField);

	const VeChar8* GetField();

	virtual PropertyType GetType() = 0;

	virtual void* GetDefaultData() = 0;

	virtual VeUInt32 GetDefaultSize() = 0;

	virtual void* GetData() = 0;

	virtual VeUInt32 GetSize() = 0;

	virtual void* GetClientData() = 0;

	virtual VeUInt32 GetClientSize() = 0;

	virtual void SetData(void* pvData, VeUInt32 u32Size) = 0;

protected:
	void NotifyDirty();

	const VeChar8* m_pcField;
	VeRefNode<PropertySValue*> m_kDBNode;
	VeRefNode<PropertySValue*> m_kSyncNode;

};

template <class T>
class PropertySBase : public PropertySValue
{
public:
	PropertySBase(PropertySContainer* pkParent, VeUInt32 u32Attribute, const VeChar8* pcField, const T& tVal = 0)
		: PropertySValue(pkParent, u32Attribute, pcField), m_tDefault(tVal), m_tValue(tVal)
	{

	}

	virtual PropertyType GetType()
	{
		return ms_eType;
	}

	virtual void* GetDefaultData()
	{
		return &m_tDefault;
	}

	virtual VeUInt32 GetDefaultSize()
	{
		return sizeof(T);
	}

	virtual void* GetData()
	{
		return &m_tValue;
	}

	virtual VeUInt32 GetSize()
	{
		return sizeof(T);
	}

	virtual void* GetClientData()
	{
		return &m_tValue;
	}

	virtual VeUInt32 GetClientSize()
	{
		return sizeof(T);
	}

	virtual void SetData(void* pvData, VeUInt32 u32Size)
	{
		VE_ASSERT(u32Size == sizeof(T));
		m_tValue = *(T*)pvData;
	}

	operator const T& ()
	{
		return m_tValue;
	}

	T& operator = (const T& tVal)
	{
		if(m_tValue != tVal)
		{
			m_tValue = tVal;
			NotifyDirty();
		}
		return m_tValue;
	}

	T& operator += (const T& tVal)
	{
		m_tValue += tVal;
		NotifyDirty();
		return m_tValue;
	}

	T& operator -= (const T& tVal)
	{
		m_tValue -= tVal;
		NotifyDirty();
		return m_tValue;
	}

	T& operator *= (const T& tVal)
	{
		m_tValue *= tVal;
		NotifyDirty();
		return m_tValue;
	}

	T& operator /= (const T& tVal)
	{
		m_tValue /= tVal;
		NotifyDirty();
		return m_tValue;
	}

	T operator + () const
	{
		return m_tValue;
	}

	T operator - () const
	{
		return -m_tValue;
	}

	T operator + (const T& tVal) const
	{
		return m_tValue + tVal;
	}

	T operator - (const T& tVal) const
	{
		return m_tValue - tVal;
	}

	T operator * (const T& tVal) const
	{
		return m_tValue * tVal;
	}

	T operator / (const T& tVal) const
	{
		return m_tValue / tVal;
	}

	static const PropertyType ms_eType;

protected:
	T m_tDefault;
	T m_tValue;

};

#define PROPERTY_S_BASE(type)					\
	typedef PropertySBase<type> type##_S;		\
	template<> const PropertyType PropertySBase<type>::ms_eType = TYPE_##type;

PROPERTY_S_BASE(VeUInt8);
PROPERTY_S_BASE(VeUInt16);
PROPERTY_S_BASE(VeUInt32);
PROPERTY_S_BASE(VeUInt64);
PROPERTY_S_BASE(VeInt8);
PROPERTY_S_BASE(VeInt16);
PROPERTY_S_BASE(VeInt32);
PROPERTY_S_BASE(VeInt64);

class VeStringA_S : public PropertySValue
{
public:
	VeStringA_S(PropertySContainer* pkParent, VeUInt32 u32Attribute, const VeChar8* pcField, const VeChar8* pcVal = "");

	virtual PropertyType GetType();

	virtual void* GetDefaultData();

	virtual VeUInt32 GetDefaultSize();

	virtual void* GetData();

	virtual VeUInt32 GetSize();

	virtual void* GetClientData();

	virtual VeUInt32 GetClientSize();

	virtual void SetData(void* pvData, VeUInt32 u32Size);

	operator const VeStringA& ();

	const VeChar8* GetString();

	VeStringA& operator = (const VeStringA& kVal);

protected:
	void UpdateValueToGBKData();

	void UpdateGBKToValueData();

	VeStringA m_kDefault;
	VeStringA m_kValue;
	VeStringA m_kGBKData;

};

class PropertySContainer : public VeMemObject
{
public:
	PropertySContainer(Server* pkParent);

	virtual ~PropertySContainer();

	VeUInt32 LoadProperties();

	VeUInt32 UpdateProperties();

	virtual const VeChar8* GetTableName() = 0;

	virtual const VeChar8* GetKeyField() = 0;

	virtual const VeChar8* GetKey() = 0;

	void Attach(PropertySValue* pkChild);

	void DirtyDB(VeRefNode<PropertySValue*>& kNode);

	void DirtySync(VeRefNode<PropertySValue*>& kNode);

protected:
	friend class DBBusiness;
	Server* m_pkParent;
	VeVector<PropertySValue*> m_kDBChildren;
	VeVector<PropertySValue*> m_kSyncChildren;
	VeRefList<PropertySValue*> m_kDBDirty;
	VeRefList<PropertySValue*> m_kSyncDirty;
	VeRefNode<PropertySContainer*> m_kDBNode;

};

class EntityS;

struct EntityCache
{
	VeStringMap<EntityS*> m_kIndexMap;
	VeRefList<EntityS*> m_kWildEntities;
	VeRefList<EntityS*> m_kHomeEntities;
};

class EntityS : public PropertySContainer
{
public:
	EntityS(Server* pkParent, const VeChar8* pcName, const VeChar8* pcIndex);

	virtual ~EntityS();

	bool OnLoad();

	void OnUnload();

	void SyncAllToClient();

	void SyncToClient();

	void DestoryFromClient();

	void AttachAgent(ClientAgent* pkAgent);

	void DetachAgent();

	bool HasAgent();

	bool Unload();

	void Lock();

	void Unlock();

	bool IsLocked();

	virtual const VeChar8* GetKey();

	void ToWild();

	void ToHome();

	virtual void OnAfterLoad(bool bCreate) = 0;

	virtual void OnBeforeUnload() = 0;

	virtual void OnAfterAttach() = 0;

	virtual void OnBeforeDetach() = 0;

	virtual void OnRPCCallback(BitStream& kStream) = 0;	

protected:
	void SendPacket(const BitStream& kStream);

	friend class ClientAgent;
	friend class Server;
	ClientAgent* m_pkAgent;
	const VeString m_kName;
	const VeString m_kIndex;
	EntityCache* m_pkCache;
	VeRefNode<EntityS*> m_kServerNode;
	VeRefNode<EntityS*> m_kAgentNode;
	VeUInt32 m_u32HomeCount;
	bool m_bLock;

};
