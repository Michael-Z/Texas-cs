////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 GetTime.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#ifndef __GET_TIME_H
#define __GET_TIME_H

#include "Export.h"
#include "VENetTime.h"

namespace VENet
{
VENet::Time VE_DLL_EXPORT GetTime( void );

VENet::TimeMS VE_DLL_EXPORT GetTimeMS( void );

VENet::TimeUS VE_DLL_EXPORT GetTimeUS( void );

extern VE_DLL_EXPORT bool GreaterThan(VENet::Time a, VENet::Time b);
extern VE_DLL_EXPORT bool LessThan(VENet::Time a, VENet::Time b);
}

#endif
