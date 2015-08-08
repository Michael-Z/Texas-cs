////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PacketFileLogger.h
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
#include <stdio.h>

namespace VENet
{

/// \ingroup PACKETLOGGER_GROUP
/// \brief Packetlogger that outputs to a file
class VE_DLL_EXPORT  PacketFileLogger : public PacketLogger
{
public:
	PacketFileLogger();
	virtual ~PacketFileLogger();
	void StartLog(const char *filenamePrefix);
	virtual void WriteLog(const char *str);
protected:
	FILE *packetLogFile;
};

}

#endif
