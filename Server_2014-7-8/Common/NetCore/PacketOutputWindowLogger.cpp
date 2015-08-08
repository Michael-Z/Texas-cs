#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_PacketLogger==1

#if defined(UNICODE)
#include "VEWString.h"
#endif

#include "PacketOutputWindowLogger.h"
#include "VEString.h"
#if defined(_WIN32) && !defined(X360__)
#include "WindowsIncludes.h"
#endif

using namespace VENet;

PacketOutputWindowLogger::PacketOutputWindowLogger()
{
}
PacketOutputWindowLogger::~PacketOutputWindowLogger()
{
}
void PacketOutputWindowLogger::WriteLog(const char *str)
{
#if defined(_WIN32) && !defined(X360__)

#if defined(UNICODE)
    VENet::VEWString str2 = str;
    str2+="\n";
    OutputDebugString(str2.C_String());
#else
    VENet::VEString str2 = str;
    str2+="\n";
    OutputDebugString(str2.C_String());
#endif
#elif defined(__native_client__)
    fprintf(stderr, "%s\n", str);
#endif
}

#endif
