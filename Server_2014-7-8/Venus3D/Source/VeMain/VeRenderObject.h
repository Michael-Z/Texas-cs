////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeRenderObject.h
//  Version:     v1.00
//  Created:     30/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_MAIN_API VeRenderObject : public VeRefObject
{
	VeDeclareRootRTTI(VeRenderObject);
public:
	enum Type
	{
		TYPE_BLEND_STATE,
		TYPE_COUNTER,
		TYPE_DEPTH_STENCIL_STATE,
		TYPE_INPUT_LAYOUT,
		TYPE_PREDICATE,
		TYPE_QUERY,
		TYPE_RASTERIZER_STATE,
		TYPE_SAMPLER_STATE,
		
		TYPE_BUFFER,
		TYPE_DEPTH_STENCIL_VIEW,
		TYPE_RENDER_TARGET_VIEW,
		TYPE_SHADER_RESOURCE_VIEW,
		TYPE_TEXTURE_1D,
		TYPE_TEXTURE_2D,
		TYPE_TEXTURE_3D,
		TYPE_UNORDERED_ACCESS_VIEW,

		TYPE_CLASS_LINKAGE,
		TYPE_COMPUTE_SHADER,
		TYPE_DOMAIN_SHADER,
		TYPE_GEOMETRY_SHADER,
		TYPE_HULL_SHADER,
		TYPE_PIXEL_SHADER,
		TYPE_VERTEX_SHADER,

		TYPE_MAX
	};

	VE_INLINE Type GetType();

	VE_INLINE const VeChar8* GetName();

	virtual bool Update(VeBinaryIStream& kStream);

	virtual void OnAdd() {}

	virtual void OnDel() {}

protected:
	VeRenderObject(Type eType, const VeChar8* pcName);

	virtual ~VeRenderObject();

	const Type m_eType;
	VeFixedStringA m_kName;

};

VeSmartPointer(VeRenderObject);

class VE_MAIN_API VeROBlendState : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROBlendState(const VeChar8* pcName);

};

VeSmartPointer(VeROBlendState);

class VE_MAIN_API VeRODepthStencilState : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeRODepthStencilState(const VeChar8* pcName);

};

VeSmartPointer(VeRODepthStencilState);

class VE_MAIN_API VeROInputLayout : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROInputLayout(const VeChar8* pcName);

};

VeSmartPointer(VeROInputLayout);

class VE_MAIN_API VeRORasterizeState : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeRORasterizeState(const VeChar8* pcName);

};

VeSmartPointer(VeRORasterizeState);

class VE_MAIN_API VeROSamplerState : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROSamplerState(const VeChar8* pcName);

};

VeSmartPointer(VeROSamplerState);

class VE_MAIN_API VeROBuffer : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

	virtual VeUInt32 GetBindFlag() = 0;

	virtual VeUInt32 GetCpuAccessFlag() = 0;

	virtual VeUInt32 GetResourceMiscFlag() = 0;

protected:
	VeROBuffer(const VeChar8* pcName);

};

VeSmartPointer(VeROBuffer);

class VE_MAIN_API VeRODepthStencilView : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeRODepthStencilView(const VeChar8* pcName);

};

VeSmartPointer(VeRODepthStencilView);

class VE_MAIN_API VeRORenderTargetView : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeRORenderTargetView(const VeChar8* pcName);

};

VeSmartPointer(VeRORenderTargetView);

class VE_MAIN_API VeROShaderResourceView : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROShaderResourceView(const VeChar8* pcName);

};

VeSmartPointer(VeROShaderResourceView);

class VE_MAIN_API VeROTexture1D : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROTexture1D(const VeChar8* pcName);

};

VeSmartPointer(VeROTexture1D);

class VE_MAIN_API VeROTexture2D : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROTexture2D(const VeChar8* pcName);

};

VeSmartPointer(VeROTexture2D);

class VE_MAIN_API VeROTexture3D : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROTexture3D(const VeChar8* pcName);

};

VeSmartPointer(VeROTexture3D);

class VE_MAIN_API VeROComputeShader : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROComputeShader(const VeChar8* pcName);

};

VeSmartPointer(VeROComputeShader);

class VE_MAIN_API VeRODomainShader : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeRODomainShader(const VeChar8* pcName);

};

VeSmartPointer(VeRODomainShader);

class VE_MAIN_API VeROGeometryShader : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROGeometryShader(const VeChar8* pcName);

};

VeSmartPointer(VeROGeometryShader);

class VE_MAIN_API VeROHullShader : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROHullShader(const VeChar8* pcName);

};

VeSmartPointer(VeROHullShader);

class VE_MAIN_API VeROPixelShader : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROPixelShader(const VeChar8* pcName);

};

VeSmartPointer(VeROPixelShader);

class VE_MAIN_API VeROVertexShader : public VeRenderObject
{
	VeDeclareRTTI;
public:
	static Type ROType();

protected:
	VeROVertexShader(const VeChar8* pcName);

};

VeSmartPointer(VeROVertexShader);

#ifndef VE_NO_INLINE
#	include "VeRenderObject.inl"
#endif
