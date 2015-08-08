#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TCPInterface==1

#pragma once

#include "VEMemoryOverride.h"
#include "DS_List.h"
#include "VENetTypes.h"
#include "Export.h"
#include "VEThread.h"
#include "DS_Queue.h"
#include "SimpleMutex.h"
#include "VENetDefines.h"
#include "SocketIncludes.h"
#include "DS_ByteQueue.h"
#include "DS_ThreadsafeAllocatingQueue.h"
#include "LocklessTypes.h"

#if OPEN_SSL_CLIENT_SUPPORT==1
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

namespace VENet
{
struct RemoteClient;

class VE_DLL_EXPORT TCPInterface
{
public:
    STATIC_FACTORY_DECLARATIONS(TCPInterface)

    TCPInterface();
    virtual ~TCPInterface();

    bool Start(unsigned short port, unsigned short maxIncomingConnections, unsigned short maxConnections=0, int _threadPriority=-99999, unsigned short socketFamily=AF_INET);

    void Stop(void);

    SystemAddress Connect(const char* host, unsigned short remotePort, bool block=true, unsigned short socketFamily=AF_INET);

#if OPEN_SSL_CLIENT_SUPPORT==1
    void StartSSLClient(SystemAddress systemAddress);

    bool IsSSLActive(SystemAddress systemAddress);
#endif

    void Send( const char *data, unsigned int length, const SystemAddress &systemAddress, bool broadcast );

    bool SendList( const char **data, const unsigned int  *lengths, const int numParameters, const SystemAddress &systemAddress, bool broadcast );

    unsigned int GetOutgoingDataBufferSize(SystemAddress systemAddress) const;

    virtual bool ReceiveHasPackets( void );

    Packet* Receive( void );

    void CloseConnection( SystemAddress systemAddress );

    void DeallocatePacket( Packet *packet );

    void GetConnectionList( SystemAddress *remoteSystems, unsigned short *numberOfSystems ) const;

    unsigned short GetConnectionCount(void) const;

    SystemAddress HasCompletedConnectionAttempt(void);

    SystemAddress HasFailedConnectionAttempt(void);

    SystemAddress HasNewIncomingConnection(void);

    SystemAddress HasLostConnection(void);

    Packet* AllocatePacket(unsigned dataSize);

    virtual void PushBackPacket( Packet *packet, bool pushAtHead );

    static const char *Base64Map(void)
    {
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    }

    static int Base64Encoding(const char *inputData, int dataLength, char *outputData);

    bool WasStarted(void) const;

protected:

    VENet::LocklessUint32_t isStarted, threadRunning;
    SOCKET listenSocket;

    DataStructures::Queue<Packet*> headPush, tailPush;
    RemoteClient* remoteClients;
    int remoteClientsLength;

    DataStructures::ThreadsafeAllocatingQueue<Packet> incomingMessages;
    DataStructures::ThreadsafeAllocatingQueue<SystemAddress> newIncomingConnections, lostConnections, requestedCloseConnections;
    DataStructures::ThreadsafeAllocatingQueue<RemoteClient*> newRemoteClients;
    SimpleMutex completedConnectionAttemptMutex, failedConnectionAttemptMutex;
    DataStructures::Queue<SystemAddress> completedConnectionAttempts, failedConnectionAttempts;

    int threadPriority;

    DataStructures::List<SOCKET> blockingSocketList;
    SimpleMutex blockingSocketListMutex;





    friend VE_THREAD_DECLARATION(UpdateTCPInterfaceLoop);
    friend VE_THREAD_DECLARATION(ConnectionAttemptLoop);

    SOCKET SocketConnect(const char* host, unsigned short remotePort, unsigned short socketFamily);

    struct ThisPtrPlusSysAddr
    {
        TCPInterface *tcpInterface;
        SystemAddress systemAddress;
        bool useSSL;
        unsigned short socketFamily;
    };

#if OPEN_SSL_CLIENT_SUPPORT==1
    SSL_CTX* ctx;
    SSL_METHOD *meth;
    DataStructures::ThreadsafeAllocatingQueue<SystemAddress> startSSL;
    DataStructures::List<SystemAddress> activeSSLConnections;
    SimpleMutex sharedSslMutex;
#endif
};

struct RemoteClient
{
    RemoteClient()
    {
#if OPEN_SSL_CLIENT_SUPPORT==1
        ssl=0;
#endif
        isActive=false;
        socket=INVALID_SOCKET;
    }
    SOCKET socket;
    SystemAddress systemAddress;
    DataStructures::ByteQueue outgoingData;
    bool isActive;
    SimpleMutex outgoingDataMutex;
    SimpleMutex isActiveMutex;

#if OPEN_SSL_CLIENT_SUPPORT==1
    SSL*     ssl;
    void InitSSL(SSL_CTX* ctx, SSL_METHOD *meth);
    void DisconnectSSL(void);
    void FreeSSL(void);
    int Send(const char *data, unsigned int length);
    int Recv(char *data, const int dataSize);
#else
    int Send(const char *data, unsigned int length);
    int Recv(char *data, const int dataSize);
#endif
    void Reset(void)
    {
        outgoingDataMutex.Lock();
        outgoingData.Clear(_FILE_AND_LINE_);
        outgoingDataMutex.Unlock();
    }
    void SetActive(bool a);
    void SendOrBuffer(const char **data, const unsigned int *lengths, const int numParameters);
};

}

#endif

