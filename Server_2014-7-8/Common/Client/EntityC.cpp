#include "EntityC.h"
#include "ServerAgent.h"

//--------------------------------------------------------------------------
PropertyC::PropertyC(PropertyCContainer* pkParent) : m_pkParent(pkParent)
{

}
//--------------------------------------------------------------------------
PropertyC::~PropertyC()
{

}
//--------------------------------------------------------------------------
PropertyCValue::PropertyCValue(PropertyCContainer* pkParent,
	const VeChar8* pcField)
	: PropertyC(pkParent), m_pcField(pcField)
{
	m_pkParent->Attach(this);
}
//--------------------------------------------------------------------------
const VeChar8* PropertyCValue::GetField()
{
	return m_pcField;
}
//--------------------------------------------------------------------------
VeStringA_C::VeStringA_C(PropertyCContainer* pkParent,
	const VeChar8* pcField)
	: PropertyCValue(pkParent, pcField)
{

}
//--------------------------------------------------------------------------
PropertyType VeStringA_C::GetType()
{
	return TYPE_VeStringA;
}
//--------------------------------------------------------------------------
void VeStringA_C::SetData(void* pvData, VeUInt32 u32Size)
{
	m_kValue = VeStringA((const VeChar8*)pvData, u32Size);
}
//--------------------------------------------------------------------------
VeStringA_C::operator const VeStringA&()
{
	return m_kValue;
}
//--------------------------------------------------------------------------
VeStringA_C::operator const VeChar8* ()
{
	return m_kValue;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA_C::operator = (const VeStringA& kVal)
{
	m_kValue = kVal;
	return m_kValue;
}
//--------------------------------------------------------------------------
PropertyCContainer::PropertyCContainer()
{

}
//--------------------------------------------------------------------------
PropertyCContainer::~PropertyCContainer()
{

}
//--------------------------------------------------------------------------
void PropertyCContainer::Attach(PropertyCValue* pkChild)
{
	m_kValueMap[pkChild->GetField()] = pkChild;
	m_kValueArray.PushBack(pkChild);
}
//--------------------------------------------------------------------------
PropertyCValue* PropertyCContainer::GetValue(const VeChar8* pcField)
{
	VeStringMap<PropertyCValue*>::iterator it = m_kValueMap.Find(pcField);
	return it ? (*it) : NULL;
}
//--------------------------------------------------------------------------
EntityC::EntityC(ServerAgent* pkAgent, const VeChar8* pcName)
	: m_pkAgent(pkAgent), m_kName(pcName)
{
	m_kNode.m_tContent = this;
}
//--------------------------------------------------------------------------
EntityC::~EntityC()
{

}
//--------------------------------------------------------------------------
void EntityC::Update(BitStream& kStream)
{
	VeChar8 acBuffer[VE_UINT8_MAX];
	VeUInt16 u16Size;
	kStream >> u16Size;
	for(VeUInt32 i(0); i < u16Size; ++i)
	{
		VEString kField;
		kStream >> kField;
		PropertyCValue* pkValue = GetValue(kField);
		VeUInt32 u32Size;
		kStream.ReadAlignedBytesSafe(acBuffer, u32Size, VE_UINT8_MAX);
		pkValue->SetData(acBuffer, u32Size);
	}
	OnUpdate();
}
//--------------------------------------------------------------------------
void EntityC::RequestUpdate()
{
	BitStream kStream;
	kStream.Reset();
	kStream << VeUInt8(ID_SERVER_REQ_ENTITY_UPDATE);
	kStream << (const VeChar8*)m_kName;
	m_pkAgent->SendPacket(kStream);
}
//--------------------------------------------------------------------------
bool EntityC::SendPacket(const BitStream& kStream)
{
	return m_pkAgent->SendPacket(kStream);
}
//--------------------------------------------------------------------------
