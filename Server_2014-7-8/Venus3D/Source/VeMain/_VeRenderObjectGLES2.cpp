////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeRenderObjectGLES2.cpp
//  Version:     v1.00
//  Created:     14/11/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#include "_VeRenderObjectGLES2.h"
#include "_VeRendererGLES2.h"

//--------------------------------------------------------------------------
#ifdef VE_ENABLE_GLES2
//--------------------------------------------------------------------------
VeImplementRTTI(VeROBlendStateGLES2, VeROBlendState);
//--------------------------------------------------------------------------
VeROBlendStateGLES2::VeROBlendStateGLES2(const VeChar8* pcName)
	: VeROBlendState(pcName), m_bAlphaToCoverageEnable(false), m_bBlendEnable(false)
	, m_u16WriteMask(VeRenderer::COLOR_WRITE_ENABLE_ALL)
{

}
//--------------------------------------------------------------------------
VeROBlendStateGLES2::~VeROBlendStateGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRODepthStencilStateGLES2, VeRODepthStencilState);
//--------------------------------------------------------------------------
VeRODepthStencilStateGLES2::VeRODepthStencilStateGLES2(const VeChar8* pcName)
	: VeRODepthStencilState(pcName), m_bDepthEnable(false), m_bStencilEnable(false)
{


}
//--------------------------------------------------------------------------
VeRODepthStencilStateGLES2::~VeRODepthStencilStateGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROInputLayoutGLES2, VeROInputLayout);
//--------------------------------------------------------------------------
VeROInputLayoutGLES2::VeROInputLayoutGLES2(const VeChar8* pcName)
	: VeROInputLayout(pcName)
{

}
//--------------------------------------------------------------------------
VeROInputLayoutGLES2::~VeROInputLayoutGLES2()
{

}
//--------------------------------------------------------------------------
void VeROInputLayoutGLES2::InputElementDesc::SetFormat(VeRenderer::Format eFormat)
{
	switch(eFormat)
	{
	case VeRenderer::FMT_R32_FLOAT:
		m_u16Type = GL_FLOAT;
		m_u8Size = 1;
		m_u8Normalized = GL_FALSE;
		break;
	case VeRenderer::FMT_R32G32_FLOAT:
		m_u16Type = GL_FLOAT;
		m_u8Size = 2;
		m_u8Normalized = GL_FALSE;
		break;
	case VeRenderer::FMT_R32G32B32_FLOAT:
		m_u16Type = GL_FLOAT;
		m_u8Size = 3;
		m_u8Normalized = GL_FALSE;
		break;
	case VeRenderer::FMT_R32G32B32A32_FLOAT:
		m_u16Type = GL_FLOAT;
		m_u8Size = 4;
		m_u8Normalized = GL_FALSE;
		break;
	case VeRenderer::FMT_R8G8B8A8_UNORM:
		m_u16Type = GL_UNSIGNED_BYTE;
		m_u8Size = 4;
		m_u8Normalized = GL_TRUE;
		break;
	default:
		VE_ASSERT(!"Wrong input type");
		break;
	}
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRORasterizeStateGLES2, VeRORasterizeState);
//--------------------------------------------------------------------------
VeRORasterizeStateGLES2::VeRORasterizeStateGLES2(const VeChar8* pcName)
	: VeRORasterizeState(pcName)
{

}
//--------------------------------------------------------------------------
VeRORasterizeStateGLES2::~VeRORasterizeStateGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROSamplerStateGLES2, VeROSamplerState);
//--------------------------------------------------------------------------
VeROSamplerStateGLES2::VeROSamplerStateGLES2(const VeChar8* pcName)
	: VeROSamplerState(pcName), m_eFilter(VeRenderer::FILTER_MIN_MAG_MIP_POINT)
	, m_eAddressU(VeRenderer::TEXTURE_ADDRESS_CLAMP)
	, m_eAddressV(VeRenderer::TEXTURE_ADDRESS_CLAMP)
	, m_u32MaxAnisotropy(0)
{

}
//--------------------------------------------------------------------------
VeROSamplerStateGLES2::~VeROSamplerStateGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROBufferGLES2, VeROBuffer);
//--------------------------------------------------------------------------
VeROBufferGLES2::VeROBufferGLES2(const VeChar8* pcName, VeUInt32 u32Target,
	VeUInt32 u32Buffer, VeUInt32 u32ByteWidth)
	: VeROBuffer(pcName), m_u32Target(u32Target), m_u32Buffer(u32Buffer)
	, m_u32ByteWidth(u32ByteWidth)
{

}
//--------------------------------------------------------------------------
VeROBufferGLES2::VeROBufferGLES2(const VeChar8* pcName, VeUInt32 u32Target,
	void* pvBuffer, VeUInt32 u32ByteWidth)
	: VeROBuffer(pcName), m_u32Target(u32Target), m_pvBuffer(pvBuffer)
	, m_u32ByteWidth(u32ByteWidth)
{

}
//--------------------------------------------------------------------------
VeROBufferGLES2::~VeROBufferGLES2()
{
	if(m_u32Target == VeRenderer::BIND_CONSTANT_BUFFER)
	{
		if(m_pvBuffer)
		{
			VeFree(m_pvBuffer);
			m_pvBuffer = NULL;
		}
	}
	else if(m_u32Buffer)
	{
		if(m_u32Target == GL_RENDERBUFFER)
		{
			glDeleteRenderbuffers(1, &m_u32Buffer);
		}
		else
		{
			glDeleteBuffers(1, &m_u32Buffer);
		}
		m_u32Buffer = 0;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeROBufferGLES2::GetBindFlag()
{
	switch(m_u32Target)
	{
	case VeRenderer::BIND_CONSTANT_BUFFER:
		return VeRenderer::BIND_CONSTANT_BUFFER;
	case GL_ARRAY_BUFFER:
		return VeRenderer::BIND_VERTEX_BUFFER;
	case GL_ELEMENT_ARRAY_BUFFER:
		return VeRenderer::BIND_INDEX_BUFFER;
	case GL_RENDERBUFFER:
		return VeRenderer::BIND_RENDER_TARGET | VeRenderer::BIND_DEPTH_STENCIL;
	default:
		return 0;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeROBufferGLES2::GetCpuAccessFlag()
{
	return VeRenderer::CPU_ACCESS_WRITE & VeRenderer::CPU_ACCESS_READ;
}
//--------------------------------------------------------------------------
VeUInt32 VeROBufferGLES2::GetResourceMiscFlag()
{
	return 0;
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRODepthStencilViewGLES2, VeRODepthStencilView);
//--------------------------------------------------------------------------
VeRODepthStencilViewGLES2::VeRODepthStencilViewGLES2(const VeChar8* pcName, const VeROTexture2DGLES2Ptr& spTex, VeUInt32 u32Level)
	: VeRODepthStencilView(pcName), m_spTexture(spTex), m_u32Level(u32Level)
{

}
//--------------------------------------------------------------------------
VeRODepthStencilViewGLES2::~VeRODepthStencilViewGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRORenderTargetViewGLES2, VeRORenderTargetView);
//--------------------------------------------------------------------------
VeRORenderTargetViewGLES2::VeRORenderTargetViewGLES2(const VeChar8* pcName, const VeROTexture2DGLES2Ptr& spTex, VeUInt32 u32Level)
	: VeRORenderTargetView(pcName), m_spTexture(spTex), m_u32Level(u32Level)
{

}
//--------------------------------------------------------------------------
VeRORenderTargetViewGLES2::~VeRORenderTargetViewGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROShaderResourceViewGLES2, VeROShaderResourceView);
//--------------------------------------------------------------------------
VeROShaderResourceViewGLES2::VeROShaderResourceViewGLES2(const VeChar8* pcName, const VeROTexture2DGLES2Ptr& spTex)
	: VeROShaderResourceView(pcName), m_spTexture(spTex)
{
	m_u32MinFilter = GL_NEAREST;
	m_u32MagFilter = GL_NEAREST;
	m_u32WrapS = GL_CLAMP_TO_EDGE;
	m_u32WrapT = GL_CLAMP_TO_EDGE;
	m_u32MaxAnisotropy = 1;
}
//--------------------------------------------------------------------------
VeROShaderResourceViewGLES2::~VeROShaderResourceViewGLES2()
{

}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROTexture2DGLES2, VeROTexture2D);
//--------------------------------------------------------------------------
VeROTexture2DGLES2::VeROTexture2DGLES2(const VeChar8* pcName)
	: VeROTexture2D(pcName), m_u32Target(0), m_u32Texture(0)
	, m_u32Width(0), m_u32Height(0), m_u32MipLevels(1)
{

}
//--------------------------------------------------------------------------
VeROTexture2DGLES2::~VeROTexture2DGLES2()
{
	if(m_u32Texture)
	{
		if(m_u32Target == GL_TEXTURE_2D)
		{
			glDeleteTextures(1, &m_u32Texture);
		}
		else if(m_u32Target == GL_RENDERBUFFER)
		{
			glDeleteRenderbuffers(1, &m_u32Texture);
		}
		else
		{
			VE_ASSERT(!"Unknown texture type");
		}
		m_u32Texture = 0;
	}
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROPixelShaderGLES2, VeROPixelShader);
//--------------------------------------------------------------------------
VeROPixelShaderGLES2::VeROPixelShaderGLES2(const VeChar8* pcName, VeUInt32 u32Shader)
	: VeROPixelShader(pcName), m_u32Shader(u32Shader)
{

}
//--------------------------------------------------------------------------
VeROPixelShaderGLES2::~VeROPixelShaderGLES2()
{
	if(m_u32Shader)
	{
		glDeleteShader(m_u32Shader);
		m_u32Shader = 0;
	}
}
//--------------------------------------------------------------------------
bool VeROPixelShaderGLES2::Update(VeMemoryIStream& kData)
{
	const GLchar* pcShader = (const VeChar8*)kData.GetData();
	GLint iLen = kData.RemainingLength();
	kData.Finish();
	VeUInt32 u32Shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(u32Shader, 1, &pcShader, &iLen);
	glCompileShader(u32Shader);
	GLint iShaderCompiled(0);
	glGetShaderiv(u32Shader, GL_COMPILE_STATUS, &iShaderCompiled);
	if(iShaderCompiled)
	{
		if(m_u32Shader)
		{
			glDeleteShader(m_u32Shader);
		}
		m_u32Shader = u32Shader;
		return true;
	}
	else
	{
		glDeleteShader(u32Shader);
		return false;
	}
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROVertexShaderGLES2, VeROVertexShader);
//--------------------------------------------------------------------------
VeROVertexShaderGLES2::VeROVertexShaderGLES2(const VeChar8* pcName, VeUInt32 u32Shader)
	: VeROVertexShader(pcName), m_u32Shader(u32Shader)
{

}
//--------------------------------------------------------------------------
VeROVertexShaderGLES2::~VeROVertexShaderGLES2()
{
	if(m_u32Shader)
	{
		glDeleteShader(m_u32Shader);
		m_u32Shader = 0;
	}
}
//--------------------------------------------------------------------------
bool VeROVertexShaderGLES2::Update(VeMemoryIStream& kData)
{
	const GLchar* pcShader = (const VeChar8*)kData.GetData();
	GLint iLen = kData.RemainingLength();
	kData.Finish();
	VeUInt32 u32Shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(u32Shader, 1, &pcShader, &iLen);
	glCompileShader(u32Shader);
	GLint iShaderCompiled(0);
	glGetShaderiv(u32Shader, GL_COMPILE_STATUS, &iShaderCompiled);
	if(iShaderCompiled)
	{
		if(m_u32Shader)
		{
			glDeleteShader(m_u32Shader);
		}
		m_u32Shader = u32Shader;
		return true;
	}
	else
	{
		glDeleteShader(u32Shader);
		return false;
	}
}
//--------------------------------------------------------------------------
#ifdef VE_PLATFORM_IOS
//--------------------------------------------------------------------------
VeMSAAResolveGLES2APPLE::VeMSAAResolveGLES2APPLE(const VeChar8* pcName)
	: VeRenderProcess(TYPE_MSAA_RESOLVE, pcName)
{

}
//--------------------------------------------------------------------------
VeMSAAResolveGLES2APPLE::~VeMSAAResolveGLES2APPLE()
{

}
//--------------------------------------------------------------------------
bool VeMSAAResolveGLES2APPLE::Update(VeBinaryIStream& kStream)
{
    VeUInt16 u16RenderTargetNum, u16DepthStencilTargetNum;
    kStream >> u16RenderTargetNum;
    kStream >> u16DepthStencilTargetNum;
    if(u16RenderTargetNum == 1)
    {
        if(u16DepthStencilTargetNum)
        {
            VeRORenderTargetViewPtr spColor;
            VeRODepthStencilViewPtr spDepth;
            VeRORenderTargetViewPtr spColorAA;
            VeRODepthStencilViewPtr spDepthAA;
            VeStringA kName;
            kStream.GetStringAligned4(kName);
            g_pRenderer->GetRenderObject(kName, spColor);
            VE_ASSERT(spColor);
            kStream.GetStringAligned4(kName);
            g_pRenderer->GetRenderObject(kName, spColorAA);
            VE_ASSERT(spColorAA);
			kStream.GetStringAligned4(kName);
			g_pRenderer->GetRenderObject(kName, spDepth);
            kStream.GetStringAligned4(kName);
            g_pRenderer->GetRenderObject(kName, spDepthAA);
            m_u32DrawBuffer = ((VeRendererGLES2*)g_pRenderer)->GetFrameBuffer(VeSmartPointerCast(VeRORenderTargetViewGLES2, spColor), VeSmartPointerCast(VeRODepthStencilViewGLES2, spDepth));
            m_u32ReadBuffer = ((VeRendererGLES2*)g_pRenderer)->GetFrameBuffer(VeSmartPointerCast(VeRORenderTargetViewGLES2, spColorAA), VeSmartPointerCast(VeRODepthStencilViewGLES2, spDepthAA));
            
        }
        else
        {
            VeRORenderTargetViewPtr spColor;
            VeRORenderTargetViewPtr spColorAA;
            VeStringA kName;
            kStream.GetStringAligned4(kName);
            g_pRenderer->GetRenderObject(kName, spColor);
            VE_ASSERT(spColor);
            kStream.GetStringAligned4(kName);
            g_pRenderer->GetRenderObject(kName, spColorAA);
            VE_ASSERT(spColorAA);
            m_u32DrawBuffer = ((VeRendererGLES2*)g_pRenderer)->GetFrameBuffer(VeSmartPointerCast(VeRORenderTargetViewGLES2, spColor), NULL);
            m_u32ReadBuffer = ((VeRendererGLES2*)g_pRenderer)->GetFrameBuffer(VeSmartPointerCast(VeRORenderTargetViewGLES2, spColorAA), NULL);
        }
        return true;
    }
    else
    {
        VeStringA kTemp;
        for(VeUInt32 i(0); i < VeUInt32((u16RenderTargetNum + u16DepthStencilTargetNum) * 2); ++i)
        {
            kStream.GetStringAligned4(kTemp);
        }
        return false;
    }
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
