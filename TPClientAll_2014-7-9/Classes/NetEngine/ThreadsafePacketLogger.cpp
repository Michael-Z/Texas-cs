#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_PacketLogger==1

#include "ThreadsafePacketLogger.h"
#include <string.h>

using namespace VENet;

ThreadsafePacketLogger::ThreadsafePacketLogger()
{

}
ThreadsafePacketLogger::~ThreadsafePacketLogger()
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		veFree_Ex((*msg), _FILE_AND_LINE_ );
	}
}
void ThreadsafePacketLogger::Update(void)
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		WriteLog(*msg);
		veFree_Ex((*msg), _FILE_AND_LINE_ );
	}
}
void ThreadsafePacketLogger::AddToLog(const char *str)
{
	char **msg = logMessages.WriteLock();
	*msg = (char*) rakMalloc_Ex( strlen(str)+1, _FILE_AND_LINE_ );
	strcpy(*msg, str);
	logMessages.WriteUnlock();
}

#endif
