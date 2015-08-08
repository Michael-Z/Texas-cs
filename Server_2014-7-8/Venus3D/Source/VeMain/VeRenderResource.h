////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeRenderResource.h
//  Version:     v1.00
//  Created:     8/11/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_MAIN_API VeShaderResource : public VeResource
{
public:
	VeShaderResource(Type eType, const VeChar8* pcName, const VeChar8* pcGroup);

	virtual ~VeShaderResource();

	virtual const VeChar8* GetLoadName();

	VE_INLINE const VeMemoryIStreamPtr& GetData();

	static VeResource* CreateVertexShaderResource(const VeChar8* pcName, const VeChar8* pcGroup);

	static VeResource* CreatePixelShaderResource(const VeChar8* pcName, const VeChar8* pcGroup);

protected:
	virtual State LoadImpl(const VeMemoryIStreamPtr& spStream);

	virtual void UnloadImpl();

	VeStringA m_kLoadName;
	VeMemoryIStreamPtr m_spStream;

};

VeSmartPointer(VeShaderResource);

#define VeGetVertexShader(name,group) VeSmartPointerCast(VeShaderResource,VeGetResource(VeResource::VERTEX_SHADER,name,group))

#define VeLoadVertexShader(name,group,async) VeSmartPointerCast(VeShaderResource,VeLoadResource(VeResource::VERTEX_SHADER,name,group,async))

#define VeLoadVertexShaderSync(name,group) VeLoadVertexShader(name,group,false)

#define VeLoadVertexShaderAsync(name,group) VeLoadVertexShader(name,group,true)

#define VeGetPixelShader(name,group) VeSmartPointerCast(VeShaderResource,VeGetResource(VeResource::PIXEL_SHADER,name,group))

#define VeLoadPixelShader(name,group,async) VeSmartPointerCast(VeShaderResource,VeLoadResource(VeResource::PIXEL_SHADER,name,group,async))

#define VeLoadPixelShaderSync(name,group) VeLoadPixelShader(name,group,false)

#define VeLoadPixelShaderAsync(name,group) VeLoadPixelShader(name,group,true)

#ifndef VE_NO_INLINE
#	include "VeRenderResource.inl"
#endif
