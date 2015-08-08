////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeRenderObjectGLES2.h
//  Version:     v1.00
//  Created:     14/11/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef VE_ENABLE_GLES2

class VeROBlendStateGLES2 : public VeROBlendState
{
	VeDeclareRTTI;
public:
	VeROBlendStateGLES2(const VeChar8* pcName);

	virtual ~VeROBlendStateGLES2();

	bool m_bAlphaToCoverageEnable;
	bool m_bBlendEnable;
	VeUInt16 m_u16SrcBlend;
	VeUInt16 m_u16DstBlend;
	VeUInt16 m_u16BlendOp;
	VeUInt16 m_u16SrcBlendAlpha;
	VeUInt16 m_u16DstBlendAlpha;
	VeUInt16 m_u16BlendOpAlpha;
	VeUInt16 m_u16WriteMask;
};

VeSmartPointer(VeROBlendStateGLES2);

class VeRODepthStencilStateGLES2 : public VeRODepthStencilState
{
	VeDeclareRTTI;
public:
	VeRODepthStencilStateGLES2(const VeChar8* pcName);

	virtual ~VeRODepthStencilStateGLES2();

	bool m_bDepthEnable;
	bool m_bDepthWrite;
	VeUInt16 m_u16DepthFunc;
	bool m_bStencilEnable;
	VeUInt8 m_u8StencilReadMask;
	VeUInt8 m_u8StencilWriteMask;
	VeUInt8 m_u8Aligned;
	VeUInt16 m_u16FrontFailOp;
	VeUInt16 m_u16FrontDepthFailOp;
	VeUInt16 m_u16FrontPassOp;
	VeUInt16 m_u16FrontFunc;
	VeUInt16 m_u16BackFailOp;
	VeUInt16 m_u16BackDepthFailOp;
	VeUInt16 m_u16BackPassOp;
	VeUInt16 m_u16BackFunc;
	
};

VeSmartPointer(VeRODepthStencilStateGLES2);

class VeROInputLayoutGLES2 : public VeROInputLayout
{
	VeDeclareRTTI;
public:
	struct InputElementDesc
	{
		VeRenderer::Semantic m_eSemanticType;
		VeUInt32 m_u32SemanticIndex;
		VeUInt8 m_u8Size;
		VeUInt8 m_u8Normalized;
		VeUInt16 m_u16Type;
		VeUInt16 m_u16InputSlot;
		VeUInt16 m_u16AlignedByteOffset;

		void SetFormat(VeRenderer::Format eFormat);
	};

	VeROInputLayoutGLES2(const VeChar8* pcName);

	virtual ~VeROInputLayoutGLES2();

	VeVector<InputElementDesc> m_kElementDescArray;
};

VeSmartPointer(VeROInputLayoutGLES2);

class VeRORasterizeStateGLES2 : public VeRORasterizeState
{
	VeDeclareRTTI;
public:
	VeRORasterizeStateGLES2(const VeChar8* pcName);

	virtual ~VeRORasterizeStateGLES2();

	bool m_bCullEnable;
	bool m_bDepthBiasEnable;
	bool m_bMultisampleEnable;
	bool m_bScissorEnable;
	VeUInt16 m_u16CullFace;
	VeUInt16 m_u16FrontFace;
	VeFloat32 m_f32PolyFactor;
	VeFloat32 m_f32PolyUnits;

};

VeSmartPointer(VeRORasterizeStateGLES2);

class VeROSamplerStateGLES2 : public VeROSamplerState
{
	VeDeclareRTTI;
public:
	VeROSamplerStateGLES2(const VeChar8* pcName);

	virtual ~VeROSamplerStateGLES2();

	VeRenderer::Filter m_eFilter;
	VeRenderer::TextureAddressMode m_eAddressU;
	VeRenderer::TextureAddressMode m_eAddressV;
	VeUInt32 m_u32MaxAnisotropy;

};

VeSmartPointer(VeROSamplerStateGLES2);

class VeROBufferGLES2 : public VeROBuffer
{
	VeDeclareRTTI;
public:
	VeROBufferGLES2(const VeChar8* pcName, VeUInt32 u32Target, VeUInt32 u32Buffer, VeUInt32 u32ByteWidth);

	VeROBufferGLES2(const VeChar8* pcName, VeUInt32 u32Target, void* pvBuffer, VeUInt32 u32ByteWidth);

	virtual ~VeROBufferGLES2();

	virtual VeUInt32 GetBindFlag();

	virtual VeUInt32 GetCpuAccessFlag();

	virtual VeUInt32 GetResourceMiscFlag();

	VeUInt32 m_u32Target;
	union
	{
		VeUInt32 m_u32Buffer;
		void* m_pvBuffer;
	};
	VeUInt32 m_u32ByteWidth;
};

VeSmartPointer(VeROBufferGLES2);

VeSmartPointer(VeROTexture2DGLES2);

class VeRODepthStencilViewGLES2 : public VeRODepthStencilView
{
	VeDeclareRTTI;
public:
	VeRODepthStencilViewGLES2(const VeChar8* pcName, const VeROTexture2DGLES2Ptr& spTex, VeUInt32 u32Level);

	virtual ~VeRODepthStencilViewGLES2();

	VeROTexture2DGLES2Ptr m_spTexture;
	VeUInt32 m_u32Level;

};

VeSmartPointer(VeRODepthStencilViewGLES2);

class VeRORenderTargetViewGLES2 : public VeRORenderTargetView
{
	VeDeclareRTTI;
public:
	VeRORenderTargetViewGLES2(const VeChar8* pcName, const VeROTexture2DGLES2Ptr& spTex, VeUInt32 u32Level);

	virtual ~VeRORenderTargetViewGLES2();

	VeROTexture2DGLES2Ptr m_spTexture;
	VeUInt32 m_u32Level;

};

VeSmartPointer(VeRORenderTargetViewGLES2);

class VeROShaderResourceViewGLES2 : public VeROShaderResourceView
{
	VeDeclareRTTI;
public:
	VeROShaderResourceViewGLES2(const VeChar8* pcName, const VeROTexture2DGLES2Ptr& spTex);

	virtual ~VeROShaderResourceViewGLES2();

	VeROTexture2DGLES2Ptr m_spTexture;
	VeUInt32 m_u32MinFilter;
	VeUInt32 m_u32MagFilter;
	VeUInt32 m_u32WrapS;
	VeUInt32 m_u32WrapT;
	VeUInt32 m_u32MaxAnisotropy;

};

VeSmartPointer(VeROShaderResourceViewGLES2);

class VeROTexture2DGLES2 : public VeROTexture2D
{
	VeDeclareRTTI;
public:
	VeROTexture2DGLES2(const VeChar8* pcName);

	virtual ~VeROTexture2DGLES2();

	VeUInt32 m_u32Target;
	VeUInt32 m_u32Texture;
	VeUInt32 m_u32Width;
	VeUInt32 m_u32Height;
	VeUInt32 m_u32MipLevels;

};

class VeROPixelShaderGLES2 : public VeROPixelShader
{
	VeDeclareRTTI;
public:
	VeROPixelShaderGLES2(const VeChar8* pcName, VeUInt32 u32Shader);

	virtual ~VeROPixelShaderGLES2();

	virtual bool Update(VeMemoryIStream& kData);

	VeUInt32 m_u32Shader;

};

VeSmartPointer(VeROPixelShaderGLES2);

class VeROVertexShaderGLES2 : public VeROVertexShader
{
	VeDeclareRTTI;
public:
	VeROVertexShaderGLES2(const VeChar8* pcName, VeUInt32 u32Shader);

	virtual ~VeROVertexShaderGLES2();

	virtual bool Update(VeMemoryIStream& kData);

	VeUInt32 m_u32Shader;

};

VeSmartPointer(VeROVertexShaderGLES2);

#ifdef VE_PLATFORM_IOS

class VeMSAAResolveGLES2APPLE : public VeRenderProcess
{
public:
	VeMSAAResolveGLES2APPLE(const VeChar8* pcName);

	virtual ~VeMSAAResolveGLES2APPLE();

	virtual bool Update(VeBinaryIStream& kStream);
	
	VeUInt32 m_u32ReadBuffer;
	VeUInt32 m_u32DrawBuffer;

};

VeSmartPointer(VeMSAAResolveGLES2APPLE);

#endif

#endif
