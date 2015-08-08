////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeDirectory.cpp
//  Version:     v1.00
//  Created:     6/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"

//--------------------------------------------------------------------------
VeDirectory::VeDirectory(const VeChar8* pcName) : m_kName(pcName)
{
	VE_ASSERT(pcName[VeStrlen(pcName) - 1] != '/');
}
//--------------------------------------------------------------------------
VeDirectory::~VeDirectory()
{

}
//--------------------------------------------------------------------------
const VeChar8* VeDirectory::GetName()
{
	return m_kName;
}
//--------------------------------------------------------------------------
