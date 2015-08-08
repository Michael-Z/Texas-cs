////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Test Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   GraphicsTestMain.cpp
//  Version:     v1.00
//  Created:     13/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#if defined(VE_PLATFORM_WIN)
#	include <vld.h>
#endif
#include <VeMainPCH.h>
#include "VeGraphicsTest.h"

VE_MAIN_FUNCTION
{
	VeStartParams kParams;
	VE_FILL_START_PARAMS(kParams, VE_TEXT("VeGraphicsTest"));
	VE_NEW VeGraphicsTest;
	return VeStartEngine(kParams);
}
