#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_PacketLogger==1

#pragma once

#include "PacketLogger.h"
#include "SingleProducerConsumer.h"

namespace VENet
{

class VE_DLL_EXPORT ThreadsafePacketLogger : public PacketLogger
{
public:
	ThreadsafePacketLogger();
	virtual ~ThreadsafePacketLogger();

	virtual void Update(void);

protected:
	virtual void AddToLog(const char *str);

	DataStructures::SingleProducerConsumer<char*> logMessages;
};

}

#endif
