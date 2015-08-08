#include "VENetSocket.h"
#include "SocketIncludes.h"
#include "SocketDefines.h"
#include "VEMemoryOverride.h"

using namespace VENet;

#if defined(__native_client__)
using namespace pp;
#endif

VENetSocket::VENetSocket() {
	s = INVALID_SOCKET;

#if defined (_WIN32) && defined(USE_WAIT_FOR_MULTIPLE_EVENTS)
	recvEvent=INVALID_HANDLE_VALUE;
#endif
}
VENetSocket::~VENetSocket() 
{
	if ((SOCKET)s != INVALID_SOCKET)
		closesocket__(s);

#if defined (_WIN32) && defined(USE_WAIT_FOR_MULTIPLE_EVENTS)
	if (recvEvent!=INVALID_HANDLE_VALUE)
	{
		CloseHandle( recvEvent );
		recvEvent = INVALID_HANDLE_VALUE;
	}
#endif
}
