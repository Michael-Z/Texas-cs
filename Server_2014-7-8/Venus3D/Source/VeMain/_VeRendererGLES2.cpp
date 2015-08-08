////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeRendererGLES2.cpp
//  Version:     v1.00
//  Created:     21/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#include "_VeRendererGLES2.h"

//--------------------------------------------------------------------------
#ifdef VE_ENABLE_GLES2
//--------------------------------------------------------------------------
#if defined(VE_PLATFORM_MOBILE_SIM) || defined(VE_PLATFORM_ANDROID)
//--------------------------------------------------------------------------
bool VeTestEGLError(const VeChar8* pcLocation)
{
	EGLint iErr = eglGetError();
	if(iErr != EGL_SUCCESS)
	{
		VeDebugOutput("%s failed (%d).\n", pcLocation, iErr);
		return false;
	}
	return true;
}
//--------------------------------------------------------------------------
#elif defined(VE_PLATFORM_IOS)
//--------------------------------------------------------------------------
void VeRenderbufferStorageLayer();
//--------------------------------------------------------------------------
void VeGLESPresent();
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
VeRendererGLES2::VeRendererGLES2()
#if defined(VE_PLATFORM_MOBILE_SIM)
	: m_hDeviceContext(NULL)
	, m_eglDisplay(NULL)
	, m_eglConfig(NULL)
	, m_eglSurface(NULL)
	, m_eglContext(NULL)
#elif defined(VE_PLATFORM_ANDROID)
	: m_eglDisplay(NULL)
	, m_eglConfig(NULL)
	, m_eglSurface(NULL)
	, m_eglContext(NULL)
#elif defined(VE_PLATFORM_IOS)
	: m_u32BackFrameBuffer(0)
	, m_u32BackRenderBuffer(0)
#endif
{
	m_i32MaxAnisotropy = 0;
	m_bSupportDepthTexute = false;
	m_bSupportAnisotropic = false;
	m_bSupportMSAA = false;
	m_u8MaxAASamples = 1;
}
//--------------------------------------------------------------------------
VeRendererGLES2::~VeRendererGLES2()
{

}
//--------------------------------------------------------------------------
VeRenderer::API VeRendererGLES2::GetAPI()
{
	return API_OGLES2;
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Init()
{
	if(m_bInited) return;

	GLCacheInit();

#if defined(VE_PLATFORM_MOBILE_SIM)

	m_hDeviceContext = GetDC(g_hWindow);
	m_eglDisplay = eglGetDisplay(m_hDeviceContext);
	if(m_eglDisplay == EGL_NO_DISPLAY) m_eglDisplay = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);
	EGLint iMajorVersion, iMinorVersion;
	VE_ASSERT_EQ(eglInitialize(m_eglDisplay, &iMajorVersion, &iMinorVersion), VE_TRUE);
	eglBindAPI(EGL_OPENGL_ES_API);
	VE_ASSERT_EQ(VeTestEGLError("eglBindAPI"), VE_TRUE);
	const EGLint pi32ConfigAttribs[] =
	{
		EGL_LEVEL,				0,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,			8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE,			8,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_NATIVE_RENDERABLE,	EGL_FALSE,
		EGL_DEPTH_SIZE,			EGL_DONT_CARE,
		EGL_NONE
	};
	VeInt32 i32Configs;
	VE_ASSERT_EQ(eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &i32Configs) || (i32Configs != 1), VE_TRUE);
	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, g_hWindow, NULL);
	if(m_eglConfig == EGL_NO_SURFACE)
	{
		eglGetError();
		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, NULL, NULL);
	}
	VE_ASSERT_EQ(VeTestEGLError("eglCreateWindowSurface"), VE_TRUE);
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, ai32ContextAttribs);
	VE_ASSERT_EQ(VeTestEGLError("eglCreateContext"), VE_TRUE);
	eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
	VE_ASSERT_EQ(VeTestEGLError("eglMakeCurrent"), VE_TRUE);

#elif defined(VE_PLATFORM_ANDROID)

	m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	VE_ASSERT_EQ(eglInitialize(m_eglDisplay, NULL, NULL), VE_TRUE);
	eglBindAPI(EGL_OPENGL_ES_API);
	VE_ASSERT_EQ(VeTestEGLError("eglBindAPI"), VE_TRUE);

	const EGLint pi32ConfigAttribs[] =
	{
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,			8,
		EGL_BLUE_SIZE,			8,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_DEPTH_SIZE,			EGL_DONT_CARE,
		EGL_NONE
	};

	VeInt32 i32Configs;
	VE_ASSERT_EQ(eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &i32Configs), VE_TRUE);
	VE_ASSERT(i32Configs == 1);

	VeInt32 i32VisualID;
	eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_NATIVE_VISUAL_ID, &i32VisualID);
	ANativeWindow_setBuffersGeometry(g_pkAppState->window, 0, 0, i32VisualID);

	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, g_pkAppState->window, NULL);
	VE_ASSERT_EQ(VeTestEGLError("eglCreateWindowSurface"), VE_TRUE);

	VeInt32 ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, ai32ContextAttribs);
	VE_ASSERT_EQ(VeTestEGLError("eglCreateContext"), VE_TRUE);

	eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);

#elif defined(VE_PLATFORM_IOS)

	glGenFramebuffers(1, &m_u32BackFrameBuffer);
	BindFrameBuffer(m_u32BackFrameBuffer);
	glGenRenderbuffers(1, &m_u32BackRenderBuffer);
	BindRenderBuffer(m_u32BackRenderBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_u32BackRenderBuffer);
	VeRenderbufferStorageLayer();
	VE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

#endif

	UpdateBufferSize();
	GLInitGlobal();
	ResetStateCache();

	m_bInited = true;
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Term()
{
	if(!m_bInited) return;

	GLTermGlobal();

	for(VeVector<VeRenderObjectPtr>::iterator it = m_kObjectArray.Begin(); it != m_kObjectArray.End(); ++it)
	{
		(*it)->OnDel();
	}
	m_kObjectArray.Clear();
	for(VeUInt32 i(0); i < VeRenderObject::TYPE_MAX; ++i)
	{
		m_akObjectMap[i].Clear();
	}
	for(VeVector<VeRenderProcessPtr>::iterator it = m_kProcessArray.Begin(); it != m_kProcessArray.End(); ++it)
	{
		(*it)->OnDel();
	}
	m_kProcessMap.Clear();
	m_kProcessArray.Clear();
	m_kNodeDataMap.Clear();
	m_kNodeDataArray.Clear();

#if defined(VE_PLATFORM_MOBILE_SIM)

	eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(m_eglDisplay);
	if(m_hDeviceContext) ReleaseDC(g_hWindow, m_hDeviceContext);

#elif defined(VE_PLATFORM_ANDROID)

	eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_eglDisplay, m_eglContext);
	eglDestroySurface(m_eglDisplay, m_eglSurface);
	eglTerminate(m_eglDisplay);

	m_eglDisplay = NULL;
	m_eglConfig = NULL;
	m_eglSurface = NULL;
	m_eglContext = NULL;

#elif defined(VE_PLATFORM_IOS)

	if(m_u32BackFrameBuffer)
	{
		glDeleteRenderbuffers(1, &m_u32BackRenderBuffer);
		glDeleteFramebuffers(1, &m_u32BackFrameBuffer);
		m_u32BackRenderBuffer = 0;
		m_u32BackFrameBuffer = 0;
	}

#endif

	m_bInited = false;
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Render()
{
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Present()
{
#	if defined(VE_PLATFORM_MOBILE_SIM)
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
#	elif defined(VE_PLATFORM_ANDROID)
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
#   elif defined(VE_PLATFORM_IOS)
    BindRenderBuffer(m_u32BackRenderBuffer);
	VeGLESPresent();
#	endif
	VeZeroMemory(m_apkCurVSCBuffers, sizeof(m_apkCurVSCBuffers));
}
//--------------------------------------------------------------------------
void VeRendererGLES2::OnBufferResized(VeUInt32 u32Width, VeUInt32 u32Height)
{
#ifdef VE_PLATFORM_MOBILE_SIM
	g_pStartConfig->SetResolution(u32Width, u32Height);
#endif
	g_pApplication->OnBufferResized(u32Width, u32Height);
}
//--------------------------------------------------------------------------
#ifdef VE_PLATFORM_MOBILE_SIM
//--------------------------------------------------------------------------
void VeRendererGLES2::ResizeWindow(VeUInt32 u32Width, VeUInt32 u32Height)
{
	VeStringParser::Resolution kResolution = g_pStartConfig->GetConfig("Resolution", kResolution);
	if(kResolution.m_u32Width != u32Width || kResolution.m_u32Height != u32Height)
	{
		g_pStartConfig->SetResolution(u32Width, u32Height);
		kResolution = g_pStartConfig->GetConfig("Resolution", kResolution);
		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		RECT kRect = { 0, 0, kResolution.m_u32Width, kResolution.m_u32Height };
		AdjustWindowRect(&kRect, dwStyle, FALSE);
		VeInt32 i32Left(CW_USEDEFAULT), i32Top(CW_USEDEFAULT);
		VeInt32 i32ScreenW = GetSystemMetrics(SM_CXSCREEN);
		VeInt32 i32ScreenH = GetSystemMetrics(SM_CYSCREEN);
		i32Left = ((i32ScreenW - kResolution.m_u32Width) >> 1) + kRect.left;
		i32Top = ((i32ScreenH - kResolution.m_u32Height) >> 1) + kRect.top;
		SetWindowPos(g_hWindow, HWND_NOTOPMOST, i32Left, i32Top, kRect.right - kRect.left, kRect.bottom - kRect.top,
			SWP_NOZORDER | SWP_NOACTIVATE);
		UpdateBufferSize();
	}
}
//--------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------
void VeRendererGLES2::OnBufferRestorage()
{
#   if defined(VE_PLATFORM_IOS)
	BindFrameBuffer(m_u32BackFrameBuffer);
	BindRenderBuffer(m_u32BackRenderBuffer);
	VeRenderbufferStorageLayer();
	VE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#   endif
	UpdateBufferSize();
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
void VeRendererGLES2::GetMatrixPerspectiveFov(VeFloat32* pf32OutMat4x4,
	VeFloat32 fov, VeFloat32 znear, VeFloat32 zfar)
{
	VeMatrixPerspectiveFovLH_GL(pf32OutMat4x4, fov,
		GetHeightF() * GetInvWidthF(), znear, zfar);
}
//--------------------------------------------------------------------------
const VeChar8* VeRendererGLES2::GetShaderName()
{
	return "glsl_es";
}
//--------------------------------------------------------------------------
const VeChar8* VeRendererGLES2::GetShaderProfile()
{
	return "1_0";
}
//--------------------------------------------------------------------------
VeUInt32 VeRendererGLES2::GetMultisampleLevelCount()
{
	return m_bSupportMSAA ? 1 : 0;
}
//--------------------------------------------------------------------------
void VeRendererGLES2::EnumMultisampleLevel(VeUInt32 u32Index, VeUInt32& u32Count, VeUInt32& u32Quality)
{
	VE_ASSERT(u32Index < GetMultisampleLevelCount());
	u32Count = m_u8MaxAASamples;
	u32Quality = 0;
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateVertexShader(const VeChar8* pcName, VeBinaryIStream& kStream, VeROVertexShaderPtr* pspShader)
{
	GLint iLen = kStream.RemainingLength();
	const GLchar* pcShader = (const VeChar8*)kStream.Skip(iLen);
	kStream.Finish();
	VeUInt32 u32Shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(u32Shader, 1, &pcShader, &iLen);
	glCompileShader(u32Shader);
	GLint iShaderCompiled(0);
	glGetShaderiv(u32Shader, GL_COMPILE_STATUS, &iShaderCompiled);
	if(iShaderCompiled)
	{
		VeROVertexShaderGLES2* pkShader = VE_NEW VeROVertexShaderGLES2(pcName, u32Shader);
		AddObject(pkShader);
		if(pspShader)
		{
			*pspShader = pkShader;
		}
		return true;
	}
	else
	{
		if(pspShader)
		{
			*pspShader = NULL;
		}
		return false;
	}
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreatePixelShader(const VeChar8* pcName, VeBinaryIStream& kStream, VeROPixelShaderPtr* pspShader)
{
	GLint iLen = kStream.RemainingLength();
	const GLchar* pcShader = (const VeChar8*)kStream.Skip(iLen);
	kStream.Finish();
	VeUInt32 u32Shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(u32Shader, 1, &pcShader, &iLen);
	glCompileShader(u32Shader);
	GLint iShaderCompiled(0);
	glGetShaderiv(u32Shader, GL_COMPILE_STATUS, &iShaderCompiled);
	if(iShaderCompiled)
	{
		VeROPixelShaderGLES2* pkShader = VE_NEW VeROPixelShaderGLES2(pcName, u32Shader);
		AddObject(pkShader);
		if(pspShader)
		{
			*pspShader = pkShader;
		}
		return true;
	}
	else
	{
		if(pspShader)
		{
			*pspShader = NULL;
		}
		return false;
	}
}
//--------------------------------------------------------------------------
VeUInt16 GetGLBlend(VeRenderer::Blend eBlend, bool bAlpha)
{
	switch(eBlend)
	{
	case VeRenderer::BLEND_ZERO:
		return GL_ZERO;
	case VeRenderer::BLEND_ONE:
		return GL_ONE;
	case VeRenderer::BLEND_SRC_COLOR:
		return GL_SRC_COLOR;
	case VeRenderer::BLEND_INV_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	case VeRenderer::BLEND_SRC_ALPHA:
		return GL_SRC_ALPHA;
	case VeRenderer::BLEND_INV_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case VeRenderer::BLEND_DEST_ALPHA:
		return GL_DST_ALPHA;
	case VeRenderer::BLEND_INV_DEST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case VeRenderer::BLEND_DEST_COLOR:
		return GL_DST_COLOR;
	case VeRenderer::BLEND_INV_DEST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case VeRenderer::BLEND_SRC_ALPHA_SAT:
		return GL_SRC_ALPHA_SATURATE;
	case VeRenderer::BLEND_BLEND_FACTOR:
		if(bAlpha)
		{
			return GL_CONSTANT_ALPHA;
		}
		else
		{
			return GL_CONSTANT_COLOR;
		}
	case VeRenderer::BLEND_INV_BLEND_FACTOR:
		if(bAlpha)
		{
			return GL_ONE_MINUS_CONSTANT_ALPHA;
		}
		else
		{
			return GL_ONE_MINUS_CONSTANT_COLOR;
		} 
	default:
		return GL_ZERO;
	}
}
//--------------------------------------------------------------------------
VeUInt16 GetGLBlendOp(VeRenderer::BlendOp eBlendOp)
{
	switch(eBlendOp)
	{
	case VeRenderer::BLEND_OP_ADD:
		return GL_FUNC_ADD;
	case VeRenderer::BLEND_OP_SUBTRACT:
		return GL_FUNC_SUBTRACT;
	case VeRenderer::BLEND_OP_REV_SUBTRACT:
		return GL_FUNC_REVERSE_SUBTRACT;
	default:
		return GL_FUNC_ADD;
	}
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateBlendState(const VeChar8* pcName, VeBinaryIStream& kStream, VeROBlendStatePtr* pspBlendState)
{
	VeUInt16 u16AlphaToCoverage, u16NumRTDesc;
	kStream >> u16AlphaToCoverage;
	kStream >> u16NumRTDesc;
	if(u16NumRTDesc > 1)
	{
		for(VeUInt16 i(0); i < u16NumRTDesc; ++i)
		{
			VeUInt8 u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
			kStream >> u8Temp;
		}
		return false;
	}
	else
	{
		VeROBlendStateGLES2* pkBlendState = VE_NEW VeROBlendStateGLES2(pcName);
		pkBlendState->m_bAlphaToCoverageEnable = u16AlphaToCoverage ? true : false;
		if(u16NumRTDesc)
		{
			VeUInt8 u8Temp;
			kStream >> u8Temp;
			pkBlendState->m_bBlendEnable = u8Temp ? true : false;
			kStream >> u8Temp;
			pkBlendState->m_u16SrcBlend = GetGLBlend((VeRenderer::Blend)u8Temp, false);
			kStream >> u8Temp;
			pkBlendState->m_u16DstBlend = GetGLBlend((VeRenderer::Blend)u8Temp, false);
			kStream >> u8Temp;
			pkBlendState->m_u16BlendOp = GetGLBlendOp((VeRenderer::BlendOp)u8Temp);
			kStream >> u8Temp;
			pkBlendState->m_u16SrcBlendAlpha = GetGLBlend((VeRenderer::Blend)u8Temp, true);
			kStream >> u8Temp;
			pkBlendState->m_u16DstBlendAlpha = GetGLBlend((VeRenderer::Blend)u8Temp, true);
			kStream >> u8Temp;
			pkBlendState->m_u16BlendOpAlpha = GetGLBlendOp((VeRenderer::BlendOp)u8Temp);
			kStream >> u8Temp;
			pkBlendState->m_u16WriteMask = u8Temp;
		}
		AddObject(pkBlendState);
		if(pspBlendState)
		{
			*pspBlendState = pkBlendState;
		}
		return true;
	}
}
//--------------------------------------------------------------------------
VeUInt16 GetGLComparisonFunc(VeRenderer::ComparisonFunc eFunc)
{
	switch(eFunc)
	{
	case VeRenderer::COMPARISON_NEVER:
		return GL_NEVER;
	case VeRenderer::COMPARISON_LESS:
		return GL_LESS;
	case VeRenderer::COMPARISON_EQUAL:
		return GL_EQUAL;
	case VeRenderer::COMPARISON_LESS_EQUAL:
		return GL_LEQUAL;
	case VeRenderer::COMPARISON_GREATER:
		return GL_GREATER;
	case VeRenderer::COMPARISON_NOT_EQUAL:
		return GL_NOTEQUAL;
	case VeRenderer::COMPARISON_GREATER_EQUAL:
		return GL_GEQUAL;
	case VeRenderer::COMPARISON_ALWAYS:
		return GL_ALWAYS;
	default:
		return GL_NEVER;
	}
}
//--------------------------------------------------------------------------
VeUInt16 GetGLStencilOp(VeRenderer::StencilOp eStencilOp)
{
	switch(eStencilOp)
	{
	case VeRenderer::STENCIL_OP_KEEP:
		return GL_KEEP;
	case VeRenderer::STENCIL_OP_ZERO:
		return GL_ZERO;
	case VeRenderer::STENCIL_OP_REPLACE:
		return GL_REPLACE;
	case VeRenderer::STENCIL_OP_INCR_SAT:
		return GL_INCR;
	case VeRenderer::STENCIL_OP_DECR_SAT:
		return GL_DECR;
	case VeRenderer::STENCIL_OP_INVERT:
		return GL_INVERT;
	case VeRenderer::STENCIL_OP_INCR:
		return GL_INCR_WRAP;
	case VeRenderer::STENCIL_OP_DECR:
		return GL_DECR_WRAP;
	default:
		return GL_KEEP;
	}
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateDepthStencilState(const VeChar8* pcName, VeBinaryIStream& kStream, VeRODepthStencilStatePtr* pspDepthStencilState)
{
	VeRODepthStencilStateGLES2* pkDepthStencilState = VE_NEW VeRODepthStencilStateGLES2(pcName);
	VeByte byTemp;
	kStream >> byTemp;
	pkDepthStencilState->m_bDepthEnable = byTemp ? true : false;
	kStream >> byTemp;
	pkDepthStencilState->m_bDepthWrite = byTemp ? true : false;
	kStream >> byTemp;
	pkDepthStencilState->m_u16DepthFunc = GetGLComparisonFunc((VeRenderer::ComparisonFunc)byTemp);
	kStream >> byTemp;
	pkDepthStencilState->m_bStencilEnable = byTemp ? true : false;
	if(pkDepthStencilState->m_bStencilEnable)
	{
		kStream >> pkDepthStencilState->m_u8StencilReadMask;
		kStream >> pkDepthStencilState->m_u8StencilWriteMask;
		kStream >> byTemp;
		pkDepthStencilState->m_u16FrontFailOp = GetGLStencilOp((VeRenderer::StencilOp)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16FrontDepthFailOp = GetGLStencilOp((VeRenderer::StencilOp)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16FrontPassOp = GetGLStencilOp((VeRenderer::StencilOp)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16FrontFunc = GetGLComparisonFunc((VeRenderer::ComparisonFunc)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16BackFailOp = GetGLStencilOp((VeRenderer::StencilOp)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16BackDepthFailOp = GetGLStencilOp((VeRenderer::StencilOp)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16BackPassOp = GetGLStencilOp((VeRenderer::StencilOp)byTemp);
		kStream >> byTemp;
		pkDepthStencilState->m_u16BackFunc = GetGLComparisonFunc((VeRenderer::ComparisonFunc)byTemp);
	}
	else
	{
		pkDepthStencilState->m_u8StencilReadMask = 0xff;
		pkDepthStencilState->m_u8StencilWriteMask = 0xff;
		pkDepthStencilState->m_u16FrontFailOp = GL_KEEP;
		pkDepthStencilState->m_u16FrontDepthFailOp = GL_KEEP;
		pkDepthStencilState->m_u16FrontPassOp = GL_KEEP;
		pkDepthStencilState->m_u16FrontFunc = GL_ALWAYS;
		pkDepthStencilState->m_u16BackFailOp = GL_KEEP;
		pkDepthStencilState->m_u16BackDepthFailOp = GL_KEEP;
		pkDepthStencilState->m_u16BackPassOp = GL_KEEP;
		pkDepthStencilState->m_u16BackFunc = GL_ALWAYS;
	}
	AddObject(pkDepthStencilState);
	if(pspDepthStencilState)
	{
		*pspDepthStencilState = pkDepthStencilState;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateInputLayout(const VeChar8* pcName, VeBinaryIStream& kStream, VeROInputLayoutPtr* pspLayout)
{
	VeStringA kLayoutName;
	kStream.GetStringAligned4(kLayoutName);
	VeUInt32 u32DescNum;
	kStream >> u32DescNum;
	VeROInputLayoutGLES2* pkLayoutGLES2 = VE_NEW VeROInputLayoutGLES2(pcName);
	VeVector<VeROInputLayoutGLES2::InputElementDesc>& kDescArray = pkLayoutGLES2->m_kElementDescArray;
	kDescArray.Resize(u32DescNum);
	bool bRes = true;
	for(VeUInt32 i(0); i < kDescArray.Size(); ++i)
	{
		VeByte byTemp;
		kStream >> byTemp;
		kDescArray[i].m_eSemanticType = (Semantic)byTemp;
		kStream >> byTemp;
		kDescArray[i].m_u32SemanticIndex = byTemp;
		kStream >> byTemp;
		kDescArray[i].SetFormat((Format)byTemp);
		kStream >> byTemp;
		kDescArray[i].m_u16InputSlot = byTemp;
		kStream >> byTemp;
		kDescArray[i].m_u16AlignedByteOffset = byTemp;
		kStream >> byTemp;
		if(byTemp) bRes = false;
		VeUInt16 u16InstanceDataRate;
		kStream >> u16InstanceDataRate;
		if(u16InstanceDataRate) bRes = false;
	}
	if(bRes)
	{
		AddObject(pkLayoutGLES2);
		if(pspLayout)
		{
			*pspLayout = pkLayoutGLES2;
		}
		return true;
	}
	else
	{
		if(pspLayout)
		{
			*pspLayout = NULL;
		}
		return false;
	}
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateRasterizerState(const VeChar8* pcName, VeBinaryIStream& kStream, VeRORasterizeStatePtr* pspRasterizeState)
{
	VeUInt8 u8FillMode, u8CullMode, u8FrontCCW, u8DepthBiasMode, u8DepthClip, u8Scissor, u8Multisample, u8AntialiasedLine;
	kStream >> u8FillMode;
	kStream >> u8CullMode;
	kStream >> u8FrontCCW;
	kStream >> u8DepthBiasMode;
	kStream >> u8DepthClip;
	kStream >> u8Scissor;
	kStream >> u8Multisample;
	kStream >> u8AntialiasedLine;

	if(u8FillMode != VeRenderer::FILL_SOLID || (!u8DepthClip))
	{
		if(pspRasterizeState)
		{
			*pspRasterizeState = NULL;
		}
		return false;
	}

	VeRORasterizeStateGLES2* pkRSState = VE_NEW VeRORasterizeStateGLES2(pcName);
	switch(u8CullMode)
	{
	case CULL_FRONT:
		pkRSState->m_bCullEnable = true;
		pkRSState->m_u16CullFace = GL_FRONT;
		break;
	case CULL_BACK:
		pkRSState->m_bCullEnable = true;
		pkRSState->m_u16CullFace = GL_BACK;
		break;
	default:
		pkRSState->m_bCullEnable = false;
		pkRSState->m_u16CullFace = GL_BACK;
		break;
	}
	pkRSState->m_u16FrontFace = u8FrontCCW ? GL_CCW : GL_CW;
	switch(u8DepthBiasMode)
	{
	case DBIAS_NONE:
	default:
		pkRSState->m_bDepthBiasEnable = false;
		pkRSState->m_f32PolyFactor = 0;
		pkRSState->m_f32PolyUnits = 0;
		break;
	}
	pkRSState->m_bMultisampleEnable = u8Multisample ? true : false;
	pkRSState->m_bScissorEnable = u8Scissor ? true : false;
	AddObject(pkRSState);
	if(pspRasterizeState)
	{
		*pspRasterizeState = pkRSState;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateBuffer(const VeChar8* pcName, VeBinaryIStream& kStream, VeROBufferPtr* pspBuffer)
{
	VeUInt32 u32ByteWidth;
	kStream >> u32ByteWidth;
	VeUInt16 u16Temp;
	kStream >> u16Temp;
	kStream >> u16Temp;
	VeByte byTemp;
	kStream >> byTemp;
	Usage eUsage = (Usage)byTemp;
	kStream >> byTemp;
	BindFlag eBindFlag = (BindFlag)byTemp;
	kStream >> byTemp;
	kStream >> byTemp;
	VeUInt32 u32SysMemPitch;
	VeUInt32 u32SysMemSlicePitch;
	void* pvInitBuffer(NULL);
	if(byTemp)
	{
		kStream >> u32SysMemPitch;
		kStream >> u32SysMemSlicePitch;
		VE_ASSERT((u32ByteWidth & 3) == 0);
		pvInitBuffer = VeMalloc(u32ByteWidth);
		kStream.Read(pvInitBuffer, u32ByteWidth);
	}

	GLenum eTarget;
	switch(eBindFlag)
	{
	case BIND_VERTEX_BUFFER:
		eTarget = GL_ARRAY_BUFFER;
		break;
	case BIND_INDEX_BUFFER:
		eTarget = GL_ELEMENT_ARRAY_BUFFER;
		break;
	case BIND_CONSTANT_BUFFER:
		eTarget = BIND_CONSTANT_BUFFER;
		break;
	default:
		if(pspBuffer)
		{
			*pspBuffer = NULL;
		}
		return false;
	}

	VeROBufferGLES2* pkBuffer;

	if(eTarget == BIND_CONSTANT_BUFFER)
	{
		if(!pvInitBuffer)
		{
			pvInitBuffer = VeMalloc(u32ByteWidth);
		}
		pkBuffer = VE_NEW VeROBufferGLES2(pcName, eTarget, pvInitBuffer, u32ByteWidth);
	}
	else
	{
		GLenum eUsageGL(GL_DYNAMIC_DRAW);
		switch(eUsage)
		{
		case USAGE_DEFAULT:
		case USAGE_IMMUTABLE:
			eUsageGL = GL_STATIC_DRAW;
			break;
		default:
			break;
		}

		GLuint uiBuffer(0);
		glGenBuffers(1, &uiBuffer);
		BindBuffer(eTarget, uiBuffer);
		glBufferData(eTarget, u32ByteWidth, pvInitBuffer, eUsageGL);
		pkBuffer = VE_NEW VeROBufferGLES2(pcName, eTarget, uiBuffer, u32ByteWidth);

		if(pvInitBuffer)
		{
			VeFree(pvInitBuffer);
		}
	}	

	AddObject(pkBuffer);
	if(pspBuffer)
	{
		*pspBuffer = pkBuffer;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateSamplerState(const VeChar8* pcName, VeBinaryIStream& kStream, VeROSamplerStatePtr* pspSamplerState)
{
	if(pspSamplerState)
	{
		*pspSamplerState = NULL;
	}
	VeByte byTemp;
	kStream >> byTemp;
	VeRenderer::Filter eFilter = (VeRenderer::Filter)byTemp;
	kStream >> byTemp;
	VeRenderer::TextureAddressMode eAddressU = (VeRenderer::TextureAddressMode)byTemp;
	kStream >> byTemp;
	VeRenderer::TextureAddressMode eAddressV = (VeRenderer::TextureAddressMode)byTemp;
	kStream >> byTemp;
	VeUInt16 u16Temp;
	kStream >> u16Temp;
	VeUInt32 u32MaxAnisotropy = VE_MIN(u16Temp, m_i32MaxAnisotropy);
	kStream >> u16Temp;
	kStream.Skip(28);
	if(eFilter > FILTER_ANISOTROPIC) return false;
	if(eFilter == FILTER_ANISOTROPIC && (!m_bSupportAnisotropic)) return false;
	if(eAddressU > TEXTURE_ADDRESS_CLAMP || eAddressV > TEXTURE_ADDRESS_CLAMP) return false;
	VeROSamplerStateGLES2* pkSampler = VE_NEW VeROSamplerStateGLES2(pcName);
	pkSampler->m_eFilter = eFilter;
	pkSampler->m_eAddressU = eAddressU;
	pkSampler->m_eAddressV = eAddressV;
	pkSampler->m_u32MaxAnisotropy = u32MaxAnisotropy;
	AddObject(pkSampler);
	if(pspSamplerState)
	{
		*pspSamplerState = pkSampler;
	}
	return true;
}
//--------------------------------------------------------------------------
struct VeTexInit
{
	VeUInt32 m_u32SysMemPitch;
	VeUInt32 m_u32SysMemSlicePitch;
	void* m_pvSysMem;
};
//--------------------------------------------------------------------------
bool CheckFormat(VeRenderer::Format eFormat, GLenum& eTexFormat, GLenum& eTexInternalFormat, GLenum& eTexType, GLenum& eRBOType, VeUInt32& u32BitPerPixel, bool& bIsCompressed)
{
	switch(eFormat)
	{
	case VeRenderer::FMT_R8G8B8A8_UNORM:
		eTexFormat = eTexInternalFormat = GL_RGBA;
		eTexType = GL_UNSIGNED_BYTE;
		eRBOType = GL_RGBA8_OES;
		u32BitPerPixel = 32;
		bIsCompressed = false;
		return true;
	case VeRenderer::FMT_D24_UNORM_S8_UINT:
		eTexFormat = eTexInternalFormat = GL_DEPTH_STENCIL_OES;
		eTexType = GL_UNSIGNED_INT_24_8_OES;
		eRBOType = GL_DEPTH24_STENCIL8_OES;
		u32BitPerPixel = 32;
		bIsCompressed = false;
		return true;
	default:
		return false;
	}
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateTexture2D(const VeChar8* pcName, VeBinaryIStream& kStream, VeROTexture2DPtr* pspTexture)
{
	if(pspTexture)
	{
		*pspTexture = NULL;
	}
	VeUInt16 u16Width, u16Height, u16MipLevels, u16ArraySize;
	kStream >> u16Width;
	kStream >> u16Height;
	kStream >> u16MipLevels;
	kStream >> u16ArraySize;
	VeByte byTemp;
	kStream >> byTemp;
	Format eFormat = (Format)byTemp;
	VeUInt8 u8Conut, u8Quality;
	kStream >> u8Conut;
	kStream >> u8Quality;
	kStream >> byTemp;
	VeUInt8 u8BindFlags;
	kStream >> u8BindFlags;
	kStream >> byTemp;
	VeUInt16 u16Temp;
	kStream >> u16Temp;
	bool bCreateViews;
	kStream >> u16Temp;
	bCreateViews = u16Temp ? true : false;
	kStream >> u16Temp;
	VeVector<VeTexInit> kInitData;
	kInitData.Resize(u16Temp);
	for(VeUInt16 i(0); i < u16Temp; ++i)
	{
		kStream >> kInitData[i].m_u32SysMemPitch;
		kStream >> kInitData[i].m_u32SysMemSlicePitch;
		VeUInt32 u32ByteWidth;
		kStream >> u32ByteWidth;
		VE_ASSERT((u32ByteWidth & 3) == 0);
		kInitData[i].m_pvSysMem = kStream.Skip(u32ByteWidth);
	}
	if(u16ArraySize != 1) return false;
	bool bSRV = VE_MASK_HAS_ANY(u8BindFlags, BIND_SHADER_RESOURCE);
	bool bRTV = VE_MASK_HAS_ANY(u8BindFlags, BIND_RENDER_TARGET);
	bool bDSV = VE_MASK_HAS_ANY(u8BindFlags, BIND_DEPTH_STENCIL);
    if(u8Conut != 1)
    {
        if(u8Conut != m_u8MaxAASamples) return false;
        if(bSRV) return false;
    }
	if((!bSRV) && kInitData.Size()) return false;
	if(bRTV && bDSV) return false;
	if((!m_bSupportDepthTexute) && bDSV && bSRV)
    {
        bSRV = false;
    }
	if(bDSV && (eFormat != FMT_D24_UNORM_S8_UINT)) return false;
	GLenum eTexFormat, eTexInternalFormat, eTexType, eRBOType;
	VeUInt32 u32BitPerPixel;
	bool bIsCompressed;
	if(!CheckFormat(eFormat, eTexFormat, eTexInternalFormat, eTexType, eRBOType, u32BitPerPixel, bIsCompressed)) return false;
	if(bIsCompressed) return false;

	VeROTexture2DGLES2* pkTexture = VE_NEW VeROTexture2DGLES2(pcName);
	pkTexture->m_u32Width = u16Width;
	pkTexture->m_u32Height = u16Height;
	pkTexture->m_u32MipLevels = u16MipLevels;
	if(bSRV)
	{
		pkTexture->m_u32Target = GL_TEXTURE_2D;
		glGenTextures(1, &(pkTexture->m_u32Texture));
		SetPSTexture(0, pkTexture->m_u32Texture);

		VeUInt32 u32SizeX = u16Width, u32SizeY = u16Height;

		if(bIsCompressed)
		{

		}
		else
		{
			if(u16MipLevels)
			{
				for(VeUInt32 i(0); i < u16MipLevels; u32SizeX = VE_MAX(u32SizeX >> 1, 1), u32SizeY = VE_MAX(u32SizeY >> 1, 1), ++i)
				{
					void* pvData = NULL;
					if(kInitData.Size() > i) pvData = kInitData[i].m_pvSysMem;
					glTexImage2D(GL_TEXTURE_2D, i, eTexInternalFormat, u32SizeX, u32SizeY, 0, eTexFormat, eTexType, pvData);
					VE_ASSERT(glGetError() == GL_NO_ERROR);
				}
			}
			else
			{
				for(VeUInt32 i(0); (u32SizeX > 1 || u32SizeY > 1); u32SizeX = VE_MAX(u32SizeX >> 1, 1), u32SizeY = VE_MAX(u32SizeY >> 1, 1), ++i)
				{
					void* pvData = NULL;
					if(kInitData.Size() > i) pvData = kInitData[i].m_pvSysMem;
					glTexImage2D(GL_TEXTURE_2D, i, eTexInternalFormat, u32SizeX, u32SizeY, 0, eTexFormat, eTexType, pvData);
					VE_ASSERT(glGetError() == GL_NO_ERROR);
				}
			}
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if(m_bSupportAnisotropic)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
		}
	}
	else
	{
		pkTexture->m_u32Target = GL_RENDERBUFFER;
		glGenRenderbuffers(1, &(pkTexture->m_u32Texture));
        BindRenderBuffer(pkTexture->m_u32Texture);
        if(u8Conut == 1)
        {
            glRenderbufferStorage(GL_RENDERBUFFER, eRBOType, u16Width, u16Height);
        }
        else
        {
#			if defined(VE_PLATFORM_IOS)
            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, u8Conut, eRBOType, u16Width, u16Height);
#			endif
        }
        BindRenderBuffer(0);
		VE_ASSERT(glGetError() == GL_NO_ERROR);
	}
	AddObject(pkTexture);
	if(bCreateViews)
	{
		if(bSRV)
		{
			AddObject(VE_NEW VeROShaderResourceViewGLES2(pcName, pkTexture));
		}
		if(bRTV)
		{
			AddObject(VE_NEW VeRORenderTargetViewGLES2(pcName, pkTexture, 0));
		}
		if(bDSV)
		{
			AddObject(VE_NEW VeRODepthStencilViewGLES2(pcName, pkTexture, 0));
		}
	}

	if(pspTexture)
	{
		*pspTexture = pkTexture;
	}
	return false;
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::CreateProcessByType(VeRenderProcess::Type eType, const VeChar8* pcName, VeBinaryIStream& kStream, VeRenderProcessPtr* pspProcess)
{
	switch(eType)
	{
	case VeRenderProcess::TYPE_MSAA_RESOLVE:
#		if defined(VE_PLATFORM_IOS)
		{
			VeMSAAResolveGLES2APPLEPtr spMSAAProcess = VE_NEW VeMSAAResolveGLES2APPLE(pcName);
			bool bRes = spMSAAProcess->Update(kStream);
            if(bRes)
            {
                AddProcess(VeSmartPointerCast(VeMSAAResolveGLES2APPLE, spMSAAProcess));
                if(pspProcess)
                {
                    *pspProcess = VeSmartPointerCast(VeMSAAResolveGLES2APPLE, spMSAAProcess);
                }
            }
			
			return bRes;
		}
#		else
		break;
#		endif
	default:
		break;
	}
	return false;
}
//--------------------------------------------------------------------------
bool VeRendererGLES2::MapRes(const VeRenderObjectPtr& spObject,
	VeUInt32 u32SubResource, Map eType, VE_BOOL bSync,
	MappedSubresource* pkMapped)
{
	VE_ASSERT(pkMapped);
	switch(spObject->GetType())
	{
	case VeRenderObject::TYPE_BUFFER:
		{
			VeROBufferGLES2* pkBuffer = VeDynamicCast(VeROBufferGLES2, VeSmartPointerCast(VeRenderObject, spObject));
			if(pkBuffer && pkBuffer->GetBindFlag() == VeRenderer::BIND_CONSTANT_BUFFER)
			{
				if(eType != MAP_WRITE_NO_OVERWRITE && u32SubResource == 0)
				{
					VE_ASSERT(pkBuffer->m_pvBuffer);
					pkMapped->m_pvData = pkBuffer->m_pvBuffer;
					return true;
				}
			}
		}
		break;
	default:
		break;
	}
	return VeRenderer::MapRes(spObject, u32SubResource, eType, bSync,
		pkMapped);
}
//--------------------------------------------------------------------------
void VeRendererGLES2::UnMapRes(const VeRenderObjectPtr& spObject,
	VeUInt32 u32SubResource)
{
	VeRenderer::UnMapRes(spObject, u32SubResource);
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Render(VeRenderNode* pkNode)
{
	VE_ASSERT(pkNode);
	if(pkNode->m_spData)
	{
		VeRefList<VeRenderTick*>& kTickList = pkNode->m_spData->m_kTickList;
		kTickList.BeginIterator();
		while(!kTickList.IsEnd())
		{
			VeRenderTick* pkTick = kTickList.GetIterNode()->m_tContent;
			kTickList.Next();
			PushTick(pkTick);
		}

		FlushStates();

		VeRefList<VeRenderCall*>& kCallList = pkNode->m_spData->m_kCallList;
		kCallList.BeginIterator();
		while(!kCallList.IsEnd())
		{
			VeRenderCall* pkCall = kCallList.GetIterNode()->m_tContent;
			kCallList.Next();
			Call(pkCall);
		}

		pkNode->m_kRenderChildren.BeginIterator();
		while(!pkNode->m_kRenderChildren.IsEnd())
		{
			VeRenderNode* pkNodeIter = pkNode->m_kRenderChildren.GetIterNode()->m_tContent;
			pkNode->m_kRenderChildren.Next();
			Render(pkNodeIter);
			pkNodeIter->Detach();
		}

		kTickList.BeginIterator();
		while(!kTickList.IsEnd())
		{
			VeRenderTick* pkTick = kTickList.GetIterNode()->m_tContent;
			kTickList.Next();
			PopTick(pkTick);
		}
	}
	else
	{
		pkNode->m_kRenderChildren.BeginIterator();
		while(!pkNode->m_kRenderChildren.IsEnd())
		{
			VeRenderNode* pkNodeIter = pkNode->m_kRenderChildren.GetIterNode()->m_tContent;
			pkNode->m_kRenderChildren.Next();
			Render(pkNodeIter);
			pkNodeIter->Detach();
		}
	}
	VE_ASSERT(pkNode->m_kRenderChildren.Empty());
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Process(const VeRenderProcessPtr& spProcess)
{
	VE_ASSERT(spProcess);
	switch(spProcess->GetType())
	{
	case VeRenderProcess::TYPE_MSAA_RESOLVE:
#		if defined(VE_PLATFORM_IOS)
		{
			VeMSAAResolveGLES2APPLE* pkProcess = (VeMSAAResolveGLES2APPLE*)(VeRenderProcess*)spProcess;
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, pkProcess->m_u32DrawBuffer);
            glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, pkProcess->m_u32ReadBuffer);
            glResolveMultisampleFramebufferAPPLE();
		}
#		endif
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::PushTick(VeRenderTick* pkTick)
{
	VE_ASSERT(pkTick);
	switch(pkTick->GetType())
	{
	case VeRenderTick::IA_INDEX_BUFFER:
		m_kIndexBufferStack.PushBack((VeROBufferGLES2*)(VeROBuffer*)(((VeIAIndexBuffer*)pkTick)->m_spBuffer));
		VE_ASSERT((((VeIAIndexBuffer*)pkTick)->m_u32Format) == VeRenderer::FMT_R16_UINT);
		break;
	case VeRenderTick::IA_INPUT_LAYOUT:
		m_kInputLayoutStack.PushBack((VeROInputLayoutGLES2*)(VeROInputLayout*)(((VeIAInputLayout*)pkTick)->m_spInputLayout));
		break;
	case VeRenderTick::IA_PRIMITIVE_TOPOLOGY:
		m_kTopologyStack.PushBack(GetGLTopology(((VeIAPrimitiveTopology*)pkTick)->m_eTopology));
		break;
	case VeRenderTick::IA_VERTEX_BUFFER:
		{
			VeIAVertexBuffer* pkBuffer = (VeIAVertexBuffer*)pkTick;
			VE_ASSERT(VE_MASK_HAS_ANY(pkBuffer->m_spBuffer->GetBindFlag(), BIND_VERTEX_BUFFER));
			VeUInt32 u32Slot = pkBuffer->m_u32Slot;
			VeVector<VertexBufferForSet>& kStack = m_akVertexBufferStack[u32Slot];
			kStack.Resize(kStack.Size() + 1);
			kStack.Back().m_pkBuffer = (VeROBufferGLES2*)(VeROBuffer*)(pkBuffer->m_spBuffer);
			kStack.Back().m_u32Stride = pkBuffer->m_u32Stride;
			m_u32VertexBufferChangeMin = VE_MIN(m_u32VertexBufferChangeMin, u32Slot);
			m_u32VertexBufferChangeMax = (m_u32VertexBufferChangeMax == VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT) ? u32Slot : VE_MAX(m_u32VertexBufferChangeMax, u32Slot);
		}
		break;
	case VeRenderTick::VS_CONSTANT_BUFFER:
		{
			VeVSCBuffer* pkBuffer = (VeVSCBuffer*)pkTick;
			VE_ASSERT(VE_MASK_HAS_ANY(pkBuffer->m_spBuffer->GetBindFlag(), BIND_CONSTANT_BUFFER));
			VeUInt32 u32Slot = pkBuffer->m_u32Slot;
			VeVector<VeROBufferGLES2*>& kStack = m_akVSCBufferStack[u32Slot];
			kStack.PushBack((VeROBufferGLES2*)(VeROBuffer*)(pkBuffer->m_spBuffer));
			m_u32VSChangeCBufferMin = VE_MIN(m_u32VSChangeCBufferMin, u32Slot);
			m_u32VSChangeCBufferMax = (m_u32VSChangeCBufferMax == VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) ? u32Slot : VE_MAX(m_u32VSChangeCBufferMax, u32Slot);
		}
		break;
	case VeRenderTick::VS_SAMPLER:
		break;
	case VeRenderTick::VS_SHADER:
		m_kVertexShaderStack.PushBack((VeROVertexShaderGLES2*)(VeROVertexShader*)(((VeVSShader*)pkTick)->m_spShader));
		break;
	case VeRenderTick::VS_SHADER_RESOURCE:
		break;
	case VeRenderTick::HS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::HS_SAMPLER:
		break;
	case VeRenderTick::HS_SHADER:
		break;
	case VeRenderTick::HS_SHADER_RESOURCE:
		break;
	case VeRenderTick::DS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::DS_SAMPLER:
		break;
	case VeRenderTick::DS_SHADER:
		break;
	case VeRenderTick::DS_SHADER_RESOURCE:
		break;
	case VeRenderTick::GS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::GS_SAMPLER:
		break;
	case VeRenderTick::GS_SHADER:
		break;
	case VeRenderTick::GS_SHADER_RESOURCE:
		break;
	case VeRenderTick::SO_TARGET:
		break;
	case VeRenderTick::RS_SCISSOR_RECT:
		{
			VeRSScissorRectTick* pkScissorTick = ((VeRSScissorRectTick*)pkTick);
			VE_ASSERT(pkScissorTick->m_u32Slot == 0);
			m_kScissorRectStack.Resize(m_kScissorRectStack.Size() + 1);
			m_kScissorRectStack.Back().x = pkScissorTick->m_i16Left;
			m_kScissorRectStack.Back().y = pkScissorTick->m_i16Top;
			m_kScissorRectStack.Back().w = pkScissorTick->m_i16Width;
			m_kScissorRectStack.Back().h = pkScissorTick->m_i16Height;
		}
		break;
	case VeRenderTick::RS_STATE:
		{
			VeRSState* pkRSStateTick = ((VeRSState*)pkTick);
			if(pkRSStateTick->m_spState)
			{
				m_kRasterizeStateStack.PushBack((VeRORasterizeStateGLES2*)(VeRORasterizeState*)(pkRSStateTick->m_spState));
			}
			else
			{
				m_kRasterizeStateStack.PushBack(NULL);
			}
		}
		break;
	case VeRenderTick::RS_VIEWPORT:
		{
			VeRSViewportTick* pkViewportTick = ((VeRSViewportTick*)pkTick);
			VE_ASSERT(pkViewportTick->m_u32Slot == 0);
			m_kViewportStack.PushBack(pkViewportTick->m_kViewport);
		}
		break;
	case VeRenderTick::PS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::PS_SAMPLER:
		break;
	case VeRenderTick::PS_SHADER:
		m_kPixelShaderStack.PushBack((VeROPixelShaderGLES2*)(VeROPixelShader*)(((VePSShader*)pkTick)->m_spShader));
		break;
	case VeRenderTick::PS_SHADER_RESOURCE:
		{
			VePSShaderResource* pkShaderResource = (VePSShaderResource*)pkTick;
			VeUInt32 u32Slot = pkShaderResource->m_u32Slot;
			m_akPSShaderResourceViewStack[u32Slot].PushBack((VeROShaderResourceViewGLES2*)(VeROShaderResourceView*)(pkShaderResource->m_spShaderResourceView));
		}
		break;
	case VeRenderTick::OM_BLEND_STATE:
		{
			VeOMBlendStateTick* pkBlendStateTick = (VeOMBlendStateTick*)pkTick;
			m_kBlendStateStack.Resize(m_kBlendStateStack.Size() + 1);
			m_kBlendStateStack.Back().m_pkBlendState = pkBlendStateTick->m_spBlendState ? (VeROBlendStateGLES2*)(VeROBlendState*)(pkBlendStateTick->m_spBlendState) : NULL;
			VeCrazyCopy(&(m_kBlendStateStack.Back().m_kFactor), pkBlendStateTick->m_af32Factor, sizeof(m_kBlendStateStack.Back().m_kFactor));
			m_bBlendStateChange = VE_TRUE;
		}
		break;
	case VeRenderTick::OM_DEPTH_STENCIL_STATE:
		{
			VeOMDepthStencilStateTick* pkDepthStencilStateTick = (VeOMDepthStencilStateTick*)pkTick;
			m_kDepthStencilStateStack.Resize(m_kDepthStencilStateStack.Size() + 1);
			m_kDepthStencilStateStack.Back().m_pkDepthStencilState = pkDepthStencilStateTick->m_spDepthStencilState ? (VeRODepthStencilStateGLES2*)(VeRODepthStencilState*)(pkDepthStencilStateTick->m_spDepthStencilState) : NULL;
			m_kDepthStencilStateStack.Back().m_u32StencilRef = pkDepthStencilStateTick->m_u32StencilRef;
			m_bDepthStencilStateChange = VE_TRUE;
		}
		break;
	case VeRenderTick::OM_RENDER_TARGET_VIEW:
		{
			VeOMRenderTargetViewTick* pkRTVTick = (VeOMRenderTargetViewTick*)pkTick;
			VE_ASSERT(pkRTVTick->m_u32Slot == 0);
			m_kRenderTargetViewStack.PushBack((VeRORenderTargetViewGLES2*)(VeRORenderTargetView*)(((VeOMRenderTargetViewTick*)pkTick)->m_spRenderTarget));
		}
		break;
	case VeRenderTick::OM_DEPTH_STENCIL_VIEW:
		m_kDepthStencilViewStack.PushBack((VeRODepthStencilViewGLES2*)(VeRODepthStencilView*)(((VeOMDepthStencilViewTick*)pkTick)->m_spDepthStencil));
		break;
	case VeRenderTick::OM_UNORDERED_ACCESS_VIEW:
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::PopTick(VeRenderTick* pkTick)
{
	VE_ASSERT(pkTick);
	switch(pkTick->GetType())
	{
	case VeRenderTick::IA_INDEX_BUFFER:
		m_kIndexBufferStack.PopBack();
		break;
	case VeRenderTick::IA_INPUT_LAYOUT:
		m_kInputLayoutStack.PopBack();
		break;
	case VeRenderTick::IA_PRIMITIVE_TOPOLOGY:
		m_kTopologyStack.PopBack();
		break;
	case VeRenderTick::IA_VERTEX_BUFFER:
		{
			VeIAVertexBuffer* pkBuffer = (VeIAVertexBuffer*)pkTick;
			VE_ASSERT(VE_MASK_HAS_ANY(pkBuffer->m_spBuffer->GetBindFlag(), BIND_VERTEX_BUFFER));
			VeUInt32 u32Slot = pkBuffer->m_u32Slot;
			m_akVertexBufferStack[u32Slot].PopBack();
			m_u32VertexBufferChangeMin = VE_MIN(m_u32VertexBufferChangeMin, u32Slot);
			m_u32VertexBufferChangeMax = (m_u32VertexBufferChangeMax == VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT) ? u32Slot : VE_MAX(m_u32VertexBufferChangeMax, u32Slot);
		}
		break;
	case VeRenderTick::VS_CONSTANT_BUFFER:
		{
			VeVSCBuffer* pkBuffer = (VeVSCBuffer*)pkTick;
			VE_ASSERT(VE_MASK_HAS_ANY(pkBuffer->m_spBuffer->GetBindFlag(), BIND_CONSTANT_BUFFER));
			VeUInt32 u32Slot = pkBuffer->m_u32Slot;
			m_akVSCBufferStack[u32Slot].PopBack();
			m_u32VSChangeCBufferMin = VE_MIN(m_u32VSChangeCBufferMin, u32Slot);
			m_u32VSChangeCBufferMax = (m_u32VSChangeCBufferMax == VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) ? u32Slot : VE_MAX(m_u32VSChangeCBufferMax, u32Slot);
		}
		break;
	case VeRenderTick::VS_SAMPLER:
		break;
	case VeRenderTick::VS_SHADER:
		m_kVertexShaderStack.PopBack();
		break;
	case VeRenderTick::VS_SHADER_RESOURCE:
		break;
	case VeRenderTick::HS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::HS_SAMPLER:
		break;
	case VeRenderTick::HS_SHADER:
		break;
	case VeRenderTick::HS_SHADER_RESOURCE:
		break;
	case VeRenderTick::DS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::DS_SAMPLER:
		break;
	case VeRenderTick::DS_SHADER:
		break;
	case VeRenderTick::DS_SHADER_RESOURCE:
		break;
	case VeRenderTick::GS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::GS_SAMPLER:
		break;
	case VeRenderTick::GS_SHADER:
		break;
	case VeRenderTick::GS_SHADER_RESOURCE:
		break;
	case VeRenderTick::SO_TARGET:
		break;
	case VeRenderTick::RS_SCISSOR_RECT:
		m_kScissorRectStack.PopBack();
		break;
	case VeRenderTick::RS_STATE:
		m_kRasterizeStateStack.PopBack();
		break;
	case VeRenderTick::RS_VIEWPORT:
		m_kViewportStack.PopBack();
		break;
	case VeRenderTick::PS_CONSTANT_BUFFER:
		break;
	case VeRenderTick::PS_SAMPLER:
		break;
	case VeRenderTick::PS_SHADER:
		m_kPixelShaderStack.PopBack();
		break;
	case VeRenderTick::PS_SHADER_RESOURCE:
		{
			VePSShaderResource* pkShaderResource = (VePSShaderResource*)pkTick;
			VeUInt32 u32Slot = pkShaderResource->m_u32Slot;
			m_akPSShaderResourceViewStack[u32Slot].PopBack();
		}
		break;
	case VeRenderTick::OM_BLEND_STATE:
		m_kBlendStateStack.PopBack();
		m_bBlendStateChange = VE_TRUE;
		break;
	case VeRenderTick::OM_DEPTH_STENCIL_STATE:
		m_kDepthStencilStateStack.PopBack();
		m_bDepthStencilStateChange = VE_TRUE;
		break;
	case VeRenderTick::OM_RENDER_TARGET_VIEW:
		m_kRenderTargetViewStack.PopBack();
		break;
	case VeRenderTick::OM_DEPTH_STENCIL_VIEW:
		m_kDepthStencilViewStack.PopBack();
		break;
	case VeRenderTick::OM_UNORDERED_ACCESS_VIEW:
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::Call(VeRenderCall* pkCall)
{
	VE_ASSERT(m_kTopologyStack.Size());
	switch(pkCall->GetType())
	{
	case VeRenderCall::CLEAR:
		{
			VeClearCall* pkClearCall = (VeClearCall*)pkCall;
			VeUInt32 u32Flag(0);
			if(pkClearCall->m_bClearColor)
			{
				u32Flag |= GL_COLOR_BUFFER_BIT;
				ClearColor(pkClearCall->m_kColorArray[0]);
			}
			if(pkClearCall->m_bClearDepth)
			{
				u32Flag |= GL_DEPTH_BUFFER_BIT;
				ClearDepth(pkClearCall->m_bClearDepth);
			}
			if(pkClearCall->m_bClearStencil)
			{
				u32Flag |= GL_STENCIL_BUFFER_BIT;
				ClearStencil(pkClearCall->m_bClearStencil);
			}
			glClear(u32Flag);
		}
		break;
	case VeRenderCall::DRAW:
		glDrawArrays(m_kTopologyStack.Back(), ((VeDrawCall*)pkCall)->m_u32Location, ((VeDrawCall*)pkCall)->m_u32Count);
		break;
	case VeRenderCall::DRAW_INDEXED:
		glDrawElements(GL_TRIANGLES, ((VeDrawIndexedCall*)pkCall)->m_u32IndexCount, GL_UNSIGNED_SHORT, (void*)((((VeDrawIndexedCall*)pkCall)->m_u32StartIndex) << 1));
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::FlushStates()
{
	bool bProgramUpdate = false;
	VE_ASSERT(m_kVertexShaderStack.Size());
	if(m_kVertexShaderStack.Back() != m_pkCurVertexShader)
	{
		m_pkCurVertexShader = m_kVertexShaderStack.Back();
		bProgramUpdate = true;
	}
	VE_ASSERT(m_kPixelShaderStack.Size());
	if(m_kPixelShaderStack.Back() != m_pkCurPixelShader)
	{
		m_pkCurPixelShader = m_kPixelShaderStack.Back();
		bProgramUpdate = true;
	}

	if(m_bBlendStateChange)
	{
		VE_ASSERT(m_kBlendStateStack.Size());
		BlendStateForSet& kLast = m_kBlendStateStack.Back();
		if(kLast.m_pkBlendState)
		{
			if(kLast.m_pkBlendState != m_kCurBlendState.m_pkBlendState)
			{
				m_kCurBlendState.m_pkBlendState = kLast.m_pkBlendState;
				SetAlphaToCoverageEnable(m_kCurBlendState.m_pkBlendState->m_bAlphaToCoverageEnable);
				SetBlendEnable(m_kCurBlendState.m_pkBlendState->m_bBlendEnable);
				SetBlendFunc(m_kCurBlendState.m_pkBlendState->m_u16SrcBlend,
					m_kCurBlendState.m_pkBlendState->m_u16DstBlend,
					m_kCurBlendState.m_pkBlendState->m_u16SrcBlendAlpha,
					m_kCurBlendState.m_pkBlendState->m_u16DstBlendAlpha);
				SetBlendOp(m_kCurBlendState.m_pkBlendState->m_u16BlendOp, m_kCurBlendState.m_pkBlendState->m_u16BlendOpAlpha);
				SetWriteMask(m_kCurBlendState.m_pkBlendState->m_u16WriteMask);
			}
			if(kLast.m_kFactor != m_kCurBlendState.m_kFactor)
			{
				m_kCurBlendState.m_kFactor = kLast.m_kFactor;
				glBlendColor(m_kCurBlendState.m_kFactor.r, m_kCurBlendState.m_kFactor.g, m_kCurBlendState.m_kFactor.b, m_kCurBlendState.m_kFactor.a);
			}
		}
		else
		{
			SetAlphaToCoverageEnable(false);
			SetBlendEnable(false);
			SetWriteMask(VeRenderer::COLOR_WRITE_ENABLE_ALL);
			m_kCurBlendState.m_pkBlendState = NULL;
		}		
		m_bBlendStateChange = VE_FALSE;
	}

	if(m_bDepthStencilStateChange)
	{
		VE_ASSERT(m_kDepthStencilStateStack.Size());
		DepthStencilStateForSet& kLast = m_kDepthStencilStateStack.Back();
		if(kLast.m_pkDepthStencilState)
		{
			bool bChange = false;
			if(kLast.m_pkDepthStencilState != m_kCurDepthStencilState.m_pkDepthStencilState)
			{
				m_kCurDepthStencilState.m_pkDepthStencilState = kLast.m_pkDepthStencilState;
				bChange = true;
			}
			if(kLast.m_u32StencilRef != m_kCurDepthStencilState.m_u32StencilRef)
			{
				m_kCurDepthStencilState.m_u32StencilRef = kLast.m_u32StencilRef;
				bChange = true;
			}
			if(bChange)
			{
				VeRODepthStencilStateGLES2* pkObject = m_kCurDepthStencilState.m_pkDepthStencilState;
				SetDepthEnable(pkObject->m_bDepthEnable);
				if(pkObject->m_bDepthEnable)
				{
					SetDepthWrite(pkObject->m_bDepthWrite);
					SetDepthFunc(pkObject->m_u16DepthFunc);
				}
				SetStencilEnable(pkObject->m_bStencilEnable);
				if(pkObject->m_bStencilEnable)
				{
					SetStencilWriteMask(pkObject->m_u8StencilWriteMask);
					SetStencilFunc(pkObject->m_u16FrontFunc, pkObject->m_u16BackFunc, m_kCurDepthStencilState.m_u32StencilRef, pkObject->m_u8StencilReadMask);
					SetStencilFrontOp(pkObject->m_u16FrontFailOp, pkObject->m_u16FrontDepthFailOp, pkObject->m_u16FrontPassOp);
					SetStencilBackOp(pkObject->m_u16BackFailOp, pkObject->m_u16BackDepthFailOp, pkObject->m_u16BackPassOp);
				}
			}
		}
		else
		{
			SetDepthEnable(false);
			SetStencilEnable(false);
			m_kCurDepthStencilState.m_pkDepthStencilState = NULL;
		}
	}

	VeRORasterizeStateGLES2* pkRSState = m_kRasterizeStateStack.Size() ? m_kRasterizeStateStack.Back() : NULL;
	if(pkRSState != m_pkCurRasterizeState)
	{
		if(pkRSState)
		{
			SetCullEnable(pkRSState->m_bCullEnable);
			SetDepthBiasEnable(pkRSState->m_bDepthBiasEnable);
			SetScissorEnable(pkRSState->m_bScissorEnable);
			if(pkRSState->m_bCullEnable)
			{
				SetCullFace(pkRSState->m_u16CullFace);
				SetFrontFace(pkRSState->m_u16FrontFace);
			}
			if(pkRSState->m_bDepthBiasEnable)
			{
				SetPolyOffset(pkRSState->m_f32PolyFactor, pkRSState->m_f32PolyUnits);
			}
		}
		else
		{
			SetCullEnable(true);
			SetDepthBiasEnable(false);
			SetScissorEnable(false);
			SetCullFace(GL_BACK);
			SetFrontFace(GL_CW);
		}
		m_pkCurRasterizeState = pkRSState;
	}

	bool bInputLayoutUpdate = false;
	VE_ASSERT(m_kInputLayoutStack.Size());
	if(m_kInputLayoutStack.Back() != m_pkCurInputLayout)
	{
		m_pkCurInputLayout = m_kInputLayoutStack.Back();
		bInputLayoutUpdate = true;
	}

	if(bProgramUpdate || bInputLayoutUpdate)
	{
		static VeChar8 acBuffer[256];
		{
			VeUInt32 u32Pointer(0);
			u32Pointer += VeSprintf(acBuffer + u32Pointer, 256, "i:");
			for(VeUInt32 i(0); i < m_pkCurInputLayout->m_kElementDescArray.Size(); ++i)
			{
				VeROInputLayoutGLES2::InputElementDesc& kDesc = m_pkCurInputLayout->m_kElementDescArray[i];
				if(i)
				{
					u32Pointer += VeSprintf(acBuffer + u32Pointer, 256, ",%d_%d", kDesc.m_eSemanticType, kDesc.m_u32SemanticIndex);
				}
				else
				{
					u32Pointer += VeSprintf(acBuffer + u32Pointer, 256, "%d_%d", kDesc.m_eSemanticType, kDesc.m_u32SemanticIndex);
				}
			}
			VeSprintf(acBuffer + u32Pointer, 256, ";v:%s;p:%s", m_pkCurVertexShader->GetName(), m_pkCurPixelShader->GetName());
		}

		VeUInt32* pu32Iter = m_kProgramMap.Find(acBuffer);
		if(!pu32Iter)
		{
			VeUInt32 u32Program = glCreateProgram();
			glAttachShader(u32Program, m_pkCurVertexShader->m_u32Shader);
			glAttachShader(u32Program, m_pkCurPixelShader->m_u32Shader);
			for(VeUInt32 i(0); i < m_pkCurInputLayout->m_kElementDescArray.Size(); ++i)
			{
				VeROInputLayoutGLES2::InputElementDesc& kDesc = m_pkCurInputLayout->m_kElementDescArray[i];
				switch(kDesc.m_eSemanticType)
				{
				case VeRenderer::SE_POSITION:
					VeSprintf(acBuffer, 256, "aPosition%d", kDesc.m_u32SemanticIndex);
					break;
				case VeRenderer::SE_NORMAL:
					VeSprintf(acBuffer, 256, "aNormal%d", kDesc.m_u32SemanticIndex);
					break;
				case VeRenderer::SE_COLOR:
					VeSprintf(acBuffer, 256, "aColor%d", kDesc.m_u32SemanticIndex);
					break;
				case VeRenderer::SE_TEXCOORD:
					VeSprintf(acBuffer, 256, "aTexcoord%d", kDesc.m_u32SemanticIndex);
					break;
				case VeRenderer::SE_TANGENT:
					VeSprintf(acBuffer, 256, "aTangent%d", kDesc.m_u32SemanticIndex);
					break;
				case VeRenderer::SE_BINORMAL:
					VeSprintf(acBuffer, 256, "aBinormal%d", kDesc.m_u32SemanticIndex);
					break;
				case VeRenderer::SE_BLENDINDICES:
					VeSprintf(acBuffer, 256, "aBlendIndices%d", kDesc.m_u32SemanticIndex);
				case VeRenderer::SE_BLENDWEIGHT:
					VeSprintf(acBuffer, 256, "aBlendWeight%d", kDesc.m_u32SemanticIndex);
					break;
				default:
					VE_ASSERT(!"Wrong semantic");
					break;
				}
				glBindAttribLocation(u32Program, i, acBuffer);
			}
			glLinkProgram(u32Program);
#			ifdef VE_DEBUG
			GLint bLinked;
			glGetProgramiv(u32Program, GL_LINK_STATUS, &bLinked);
			VE_ASSERT(bLinked);
#			endif
			m_kProgramArray.PushBack(u32Program);
			pu32Iter = &(m_kProgramMap[acBuffer]);
			*pu32Iter = u32Program;
		}
		UseProgram(*pu32Iter);
	}

	bool bVertexBufferUpdate = false;
	if(m_u32VertexBufferChangeMax < VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
	{
		VE_ASSERT(m_u32VertexBufferChangeMin <= m_u32VertexBufferChangeMax);
		for(VeUInt32 i(m_u32VertexBufferChangeMin); i <= m_u32VertexBufferChangeMax; ++i)
		{
			VeVector<VertexBufferForSet>& kStack = m_akVertexBufferStack[i];
			if(kStack.Size())
			{
				VertexBufferForSet& kVertexBuffer = kStack.Back();
				if(kVertexBuffer.m_pkBuffer != m_akCurVertexBuffers[i].m_pkBuffer)
				{
					m_akCurVertexBuffers[i].m_pkBuffer = kVertexBuffer.m_pkBuffer;
					bVertexBufferUpdate = true;
				}
				if(kVertexBuffer.m_u32Stride != m_akCurVertexBuffers[i].m_u32Stride)
				{
					m_akCurVertexBuffers[i].m_u32Stride = kVertexBuffer.m_u32Stride;
					bVertexBufferUpdate = true;
				}
			}
			else if(m_akCurVertexBuffers[i].m_pkBuffer)
			{
				m_akCurVertexBuffers[i].m_pkBuffer = NULL;
				bVertexBufferUpdate = true;
			}

		}
		m_u32VertexBufferChangeMin = VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
		m_u32VertexBufferChangeMax = VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
	}

	if(bVertexBufferUpdate || bInputLayoutUpdate)
	{
		for(VeUInt32 i(0); i < m_pkCurInputLayout->m_kElementDescArray.Size(); ++i)
		{
			VeROInputLayoutGLES2::InputElementDesc& kDesc = m_pkCurInputLayout->m_kElementDescArray[i];
			VertexBufferForSet& kVBForSet = m_akCurVertexBuffers[kDesc.m_u16InputSlot];
			BindBuffer(GL_ARRAY_BUFFER, kVBForSet.m_pkBuffer->m_u32Buffer);
			EnableVertexAttribArray(i);
			VE_ASSERT(kVBForSet.m_pkBuffer->m_u32Target == GL_ARRAY_BUFFER);
			glVertexAttribPointer(i, kDesc.m_u8Size, kDesc.m_u16Type, kDesc.m_u8Normalized, kVBForSet.m_u32Stride, (void*)(VeSizeT)(kDesc.m_u16AlignedByteOffset));
		}
	}

	bool bIndexBufferUpdate = false;
	VeROBufferGLES2* pkCurrentIB = m_kIndexBufferStack.Size() ? m_kIndexBufferStack.Back() : NULL;
	if(pkCurrentIB != m_pkCurIndexBuffer)
	{
		m_pkCurIndexBuffer = pkCurrentIB;
		bIndexBufferUpdate = true;
	}

	if(bIndexBufferUpdate)
	{
		if(m_pkCurIndexBuffer)
		{
			VE_ASSERT(m_pkCurIndexBuffer->m_u32Target == GL_ELEMENT_ARRAY_BUFFER);
			BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pkCurIndexBuffer->m_u32Buffer);
		}
		else
		{
			BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	if(m_i32VSCBufferLoc >= 0 && m_u32VSChangeCBufferMax < VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
	{
		VE_ASSERT(m_u32VSChangeCBufferMin <= m_u32VSChangeCBufferMax);
		VeUInt32 u32StartSlot = VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
		VeUInt32 u32EndSlot = VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
		VeUInt32 u32PreEnd(0), u32CurEnd(0);
		for(VeUInt32 i(m_u32VSChangeCBufferMin); i <= m_u32VSChangeCBufferMax; ++i)
		{
			VeVector<VeROBufferGLES2*>& kStack = m_akVSCBufferStack[i];
			if(kStack.Size())
			{
				bool bChange(false);

				VeROBufferGLES2* pkBuffer = kStack.Back();
				if(pkBuffer != m_apkCurVSCBuffers[i])
				{
					if(m_apkCurVSCBuffers[i])
					{
						u32PreEnd += m_apkCurVSCBuffers[i]->m_u32ByteWidth >> 4;
					}
					u32CurEnd += pkBuffer->m_u32ByteWidth >> 4;
					m_apkCurVSCBuffers[i] = pkBuffer;
					bChange = true;
				}
				if(bChange)
				{
					if(u32StartSlot == VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
					{
						u32StartSlot = i;
						u32EndSlot = i;
					}
					else
					{
						u32EndSlot = i;
					}
				}
			}
			else
			{
				m_apkCurVSCBuffers[i] = NULL;
				break;
			}
		}
		if(u32StartSlot < VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
		{
			VeUInt32 u32Start(0);
			for(VeUInt32 i(0); i < u32StartSlot; ++i)
			{
				u32Start += m_apkCurVSCBuffers[i]->m_u32ByteWidth >> 4;
			}
			VeVector4D* pkBuffer = m_akVSCBufferCache + u32Start;
			for(VeUInt32 i(u32StartSlot); i <= u32EndSlot; ++i)
			{
				VeCrazyCopy(pkBuffer, m_apkCurVSCBuffers[i]->m_pvBuffer, m_apkCurVSCBuffers[i]->m_u32ByteWidth);
				pkBuffer += m_apkCurVSCBuffers[i]->m_u32ByteWidth >> 4;
			}
			VeUInt32 u32EndSlotNext = u32EndSlot + 1;
			if(u32PreEnd != u32CurEnd)
			{
				for(VeUInt32 i(u32EndSlotNext); i < VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT && m_apkCurVSCBuffers[i]; ++i)
				{
					VeCrazyCopy(pkBuffer, m_apkCurVSCBuffers[i]->m_pvBuffer, m_apkCurVSCBuffers[i]->m_u32ByteWidth);
					pkBuffer += m_apkCurVSCBuffers[i]->m_u32ByteWidth >> 4;
				}
			}
			if((m_i32VSCBufferLoc >= 0) && (!m_bVSCBufferNeedUpdate))
			{
				glUniform4fv(m_i32VSCBufferLoc + u32Start, pkBuffer - m_akVSCBufferCache - u32Start, (const GLfloat*)(m_akVSCBufferCache + u32Start));
			}
		}
		m_u32VSChangeCBufferMin = VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
		m_u32VSChangeCBufferMax = VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
	}
	if(m_bVSCBufferNeedUpdate)
	{
		VE_ASSERT(m_i32VSCBufferLoc >= 0);
		VeUInt32 u32Count(0);
		for(VeUInt32 i(0); i < VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; ++i)
		{
			if(m_apkCurVSCBuffers[i])
			{
				u32Count += m_apkCurVSCBuffers[i]->m_u32ByteWidth >> 4;
			}
			else
			{
				break;
			}
		}
		if(u32Count)
		{
			glUniform4fv(m_i32VSCBufferLoc, u32Count, (const GLfloat*)m_akVSCBufferCache);
		}
	}

	for(VeUInt32 i(0); i < VE_GLES2_PS_TEX_NUM; ++i)
	{
		VeROShaderResourceViewGLES2* pkNew = m_akPSShaderResourceViewStack[i].Size() ? m_akPSShaderResourceViewStack[i].Back() : NULL;
		if(pkNew != m_apkCurPSShaderResourceViews[i])
		{
			if(pkNew)
			{
				if(!m_bEnablePSTexture)
				{
					glUniform1i(glGetUniformLocation(m_u32UsedProgram, "uPSTexture"), 0);
				}
				SetPSTexture(i, pkNew->m_spTexture->m_u32Texture);
			}
			else
			{
				SetPSTexture(i, 0);
			}			
			m_apkCurPSShaderResourceViews[i] = pkNew;
		}
	}

	bool bRenderTargetChanged = false;
	VE_ASSERT(m_kRenderTargetViewStack.Size());
	if(m_kRenderTargetViewStack.Back() != m_pkCurRenderTargetView)
	{
		m_pkCurRenderTargetView = m_kRenderTargetViewStack.Back();
		bRenderTargetChanged = true;
	}
	if((m_kDepthStencilViewStack.Size() ? m_kDepthStencilViewStack.Back() : NULL) != m_pkCurDepthStencilView)
	{
		m_pkCurDepthStencilView = (m_kDepthStencilViewStack.Size() ? m_kDepthStencilViewStack.Back() : NULL);
		bRenderTargetChanged = true;
	}
	if(bRenderTargetChanged)
	{
		m_kCurViewport = VeVector4D::ZERO;
		BindFrameBuffer(GetFrameBuffer(m_pkCurRenderTargetView, m_pkCurDepthStencilView));
		VE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	GLint iHeight = (GLint)GetHeight();
	VE_ASSERT(m_pkCurRenderTargetView);
	if(m_pkCurRenderTargetView->m_spTexture)
	{
		iHeight = m_pkCurRenderTargetView->m_spTexture->m_u32Height;
	}

	VE_ASSERT(m_kViewportStack.Size());
	if(m_kViewportStack.Back() != m_kCurViewport)
	{
		m_kCurViewport = m_kViewportStack.Back();
		glViewport(GLint(m_kCurViewport.x), iHeight - GLint(m_kCurViewport.y) - GLint(m_kCurViewport.w), GLint(m_kCurViewport.z), GLint(m_kCurViewport.w));
	}

	if(m_kScissorRectStack.Size())
	{
		if(m_kCurScissorRect != m_kScissorRectStack.Back())
		{
			m_kCurScissorRect = m_kScissorRectStack.Back();
			glScissor(m_kCurScissorRect.x, iHeight - m_kCurScissorRect.y - m_kCurScissorRect.h, m_kCurScissorRect.w, m_kCurScissorRect.h);
		}
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeRendererGLES2::GetGLTopology(VeIAPrimitiveTopology::Topology eTopology)
{
	switch(eTopology)
	{
	case VeIAPrimitiveTopology::TOPOLOGY_POINTLIST:
		return GL_POINTS;
	case VeIAPrimitiveTopology::TOPOLOGY_LINELIST:
		return GL_LINES;
	case VeIAPrimitiveTopology::TOPOLOGY_LINESTRIP:
		return GL_LINE_STRIP;
	case VeIAPrimitiveTopology::TOPOLOGY_TRIANGLELIST:
		return GL_TRIANGLES;
	case VeIAPrimitiveTopology::TOPOLOGY_TRIANGLESTRIP:
		return GL_TRIANGLE_STRIP;
	default:
		return VE_INFINITE;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::ResetStateCache()
{
	m_kVertexShaderStack.Clear();
	m_pkCurVertexShader = NULL;
	m_kPixelShaderStack.Clear();
	m_pkCurPixelShader = NULL;
	m_kBlendStateStack.Clear();
	m_kCurBlendState.m_pkBlendState = NULL;
	m_kCurBlendState.m_kFactor = VeColorA::WHITE;
	m_bBlendStateChange = VE_FALSE;
	m_kDepthStencilStateStack.Clear();
	m_kCurDepthStencilState.m_pkDepthStencilState = NULL;
	m_kCurDepthStencilState.m_u32StencilRef = 0;
	m_bDepthStencilStateChange = VE_FALSE;
	m_kInputLayoutStack.Clear();
	m_pkCurInputLayout = NULL;
	m_kRasterizeStateStack.Clear();
	m_pkCurRasterizeState = NULL;
	m_kTopologyStack.Clear();
	m_kIndexBufferStack.Clear();
	m_pkCurIndexBuffer = NULL;

	for(VeUInt32 i(0); i < VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; ++i)
	{
		m_akVertexBufferStack[i].Clear();
	}
	VeZeroMemory(m_akCurVertexBuffers, sizeof(m_akCurVertexBuffers));
	m_u32VertexBufferChangeMin = VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
	m_u32VertexBufferChangeMax = VE_GLES2_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;

	for(VeUInt32 i(0); i < VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; ++i)
	{
		m_akVSCBufferStack[i].Clear();
	}
	VeZeroMemory(m_apkCurVSCBuffers, sizeof(m_apkCurVSCBuffers));
	m_u32VSChangeCBufferMin = VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
	m_u32VSChangeCBufferMax = VE_GLES2_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
	VeZeroMemory(m_akVSCBufferCache, sizeof(m_akVSCBufferCache));
	m_u32VSCBufferCachePointer = 0;

	for(VeUInt32 i(0); i < VE_GLES2_PS_TEX_NUM; ++i)
	{
		m_akPSShaderResourceViewStack[i].Clear();
		m_apkCurPSShaderResourceViews[i] = NULL;
	}

	m_kViewportStack.Clear();
	m_kCurViewport = VeVector4D::ZERO;

	m_kScissorRectStack.Clear();
	VeZeroMemory(&m_kCurScissorRect, sizeof(m_kCurScissorRect));

	m_kRenderTargetViewStack.Clear();
	m_kDepthStencilViewStack.Clear();
	m_pkCurRenderTargetView = NULL;
	m_pkCurDepthStencilView = NULL;
}
//--------------------------------------------------------------------------
void VeRendererGLES2::UpdateBufferSize()
{
	ResetStateCache();
#	if defined(VE_PLATFORM_MOBILE_SIM) || defined(VE_PLATFORM_ANDROID)
	EGLint iWidth, iHeight;
	eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_WIDTH, &iWidth);
	eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_HEIGHT, &iHeight);
	if(GetWidth() != (VeUInt32)iWidth || GetHeight() != (VeUInt32)iHeight)
	{
		SetScreenSize(iWidth, iHeight);
		OnBufferResized(GetWidth(), GetHeight());
	}
#   elif defined(VE_PLATFORM_IOS)
	BindRenderBuffer(m_u32BackRenderBuffer);
	VeUInt32 u32Width, u32Height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, (GLint*)&u32Width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, (GLint*)&u32Height);
	if(GetWidth() != u32Width || GetHeight() != u32Height)
	{
		SetScreenSize(u32Width, u32Height);
		OnBufferResized(GetWidth(), GetHeight());
		//glViewport(0, 0, u32Width, u32Height);
	}
#	endif
}
//--------------------------------------------------------------------------
void VeRendererGLES2::GLInitGlobal()
{
	m_kDescription.m_kVendor = (const VeChar8*)glGetString(GL_VENDOR);
	m_kDescription.m_kRenderer = (const VeChar8*)glGetString(GL_RENDERER);
	m_kDescription.m_kVersion = (const VeChar8*)glGetString(GL_VERSION);
	m_kDescription.m_kGLSLVersion = (const VeChar8*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	VE_LOG_D("Venus3D", "GL_VENDOR: %s", (const VeChar8*)m_kDescription.m_kVendor);
	VE_LOG_D("Venus3D", "GL_RENDERER: %s", (const VeChar8*)m_kDescription.m_kRenderer);
	VE_LOG_D("Venus3D", "GL_VERSION: %s", (const VeChar8*)m_kDescription.m_kVersion);
	VE_LOG_D("Venus3D", "GL_SHADING_LANGUAGE_VERSION: %s", (const VeChar8*)m_kDescription.m_kGLSLVersion);
	VeChar8 acExtensions[2048];
	VeStrcpy(acExtensions, 2048, (const VeChar8*)glGetString(GL_EXTENSIONS));
	VeChar8* pcContent;
	VeChar8* pcTemp = VeStrtok(acExtensions, " ", &pcContent);
	while(pcTemp)
	{
		m_kDescription.m_kExtensionMap[pcTemp] = m_kDescription.m_kExtensionArray.Size();
		m_kDescription.m_kExtensionArray.PushBack(pcTemp);
		VE_LOG_D("Venus3D", "GL_EXTENSION: %s", (const VeChar8*)m_kDescription.m_kExtensionArray.Back());
		pcTemp = VeStrtok(NULL, " ", &pcContent);
	}

#   ifdef VE_PLATFORM_MOBILE_SIM
	m_bSupportDepthTexute = false;
#	else
	m_bSupportDepthTexute = m_kDescription.m_kExtensionMap.Find("GL_OES_depth_texture") ? true : false;
#	endif
	VeDebugOutput("Depth texture support: %s", (const VeChar8*)(VeString::From(m_bSupportDepthTexute)));

#   ifdef VE_PLATFORM_MOBILE_SIM
	m_bSupportAnisotropic = true;
#	else
	m_bSupportAnisotropic = m_kDescription.m_kExtensionMap.Find("GL_EXT_texture_filter_anisotropic") ? true : false;
#	endif
	VeDebugOutput("Anisotropic filter support: %s", (const VeChar8*)(VeString::From(m_bSupportAnisotropic)));
	if(m_bSupportAnisotropic)
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_i32MaxAnisotropy);
		VeDebugOutput("Max Anisotropic is: %d", m_i32MaxAnisotropy);
	}

#	if defined(VE_PLATFORM_IOS)
    m_bSupportMSAA = m_kDescription.m_kExtensionMap.Find("GL_APPLE_framebuffer_multisample") ? true : false;
    if(m_bSupportMSAA)
    {
        GLint iTemp;
        glGetIntegerv(GL_MAX_SAMPLES_APPLE, &iTemp);
        m_u8MaxAASamples = iTemp;
    }
#	endif
	VeDebugOutput("Multisample Anti-Aliasing support: %s", (const VeChar8*)(VeString::From(m_bSupportMSAA)));
	if(m_bSupportMSAA)
	{
		VeDebugOutput("Max samples is: %d", (VeUInt32)m_u8MaxAASamples);
	}
    
    VE_ASSERT(m_kFrameBufferObjectMap.Empty() && m_kFrameBufferObjectArray.Empty());
	VE_ASSERT(m_kProgramMap.Empty() && m_kProgramArray.Empty());
#	if defined(VE_PLATFORM_IOS)
    m_kFrameBufferObjectMap["back_buffer"] = m_u32BackFrameBuffer;
    AddObject(VE_NEW VeRORenderTargetViewGLES2("back_buffer", NULL, 0));
#   else
	GLint iOriginalFramebuffer(0);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &iOriginalFramebuffer);
	m_kFrameBufferObjectMap["back_buffer"] = iOriginalFramebuffer;
	AddObject(VE_NEW VeRORenderTargetViewGLES2("back_buffer", NULL, 0));
#   endif

	SetCullEnable(true);
	SetFrontFace(GL_CW);

	glClearColor(0, 0, 0, 0);
	glClearDepthf(1.0f);
	glClearStencil(0);

	m_kClearColor = VeColorA::BLACK;
	m_f32ClearDepth = 1.0f;
	m_u8ClearStencil = 0;
}
//--------------------------------------------------------------------------
void VeRendererGLES2::GLTermGlobal()
{
	m_kFrameBufferObjectMap.Clear();
	if(m_kFrameBufferObjectArray.Size())
	{
		glDeleteFramebuffers(m_kFrameBufferObjectArray.Size(), m_kFrameBufferObjectArray.Begin());
	}
	m_kFrameBufferObjectArray.Clear();
	m_kProgramMap.Clear();
	for(VeUInt32 i(0); i < m_kProgramArray.Size(); ++i)
	{
		glDeleteProgram(m_kProgramArray[i]);
	}
	m_kProgramArray.Clear();
}
//--------------------------------------------------------------------------
void VeRendererGLES2::GLCacheInit()
{
	VeZeroMemory(m_au32BindBuffers, sizeof(m_au32BindBuffers));
	m_u32BindRenderBuffer = 0;
	m_u32BindFrameBuffer = 0;
	m_u32UsedProgram = 0;
	m_i32VSCBufferLoc = -1;
	m_i32PSCBufferLoc = -1;
	m_bVSCBufferNeedUpdate = VE_FALSE;
	m_bPSCBufferNeedUpdate = VE_FALSE;
	m_bEnableVSTexture = false;
	m_bEnablePSTexture = false;
	VeZeroMemory(m_abVertexAttribArrayState, sizeof(m_abVertexAttribArrayState));

	m_bAlphaToCoverageEnable = false;
	m_bBlendEnable = false;
	m_u16SrcBlend = GL_ONE;
	m_u16DstBlend = GL_ZERO;
	m_u16SrcBlendAlpha = GL_ONE;
	m_u16DstBlendAlpha = GL_ZERO;
	m_u16BlendOp = GL_FUNC_ADD;
	m_u16BlendOpAlpha = GL_FUNC_ADD;
	m_u16WriteMask = VeRenderer::COLOR_WRITE_ENABLE_ALL;

	m_bDepthEnable = false;
	m_bDepthWrite = true;
	m_u16DepthFunc = 0;
	m_bStencilEnable = false;
	m_u8StencilReadMask = 0xff;
	m_u8StencilWriteMask = 0xff;
	m_u16StencilFrontFailOp = 0;
	m_u16StencilFrontDepthFailOp = 0;
	m_u16StencilFrontPassOp = 0;
	m_u16StencilFrontFunc = 0;
	m_u16StencilBackFailOp = 0;
	m_u16StencilBackDepthFailOp = 0;
	m_u16StencilBackPassOp = 0;
	m_u16StencilBackFunc = 0;
	m_u32StencilRef = 0;

	m_bCullEnable = false;
	m_bDepthBiasEnable = false;
	m_bMultisampleEnable = false;
	m_bScissorEnable = false;
	m_u16CullFace = GL_BACK;
	m_u16FrontFace = GL_CCW;
	m_f32PolyFactor = 0;
	m_f32PolyUnits = 0;

	VeZeroMemory(m_au32PSTexture, sizeof(m_au32PSTexture));
	VeZeroMemory(m_au32VSTexture, sizeof(m_au32VSTexture));
}
//--------------------------------------------------------------------------
void VeRendererGLES2::BindBuffer(VeUInt32 u32Target, VeUInt32 u32Buffer)
{
	VeUInt32 u32Index(TARGET_NUM);
	switch(u32Target)
	{
	case GL_ARRAY_BUFFER:
		u32Index = TARGET_ARRAY_BUFFER;
		break;
	case GL_ELEMENT_ARRAY_BUFFER:
		u32Index = TARGET_ELEMENT_ARRAY_BUFFER;
		break;
	default:
		break;
	}
	VE_ASSERT(u32Index < TARGET_NUM);
	if(m_au32BindBuffers[u32Index] != u32Buffer)
	{
		glBindBuffer(u32Target, u32Buffer);
		m_au32BindBuffers[u32Index] = u32Buffer;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::BindRenderBuffer(VeUInt32 u32Buffer)
{
	if(m_u32BindRenderBuffer != u32Buffer)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, u32Buffer);
		m_u32BindRenderBuffer = u32Buffer;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::BindFrameBuffer(VeUInt32 u32Buffer)
{
	if(m_u32BindFrameBuffer != u32Buffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, u32Buffer);
		m_u32BindFrameBuffer = u32Buffer;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::UseProgram(VeUInt32 u32Program)
{
	if(u32Program != m_u32UsedProgram)
	{
		glUseProgram(u32Program);
		m_u32UsedProgram = u32Program;
		m_i32VSCBufferLoc = glGetUniformLocation(m_u32UsedProgram, "uVSCBuffer");
		m_i32PSCBufferLoc = glGetUniformLocation(m_u32UsedProgram, "uPSCBuffer");
		m_bVSCBufferNeedUpdate = (m_i32VSCBufferLoc >= 0) ? VE_TRUE : VE_FALSE;
		m_bPSCBufferNeedUpdate = (m_i32PSCBufferLoc >= 0) ? VE_TRUE : VE_FALSE;
		m_bEnableVSTexture = false;
		m_bEnablePSTexture = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::EnableVertexAttribArray(VeUInt32 u32Array)
{
	VE_ASSERT(u32Array < VE_GLES2_IA_VERTEX_ATTRIB_ARRAY_NUM);
	if(!m_abVertexAttribArrayState[u32Array])
	{
		glEnableVertexAttribArray(u32Array);
		m_abVertexAttribArrayState[u32Array] = true;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::DisableVertexAttribArray(VeUInt32 u32Array)
{
	VE_ASSERT(u32Array < VE_GLES2_IA_VERTEX_ATTRIB_ARRAY_NUM);
	if(m_abVertexAttribArrayState[u32Array])
	{
		glDisableVertexAttribArray(u32Array);
		m_abVertexAttribArrayState[u32Array] = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetAlphaToCoverageEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bAlphaToCoverageEnable)
		{
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			m_bAlphaToCoverageEnable = true;
		}
	}
	else if(m_bAlphaToCoverageEnable)
	{
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		m_bAlphaToCoverageEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetBlendEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bBlendEnable)
		{
			glEnable(GL_BLEND);
			m_bBlendEnable = true;
		}
	}
	else if(m_bBlendEnable)
	{
		glDisable(GL_BLEND);
		m_bBlendEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetBlendFunc(VeUInt16 u16SrcColor, VeUInt16 u16DstColor, VeUInt16 u16SrcAlpha, VeUInt16 u16DstAlpha)
{
	if(u16SrcColor != m_u16SrcBlend || u16DstColor != m_u16DstBlend || u16SrcAlpha != m_u16SrcBlendAlpha || u16DstAlpha != m_u16DstBlendAlpha)
	{
		glBlendFuncSeparate(u16SrcColor, u16DstColor, u16SrcAlpha, u16DstAlpha);
		m_u16SrcBlend = u16SrcColor;
		m_u16DstBlend = u16DstColor;
		m_u16SrcBlendAlpha = u16SrcAlpha;
		m_u16DstBlendAlpha = u16DstAlpha;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetBlendOp(VeUInt16 u16ColorOp, VeUInt16 u16AlphaOp)
{
	if(u16ColorOp != m_u16BlendOp || u16AlphaOp != m_u16BlendOpAlpha)
	{
		glBlendEquationSeparate(u16ColorOp, u16AlphaOp);
		m_u16BlendOp = u16ColorOp;
		m_u16BlendOpAlpha = u16AlphaOp;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetWriteMask(VeUInt16 u16Mask)
{
	if(m_u16WriteMask != u16Mask)
	{
		glColorMask(VE_MASK_HAS_ANY(u16Mask, VeRenderer::COLOR_WRITE_ENABLE_RED) ? GL_TRUE : GL_FALSE,
			VE_MASK_HAS_ANY(u16Mask, VeRenderer::COLOR_WRITE_ENABLE_GREEN) ? GL_TRUE : GL_FALSE,
			VE_MASK_HAS_ANY(u16Mask, VeRenderer::COLOR_WRITE_ENABLE_BLUE) ? GL_TRUE : GL_FALSE,
			VE_MASK_HAS_ANY(u16Mask, VeRenderer::COLOR_WRITE_ENABLE_ALPHA) ? GL_TRUE : GL_FALSE);
		m_u16WriteMask = u16Mask;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetDepthEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bDepthEnable)
		{
			glEnable(GL_DEPTH_TEST);
			m_bDepthEnable = true;
		}
	}
	else if(m_bDepthEnable)
	{
		glDisable(GL_DEPTH_TEST);
		m_bDepthEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetDepthWrite(bool bWrite)
{
	if(bWrite)
	{
		if(!m_bDepthWrite)
		{
			glDepthMask(GL_FALSE);
			m_bDepthWrite = true;
		}
	}
	else if(m_bDepthWrite)
	{
		glDepthMask(GL_TRUE);
		m_bDepthWrite = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetDepthFunc(VeUInt16 u16Func)
{
	if(m_u16DepthFunc != u16Func)
	{
		glDepthFunc(u16Func);
		m_u16DepthFunc = u16Func;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetStencilEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bStencilEnable)
		{
			glEnable(GL_STENCIL_TEST);
			m_bStencilEnable = true;
		}
	}
	else if(m_bStencilEnable)
	{
		glDisable(GL_STENCIL_TEST);
		m_bStencilEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetStencilWriteMask(VeUInt8 u8Mask)
{
	if(m_u8StencilWriteMask != u8Mask)
	{
		glStencilMask(VE_INFINITE & u8Mask);
		m_u8StencilWriteMask = u8Mask;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetStencilFunc(VeUInt16 u16FrontFunc, VeUInt16 u16BackFunc, VeUInt32 u32Ref, VeUInt8 u8ReadMask)
{
	bool bFrontChange = false;
	bool bBackChange = false;
	if(u16FrontFunc != m_u16StencilFrontFunc)
	{
		m_u16StencilFrontFunc = u16FrontFunc;
		bFrontChange = true;
	}
	if(u16BackFunc != m_u16StencilBackFunc)
	{
		m_u16StencilBackFunc = u16BackFunc;
		bBackChange = true;
	}
	if(u32Ref != m_u32StencilRef)
	{
		m_u32StencilRef = u32Ref;
		bFrontChange = true;
		bBackChange = true;
	}
	if(u8ReadMask != m_u8StencilReadMask)
	{
		m_u8StencilReadMask = u8ReadMask;
		bFrontChange = true;
		bBackChange = true;
	}
	if(bFrontChange)
	{
		glStencilFuncSeparate(GL_FRONT, m_u16StencilFrontFunc, m_u32StencilRef, VE_INFINITE & m_u8StencilReadMask);
	}
	if(bBackChange)
	{
		glStencilFuncSeparate(GL_BACK, m_u16StencilBackFunc, m_u32StencilRef, VE_INFINITE & m_u8StencilReadMask);
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetStencilFrontOp(VeUInt16 u16Fail, VeUInt16 u16DepthFail, VeUInt16 u16Pass)
{
	bool bChange = false;
	if(m_u16StencilFrontFailOp != u16Fail)
	{
		m_u16StencilFrontFailOp = u16Fail;
		bChange = true;
	}
	if(m_u16StencilFrontDepthFailOp != u16DepthFail)
	{
		m_u16StencilFrontDepthFailOp = u16DepthFail;
		bChange = true;
	}
	if(m_u16StencilFrontPassOp != u16Pass)
	{
		m_u16StencilFrontPassOp = u16Pass;
		bChange = true;
	}
	if(bChange)
	{
		glStencilOpSeparate(GL_FRONT, m_u16StencilFrontFailOp, m_u16StencilFrontDepthFailOp, m_u16StencilFrontPassOp);
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetStencilBackOp(VeUInt16 u16Fail, VeUInt16 u16DepthFail, VeUInt16 u16Pass)
{
	bool bChange = false;
	if(m_u16StencilBackFailOp != u16Fail)
	{
		m_u16StencilBackFailOp = u16Fail;
		bChange = true;
	}
	if(m_u16StencilBackDepthFailOp != u16DepthFail)
	{
		m_u16StencilBackDepthFailOp = u16DepthFail;
		bChange = true;
	}
	if(m_u16StencilBackPassOp != u16Pass)
	{
		m_u16StencilBackPassOp = u16Pass;
		bChange = true;
	}
	if(bChange)
	{
		glStencilOpSeparate(GL_BACK, m_u16StencilBackFailOp, m_u16StencilBackDepthFailOp, m_u16StencilBackPassOp);
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetCullEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bCullEnable)
		{
			glEnable(GL_CULL_FACE);
			m_bCullEnable = true;
		}
	}
	else if(m_bCullEnable)
	{
		glDisable(GL_CULL_FACE);
		m_bCullEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetDepthBiasEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bDepthBiasEnable)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			m_bDepthBiasEnable = true;
		}
	}
	else if(m_bDepthBiasEnable)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		m_bDepthBiasEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetMultisampleEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bMultisampleEnable)
		{
			glEnable(GL_SAMPLE_COVERAGE);
			m_bMultisampleEnable = true;
		}
	}
	else if(m_bMultisampleEnable)
	{
		glDisable(GL_SAMPLE_COVERAGE);
		m_bMultisampleEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetScissorEnable(bool bEnable)
{
	if(bEnable)
	{
		if(!m_bScissorEnable)
		{
			glEnable(GL_SCISSOR_TEST);
			m_bScissorEnable = true;
		}
	}
	else if(m_bScissorEnable)
	{
		glDisable(GL_SCISSOR_TEST);
		m_bScissorEnable = false;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetCullFace(VeUInt16 u16Cull)
{
	if(m_u16CullFace != u16Cull)
	{
		glCullFace(u16Cull);
		m_u16CullFace = u16Cull;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetFrontFace(VeUInt16 u16Front)
{
	if(m_u16FrontFace != u16Front)
	{
		glFrontFace(u16Front);
		m_u16FrontFace = u16Front;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetPolyOffset(VeFloat32 f32Factor, VeFloat32 f32Units)
{
	bool bChange = false;
	if(m_f32PolyFactor != f32Factor)
	{
		m_f32PolyFactor = f32Factor;
		bChange = true;
	}
	if(m_f32PolyUnits != f32Units)
	{
		m_f32PolyUnits = f32Units;
		bChange = true;
	}
	if(bChange)
	{
		glPolygonOffset(m_f32PolyFactor, m_f32PolyUnits);
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetPSTexture(VeUInt32 u32Slot, VeUInt32 u32Texture)
{
	if(m_au32PSTexture[u32Slot] != u32Texture)
	{
		glActiveTexture(GL_TEXTURE0 + u32Slot);
		glBindTexture(GL_TEXTURE_2D, u32Texture);
		m_au32PSTexture[u32Slot] = u32Texture;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::SetVSTexture(VeUInt32 u32Slot, VeUInt32 u32Texture)
{
	if(m_au32VSTexture[u32Slot] != u32Texture)
	{
		glActiveTexture(GL_TEXTURE0 + VE_GLES2_PS_TEX_NUM + u32Slot);
		glBindTexture(GL_TEXTURE_2D, u32Texture);
		m_au32VSTexture[u32Slot] = u32Texture;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::ClearColor(const VeColorA& kColor)
{
	if(kColor != m_kClearColor)
	{
		glClearColor(kColor.r, kColor.g, kColor.b, kColor.a);
		m_kClearColor = kColor;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::ClearDepth(VeFloat32 f32Depth)
{
	if(f32Depth != m_f32ClearDepth)
	{
		glClearDepthf(f32Depth);
		m_f32ClearDepth = f32Depth;
	}
}
//--------------------------------------------------------------------------
void VeRendererGLES2::ClearStencil(VeUInt8 u8Stencil)
{
	if(u8Stencil != m_u8ClearStencil)
	{
		glClearStencil(u8Stencil);
		m_u8ClearStencil = u8Stencil;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeRendererGLES2::GetFrameBuffer(VeRORenderTargetViewGLES2* pkRTV, VeRODepthStencilViewGLES2* pkDSV)
{
	static VeChar8 acBuffer[256];
	if(pkDSV)
	{
		VeSprintf(acBuffer, 256, "%s#%s", pkRTV->GetName(), pkDSV->GetName());
	}
	else
	{
		VeSprintf(acBuffer, 256, "%s", pkRTV->GetName());
	}
	VeUInt32* pu32Iter = m_kFrameBufferObjectMap.Find(acBuffer);
	if(!pu32Iter)
	{
		VeUInt32 u32FrameBufferObject(0);
		glGenFramebuffers(1, &u32FrameBufferObject);
		BindFrameBuffer(u32FrameBufferObject);
		VE_ASSERT(pkRTV && pkRTV->m_spTexture);
		if(pkRTV->m_spTexture->m_u32Target == GL_TEXTURE_2D)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pkRTV->m_spTexture->m_u32Texture, pkRTV->m_u32Level);
		}
		else
		{
			VE_ASSERT(pkRTV->m_spTexture->m_u32Target == GL_RENDERBUFFER);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pkRTV->m_spTexture->m_u32Texture);
		}
		if(pkDSV)
		{
			if(pkDSV->m_spTexture->m_u32Target == GL_TEXTURE_2D)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pkDSV->m_spTexture->m_u32Texture, pkDSV->m_u32Level);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, pkDSV->m_spTexture->m_u32Texture, pkDSV->m_u32Level);
			}
			else
			{
				VE_ASSERT(pkDSV->m_spTexture->m_u32Target == GL_RENDERBUFFER);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pkDSV->m_spTexture->m_u32Texture);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pkDSV->m_spTexture->m_u32Texture);
			}
		}
		m_kFrameBufferObjectArray.PushBack(u32FrameBufferObject);
		pu32Iter = &(m_kFrameBufferObjectMap[acBuffer]);
		*pu32Iter = u32FrameBufferObject;
	}
	return *pu32Iter;
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
