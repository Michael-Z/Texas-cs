#pragma once

#include "VEMemoryOverride.h"
#include "SocketIncludes.h"
#include "VENetTypes.h"
#include "VENetSmartPtr.h"
#include "VENetSocket.h"
#include "Export.h"
#include "MTUSize.h"
#include "VEString.h"

namespace VENet
{
class VEPeer;

class VE_DLL_EXPORT SocketLayerOverride
{
public:
    SocketLayerOverride() {}
    virtual ~SocketLayerOverride() {}

    virtual int VENetSendTo( SOCKET s, const char *data, int length, const SystemAddress &systemAddress )=0;

    virtual int VENetRecvFrom( const SOCKET sIn, VEPeer *rakPeerIn, char dataOut[ MAXIMUM_MTU_SIZE ], SystemAddress *senderOut, bool calledFromMainThread )=0;
};


class VE_DLL_EXPORT SocketLayer
{

public:

    SocketLayer();

    ~SocketLayer();

    static SOCKET CreateBoundSocket( VEPeer *peer, unsigned short port, bool blockingSocket, const char *forceHostAddress, unsigned int sleepOn10048, unsigned int extraSocketOptions, unsigned short socketFamily, _PP_Instance_ chromeInstance );
    static SOCKET CreateBoundSocket_Old( VEPeer *peer, unsigned short port, bool blockingSocket, const char *forceHostAddress, unsigned int sleepOn10048, unsigned int extraSocketOptions, _PP_Instance_ chromeInstance );
    static SOCKET CreateBoundSocket_PS3Lobby( unsigned short port, bool blockingSocket, const char *forceHostAddress, unsigned short socketFamily );
    static SOCKET CreateBoundSocket_PSP2( unsigned short port, bool blockingSocket, const char *forceHostAddress, unsigned short socketFamily );

    static bool IsPortInUse_Old(unsigned short port, const char *hostAddress);
    static bool IsPortInUse(unsigned short port, const char *hostAddress, unsigned short socketFamily );
    static bool IsSocketFamilySupported(const char *hostAddress, unsigned short socketFamily);

    static const char* DomainNameToIP_Old( const char *domainName );
    static const char* DomainNameToIP( const char *domainName );

    static void Write( const SOCKET writeSocket, const char* data, const int length );

    static void RecvFromBlocking_Old( const SOCKET s, VEPeer *rakPeer, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, char *dataOut, int *bytesReadOut, SystemAddress *systemAddressOut, VENet::TimeUS *timeRead );
    static void RecvFromBlocking( const SOCKET s, VEPeer *rakPeer, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, char *dataOut, int *bytesReadOut, SystemAddress *systemAddressOut, VENet::TimeUS *timeRead );

    static VENet::VEString GetSubNetForSocketAndIp(SOCKET inSock, VENet::VEString inIpString);


    static void SetNonBlocking( SOCKET listenSocket);

    static void GetMyIP( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] );


    static int SendToTTL( SOCKET s, const char *data, int length, SystemAddress &systemAddress, int ttl );

    static int SendTo( SOCKET s, const char *data, int length, SystemAddress &systemAddress, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, const char *file, const long line );

    static unsigned short GetLocalPort(SOCKET s);
    static void GetSystemAddress_Old ( SOCKET s, SystemAddress *systemAddressOut );
    static void GetSystemAddress ( SOCKET s, SystemAddress *systemAddressOut );

    static void SetSocketLayerOverride(SocketLayerOverride *_slo);
    static SocketLayerOverride* GetSocketLayerOverride(void)
    {
        return slo;
    }

    static int SendTo_PS3Lobby( SOCKET s, const char *data, int length, const SystemAddress &systemAddress, unsigned short remotePortVENetWasStartedOn_PS3 );
    static int SendTo_PSP2( SOCKET s, const char *data, int length, const SystemAddress &systemAddress, unsigned short remotePortVENetWasStartedOn_PS3 );
    static int SendTo_360( SOCKET s, const char *data, int length, const char *voiceData, int voiceLength, const SystemAddress &systemAddress, unsigned int extraSocketOptions );
    static int SendTo_PC( SOCKET s, const char *data, int length, const SystemAddress &systemAddress, const char *file, const long line );

    static void SetDoNotFragment( SOCKET listenSocket, int opt, int IPPROTO );

    static bool GetFirstBindableIP(char firstBindable[128], int ipProto);

private:

    static void SetSocketOptions( SOCKET listenSocket, bool blockingSocket);
    static SocketLayerOverride *slo;
};

}