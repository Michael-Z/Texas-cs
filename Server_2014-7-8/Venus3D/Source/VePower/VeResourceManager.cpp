////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeResourceManager.cpp
//  Version:     v1.00
//  Created:     6/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"

//--------------------------------------------------------------------------
VeResourceManager::VeResourceManager()
{
	m_kBGQueue[BG_QUEUE_FILE].SetTickOnce(true);
}
//--------------------------------------------------------------------------
VeResourceManager::~VeResourceManager()
{
	for(VeUInt32 i(0); i < BG_QUEUE_NUM; ++i)
	{
		m_kBGQueue[i].SetTickOnce(false);
	}
	while(HasBGTask())
	{
		Update();
		VeSleep(10);
	}
	VE_ASSERT(m_kActiveZipMap.Empty());
}
//--------------------------------------------------------------------------
void VeResourceManager::AddBGTask(BGQueue eQueue, const VeBackgroundTaskPtr& spTask)
{
	VE_ASSERT(eQueue < BG_QUEUE_NUM);
	m_kBGQueue[eQueue].AddBackgroundTask(spTask);
}
//--------------------------------------------------------------------------
void VeResourceManager::AddFGTask(BGQueue eQueue, const VeBackgroundTaskPtr& spTask)
{
	VE_ASSERT(eQueue < BG_QUEUE_NUM);
	m_kBGQueue[eQueue].AddMainThreadTask(spTask);
}
//--------------------------------------------------------------------------
void VeResourceManager::Update()
{
	for(VeUInt32 i(0); i < BG_QUEUE_NUM; ++i)
	{
		m_kBGQueue[i].Update();
	}
}
//--------------------------------------------------------------------------
VeDirectory* VeResourceManager::CreateDir(const VeChar8* pcName)
{
	VeChar8 acBuffer[VE_MAX_PATH_LEN];
	VeStrcpy(acBuffer, VE_MAX_PATH_LEN, pcName);
	VeChar8* pcContext;
	VeChar8* pcTemp = VeStrtok(acBuffer, "#", &pcContext);
	VeDirectory::Type eType = g_pStringParser->Parse(pcTemp, VeDirectory::TYPE_NUM);
	VeDirectory* pkDir(NULL);
	switch(eType)
	{
	case VeDirectory::TYPE_FILE:
		{
			pcTemp = VeStrtok(NULL, "#", &pcContext);
			pkDir = VE_NEW VeFileDir(pcTemp ? pcTemp : "");
		}
		break;
	case VeDirectory::TYPE_ZIP:
		{
			pcTemp = VeStrtok(NULL, "#", &pcContext);
			VeZipPtr spZip = g_pResourceManager->GetZip(pcTemp);
			pcTemp = VeStrtok(NULL, "#", &pcContext);
			pkDir = VE_NEW VeZipDir(pcTemp ? pcTemp : "", spZip);
		}
		break;
#if defined(VE_PLATFORM_ANDROID)
	case VeDirectory::TYPE_ASSET:
		{
			pcTemp = VeStrtok(NULL, "#", &pcContext);
			pkDir = VE_NEW VeAssetDir(pcTemp ? pcTemp : "");
		}
		break;
#endif
	case VeDirectory::TYPE_URL:
		{
			pcTemp = VeStrtok(NULL, "#", &pcContext);
			pkDir = VE_NEW VeURLDir(pcTemp ? pcTemp : "");
		}
		break;
    default:
        break;
	}
	return pkDir;
}
//--------------------------------------------------------------------------
void VeResourceManager::DestoryDir(VeDirectory* pkDir)
{
	VE_ASSERT(pkDir);
	VE_DELETE(pkDir);
}
//--------------------------------------------------------------------------
VeZipPtr VeResourceManager::GetZip(const VeChar8* pcName, bool bLoad)
{
	VE_AUTO_LOCK_MUTEX(m_kZipMutex);
	VeStringMap<VeZip*>::iterator it = m_kActiveZipMap.Find(pcName);
	if(it)
	{
		return *it;
	}
	else if(!bLoad)
	{
		return NULL;
	}
	else
	{
		VeZipPtr spZip = VeZip::LoadZip(pcName);
		if(spZip)
		{
			m_kActiveZipMap.Insert(pcName, spZip);
		}
		return spZip;
	}
}
//--------------------------------------------------------------------------
bool VeResourceManager::HasBGTask()
{
	for(VeUInt32 i(0); i < BG_QUEUE_NUM; ++i)
	{
		if(m_kBGQueue[i].HasTask())
		{
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------
void VeResourceManager::SetResourceCreateFunction(VeResource::Type eType, ResourceCreateFunc pfuncCreate)
{
	VE_ASSERT(eType < VeResource::TYPE_NUM);
	if(!pfuncCreate)
	{
		for(VeUInt32 i(0); i < m_akResourcePools[eType].m_kVec.Size(); ++i)
		{
			m_akResourcePools[eType].m_kVec[i]->Unload();
		}
		m_akResourcePools[eType].m_kMap.Clear();
		m_akResourcePools[eType].m_kVec.Clear();

	}
	m_akResourcePools[eType].m_pfuncCreateFunc = pfuncCreate;
	
}
//--------------------------------------------------------------------------
#ifdef VE_MEM_DEBUG
VeResourcePtr VeResourceManager::GetResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup, const VeChar8* pcSourceFile, VeInt32 i32SourceLine, const VeChar8* pcFunction)
#else
VeResourcePtr VeResourceManager::GetResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup)
#endif
{
	ResourcePool& kPool = m_akResourcePools[eType];
	VeUInt32* pu32Pos = kPool.m_kMap.Find(pcName);
	if(pu32Pos)
	{
		return kPool.m_kVec[*pu32Pos];
	}
	else if(pcGroup)
	{
		VE_ASSERT(kPool.m_pfuncCreateFunc);
		VeResourcePtr spRes = kPool.m_pfuncCreateFunc(pcName, pcGroup);
		if(spRes)
		{
			kPool.m_kMap.Insert(pcName, kPool.m_kVec.Size());
			kPool.m_kVec.PushBack(spRes);
		}
		return spRes;
	}
	else
	{
		return NULL;
	}
}
//--------------------------------------------------------------------------
#ifdef VE_MEM_DEBUG
VeResourcePtr VeResourceManager::LoadResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup, bool bBackground, const VeChar8* pcSourceFile, VeInt32 i32SourceLine, const VeChar8* pcFunction)
#else
VeResourcePtr VeResourceManager::LoadResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup, bool bBackground)
#endif
{
	VeResourcePtr spRes;
#	ifdef VE_MEM_DEBUG
	spRes = GetResource(eType, pcName, pcGroup, pcSourceFile, i32SourceLine, pcFunction);
#	else
	spRes = GetResource(eType, pcName, pcGroup);
#	endif
	if(spRes)
	{
		if(bBackground)
		{
			spRes->Load();
		}
		else
		{
			spRes->LoadNow();
		}
	}
	return spRes;
}
//--------------------------------------------------------------------------
VeResourceManager::ResourcePool::ResourcePool()
	: m_pfuncCreateFunc(NULL)
{

}
//--------------------------------------------------------------------------
