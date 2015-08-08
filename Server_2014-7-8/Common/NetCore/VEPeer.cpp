#define CAT_NEUTER_EXPORT

#include "VENetDefines.h"
#include "VEPeer.h"
#include "VENetTypes.h"

#ifdef _WIN32

#else
#include <unistd.h>
#endif

#include <time.h>
#include <ctype.h>
#include <string.h>
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "DS_HuffmanEncodingTree.h"
#include "Rand.h"
#include "PluginInterface2.h"
#include "StringCompressor.h"
#include "StringTable.h"
#include "NetworkIDObject.h"
#include "VENetTypes.h"
#include "SHA1.h"
#include "VESleep.h"
#include "VEAssert.h"
#include "VENetVersion.h"
#include "NetworkIDManager.h"
#include "gettimeofday.h"
#include "SignaledEvent.h"
#include "SuperFastHash.h"
#include "VEAlloca.h"
#include "WSAStartupSingleton.h"

#ifdef USE_THREADED_SEND
#include "SendToThread.h"
#endif

#ifdef CAT_AUDIT
#define CAT_AUDIT_PRINTF(...) printf(__VA_ARGS__)
#else
#define CAT_AUDIT_PRINTF(...)
#endif

namespace VENet
{
VE_THREAD_DECLARATION(UpdateNetworkLoop);
VE_THREAD_DECLARATION(RecvFromLoop);
VE_THREAD_DECLARATION(UDTConnect);
}
#define REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE 8

#if !defined ( __APPLE__ ) && !defined ( __APPLE_CC__ )
#include <stdlib.h>
#endif



#if   defined(_WIN32)
//
#else

#endif


static const int NUM_MTU_SIZES=3;



static const int mtuSizes[NUM_MTU_SIZES]={MAXIMUM_MTU_SIZE, 1200, 576};

#ifdef _MSC_VER
#pragma warning( push )
#endif

using namespace VENet;

static VENetRandom rnr;

struct RakPeerAndIndex
{
    SOCKET s;
    unsigned short remotePortVENetWasStartedOn_PS3;
    VEPeer *rakPeer;
    unsigned int extraSocketOptions;
    bool blockingSocket;
};

static const unsigned int MAX_OFFLINE_DATA_LENGTH=400;
#if   !defined(__GNUC__)
#pragma warning(disable:4309)
#endif
static const char OFFLINE_MESSAGE_DATA_ID[16]={0x00,0xFF,0xFF,0x00,0xFE,0xFE,0xFE,0xFE,0xFD,0xFD,0xFD,0xFD,0x12,0x34,0x56,0x78};

struct PacketFollowedByData
{
    Packet p;
    unsigned char data[1];
};

Packet *VEPeer::AllocPacket(unsigned dataSize, const char *file, unsigned int line)
{
    VENet::Packet *p;
    packetAllocationPoolMutex.Lock();
    p = packetAllocationPool.Allocate(file,line);
    packetAllocationPoolMutex.Unlock();
    p = new ((void*)p) Packet;
    p->data=(unsigned char*) rakMalloc_Ex(dataSize,file,line);
    p->length=dataSize;
    p->bitSize=BYTES_TO_BITS(dataSize);
    p->deleteData=true;
    p->guid=UNASSIGNED_VENET_GUID;
    p->wasGeneratedLocally=false;
    return p;
}

Packet *VEPeer::AllocPacket(unsigned dataSize, unsigned char *data, const char *file, unsigned int line)
{
    VENet::Packet *p;
    packetAllocationPoolMutex.Lock();
    p = packetAllocationPool.Allocate(file,line);
    packetAllocationPoolMutex.Unlock();
    p = new ((void*)p) Packet;
    VEAssert(p);
    p->data=data;
    p->length=dataSize;
    p->bitSize=BYTES_TO_BITS(dataSize);
    p->deleteData=true;
    p->guid=UNASSIGNED_VENET_GUID;
    p->wasGeneratedLocally=false;
    return p;
}

STATIC_FACTORY_DEFINITIONS(VEPeerInterface,VEPeer)

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::VEPeer()
{
#if LIBCAT_SECURITY==1
    CAT_AUDIT_PRINTF("AUDIT: Initializing RakPeer security flags: using_security = false, server_handshake = null, cookie_jar = null\n");
    _using_security = false;
    _server_handshake = 0;
    _cookie_jar = 0;
#endif

    StringCompressor::AddReference();
    VENet::StringTable::AddReference();
    WSAStartupSingleton::AddRef();

    defaultMTUSize = mtuSizes[NUM_MTU_SIZES-1];
    trackFrequencyTable = false;
    maximumIncomingConnections = 0;
    maximumNumberOfPeers = 0;
    remoteSystemList = 0;
    activeSystemList = 0;
    activeSystemListSize=0;
    remoteSystemLookup=0;
    bytesSentPerSecond = bytesReceivedPerSecond = 0;
    endThreads = true;
    isMainLoopThreadActive = false;


#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
    occasionalPing = true;
#else
    occasionalPing = false;
#endif
    allowInternalRouting=false;
    for (unsigned int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
        ipList[i]=UNASSIGNED_SYSTEM_ADDRESS;
    allowConnectionResponseIPMigration = false;
    incomingPasswordLength=0;
    splitMessageProgressInterval=0;
    unreliableTimeout=1000;
    maxOutgoingBPS=0;
    firstExternalID=UNASSIGNED_SYSTEM_ADDRESS;
    myGuid=UNASSIGNED_VENET_GUID;
    userUpdateThreadPtr=0;
    userUpdateThreadData=0;

#ifdef _DEBUG
    defaultTimeoutTime=30000;
#else
    defaultTimeoutTime=10000;
#endif

#ifdef _DEBUG
    _packetloss=0.0;
    _minExtraPing=0;
    _extraPingVariance=0;
#endif

    bufferedCommands.SetPageSize(sizeof(BufferedCommandStruct)*16);
    socketQueryOutput.SetPageSize(sizeof(SocketQueryOutput)*8);
    bufferedPackets.SetPageSize(sizeof(RecvFromStruct)*BUFFERED_PACKETS_PAGE_SIZE); // This is big, keep the size down

    packetAllocationPoolMutex.Lock();
    packetAllocationPool.SetPageSize(sizeof(DataStructures::MemoryPool<Packet>::MemoryWithPage)*32);
    packetAllocationPoolMutex.Unlock();

    remoteSystemIndexPool.SetPageSize(sizeof(DataStructures::MemoryPool<RemoteSystemIndex>::MemoryWithPage)*32);

    GenerateGUID();

    quitAndDataEvents.InitEvent();
    limitConnectionFrequencyFromTheSameIP=false;
    ResetSendReceipt();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::~VEPeer()
{
    Shutdown( 0, 0 );

    // Free the ban list.
    ClearBanList();

    StringCompressor::RemoveReference();
    VENet::StringTable::RemoveReference();
    WSAStartupSingleton::Deref();

    quitAndDataEvents.CloseEvent();

#if LIBCAT_SECURITY==1
    // Encryption and security
    CAT_AUDIT_PRINTF("AUDIT: Deleting RakPeer security objects, handshake = %x, cookie jar = %x\n", _server_handshake, _cookie_jar);
    if (_server_handshake) VENet::OP_DELETE(_server_handshake,_FILE_AND_LINE_);
    if (_cookie_jar) VENet::OP_DELETE(_cookie_jar,_FILE_AND_LINE_);
#endif

}

StartupResult VEPeer::Startup( unsigned short maxConnections, SocketDescriptor *socketDescriptors, unsigned socketDescriptorCount, int threadPriority )
{
    if (IsActive())
        return VENET_ALREADY_STARTED;

    if (threadPriority==-99999)
    {
#if   defined(_WIN32)
        threadPriority=0;
#else
        threadPriority=1000;
#endif
    }
    unsigned int i;

    SocketLayer::GetMyIP( ipList );


    if (myGuid==UNASSIGNED_VENET_GUID)
    {
        rnr.SeedMT( GenerateSeedFromGuid() );
    }

    VEAssert(socketDescriptors && socketDescriptorCount>=1);

    if (socketDescriptors==0 || socketDescriptorCount<1)
        return INVALID_SOCKET_DESCRIPTORS;

    VEAssert( maxConnections > 0 );

    if ( maxConnections <= 0 )
        return INVALID_MAX_CONNECTIONS;

    DerefAllSockets();


    for (i=0; i<socketDescriptorCount; i++)
    {
        const char *addrToBind;
        if (socketDescriptors[i].hostAddress[0]==0)
            addrToBind=0;
        else
            addrToBind=socketDescriptors[i].hostAddress;

#if VENET_SUPPORT_IPV6==1
        if (SocketLayer::IsSocketFamilySupported(addrToBind, socketDescriptors[i].socketFamily)==false)
            return SOCKET_FAMILY_NOT_SUPPORTED;
#endif

        if (socketDescriptors[i].port!=0 && SocketLayer::IsPortInUse(socketDescriptors[i].port, addrToBind, socketDescriptors[i].socketFamily)==true)
        {
            DerefAllSockets();
            return SOCKET_PORT_ALREADY_IN_USE;
        }

        VENetSmartPtr<VENetSocket> rns(VENet::OP_NEW<VENetSocket>(_FILE_AND_LINE_));
        if (socketDescriptors[i].remotePortVENetWasStartedOn_PS3_PSP2==0)
        {
            rns->s = (SOCKET) SocketLayer::CreateBoundSocket( this, socketDescriptors[i].port, socketDescriptors[i].blockingSocket, addrToBind, 100, socketDescriptors[i].extraSocketOptions, socketDescriptors[i].socketFamily, socketDescriptors[i].chromeInstance );
        }
        else
        {

        }

        if (rns->s==INVALID_SOCKET)
        {
            DerefAllSockets();
            return SOCKET_FAILED_TO_BIND;
        }

        SocketLayer::GetSystemAddress( rns->s, &rns->boundAddress );

        rns->remotePortVENetWasStartedOn_PS3_PSP2=socketDescriptors[i].remotePortVENetWasStartedOn_PS3_PSP2;
        rns->chromeInstance=socketDescriptors[i].chromeInstance;
        rns->extraSocketOptions=socketDescriptors[i].extraSocketOptions;
        rns->userConnectionSocketIndex=i;
        rns->blockingSocket=socketDescriptors[i].blockingSocket;

#if VENET_SUPPORT_IPV6==0
        if (addrToBind==0)
            rns->boundAddress.SetToLoopback(4);
#endif

#if !defined(__native_client__)
        int zero=0;
        if (SocketLayer::SendTo((SOCKET)rns->s, (const char*) &zero,4,rns->boundAddress, rns->remotePortVENetWasStartedOn_PS3_PSP2, rns->extraSocketOptions, _FILE_AND_LINE_)!=0)
        {
            DerefAllSockets();
            return SOCKET_FAILED_TEST_SEND;
        }
#endif

        socketList.Push(rns, _FILE_AND_LINE_ );

    }


    for (i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
    {
        if (ipList[i]==UNASSIGNED_SYSTEM_ADDRESS)
            break;
        ipList[i].SetPort(socketList[0]->boundAddress.GetPort());
    }



    if ( maximumNumberOfPeers == 0 )
    {
        if ( maximumIncomingConnections > maxConnections )
            maximumIncomingConnections = maxConnections;

        maximumNumberOfPeers = maxConnections;
        remoteSystemList = VENet::OP_NEW_ARRAY<RemoteSystemStruct>(maximumNumberOfPeers, _FILE_AND_LINE_ );

        remoteSystemLookup = VENet::OP_NEW_ARRAY<RemoteSystemIndex*>((unsigned int) maximumNumberOfPeers * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE, _FILE_AND_LINE_ );

        activeSystemList = VENet::OP_NEW_ARRAY<RemoteSystemStruct*>(maximumNumberOfPeers, _FILE_AND_LINE_ );

        for ( i = 0; i < maximumNumberOfPeers; i++ )
        {
            remoteSystemList[ i ].isActive = false;
            remoteSystemList[ i ].systemAddress = UNASSIGNED_SYSTEM_ADDRESS;
            remoteSystemList[ i ].guid = UNASSIGNED_VENET_GUID;
            remoteSystemList[ i ].myExternalSystemAddress = UNASSIGNED_SYSTEM_ADDRESS;
            remoteSystemList[ i ].connectMode=RemoteSystemStruct::NO_ACTION;
            remoteSystemList[ i ].MTUSize = defaultMTUSize;
            remoteSystemList[ i ].remoteSystemIndex = (SystemIndex) i;
#ifdef _DEBUG
            remoteSystemList[ i ].reliabilityLayer.ApplyNetworkSimulator(_packetloss, _minExtraPing, _extraPingVariance);
#endif

            activeSystemList[ i ] = &remoteSystemList[ i ];
        }

        for (unsigned int i=0; i < (unsigned int) maximumNumberOfPeers*REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE; i++)
        {
            remoteSystemLookup[i]=0;
        }
    }

    if ( endThreads )
    {
        updateCycleIsRunning = false;
        endThreads = false;
        firstExternalID=UNASSIGNED_SYSTEM_ADDRESS;

        ClearBufferedCommands();
        ClearBufferedPackets();
        ClearSocketQueryOutput();

        if ( isMainLoopThreadActive == false )
        {
#if VEPEER_USER_THREADED!=1

            int errorCode;
            errorCode = VENet::VEThread::Create(UpdateNetworkLoop, this, threadPriority);

            if ( errorCode != 0 )
            {
                Shutdown( 0, 0 );
                return FAILED_TO_CREATE_NETWORK_THREAD;
            }
            VEAssert(isRecvFromLoopThreadActive.GetValue()==0);
#endif

            for (i=0; i<socketDescriptorCount; i++)
            {
                RakPeerAndIndex *rpai = VENet::OP_NEW<RakPeerAndIndex>(_FILE_AND_LINE_);
                rpai->remotePortVENetWasStartedOn_PS3=socketDescriptors[i].remotePortVENetWasStartedOn_PS3_PSP2;
                rpai->extraSocketOptions=socketDescriptors[i].extraSocketOptions;
                rpai->s=socketList[i]->s;
                rpai->rakPeer=this;
                rpai->blockingSocket=socketDescriptors[i].blockingSocket;

#if VEPEER_USER_THREADED!=1

                errorCode = VENet::VEThread::Create(RecvFromLoop, rpai, threadPriority);

                if ( errorCode != 0 )
                {
                    Shutdown( 0, 0 );
                    return FAILED_TO_CREATE_NETWORK_THREAD;
                }
#endif
            }


#if VEPEER_USER_THREADED!=1

            while (  isRecvFromLoopThreadActive.GetValue() < (uint32_t) socketDescriptorCount )
                VESleep(10);
#endif
        }

#if VEPEER_USER_THREADED!=1
        while (  isMainLoopThreadActive == false )
            VESleep(10);
#endif
    }

    for (i=0; i < pluginListTS.Size(); i++)
    {
        pluginListTS[i]->OnRakPeerStartup();
    }

    for (i=0; i < pluginListNTS.Size(); i++)
    {
        pluginListNTS[i]->OnRakPeerStartup();
    }

#ifdef USE_THREADED_SEND
    VENet::SendToThread::AddRef();
#endif

    return VENET_STARTED;
}

bool VEPeer::InitializeSecurity(const char *public_key, const char *private_key, bool bRequireClientKey)
{
#if LIBCAT_SECURITY==1
    if ( endThreads == false )
        return false;

    // Copy client public key requirement flag
    _require_client_public_key = bRequireClientKey;

    if (_server_handshake)
    {
        CAT_AUDIT_PRINTF("AUDIT: Deleting old server_handshake %x\n", _server_handshake);
        VENet::OP_DELETE(_server_handshake,_FILE_AND_LINE_);
    }
    if (_cookie_jar)
    {
        CAT_AUDIT_PRINTF("AUDIT: Deleting old cookie jar %x\n", _cookie_jar);
        VENet::OP_DELETE(_cookie_jar,_FILE_AND_LINE_);
    }

    _server_handshake = VENet::OP_NEW<cat::ServerEasyHandshake>(_FILE_AND_LINE_);
    _cookie_jar = VENet::OP_NEW<cat::CookieJar>(_FILE_AND_LINE_);

    CAT_AUDIT_PRINTF("AUDIT: Created new server_handshake %x\n", _server_handshake);
    CAT_AUDIT_PRINTF("AUDIT: Created new cookie jar %x\n", _cookie_jar);
    CAT_AUDIT_PRINTF("AUDIT: Running _server_handshake->Initialize()\n");

    if (_server_handshake->Initialize(public_key, private_key))
    {
        CAT_AUDIT_PRINTF("AUDIT: Successfully initialized, filling cookie jar with goodies, storing public key and setting using security flag to true\n");

        _server_handshake->FillCookieJar(_cookie_jar);

        memcpy(my_public_key, public_key, sizeof(my_public_key));

        _using_security = true;
        return true;
    }

    CAT_AUDIT_PRINTF("AUDIT: Failure to initialize so deleting server handshake and cookie jar; also setting using_security flag = false\n");

    VENet::OP_DELETE(_server_handshake,_FILE_AND_LINE_);
    _server_handshake=0;
    VENet::OP_DELETE(_cookie_jar,_FILE_AND_LINE_);
    _cookie_jar=0;
    _using_security = false;
    return false;
#else
    (void) public_key;
    (void) private_key;
    (void) bRequireClientKey;

    return false;
#endif
}

void VEPeer::DisableSecurity( void )
{
#if LIBCAT_SECURITY==1
    CAT_AUDIT_PRINTF("AUDIT: DisableSecurity() called, so deleting _server_handshake %x and cookie_jar %x\n", _server_handshake, _cookie_jar);
    VENet::OP_DELETE(_server_handshake,_FILE_AND_LINE_);
    _server_handshake=0;
    VENet::OP_DELETE(_cookie_jar,_FILE_AND_LINE_);
    _cookie_jar=0;

    _using_security = false;
#endif
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::AddToSecurityExceptionList(const char *ip)
{
    securityExceptionMutex.Lock();
    securityExceptionList.Insert(VEString(ip), _FILE_AND_LINE_);
    securityExceptionMutex.Unlock();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::RemoveFromSecurityExceptionList(const char *ip)
{
    if (securityExceptionList.Size()==0)
        return;

    if (ip==0)
    {
        securityExceptionMutex.Lock();
        securityExceptionList.Clear(false, _FILE_AND_LINE_);
        securityExceptionMutex.Unlock();
    }
    else
    {
        unsigned i=0;
        securityExceptionMutex.Lock();
        while (i < securityExceptionList.Size())
        {
            if (securityExceptionList[i].IPAddressMatch(ip))
            {
                securityExceptionList[i]=securityExceptionList[securityExceptionList.Size()-1];
                securityExceptionList.RemoveAtIndex(securityExceptionList.Size()-1);
            }
            else
                i++;
        }
        securityExceptionMutex.Unlock();
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::IsInSecurityExceptionList(const char *ip)
{
    if (securityExceptionList.Size()==0)
        return false;

    unsigned i=0;
    securityExceptionMutex.Lock();
    for (; i < securityExceptionList.Size(); i++)
    {
        if (securityExceptionList[i].IPAddressMatch(ip))
        {
            securityExceptionMutex.Unlock();
            return true;
        }
    }
    securityExceptionMutex.Unlock();
    return false;
}


void VEPeer::SetMaximumIncomingConnections( unsigned short numberAllowed )
{
    maximumIncomingConnections = numberAllowed;
}


unsigned short VEPeer::GetMaximumIncomingConnections( void ) const
{
    return maximumIncomingConnections;
}


unsigned short VEPeer::NumberOfConnections(void) const
{
    DataStructures::List<SystemAddress> addresses;
    DataStructures::List<VENetGUID> guids;
    GetSystemList(addresses, guids);
    return (unsigned short) addresses.Size();
}

void VEPeer::SetIncomingPassword( const char* passwordData, int passwordDataLength )
{
    if (passwordDataLength > 255)
        passwordDataLength=255;

    if (passwordData==0)
        passwordDataLength=0;

    if (passwordDataLength>0)
        memcpy(incomingPassword, passwordData, passwordDataLength);
    incomingPasswordLength=(unsigned char)passwordDataLength;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::GetIncomingPassword( char* passwordData, int *passwordDataLength  )
{
    if (passwordData==0)
    {
        *passwordDataLength=incomingPasswordLength;
        return;
    }

    if (*passwordDataLength > incomingPasswordLength)
        *passwordDataLength=incomingPasswordLength;

    if (*passwordDataLength>0)
        memcpy(passwordData, incomingPassword, *passwordDataLength);
}

ConnectionAttemptResult VEPeer::Connect( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, VENet::TimeMS timeoutTime )
{
    if ( host == 0 || endThreads || connectionSocketIndex>=socketList.Size() )
        return INVALID_PARAMETER;

    VEAssert(remotePort!=0);

    connectionSocketIndex=GetVENetSocketFromUserConnectionSocketIndex(connectionSocketIndex);

    if (passwordDataLength>255)
        passwordDataLength=255;

    if (passwordData==0)
        passwordDataLength=0;

    return SendConnectionRequest( host, remotePort, passwordData, passwordDataLength, publicKey, connectionSocketIndex, 0, sendConnectionAttemptCount, timeBetweenSendConnectionAttemptsMS, timeoutTime);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ConnectionAttemptResult VEPeer::ConnectWithSocket(const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, VENetSmartPtr<VENetSocket> socket, PublicKey *publicKey, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, VENet::TimeMS timeoutTime)
{
    if ( host == 0 || endThreads || socket.IsNull() )
        return INVALID_PARAMETER;

    if (passwordDataLength>255)
        passwordDataLength=255;

    if (passwordData==0)
        passwordDataLength=0;

    return SendConnectionRequest( host, remotePort, passwordData, passwordDataLength, publicKey, 0, 0, sendConnectionAttemptCount, timeBetweenSendConnectionAttemptsMS, timeoutTime, socket );

}


void VEPeer::Shutdown( unsigned int blockDuration, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority )
{
    unsigned i,j;
    bool anyActive;
    VENet::TimeMS startWaitingTime;
    VENet::TimeMS time;
    unsigned short systemListSize = maximumNumberOfPeers;

    if ( blockDuration > 0 )
    {
        for ( i = 0; i < systemListSize; i++ )
        {
            if (remoteSystemList[i].isActive)
                NotifyAndFlagForShutdown(remoteSystemList[i].systemAddress, false, orderingChannel, disconnectionNotificationPriority);
        }

        time = VENet::GetTimeMS();
        startWaitingTime = time;
        while ( time - startWaitingTime < blockDuration )
        {
            anyActive=false;
            for (j=0; j < systemListSize; j++)
            {
                if (remoteSystemList[j].isActive)
                {
                    anyActive=true;
                    break;
                }
            }

            if ( anyActive==false )
                break;

            VESleep(15);
            time = VENet::GetTimeMS();
        }
    }
    for (i=0; i < pluginListTS.Size(); i++)
    {
        pluginListTS[i]->OnVEPeerShutdown();
    }
    for (i=0; i < pluginListNTS.Size(); i++)
    {
        pluginListNTS[i]->OnVEPeerShutdown();
    }

    activeSystemListSize=0;

    quitAndDataEvents.SetEvent();

    endThreads = true;

    VENet::TimeMS timeout;
#if VEPEER_USER_THREADED!=1

    for (i=0; i < socketList.Size(); i++)
    {
        SystemAddress sa2 = socketList[i]->boundAddress;
        if (SocketLayer::SendTo(socketList[i]->s, (const char*) &i,1,sa2, socketList[i]->remotePortVENetWasStartedOn_PS3_PSP2, socketList[i]->extraSocketOptions, _FILE_AND_LINE_)!=0)
            break;
    }
    while ( isMainLoopThreadActive )
    {
        endThreads = true;
        VESleep(15);
    }

    timeout = VENet::GetTimeMS()+1000;
    while ( isRecvFromLoopThreadActive.GetValue()>0 && VENet::GetTimeMS()<timeout )
    {
        for (i=0; i < socketList.Size(); i++)
        {
            SystemAddress sa2 = socketList[i]->boundAddress;
            SocketLayer::SendTo(socketList[i]->s, (const char*) &i,1,sa2, socketList[i]->remotePortVENetWasStartedOn_PS3_PSP2, socketList[i]->extraSocketOptions, _FILE_AND_LINE_);
        }

        VESleep(30);
    }


#endif
    for ( i = 0; i < systemListSize; i++ )
    {
        remoteSystemList[ i ].isActive = false;

        remoteSystemList[ i ].reliabilityLayer.Reset(false, remoteSystemList[ i ].MTUSize, false);
        remoteSystemList[ i ].VENetSocket.SetNull();

    }

    maximumNumberOfPeers = 0;
    packetReturnMutex.Lock();
    for (i=0; i < packetReturnQueue.Size(); i++)
        DeallocatePacket(packetReturnQueue[i]);
    packetReturnQueue.Clear(_FILE_AND_LINE_);
    packetReturnMutex.Unlock();
    packetAllocationPoolMutex.Lock();
    packetAllocationPool.Clear(_FILE_AND_LINE_);
    packetAllocationPoolMutex.Unlock();

    if (isRecvFromLoopThreadActive.GetValue()>0)
    {
        timeout = VENet::GetTimeMS()+1000;
        while ( isRecvFromLoopThreadActive.GetValue()>0 && VENet::GetTimeMS()<timeout )
        {
            VESleep(30);
        }
    }

    DerefAllSockets();

    ClearBufferedCommands();
    ClearBufferedPackets();
    ClearSocketQueryOutput();
    bytesSentPerSecond = bytesReceivedPerSecond = 0;

    ClearRequestedConnectionList();


    RemoteSystemStruct * temp = remoteSystemList;
    remoteSystemList = 0;
    VENet::OP_DELETE_ARRAY(temp, _FILE_AND_LINE_);
    VENet::OP_DELETE_ARRAY(activeSystemList, _FILE_AND_LINE_);
    activeSystemList=0;

    ClearRemoteSystemLookup();

#ifdef USE_THREADED_SEND
    VENet::SendToThread::Deref();
#endif

    ResetSendReceipt();
}


inline bool VEPeer::IsActive( void ) const
{
    return endThreads == false;
}

bool VEPeer::GetConnectionList( SystemAddress *remoteSystems, unsigned short *numberOfSystems ) const
{
    if (numberOfSystems==0)
        return false;

    if ( remoteSystemList == 0 || endThreads == true )
    {
        if (numberOfSystems)
            *numberOfSystems=0;
        return false;
    }

    DataStructures::List<SystemAddress> addresses;
    DataStructures::List<VENetGUID> guids;
    GetSystemList(addresses, guids);
    if (remoteSystems)
    {
        unsigned short i;
        for (i=0; i < *numberOfSystems && i < addresses.Size(); i++)
            remoteSystems[i]=addresses[i];
        *numberOfSystems=i;
    }
    else
    {
        *numberOfSystems=(unsigned short) addresses.Size();
    }
    return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t VEPeer::GetNextSendReceipt(void)
{
    sendReceiptSerialMutex.Lock();
    uint32_t retVal = sendReceiptSerial;
    sendReceiptSerialMutex.Unlock();
    return retVal;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t VEPeer::IncrementNextSendReceipt(void)
{
    sendReceiptSerialMutex.Lock();
    uint32_t returned = sendReceiptSerial;
    if (++sendReceiptSerial==0)
        sendReceiptSerial=1;
    sendReceiptSerialMutex.Unlock();
    return returned;
}

uint32_t VEPeer::Send( const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber )
{
#ifdef _DEBUG
    VEAssert( data && length > 0 );
#endif
    VEAssert( !( reliability >= NUMBER_OF_RELIABILITIES || reliability < 0 ) );
    VEAssert( !( priority > NUMBER_OF_PRIORITIES || priority < 0 ) );
    VEAssert( !( orderingChannel >= NUMBER_OF_ORDERED_STREAMS ) );

    if ( data == 0 || length < 0 )
        return 0;

    if ( remoteSystemList == 0 || endThreads == true )
        return 0;

    if ( broadcast == false && systemIdentifier.IsUndefined())
        return 0;

    uint32_t usedSendReceipt;
    if (forceReceiptNumber!=0)
        usedSendReceipt=forceReceiptNumber;
    else
        usedSendReceipt=IncrementNextSendReceipt();

    if (broadcast==false && IsLoopbackAddress(systemIdentifier,true))
    {
        SendLoopback(data,length);

        if (reliability>=UNRELIABLE_WITH_ACK_RECEIPT)
        {
            char buff[5];
            buff[0]=ID_SND_RECEIPT_ACKED;
            sendReceiptSerialMutex.Lock();
            memcpy(buff+1, &sendReceiptSerial, 4);
            sendReceiptSerialMutex.Unlock();
            SendLoopback( buff, 5 );
        }

        return usedSendReceipt;
    }

    SendBuffered(data, length*8, priority, reliability, orderingChannel, systemIdentifier, broadcast, RemoteSystemStruct::NO_ACTION, usedSendReceipt);

    return usedSendReceipt;
}

void VEPeer::SendLoopback( const char *data, const int length )
{
    if ( data == 0 || length < 0 )
        return;

    Packet *packet = AllocPacket(length, _FILE_AND_LINE_);
    memcpy(packet->data, data, length);
    packet->systemAddress = GetLoopbackAddress();
    packet->guid=myGuid;
    PushBackPacket(packet, false);
}

uint32_t VEPeer::Send( const VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber )
{
#ifdef _DEBUG
    VEAssert( bitStream->GetNumberOfBytesUsed() > 0 );
#endif

    VEAssert( !( reliability >= NUMBER_OF_RELIABILITIES || reliability < 0 ) );
    VEAssert( !( priority > NUMBER_OF_PRIORITIES || priority < 0 ) );
    VEAssert( !( orderingChannel >= NUMBER_OF_ORDERED_STREAMS ) );

    if ( bitStream->GetNumberOfBytesUsed() == 0 )
        return 0;

    if ( remoteSystemList == 0 || endThreads == true )
        return 0;

    if ( broadcast == false && systemIdentifier.IsUndefined() )
        return 0;

    uint32_t usedSendReceipt;
    if (forceReceiptNumber!=0)
        usedSendReceipt=forceReceiptNumber;
    else
        usedSendReceipt=IncrementNextSendReceipt();

    if (broadcast==false && IsLoopbackAddress(systemIdentifier,true))
    {
        SendLoopback((const char*) bitStream->GetData(),bitStream->GetNumberOfBytesUsed());
        if (reliability>=UNRELIABLE_WITH_ACK_RECEIPT)
        {
            char buff[5];
            buff[0]=ID_SND_RECEIPT_ACKED;
            sendReceiptSerialMutex.Lock();
            memcpy(buff+1, &sendReceiptSerial,4);
            sendReceiptSerialMutex.Unlock();
            SendLoopback( buff, 5 );
        }
        return usedSendReceipt;
    }

    SendBuffered((const char*)bitStream->GetData(), bitStream->GetNumberOfBitsUsed(), priority, reliability, orderingChannel, systemIdentifier, broadcast, RemoteSystemStruct::NO_ACTION, usedSendReceipt);


    return usedSendReceipt;
}

uint32_t VEPeer::SendList( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber )
{
#ifdef _DEBUG
    VEAssert( data );
#endif

    if ( data == 0 || lengths == 0 )
        return 0;

    if ( remoteSystemList == 0 || endThreads == true )
        return 0;

    if (numParameters==0)
        return 0;

    if (lengths==0)
        return 0;

    if ( broadcast == false && systemIdentifier.IsUndefined() )
        return 0;

    uint32_t usedSendReceipt;
    if (forceReceiptNumber!=0)
        usedSendReceipt=forceReceiptNumber;
    else
        usedSendReceipt=IncrementNextSendReceipt();

    SendBufferedList(data, lengths, numParameters, priority, reliability, orderingChannel, systemIdentifier, broadcast, RemoteSystemStruct::NO_ACTION, usedSendReceipt);

    return usedSendReceipt;
}

#ifdef _MSC_VER
#pragma warning( disable : 4701 )
#endif
Packet* VEPeer::Receive( void )
{
    if ( !( IsActive() ) )
        return 0;

    VENet::Packet *packet;
    PluginReceiveResult pluginResult;

    int offset;
    unsigned int i;

    for (i=0; i < pluginListTS.Size(); i++)
    {
        pluginListTS[i]->Update();
    }
    for (i=0; i < pluginListNTS.Size(); i++)
    {
        pluginListNTS[i]->Update();
    }

    do
    {
        packetReturnMutex.Lock();
        if (packetReturnQueue.IsEmpty())
            packet=0;
        else
            packet = packetReturnQueue.Pop();
        packetReturnMutex.Unlock();
        if (packet==0)
            return 0;

        if ( ( packet->length >= sizeof(unsigned char) + sizeof( VENet::Time ) ) &&
                ( (unsigned char) packet->data[ 0 ] == ID_TIMESTAMP ) )
        {
            offset = sizeof(unsigned char);
            ShiftIncomingTimestamp( packet->data + offset, packet->systemAddress );
        }

        CallPluginCallbacks(pluginListTS, packet);
        CallPluginCallbacks(pluginListNTS, packet);

        for (i=0; i < pluginListTS.Size(); i++)
        {
            pluginResult=pluginListTS[i]->OnReceive(packet);
            if (pluginResult==RR_STOP_PROCESSING_AND_DEALLOCATE)
            {
                DeallocatePacket( packet );
                packet=0;
                break;
            }
            else if (pluginResult==RR_STOP_PROCESSING)
            {
                packet=0;
                break;
            }
        }

        for (i=0; i < pluginListNTS.Size(); i++)
        {
            pluginResult=pluginListNTS[i]->OnReceive(packet);
            if (pluginResult==RR_STOP_PROCESSING_AND_DEALLOCATE)
            {
                DeallocatePacket( packet );
                packet=0;
                break;
            }
            else if (pluginResult==RR_STOP_PROCESSING)
            {
                packet=0;
                break;
            }
        }

    }
    while (packet==0);

#ifdef _DEBUG
    VEAssert( packet->data );
#endif

    return packet;
}

void VEPeer::DeallocatePacket( Packet *packet )
{
    if ( packet == 0 )
        return;

    if (packet->deleteData)
    {
        veFree_Ex(packet->data, _FILE_AND_LINE_ );
        packet->~Packet();
        packetAllocationPoolMutex.Lock();
        packetAllocationPool.Release(packet,_FILE_AND_LINE_);
        packetAllocationPoolMutex.Unlock();
    }
    else
    {
        veFree_Ex(packet, _FILE_AND_LINE_ );
    }
}

unsigned short VEPeer::GetMaximumNumberOfPeers( void ) const
{
    return maximumNumberOfPeers;
}

void VEPeer::CloseConnection( const AddressOrGUID target, bool sendDisconnectionNotification, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority )
{
    CloseConnectionInternal(target, sendDisconnectionNotification, false, orderingChannel, disconnectionNotificationPriority);

    if (sendDisconnectionNotification==false && GetConnectionState(target)==IS_CONNECTED)
    {
        Packet *packet=AllocPacket(sizeof( char ), _FILE_AND_LINE_);
        packet->data[ 0 ] = ID_CONNECTION_LOST;
        packet->guid = target.veNetGuid==UNASSIGNED_VENET_GUID ? GetGuidFromSystemAddress(target.systemAddress) : target.veNetGuid;
        packet->systemAddress = target.systemAddress==UNASSIGNED_SYSTEM_ADDRESS ? GetSystemAddressFromGuid(target.veNetGuid) : target.systemAddress;
        packet->systemAddress.systemIndex = (SystemIndex) GetIndexFromSystemAddress(packet->systemAddress);
        packet->guid.systemIndex=packet->systemAddress.systemIndex;
        packet->wasGeneratedLocally=true;
        AddPacketToProducer(packet);
    }
}

void VEPeer::CancelConnectionAttempt( const SystemAddress target )
{
    unsigned int i;

    i=0;
    requestedConnectionQueueMutex.Lock();
    while (i < requestedConnectionQueue.Size())
    {
        if (requestedConnectionQueue[i]->systemAddress==target)
        {
#if LIBCAT_SECURITY==1
            CAT_AUDIT_PRINTF("AUDIT: Deleting requestedConnectionQueue %i client_handshake %x\n", i, requestedConnectionQueue[ i ]->client_handshake);
            VENet::OP_DELETE(requestedConnectionQueue[i]->client_handshake, _FILE_AND_LINE_ );
#endif
            VENet::OP_DELETE(requestedConnectionQueue[i], _FILE_AND_LINE_ );
            requestedConnectionQueue.RemoveAtIndex(i);
            break;
        }
        else
            i++;
    }
    requestedConnectionQueueMutex.Unlock();

}


ConnectionState VEPeer::GetConnectionState(const AddressOrGUID systemIdentifier)
{
    if (systemIdentifier.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
    {
        unsigned int i=0;
        requestedConnectionQueueMutex.Lock();
        for (; i < requestedConnectionQueue.Size(); i++)
        {
            if (requestedConnectionQueue[i]->systemAddress==systemIdentifier.systemAddress)
            {
                requestedConnectionQueueMutex.Unlock();
                return IS_PENDING;
            }
        }
        requestedConnectionQueueMutex.Unlock();
    }

    int index;
    if (systemIdentifier.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
    {
        index = GetIndexFromSystemAddress(systemIdentifier.systemAddress, false);
    }
    else
    {
        index = GetIndexFromGuid(systemIdentifier.veNetGuid);
    }

    if (index==-1)
        return IS_NOT_CONNECTED;

    if (remoteSystemList[index].isActive==false)
        return IS_DISCONNECTED;

    switch (remoteSystemList[index].connectMode)
    {
    case RemoteSystemStruct::DISCONNECT_ASAP:
        return IS_DISCONNECTING;
    case RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY:
        return IS_SILENTLY_DISCONNECTING;
    case RemoteSystemStruct::DISCONNECT_ON_NO_ACK:
        return IS_DISCONNECTING;
    case RemoteSystemStruct::REQUESTED_CONNECTION:
        return IS_CONNECTING;
    case RemoteSystemStruct::HANDLING_CONNECTION_REQUEST:
        return IS_CONNECTING;
    case RemoteSystemStruct::UNVERIFIED_SENDER:
        return IS_CONNECTING;
    case RemoteSystemStruct::CONNECTED:
        return IS_CONNECTED;
    default:
        return IS_NOT_CONNECTED;
    }

    return IS_NOT_CONNECTED;
}


int VEPeer::GetIndexFromSystemAddress( const SystemAddress systemAddress ) const
{
    return GetIndexFromSystemAddress(systemAddress, false);
}

SystemAddress VEPeer::GetSystemAddressFromIndex( int index )
{
    if ( index >= 0 && index < maximumNumberOfPeers )
        if (remoteSystemList[index].isActive && remoteSystemList[ index ].connectMode==VEPeer::RemoteSystemStruct::CONNECTED) // Don't give the user players that aren't fully connected, since sends will fail
            return remoteSystemList[ index ].systemAddress;

    return UNASSIGNED_SYSTEM_ADDRESS;
}

VENetGUID VEPeer::GetGUIDFromIndex( int index )
{
    if ( index >= 0 && index < maximumNumberOfPeers )
        if (remoteSystemList[index].isActive && remoteSystemList[ index ].connectMode==VEPeer::RemoteSystemStruct::CONNECTED) // Don't give the user players that aren't fully connected, since sends will fail
            return remoteSystemList[ index ].guid;

    return UNASSIGNED_VENET_GUID;
}

void VEPeer::GetSystemList(DataStructures::List<SystemAddress> &addresses, DataStructures::List<VENetGUID> &guids) const
{
    addresses.Clear(false, _FILE_AND_LINE_);
    guids.Clear(false, _FILE_AND_LINE_);

    if ( remoteSystemList == 0 || endThreads == true )
        return;

    unsigned int i;
    for (i=0; i < activeSystemListSize; i++)
    {
        if ((activeSystemList[i])->isActive &&
                (activeSystemList[i])->connectMode==VEPeer::RemoteSystemStruct::CONNECTED)
        {
            addresses.Push((activeSystemList[i])->systemAddress, _FILE_AND_LINE_ );
            guids.Push((activeSystemList[i])->guid, _FILE_AND_LINE_ );
        }
    }
}

void VEPeer::AddToBanList( const char *IP, VENet::TimeMS milliseconds )
{
    unsigned index;
    VENet::TimeMS time = VENet::GetTimeMS();

    if ( IP == 0 || IP[ 0 ] == 0 || strlen( IP ) > 15 )
        return ;

    index = 0;

    banListMutex.Lock();

    for ( ; index < banList.Size(); index++ )
    {
        if ( strcmp( IP, banList[ index ]->IP ) == 0 )
        {
            if (milliseconds==0)
                banList[ index ]->timeout=0;
            else
                banList[ index ]->timeout=time+milliseconds;
            banListMutex.Unlock();
            return;
        }
    }

    banListMutex.Unlock();

    BanStruct *banStruct = VENet::OP_NEW<BanStruct>( _FILE_AND_LINE_ );
    banStruct->IP = (char*) rakMalloc_Ex( 16, _FILE_AND_LINE_ );
    if (milliseconds==0)
        banStruct->timeout=0; // Infinite
    else
        banStruct->timeout=time+milliseconds;
    strcpy( banStruct->IP, IP );
    banListMutex.Lock();
    banList.Insert( banStruct, _FILE_AND_LINE_ );
    banListMutex.Unlock();
}

void VEPeer::RemoveFromBanList( const char *IP )
{
    unsigned index;
    BanStruct *temp;

    if ( IP == 0 || IP[ 0 ] == 0 || strlen( IP ) > 15 )
        return ;

    index = 0;
    temp=0;

    banListMutex.Lock();

    for ( ; index < banList.Size(); index++ )
    {
        if ( strcmp( IP, banList[ index ]->IP ) == 0 )
        {
            temp = banList[ index ];
            banList[ index ] = banList[ banList.Size() - 1 ];
            banList.RemoveAtIndex( banList.Size() - 1 );
            break;
        }
    }

    banListMutex.Unlock();

    if (temp)
    {
        veFree_Ex(temp->IP, _FILE_AND_LINE_ );
        VENet::OP_DELETE(temp, _FILE_AND_LINE_);
    }

}

void VEPeer::ClearBanList( void )
{
    unsigned index;
    index = 0;
    banListMutex.Lock();

    for ( ; index < banList.Size(); index++ )
    {
        veFree_Ex(banList[ index ]->IP, _FILE_AND_LINE_ );
        VENet::OP_DELETE(banList[ index ], _FILE_AND_LINE_);
    }

    banList.Clear(false, _FILE_AND_LINE_);

    banListMutex.Unlock();
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::SetLimitIPConnectionFrequency(bool b)
{
    limitConnectionFrequencyFromTheSameIP=b;
}

bool VEPeer::IsBanned( const char *IP )
{
    unsigned banListIndex, characterIndex;
    VENet::TimeMS time;
    BanStruct *temp;

    if ( IP == 0 || IP[ 0 ] == 0 || strlen( IP ) > 15 )
        return false;

    banListIndex = 0;

    if ( banList.Size() == 0 )
        return false;

    time = VENet::GetTimeMS();

    banListMutex.Lock();

    while ( banListIndex < banList.Size() )
    {
        if (banList[ banListIndex ]->timeout>0 && banList[ banListIndex ]->timeout<time)
        {
            temp = banList[ banListIndex ];
            banList[ banListIndex ] = banList[ banList.Size() - 1 ];
            banList.RemoveAtIndex( banList.Size() - 1 );
            veFree_Ex(temp->IP, _FILE_AND_LINE_ );
            VENet::OP_DELETE(temp, _FILE_AND_LINE_);
        }
        else
        {
            characterIndex = 0;

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
            while ( true )
            {
                if ( banList[ banListIndex ]->IP[ characterIndex ] == IP[ characterIndex ] )
                {
                    if ( IP[ characterIndex ] == 0 )
                    {
                        banListMutex.Unlock();
                        return true;
                    }

                    characterIndex++;
                }

                else
                {
                    if ( banList[ banListIndex ]->IP[ characterIndex ] == 0 || IP[ characterIndex ] == 0 )
                    {
                        break;
                    }

                    if ( banList[ banListIndex ]->IP[ characterIndex ] == '*' )
                    {
                        banListMutex.Unlock();

                        return true;
                    }

                    break;
                }
            }

            banListIndex++;
        }
    }

    banListMutex.Unlock();

    return false;
}

void VEPeer::Ping( const SystemAddress target )
{
    PingInternal(target, false, UNRELIABLE);
}

bool VEPeer::Ping( const char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections, unsigned connectionSocketIndex )
{
    if ( host == 0 )
        return false;

    VEAssert(connectionSocketIndex < socketList.Size());

    VENet::BitStream bitStream( sizeof(unsigned char) + sizeof(VENet::Time) );
    if ( onlyReplyOnAcceptingConnections )
        bitStream.Write((MessageID)ID_UNCONNECTED_PING_OPEN_CONNECTIONS);
    else
        bitStream.Write((MessageID)ID_UNCONNECTED_PING);

    bitStream.Write(VENet::GetTime());

    bitStream.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));

    unsigned int realIndex = GetVENetSocketFromUserConnectionSocketIndex(connectionSocketIndex);

    SystemAddress systemAddress;
    systemAddress.FromStringExplicitPort(host,remotePort, socketList[realIndex]->boundAddress.GetIPVersion());
    systemAddress.FixForIPVersion(socketList[realIndex]->boundAddress);

    unsigned i;
    for (i=0; i < pluginListNTS.Size(); i++)
        pluginListNTS[i]->OnDirectSocketSend((const char*)bitStream.GetData(), bitStream.GetNumberOfBitsUsed(), systemAddress);
    SocketLayer::SendTo( socketList[realIndex]->s, (const char*)bitStream.GetData(), (int) bitStream.GetNumberOfBytesUsed(), systemAddress, socketList[realIndex]->remotePortVENetWasStartedOn_PS3_PSP2, socketList[realIndex]->extraSocketOptions, _FILE_AND_LINE_ );

    return true;
}

int VEPeer::GetAveragePing( const AddressOrGUID systemIdentifier )
{
    int sum, quantity;
    RemoteSystemStruct *remoteSystem = GetRemoteSystem( systemIdentifier, false, false );

    if ( remoteSystem == 0 )
        return -1;

    for ( sum = 0, quantity = 0; quantity < PING_TIMES_ARRAY_SIZE; quantity++ )
    {
        if ( remoteSystem->pingAndClockDifferential[ quantity ].pingTime == 65535 )
            break;
        else
            sum += remoteSystem->pingAndClockDifferential[ quantity ].pingTime;
    }

    if ( quantity > 0 )
        return sum / quantity;
    else
        return -1;
}

int VEPeer::GetLastPing( const AddressOrGUID systemIdentifier ) const
{
    RemoteSystemStruct * remoteSystem = GetRemoteSystem( systemIdentifier, false, false );

    if ( remoteSystem == 0 )
        return -1;


    if ( remoteSystem->pingAndClockDifferentialWriteIndex == 0 )
        return remoteSystem->pingAndClockDifferential[ PING_TIMES_ARRAY_SIZE - 1 ].pingTime;
    else
        return remoteSystem->pingAndClockDifferential[ remoteSystem->pingAndClockDifferentialWriteIndex - 1 ].pingTime;
}

int VEPeer::GetLowestPing( const AddressOrGUID systemIdentifier ) const
{
    RemoteSystemStruct * remoteSystem = GetRemoteSystem( systemIdentifier, false, false );

    if ( remoteSystem == 0 )
        return -1;

    return remoteSystem->lowestPing;
}


void VEPeer::SetOccasionalPing( bool doPing )
{
    occasionalPing = doPing;
}

void VEPeer::SetOfflinePingResponse( const char *data, const unsigned int length )
{
    VEAssert(length < 400);

    vePeerMutexes[ offlinePingResponse_Mutex ].Lock();
    offlinePingResponse.Reset();

    if ( data && length > 0 )
        offlinePingResponse.Write( data, length );

    vePeerMutexes[ offlinePingResponse_Mutex ].Unlock();
}


void VEPeer::GetOfflinePingResponse( char **data, unsigned int *length )
{
    vePeerMutexes[ offlinePingResponse_Mutex ].Lock();
    *data = (char*) offlinePingResponse.GetData();
    *length = (int) offlinePingResponse.GetNumberOfBytesUsed();
    vePeerMutexes[ offlinePingResponse_Mutex ].Unlock();
}


SystemAddress VEPeer::GetInternalID( const SystemAddress systemAddress, const int index ) const
{
    if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS)
    {
        return ipList[index];
    }
    else
    {

        RemoteSystemStruct * remoteSystem = GetRemoteSystemFromSystemAddress( systemAddress, false, true );
        if (remoteSystem==0)
            return UNASSIGNED_SYSTEM_ADDRESS;

        return remoteSystem->theirInternalSystemAddress[index];
    }
}

SystemAddress VEPeer::GetExternalID( const SystemAddress target ) const
{
    unsigned i;
    SystemAddress inactiveExternalId;

    inactiveExternalId=UNASSIGNED_SYSTEM_ADDRESS;

    if (target==UNASSIGNED_SYSTEM_ADDRESS)
        return firstExternalID;

    for ( i = 0; i < maximumNumberOfPeers; i++ )
    {
        if (remoteSystemList[ i ].systemAddress == target )
        {
            if ( remoteSystemList[ i ].isActive )
                return remoteSystemList[ i ].myExternalSystemAddress;
            else if (remoteSystemList[ i ].myExternalSystemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
                inactiveExternalId=remoteSystemList[ i ].myExternalSystemAddress;
        }
    }

    return inactiveExternalId;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const VENetGUID VEPeer::GetMyGUID(void) const
{
    return myGuid;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SystemAddress VEPeer::GetMyBoundAddress(const int socketIndex)
{
    DataStructures::List<VENetSmartPtr<VENetSocket> > sockets;
    GetSockets( sockets );
    if (sockets.Size()>0)
        return sockets[socketIndex]->boundAddress;
    else
        return UNASSIGNED_SYSTEM_ADDRESS;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const VENetGUID& VEPeer::GetGuidFromSystemAddress( const SystemAddress input ) const
{
    if (input==UNASSIGNED_SYSTEM_ADDRESS)
        return myGuid;

    if (input.systemIndex!=(SystemIndex)-1 && input.systemIndex<maximumNumberOfPeers && remoteSystemList[ input.systemIndex ].systemAddress == input)
        return remoteSystemList[ input.systemIndex ].guid;

    unsigned int i;
    for ( i = 0; i < maximumNumberOfPeers; i++ )
    {
        if (remoteSystemList[ i ].systemAddress == input )
        {
            remoteSystemList[i].guid.systemIndex = (SystemIndex) i;

            return remoteSystemList[ i ].guid;
        }
    }

    return UNASSIGNED_VENET_GUID;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int VEPeer::GetSystemIndexFromGuid( const VENetGUID input ) const
{
    if (input==UNASSIGNED_VENET_GUID)
        return (unsigned int) -1;

    if (input==myGuid)
        return (unsigned int) -1;

    if (input.systemIndex!=(SystemIndex)-1 && input.systemIndex<maximumNumberOfPeers && remoteSystemList[ input.systemIndex ].guid == input)
        return input.systemIndex;

    unsigned int i;
    for ( i = 0; i < maximumNumberOfPeers; i++ )
    {
        if (remoteSystemList[ i ].guid == input )
        {
            remoteSystemList[i].guid.systemIndex = (SystemIndex) i;

            return i;
        }
    }

    return (unsigned int) -1;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SystemAddress VEPeer::GetSystemAddressFromGuid( const VENetGUID input ) const
{
    if (input==UNASSIGNED_VENET_GUID)
        return UNASSIGNED_SYSTEM_ADDRESS;

    if (input==myGuid)
        return GetInternalID(UNASSIGNED_SYSTEM_ADDRESS);

    if (input.systemIndex!=(SystemIndex)-1 && input.systemIndex<maximumNumberOfPeers && remoteSystemList[ input.systemIndex ].guid == input)
        return remoteSystemList[ input.systemIndex ].systemAddress;

    unsigned int i;
    for ( i = 0; i < maximumNumberOfPeers; i++ )
    {
        if (remoteSystemList[ i ].guid == input )
        {
            remoteSystemList[i].guid.systemIndex = (SystemIndex) i;

            return remoteSystemList[ i ].systemAddress;
        }
    }

    return UNASSIGNED_SYSTEM_ADDRESS;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool VEPeer::GetClientPublicKeyFromSystemAddress( const SystemAddress input, char *client_public_key ) const
{
#if LIBCAT_SECURITY == 1
    if (input == UNASSIGNED_SYSTEM_ADDRESS)
        return false;

    char *copy_source = 0;

    if (input.systemIndex!=(SystemIndex)-1 && input.systemIndex<maximumNumberOfPeers && remoteSystemList[ input.systemIndex ].systemAddress == input)
    {
        copy_source = remoteSystemList[ input.systemIndex ].client_public_key;
    }
    else
    {
        for ( unsigned int i = 0; i < maximumNumberOfPeers; i++ )
        {
            if (remoteSystemList[ i ].systemAddress == input )
            {
                copy_source = remoteSystemList[ i ].client_public_key;
                break;
            }
        }
    }

    if (copy_source)
    {
        // Verify that at least one byte in the public key is non-zero to indicate that the key was received
        for (int ii = 0; ii < cat::EasyHandshake::PUBLIC_KEY_BYTES; ++ii)
        {
            if (copy_source[ii] != 0)
            {
                memcpy(client_public_key, copy_source, cat::EasyHandshake::PUBLIC_KEY_BYTES);
                return true;
            }
        }
    }

#else
    (void) input;
    (void) client_public_key;
#endif

    return false;
}

void VEPeer::SetTimeoutTime( VENet::TimeMS timeMS, const SystemAddress target )
{
    if (target==UNASSIGNED_SYSTEM_ADDRESS)
    {
        defaultTimeoutTime=timeMS;

        unsigned i;
        for ( i = 0; i < maximumNumberOfPeers; i++ )
        {
            if (remoteSystemList[ i ].isActive)
            {
                if ( remoteSystemList[ i ].isActive )
                    remoteSystemList[ i ].reliabilityLayer.SetTimeoutTime(timeMS);
            }
        }
    }
    else
    {
        RemoteSystemStruct * remoteSystem = GetRemoteSystemFromSystemAddress( target, false, true );

        if ( remoteSystem != 0 )
            remoteSystem->reliabilityLayer.SetTimeoutTime(timeMS);
    }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

VENet::TimeMS VEPeer::GetTimeoutTime( const SystemAddress target )
{
    if (target==UNASSIGNED_SYSTEM_ADDRESS)
    {
        return defaultTimeoutTime;
    }
    else
    {
        RemoteSystemStruct * remoteSystem = GetRemoteSystemFromSystemAddress( target, false, true );

        if ( remoteSystem != 0 )
            remoteSystem->reliabilityLayer.GetTimeoutTime();
    }
    return defaultTimeoutTime;
}

int VEPeer::GetMTUSize( const SystemAddress target ) const
{
    if (target!=UNASSIGNED_SYSTEM_ADDRESS)
    {
        RemoteSystemStruct *rss=GetRemoteSystemFromSystemAddress(target, false, true);
        if (rss)
            return rss->MTUSize;
    }
    return defaultMTUSize;
}

unsigned int VEPeer::GetNumberOfAddresses( void )
{

    int i = 0;

    while ( ipList[ i ]!=UNASSIGNED_SYSTEM_ADDRESS )
        i++;

    return i;
}

const char* VEPeer::GetLocalIP( unsigned int index )
{
    if (IsActive()==false)
    {

        SocketLayer::GetMyIP( ipList );

    }


    static char str[128];
    ipList[index].ToString(false,str);
    return str;




}


bool VEPeer::IsLocalIP( const char *ip )
{
    if (ip==0 || ip[0]==0)
        return false;


    if (strcmp(ip, "127.0.0.1")==0 || strcmp(ip, "localhost")==0)
        return true;

    int num = GetNumberOfAddresses();
    int i;
    for (i=0; i < num; i++)
    {
        if (strcmp(ip, GetLocalIP(i))==0)
            return true;
    }
    return false;
}


void VEPeer::AllowConnectionResponseIPMigration( bool allow )
{
    allowConnectionResponseIPMigration = allow;
}

bool VEPeer::AdvertiseSystem( const char *host, unsigned short remotePort, const char *data, int dataLength, unsigned connectionSocketIndex )
{
    VENet::BitStream bs;
    bs.Write((MessageID)ID_ADVERTISE_SYSTEM);
    bs.WriteAlignedBytes((const unsigned char*) data,dataLength);
    return SendOutOfBand(host, remotePort, (const char*) bs.GetData(), bs.GetNumberOfBytesUsed(), connectionSocketIndex );
}

void VEPeer::SetSplitMessageProgressInterval(int interval)
{
    VEAssert(interval>=0);
    splitMessageProgressInterval=interval;
    for ( unsigned short i = 0; i < maximumNumberOfPeers; i++ )
        remoteSystemList[ i ].reliabilityLayer.SetSplitMessageProgressInterval(splitMessageProgressInterval);
}

int VEPeer::GetSplitMessageProgressInterval(void) const
{
    return splitMessageProgressInterval;
}

void VEPeer::SetUnreliableTimeout(VENet::TimeMS timeoutMS)
{
    unreliableTimeout=timeoutMS;
    for ( unsigned short i = 0; i < maximumNumberOfPeers; i++ )
        remoteSystemList[ i ].reliabilityLayer.SetUnreliableTimeout(unreliableTimeout);
}

void VEPeer::SendTTL( const char* host, unsigned short remotePort, int ttl, unsigned connectionSocketIndex )
{
    char fakeData[2];
    fakeData[0]=0;
    fakeData[1]=1;
    unsigned int realIndex = GetVENetSocketFromUserConnectionSocketIndex(connectionSocketIndex);
    SystemAddress systemAddress = socketList[realIndex]->boundAddress;
    systemAddress.FromStringExplicitPort(host,remotePort);
    SocketLayer::SendToTTL( socketList[realIndex]->s, (char*)fakeData, 2, systemAddress, ttl );
}

void VEPeer::AttachPlugin( PluginInterface2 *plugin )
{
    bool isNotThreadsafe = plugin->UsesReliabilityLayer();
    if (isNotThreadsafe)
    {
        if (pluginListNTS.GetIndexOf(plugin)==MAX_UNSIGNED_LONG)
        {
            plugin->SetRakPeerInterface(this);
            plugin->OnAttach();
            pluginListNTS.Insert(plugin, _FILE_AND_LINE_);
        }
    }
    else
    {
        if (pluginListTS.GetIndexOf(plugin)==MAX_UNSIGNED_LONG)
        {
            plugin->SetRakPeerInterface(this);
            plugin->OnAttach();
            pluginListTS.Insert(plugin, _FILE_AND_LINE_);
        }
    }
}

void VEPeer::DetachPlugin( PluginInterface2 *plugin )
{
    if (plugin==0)
        return;

    unsigned int index;

    bool isNotThreadsafe = plugin->UsesReliabilityLayer();
    if (isNotThreadsafe)
    {
        index = pluginListNTS.GetIndexOf(plugin);
        if (index!=MAX_UNSIGNED_LONG)
        {
            pluginListNTS[index]=pluginListNTS[pluginListNTS.Size()-1];
            pluginListNTS.RemoveFromEnd();
        }
    }
    else
    {
        index = pluginListTS.GetIndexOf(plugin);
        if (index!=MAX_UNSIGNED_LONG)
        {
            pluginListTS[index]=pluginListTS[pluginListTS.Size()-1];
            pluginListTS.RemoveFromEnd();
        }
    }
    plugin->OnDetach();
    plugin->SetRakPeerInterface(0);
}

void VEPeer::PushBackPacket( Packet *packet, bool pushAtHead)
{
    if (packet==0)
        return;

    unsigned i;
    for (i=0; i < pluginListTS.Size(); i++)
        pluginListTS[i]->OnPushBackPacket((const char*) packet->data, packet->bitSize, packet->systemAddress);
    for (i=0; i < pluginListNTS.Size(); i++)
        pluginListNTS[i]->OnPushBackPacket((const char*) packet->data, packet->bitSize, packet->systemAddress);

    packetReturnMutex.Lock();
    if (pushAtHead)
        packetReturnQueue.PushAtHead(packet,0,_FILE_AND_LINE_);
    else
        packetReturnQueue.Push(packet,_FILE_AND_LINE_);
    packetReturnMutex.Unlock();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ChangeSystemAddress(VENetGUID guid, const SystemAddress &systemAddress)
{
    BufferedCommandStruct *bcs;

    bcs=bufferedCommands.Allocate( _FILE_AND_LINE_ );
    bcs->data = 0;
    bcs->systemIdentifier.systemAddress=systemAddress;
    bcs->systemIdentifier.veNetGuid=guid;
    bcs->command=BufferedCommandStruct::BCS_CHANGE_SYSTEM_ADDRESS;
    bufferedCommands.Push(bcs);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Packet* VEPeer::AllocatePacket(unsigned dataSize)
{
    return AllocPacket(dataSize, _FILE_AND_LINE_);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VENetSmartPtr<VENetSocket> VEPeer::GetSocket( const SystemAddress target )
{
    BufferedCommandStruct *bcs;
    bcs=bufferedCommands.Allocate( _FILE_AND_LINE_ );
    bcs->command=BufferedCommandStruct::BCS_GET_SOCKET;
    bcs->systemIdentifier=target;
    bcs->data=0;
    bufferedCommands.Push(bcs);

    SocketQueryOutput *sqo;
    VENet::TimeMS stopWaiting = VENet::GetTimeMS()+1000;
    DataStructures::List<VENetSmartPtr<VENetSocket> > output;
    while (VENet::GetTimeMS() < stopWaiting)
    {
        if (isMainLoopThreadActive==false)
            return VENetSmartPtr<VENetSocket>();

        VESleep(0);

        sqo = socketQueryOutput.Pop();
        if (sqo)
        {
            output=sqo->sockets;
            sqo->sockets.Clear(false, _FILE_AND_LINE_);
            socketQueryOutput.Deallocate(sqo, _FILE_AND_LINE_);
            if (output.Size())
                return output[0];
            break;
        }
    }
    return VENetSmartPtr<VENetSocket>();
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::GetSockets( DataStructures::List<VENetSmartPtr<VENetSocket> > &sockets )
{
    sockets.Clear(false, _FILE_AND_LINE_);

    BufferedCommandStruct *bcs;

    bcs=bufferedCommands.Allocate( _FILE_AND_LINE_ );
    bcs->command=BufferedCommandStruct::BCS_GET_SOCKET;
    bcs->systemIdentifier=UNASSIGNED_SYSTEM_ADDRESS;
    bcs->data=0;
    bufferedCommands.Push(bcs);

    SocketQueryOutput *sqo;
    VENetSmartPtr<VENetSocket> output;
    while (1)
    {
        if (isMainLoopThreadActive==false)
            return;

        VESleep(0);

        sqo = socketQueryOutput.Pop();
        if (sqo)
        {
            sockets=sqo->sockets;
            sqo->sockets.Clear(false, _FILE_AND_LINE_);
            socketQueryOutput.Deallocate(sqo, _FILE_AND_LINE_);
            return;
        }
    }
    return;
}
void VEPeer::ReleaseSockets( DataStructures::List<VENetSmartPtr<VENetSocket> > &sockets )
{
    sockets.Clear(false,_FILE_AND_LINE_);
}

void VEPeer::ApplyNetworkSimulator( float packetloss, unsigned short minExtraPing, unsigned short extraPingVariance)
{
#ifdef _DEBUG
    if (remoteSystemList)
    {
        unsigned short i;
        for (i=0; i < maximumNumberOfPeers; i++)
            remoteSystemList[i].reliabilityLayer.ApplyNetworkSimulator(packetloss, minExtraPing, extraPingVariance);
    }

    _packetloss=packetloss;
    _minExtraPing=minExtraPing;
    _extraPingVariance=extraPingVariance;
#endif
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void VEPeer::SetPerConnectionOutgoingBandwidthLimit( unsigned maxBitsPerSecond )
{
    maxOutgoingBPS=maxBitsPerSecond;
}

bool VEPeer::IsNetworkSimulatorActive( void )
{
#ifdef _DEBUG
    return _packetloss>0 || _minExtraPing>0 || _extraPingVariance>0;
#else
    return false;
#endif
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::WriteOutOfBandHeader(VENet::BitStream *bitStream)
{
    bitStream->Write((MessageID)ID_OUT_OF_BAND_INTERNAL);
    bitStream->Write(myGuid);
    bitStream->WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::SetUserUpdateThread(void (*_userUpdateThreadPtr)(VEPeerInterface *, void *), void *_userUpdateThreadData)
{
    userUpdateThreadPtr=_userUpdateThreadPtr;
    userUpdateThreadData=_userUpdateThreadData;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::SendOutOfBand(const char *host, unsigned short remotePort, const char *data, BitSize_t dataLength, unsigned connectionSocketIndex )
{
    if ( IsActive() == false )
        return false;

    if (host==0  || host[0]==0)
        return false;

    VEAssert(connectionSocketIndex < socketList.Size());

    VEAssert(dataLength <= (MAX_OFFLINE_DATA_LENGTH + sizeof(unsigned char)+sizeof(VENet::Time)+VENetGUID::size()+sizeof(OFFLINE_MESSAGE_DATA_ID)));

    if (host==0)
        return false;

    VENet::BitStream bitStream;
    WriteOutOfBandHeader(&bitStream);

    if (dataLength>0)
    {
        bitStream.Write(data, dataLength);
    }

    unsigned int realIndex = GetVENetSocketFromUserConnectionSocketIndex(connectionSocketIndex);

    SystemAddress systemAddress;
    systemAddress.FromStringExplicitPort(host,remotePort, socketList[realIndex]->boundAddress.GetIPVersion());
    systemAddress.FixForIPVersion(socketList[realIndex]->boundAddress);

    unsigned i;
    for (i=0; i < pluginListNTS.Size(); i++)
        pluginListNTS[i]->OnDirectSocketSend((const char*)bitStream.GetData(), bitStream.GetNumberOfBitsUsed(), systemAddress);

    SocketLayer::SendTo( socketList[realIndex]->s, (const char*)bitStream.GetData(), (int) bitStream.GetNumberOfBytesUsed(), systemAddress, socketList[realIndex]->remotePortVENetWasStartedOn_PS3_PSP2, socketList[realIndex]->extraSocketOptions, _FILE_AND_LINE_ );

    return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VENetStatistics * VEPeer::GetStatistics( const SystemAddress systemAddress, VENetStatistics *rns )
{
    static VENetStatistics staticStatistics;
    VENetStatistics *systemStats;
    if (rns==0)
        systemStats=&staticStatistics;
    else
        systemStats=rns;

    if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS)
    {
        bool firstWrite=false;
        for ( unsigned short i = 0; i < maximumNumberOfPeers; i++ )
        {
            if (remoteSystemList[ i ].isActive)
            {
                VENetStatistics rnsTemp;
                remoteSystemList[ i ].reliabilityLayer.GetStatistics(&rnsTemp);

                if (firstWrite==false)
                {
                    memcpy(systemStats, &rnsTemp, sizeof(VENetStatistics));
                    firstWrite=true;
                }
                else
                    (*systemStats)+=rnsTemp;
            }
        }
        return systemStats;
    }
    else
    {
        RemoteSystemStruct * rss;
        rss = GetRemoteSystemFromSystemAddress( systemAddress, false, false );
        if ( rss && endThreads==false )
        {
            rss->reliabilityLayer.GetStatistics(systemStats);
            return systemStats;
        }
    }

    return 0;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::GetStatisticsList(DataStructures::List<SystemAddress> &addresses, DataStructures::List<VENetGUID> &guids, DataStructures::List<VENetStatistics> &statistics)
{
    addresses.Clear(false, _FILE_AND_LINE_);
    guids.Clear(false, _FILE_AND_LINE_);
    statistics.Clear(false, _FILE_AND_LINE_);

    if ( remoteSystemList == 0 || endThreads == true )
        return;

    unsigned int i;
    for (i=0; i < activeSystemListSize; i++)
    {
        if ((activeSystemList[i])->isActive &&
                (activeSystemList[i])->connectMode==VEPeer::RemoteSystemStruct::CONNECTED)
        {
            addresses.Push((activeSystemList[i])->systemAddress, _FILE_AND_LINE_ );
            guids.Push((activeSystemList[i])->guid, _FILE_AND_LINE_ );
            VENetStatistics rns;
            (activeSystemList[i])->reliabilityLayer.GetStatistics(&rns);
            statistics.Push(rns, _FILE_AND_LINE_);
        }
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::GetStatistics( const int index, VENetStatistics *rns )
{
    if (index < maximumNumberOfPeers && remoteSystemList[ index ].isActive)
    {
        remoteSystemList[ index ].reliabilityLayer.GetStatistics(rns);
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int VEPeer::GetReceiveBufferSize(void)
{
    unsigned int size;
    packetReturnMutex.Lock();
    size=packetReturnQueue.Size();
    packetReturnMutex.Unlock();
    return size;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int VEPeer::GetIndexFromSystemAddress( const SystemAddress systemAddress, bool calledFromNetworkThread ) const
{
    unsigned i;

    if ( systemAddress == UNASSIGNED_SYSTEM_ADDRESS )
        return -1;

    if (systemAddress.systemIndex!=(SystemIndex)-1 && systemAddress.systemIndex < maximumNumberOfPeers && remoteSystemList[systemAddress.systemIndex].systemAddress==systemAddress && remoteSystemList[ systemAddress.systemIndex ].isActive)
        return systemAddress.systemIndex;

    if (calledFromNetworkThread)
    {
        return GetRemoteSystemIndex(systemAddress);
    }
    else
    {
        for ( i = 0; i < maximumNumberOfPeers; i++ )
            if ( remoteSystemList[ i ].isActive && remoteSystemList[ i ].systemAddress == systemAddress )
                return i;

        for ( i = 0; i < maximumNumberOfPeers; i++ )
            if ( remoteSystemList[ i ].systemAddress == systemAddress )
                return i;
    }

    return -1;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int VEPeer::GetIndexFromGuid( const VENetGUID guid )
{
    unsigned i;

    if ( guid == UNASSIGNED_VENET_GUID )
        return -1;

    if (guid.systemIndex!=(SystemIndex)-1 && guid.systemIndex < maximumNumberOfPeers && remoteSystemList[guid.systemIndex].guid==guid && remoteSystemList[ guid.systemIndex ].isActive)
        return guid.systemIndex;

    for ( i = 0; i < maximumNumberOfPeers; i++ )
        if ( remoteSystemList[ i ].isActive && remoteSystemList[ i ].guid == guid )
            return i;

    for ( i = 0; i < maximumNumberOfPeers; i++ )
        if ( remoteSystemList[ i ].guid == guid )
            return i;

    return -1;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if LIBCAT_SECURITY==1
bool VEPeer::GenerateConnectionRequestChallenge(RequestedConnectionStruct *rcs,PublicKey *publicKey)
{
    CAT_AUDIT_PRINTF("AUDIT: In GenerateConnectionRequestChallenge()\n");

    rcs->client_handshake = 0;
    rcs->publicKeyMode = PKM_INSECURE_CONNECTION;

    if (!publicKey) return true;

    switch (publicKey->publicKeyMode)
    {
    default:
    case PKM_INSECURE_CONNECTION:
        break;

    case PKM_ACCEPT_ANY_PUBLIC_KEY:
        CAT_OBJCLR(rcs->remote_public_key);
        rcs->client_handshake = VENet::OP_NEW<cat::ClientEasyHandshake>(_FILE_AND_LINE_);

        rcs->publicKeyMode = PKM_ACCEPT_ANY_PUBLIC_KEY;
        break;

    case PKM_USE_TWO_WAY_AUTHENTICATION:
        if (publicKey->myPublicKey == 0 || publicKey->myPrivateKey == 0 ||
                publicKey->remoteServerPublicKey == 0)
        {
            return false;
        }

        rcs->client_handshake = VENet::OP_NEW<cat::ClientEasyHandshake>(_FILE_AND_LINE_);
        memcpy(rcs->remote_public_key, publicKey->remoteServerPublicKey, cat::EasyHandshake::PUBLIC_KEY_BYTES);

        if (!rcs->client_handshake->Initialize(publicKey->remoteServerPublicKey) ||
                !rcs->client_handshake->SetIdentity(publicKey->myPublicKey, publicKey->myPrivateKey) ||
                !rcs->client_handshake->GenerateChallenge(rcs->handshakeChallenge))
        {
            CAT_AUDIT_PRINTF("AUDIT: Failure initializing new client_handshake object with identity for this RequestedConnectionStruct\n");
            VENet::OP_DELETE(rcs->client_handshake,_FILE_AND_LINE_);
            rcs->client_handshake=0;
            return false;
        }

        CAT_AUDIT_PRINTF("AUDIT: Success initializing new client handshake object with identity for this RequestedConnectionStruct -- pre-generated challenge\n");

        rcs->publicKeyMode = PKM_USE_TWO_WAY_AUTHENTICATION;
        break;

    case PKM_USE_KNOWN_PUBLIC_KEY:
        if (publicKey->remoteServerPublicKey == 0)
            return false;

        rcs->client_handshake = VENet::OP_NEW<cat::ClientEasyHandshake>(_FILE_AND_LINE_);
        memcpy(rcs->remote_public_key, publicKey->remoteServerPublicKey, cat::EasyHandshake::PUBLIC_KEY_BYTES);

        if (!rcs->client_handshake->Initialize(publicKey->remoteServerPublicKey) ||
                !rcs->client_handshake->GenerateChallenge(rcs->handshakeChallenge))
        {
            CAT_AUDIT_PRINTF("AUDIT: Failure initializing new client_handshake object for this RequestedConnectionStruct\n");
            VENet::OP_DELETE(rcs->client_handshake,_FILE_AND_LINE_);
            rcs->client_handshake=0;
            return false;
        }

        CAT_AUDIT_PRINTF("AUDIT: Success initializing new client handshake object for this RequestedConnectionStruct -- pre-generated challenge\n");

        rcs->publicKeyMode = PKM_USE_KNOWN_PUBLIC_KEY;
        break;
    }

    return true;
}
#endif
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConnectionAttemptResult VEPeer::SendConnectionRequest( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned int extraData, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, VENet::TimeMS timeoutTime )
{
    VEAssert(passwordDataLength <= 256);
    VEAssert(remotePort!=0);
    SystemAddress systemAddress;
    if (!systemAddress.FromStringExplicitPort(host,remotePort,socketList[connectionSocketIndex]->boundAddress.GetIPVersion()))
        return CANNOT_RESOLVE_DOMAIN_NAME;

    if (GetRemoteSystemFromSystemAddress(systemAddress, false, true))
        return ALREADY_CONNECTED_TO_ENDPOINT;

    RequestedConnectionStruct *rcs = VENet::OP_NEW<RequestedConnectionStruct>(_FILE_AND_LINE_);

    rcs->systemAddress=systemAddress;
    rcs->nextRequestTime=VENet::GetTimeMS();
    rcs->requestsMade=0;
    rcs->data=0;
    rcs->extraData=extraData;
    rcs->socketIndex=connectionSocketIndex;
    rcs->actionToTake=RequestedConnectionStruct::CONNECT;
    rcs->sendConnectionAttemptCount=sendConnectionAttemptCount;
    rcs->timeBetweenSendConnectionAttemptsMS=timeBetweenSendConnectionAttemptsMS;
    memcpy(rcs->outgoingPassword, passwordData, passwordDataLength);
    rcs->outgoingPasswordLength=(unsigned char) passwordDataLength;
    rcs->timeoutTime=timeoutTime;

#if LIBCAT_SECURITY==1
    CAT_AUDIT_PRINTF("AUDIT: In SendConnectionRequest()\n");
    if (!GenerateConnectionRequestChallenge(rcs,publicKey))
        return SECURITY_INITIALIZATION_FAILED;
#else
    (void) publicKey;
#endif

    unsigned int i=0;
    requestedConnectionQueueMutex.Lock();
    for (; i < requestedConnectionQueue.Size(); i++)
    {
        if (requestedConnectionQueue[i]->systemAddress==systemAddress)
        {
            requestedConnectionQueueMutex.Unlock();
            VENet::OP_DELETE(rcs,_FILE_AND_LINE_);
            return CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS;
        }
    }
    requestedConnectionQueue.Push(rcs, _FILE_AND_LINE_ );
    requestedConnectionQueueMutex.Unlock();

    return CONNECTION_ATTEMPT_STARTED;
}
ConnectionAttemptResult VEPeer::SendConnectionRequest( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned int extraData, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, VENet::TimeMS timeoutTime, VENetSmartPtr<VENetSocket> socket )
{
    VEAssert(passwordDataLength <= 256);
    SystemAddress systemAddress;
    systemAddress.FromStringExplicitPort(host,remotePort);

    if (GetRemoteSystemFromSystemAddress(systemAddress, false, true))
        return ALREADY_CONNECTED_TO_ENDPOINT;

    RequestedConnectionStruct *rcs = VENet::OP_NEW<RequestedConnectionStruct>(_FILE_AND_LINE_);

    rcs->systemAddress=systemAddress;
    rcs->nextRequestTime=VENet::GetTimeMS();
    rcs->requestsMade=0;
    rcs->data=0;
    rcs->extraData=extraData;
    rcs->socketIndex=connectionSocketIndex;
    rcs->actionToTake=RequestedConnectionStruct::CONNECT;
    rcs->sendConnectionAttemptCount=sendConnectionAttemptCount;
    rcs->timeBetweenSendConnectionAttemptsMS=timeBetweenSendConnectionAttemptsMS;
    memcpy(rcs->outgoingPassword, passwordData, passwordDataLength);
    rcs->outgoingPasswordLength=(unsigned char) passwordDataLength;
    rcs->timeoutTime=timeoutTime;
    rcs->socket=socket;

#if LIBCAT_SECURITY==1
    if (!GenerateConnectionRequestChallenge(rcs,publicKey))
        return SECURITY_INITIALIZATION_FAILED;
#else
    (void) publicKey;
#endif

    unsigned int i=0;
    requestedConnectionQueueMutex.Lock();
    for (; i < requestedConnectionQueue.Size(); i++)
    {
        if (requestedConnectionQueue[i]->systemAddress==systemAddress)
        {
            requestedConnectionQueueMutex.Unlock();
            VENet::OP_DELETE(rcs,_FILE_AND_LINE_);
            return CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS;
        }
    }
    requestedConnectionQueue.Push(rcs, _FILE_AND_LINE_ );
    requestedConnectionQueueMutex.Unlock();

    return CONNECTION_ATTEMPT_STARTED;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ValidateRemoteSystemLookup(void) const
{
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::RemoteSystemStruct *VEPeer::GetRemoteSystem( const AddressOrGUID systemIdentifier, bool calledFromNetworkThread, bool onlyActive ) const
{
    if (systemIdentifier.veNetGuid!=UNASSIGNED_VENET_GUID)
        return GetRemoteSystemFromGUID(systemIdentifier.veNetGuid, onlyActive);
    else
        return GetRemoteSystemFromSystemAddress(systemIdentifier.systemAddress, calledFromNetworkThread, onlyActive);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::RemoteSystemStruct *VEPeer::GetRemoteSystemFromSystemAddress( const SystemAddress systemAddress, bool calledFromNetworkThread, bool onlyActive ) const
{
    unsigned i;

    if ( systemAddress == UNASSIGNED_SYSTEM_ADDRESS )
        return 0;

    if (calledFromNetworkThread)
    {
        unsigned int index = GetRemoteSystemIndex(systemAddress);
        if (index!=(unsigned int) -1)
        {
            if (onlyActive==false || remoteSystemList[ index ].isActive==true )
            {
                VEAssert(remoteSystemList[index].systemAddress==systemAddress);
                return remoteSystemList + index;
            }
        }
    }
    else
    {
        int deadConnectionIndex=-1;

        for ( i = 0; i < maximumNumberOfPeers; i++ )
        {
            if (remoteSystemList[ i ].systemAddress == systemAddress)
            {
                if ( remoteSystemList[ i ].isActive )
                    return remoteSystemList + i;
                else if (deadConnectionIndex==-1)
                    deadConnectionIndex=i;
            }
        }

        if (deadConnectionIndex!=-1 && onlyActive==false)
            return remoteSystemList + deadConnectionIndex;
    }

    return 0;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::RemoteSystemStruct *VEPeer::GetRemoteSystemFromGUID( const VENetGUID guid, bool onlyActive ) const
{
    if (guid==UNASSIGNED_VENET_GUID)
        return 0;

    unsigned i;
    for ( i = 0; i < maximumNumberOfPeers; i++ )
    {
        if (remoteSystemList[ i ].guid == guid && (onlyActive==false || remoteSystemList[ i ].isActive))
        {
            return remoteSystemList + i;
        }
    }
    return 0;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ParseConnectionRequestPacket( VEPeer::RemoteSystemStruct *remoteSystem, const SystemAddress &systemAddress, const char *data, int byteSize )
{
    VENet::BitStream bs((unsigned char*) data,byteSize,false);
    bs.IgnoreBytes(sizeof(MessageID));
    VENetGUID guid;
    bs.Read(guid);
    VENet::Time incomingTimestamp;
    bs.Read(incomingTimestamp);
    unsigned char doSecurity;
    bs.Read(doSecurity);

#if LIBCAT_SECURITY==1
    unsigned char doClientKey;
    if (_using_security)
    {
        if (doSecurity != 1 || !remoteSystem->reliabilityLayer.GetAuthenticatedEncryption())
            return;

        // Validate client proof of key
        unsigned char proof[cat::EasyHandshake::PROOF_BYTES];
        bs.ReadAlignedBytes(proof, sizeof(proof));
        if (!remoteSystem->reliabilityLayer.GetAuthenticatedEncryption()->ValidateProof(proof, sizeof(proof)))
        {
            remoteSystem->connectMode = RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY;
            return;
        }

        CAT_OBJCLR(remoteSystem->client_public_key);

        bs.Read(doClientKey);

        if (doClientKey == 1)
        {
            unsigned char ident[cat::EasyHandshake::IDENTITY_BYTES];
            bs.ReadAlignedBytes(ident, sizeof(ident));

            if (_require_client_public_key)
            {
                if (!_server_handshake->VerifyInitiatorIdentity(remoteSystem->answer, ident, remoteSystem->client_public_key))
                {
                    VENet::BitStream bitStream;
                    bitStream.Write((MessageID)ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY); // Report an error since the client is not providing an identity when it is necessary to connect
                    bitStream.Write((unsigned char)2); // Indicate client identity is invalid
                    SendImmediate((char*) bitStream.GetData(), bitStream.GetNumberOfBytesUsed(), IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false, false, VENet::GetTimeUS(), 0);
                    remoteSystem->connectMode = RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY;
                    return;
                }
            }
        }
        else
        {
            if (_require_client_public_key)
            {
                VENet::BitStream bitStream;
                bitStream.Write((MessageID)ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY);
                bitStream.Write((unsigned char)1);
                SendImmediate((char*) bitStream.GetData(), bitStream.GetNumberOfBytesUsed(), IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false, false, VENet::GetTimeUS(), 0);
                remoteSystem->connectMode = RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY;
                return;
            }
        }
    }
#endif
    unsigned char *password = bs.GetData()+BITS_TO_BYTES(bs.GetReadOffset());
    int passwordLength = byteSize - BITS_TO_BYTES(bs.GetReadOffset());
    if ( incomingPasswordLength != passwordLength ||
            memcmp( password, incomingPassword, incomingPasswordLength ) != 0 )
    {
        CAT_AUDIT_PRINTF("AUDIT: Invalid password\n");
        VENet::BitStream bitStream;
        bitStream.Write((MessageID)ID_INVALID_PASSWORD);
        bitStream.Write(GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));
        SendImmediate((char*) bitStream.GetData(), bitStream.GetNumberOfBytesUsed(), IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false, false, VENet::GetTimeUS(), 0);
        remoteSystem->connectMode=RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY;
        return;
    }

    remoteSystem->connectMode=RemoteSystemStruct::HANDLING_CONNECTION_REQUEST;

    OnConnectionRequest( remoteSystem, incomingTimestamp );
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::OnConnectionRequest( VEPeer::RemoteSystemStruct *remoteSystem, VENet::Time incomingTimestamp )
{
    VENet::BitStream bitStream;
    bitStream.Write((MessageID)ID_CONNECTION_REQUEST_ACCEPTED);
    bitStream.Write(remoteSystem->systemAddress);
    SystemIndex systemIndex = (SystemIndex) GetIndexFromSystemAddress( remoteSystem->systemAddress, true );
    VEAssert(systemIndex!=65535);
    bitStream.Write(systemIndex);
    for (unsigned int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
        bitStream.Write(ipList[i]);
    bitStream.Write(incomingTimestamp);
    bitStream.Write(VENet::GetTime());

    SendImmediate((char*)bitStream.GetData(), bitStream.GetNumberOfBitsUsed(), IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, remoteSystem->systemAddress, false, false, VENet::GetTimeUS(), 0);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::NotifyAndFlagForShutdown( const SystemAddress systemAddress, bool performImmediate, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority )
{
    VENet::BitStream temp( sizeof(unsigned char) );
    temp.Write( (MessageID)ID_DISCONNECTION_NOTIFICATION );
    if (performImmediate)
    {
        SendImmediate((char*)temp.GetData(), temp.GetNumberOfBitsUsed(), disconnectionNotificationPriority, RELIABLE_ORDERED, orderingChannel, systemAddress, false, false, VENet::GetTimeUS(), 0);
        RemoteSystemStruct *rss=GetRemoteSystemFromSystemAddress(systemAddress, true, true);
        rss->connectMode=RemoteSystemStruct::DISCONNECT_ASAP;
    }
    else
    {
        SendBuffered((const char*)temp.GetData(), temp.GetNumberOfBitsUsed(), disconnectionNotificationPriority, RELIABLE_ORDERED, orderingChannel, systemAddress, false, RemoteSystemStruct::DISCONNECT_ASAP, 0);
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned short VEPeer::GetNumberOfRemoteInitiatedConnections( void ) const
{
    if ( remoteSystemList == 0 || endThreads == true )
        return 0;

    unsigned short numberOfIncomingConnections;
    numberOfIncomingConnections = 0;
    unsigned int i;
    for (i=0; i < activeSystemListSize; i++)
    {
        if ((activeSystemList[i])->isActive &&
                (activeSystemList[i])->connectMode==VEPeer::RemoteSystemStruct::CONNECTED &&
                (activeSystemList[i])->weInitiatedTheConnection==false
           )
        {
            numberOfIncomingConnections++;
        }
    }
    return numberOfIncomingConnections;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::RemoteSystemStruct * VEPeer::AssignSystemAddressToRemoteSystemList( const SystemAddress systemAddress, RemoteSystemStruct::ConnectMode connectionMode, VENetSmartPtr<VENetSocket> incomingVENetSocket, bool *thisIPConnectedRecently, SystemAddress bindingAddress, int incomingMTU, VENetGUID guid, bool useSecurity )
{
    RemoteSystemStruct * remoteSystem;
    unsigned i,j,assignedIndex;
    VENet::TimeMS time = VENet::GetTimeMS();
#ifdef _DEBUG
    VEAssert(systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
#endif

    if (limitConnectionFrequencyFromTheSameIP)
    {
        if (IsLoopbackAddress(systemAddress,false)==false)
        {
            for ( i = 0; i < maximumNumberOfPeers; i++ )
            {
                if ( remoteSystemList[ i ].isActive==true &&
                        remoteSystemList[ i ].systemAddress.EqualsExcludingPort(systemAddress) &&
                        time >= remoteSystemList[ i ].connectionTime &&
                        time - remoteSystemList[ i ].connectionTime < 100
                   )
                {
                    *thisIPConnectedRecently=true;
                    ValidateRemoteSystemLookup();
                    return 0;
                }
            }
        }
    }

    bindingAddress.CopyPort(incomingVENetSocket->boundAddress);

    *thisIPConnectedRecently=false;
    for ( assignedIndex = 0; assignedIndex < maximumNumberOfPeers; assignedIndex++ )
    {
        if ( remoteSystemList[ assignedIndex ].isActive==false )
        {
            remoteSystem=remoteSystemList+assignedIndex;
            ReferenceRemoteSystem(systemAddress, assignedIndex);
            remoteSystem->MTUSize=defaultMTUSize;
            remoteSystem->guid=guid;
            remoteSystem->isActive = true;
            if (incomingMTU > remoteSystem->MTUSize)
                remoteSystem->MTUSize=incomingMTU;
            remoteSystem->reliabilityLayer.Reset(true, remoteSystem->MTUSize, useSecurity);
            remoteSystem->reliabilityLayer.SetSplitMessageProgressInterval(splitMessageProgressInterval);
            remoteSystem->reliabilityLayer.SetUnreliableTimeout(unreliableTimeout);
            remoteSystem->reliabilityLayer.SetTimeoutTime(defaultTimeoutTime);
            AddToActiveSystemList(assignedIndex);
            if (incomingVENetSocket->boundAddress==bindingAddress)
            {
                remoteSystem->VENetSocket=incomingVENetSocket;
            }
            else
            {
                char str[256];
                bindingAddress.ToString(true,str);
                unsigned int ipListIndex, foundIndex=(unsigned int)-1;

                for (ipListIndex=0; ipListIndex < MAXIMUM_NUMBER_OF_INTERNAL_IDS; ipListIndex++)
                {
                    if (ipList[ipListIndex]==UNASSIGNED_SYSTEM_ADDRESS)
                        break;

                    if (bindingAddress.EqualsExcludingPort(ipList[ipListIndex]))
                    {
                        foundIndex=ipListIndex;
                        break;
                    }
                }

                if (1 || foundIndex==(unsigned int)-1)
                {
                    remoteSystem->VENetSocket=incomingVENetSocket;
                }
                else
                {
                    unsigned int socketListIndex;
                    for (socketListIndex=0; socketListIndex < socketList.Size(); socketListIndex++)
                    {
                        if (socketList[socketListIndex]->boundAddress==bindingAddress)
                        {
                            remoteSystem->VENetSocket=socketList[socketListIndex];
                            break;
                        }
                    }

                    if (socketListIndex==socketList.Size())
                    {
                        char ipListFoundIndexStr[128];
                        ipList[foundIndex].ToString(false,str);

                        VENetSmartPtr<VENetSocket> rns(VENet::OP_NEW<VENetSocket>(_FILE_AND_LINE_));
                        if (incomingVENetSocket->remotePortVENetWasStartedOn_PS3_PSP2==0)
                            rns->s = (SOCKET) SocketLayer::CreateBoundSocket( this, bindingAddress.GetPort(), incomingVENetSocket->blockingSocket, ipListFoundIndexStr, 0, incomingVENetSocket->extraSocketOptions, incomingVENetSocket->socketFamily, incomingVENetSocket->chromeInstance );
                        else
                            rns->s = (SOCKET) SocketLayer::CreateBoundSocket_PS3Lobby( bindingAddress.GetPort(), incomingVENetSocket->blockingSocket, ipListFoundIndexStr, incomingVENetSocket->socketFamily );
                        if ((SOCKET)rns->s==INVALID_SOCKET)
                        {
                            remoteSystem->VENetSocket=incomingVENetSocket;
                        }
                        else
                        {
                            rns->boundAddress=bindingAddress;
                            rns->remotePortVENetWasStartedOn_PS3_PSP2=incomingVENetSocket->remotePortVENetWasStartedOn_PS3_PSP2;
                            rns->blockingSocket=incomingVENetSocket->blockingSocket;
                            rns->extraSocketOptions=incomingVENetSocket->extraSocketOptions;
                            rns->userConnectionSocketIndex=(unsigned int)-1;
                            socketList.Push(rns, _FILE_AND_LINE_ );
                            remoteSystem->VENetSocket=rns;

                            RakPeerAndIndex *rpai = VENet::OP_NEW<RakPeerAndIndex>(_FILE_AND_LINE_);
                            rpai->remotePortVENetWasStartedOn_PS3=rns->remotePortVENetWasStartedOn_PS3_PSP2;
                            rpai->extraSocketOptions=rns->extraSocketOptions;
                            rpai->s=rns->s;
                            rpai->rakPeer=this;
                            rpai->blockingSocket=rns->blockingSocket;
#ifdef _WIN32
                            int highPriority=THREAD_PRIORITY_ABOVE_NORMAL;
#else
                            int highPriority=-10;
#endif

                            highPriority=0;

#if VEPEER_USER_THREADED!=1

                            VENet::VEThread::Create(RecvFromLoop, rpai, highPriority);

#endif
                        }
                    }
                }
            }

            for ( j = 0; j < (unsigned) PING_TIMES_ARRAY_SIZE; j++ )
            {
                remoteSystem->pingAndClockDifferential[ j ].pingTime = 65535;
                remoteSystem->pingAndClockDifferential[ j ].clockDifferential = 0;
            }

            remoteSystem->connectMode=connectionMode;
            remoteSystem->pingAndClockDifferentialWriteIndex = 0;
            remoteSystem->lowestPing = 65535;
            remoteSystem->nextPingTime = 0;
            remoteSystem->weInitiatedTheConnection = false;
            remoteSystem->connectionTime = time;
            remoteSystem->myExternalSystemAddress = UNASSIGNED_SYSTEM_ADDRESS;
            remoteSystem->lastReliableSend=time;

#ifdef _DEBUG
            int indexLoopupCheck=GetIndexFromSystemAddress( systemAddress, true );
            if ((int) indexLoopupCheck!=(int) assignedIndex)
            {
                VEAssert((int) indexLoopupCheck==(int) assignedIndex);
            }
#endif

            return remoteSystem;
        }
    }

    return 0;
}

void VEPeer::ShiftIncomingTimestamp( unsigned char *data, const SystemAddress &systemAddress ) const
{
#ifdef _DEBUG
    VEAssert( IsActive() );
    VEAssert( data );
#endif

    VENet::BitStream timeBS( data, sizeof(VENet::Time), false);
    VENet::Time encodedTimestamp;
    timeBS.Read(encodedTimestamp);

    encodedTimestamp = encodedTimestamp - GetBestClockDifferential( systemAddress );
    timeBS.SetWriteOffset(0);
    timeBS.Write(encodedTimestamp);
}

VENet::Time VEPeer::GetBestClockDifferential( const SystemAddress systemAddress ) const
{
    int counter, lowestPingSoFar;
    VENet::Time clockDifferential;
    RemoteSystemStruct *remoteSystem = GetRemoteSystemFromSystemAddress( systemAddress, true, true );

    if ( remoteSystem == 0 )
        return 0;

    lowestPingSoFar = 65535;

    clockDifferential = 0;

    for ( counter = 0; counter < PING_TIMES_ARRAY_SIZE; counter++ )
    {
        if ( remoteSystem->pingAndClockDifferential[ counter ].pingTime == 65535 )
            break;

        if ( remoteSystem->pingAndClockDifferential[ counter ].pingTime < lowestPingSoFar )
        {
            clockDifferential = remoteSystem->pingAndClockDifferential[ counter ].clockDifferential;
            lowestPingSoFar = remoteSystem->pingAndClockDifferential[ counter ].pingTime;
        }
    }

    return clockDifferential;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int VEPeer::RemoteSystemLookupHashIndex(const SystemAddress &sa) const
{
    return SystemAddress::ToInteger(sa) % ((unsigned int) maximumNumberOfPeers * REMOTE_SYSTEM_LOOKUP_HASH_MULTIPLE);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ReferenceRemoteSystem(const SystemAddress &sa, unsigned int remoteSystemListIndex)
{
    SystemAddress oldAddress = remoteSystemList[remoteSystemListIndex].systemAddress;
    if (oldAddress!=UNASSIGNED_SYSTEM_ADDRESS)
    {
        if (GetRemoteSystem(oldAddress)==&remoteSystemList[remoteSystemListIndex])
            DereferenceRemoteSystem(oldAddress);
    }
    DereferenceRemoteSystem(sa);

    remoteSystemList[remoteSystemListIndex].systemAddress=sa;

    unsigned int hashIndex = RemoteSystemLookupHashIndex(sa);
    RemoteSystemIndex *rsi;
    rsi = remoteSystemIndexPool.Allocate(_FILE_AND_LINE_);
    if (remoteSystemLookup[hashIndex]==0)
    {
        rsi->next=0;
        rsi->index=remoteSystemListIndex;
        remoteSystemLookup[hashIndex]=rsi;
    }
    else
    {
        RemoteSystemIndex *cur = remoteSystemLookup[hashIndex];
        while (cur->next!=0)
        {
            cur=cur->next;
        }

        rsi = remoteSystemIndexPool.Allocate(_FILE_AND_LINE_);
        rsi->next=0;
        rsi->index=remoteSystemListIndex;
        cur->next=rsi;
    }


    VEAssert(GetRemoteSystemIndex(sa)==remoteSystemListIndex);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::DereferenceRemoteSystem(const SystemAddress &sa)
{
    unsigned int hashIndex = RemoteSystemLookupHashIndex(sa);
    RemoteSystemIndex *cur = remoteSystemLookup[hashIndex];
    RemoteSystemIndex *last = 0;
    while (cur!=0)
    {
        if (remoteSystemList[cur->index].systemAddress==sa)
        {
            if (last==0)
            {
                remoteSystemLookup[hashIndex]=cur->next;
            }
            else
            {
                last->next=cur->next;
            }
            remoteSystemIndexPool.Release(cur,_FILE_AND_LINE_);
            break;
        }
        last=cur;
        cur=cur->next;
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int VEPeer::GetRemoteSystemIndex(const SystemAddress &sa) const
{
    unsigned int hashIndex = RemoteSystemLookupHashIndex(sa);
    RemoteSystemIndex *cur = remoteSystemLookup[hashIndex];
    while (cur!=0)
    {
        if (remoteSystemList[cur->index].systemAddress==sa)
            return cur->index;
        cur=cur->next;
    }
    return (unsigned int) -1;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VEPeer::RemoteSystemStruct* VEPeer::GetRemoteSystem(const SystemAddress &sa) const
{
    unsigned int remoteSystemIndex = GetRemoteSystemIndex(sa);
    if (remoteSystemIndex==(unsigned int)-1)
        return 0;
    return remoteSystemList + remoteSystemIndex;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ClearRemoteSystemLookup(void)
{
    remoteSystemIndexPool.Clear(_FILE_AND_LINE_);
    VENet::OP_DELETE_ARRAY(remoteSystemLookup,_FILE_AND_LINE_);
    remoteSystemLookup=0;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::AddToActiveSystemList(unsigned int remoteSystemListIndex)
{
    activeSystemList[activeSystemListSize++]=remoteSystemList+remoteSystemListIndex;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::RemoveFromActiveSystemList(const SystemAddress &sa)
{
    unsigned int i;
    for (i=0; i < activeSystemListSize; i++)
    {
        RemoteSystemStruct *rss=activeSystemList[i];
        if (rss->systemAddress==sa)
        {
            activeSystemList[i]=activeSystemList[activeSystemListSize-1];
            activeSystemListSize--;
            return;
        }
    }
    VEAssert("activeSystemList invalid, entry not found in RemoveFromActiveSystemList. Ensure that AddToActiveSystemList and RemoveFromActiveSystemList are called by the same thread." && 0);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::IsLoopbackAddress(const AddressOrGUID &systemIdentifier, bool matchPort) const
{
    if (systemIdentifier.veNetGuid!=UNASSIGNED_VENET_GUID)
        return systemIdentifier.veNetGuid==myGuid;

    for (int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS && ipList[i]!=UNASSIGNED_SYSTEM_ADDRESS; i++)
    {
        if (matchPort)
        {
            if (ipList[i]==systemIdentifier.systemAddress)
                return true;
        }
        else
        {
            if (ipList[i].EqualsExcludingPort(systemIdentifier.systemAddress))
                return true;
        }
    }

    return (matchPort==true && systemIdentifier.systemAddress==firstExternalID) ||
           (matchPort==false && systemIdentifier.systemAddress.EqualsExcludingPort(firstExternalID));
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SystemAddress VEPeer::GetLoopbackAddress(void) const
{

    return ipList[0];



}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::AllowIncomingConnections(void) const
{
    return GetNumberOfRemoteInitiatedConnections() < GetMaximumIncomingConnections();
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::PingInternal( const SystemAddress target, bool performImmediate, PacketReliability reliability )
{
    if ( IsActive() == false )
        return ;

    VENet::BitStream bitStream(sizeof(unsigned char)+sizeof(VENet::Time));
    bitStream.Write((MessageID)ID_CONNECTED_PING);
    bitStream.Write(VENet::GetTime());
    if (performImmediate)
        SendImmediate( (char*)bitStream.GetData(), bitStream.GetNumberOfBitsUsed(), IMMEDIATE_PRIORITY, reliability, 0, target, false, false, VENet::GetTimeUS(), 0 );
    else
        Send( &bitStream, IMMEDIATE_PRIORITY, reliability, 0, target, false );
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::CloseConnectionInternal( const AddressOrGUID& systemIdentifier, bool sendDisconnectionNotification, bool performImmediate, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority )
{
#ifdef _DEBUG
    VEAssert(orderingChannel < 32);
#endif

    if (systemIdentifier.IsUndefined())
        return;

    if ( remoteSystemList == 0 || endThreads == true )
        return;

    SystemAddress target;
    if (systemIdentifier.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
    {
        target=systemIdentifier.systemAddress;
    }
    else
    {
        target=GetSystemAddressFromGuid(systemIdentifier.veNetGuid);
    }

    if (target!=UNASSIGNED_SYSTEM_ADDRESS && performImmediate)
        target.FixForIPVersion(socketList[0]->boundAddress);

    if (sendDisconnectionNotification)
    {
        NotifyAndFlagForShutdown(target, performImmediate, orderingChannel, disconnectionNotificationPriority);
    }
    else
    {
        if (performImmediate)
        {
            unsigned int index = GetRemoteSystemIndex(target);
            if (index!=(unsigned int) -1)
            {
                if ( remoteSystemList[index].isActive )
                {
                    RemoveFromActiveSystemList(target);

                    remoteSystemList[index].isActive = false;

                    remoteSystemList[index].guid=UNASSIGNED_VENET_GUID;

                    remoteSystemList[index].reliabilityLayer.Reset(false, remoteSystemList[index].MTUSize, false);

                    remoteSystemList[index].VENetSocket.SetNull();
                }
            }
        }
        else
        {
            BufferedCommandStruct *bcs;
            bcs=bufferedCommands.Allocate( _FILE_AND_LINE_ );
            bcs->command=BufferedCommandStruct::BCS_CLOSE_CONNECTION;
            bcs->systemIdentifier=target;
            bcs->data=0;
            bcs->orderingChannel=orderingChannel;
            bcs->priority=disconnectionNotificationPriority;
            bufferedCommands.Push(bcs);
        }
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::SendBuffered( const char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, RemoteSystemStruct::ConnectMode connectionMode, uint32_t receipt )
{
    BufferedCommandStruct *bcs;

    bcs=bufferedCommands.Allocate( _FILE_AND_LINE_ );
    bcs->data = (char*) rakMalloc_Ex( (size_t) BITS_TO_BYTES(numberOfBitsToSend), _FILE_AND_LINE_ ); // Making a copy doesn't lose efficiency because I tell the reliability layer to use this allocation for its own copy
    if (bcs->data==0)
    {
        notifyOutOfMemory(_FILE_AND_LINE_);
        bufferedCommands.Deallocate(bcs, _FILE_AND_LINE_);
        return;
    }

    VEAssert( !( reliability >= NUMBER_OF_RELIABILITIES || reliability < 0 ) );
    VEAssert( !( priority > NUMBER_OF_PRIORITIES || priority < 0 ) );
    VEAssert( !( orderingChannel >= NUMBER_OF_ORDERED_STREAMS ) );

    memcpy(bcs->data, data, (size_t) BITS_TO_BYTES(numberOfBitsToSend));
    bcs->numberOfBitsToSend=numberOfBitsToSend;
    bcs->priority=priority;
    bcs->reliability=reliability;
    bcs->orderingChannel=orderingChannel;
    bcs->systemIdentifier=systemIdentifier;
    bcs->broadcast=broadcast;
    bcs->connectionMode=connectionMode;
    bcs->receipt=receipt;
    bcs->command=BufferedCommandStruct::BCS_SEND;
    bufferedCommands.Push(bcs);

    if (priority==IMMEDIATE_PRIORITY)
    {
        quitAndDataEvents.SetEvent();
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::SendBufferedList( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, RemoteSystemStruct::ConnectMode connectionMode, uint32_t receipt )
{
    BufferedCommandStruct *bcs;
    unsigned int totalLength=0;
    unsigned int lengthOffset;
    int i;
    for (i=0; i < numParameters; i++)
    {
        if (lengths[i]>0)
            totalLength+=lengths[i];
    }
    if (totalLength==0)
        return;

    char *dataAggregate;
    dataAggregate = (char*) rakMalloc_Ex( (size_t) totalLength, _FILE_AND_LINE_ ); // Making a copy doesn't lose efficiency because I tell the reliability layer to use this allocation for its own copy
    if (dataAggregate==0)
    {
        notifyOutOfMemory(_FILE_AND_LINE_);
        return;
    }
    for (i=0, lengthOffset=0; i < numParameters; i++)
    {
        if (lengths[i]>0)
        {
            memcpy(dataAggregate+lengthOffset, data[i], lengths[i]);
            lengthOffset+=lengths[i];
        }
    }

    if (broadcast==false && IsLoopbackAddress(systemIdentifier,true))
    {
        SendLoopback(dataAggregate,totalLength);
        veFree_Ex(dataAggregate,_FILE_AND_LINE_);
        return;
    }

    VEAssert( !( reliability >= NUMBER_OF_RELIABILITIES || reliability < 0 ) );
    VEAssert( !( priority > NUMBER_OF_PRIORITIES || priority < 0 ) );
    VEAssert( !( orderingChannel >= NUMBER_OF_ORDERED_STREAMS ) );

    bcs=bufferedCommands.Allocate( _FILE_AND_LINE_ );
    bcs->data = dataAggregate;
    bcs->numberOfBitsToSend=BYTES_TO_BITS(totalLength);
    bcs->priority=priority;
    bcs->reliability=reliability;
    bcs->orderingChannel=orderingChannel;
    bcs->systemIdentifier=systemIdentifier;
    bcs->broadcast=broadcast;
    bcs->connectionMode=connectionMode;
    bcs->receipt=receipt;
    bcs->command=BufferedCommandStruct::BCS_SEND;
    bufferedCommands.Push(bcs);

    if (priority==IMMEDIATE_PRIORITY)
    {
        quitAndDataEvents.SetEvent();
    }
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::SendImmediate( char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, bool useCallerDataAllocation, VENet::TimeUS currentTime, uint32_t receipt )
{
    unsigned *sendList;
    unsigned sendListSize;
    bool callerDataAllocationUsed;
    unsigned int remoteSystemIndex, sendListIndex;
    callerDataAllocationUsed=false;

    sendListSize=0;

    if (systemIdentifier.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
        remoteSystemIndex=GetIndexFromSystemAddress( systemIdentifier.systemAddress, true );
    else if (systemIdentifier.veNetGuid!=UNASSIGNED_VENET_GUID)
        remoteSystemIndex=GetSystemIndexFromGuid(systemIdentifier.veNetGuid);
    else
        remoteSystemIndex=(unsigned int) -1;

    if (broadcast==false)
    {
        if (remoteSystemIndex==(unsigned int) -1)
        {
#ifdef _DEBUG
#endif
            return false;
        }


        sendList=(unsigned *)alloca(sizeof(unsigned));




        if (remoteSystemList[remoteSystemIndex].isActive &&
                remoteSystemList[remoteSystemIndex].connectMode!=RemoteSystemStruct::DISCONNECT_ASAP &&
                remoteSystemList[remoteSystemIndex].connectMode!=RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY &&
                remoteSystemList[remoteSystemIndex].connectMode!=RemoteSystemStruct::DISCONNECT_ON_NO_ACK)
        {
            sendList[0]=remoteSystemIndex;
            sendListSize=1;
        }
    }
    else
    {
        sendList=(unsigned *)alloca(sizeof(unsigned)*maximumNumberOfPeers);

        unsigned int idx;
        for ( idx = 0; idx < maximumNumberOfPeers; idx++ )
        {
            if (remoteSystemIndex!=(unsigned int) -1 && idx==remoteSystemIndex)
                continue;

            if ( remoteSystemList[ idx ].isActive && remoteSystemList[ idx ].systemAddress != UNASSIGNED_SYSTEM_ADDRESS )
                sendList[sendListSize++]=idx;
        }
    }

    if (sendListSize==0)
    {



        return false;
    }

    for (sendListIndex=0; sendListIndex < sendListSize; sendListIndex++)
    {
        bool useData = useCallerDataAllocation && callerDataAllocationUsed==false && sendListIndex+1==sendListSize;
        remoteSystemList[sendList[sendListIndex]].reliabilityLayer.Send( data, numberOfBitsToSend, priority, reliability, orderingChannel, useData==false, remoteSystemList[sendList[sendListIndex]].MTUSize, currentTime, receipt );
        if (useData)
            callerDataAllocationUsed=true;

        if (reliability==RELIABLE ||
                reliability==RELIABLE_ORDERED ||
                reliability==RELIABLE_SEQUENCED ||
                reliability==RELIABLE_WITH_ACK_RECEIPT ||
                reliability==RELIABLE_ORDERED_WITH_ACK_RECEIPT
           )
            remoteSystemList[sendList[sendListIndex]].lastReliableSend=(VENet::TimeMS)(currentTime/(VENet::TimeUS)1000);
    }

    return callerDataAllocationUsed;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ResetSendReceipt(void)
{
    sendReceiptSerialMutex.Lock();
    sendReceiptSerial=1;
    sendReceiptSerialMutex.Unlock();
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::OnConnectedPong(VENet::Time sendPingTime, VENet::Time sendPongTime, RemoteSystemStruct *remoteSystem)
{
    VENet::Time ping;
    VENet::Time time = VENet::GetTime();
    if (time > sendPingTime)
        ping = time - sendPingTime;
    else
        ping=0;

    remoteSystem->pingAndClockDifferential[ remoteSystem->pingAndClockDifferentialWriteIndex ].pingTime = ( unsigned short ) ping;
    remoteSystem->pingAndClockDifferential[ remoteSystem->pingAndClockDifferentialWriteIndex ].clockDifferential = sendPongTime - ( time/2 + sendPingTime/2 );

    if ( remoteSystem->lowestPing == (unsigned short)-1 || remoteSystem->lowestPing > (int) ping )
        remoteSystem->lowestPing = (unsigned short) ping;

    if ( ++( remoteSystem->pingAndClockDifferentialWriteIndex ) == (VENet::Time) PING_TIMES_ARRAY_SIZE )
        remoteSystem->pingAndClockDifferentialWriteIndex = 0;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ClearBufferedCommands(void)
{
    BufferedCommandStruct *bcs;

    while ((bcs=bufferedCommands.Pop())!=0)
    {
        if (bcs->data)
            veFree_Ex(bcs->data, _FILE_AND_LINE_ );

        bufferedCommands.Deallocate(bcs, _FILE_AND_LINE_);
    }
    bufferedCommands.Clear(_FILE_AND_LINE_);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ClearBufferedPackets(void)
{
    RecvFromStruct *bcs;

    while ((bcs=bufferedPackets.Pop())!=0)
    {
        bufferedPackets.Deallocate(bcs, _FILE_AND_LINE_);
    }
    bufferedPackets.Clear(_FILE_AND_LINE_);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ClearSocketQueryOutput(void)
{
    socketQueryOutput.Clear(_FILE_AND_LINE_);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::ClearRequestedConnectionList(void)
{
    DataStructures::Queue<RequestedConnectionStruct*> freeQueue;
    requestedConnectionQueueMutex.Lock();
    while (requestedConnectionQueue.Size())
        freeQueue.Push(requestedConnectionQueue.Pop(), _FILE_AND_LINE_ );
    requestedConnectionQueueMutex.Unlock();
    unsigned i;
    for (i=0; i < freeQueue.Size(); i++)
    {
#if LIBCAT_SECURITY==1
        CAT_AUDIT_PRINTF("AUDIT: In ClearRequestedConnectionList(), Deleting freeQueue index %i client_handshake %x\n", i, freeQueue[i]->client_handshake);
        VENet::OP_DELETE(freeQueue[i]->client_handshake,_FILE_AND_LINE_);
#endif
        VENet::OP_DELETE(freeQueue[i], _FILE_AND_LINE_ );
    }
}
inline void VEPeer::AddPacketToProducer(VENet::Packet *p)
{
    packetReturnMutex.Lock();
    packetReturnQueue.Push(p,_FILE_AND_LINE_);
    packetReturnMutex.Unlock();
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
union Buff6AndBuff8
{
    unsigned char buff6[6];
    uint64_t buff8;
};
uint64_t VEPeerInterface::Get64BitUniqueRandomNumber(void)
{
#if   defined(_WIN32)
    uint64_t g=VENet::GetTimeUS();

    VENet::TimeUS lastTime, thisTime;
    int j;
    for (j=0; j < 8; j++)
    {
        lastTime = VENet::GetTimeUS();
        VESleep(1);
        VESleep(0);
        thisTime = VENet::GetTimeUS();
        VENet::TimeUS diff = thisTime-lastTime;
        unsigned int diff4Bits = (unsigned int) (diff & 15);
        diff4Bits <<= 32-4;
        diff4Bits >>= j*4;
        ((char*)&g)[j] ^= diff4Bits;
    }
    return g;

#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec * 1000000;
#endif
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::GenerateGUID(void)
{
    myGuid.g=Get64BitUniqueRandomNumber();

}

namespace VENet
{
bool ProcessOfflineNetworkPacket( SystemAddress systemAddress, const char *data, const int length, VEPeer *rakPeer, VENetSmartPtr<VENetSocket> VENetSocket, bool *isOfflineMessage, VENet::TimeUS timeRead )
{
    (void) timeRead;
    VEPeer::RemoteSystemStruct *remoteSystem;
    VENet::Packet *packet;
    unsigned i;


    char str1[64];
    systemAddress.ToString(false, str1);
    if (rakPeer->IsBanned( str1 ))
    {
        for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
            rakPeer->pluginListNTS[i]->OnDirectSocketReceive(data, length*8, systemAddress);

        VENet::BitStream bs;
        bs.Write((MessageID)ID_CONNECTION_BANNED);
        bs.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
        bs.Write(rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));

        unsigned i;
        for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
            rakPeer->pluginListNTS[i]->OnDirectSocketSend((char*) bs.GetData(), bs.GetNumberOfBitsUsed(), systemAddress);
        SocketLayer::SendTo( VENetSocket->s, (char*) bs.GetData(), bs.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );

        return true;
    }

    if (length <=2)
    {
        *isOfflineMessage=true;
    }
    else if (
        ((unsigned char)data[0] == ID_UNCONNECTED_PING ||
         (unsigned char)data[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS) &&
        length == sizeof(unsigned char) + sizeof(VENet::Time) + sizeof(OFFLINE_MESSAGE_DATA_ID))
    {
        *isOfflineMessage=memcmp(data+sizeof(unsigned char) + sizeof(VENet::Time), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
    }
    else if ((unsigned char)data[0] == ID_UNCONNECTED_PONG && (size_t) length >= sizeof(unsigned char) + sizeof(VENet::TimeMS) + VENetGUID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID))
    {
        *isOfflineMessage=memcmp(data+sizeof(unsigned char) + sizeof(VENet::Time) + VENetGUID::size(), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
    }
    else if (
        (unsigned char)data[0] == ID_OUT_OF_BAND_INTERNAL	&&
        (size_t) length >= sizeof(MessageID) + VENetGUID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID))
    {
        *isOfflineMessage=memcmp(data+sizeof(MessageID) + VENetGUID::size(), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
    }
    else if (
        (
            (unsigned char)data[0] == ID_OPEN_CONNECTION_REPLY_1 ||
            (unsigned char)data[0] == ID_OPEN_CONNECTION_REPLY_2 ||
            (unsigned char)data[0] == ID_OPEN_CONNECTION_REQUEST_1 ||
            (unsigned char)data[0] == ID_OPEN_CONNECTION_REQUEST_2 ||
            (unsigned char)data[0] == ID_CONNECTION_ATTEMPT_FAILED ||
            (unsigned char)data[0] == ID_NO_FREE_INCOMING_CONNECTIONS ||
            (unsigned char)data[0] == ID_CONNECTION_BANNED ||
            (unsigned char)data[0] == ID_ALREADY_CONNECTED ||
            (unsigned char)data[0] == ID_IP_RECENTLY_CONNECTED) &&
        (size_t) length >= sizeof(MessageID) + VENetGUID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID))
    {
        *isOfflineMessage=memcmp(data+sizeof(MessageID), OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
    }
    else if (((unsigned char)data[0] == ID_INCOMPATIBLE_PROTOCOL_VERSION&&
              (size_t) length == sizeof(MessageID)*2 + VENetGUID::size() + sizeof(OFFLINE_MESSAGE_DATA_ID)))
    {
        *isOfflineMessage=memcmp(data+sizeof(MessageID)*2, OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID))==0;
    }
    else
    {
        *isOfflineMessage=false;
    }

    if (*isOfflineMessage)
    {
        for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
            rakPeer->pluginListNTS[i]->OnDirectSocketReceive(data, length*8, systemAddress);

        if ( ( (unsigned char) data[ 0 ] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS
                || (unsigned char)(data)[0] == ID_UNCONNECTED_PING)	&& length == sizeof(unsigned char)+sizeof(VENet::Time)+sizeof(OFFLINE_MESSAGE_DATA_ID) )
        {
            if ( (unsigned char)(data)[0] == ID_UNCONNECTED_PING ||
                    rakPeer->AllowIncomingConnections() )
            {
                VENet::BitStream inBitStream( (unsigned char *) data, length, false );
                inBitStream.IgnoreBits(8);
                VENet::Time sendPingTime;
                inBitStream.Read(sendPingTime);

                VENet::BitStream outBitStream;
                outBitStream.Write((MessageID)ID_UNCONNECTED_PONG);
                outBitStream.Write(sendPingTime);
                outBitStream.Write(rakPeer->myGuid);
                outBitStream.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));

                rakPeer->vePeerMutexes[ VEPeer::offlinePingResponse_Mutex ].Lock();
                outBitStream.Write( (char*)rakPeer->offlinePingResponse.GetData(), rakPeer->offlinePingResponse.GetNumberOfBytesUsed() );
                rakPeer->vePeerMutexes[ VEPeer::offlinePingResponse_Mutex ].Unlock();

                unsigned i;
                for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                    rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*)outBitStream.GetData(), outBitStream.GetNumberOfBytesUsed(), systemAddress);

                SocketLayer::SendTo( VENetSocket->s, (const char*)outBitStream.GetData(), (unsigned int) outBitStream.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );

                packet=rakPeer->AllocPacket(sizeof(MessageID), _FILE_AND_LINE_);
                packet->data[0]=data[0];
                packet->systemAddress = systemAddress;
                packet->guid=UNASSIGNED_VENET_GUID;
                packet->systemAddress.systemIndex = ( SystemIndex ) rakPeer->GetIndexFromSystemAddress( systemAddress, true );
                packet->guid.systemIndex=packet->systemAddress.systemIndex;
                rakPeer->AddPacketToProducer(packet);
            }
        }
        else if ((unsigned char) data[ 0 ] == ID_UNCONNECTED_PONG && (size_t) length >= sizeof(unsigned char)+sizeof(VENet::Time)+VENetGUID::size()+sizeof(OFFLINE_MESSAGE_DATA_ID) && (size_t) length < sizeof(unsigned char)+sizeof(VENet::Time)+VENetGUID::size()+sizeof(OFFLINE_MESSAGE_DATA_ID)+MAX_OFFLINE_DATA_LENGTH)
        {
            packet=rakPeer->AllocPacket((unsigned int) (length-sizeof(OFFLINE_MESSAGE_DATA_ID)-VENetGUID::size()-sizeof(VENet::Time)+sizeof(VENet::TimeMS)), _FILE_AND_LINE_);
            VENet::BitStream bsIn((unsigned char*) data, length, false);
            bsIn.IgnoreBytes(sizeof(unsigned char));
            VENet::Time ping;
            bsIn.Read(ping);
            bsIn.Read(packet->guid);

            VENet::BitStream bsOut((unsigned char*) packet->data, packet->length, false);
            bsOut.ResetWritePointer();
            bsOut.Write((unsigned char)ID_UNCONNECTED_PONG);
            VENet::TimeMS pingMS=(VENet::TimeMS)ping;
            bsOut.Write(pingMS);
            bsOut.WriteAlignedBytes(
                (const unsigned char*)data+sizeof(unsigned char)+sizeof(VENet::Time)+VENetGUID::size()+sizeof(OFFLINE_MESSAGE_DATA_ID),
                length-sizeof(unsigned char)-sizeof(VENet::Time)-VENetGUID::size()-sizeof(OFFLINE_MESSAGE_DATA_ID)
            );

            packet->systemAddress = systemAddress;
            packet->systemAddress.systemIndex = ( SystemIndex ) rakPeer->GetIndexFromSystemAddress( systemAddress, true );
            packet->guid.systemIndex=packet->systemAddress.systemIndex;
            rakPeer->AddPacketToProducer(packet);
        }
        else if ((unsigned char) data[ 0 ] == ID_OUT_OF_BAND_INTERNAL &&
                 (size_t) length < MAX_OFFLINE_DATA_LENGTH+sizeof(OFFLINE_MESSAGE_DATA_ID)+sizeof(MessageID)+VENetGUID::size())
        {
            unsigned int dataLength = (unsigned int) (length-sizeof(OFFLINE_MESSAGE_DATA_ID)-VENetGUID::size()-sizeof(MessageID));
            VEAssert(dataLength<1024);
            packet=rakPeer->AllocPacket(dataLength+1, _FILE_AND_LINE_);
            VEAssert(packet->length<1024);

            VENet::BitStream bs2((unsigned char*) data, length, false);
            bs2.IgnoreBytes(sizeof(MessageID));
            bs2.Read(packet->guid);

            if (data[sizeof(OFFLINE_MESSAGE_DATA_ID)+sizeof(MessageID) + VENetGUID::size()]==ID_ADVERTISE_SYSTEM)
            {
                packet->length--;
                packet->bitSize=BYTES_TO_BITS(packet->length);
                packet->data[0]=ID_ADVERTISE_SYSTEM;
                memcpy(packet->data+1, data+sizeof(OFFLINE_MESSAGE_DATA_ID)+sizeof(MessageID)*2 + VENetGUID::size(), dataLength-1);
            }
            else
            {
                packet->data[0]=ID_OUT_OF_BAND_INTERNAL;
                memcpy(packet->data+1, data+sizeof(OFFLINE_MESSAGE_DATA_ID)+sizeof(MessageID) + VENetGUID::size(), dataLength);
            }

            packet->systemAddress = systemAddress;
            packet->systemAddress.systemIndex = ( SystemIndex ) rakPeer->GetIndexFromSystemAddress( systemAddress, true );
            packet->guid.systemIndex=packet->systemAddress.systemIndex;
            rakPeer->AddPacketToProducer(packet);
        }
        else if ((unsigned char)(data)[0] == (MessageID)ID_OPEN_CONNECTION_REPLY_1)
        {
            for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                rakPeer->pluginListNTS[i]->OnDirectSocketReceive(data, length*8, systemAddress);

            VENet::BitStream bsIn((unsigned char*) data,length,false);
            bsIn.IgnoreBytes(sizeof(MessageID));
            bsIn.IgnoreBytes(sizeof(OFFLINE_MESSAGE_DATA_ID));
            VENetGUID serverGuid;
            bsIn.Read(serverGuid);
            unsigned char serverHasSecurity;
            uint32_t cookie;
            (void) cookie;
            bsIn.Read(serverHasSecurity);
            if (serverHasSecurity)
            {
                bsIn.Read(cookie);
            }

            VENet::BitStream bsOut;
            bsOut.Write((MessageID)ID_OPEN_CONNECTION_REQUEST_2);
            bsOut.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            if (serverHasSecurity)
                bsOut.Write(cookie);

            unsigned i;
            rakPeer->requestedConnectionQueueMutex.Lock();
            for (i=0; i <  rakPeer->requestedConnectionQueue.Size(); i++)
            {
                VEPeer::RequestedConnectionStruct *rcs;
                rcs=rakPeer->requestedConnectionQueue[i];
                if (rcs->systemAddress==systemAddress)
                {
                    if (serverHasSecurity)
                    {
#if LIBCAT_SECURITY==1
                        unsigned char public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
                        bsIn.ReadAlignedBytes(public_key, sizeof(public_key));

                        if (rcs->publicKeyMode==PKM_ACCEPT_ANY_PUBLIC_KEY)
                        {
                            memcpy(rcs->remote_public_key, public_key, cat::EasyHandshake::PUBLIC_KEY_BYTES);
                            if (!rcs->client_handshake->Initialize(public_key) ||
                                    !rcs->client_handshake->GenerateChallenge(rcs->handshakeChallenge))
                            {
                                CAT_AUDIT_PRINTF("AUDIT: Server passed a bad public key with PKM_ACCEPT_ANY_PUBLIC_KEY");
                                return true;
                            }
                        }

                        if (cat::SecureEqual(public_key,
                                             rcs->remote_public_key,
                                             cat::EasyHandshake::PUBLIC_KEY_BYTES)==false)
                        {
                            rakPeer->requestedConnectionQueueMutex.Unlock();
                            CAT_AUDIT_PRINTF("AUDIT: Expected public key does not match what was sent by server -- Reporting back ID_PUBLIC_KEY_MISMATCH to user\n");

                            packet=rakPeer->AllocPacket(sizeof( char ), _FILE_AND_LINE_);
                            packet->data[ 0 ] = ID_PUBLIC_KEY_MISMATCH;
                            packet->bitSize = ( sizeof( char ) * 8);
                            packet->systemAddress = rcs->systemAddress;
                            packet->guid=serverGuid;
                            rakPeer->AddPacketToProducer(packet);
                            return true;
                        }

                        if (rcs->client_handshake==0)
                        {
                            bsOut.Write((unsigned char)0);
                        }
                        else
                        {
                            // Message contains a challenge
                            bsOut.Write((unsigned char)1);
                            // challenge
                            CAT_AUDIT_PRINTF("AUDIT: Sending challenge\n");
                            bsOut.WriteAlignedBytes((const unsigned char*) rcs->handshakeChallenge,cat::EasyHandshake::CHALLENGE_BYTES);
                        }
#else
                        bsOut.Write((unsigned char)0);
#endif
                    }
                    else
                    {
#if LIBCAT_SECURITY==1
                        if (rcs->client_handshake!=0)
                        {
                            rakPeer->requestedConnectionQueueMutex.Unlock();
                            CAT_AUDIT_PRINTF("AUDIT: Security disabled by server but we expected security (indicated by client_handshake not null) so failing!\n");

                            packet=rakPeer->AllocPacket(sizeof( char ), _FILE_AND_LINE_);
                            packet->data[ 0 ] = ID_OUR_SYSTEM_REQUIRES_SECURITY;
                            packet->bitSize = ( sizeof( char ) * 8);
                            packet->systemAddress = rcs->systemAddress;
                            packet->guid=serverGuid;
                            rakPeer->AddPacketToProducer(packet);
                            return true;
                        }
#endif
                    }

                    uint16_t mtu;
                    bsIn.Read(mtu);

                    bsOut.Write(rcs->systemAddress);
                    rakPeer->requestedConnectionQueueMutex.Unlock();
                    bsOut.Write(mtu);
                    bsOut.Write(rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));

                    for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                        rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsOut.GetData(), bsOut.GetNumberOfBitsUsed(), rcs->systemAddress);

                    SocketLayer::SendTo( rakPeer->socketList[rcs->socketIndex]->s, (const char*) bsOut.GetData(), bsOut.GetNumberOfBytesUsed(), rcs->systemAddress, rakPeer->socketList[rcs->socketIndex]->remotePortVENetWasStartedOn_PS3_PSP2, rakPeer->socketList[rcs->socketIndex]->extraSocketOptions, _FILE_AND_LINE_ );

                    return true;
                }
            }
            rakPeer->requestedConnectionQueueMutex.Unlock();
        }
        else if ((unsigned char)(data)[0] == (MessageID)ID_OPEN_CONNECTION_REPLY_2)
        {
            for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                rakPeer->pluginListNTS[i]->OnDirectSocketReceive(data, length*8, systemAddress);

            VENet::BitStream bs((unsigned char*) data,length,false);
            bs.IgnoreBytes(sizeof(MessageID));
            bs.IgnoreBytes(sizeof(OFFLINE_MESSAGE_DATA_ID));
            VENetGUID guid;
            bs.Read(guid);
            SystemAddress bindingAddress;
            bool b = bs.Read(bindingAddress);
            VEAssert(b);
            uint16_t mtu;
            b=bs.Read(mtu);
            VEAssert(b);
            bool doSecurity=false;
            b=bs.Read(doSecurity);
            VEAssert(b);

#if LIBCAT_SECURITY==1
            char answer[cat::EasyHandshake::ANSWER_BYTES];
            CAT_AUDIT_PRINTF("AUDIT: Got ID_OPEN_CONNECTION_REPLY_2 and given doSecurity=%i\n", (int)doSecurity);
            if (doSecurity)
            {
                CAT_AUDIT_PRINTF("AUDIT: Reading cookie and public key\n");
                bs.ReadAlignedBytes((unsigned char*) answer, sizeof(answer));
            }
            cat::ClientEasyHandshake *client_handshake=0;
#endif
            VEPeer::RequestedConnectionStruct *rcs;
            bool unlock=true;
            unsigned i;
            rakPeer->requestedConnectionQueueMutex.Lock();
            for (i=0; i <  rakPeer->requestedConnectionQueue.Size(); i++)
            {
                rcs=rakPeer->requestedConnectionQueue[i];


                if (rcs->systemAddress==systemAddress)
                {
#if LIBCAT_SECURITY==1
                    CAT_AUDIT_PRINTF("AUDIT: System address matches an entry in the requestedConnectionQueue and doSecurity=%i\n", (int)doSecurity);
                    if (doSecurity)
                    {
                        if (rcs->client_handshake==0)
                        {
                            CAT_AUDIT_PRINTF("AUDIT: Server wants security but we didn't set a public key -- Reporting back ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY to user\n");
                            rakPeer->requestedConnectionQueueMutex.Unlock();

                            packet=rakPeer->AllocPacket(2, _FILE_AND_LINE_);
                            packet->data[ 0 ] = ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY;
                            packet->data[ 1 ] = 0;
                            packet->bitSize = ( sizeof( char ) * 8);
                            packet->systemAddress = rcs->systemAddress;
                            packet->guid=guid;
                            rakPeer->AddPacketToProducer(packet);
                            return true;
                        }

                        CAT_AUDIT_PRINTF("AUDIT: Looks good, preparing to send challenge to server! client_handshake = %x\n", client_handshake);
                    }

#endif
                    rakPeer->requestedConnectionQueueMutex.Unlock();
                    unlock=false;

                    VEAssert(rcs->actionToTake==VEPeer::RequestedConnectionStruct::CONNECT);
                    bool thisIPConnectedRecently=false;
                    remoteSystem=rakPeer->GetRemoteSystemFromSystemAddress( systemAddress, true, true );
                    if (remoteSystem==0)
                    {
                        if (rcs->socket.IsNull())
                        {
                            remoteSystem=rakPeer->AssignSystemAddressToRemoteSystemList(systemAddress, VEPeer::RemoteSystemStruct::UNVERIFIED_SENDER, VENetSocket, &thisIPConnectedRecently, bindingAddress, mtu, guid, doSecurity);
                        }
                        else
                        {
                            remoteSystem=rakPeer->AssignSystemAddressToRemoteSystemList(systemAddress, VEPeer::RemoteSystemStruct::UNVERIFIED_SENDER, rcs->socket, &thisIPConnectedRecently, bindingAddress, mtu, guid, doSecurity);
                        }
                    }

                    if (thisIPConnectedRecently==false)
                    {
                        if (remoteSystem)
                        {
#if LIBCAT_SECURITY==1
                            cat::u8 ident[cat::EasyHandshake::IDENTITY_BYTES];
                            bool doIdentity = false;

                            if (rcs->client_handshake)
                            {
                                CAT_AUDIT_PRINTF("AUDIT: Processing answer\n");
                                if (rcs->publicKeyMode == PKM_USE_TWO_WAY_AUTHENTICATION)
                                {
                                    if (!rcs->client_handshake->ProcessAnswerWithIdentity(answer, ident, remoteSystem->reliabilityLayer.GetAuthenticatedEncryption()))
                                    {
                                        CAT_AUDIT_PRINTF("AUDIT: Processing answer -- Invalid Answer\n");
                                        rakPeer->requestedConnectionQueueMutex.Unlock();

                                        return true;
                                    }

                                    doIdentity = true;
                                }
                                else
                                {
                                    if (!rcs->client_handshake->ProcessAnswer(answer, remoteSystem->reliabilityLayer.GetAuthenticatedEncryption()))
                                    {
                                        CAT_AUDIT_PRINTF("AUDIT: Processing answer -- Invalid Answer\n");
                                        rakPeer->requestedConnectionQueueMutex.Unlock();

                                        return true;
                                    }
                                }
                                CAT_AUDIT_PRINTF("AUDIT: Success!\n");

                                VENet::OP_DELETE(rcs->client_handshake,_FILE_AND_LINE_);
                                rcs->client_handshake=0;
                            }
#endif
                            remoteSystem->weInitiatedTheConnection=true;
                            remoteSystem->connectMode=VEPeer::RemoteSystemStruct::REQUESTED_CONNECTION;
                            if (rcs->timeoutTime!=0)
                                remoteSystem->reliabilityLayer.SetTimeoutTime(rcs->timeoutTime);

                            VENet::BitStream temp;
                            temp.Write( (MessageID)ID_CONNECTION_REQUEST);
                            temp.Write(rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));
                            temp.Write(VENet::GetTime());

#if LIBCAT_SECURITY==1
                            temp.Write((unsigned char)(doSecurity ? 1 : 0));

                            if (doSecurity)
                            {
                                unsigned char proof[32];
                                remoteSystem->reliabilityLayer.GetAuthenticatedEncryption()->GenerateProof(proof, sizeof(proof));
                                temp.WriteAlignedBytes(proof, sizeof(proof));

                                temp.Write((unsigned char)(doIdentity ? 1 : 0));

                                if (doIdentity)
                                {
                                    temp.WriteAlignedBytes(ident, sizeof(ident));
                                }
                            }
#else
                            temp.Write((unsigned char)0);
#endif
                            if ( rcs->outgoingPasswordLength > 0 )
                                temp.Write( ( char* ) rcs->outgoingPassword,  rcs->outgoingPasswordLength );

                            rakPeer->SendImmediate((char*)temp.GetData(), temp.GetNumberOfBitsUsed(), IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false, false, timeRead, 0 );
                        }
                        else
                        {
                            packet=rakPeer->AllocPacket(sizeof( char ), _FILE_AND_LINE_);
                            packet->data[ 0 ] = ID_CONNECTION_ATTEMPT_FAILED;
                            packet->bitSize = ( sizeof( char ) * 8);
                            packet->systemAddress = rcs->systemAddress;
                            packet->guid=guid;
                            rakPeer->AddPacketToProducer(packet);
                        }
                    }

                    rakPeer->requestedConnectionQueueMutex.Lock();
                    for (unsigned int k=0; k < rakPeer->requestedConnectionQueue.Size(); k++)
                    {
                        if (rakPeer->requestedConnectionQueue[k]->systemAddress==systemAddress)
                        {
                            rakPeer->requestedConnectionQueue.RemoveAtIndex(k);
                            break;
                        }
                    }
                    rakPeer->requestedConnectionQueueMutex.Unlock();

#if LIBCAT_SECURITY==1
                    CAT_AUDIT_PRINTF("AUDIT: Deleting client_handshake object %x and rcs->client_handshake object %x\n", client_handshake, rcs->client_handshake);
                    VENet::OP_DELETE(client_handshake,_FILE_AND_LINE_);
                    VENet::OP_DELETE(rcs->client_handshake,_FILE_AND_LINE_);
#endif
                    VENet::OP_DELETE(rcs,_FILE_AND_LINE_);

                    break;
                }
            }

            if (unlock)
                rakPeer->requestedConnectionQueueMutex.Unlock();

            return true;

        }
        else if ((unsigned char)(data)[0] == (MessageID)ID_CONNECTION_ATTEMPT_FAILED ||
                 (unsigned char)(data)[0] == (MessageID)ID_NO_FREE_INCOMING_CONNECTIONS ||
                 (unsigned char)(data)[0] == (MessageID)ID_CONNECTION_BANNED ||
                 (unsigned char)(data)[0] == (MessageID)ID_ALREADY_CONNECTED ||
                 (unsigned char)(data)[0] == (MessageID)ID_INVALID_PASSWORD ||
                 (unsigned char)(data)[0] == (MessageID)ID_IP_RECENTLY_CONNECTED ||
                 (unsigned char)(data)[0] == (MessageID)ID_INCOMPATIBLE_PROTOCOL_VERSION)
        {

            VENet::BitStream bs((unsigned char*) data,length,false);
            bs.IgnoreBytes(sizeof(MessageID));
            bs.IgnoreBytes(sizeof(OFFLINE_MESSAGE_DATA_ID));
            if ((unsigned char)(data)[0] == (MessageID)ID_INCOMPATIBLE_PROTOCOL_VERSION)
                bs.IgnoreBytes(sizeof(unsigned char));

            VENetGUID guid;
            bs.Read(guid);

            VEPeer::RequestedConnectionStruct *rcs;
            bool connectionAttemptCancelled=false;
            unsigned i;
            rakPeer->requestedConnectionQueueMutex.Lock();
            for (i=0; i <  rakPeer->requestedConnectionQueue.Size(); i++)
            {
                rcs=rakPeer->requestedConnectionQueue[i];
                if (rcs->actionToTake==VEPeer::RequestedConnectionStruct::CONNECT && rcs->systemAddress==systemAddress)
                {
                    connectionAttemptCancelled=true;
                    rakPeer->requestedConnectionQueue.RemoveAtIndex(i);

#if LIBCAT_SECURITY==1
                    CAT_AUDIT_PRINTF("AUDIT: Connection attempt canceled so deleting rcs->client_handshake object %x\n", rcs->client_handshake);
                    VENet::OP_DELETE(rcs->client_handshake,_FILE_AND_LINE_);
#endif
                    VENet::OP_DELETE(rcs,_FILE_AND_LINE_);
                    break;
                }
            }

            rakPeer->requestedConnectionQueueMutex.Unlock();

            if (connectionAttemptCancelled)
            {
                packet=rakPeer->AllocPacket(sizeof( char ), _FILE_AND_LINE_);
                packet->data[ 0 ] = data[0];
                packet->bitSize = ( sizeof( char ) * 8);
                packet->systemAddress = systemAddress;
                packet->guid=guid;
                rakPeer->AddPacketToProducer(packet);
            }
        }
        else if ((unsigned char)(data)[0] == ID_OPEN_CONNECTION_REQUEST_1 && length > (int) (1+sizeof(OFFLINE_MESSAGE_DATA_ID)))
        {

            char remoteProtocol=data[1+sizeof(OFFLINE_MESSAGE_DATA_ID)];
            if (remoteProtocol!=VENET_PROTOCOL_VERSION)
            {
                VENet::BitStream bs;
                bs.Write((MessageID)ID_INCOMPATIBLE_PROTOCOL_VERSION);
                bs.Write((unsigned char)VENET_PROTOCOL_VERSION);
                bs.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                bs.Write(rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));

                for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                    rakPeer->pluginListNTS[i]->OnDirectSocketSend((char*)bs.GetData(), bs.GetNumberOfBitsUsed(), systemAddress);

                SocketLayer::SendTo( VENetSocket->s, (char*)bs.GetData(), bs.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );
                return true;
            }

            for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                rakPeer->pluginListNTS[i]->OnDirectSocketReceive(data, length*8, systemAddress);

            VENet::BitStream bsOut;
            bsOut.Write((MessageID)ID_OPEN_CONNECTION_REPLY_1);
            bsOut.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            bsOut.Write(rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));
#if LIBCAT_SECURITY==1
            if (rakPeer->_using_security)
            {
                bsOut.Write((unsigned char) 1);
                uint32_t cookie = rakPeer->_cookie_jar->Generate(&systemAddress.address,sizeof(systemAddress.address));
                CAT_AUDIT_PRINTF("AUDIT: Writing cookie %i to %i:%i\n", cookie, systemAddress);
                bsOut.Write(cookie);
                bsOut.WriteAlignedBytes((const unsigned char *) rakPeer->my_public_key,sizeof(rakPeer->my_public_key));
            }
            else
#endif
                bsOut.Write((unsigned char) 0);

            bsOut.Write((uint16_t) (length+UDP_HEADER_SIZE));

            for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsOut.GetData(), bsOut.GetNumberOfBitsUsed(), systemAddress);
            SocketLayer::SendTo( VENetSocket->s, (const char*) bsOut.GetData(), bsOut.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );
        }
        else if ((unsigned char)(data)[0] == ID_OPEN_CONNECTION_REQUEST_2)
        {
            SystemAddress bindingAddress;
            VENetGUID guid;
            VENet::BitStream bsOut;
            VENet::BitStream bs((unsigned char*) data, length, false);
            bs.IgnoreBytes(sizeof(MessageID));
            bs.IgnoreBytes(sizeof(OFFLINE_MESSAGE_DATA_ID));

            bool requiresSecurityOfThisClient=false;
#if LIBCAT_SECURITY==1
            char remoteHandshakeChallenge[cat::EasyHandshake::CHALLENGE_BYTES];

            if (rakPeer->_using_security)
            {
                char str1[64];
                systemAddress.ToString(false, str1);
                requiresSecurityOfThisClient=rakPeer->IsInSecurityExceptionList(str1)==false;

                uint32_t cookie;
                bs.Read(cookie);
                CAT_AUDIT_PRINTF("AUDIT: Got cookie %i from %i:%i\n", cookie, systemAddress);
                if (rakPeer->_cookie_jar->Verify(&systemAddress.address,sizeof(systemAddress.address), cookie)==false)
                {
                    return true;
                }
                CAT_AUDIT_PRINTF("AUDIT: Cookie good!\n");

                unsigned char clientWroteChallenge;
                bs.Read(clientWroteChallenge);

                if (requiresSecurityOfThisClient==true && clientWroteChallenge==0)
                {
                    return true;
                }

                if (clientWroteChallenge)
                {
                    bs.ReadAlignedBytes((unsigned char*) remoteHandshakeChallenge, cat::EasyHandshake::CHALLENGE_BYTES);
#ifdef CAT_AUDIT
                    printf("AUDIT: RECV CHALLENGE ");
                    for (int ii = 0; ii < sizeof(remoteHandshakeChallenge); ++ii)
                    {
                        printf("%02x", (cat::u8)remoteHandshakeChallenge[ii]);
                    }
                    printf("\n");
#endif
                }
            }
#endif
            bs.Read(bindingAddress);
            uint16_t mtu;
            bs.Read(mtu);
            bs.Read(guid);

            VEPeer::RemoteSystemStruct *rssFromSA = rakPeer->GetRemoteSystemFromSystemAddress( systemAddress, true, true );
            bool IPAddrInUse = rssFromSA != 0 && rssFromSA->isActive;
            VEPeer::RemoteSystemStruct *rssFromGuid = rakPeer->GetRemoteSystemFromGUID(guid, true);
            bool GUIDInUse = rssFromGuid != 0 && rssFromGuid->isActive;

            int outcome;
            if (IPAddrInUse & GUIDInUse)
            {
                if (rssFromSA==rssFromGuid && rssFromSA->connectMode==VEPeer::RemoteSystemStruct::UNVERIFIED_SENDER)
                {
                    outcome=1;
                }
                else
                {
                    outcome=2;
                }
            }
            else if (IPAddrInUse==false && GUIDInUse==true)
            {
                outcome=3;
            }
            else if (IPAddrInUse==true && GUIDInUse==false)
            {
                outcome=4;
            }
            else
            {
                outcome=0;
            }

            VENet::BitStream bsAnswer;
            bsAnswer.Write((MessageID)ID_OPEN_CONNECTION_REPLY_2);
            bsAnswer.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
            bsAnswer.Write(rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));
            bsAnswer.Write(systemAddress);
            bsAnswer.Write(mtu);
            bsAnswer.Write(requiresSecurityOfThisClient);

            if (outcome==1)
            {
#if LIBCAT_SECURITY==1
                if (requiresSecurityOfThisClient)
                {
                    CAT_AUDIT_PRINTF("AUDIT: Resending public key and answer from packetloss.  Sending ID_OPEN_CONNECTION_REPLY_2\n");
                    bsAnswer.WriteAlignedBytes((const unsigned char *) rssFromSA->answer,sizeof(rssFromSA->answer));
                }
#endif
                unsigned int i;
                for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                    rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsAnswer.GetData(), bsAnswer.GetNumberOfBitsUsed(), systemAddress);
                SocketLayer::SendTo( VENetSocket->s, (const char*) bsAnswer.GetData(), bsAnswer.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );

                return true;
            }
            else if (outcome!=0)
            {
                bsOut.Write((MessageID)ID_ALREADY_CONNECTED);
                bsOut.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                bsOut.Write(guid);
                for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                    rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsOut.GetData(), bsOut.GetNumberOfBitsUsed(), systemAddress);
                SocketLayer::SendTo( VENetSocket->s, (const char*) bsOut.GetData(), bsOut.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );

                return true;
            }

            if (rakPeer->AllowIncomingConnections()==false)
            {
                bsOut.Write((MessageID)ID_NO_FREE_INCOMING_CONNECTIONS);
                bsOut.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                bsOut.Write(guid);
                for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                    rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsOut.GetData(), bsOut.GetNumberOfBitsUsed(), systemAddress);
                SocketLayer::SendTo( VENetSocket->s, (const char*) bsOut.GetData(), bsOut.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );

                return true;
            }

            bool thisIPConnectedRecently=false;
            rssFromSA = rakPeer->AssignSystemAddressToRemoteSystemList(systemAddress, VEPeer::RemoteSystemStruct::UNVERIFIED_SENDER, VENetSocket, &thisIPConnectedRecently, bindingAddress, mtu, guid, requiresSecurityOfThisClient);

            if (thisIPConnectedRecently==true)
            {
                bsOut.Write((MessageID)ID_IP_RECENTLY_CONNECTED);
                bsOut.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                bsOut.Write(guid);
                for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                    rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsOut.GetData(), bsOut.GetNumberOfBitsUsed(), systemAddress);
                SocketLayer::SendTo( VENetSocket->s, (const char*) bsOut.GetData(), bsOut.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );

                return true;
            }

#if LIBCAT_SECURITY==1
            if (requiresSecurityOfThisClient)
            {
                CAT_AUDIT_PRINTF("AUDIT: Writing public key.  Sending ID_OPEN_CONNECTION_REPLY_2\n");
                if (rakPeer->_server_handshake->ProcessChallenge(remoteHandshakeChallenge, rssFromSA->answer, rssFromSA->reliabilityLayer.GetAuthenticatedEncryption() ))
                {
                    CAT_AUDIT_PRINTF("AUDIT: Challenge good!\n");
                }
                else
                {
                    CAT_AUDIT_PRINTF("AUDIT: Challenge BAD!\n");

                    rakPeer->DereferenceRemoteSystem(systemAddress);
                    return true;
                }

                bsAnswer.WriteAlignedBytes((const unsigned char *) rssFromSA->answer,sizeof(rssFromSA->answer));
            }
#endif
            unsigned int i;
            for (i=0; i < rakPeer->pluginListNTS.Size(); i++)
                rakPeer->pluginListNTS[i]->OnDirectSocketSend((const char*) bsAnswer.GetData(), bsAnswer.GetNumberOfBitsUsed(), systemAddress);
            SocketLayer::SendTo( VENetSocket->s, (const char*) bsAnswer.GetData(), bsAnswer.GetNumberOfBytesUsed(), systemAddress, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, _FILE_AND_LINE_ );
        }
        return true;
    }

    return false;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessNetworkPacket( SystemAddress systemAddress, const char *data, const int length, VEPeer *rakPeer, VENet::TimeUS timeRead, BitStream &updateBitStream )
{
    ProcessNetworkPacket(systemAddress,data,length,rakPeer,rakPeer->socketList[0],timeRead, updateBitStream);
}
void ProcessNetworkPacket( SystemAddress systemAddress, const char *data, const int length, VEPeer *rakPeer, VENetSmartPtr<VENetSocket> VENetSocket, VENet::TimeUS timeRead, BitStream &updateBitStream )
{
#if LIBCAT_SECURITY==1
#ifdef CAT_AUDIT
    printf("AUDIT: RECV ");
    for (int ii = 0; ii < length; ++ii)
    {
        printf("%02x", (cat::u8)data[ii]);
    }
    printf("\n");
#endif
#endif
    VEAssert(systemAddress.GetPort());
    bool isOfflineMessage;
    if (ProcessOfflineNetworkPacket(systemAddress, data, length, rakPeer, VENetSocket, &isOfflineMessage, timeRead))
    {
        return;
    }

    VEPeer::RemoteSystemStruct *remoteSystem;

    remoteSystem = rakPeer->GetRemoteSystemFromSystemAddress( systemAddress, true, true );
    if ( remoteSystem )
    {
        if ( isOfflineMessage==false)
        {
            remoteSystem->reliabilityLayer.HandleSocketReceiveFromConnectedPlayer(
                data, length, systemAddress, rakPeer->pluginListNTS, remoteSystem->MTUSize,
                VENetSocket->s, &rnr, VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, VENetSocket->extraSocketOptions, timeRead, updateBitStream);
        }
    }
}

}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int VEPeer::GenerateSeedFromGuid(void)
{
    return (unsigned int) ((myGuid.g >> 32) ^ myGuid.g);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void VEPeer::DerefAllSockets(void)
{
    unsigned int i;
    for (i=0; i < socketList.Size(); i++)
    {
        socketList[i].SetNull();
    }
    socketList.Clear(false, _FILE_AND_LINE_);
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int VEPeer::GetVENetSocketFromUserConnectionSocketIndex(unsigned int userIndex) const
{
    unsigned int i;
    for (i=0; i < socketList.Size(); i++)
    {
        if (socketList[i]->userConnectionSocketIndex==userIndex)
            return i;
    }
    VEAssert("GetVENetSocketFromUserConnectionSocketIndex failed" && 0);
    return (unsigned int) -1;
}

void VEPeer::ProcessChromePacket(SOCKET s, const char *buffer, int dataSize, const SystemAddress& recvFromAddress, VENet::TimeUS timeRead)
{
    VEAssert(buffer);
    VEAssert(dataSize > 0);
    VEAssert(recvFromAddress.GetPort());

    VEPeer::RecvFromStruct *recvFromStruct;
    recvFromStruct=bufferedPackets.Allocate( _FILE_AND_LINE_ );
    VEAssert(dataSize <= (int)sizeof(recvFromStruct->data));
    memcpy(recvFromStruct->data, buffer, dataSize);
    recvFromStruct->bytesRead=dataSize;
    recvFromStruct->systemAddress=recvFromAddress;
    recvFromStruct->timeRead=timeRead;
    recvFromStruct->s=s;
    recvFromStruct->remotePortVENetWasStartedOn_PS3=0;
    recvFromStruct->extraSocketOptions=0;
    bufferedPackets.Push(recvFromStruct);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::RunRecvFromOnce( SOCKET s, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions )
{
    VEPeer::RecvFromStruct *recvFromStruct;

    recvFromStruct=bufferedPackets.Allocate( _FILE_AND_LINE_ );
    if (recvFromStruct != NULL)
    {
        recvFromStruct->s=s;
        recvFromStruct->remotePortVENetWasStartedOn_PS3=remotePortVENetWasStartedOn_PS3;
        recvFromStruct->extraSocketOptions=extraSocketOptions;
        SocketLayer::RecvFromBlocking(s, this, remotePortVENetWasStartedOn_PS3, extraSocketOptions, recvFromStruct->data, &recvFromStruct->bytesRead, &recvFromStruct->systemAddress, &recvFromStruct->timeRead);

        if (recvFromStruct->bytesRead>0)
        {
            VEAssert(recvFromStruct->systemAddress.GetPort());
            bufferedPackets.Push(recvFromStruct);
            quitAndDataEvents.SetEvent();

            return true;
        }
        else
        {
            bufferedPackets.Deallocate(recvFromStruct, _FILE_AND_LINE_);
        }
    }
    return false;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool VEPeer::RunUpdateCycle(BitStream &updateBitStream )
{
    VEPeer::RemoteSystemStruct * remoteSystem;
    unsigned int activeSystemListIndex;
    Packet *packet;
    BitSize_t bitSize;
    unsigned int byteSize;
    unsigned char *data;
    SystemAddress systemAddress;
    BufferedCommandStruct *bcs;
    bool callerDataAllocationUsed;
    VENetStatistics *rnss;
    VENet::TimeUS timeNS=0;
    VENet::Time timeMS=0;

    if (SocketLayer::GetSocketLayerOverride())
    {
        int len;
        SystemAddress sender;
        char dataOut[ MAXIMUM_MTU_SIZE ];
        do
        {
            len = SocketLayer::GetSocketLayerOverride()->VENetRecvFrom(socketList[0]->s,this,dataOut,&sender,true);
            if (len>0)
                ProcessNetworkPacket( sender, dataOut, len, this, socketList[0], VENet::GetTimeUS(), updateBitStream );
        }
        while (len>0);
    }

    unsigned int socketListIndex;
    VEPeer::RecvFromStruct *recvFromStruct;
    while ((recvFromStruct=bufferedPackets.PopInaccurate())!=0)
    {
        for (socketListIndex=0; socketListIndex < socketList.Size(); socketListIndex++)
        {
            if ((SOCKET) socketList[socketListIndex]->s==recvFromStruct->s)
                break;
        }
        if (socketListIndex!=socketList.Size())
            ProcessNetworkPacket(recvFromStruct->systemAddress, recvFromStruct->data, recvFromStruct->bytesRead, this, socketList[socketListIndex], recvFromStruct->timeRead, updateBitStream);
        bufferedPackets.Deallocate(recvFromStruct, _FILE_AND_LINE_);
    }

    while ((bcs=bufferedCommands.PopInaccurate())!=0)
    {
        if (bcs->command==BufferedCommandStruct::BCS_SEND)
        {
            if (timeNS==0)
            {
                timeNS = VENet::GetTimeUS();
                timeMS = (VENet::TimeMS)(timeNS/(VENet::TimeUS)1000);
            }

            callerDataAllocationUsed=SendImmediate((char*)bcs->data, bcs->numberOfBitsToSend, bcs->priority, bcs->reliability, bcs->orderingChannel, bcs->systemIdentifier, bcs->broadcast, true, timeNS, bcs->receipt);
            if ( callerDataAllocationUsed==false )
                veFree_Ex(bcs->data, _FILE_AND_LINE_ );

            if (bcs->connectionMode!=RemoteSystemStruct::NO_ACTION )
            {
                remoteSystem=GetRemoteSystem( bcs->systemIdentifier, true, true );
                if (remoteSystem)
                    remoteSystem->connectMode=bcs->connectionMode;
            }
        }
        else if (bcs->command==BufferedCommandStruct::BCS_CLOSE_CONNECTION)
        {
            CloseConnectionInternal(bcs->systemIdentifier, false, true, bcs->orderingChannel, bcs->priority);
        }
        else if (bcs->command==BufferedCommandStruct::BCS_CHANGE_SYSTEM_ADDRESS)
        {
            VEPeer::RemoteSystemStruct *rssFromGuid = GetRemoteSystem(bcs->systemIdentifier.veNetGuid,true,true);
            if (rssFromGuid!=0)
            {
                unsigned int existingSystemIndex = GetRemoteSystemIndex(rssFromGuid->systemAddress);
                ReferenceRemoteSystem(bcs->systemIdentifier.systemAddress, existingSystemIndex);
            }
        }
        else if (bcs->command==BufferedCommandStruct::BCS_GET_SOCKET)
        {
            SocketQueryOutput *sqo;
            if (bcs->systemIdentifier.IsUndefined())
            {
                sqo = socketQueryOutput.Allocate( _FILE_AND_LINE_ );
                sqo->sockets=socketList;
                socketQueryOutput.Push(sqo);
            }
            else
            {
                remoteSystem=GetRemoteSystem( bcs->systemIdentifier, true, true );
                sqo = socketQueryOutput.Allocate( _FILE_AND_LINE_ );

                sqo->sockets.Clear(false, _FILE_AND_LINE_);
                if (remoteSystem)
                {
                    sqo->sockets.Push(remoteSystem->VENetSocket, _FILE_AND_LINE_ );
                }
                else
                {
                }
                socketQueryOutput.Push(sqo);
            }

        }

#ifdef _DEBUG
        bcs->data=0;
#endif

        bufferedCommands.Deallocate(bcs, _FILE_AND_LINE_);
    }

    if (requestedConnectionQueue.IsEmpty()==false)
    {
        if (timeNS==0)
        {
            timeNS = VENet::GetTimeUS();
            timeMS = (VENet::TimeMS)(timeNS/(VENet::TimeUS)1000);
        }

        bool condition1, condition2;
        unsigned requestedConnectionQueueIndex=0;
        requestedConnectionQueueMutex.Lock();
        while (requestedConnectionQueueIndex < requestedConnectionQueue.Size())
        {
            RequestedConnectionStruct *rcs;
            rcs = requestedConnectionQueue[requestedConnectionQueueIndex];
            requestedConnectionQueueMutex.Unlock();
            if (rcs->nextRequestTime < timeMS)
            {
                condition1=rcs->requestsMade==rcs->sendConnectionAttemptCount+1;
                condition2=(bool)((rcs->systemAddress==UNASSIGNED_SYSTEM_ADDRESS)==1);
                if (condition1 || condition2)
                {
                    if (rcs->data)
                    {
                        veFree_Ex(rcs->data, _FILE_AND_LINE_ );
                        rcs->data=0;
                    }

                    if (condition1 && !condition2 && rcs->actionToTake==RequestedConnectionStruct::CONNECT)
                    {
                        packet=AllocPacket(sizeof( char ), _FILE_AND_LINE_);
                        packet->data[ 0 ] = ID_CONNECTION_ATTEMPT_FAILED;
                        packet->bitSize = ( sizeof(	 char ) * 8);
                        packet->systemAddress = rcs->systemAddress;
                        AddPacketToProducer(packet);
                    }

#if LIBCAT_SECURITY==1
                    CAT_AUDIT_PRINTF("AUDIT: Connection attempt FAILED so deleting rcs->client_handshake object %x\n", rcs->client_handshake);
                    VENet::OP_DELETE(rcs->client_handshake,_FILE_AND_LINE_);
#endif
                    VENet::OP_DELETE(rcs,_FILE_AND_LINE_);

                    requestedConnectionQueueMutex.Lock();
                    for (unsigned int k=0; k < requestedConnectionQueue.Size(); k++)
                    {
                        if (requestedConnectionQueue[k]==rcs)
                        {
                            requestedConnectionQueue.RemoveAtIndex(k);
                            break;
                        }
                    }
                    requestedConnectionQueueMutex.Unlock();
                }
                else
                {

                    int MTUSizeIndex = rcs->requestsMade / (rcs->sendConnectionAttemptCount/NUM_MTU_SIZES);
                    if (MTUSizeIndex>=NUM_MTU_SIZES)
                        MTUSizeIndex=NUM_MTU_SIZES-1;
                    rcs->requestsMade++;
                    rcs->nextRequestTime=timeMS+rcs->timeBetweenSendConnectionAttemptsMS;

                    VENet::BitStream bitStream;
                    bitStream.Write((MessageID)ID_OPEN_CONNECTION_REQUEST_1);
                    bitStream.WriteAlignedBytes((const unsigned char*) OFFLINE_MESSAGE_DATA_ID, sizeof(OFFLINE_MESSAGE_DATA_ID));
                    bitStream.Write((MessageID)VENET_PROTOCOL_VERSION);
                    bitStream.PadWithZeroToByteLength(mtuSizes[MTUSizeIndex]-UDP_HEADER_SIZE);

                    char str[256];
                    rcs->systemAddress.ToString(true,str);

                    unsigned i;
                    for (i=0; i < pluginListNTS.Size(); i++)
                        pluginListNTS[i]->OnDirectSocketSend((const char*) bitStream.GetData(), bitStream.GetNumberOfBitsUsed(), rcs->systemAddress);

                    if (rcs->socket.IsNull())
                    {
                        rcs->systemAddress.FixForIPVersion(socketList[rcs->socketIndex]->boundAddress);
                        SocketLayer::SetDoNotFragment(socketList[rcs->socketIndex]->s, 1, socketList[rcs->socketIndex]->boundAddress.GetIPPROTO());
                        VENet::Time sendToStart=VENet::GetTime();
                        if (SocketLayer::SendTo( socketList[rcs->socketIndex]->s, (const char*) bitStream.GetData(), bitStream.GetNumberOfBytesUsed(), rcs->systemAddress, socketList[rcs->socketIndex]->remotePortVENetWasStartedOn_PS3_PSP2, socketList[rcs->socketIndex]->extraSocketOptions, _FILE_AND_LINE_ )==-10040)
                        {
                            rcs->requestsMade = (unsigned char) ((MTUSizeIndex + 1) * (rcs->sendConnectionAttemptCount/NUM_MTU_SIZES));
                            rcs->nextRequestTime=timeMS;
                        }
                        else
                        {
                            VENet::Time sendToEnd=VENet::GetTime();
                            if (sendToEnd-sendToStart>100)
                            {
                                int lowestMtuIndex = rcs->sendConnectionAttemptCount/NUM_MTU_SIZES * (NUM_MTU_SIZES - 1);
                                if (lowestMtuIndex > rcs->requestsMade)
                                {
                                    rcs->requestsMade = (unsigned char) lowestMtuIndex;
                                    rcs->nextRequestTime=timeMS;
                                }
                                else
                                    rcs->requestsMade=(unsigned char)(rcs->sendConnectionAttemptCount+1);
                            }
                        }
                        SocketLayer::SetDoNotFragment(socketList[rcs->socketIndex]->s, 0, socketList[rcs->socketIndex]->boundAddress.GetIPPROTO());
                    }
                    else
                    {
                        rcs->systemAddress.FixForIPVersion(rcs->socket->boundAddress);
                        SocketLayer::SetDoNotFragment(rcs->socket->s, 1, rcs->socket->boundAddress.GetIPPROTO());
                        VENet::Time sendToStart=VENet::GetTime();
                        if (SocketLayer::SendTo( rcs->socket->s, (const char*) bitStream.GetData(), bitStream.GetNumberOfBytesUsed(), rcs->systemAddress, socketList[rcs->socketIndex]->remotePortVENetWasStartedOn_PS3_PSP2, socketList[rcs->socketIndex]->extraSocketOptions, _FILE_AND_LINE_  )==-10040)
                        {
                            rcs->requestsMade = (unsigned char) ((MTUSizeIndex + 1) * (rcs->sendConnectionAttemptCount/NUM_MTU_SIZES));
                            rcs->nextRequestTime=timeMS;
                        }
                        else
                        {
                            VENet::Time sendToEnd=VENet::GetTime();
                            if (sendToEnd-sendToStart>100)
                            {
                                int lowestMtuIndex = rcs->sendConnectionAttemptCount/NUM_MTU_SIZES * (NUM_MTU_SIZES - 1);
                                if (lowestMtuIndex > rcs->requestsMade)
                                {
                                    rcs->requestsMade = (unsigned char) lowestMtuIndex;
                                    rcs->nextRequestTime=timeMS;
                                }
                                else
                                    rcs->requestsMade= (unsigned char) rcs->sendConnectionAttemptCount+1;
                            }
                        }
                        SocketLayer::SetDoNotFragment(rcs->socket->s, 0, rcs->socket->boundAddress.GetIPPROTO());
                    }

                    requestedConnectionQueueIndex++;
                }
            }
            else
                requestedConnectionQueueIndex++;

            requestedConnectionQueueMutex.Lock();
        }
        requestedConnectionQueueMutex.Unlock();
    }

    for ( activeSystemListIndex = 0; activeSystemListIndex < activeSystemListSize; ++activeSystemListIndex )
    {
        remoteSystem = activeSystemList[ activeSystemListIndex ];
        systemAddress = remoteSystem->systemAddress;
        VEAssert(systemAddress!=UNASSIGNED_SYSTEM_ADDRESS);
        if (timeNS==0)
        {
            timeNS = VENet::GetTimeUS();
            timeMS = (VENet::TimeMS)(timeNS/(VENet::TimeUS)1000);
        }


        if (timeMS > remoteSystem->lastReliableSend && timeMS-remoteSystem->lastReliableSend > remoteSystem->reliabilityLayer.GetTimeoutTime()/2 && remoteSystem->connectMode==RemoteSystemStruct::CONNECTED)
        {
            VENetStatistics VENetStatistics;
            rnss=remoteSystem->reliabilityLayer.GetStatistics(&VENetStatistics);
            if (rnss->messagesInResendBuffer==0)
            {
                PingInternal( systemAddress, true, RELIABLE );

                remoteSystem->lastReliableSend=timeMS;
            }
        }

        remoteSystem->reliabilityLayer.Update( remoteSystem->VENetSocket->s, systemAddress, remoteSystem->MTUSize, timeNS, maxOutgoingBPS, pluginListNTS, &rnr, remoteSystem->VENetSocket->remotePortVENetWasStartedOn_PS3_PSP2, remoteSystem->VENetSocket->extraSocketOptions, updateBitStream ); // systemAddress only used for the internet simulator test

        if ( remoteSystem->reliabilityLayer.IsDeadConnection() ||
                ((remoteSystem->connectMode==RemoteSystemStruct::DISCONNECT_ASAP || remoteSystem->connectMode==RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY) && remoteSystem->reliabilityLayer.IsOutgoingDataWaiting()==false) ||
                (remoteSystem->connectMode==RemoteSystemStruct::DISCONNECT_ON_NO_ACK && (remoteSystem->reliabilityLayer.AreAcksWaiting()==false || remoteSystem->reliabilityLayer.AckTimeout(timeMS)==true)) ||
                ((
                     (remoteSystem->connectMode==RemoteSystemStruct::REQUESTED_CONNECTION ||
                      remoteSystem->connectMode==RemoteSystemStruct::HANDLING_CONNECTION_REQUEST ||
                      remoteSystem->connectMode==RemoteSystemStruct::UNVERIFIED_SENDER)
                     && timeMS > remoteSystem->connectionTime && timeMS - remoteSystem->connectionTime > 10000))
           )
        {
            if (remoteSystem->connectMode==RemoteSystemStruct::CONNECTED || remoteSystem->connectMode==RemoteSystemStruct::REQUESTED_CONNECTION
                    || remoteSystem->connectMode==RemoteSystemStruct::DISCONNECT_ASAP || remoteSystem->connectMode==RemoteSystemStruct::DISCONNECT_ON_NO_ACK)
            {
                packet=AllocPacket(sizeof( char ), _FILE_AND_LINE_);
                if (remoteSystem->connectMode==RemoteSystemStruct::REQUESTED_CONNECTION)
                    packet->data[ 0 ] = ID_CONNECTION_ATTEMPT_FAILED;
                else if (remoteSystem->connectMode==RemoteSystemStruct::CONNECTED)
                    packet->data[ 0 ] = ID_CONNECTION_LOST;
                else
                    packet->data[ 0 ] = ID_DISCONNECTION_NOTIFICATION;


                packet->guid = remoteSystem->guid;
                packet->systemAddress = systemAddress;
                packet->systemAddress.systemIndex = remoteSystem->remoteSystemIndex;
                packet->guid.systemIndex=packet->systemAddress.systemIndex;

                AddPacketToProducer(packet);
            }

#ifdef _DO_PRINTF
            VENET_DEBUG_PRINTF("Connection dropped for player %i:%i\n", systemAddress);
#endif
            CloseConnectionInternal( systemAddress, false, true, 0, LOW_PRIORITY );
            continue;
        }

        if ( remoteSystem->connectMode==RemoteSystemStruct::CONNECTED && timeMS > remoteSystem->nextPingTime && ( occasionalPing || remoteSystem->lowestPing == (unsigned short)-1 ) )
        {
            remoteSystem->nextPingTime = timeMS + 5000;
            PingInternal( systemAddress, true, UNRELIABLE );

            quitAndDataEvents.SetEvent();
        }

        bitSize = remoteSystem->reliabilityLayer.Receive( &data );

        while ( bitSize > 0 )
        {
            if (data[0]==ID_CONNECTION_ATTEMPT_FAILED)
            {
                VEAssert(0);
                bitSize=0;
                continue;
            }

            byteSize = (unsigned int) BITS_TO_BYTES( bitSize );

            if (remoteSystem->connectMode==RemoteSystemStruct::UNVERIFIED_SENDER)
            {
                if ( (unsigned char)(data)[0] == ID_CONNECTION_REQUEST )
                {
                    ParseConnectionRequestPacket(remoteSystem, systemAddress, (const char*)data, byteSize);
                    veFree_Ex(data, _FILE_AND_LINE_ );
                }
                else
                {
                    CloseConnectionInternal( systemAddress, false, true, 0, LOW_PRIORITY );
#ifdef _DO_PRINTF
                    VENET_DEBUG_PRINTF("Temporarily banning %i:%i for sending nonsense data\n", systemAddress);
#endif

                    char str1[64];
                    systemAddress.ToString(false, str1);
                    AddToBanList(str1, remoteSystem->reliabilityLayer.GetTimeoutTime());


                    veFree_Ex(data, _FILE_AND_LINE_ );
                }
            }
            else
            {
                if ( (unsigned char)(data)[0] == ID_CONNECTION_REQUEST )
                {
                    if (remoteSystem->connectMode==RemoteSystemStruct::REQUESTED_CONNECTION)
                    {
                        ParseConnectionRequestPacket(remoteSystem, systemAddress, (const char*)data, byteSize);
                    }
                    else
                    {

                        VENet::BitStream bs((unsigned char*) data,byteSize,false);
                        bs.IgnoreBytes(sizeof(MessageID));
                        bs.IgnoreBytes(sizeof(OFFLINE_MESSAGE_DATA_ID));
                        bs.IgnoreBytes(VENetGUID::size());
                        VENet::Time incomingTimestamp;
                        bs.Read(incomingTimestamp);

                        OnConnectionRequest( remoteSystem, incomingTimestamp );
                    }
                    veFree_Ex(data, _FILE_AND_LINE_ );
                }
                else if ( (unsigned char) data[ 0 ] == ID_NEW_INCOMING_CONNECTION && byteSize > sizeof(unsigned char)+sizeof(unsigned int)+sizeof(unsigned short)+sizeof(VENet::Time)*2 )
                {
                    if (remoteSystem->connectMode==RemoteSystemStruct::HANDLING_CONNECTION_REQUEST)
                    {
                        remoteSystem->connectMode=RemoteSystemStruct::CONNECTED;
                        PingInternal( systemAddress, true, UNRELIABLE );

                        quitAndDataEvents.SetEvent();

                        VENet::BitStream inBitStream((unsigned char *) data, byteSize, false);
                        SystemAddress bsSystemAddress;

                        inBitStream.IgnoreBits(8);
                        inBitStream.Read(bsSystemAddress);
                        for (unsigned int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
                            inBitStream.Read(remoteSystem->theirInternalSystemAddress[i]);

                        VENet::Time sendPingTime, sendPongTime;
                        inBitStream.Read(sendPingTime);
                        inBitStream.Read(sendPongTime);
                        OnConnectedPong(sendPingTime,sendPongTime,remoteSystem);

                        remoteSystem->myExternalSystemAddress = bsSystemAddress;

                        if (firstExternalID==UNASSIGNED_SYSTEM_ADDRESS)
                        {
                            firstExternalID=bsSystemAddress;
                            firstExternalID.debugPort=ntohs(firstExternalID.address.addr4.sin_port);
                        }

                        packet=AllocPacket(byteSize, data, _FILE_AND_LINE_);
                        packet->bitSize = bitSize;
                        packet->systemAddress = systemAddress;
                        packet->systemAddress.systemIndex = remoteSystem->remoteSystemIndex;
                        packet->guid = remoteSystem->guid;
                        packet->guid.systemIndex=packet->systemAddress.systemIndex;
                        AddPacketToProducer(packet);
                    }
                    else
                    {
                    }
                }
                else if ( (unsigned char) data[ 0 ] == ID_CONNECTED_PONG && byteSize == sizeof(unsigned char)+sizeof(VENet::Time)*2 )
                {
                    VENet::Time sendPingTime, sendPongTime;

                    VENet::BitStream inBitStream( (unsigned char *) data, byteSize, false );
                    inBitStream.IgnoreBits(8);
                    inBitStream.Read(sendPingTime);
                    inBitStream.Read(sendPongTime);

                    OnConnectedPong(sendPingTime,sendPongTime,remoteSystem);

                    veFree_Ex(data, _FILE_AND_LINE_ );
                }
                else if ( (unsigned char)data[0] == ID_CONNECTED_PING && byteSize == sizeof(unsigned char)+sizeof(VENet::Time) )
                {
                    VENet::BitStream inBitStream( (unsigned char *) data, byteSize, false );
                    inBitStream.IgnoreBits(8);
                    VENet::Time sendPingTime;
                    inBitStream.Read(sendPingTime);

                    VENet::BitStream outBitStream;
                    outBitStream.Write((MessageID)ID_CONNECTED_PONG);
                    outBitStream.Write(sendPingTime);
                    outBitStream.Write(VENet::GetTime());
                    SendImmediate( (char*)outBitStream.GetData(), outBitStream.GetNumberOfBitsUsed(), IMMEDIATE_PRIORITY, UNRELIABLE, 0, systemAddress, false, false, VENet::GetTimeUS(), 0 );

                    quitAndDataEvents.SetEvent();

                    veFree_Ex(data, _FILE_AND_LINE_ );
                }
                else if ( (unsigned char) data[ 0 ] == ID_DISCONNECTION_NOTIFICATION )
                {
                    remoteSystem->connectMode=RemoteSystemStruct::DISCONNECT_ON_NO_ACK;
                    veFree_Ex(data, _FILE_AND_LINE_ );

                }
                else if ( (unsigned char)(data)[0] == ID_DETECT_LOST_CONNECTIONS && byteSize == sizeof(unsigned char) )
                {
                    veFree_Ex(data, _FILE_AND_LINE_ );
                }
                else if ( (unsigned char)(data)[0] == ID_INVALID_PASSWORD )
                {
                    if (remoteSystem->connectMode==RemoteSystemStruct::REQUESTED_CONNECTION)
                    {
                        packet=AllocPacket(byteSize, data, _FILE_AND_LINE_);
                        packet->bitSize = bitSize;
                        packet->systemAddress = systemAddress;
                        packet->systemAddress.systemIndex = remoteSystem->remoteSystemIndex;
                        packet->guid = remoteSystem->guid;
                        packet->guid.systemIndex=packet->systemAddress.systemIndex;
                        AddPacketToProducer(packet);

                        remoteSystem->connectMode=RemoteSystemStruct::DISCONNECT_ASAP_SILENTLY;
                    }
                    else
                    {
                        veFree_Ex(data, _FILE_AND_LINE_ );
                    }
                }
                else if ( (unsigned char)(data)[0] == ID_CONNECTION_REQUEST_ACCEPTED )
                {
                    if (byteSize > sizeof(MessageID)+sizeof(unsigned int)+sizeof(unsigned short)+sizeof(SystemIndex)+sizeof(VENet::Time)*2)
                    {
                        bool allowConnection, alreadyConnected;

                        if (remoteSystem->connectMode==RemoteSystemStruct::HANDLING_CONNECTION_REQUEST ||
                                remoteSystem->connectMode==RemoteSystemStruct::REQUESTED_CONNECTION ||
                                allowConnectionResponseIPMigration)
                            allowConnection=true;
                        else
                            allowConnection=false;

                        if (remoteSystem->connectMode==RemoteSystemStruct::HANDLING_CONNECTION_REQUEST)
                            alreadyConnected=true;
                        else
                            alreadyConnected=false;

                        if ( allowConnection )
                        {
                            SystemAddress externalID;
                            SystemIndex systemIndex;

                            VENet::BitStream inBitStream((unsigned char *) data, byteSize, false);
                            inBitStream.IgnoreBits(8);
                            inBitStream.Read(externalID);
                            inBitStream.Read(systemIndex);
                            for (unsigned int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
                                inBitStream.Read(remoteSystem->theirInternalSystemAddress[i]);

                            VENet::Time sendPingTime, sendPongTime;
                            inBitStream.Read(sendPingTime);
                            inBitStream.Read(sendPongTime);
                            OnConnectedPong(sendPingTime, sendPongTime, remoteSystem);

                            remoteSystem->myExternalSystemAddress = externalID;
                            remoteSystem->connectMode=RemoteSystemStruct::CONNECTED;

                            if (firstExternalID==UNASSIGNED_SYSTEM_ADDRESS)
                            {
                                firstExternalID=externalID;
                                firstExternalID.debugPort=ntohs(firstExternalID.address.addr4.sin_port);
                            }

                            packet=AllocPacket(byteSize, data, _FILE_AND_LINE_);
                            packet->bitSize = byteSize * 8;
                            packet->systemAddress = systemAddress;
                            packet->systemAddress.systemIndex = ( SystemIndex ) GetIndexFromSystemAddress( systemAddress, true );
                            packet->guid = remoteSystem->guid;
                            packet->guid.systemIndex=packet->systemAddress.systemIndex;
                            AddPacketToProducer(packet);

                            VENet::BitStream outBitStream;
                            outBitStream.Write((MessageID)ID_NEW_INCOMING_CONNECTION);
                            outBitStream.Write(systemAddress);
                            for (unsigned int i=0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
                                outBitStream.Write(ipList[i]);
                            outBitStream.Write(sendPongTime);
                            outBitStream.Write(VENet::GetTime());

                            VEAssert((outBitStream.GetNumberOfBytesUsed()&15)!=0);
                            SendImmediate( (char*)outBitStream.GetData(), outBitStream.GetNumberOfBitsUsed(), IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false, false, VENet::GetTimeUS(), 0 );

                            if (alreadyConnected==false)
                            {
                                PingInternal( systemAddress, true, UNRELIABLE );
                            }
                        }
                        else
                        {
                            veFree_Ex(data, _FILE_AND_LINE_ );
                        }
                    }
                    else
                    {
                        VEAssert(0);
                        veFree_Ex(data, _FILE_AND_LINE_ );
                    }
                }
                else
                {
                    if ((data[0]>=(MessageID)ID_TIMESTAMP || data[0]==ID_SND_RECEIPT_ACKED || data[0]==ID_SND_RECEIPT_LOSS) &&
                            remoteSystem->isActive
                       )
                    {
                        packet=AllocPacket(byteSize, data, _FILE_AND_LINE_);
                        packet->bitSize = bitSize;
                        packet->systemAddress = systemAddress;
                        packet->systemAddress.systemIndex = remoteSystem->remoteSystemIndex;
                        packet->guid = remoteSystem->guid;
                        packet->guid.systemIndex=packet->systemAddress.systemIndex;
                        AddPacketToProducer(packet);
                    }
                    else
                    {
                        veFree_Ex(data, _FILE_AND_LINE_ );
                    }
                }
            }

            bitSize = remoteSystem->reliabilityLayer.Receive( &data );
        }

    }

    return true;
}

VE_THREAD_DECLARATION(VENet::RecvFromLoop)
{



    RakPeerAndIndex *rpai = ( RakPeerAndIndex * ) arguments;

    VEPeer * rakPeer = rpai->rakPeer;
    SOCKET s = rpai->s;
    unsigned short remotePortVENetWasStartedOn_PS3 = rpai->remotePortVENetWasStartedOn_PS3;
    unsigned int extraSocketOptions = rpai->extraSocketOptions;
    bool blockingSocket = rpai->blockingSocket;
    VENet::OP_DELETE(rpai,_FILE_AND_LINE_);

    rakPeer->isRecvFromLoopThreadActive.Increment();

    while ( rakPeer->endThreads == false )
    {
        if (rakPeer->RunRecvFromOnce(s, remotePortVENetWasStartedOn_PS3, extraSocketOptions)==false &&
                blockingSocket==false)
            VESleep(0);
    }
    rakPeer->isRecvFromLoopThreadActive.Decrement();




    return 0;

}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VE_THREAD_DECLARATION(VENet::UpdateNetworkLoop)
{
    VEPeer * rakPeer = ( VEPeer * ) arguments;

    BitStream updateBitStream( MAXIMUM_MTU_SIZE
#if LIBCAT_SECURITY==1
                               + cat::AuthenticatedEncryption::OVERHEAD_BYTES
#endif
                             );

    rakPeer->isMainLoopThreadActive = true;

    while ( rakPeer->endThreads == false )
    {

        if (rakPeer->userUpdateThreadPtr)
            rakPeer->userUpdateThreadPtr(rakPeer, rakPeer->userUpdateThreadData);

        rakPeer->RunUpdateCycle(updateBitStream);

        rakPeer->quitAndDataEvents.WaitOnEvent(10);
    }

    rakPeer->isMainLoopThreadActive = false;

    return 0;

}

void VEPeer::CallPluginCallbacks(DataStructures::List<PluginInterface2*> &pluginList, Packet *packet)
{
    for (unsigned int i=0; i < pluginList.Size(); i++)
    {
        switch (packet->data[0])
        {
        case ID_DISCONNECTION_NOTIFICATION:
            pluginList[i]->OnClosedConnection(packet->systemAddress, packet->guid, LCR_DISCONNECTION_NOTIFICATION);
            break;
        case ID_CONNECTION_LOST:
            pluginList[i]->OnClosedConnection(packet->systemAddress, packet->guid, LCR_CONNECTION_LOST);
            break;
        case ID_NEW_INCOMING_CONNECTION:
            pluginList[i]->OnNewConnection(packet->systemAddress, packet->guid, true);
            break;
        case ID_CONNECTION_REQUEST_ACCEPTED:
            pluginList[i]->OnNewConnection(packet->systemAddress, packet->guid, false);
            break;
        case ID_CONNECTION_ATTEMPT_FAILED:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_CONNECTION_ATTEMPT_FAILED);
            break;
        case ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY);
            break;
        case ID_OUR_SYSTEM_REQUIRES_SECURITY:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_OUR_SYSTEM_REQUIRES_SECURITY);
            break;
        case ID_PUBLIC_KEY_MISMATCH:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_PUBLIC_KEY_MISMATCH);
            break;
        case ID_ALREADY_CONNECTED:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_ALREADY_CONNECTED);
            break;
        case ID_NO_FREE_INCOMING_CONNECTIONS:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_NO_FREE_INCOMING_CONNECTIONS);
            break;
        case ID_CONNECTION_BANNED:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_CONNECTION_BANNED);
            break;
        case ID_INVALID_PASSWORD:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_INVALID_PASSWORD);
            break;
        case ID_INCOMPATIBLE_PROTOCOL_VERSION:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_INCOMPATIBLE_PROTOCOL);
            break;
        case ID_IP_RECENTLY_CONNECTED:
            pluginList[i]->OnFailedConnectionAttempt(packet, FCAR_IP_RECENTLY_CONNECTED);
            break;
        }
    }
}


#ifdef _MSC_VER
#pragma warning( pop )
#endif
