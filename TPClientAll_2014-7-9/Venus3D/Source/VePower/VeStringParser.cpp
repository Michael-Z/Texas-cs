////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeStringParser.cpp
//  Version:     v1.00
//  Created:     19/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"

//--------------------------------------------------------------------------
VeStringParser::VeStringParser()
{
	InitDirType();
	InitConfig();
#	if defined(VE_PLATFORM_WIN)
	InitApi();
	InitWinDock();
	InitWinStyle();
#	elif defined(VE_PLATFORM_LINUX)
	InitWinDock();
	InitWinStyle();
#	endif
	InitBool();
#	ifdef VE_PLATFORM_PC
	InitFormat();
#	endif
}
//--------------------------------------------------------------------------
VeStringParser::~VeStringParser()
{

}
//--------------------------------------------------------------------------
void VeStringParser::InitDirType()
{
	m_kDirType["file"] = VeDirectory::TYPE_FILE;
	m_kDirType["zip"] = VeDirectory::TYPE_ZIP;
	m_kDirType["asset"] = VeDirectory::TYPE_ASSET;
	m_kDirType["url"] = VeDirectory::TYPE_URL;
}
//--------------------------------------------------------------------------
void VeStringParser::InitConfig()
{
#if defined(VE_PLATFORM_WIN)

	m_kConfig["Api"] = "dx11";
	m_kConfig["WinDock"] = "default";
	m_kConfig["WinStyle"] = "varible";
	m_kConfig["FullScreen"] = "false";
	m_kConfig["MaxScreen"] = "false";
	m_kConfig["Resolution"] = "1024x768";
	m_kConfig["VerticalSync"] = "false";
	m_kConfig["Adapter"] = "0";
	m_kConfig["Output"] = "0";
	m_kConfig["RefreshRate"] = "60";
	m_kConfig["Format"] = "C8A8";
	m_kConfig["ShowMaximized"] = "false";

#elif defined(VE_PLATFORM_MOBILE_SIM)

	m_kConfig["Resolution"] = "960x640";

#elif defined(VE_PLATFORM_LINUX)

	m_kConfig["WinDock"] = "default";
	m_kConfig["WinStyle"] = "varible";
	m_kConfig["FullScreen"] = "false";
	m_kConfig["MaxScreen"] = "false";
	m_kConfig["Resolution"] = "1024x768";
	m_kConfig["VerticalSync"] = "false";
	m_kConfig["Adapter"] = "0";
	m_kConfig["Output"] = "0";
	m_kConfig["RefreshRate"] = "60";
	m_kConfig["Format"] = "C8A8";

#elif defined(VE_PLATFORM_OSX)

	m_kConfig["FullScreen"] = "false";
	m_kConfig["Adapter"] = "0";
	m_kConfig["Output"] = "0";
	m_kConfig["RefreshRate"] = "60";
	m_kConfig["Format"] = "C8A8";

#elif defined(VE_PLATFORM_IOS)

#elif defined(VE_PLATFORM_ANDROID)

#endif
}
//--------------------------------------------------------------------------
void VeStringParser::InitApi()
{
	m_kApi["dx11"] = API_D3D11;
	m_kApi["ogl"] = API_OGL;
}
//--------------------------------------------------------------------------
void VeStringParser::InitWinDock()
{
	m_kWinDock["default"] = DOCK_DEFAULT;
	m_kWinDock["corner"] = DOCK_CORNER;
	m_kWinDock["center"] = DOCK_CENTER;
}
//--------------------------------------------------------------------------
void VeStringParser::InitWinStyle()
{
	m_kWinSytle["fixed"] = STYLE_FIXED;
	m_kWinSytle["varible"] = STYLE_VARIBLE;
	m_kWinSytle["popup"] = STYLE_POPUP;
}
//--------------------------------------------------------------------------
void VeStringParser::InitBool()
{
	m_kBool["true"] = true;
	m_kBool["false"] = false;
	m_kBool["1"] = true;
	m_kBool["0"] = false;
}
//--------------------------------------------------------------------------
void VeStringParser::InitFormat()
{
	m_kFormat["fmt_c8a8"] = FMT_C8A8;
	m_kFormat["fmt_c10a2"] = FMT_C10A2;
}
//--------------------------------------------------------------------------
VeDirectory::Type VeStringParser::ParseToType(const VeChar8* pcStr, const VeDirectory::Type& eType)
{
    VeDirectory::Type* peIter = m_kDirType.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeStringParser::API VeStringParser::ParseToType(const VeChar8* pcStr, const API& eApi)
{
	API* peApi = m_kApi.Find(pcStr);
	return peApi ? *peApi : eApi;
}
//--------------------------------------------------------------------------
VeStringParser::Resolution VeStringParser::ParseToType(const VeChar8* pcStr, const Resolution& kResolution)
{
	VeChar8 acBuffer[128];
	VeStrcpy(acBuffer, 128, pcStr);
	VeChar8* pcContent;
	VeChar8* pcTemp = VeStrtok(acBuffer, "x", &pcContent);
	Resolution kRes;
	kRes.m_u32Width = VeUInt32(VeAtoi(pcTemp));
	pcTemp = VeStrtok(NULL, "x", &pcContent);
	kRes.m_u32Height = VeUInt32(VeAtoi(pcTemp));
	return kRes;
}
//--------------------------------------------------------------------------
VeStringParser::WinDock VeStringParser::ParseToType(const VeChar8* pcStr, const WinDock& eDock)
{
	WinDock* peIter = m_kWinDock.Find(pcStr);
	return peIter ? *peIter : eDock;
}
//--------------------------------------------------------------------------
VeStringParser::WinStyle VeStringParser::ParseToType(const VeChar8* pcStr, const WinStyle& eStyle)
{
	WinStyle* peIter = m_kWinSytle.Find(pcStr);
	return peIter ? *peIter : eStyle;
}
//--------------------------------------------------------------------------
bool VeStringParser::ParseToType(const VeChar8* pcStr, const bool& bDefault)
{
	bool* pbIter = m_kBool.Find(pcStr);
	return pbIter ? *pbIter : bDefault;
}
//--------------------------------------------------------------------------
VeUInt32 VeStringParser::ParseToType(const VeChar8* pcStr, const VeUInt32& u32Default)
{
	return VeAtoi(pcStr);
}
//--------------------------------------------------------------------------
VeStringParser::BackBufferFormat VeStringParser::ParseToType(const VeChar8* pcStr, const BackBufferFormat& eFormat)
{
	BackBufferFormat* peIter = m_kFormat.Find(pcStr);
	return peIter ? *peIter : eFormat;
}
//--------------------------------------------------------------------------
bool VeStringParser::HasConfig(const VeChar8* pcStr)
{
	return m_kConfig.Find(pcStr) ? true : false;
}
//--------------------------------------------------------------------------
VeFixedStringA VeStringParser::GetConfig(const VeChar8* pcStr)
{
	VeFixedStringA* pkRes = m_kConfig.Find(pcStr);
	return pkRes ? *pkRes : "";
}
//--------------------------------------------------------------------------
