////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PacketOutputWindowLogger.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_PacketLogger==1

#pragma once

#include "PacketLogger.h"

namespace VENet
{
class VE_DLL_EXPORT  PacketOutputWindowLogger : public PacketLogger
{
public:
    PacketOutputWindowLogger();
    virtual ~PacketOutputWindowLogger();
    virtual void WriteLog(const char *str);
protected:
};

}

#endif
