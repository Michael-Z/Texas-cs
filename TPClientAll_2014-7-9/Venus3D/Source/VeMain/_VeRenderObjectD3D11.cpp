////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeRenderObjectD3D11.cpp
//  Version:     v1.00
//  Created:     31/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#include "_VeRendererD3D11.h"

//--------------------------------------------------------------------------
#ifdef VE_ENABLE_D3D11
//--------------------------------------------------------------------------
VeImplementRTTI(VeROBlendStateD3D11, VeROBlendState);
//--------------------------------------------------------------------------
VeROBlendStateD3D11::VeROBlendStateD3D11(const VeChar8* pcName, ID3D11BlendState* pkObj)
	: VeROBlendState(pcName), m_pkBlendState(pkObj)
{

}
//--------------------------------------------------------------------------
VeROBlendStateD3D11::~VeROBlendStateD3D11()
{
	VE_SAFE_RELEASE(m_pkBlendState);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRODepthStencilStateD3D11, VeRODepthStencilState);
//--------------------------------------------------------------------------
VeRODepthStencilStateD3D11::VeRODepthStencilStateD3D11(const VeChar8* pcName, ID3D11DepthStencilState* pkObj)
	: VeRODepthStencilState(pcName), m_pkDepthStencilState(pkObj)
{

}
//--------------------------------------------------------------------------
VeRODepthStencilStateD3D11::~VeRODepthStencilStateD3D11()
{
	VE_SAFE_RELEASE(m_pkDepthStencilState);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROInputLayoutD3D11, VeROInputLayout);
//--------------------------------------------------------------------------
VeROInputLayoutD3D11::VeROInputLayoutD3D11(const VeChar8* pcName, ID3D11InputLayout* pkObj)
	: VeROInputLayout(pcName), m_pkInputLayout(pkObj)
{

}
//--------------------------------------------------------------------------
VeROInputLayoutD3D11::~VeROInputLayoutD3D11()
{
	VE_SAFE_RELEASE(m_pkInputLayout);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRORasterizeStateD3D11, VeRORasterizeState);
//--------------------------------------------------------------------------
VeRORasterizeStateD3D11::VeRORasterizeStateD3D11(const VeChar8* pcName, ID3D11RasterizerState* pkObj)
	: VeRORasterizeState(pcName), m_pkRasterizerState(pkObj)
{

}
//--------------------------------------------------------------------------
VeRORasterizeStateD3D11::~VeRORasterizeStateD3D11()
{
	VE_SAFE_RELEASE(m_pkRasterizerState);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROSamplerStateD3D11, VeROSamplerState);
//--------------------------------------------------------------------------
VeROSamplerStateD3D11::VeROSamplerStateD3D11(const VeChar8* pcName, ID3D11SamplerState* pkObj)
	: VeROSamplerState(pcName), m_pkSamplerState(pkObj)
{

}
//--------------------------------------------------------------------------
VeROSamplerStateD3D11::~VeROSamplerStateD3D11()
{
	VE_SAFE_RELEASE(m_pkSamplerState);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROBufferD3D11, VeROBuffer);
//--------------------------------------------------------------------------
VeROBufferD3D11::VeROBufferD3D11(const VeChar8* pcName, ID3D11Buffer* pkObj)
	: VeROBuffer(pcName), m_pkBuffer(pkObj)
{

}
//--------------------------------------------------------------------------
VeROBufferD3D11::~VeROBufferD3D11()
{
	VE_SAFE_RELEASE(m_pkBuffer);
}
//--------------------------------------------------------------------------
VeUInt32 VeROBufferD3D11::GetBindFlag()
{
	VE_ASSERT(m_pkBuffer);
	D3D11_BUFFER_DESC kDesc;
	m_pkBuffer->GetDesc(&kDesc);
	return kDesc.BindFlags;
}
//--------------------------------------------------------------------------
VeUInt32 VeROBufferD3D11::GetCpuAccessFlag()
{
	VE_ASSERT(m_pkBuffer);
	D3D11_BUFFER_DESC kDesc;
	m_pkBuffer->GetDesc(&kDesc);
	return kDesc.CPUAccessFlags;
}
//--------------------------------------------------------------------------
VeUInt32 VeROBufferD3D11::GetResourceMiscFlag()
{
	VE_ASSERT(m_pkBuffer);
	D3D11_BUFFER_DESC kDesc;
	m_pkBuffer->GetDesc(&kDesc);
	return kDesc.MiscFlags;
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRODepthStencilViewD3D11, VeRODepthStencilView);
//--------------------------------------------------------------------------
VeRODepthStencilViewD3D11::VeRODepthStencilViewD3D11(const VeChar8* pcName, ID3D11DepthStencilView* pkObj)
	: VeRODepthStencilView(pcName), m_pkDepthStencilView(pkObj)
{

}
//--------------------------------------------------------------------------
VeRODepthStencilViewD3D11::~VeRODepthStencilViewD3D11()
{
	VE_SAFE_RELEASE(m_pkDepthStencilView);
}
//--------------------------------------------------------------------------
void VeRODepthStencilViewD3D11::OnDel()
{
	m_spShaderResourceView = NULL;
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRORenderTargetViewD3D11, VeRORenderTargetView);
//--------------------------------------------------------------------------
VeRORenderTargetViewD3D11::VeRORenderTargetViewD3D11(const VeChar8* pcName, ID3D11RenderTargetView* pkObj)
	: VeRORenderTargetView(pcName), m_pkRenderTargetView(pkObj), m_u32Active(VeUInt32(-1))
{

}
//--------------------------------------------------------------------------
VeRORenderTargetViewD3D11::~VeRORenderTargetViewD3D11()
{
	VE_SAFE_RELEASE(m_pkRenderTargetView);
}
//--------------------------------------------------------------------------
void VeRORenderTargetViewD3D11::OnDel()
{
	m_spShaderResourceView = NULL;
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROShaderResourceViewD3D11, VeROShaderResourceView);
//--------------------------------------------------------------------------
VeROShaderResourceViewD3D11::VeROShaderResourceViewD3D11(const VeChar8* pcName, ID3D11ShaderResourceView* pkObj)
	: VeROShaderResourceView(pcName), m_pkShaderResourceView(pkObj), m_u64Active(VeUInt64(-1))
{

}
//--------------------------------------------------------------------------
VeROShaderResourceViewD3D11::~VeROShaderResourceViewD3D11()
{
	VE_SAFE_RELEASE(m_pkShaderResourceView);
}
//--------------------------------------------------------------------------
void VeROShaderResourceViewD3D11::OnDel()
{
	m_spRenderTargetView = NULL;
	m_spDepthStencilView = NULL;
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROTexture1DD3D11, VeROTexture1D);
//--------------------------------------------------------------------------
VeROTexture1DD3D11::VeROTexture1DD3D11(const VeChar8* pcName, ID3D11Texture1D* pkObj)
	: VeROTexture1D(pcName), m_pkTexture1D(pkObj)
{

}
//--------------------------------------------------------------------------
VeROTexture1DD3D11::~VeROTexture1DD3D11()
{
	VE_SAFE_RELEASE(m_pkTexture1D);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROTexture2DD3D11, VeROTexture2D);
//--------------------------------------------------------------------------
VeROTexture2DD3D11::VeROTexture2DD3D11(const VeChar8* pcName, ID3D11Texture2D* pkObj)
	: VeROTexture2D(pcName), m_pkTexture2D(pkObj)
{

}
//--------------------------------------------------------------------------
VeROTexture2DD3D11::~VeROTexture2DD3D11()
{
	VE_SAFE_RELEASE(m_pkTexture2D);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROTexture3DD3D11, VeROTexture3D);
//--------------------------------------------------------------------------
VeROTexture3DD3D11::VeROTexture3DD3D11(const VeChar8* pcName, ID3D11Texture3D* pkObj)
	: VeROTexture3D(pcName), m_pkTexture3D(pkObj)
{

}
//--------------------------------------------------------------------------
VeROTexture3DD3D11::~VeROTexture3DD3D11()
{
	VE_SAFE_RELEASE(m_pkTexture3D);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROComputeShaderD3D11, VeROComputeShader);
//--------------------------------------------------------------------------
VeROComputeShaderD3D11::VeROComputeShaderD3D11(const VeChar8* pcName, ID3D11ComputeShader* pkObj)
	: VeROComputeShader(pcName), m_pkShader(pkObj)
{

}
//--------------------------------------------------------------------------
VeROComputeShaderD3D11::~VeROComputeShaderD3D11()
{
	VE_SAFE_RELEASE(m_pkShader);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeRODomainShaderD3D11, VeRODomainShader);
//--------------------------------------------------------------------------
VeRODomainShaderD3D11::VeRODomainShaderD3D11(const VeChar8* pcName, ID3D11DomainShader* pkObj)
	: VeRODomainShader(pcName), m_pkShader(pkObj)
{

}
//--------------------------------------------------------------------------
VeRODomainShaderD3D11::~VeRODomainShaderD3D11()
{
	VE_SAFE_RELEASE(m_pkShader);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROGeometryShaderD3D11, VeROGeometryShader);
//--------------------------------------------------------------------------
VeROGeometryShaderD3D11::VeROGeometryShaderD3D11(const VeChar8* pcName, ID3D11GeometryShader* pkObj)
	: VeROGeometryShader(pcName), m_pkShader(pkObj)
{

}
//--------------------------------------------------------------------------
VeROGeometryShaderD3D11::~VeROGeometryShaderD3D11()
{
	VE_SAFE_RELEASE(m_pkShader);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROHullShaderD3D11, VeROHullShader);
//--------------------------------------------------------------------------
VeROHullShaderD3D11::VeROHullShaderD3D11(const VeChar8* pcName, ID3D11HullShader* pkObj)
	: VeROHullShader(pcName), m_pkShader(pkObj)
{

}
//--------------------------------------------------------------------------
VeROHullShaderD3D11::~VeROHullShaderD3D11()
{
	VE_SAFE_RELEASE(m_pkShader);
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROPixelShaderD3D11, VeROPixelShader);
//--------------------------------------------------------------------------
VeROPixelShaderD3D11::VeROPixelShaderD3D11(const VeChar8* pcName, ID3D11PixelShader* pkObj)
	: VeROPixelShader(pcName), m_pkShader(pkObj)
{

}
//--------------------------------------------------------------------------
VeROPixelShaderD3D11::~VeROPixelShaderD3D11()
{
	VE_SAFE_RELEASE(m_pkShader);
}
//--------------------------------------------------------------------------
bool VeROPixelShaderD3D11::Update(VeMemoryIStream& kData)
{
	ID3D11PixelShader* pkD3D11Shader(NULL);
	HRESULT hRes = ((VeRendererD3D11*)g_pRenderer)->GetDevice()->CreatePixelShader(kData.GetData(), kData.RemainingLength(), NULL, &pkD3D11Shader);
	if(VE_SUCCEEDED(hRes))
	{
		VE_SAFE_RELEASE(m_pkShader);
		m_pkShader = pkD3D11Shader;
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------
VeImplementRTTI(VeROVertexShaderD3D11, VeROVertexShader);
//--------------------------------------------------------------------------
VeROVertexShaderD3D11::VeROVertexShaderD3D11(const VeChar8* pcName, ID3D11VertexShader* pkObj)
	: VeROVertexShader(pcName), m_pkShader(pkObj)
{

}
//--------------------------------------------------------------------------
VeROVertexShaderD3D11::~VeROVertexShaderD3D11()
{
	VE_SAFE_RELEASE(m_pkShader);
}
//--------------------------------------------------------------------------
bool VeROVertexShaderD3D11::Update(VeMemoryIStream& kData)
{
	ID3D11VertexShader* pkD3D11Shader(NULL);
	HRESULT hRes = ((VeRendererD3D11*)g_pRenderer)->GetDevice()->CreateVertexShader(kData.GetData(), kData.RemainingLength(), NULL, &pkD3D11Shader);
	if(VE_SUCCEEDED(hRes))
	{
		VE_SAFE_RELEASE(m_pkShader);
		m_pkShader = pkD3D11Shader;
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------
VeMSAAResolveD3D11::VeMSAAResolveD3D11(const VeChar8* pcName)
	: VeRenderProcess(TYPE_MSAA_RESOLVE, pcName)
{

}
//--------------------------------------------------------------------------
VeMSAAResolveD3D11::~VeMSAAResolveD3D11()
{
	Clear();
}
//--------------------------------------------------------------------------
bool VeMSAAResolveD3D11::Update(VeBinaryIStream& kStream)
{
	Clear();
	VeUInt16 u16RenderTargetNum, u16DepthStencilTargetNum;
	kStream >> u16RenderTargetNum;
	kStream >> u16DepthStencilTargetNum;
	VeStringA kTemp;
	for(VeUInt32 i(0); i < VeUInt32(u16RenderTargetNum + u16DepthStencilTargetNum); ++i)
	{
		VeROTexture2DPtr spTexture;
		kStream.GetStringAligned4(kTemp);
		g_pRenderer->GetRenderObject(kTemp, spTexture);
		VeROTexture2DD3D11* pkDst = (VeROTexture2DD3D11*)(VeROTexture2D*)spTexture;
		kStream.GetStringAligned4(kTemp);
		g_pRenderer->GetRenderObject(kTemp, spTexture);
		VeROTexture2DD3D11* pkSrc = (VeROTexture2DD3D11*)(VeROTexture2D*)spTexture;
		if(pkDst && pkSrc)
		{
			pkDst->m_pkTexture2D->AddRef();
			pkSrc->m_pkTexture2D->AddRef();
			m_kResolveLinks.PushBack(VePair<ID3D11Texture2D*,ID3D11Texture2D*>(pkDst->m_pkTexture2D, pkSrc->m_pkTexture2D));
		}
	}
	return m_kResolveLinks.Size() ? true : false;
}
//--------------------------------------------------------------------------
void VeMSAAResolveD3D11::Clear()
{
	for(VeUInt32 i(0); i < m_kResolveLinks.Size(); ++i)
	{
		m_kResolveLinks[i].m_tFirst->Release();
		m_kResolveLinks[i].m_tSecond->Release();
	}
	m_kResolveLinks.Clear();
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
