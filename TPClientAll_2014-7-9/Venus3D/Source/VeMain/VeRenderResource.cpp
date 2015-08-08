////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeRenderResource.cpp
//  Version:     v1.00
//  Created:     8/11/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeRenderResource.inl"
#endif

//--------------------------------------------------------------------------
VeShaderResource::VeShaderResource(Type eType, const VeChar8* pcName,
	const VeChar8* pcGroup)
	: VeResource(eType, pcName, pcGroup)
{
	m_kLoadName = g_pRenderer->GetShaderName();
	m_kLoadName += '/';
	m_kLoadName += m_kName;
	m_kLoadName += '.';
	switch(m_eType)
	{
	case VERTEX_SHADER:
		m_kLoadName += "vs_";
		break;
	case PIXEL_SHADER:
		m_kLoadName += "ps_";
		break;
	default:
		break;
	}
	m_kLoadName += g_pRenderer->GetShaderProfile();
}
//--------------------------------------------------------------------------
VeShaderResource::~VeShaderResource()
{
	Unload();
}
//--------------------------------------------------------------------------
const VeChar8* VeShaderResource::GetLoadName()
{
	return m_kLoadName;	
}
//--------------------------------------------------------------------------
VeResource::State VeShaderResource::LoadImpl(const VeMemoryIStreamPtr& spStream)
{
	m_spStream = spStream;
	VE_ASSERT(m_spStream);
	switch(m_eType)
	{
	case VERTEX_SHADER:
		{
			VeROVertexShaderPtr spShader;
			g_pRenderer->GetRenderObject(m_kName, spShader);
			if(spShader)
			{
				spShader->Update(*m_spStream);
			}
			else
			{
				VE_ASSERT_EQ(g_pRenderer->CreateVertexShader(m_kName, *m_spStream), true);
			}
		}
		break;
	case PIXEL_SHADER:
		{
			VeROPixelShaderPtr spShader;
			g_pRenderer->GetRenderObject(m_kName, spShader);
			if(spShader)
			{
				spShader->Update(*m_spStream);
			}
			else
			{
				VE_ASSERT_EQ(g_pRenderer->CreatePixelShader(m_kName, *m_spStream), true);
			}
		}
		break;
	default:
		break;
	}
	return VeResource::LOADED;
}
//--------------------------------------------------------------------------
void VeShaderResource::UnloadImpl()
{
	m_spStream->Finish();
	m_spStream = NULL;
}
//--------------------------------------------------------------------------
VeResource* VeShaderResource::CreateVertexShaderResource(
	const VeChar8* pcName, const VeChar8* pcGroup)
{
	return VE_NEW VeShaderResource(VERTEX_SHADER, pcName, pcGroup);
}
//--------------------------------------------------------------------------
VeResource* VeShaderResource::CreatePixelShaderResource(
	const VeChar8* pcName, const VeChar8* pcGroup)
{
	return VE_NEW VeShaderResource(PIXEL_SHADER, pcName, pcGroup);
}
//--------------------------------------------------------------------------
