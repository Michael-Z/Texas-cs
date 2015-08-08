////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Include File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeStartConfig.inl
//  Version:     v1.00
//  Created:     7/12/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
VE_INLINE VeRenderer::API VeStartConfig::GetAPI()
{
#if defined(VE_PLATFORM_WIN)
	VeStringParser::API eApi = g_pStringParser->Parse(GetConfig("Api"),
		VeStringParser::API_D3D11);
	if(eApi ==  VeStringParser::API_OGL)
		return VeRenderer::API_OGL;
	else
		return VeRenderer::API_D3D11;
#elif defined(VE_PLATFORM_PC)
	return VeRenderer::API_OGL;
#else
	return VeRenderer::API_OGLES2;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetFullScreen(bool bEnable)
{
#ifdef VE_PLATFORM_PC
	SetConfig("FullScreen", bEnable ? "true" : "false");
#endif
}
//--------------------------------------------------------------------------
VE_INLINE bool VeStartConfig::IsFullScreen()
{
#ifdef VE_PLATFORM_PC
	bool bRes = g_pStringParser->Parse(GetConfig("FullScreen"), false);
	return bRes;
#else
	return false;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetMaxScreen(bool bEnable)
{
#ifdef VE_PLATFORM_PC
	SetConfig("MaxScreen", bEnable ? "true" : "false");
#endif
}
//--------------------------------------------------------------------------
VE_INLINE bool VeStartConfig::IsMaxScreen()
{
#ifdef VE_PLATFORM_PC
	bool bRes = g_pStringParser->Parse(GetConfig("MaxScreen"), true);
	return bRes;
#else
	return false;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetSync(bool bSync)
{
#ifdef VE_PLATFORM_PC
	SetConfig("VerticalSync", bSync ? "true" : "false");
#endif
}
//--------------------------------------------------------------------------
VE_INLINE bool VeStartConfig::IsSync()
{
#ifdef VE_PLATFORM_PC
	bool bRes = g_pStringParser->Parse(GetConfig("VerticalSync"), false);
	return bRes;
#else
	return true;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetShowMaximized(bool bShow)
{
#ifdef VE_PLATFORM_PC
	SetConfig("ShowMaximized", bShow ? "true" : "false");
#endif
}
//--------------------------------------------------------------------------
VE_INLINE bool VeStartConfig::GetShowMaximized()
{
#ifdef VE_PLATFORM_PC
	bool bRes = g_pStringParser->Parse(GetConfig("ShowMaximized"), false);
	return bRes;
#else
	return false;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetResolution(VeUInt32 u32Width,
	VeUInt32 u32Height)
{
#if defined(VE_PLATFORM_PC) || defined(VE_PLATFORM_MOBILE_SIM)
	VeChar8 acBuffer[128];
	VeSprintf(acBuffer, 128, "%dx%d", u32Width, u32Height);
	SetConfig("Resolution", acBuffer);
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetAdapter(VeUInt32 u32Adapter)
{
#ifdef VE_PLATFORM_PC
	VeChar8 acBuffer[128];
	VeSprintf(acBuffer, 128, "%d", u32Adapter);
	SetConfig("Adapter", acBuffer);
#endif
}
//--------------------------------------------------------------------------
VE_INLINE VeUInt32 VeStartConfig::GetAdapter()
{
#ifdef VE_PLATFORM_PC
	VeUInt32 u32Res = g_pStringParser->Parse(GetConfig("Adapter"), 0U);
	return u32Res;
#else
	return 0;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetOutput(VeUInt32 u32Output)
{
#ifdef VE_PLATFORM_PC
	VeChar8 acBuffer[128];
	VeSprintf(acBuffer, 128, "%d", u32Output);
	SetConfig("Output", acBuffer);
#endif
}
//--------------------------------------------------------------------------
VE_INLINE VeUInt32 VeStartConfig::GetOutput()
{
#ifdef VE_PLATFORM_PC
	VeUInt32 u32Res = g_pStringParser->Parse(GetConfig("Output"), 0U);
	return u32Res;
#else
	return 0;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetRefreshRate(VeUInt32 u32RefreshRate)
{
#ifdef VE_PLATFORM_PC
	VeChar8 acBuffer[128];
	VeSprintf(acBuffer, 128, "%d", u32RefreshRate);
	SetConfig("RefreshRate", acBuffer);
#endif
}
//--------------------------------------------------------------------------
VE_INLINE VeUInt32 VeStartConfig::GetRefreshRate()
{
#ifdef VE_PLATFORM_PC
	VeUInt32 u32Res = g_pStringParser->Parse(GetConfig("RefreshRate"), 0U);
	return u32Res;
#else
	return 60;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetFormat(
	VeStringParser::BackBufferFormat eFormat)
{
#ifdef VE_PLATFORM_PC
	SetConfig("Format",
		(eFormat == VeStringParser::FMT_C10A2) ? "fmt_c10a2" : "fmt_c8a8");
#endif
}
//--------------------------------------------------------------------------
VE_INLINE VeStringParser::BackBufferFormat VeStartConfig::GetFormat()
{
#ifdef VE_PLATFORM_PC
	return g_pStringParser->Parse(GetConfig("Format"),
		VeStringParser::FMT_C8A8);
#else
	return VeStringParser::FMT_C8A8;
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetWinDock(VeStringParser::WinDock eDock)
{
#ifdef VE_PLATFORM_PC
	switch(eDock)
	{
	case VeStringParser::DOCK_CORNER:
		SetConfig("WinDock", "corner");
		break;
	case VeStringParser::DOCK_CENTER:
		SetConfig("WinDock", "center");
		break;
	default:
		SetConfig("WinDock", "default");
		break;
	}
#endif
}
//--------------------------------------------------------------------------
VE_INLINE void VeStartConfig::SetWinStyle(VeStringParser::WinStyle eStyle)
{
#ifdef VE_PLATFORM_PC
	switch(eStyle)
	{
	case VeStringParser::STYLE_VARIBLE:
		SetConfig("WinStyle", "varible");
		break;
	case VeStringParser::STYLE_POPUP:
		SetConfig("WinStyle", "popup");
		break;
	default:
		SetConfig("WinStyle", "fixed");
		break;
	}
#endif
}
//--------------------------------------------------------------------------
