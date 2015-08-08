#include "EntityS.h"
#include "Server.h"
#include "ClientAgent.h"

//--------------------------------------------------------------------------
PropertyS::PropertyS(PropertySContainer* pkParent, VeUInt32 u32Attribute)
	: m_pkParent(pkParent), m_u32Attribute(u32Attribute)
{

}
//--------------------------------------------------------------------------
PropertyS::~PropertyS()
{

}
//--------------------------------------------------------------------------
bool PropertyS::IsSaveDatabase()
{
	return VE_MASK_HAS_ALL(m_u32Attribute, FLAG_DATABASE);
}
//--------------------------------------------------------------------------
bool PropertyS::IsSynToClient()
{
	return VE_MASK_HAS_ALL(m_u32Attribute, FLAG_CLIENT);
}
//--------------------------------------------------------------------------
PropertySValue::PropertySValue(PropertySContainer* pkParent,
	VeUInt32 u32Attribute, const VeChar8* pcField)
	: PropertyS(pkParent, u32Attribute), m_pcField(pcField)
{
	m_kDBNode.m_tContent = this;
	m_kSyncNode.m_tContent = this;
	m_pkParent->Attach(this);
}
//--------------------------------------------------------------------------
const VeChar8* PropertySValue::GetField()
{
	return m_pcField;
}
//--------------------------------------------------------------------------
void PropertySValue::NotifyDirty()
{
	if(IsSaveDatabase())
	{
		if(!m_kDBNode.IsAttach())
		{
			m_pkParent->DirtyDB(m_kDBNode);
		}
	}
	if(IsSynToClient())
	{
		if(!m_kSyncNode.IsAttach())
		{
			m_pkParent->DirtySync(m_kSyncNode);
		}
	}
}
//--------------------------------------------------------------------------
VeStringA_S::VeStringA_S(PropertySContainer* pkParent,
	VeUInt32 u32Attribute, const VeChar8* pcField, const VeChar8* pcVal)
	: PropertySValue(pkParent, u32Attribute, pcField)
{
	m_kDefault = pcVal;
	m_kValue = m_kDefault;
	UpdateValueToGBKData();
}
//--------------------------------------------------------------------------
PropertyType VeStringA_S::GetType()
{
	return TYPE_VeStringA;
}
//--------------------------------------------------------------------------
void* VeStringA_S::GetDefaultData()
{
	return (void*)(const VeChar8*)m_kDefault;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA_S::GetDefaultSize()
{
	return m_kDefault.Length();
}
//--------------------------------------------------------------------------
void* VeStringA_S::GetData()
{
	return (void*)(const VeChar8*)m_kGBKData;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA_S::GetSize()
{
	return m_kGBKData.Length();
}
//--------------------------------------------------------------------------
void* VeStringA_S::GetClientData()
{
	return (void*)(const VeChar8*)m_kValue;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringA_S::GetClientSize()
{
	return m_kValue.Length();
}
//--------------------------------------------------------------------------
void VeStringA_S::SetData(void* pvData, VeUInt32 u32Size)
{
	m_kGBKData = VeStringA((const VeChar8*)pvData, u32Size);
	UpdateGBKToValueData();
}
//--------------------------------------------------------------------------
VeStringA_S::operator const VeStringA&()
{
	return m_kValue;
}
//--------------------------------------------------------------------------
VeStringA& VeStringA_S::operator = (const VeStringA& kVal)
{
	if(m_kValue != kVal)
	{
		m_kValue = kVal;
		UpdateValueToGBKData();
		NotifyDirty();
	}
	return m_kValue;
}
//--------------------------------------------------------------------------
const VeChar8* VeStringA_S::GetString()
{
	return m_kValue;
}
//--------------------------------------------------------------------------
void VeStringA_S::UpdateValueToGBKData()
{
	if(m_kValue.Length())
	{
		VeUInt32 u32Len = MultiByteToWideChar(CP_UTF8, 0, m_kValue, -1, NULL, 0);
		VeChar16* wszGBK = VeAlloc(VeChar16, u32Len + 1);
		VeZeroMemory(wszGBK, sizeof(VeChar16) * (u32Len + 1));
		MultiByteToWideChar(CP_UTF8, 0, m_kValue, -1, wszGBK, u32Len);
		u32Len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		VeChar8* szGBK = VeAlloc(VeChar8, u32Len + 1);
		VeZeroMemory(szGBK, sizeof(VeChar8) * (u32Len + 1));
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, u32Len, NULL, NULL);
		m_kGBKData = szGBK;
		VeFree(wszGBK);
		VeFree(szGBK);
	}
	else
	{
		m_kGBKData = "";
	}
}
//--------------------------------------------------------------------------
void VeStringA_S::UpdateGBKToValueData()
{
	if(m_kGBKData.Length())
	{
		VeUInt32 u32Len = MultiByteToWideChar(CP_ACP, 0, m_kGBKData, -1, NULL, 0);
		VeChar16* wszGBK = VeAlloc(VeChar16, u32Len + 1);
		VeZeroMemory(wszGBK, sizeof(VeChar16) * (u32Len + 1));
		MultiByteToWideChar(CP_ACP, 0, m_kGBKData, -1, wszGBK, u32Len);
		u32Len = WideCharToMultiByte(CP_UTF8, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		VeChar8* szUTF8 = VeAlloc(VeChar8, u32Len + 1);
		VeZeroMemory(szUTF8, sizeof(VeChar8) * (u32Len + 1));
		WideCharToMultiByte(CP_UTF8, 0, wszGBK, -1, szUTF8, u32Len, NULL, NULL);
		m_kValue = szUTF8;
		VeFree(wszGBK);
		VeFree(szUTF8);
	}
	else
	{
		m_kValue = "";
	}
}
//--------------------------------------------------------------------------
PropertySContainer::PropertySContainer(Server* pkParent)
	: m_pkParent(pkParent)
{
	m_kDBNode.m_tContent = this;
}
//--------------------------------------------------------------------------
PropertySContainer::~PropertySContainer()
{

}
//--------------------------------------------------------------------------
VeUInt32 PropertySContainer::LoadProperties()
{
	if(m_kDBChildren.Size())
	{
		return m_pkParent->GetDBBusiness()->Load(this);
	}
	else
	{
		return VE_S_OK;
	}
}
//--------------------------------------------------------------------------
VeUInt32 PropertySContainer::UpdateProperties()
{
	if(m_kDBDirty.Size())
	{
		VeUInt32 u32Res = m_pkParent->GetDBBusiness()->Update(this);
		if(VE_SUCCEEDED(u32Res))
		{
			m_kDBDirty.Clear();
			m_kDBNode.Detach();
		}
		return u32Res;
	}
	else
	{
		return VE_S_OK;
	}
}
//--------------------------------------------------------------------------
void PropertySContainer::Attach(PropertySValue* pkChild)
{
	if(pkChild->IsSaveDatabase())
	{
		m_kDBChildren.PushBack(pkChild);
	}
	if(pkChild->IsSynToClient())
	{
		m_kSyncChildren.PushBack(pkChild);
	}
}
//--------------------------------------------------------------------------
void PropertySContainer::DirtyDB(VeRefNode<PropertySValue*>& kNode)
{
	m_kDBDirty.AttachBack(kNode);
	if(!m_kDBNode.IsAttach())
	{
		m_pkParent->DirtyDB(m_kDBNode);
	}
}
//--------------------------------------------------------------------------
void PropertySContainer::DirtySync(VeRefNode<PropertySValue*>& kNode)
{
	m_kSyncDirty.AttachBack(kNode);
}
//--------------------------------------------------------------------------
EntityS::EntityS(Server* pkParent, const VeChar8* pcName,
	const VeChar8* pcIndex)
	: PropertySContainer(pkParent), m_pkAgent(NULL), m_kName(pcName)
	, m_kIndex(pcIndex), m_pkCache(NULL), m_bLock(false)
{
	m_kServerNode.m_tContent = this;
	m_kAgentNode.m_tContent = this;
	m_u32HomeCount = 0;
}
//--------------------------------------------------------------------------
EntityS::~EntityS()
{

}
//--------------------------------------------------------------------------
bool EntityS::OnLoad()
{
	VeUInt32 u32Res = LoadProperties();
	switch(u32Res)
	{
	case DBBusiness::RESULT_OK:
		OnAfterLoad(false);
		return true;
	case DBBusiness::RESULT_CREATE:
		OnAfterLoad(true);
		return true;
	default:
		printf("Load entity properties faile\n");
		return false;
	}	
}
//--------------------------------------------------------------------------
void EntityS::OnUnload()
{
	OnBeforeUnload();
	UpdateProperties();
}
//--------------------------------------------------------------------------
void EntityS::SyncAllToClient()
{
	if(m_pkAgent)
	{
		BitStream kStream;
		kStream.Reset();
		kStream << VeUInt8(ID_CLIENT_ENTITY_UPDATE);
		kStream << (const VeChar8*)m_kName;
		VE_ASSERT(m_kSyncChildren.Size() <= VE_UINT16_MAX);
		kStream << VeUInt16(m_kSyncChildren.Size());
		for(VeUInt32 i(0); i < m_kSyncChildren.Size(); ++i)
		{
			PropertySValue* pkValue = m_kSyncChildren[i];
			kStream << pkValue->GetField();
			VE_ASSERT(pkValue->GetSize() <= 0xff);
			kStream.WriteAlignedBytesSafe((const VeChar8*)pkValue->GetClientData(), pkValue->GetClientSize(), VE_UINT8_MAX);
		}
		m_pkAgent->SendPacket(kStream);
	}
}
//--------------------------------------------------------------------------
void EntityS::SyncToClient()
{
	if(m_pkAgent)
	{
		if(m_kSyncDirty.Size())
		{
			BitStream kStream;
			kStream.Reset();
			kStream << VeUInt8(ID_CLIENT_ENTITY_UPDATE);
			kStream << (const VeChar8*)m_kName;
			VE_ASSERT(m_kSyncDirty.Size() <= VE_UINT16_MAX);
			kStream << VeUInt16(m_kSyncDirty.Size());
			m_kSyncDirty.BeginIterator();
			while(!m_kSyncDirty.IsEnd())
			{
				PropertySValue* pkValue = m_kSyncDirty.GetIterNode()->m_tContent;
				m_kSyncDirty.Next();
				VE_ASSERT(pkValue);
				kStream << pkValue->GetField();
				VE_ASSERT(pkValue->GetSize() <= 0xff);
				kStream.WriteAlignedBytesSafe((const VeChar8*)pkValue->GetClientData(), pkValue->GetClientSize(), VE_UINT8_MAX);
			}
			m_kSyncDirty.Clear();
			m_pkAgent->SendPacket(kStream);
		}
	}
}
//--------------------------------------------------------------------------
void EntityS::DestoryFromClient()
{
	if(m_pkAgent)
	{
		BitStream kStream;
		kStream.Reset();
		kStream << VeUInt8(ID_CLIENT_ENTITY_DESTORY);
		kStream << VEString(m_kName);
		m_pkAgent->SendPacket(kStream);
	}
}
//--------------------------------------------------------------------------
void EntityS::SendPacket(const BitStream& kStream)
{
	if(m_pkAgent) m_pkAgent->SendPacket(kStream);
}
//--------------------------------------------------------------------------
void EntityS::ToWild()
{
	VE_ASSERT(m_pkCache);
	if(m_u32HomeCount)
	{
		VE_ASSERT(m_kServerNode.IsAttach(m_pkCache->m_kHomeEntities));
		if(!(--m_u32HomeCount))
		{
			m_pkCache->m_kWildEntities.AttachBack(m_kServerNode);
		}
	}
	else
	{
		VE_ASSERT(m_kServerNode.IsAttach(m_pkCache->m_kWildEntities));
	}
}
//--------------------------------------------------------------------------
void EntityS::ToHome()
{
	VE_ASSERT(m_pkCache);
	if(!m_kServerNode.IsAttach(m_pkCache->m_kHomeEntities))
	{
		VE_ASSERT(!m_u32HomeCount);
		m_pkCache->m_kHomeEntities.AttachBack(m_kServerNode);
	}
	++m_u32HomeCount;
}
//--------------------------------------------------------------------------
const VeChar8* EntityS::GetKey()
{
	return m_kIndex;
}
//--------------------------------------------------------------------------
void EntityS::AttachAgent(ClientAgent* pkAgent)
{
	if(pkAgent != m_pkAgent)
	{
		DetachAgent();
		if(pkAgent)
		{
			pkAgent->m_kEntityMap[m_kName] = this;
			pkAgent->m_kEntityList.AttachBack(m_kAgentNode);
			ToHome();
			m_pkAgent = pkAgent;
			SyncAllToClient();
			OnAfterAttach();
		}		
	}
}
//--------------------------------------------------------------------------
void EntityS::DetachAgent()
{
	if(m_pkAgent)
	{
		DestoryFromClient();
		OnBeforeDetach();
		m_pkAgent->m_kEntityMap.Remove(m_kName);
		m_kAgentNode.Detach();
		ToWild();
		m_pkAgent = NULL;
	}
}
//--------------------------------------------------------------------------
bool EntityS::HasAgent()
{
	if(m_pkAgent)
	{
		VE_ASSERT(m_kAgentNode.IsAttach());
		return true;
	}
	else
	{
		VE_ASSERT(!m_kAgentNode.IsAttach());
		return false;
	}
}
//--------------------------------------------------------------------------
bool EntityS::Unload()
{
	if(m_pkCache)
	{
		if(m_kServerNode.IsAttach(m_pkCache->m_kWildEntities))
		{
			OnUnload();
			m_pkCache->m_kIndexMap.Remove(m_kIndex);
			VE_DELETE(this);
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------
void EntityS::Lock()
{
	m_bLock = true;
}
//--------------------------------------------------------------------------
void EntityS::Unlock()
{
	m_bLock = false;
}
//--------------------------------------------------------------------------
bool EntityS::IsLocked()
{
	return m_bLock;
}
//--------------------------------------------------------------------------
