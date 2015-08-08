////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 Rackspace.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VENetTypes.h"
#include "VENetDefines.h"
#include "Export.h"
#include "SocketIncludes.h"

namespace VENet
{

struct VE_DLL_EXPORT VENetSocket
{
	VENetSocket();
	~VENetSocket();
	SOCKET s;
	unsigned int userConnectionSocketIndex;
	SystemAddress boundAddress;
	unsigned short socketFamily;
	bool blockingSocket;

#if defined (_WIN32) && defined(USE_WAIT_FOR_MULTIPLE_EVENTS)
	void* recvEvent;
#endif

	unsigned short remotePortVENetWasStartedOn_PS3_PSP2;

	unsigned int extraSocketOptions;
	_PP_Instance_ chromeInstance;
};

}
