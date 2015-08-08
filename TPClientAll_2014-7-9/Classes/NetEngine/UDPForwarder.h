#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_UDPForwarder==1

#pragma once

#include "Export.h"
#include "VENetTypes.h"
#include "SocketIncludes.h"
#include "UDPProxyCommon.h"
#include "SimpleMutex.h"
#include "VEString.h"
#include "VEThread.h"
#include "DS_Queue.h"
#include "DS_OrderedList.h"
#include "LocklessTypes.h"
#include "DS_ThreadsafeAllocatingQueue.h"

namespace VENet
{

enum UDPForwarderResult
{
	UDPFORWARDER_FORWARDING_ALREADY_EXISTS,
	UDPFORWARDER_NO_SOCKETS,
	UDPFORWARDER_BIND_FAILED,
	UDPFORWARDER_INVALID_PARAMETERS,
	UDPFORWARDER_NOT_RUNNING,
	UDPFORWARDER_SUCCESS,
	UDPFORWARDER_RESULT_COUNT
};

class VE_DLL_EXPORT UDPForwarder
{
public:
	UDPForwarder();
	virtual ~UDPForwarder();

	void Startup(void);

	void Shutdown(void);

	void SetMaxForwardEntries(unsigned short maxEntries);

	int GetMaxForwardEntries(void) const;

	int GetUsedForwardEntries(void) const;

	UDPForwarderResult StartForwarding(
		SystemAddress source, SystemAddress destination, VENet::TimeMS timeoutOnNoDataMS,
		const char *forceHostAddress, unsigned short socketFamily,
		unsigned short *forwardingPort, SOCKET *forwardingSocket);

	void StopForwarding(SystemAddress source, SystemAddress destination);


	struct ForwardEntry
	{
		ForwardEntry();
		~ForwardEntry();
		SystemAddress addr1Unconfirmed, addr2Unconfirmed, addr1Confirmed, addr2Confirmed;
		VENet::TimeMS timeLastDatagramForwarded;
		SOCKET socket;
		VENet::TimeMS timeoutOnNoDataMS;
		short socketFamily;
	};


protected:
	friend VE_THREAD_DECLARATION(UpdateUDPForwarderGlobal);

	void UpdateUDPForwarder(void);
	void RecvFrom(VENet::TimeMS curTime, ForwardEntry *forwardEntry);

	struct StartForwardingInputStruct
	{
		SystemAddress source;
		SystemAddress destination;
		VENet::TimeMS timeoutOnNoDataMS;
		VEString forceHostAddress;
		unsigned short socketFamily;
		unsigned int inputId;
	};

	DataStructures::ThreadsafeAllocatingQueue<StartForwardingInputStruct> startForwardingInput;

	struct StartForwardingOutputStruct
	{
		unsigned short forwardingPort;
		SOCKET forwardingSocket;
		UDPForwarderResult result;
		unsigned int inputId;
	};
	DataStructures::Queue<StartForwardingOutputStruct> startForwardingOutput;
	SimpleMutex startForwardingOutputMutex;

	struct StopForwardingStruct
	{
		SystemAddress source;
		SystemAddress destination;
	};
	DataStructures::ThreadsafeAllocatingQueue<StopForwardingStruct> stopForwardingCommands;
	unsigned int nextInputId;

	DataStructures::List<ForwardEntry*> forwardListNotUpdated;

	unsigned short maxForwardEntries;
	VENet::LocklessUint32_t isRunning, threadRunning;

};

}

#endif
