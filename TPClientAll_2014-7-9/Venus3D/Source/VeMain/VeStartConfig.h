////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeStartConfig.h
//  Version:     v1.00
//  Created:     18/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_MAIN_API VeStartConfig : public VeSingleton<VeStartConfig>
{
public:
	typedef VeMemberDelegate<VeStartConfig, VE_RESULT> WriteDelegate;

	VeStartConfig(const VeChar8* pcName, const VeChar8* pcCustomDir, const VeChar8* pcDefaultDir = NULL);

	~VeStartConfig();

	void UpdateXML();

	VE_INLINE VeRenderer::API GetAPI();

	VE_INLINE void SetFullScreen(bool bEnable);

	VE_INLINE bool IsFullScreen();

	VE_INLINE void SetMaxScreen(bool bEnable);

	VE_INLINE bool IsMaxScreen();

	VE_INLINE void SetSync(bool bSync);

	VE_INLINE bool IsSync();

	VE_INLINE void SetShowMaximized(bool bShow);

	VE_INLINE bool GetShowMaximized();

	VE_INLINE void SetResolution(VeUInt32 u32Width, VeUInt32 u32Height);

	VE_INLINE void SetAdapter(VeUInt32 u32Adapter);

	VE_INLINE VeUInt32 GetAdapter();

	VE_INLINE void SetOutput(VeUInt32 u32Output);

	VE_INLINE VeUInt32 GetOutput();

	VE_INLINE void SetRefreshRate(VeUInt32 u32RefreshRate);

	VE_INLINE VeUInt32 GetRefreshRate();

	VE_INLINE void SetFormat(VeStringParser::BackBufferFormat eFormat);

	VE_INLINE VeStringParser::BackBufferFormat GetFormat();

	VE_INLINE void SetWinDock(VeStringParser::WinDock eDock);

	VE_INLINE void SetWinStyle(VeStringParser::WinStyle eStyle);

	void SetConfig(const VeChar8* pcConfig, const VeChar8* pcText);

	VeFixedStringA GetConfig(const VeChar8* pcConfig);

	template <typename T>
	T GetConfig(const VeChar8* pcConfig, const T& tDefault)
	{
		return g_pStringParser->Parse(GetConfig(pcConfig), tDefault);
	}

protected:
	void FillStructs();

	void UpdateXMLDoc();

	void OnWriteEnd(VE_RESULT eRes);

	VeFixedStringA m_kName;
	VeDirectory* m_pkCustom;
	VeDirectory* m_pkDefault;
	VeXMLDocument m_kConfigDoc;
	VeVector< VePair<VeFixedStringA,VeFixedStringA> > m_kConfigArray;
	VeStringMap<VeUInt32> m_kConfigMap;
	WriteDelegate m_kDelegate;

};

#define g_pStartConfig VeStartConfig::GetSingletonPtr()

#ifndef VE_NO_INLINE
#	include "VeStartConfig.inl"
#endif
