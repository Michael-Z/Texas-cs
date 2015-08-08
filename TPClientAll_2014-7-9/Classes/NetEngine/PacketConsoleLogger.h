////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PacketConsoleLogger.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_LogCommandParser==1 && _VENET_SUPPORT_PacketLogger==1

#pragma once

#include "PacketLogger.h"

namespace VENet
{
class LogCommandParser;

class VE_DLL_EXPORT  PacketConsoleLogger : public PacketLogger
{
public:
    PacketConsoleLogger();
    virtual void SetLogCommandParser(LogCommandParser *lcp);
    virtual void WriteLog(const char *str);
protected:
    LogCommandParser *logCommandParser;
};

}
#endif
