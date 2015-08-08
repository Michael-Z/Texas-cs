////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeResourceManager.h
//  Version:     v1.00
//  Created:     6/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_POWER_API VeResourceManager : public VeSingleton<VeResourceManager>
{
public:
	enum BGQueue
	{
		BG_QUEUE_FILE,
		BG_QUEUE_NUM
	};

	typedef VeStringMap<VeUInt32> ResourceMap;
	typedef VeVector<VeResourcePtr> ResourceVec;

	typedef VeResource* (*ResourceCreateFunc)(const VeChar8* pcName, const VeChar8* pcGroup);

	VeResourceManager();

	~VeResourceManager();

	void SetResourceCreateFunction(VeResource::Type eType, ResourceCreateFunc pfuncCreate);

#ifdef VE_MEM_DEBUG
	VeResourcePtr GetResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup, const VeChar8* pcSourceFile, VeInt32 i32SourceLine, const VeChar8* pcFunction);
#else
	VeResourcePtr GetResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup);
#endif

#ifdef VE_MEM_DEBUG
	VeResourcePtr LoadResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup, bool bBackground, const VeChar8* pcSourceFile, VeInt32 i32SourceLine, const VeChar8* pcFunction);
#else
	VeResourcePtr LoadResource(VeResource::Type eType, const VeChar8* pcName, const VeChar8* pcGroup, bool bBackground);
#endif

	void AddBGTask(BGQueue eQueue, const VeBackgroundTaskPtr& spTask);

	void AddFGTask(BGQueue eQueue, const VeBackgroundTaskPtr& spTask);

	void Update();

	VeDirectory* CreateDir(const VeChar8* pcName);

	void DestoryDir(VeDirectory* pkDir);

	VeZipPtr GetZip(const VeChar8* pcName, bool bLoad = true);

	bool HasBGTask();

protected:
	friend class VeResourceGroup;
	friend class VeResource;
	friend class VeZip;
	friend class VeURL;

	VeStringMap<VeResourceGroupPtr> m_kGroups;
	struct ResourcePool
	{
		ResourcePool();
		ResourceMap m_kMap;
		ResourceVec m_kVec;
		ResourceCreateFunc m_pfuncCreateFunc;
	} m_akResourcePools[VeResource::TYPE_NUM];
	
	VeStringMap<VeZip*> m_kActiveZipMap;
	VeThread::Mutex m_kZipMutex;
	VeBackgroundTaskQueue m_kBGQueue[BG_QUEUE_NUM];
};

#define g_pResourceManager VeResourceManager::GetSingletonPtr()

#ifdef VE_MEM_DEBUG

#define VeGetResource(type,name,group) (g_pResourceManager->GetResource(type,name,group,__FILE__, __LINE__, __FUNCTION__))

#define VeLoadResource(type,name,group,async) (g_pResourceManager->LoadResource(type,name,group,async,__FILE__, __LINE__, __FUNCTION__))

#else

#define VeGetResource(type,name,group) (g_pResourceManager->GetResource(type,name,group))

#define VeLoadResource(type,name,group,async) (g_pResourceManager->LoadResource(type,name,group,async))

#endif

#define VeLoadResourceSync(type,name,group) VeLoadResource(type,name,group,false)

#define VeLoadResourceAsync(type,name,group) VeLoadResource(type,name,group,true)
