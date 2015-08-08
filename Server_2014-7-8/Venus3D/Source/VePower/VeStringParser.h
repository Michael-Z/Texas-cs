////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeStringParser.h
//  Version:     v1.00
//  Created:     19/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_POWER_API VeStringParser : public VeSingleton<VeStringParser>
{
public:
	enum API
	{
		API_D3D11,
		API_OGL
	};

	struct Resolution
	{
		VeUInt32 m_u32Width;
		VeUInt32 m_u32Height;
	};

	enum WinDock
	{
		DOCK_DEFAULT,
		DOCK_CORNER,
		DOCK_CENTER
	};

	enum WinStyle
	{
		STYLE_FIXED,
		STYLE_VARIBLE,
		STYLE_POPUP
	};

	enum BackBufferFormat
	{
		FMT_C8A8,
		FMT_C10A2
	};

	VeStringParser();

	~VeStringParser();
    
    VeDirectory::Type ParseToType(const VeChar8* pcStr, const VeDirectory::Type& eType);

	API ParseToType(const VeChar8* pcStr, const API& eApi);

	Resolution ParseToType(const VeChar8* pcStr, const Resolution& kResolution);

	WinDock ParseToType(const VeChar8* pcStr, const WinDock& eDock);

	WinStyle ParseToType(const VeChar8* pcStr, const WinStyle& eStyle);

	bool ParseToType(const VeChar8* pcStr, const bool& bDefault);

	VeUInt32 ParseToType(const VeChar8* pcStr, const VeUInt32& u32Default);

	BackBufferFormat ParseToType(const VeChar8* pcStr, const BackBufferFormat& eFormat);

	bool HasConfig(const VeChar8* pcStr);

	VeFixedStringA GetConfig(const VeChar8* pcStr);

    template <typename T>
	T Parse(const VeChar8* pcStr, const T& tDefault)
	{
		VeChar8 acBuffer[256];
		VeStrToLower(acBuffer, 256, pcStr);
		return ParseToType(acBuffer, tDefault);
	}

protected:
	void InitDirType();

	void InitConfig();

	void InitApi();

	void InitWinDock();

	void InitWinStyle();

	void InitBool();

	void InitFormat();

    VeStringMap<VeDirectory::Type> m_kDirType;
	VeStringMap<VeFixedStringA> m_kConfig;
	VeStringMap<API> m_kApi;
	VeStringMap<WinDock> m_kWinDock;
	VeStringMap<WinStyle> m_kWinSytle;
	VeStringMap<bool> m_kBool;
	VeStringMap<BackBufferFormat> m_kFormat;
    
};

#define g_pStringParser VeStringParser::GetSingletonPtr()
