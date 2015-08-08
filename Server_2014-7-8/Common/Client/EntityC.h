#pragma once

#include "General.h"

class PropertyCContainer;
class ServerAgent;

class PropertyC : public VeMemObject
{
public:
	PropertyC(PropertyCContainer* pkParent);

	virtual ~PropertyC();

protected:
	PropertyCContainer* m_pkParent;

};

class PropertyCValue : public PropertyC
{
public:
	PropertyCValue(PropertyCContainer* pkParent, const VeChar8* pcField);

	const VeChar8* GetField();

	virtual void SetData(void* pvData, VeUInt32 u32Size) = 0;

protected:
	const VeChar8* m_pcField;

};

template <class T>
class PropertyCBase : public PropertyCValue
{
public:
	PropertyCBase(PropertyCContainer* pkParent, const VeChar8* pcField)
		: PropertyCValue(pkParent, pcField)
	{

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
		m_tValue = tVal;
	}

	T& operator += (const T& tVal)
	{
		m_tValue += tVal;
	}

	T& operator -= (const T& tVal)
	{
		m_tValue -= tVal;
	}

	T& operator *= (const T& tVal)
	{
		m_tValue *= tVal;
	}

	T& operator /= (const T& tVal)
	{
		m_tValue /= tVal;
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

protected:
	T m_tValue;

};

#define PROPERTY_C_BASE(type)					\
	typedef PropertyCBase<type> type##_C;

PROPERTY_C_BASE(VeUInt8);
PROPERTY_C_BASE(VeUInt16);
PROPERTY_C_BASE(VeUInt32);
PROPERTY_C_BASE(VeUInt64);
PROPERTY_C_BASE(VeInt8);
PROPERTY_C_BASE(VeInt16);
PROPERTY_C_BASE(VeInt32);
PROPERTY_C_BASE(VeInt64);

class VeStringA_C : public PropertyCValue
{
public:
	VeStringA_C(PropertyCContainer* pkParent, const VeChar8* pcField);

	virtual PropertyType GetType();

	virtual void SetData(void* pvData, VeUInt32 u32Size);

	operator const VeStringA& ();

	operator const VeChar8* ();

	VeStringA& operator = (const VeStringA& kVal);

protected:
	VeStringA m_kValue;

};

class PropertyCContainer : public VeMemObject
{
public:
	PropertyCContainer();

	virtual ~PropertyCContainer();

	void Attach(PropertyCValue* pkChild);

	PropertyCValue* GetValue(const VeChar8* pcField);

protected:
	friend class DBBusiness;
	VeStringMap<PropertyCValue*> m_kValueMap;
	VeVector<PropertyCValue*> m_kValueArray;

};

class EntityC : public PropertyCContainer
{
public:
	EntityC(ServerAgent* pkAgent, const VeChar8* pcName);

	virtual ~EntityC();

	void Update(BitStream& kStream);

	void RequestUpdate();

	virtual void OnLoad() = 0;

	virtual void OnUnload() = 0;

	virtual void OnUpdate() = 0;

	virtual void OnRPCCallback(BitStream& kStream) = 0;

protected:
	bool SendPacket(const BitStream& kStream);

	friend class ServerAgent;
	ServerAgent* m_pkAgent;
	VeString m_kName;
	VeRefNode<EntityC*> m_kNode;

};
