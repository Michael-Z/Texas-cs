////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeResource.h
//  Version:     v1.00
//  Created:     8/11/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_POWER_API VeResourceGroup : public VeRefObject
{
public:
	VeResourceGroup(const VeChar8* pcName);

	~VeResourceGroup();

	const VeChar8* GetName();

	bool SetWriteDirectory(const VeChar8* pcName);

	bool PushReadDirectoryBack(const VeChar8* pcName);

	bool PushReadDirectoryFront(const VeChar8* pcName);

	void PopBackReadDirectory();

	void PopFrontReadDirectory();

	void Clear();

	VeUInt32 ReadDirNum();

	bool CanWrite();

protected:
	friend class VeResource;
	const VeFixedStringA m_kName;
	VeList<VeDirectory*> m_kReadDirectories;
	VeDirectory* m_pkWriteDirectory;

};

VeSmartPointer(VeResourceGroup);

class VE_POWER_API VeResource : public VeRefObject
{
public:
	enum Type
	{
		VERTEX_SHADER,
		PIXEL_SHADER,
		MATERIAL,
		SIMPLE_SOUND,
		TYPE_NUM
	};

	enum State
	{
		UNLOADED,
		LOADING,
		LOADED,
		INVALID
	};

	typedef VeMemberDelegate<VeResource, const VeMemoryIStreamPtr&> ReadDelegate;

	VeResource(Type eType, const VeChar8* pcName, const VeChar8* pcGroup);

	virtual ~VeResource();

	virtual bool LoadNow();

	virtual bool Load();

	virtual void Unload();

	virtual const VeChar8* GetLoadName();

	const VeChar8* GetName();

	void SetGroup(const VeChar8* pcGroup);

	const VeChar8* GetGroup();

	Type GetType();

	State GetState();

	void Wait(VeResource* pkRes);

protected:
	friend class VeResourceManager;	

	virtual void AsyncReadCallback(const VeMemoryIStreamPtr& spData);

	virtual void WaitResLoaded(VeResource* pkRes);

	virtual State LoadImpl(const VeMemoryIStreamPtr& spStream) { return LOADED; }

	virtual void UnloadImpl() {}

	virtual State AllWaitResLoaded() { return LOADED; }

	const Type m_eType;
	VeFixedStringA m_kName;
	VeResourceGroupPtr m_spGroup;
	volatile State m_eState;
	ReadDelegate m_kReadDelegate;
	VeList<VeResource*> m_kTickWaitList;
	VeUInt32 m_u32WaitNumber;
	
};

VeSmartPointer(VeResource);
