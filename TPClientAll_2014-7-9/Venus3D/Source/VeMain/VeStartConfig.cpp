////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeStartConfig.cpp
//  Version:     v1.00
//  Created:     18/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeStartConfig.inl"
#endif

//--------------------------------------------------------------------------
VeStartConfig::VeStartConfig(const VeChar8* pcName
	, const VeChar8* pcCustomDir, const VeChar8* pcDefaultDir)
	: m_kName(pcName), m_pkCustom(NULL)
	, m_pkDefault(NULL), m_kConfigDoc(pcName)
{
	m_kDelegate.Set(this, &VeStartConfig::OnWriteEnd);
	m_pkCustom = pcCustomDir
		? g_pResourceManager->CreateDir(pcCustomDir) : NULL;
	m_pkDefault = pcDefaultDir
		? g_pResourceManager->CreateDir(pcDefaultDir) : NULL;
	VeDirectory* pkRead(NULL);
	if(m_pkCustom && m_pkCustom->HasFile(m_kName))
	{
		pkRead = m_pkCustom;
	}
	else if(m_pkDefault && m_pkDefault->HasFile(m_kName))
	{
		pkRead = m_pkDefault;
	}

	if(pkRead)
	{
		VeBinaryIStreamPtr spStream = pkRead->OpenSync(m_kName);
		(*spStream) >> m_kConfigDoc;
		VE_ASSERT(!m_kConfigDoc.Error());
	}

	FillStructs();
}
//--------------------------------------------------------------------------
VeStartConfig::~VeStartConfig()
{
	UpdateXML();
	if(m_pkCustom)
	{
		g_pResourceManager->DestoryDir(m_pkCustom);
		m_pkCustom = NULL;
	}
	if(m_pkDefault)
	{
		g_pResourceManager->DestoryDir(m_pkDefault);
		m_pkDefault = NULL;
	}
}
//--------------------------------------------------------------------------
void VeStartConfig::FillStructs()
{
	m_kConfigArray.Clear();
	m_kConfigMap.Clear();
	VeXMLElement* pkRoot = m_kConfigDoc.RootElement();
	if(pkRoot)
	{
		VeXMLElement* pkGeneral = pkRoot->FirstChildElement("General");
		while(pkGeneral)
		{
			VeXMLElement* pkConfig = pkGeneral->FirstChildElement();
			while(pkConfig)
			{
				m_kConfigMap[pkConfig->Value()] = m_kConfigArray.Size();
				m_kConfigArray.Resize(m_kConfigArray.Size() + 1);
				m_kConfigArray.Back().m_tFirst = pkConfig->Value();
				m_kConfigArray.Back().m_tSecond = pkConfig->GetText();
				pkConfig = pkConfig->NextSiblingElement();
			}
			pkGeneral = pkGeneral->NextSiblingElement("General");
		}

		VeXMLElement* pkPlatform = pkRoot->FirstChildElement(VE_PLATFORM_NAME);
		while(pkPlatform)
		{
			VeXMLElement* pkConfig = pkPlatform->FirstChildElement();
			while(pkConfig)
			{
				VeUInt32* pu32Index = m_kConfigMap.Find(pkConfig->Value());
				const VeChar8* pcText = pkConfig->GetText();
				if(pu32Index)
				{
					m_kConfigArray[*pu32Index].m_tSecond = pcText
						? VeFixedStringA(pcText)
						: g_pStringParser->GetConfig(pkConfig->Value());
				}
				else
				{
					m_kConfigMap[pkConfig->Value()] = m_kConfigArray.Size();
					m_kConfigArray.Resize(m_kConfigArray.Size() + 1);
					m_kConfigArray.Back().m_tFirst = pkConfig->Value();
					m_kConfigArray.Back().m_tSecond = pcText
						? VeFixedStringA(pcText)
						: g_pStringParser->GetConfig(pkConfig->Value());
				}
				pkConfig = pkConfig->NextSiblingElement();
			}
			pkPlatform = pkPlatform->NextSiblingElement(VE_PLATFORM_NAME);
		}
	}
}
//--------------------------------------------------------------------------
void VeStartConfig::UpdateXMLDoc()
{
	VeXMLElement* pkRoot = m_kConfigDoc.RootElement();
	if(!pkRoot)
	{
		VeXMLElement kRoot("Config");
		pkRoot = m_kConfigDoc.InsertEndChild(kRoot)->ToElement();
		VE_ASSERT(pkRoot);
	}

	VeXMLElement* pkPlatform = pkRoot->FirstChildElement(VE_PLATFORM_NAME);
	if(!pkPlatform)
	{
		VeXMLElement kPlatform(VE_PLATFORM_NAME);
		pkPlatform = pkRoot->InsertEndChild(kPlatform)->ToElement();
		VE_ASSERT(pkPlatform);
	}

	for(VeUInt32 i(0); i < m_kConfigArray.Size(); ++i)
	{
		VePair<VeFixedStringA,VeFixedStringA>& kConfig = m_kConfigArray[i];
		VeXMLElement* pkConfig = pkPlatform->FirstChildElement(kConfig.m_tFirst);
		if(!pkConfig)
		{
			VeXMLElement kConfigElement(kConfig.m_tFirst);
			pkConfig = pkPlatform->InsertEndChild(kConfigElement)->ToElement();
			VE_ASSERT(pkConfig);
		}

		VeXMLNode* pkText = pkConfig->FirstChild();
		if(pkText && pkText->ToText())
		{
			pkText->ToText()->SetValue(kConfig.m_tSecond);
		}
		else
		{
			VeXMLText kText(kConfig.m_tSecond);
			pkConfig->InsertEndChild(kText);
		}
	}
}
//--------------------------------------------------------------------------
void VeStartConfig::UpdateXML()
{
	UpdateXMLDoc();
	if(m_pkCustom && m_pkCustom->Access(VeDirectory::ACCESS_W_OK))
	{
		VeMemoryOStreamPtr spContent = VE_NEW VeMemoryOStream();
		(*spContent) << m_kConfigDoc;
		m_pkCustom->WriteAsync(m_kName, spContent, m_kDelegate);
	}
}
//--------------------------------------------------------------------------
void VeStartConfig::OnWriteEnd(VE_RESULT eRes)
{
	
}
//--------------------------------------------------------------------------
void VeStartConfig::SetConfig(const VeChar8* pcConfig,
	const VeChar8* pcText)
{
	VeUInt32* pu32Config = m_kConfigMap.Find(pcConfig);
	if(pu32Config)
	{
		m_kConfigArray[*pu32Config].m_tSecond = pcText;
	}
	else
	{
		m_kConfigMap[pcConfig] = m_kConfigArray.Size();
		m_kConfigArray.Resize(m_kConfigArray.Size() + 1);
		m_kConfigArray.Back().m_tFirst = pcConfig;
		m_kConfigArray.Back().m_tSecond =
			g_pStringParser->GetConfig(pcConfig);
		VE_ASSERT(m_kConfigArray.Back().m_tSecond != "");
		m_kConfigArray.Back().m_tSecond = pcText;
	}
}
//--------------------------------------------------------------------------
VeFixedStringA VeStartConfig::GetConfig(const VeChar8* pcConfig)
{
	VeUInt32* pu32Config = m_kConfigMap.Find(pcConfig);
	if(pu32Config)
	{
		VeFixedStringA kA = m_kConfigArray[*pu32Config].m_tFirst;
		VeFixedStringA kB = m_kConfigArray[*pu32Config].m_tSecond;
		return kB;
	}
	else
	{
		m_kConfigMap[pcConfig] = m_kConfigArray.Size();
		m_kConfigArray.Resize(m_kConfigArray.Size() + 1);
		m_kConfigArray.Back().m_tFirst = pcConfig;
		m_kConfigArray.Back().m_tSecond =
			g_pStringParser->GetConfig(pcConfig);
		VE_ASSERT(m_kConfigArray.Back().m_tSecond != "");
		return m_kConfigArray.Back().m_tSecond;
	}
}
//--------------------------------------------------------------------------
