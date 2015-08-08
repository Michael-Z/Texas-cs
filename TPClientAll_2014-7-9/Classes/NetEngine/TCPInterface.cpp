#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TCPInterface==1

#include "TCPInterface.h"
#ifdef _WIN32
typedef int socklen_t;


#else
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include <string.h>
#include "VEAssert.h"
#include <stdio.h>
#include "VEAssert.h"
#include "VESleep.h"
#include "StringCompressor.h"
#include "StringTable.h"
#include "Itoa.h"
#include "SocketLayer.h"
#include "SocketDefines.h"

#ifdef _DO_PRINTF
#endif

#ifdef _WIN32
#include "WSAStartupSingleton.h"
#endif
namespace VENet
{
VE_THREAD_DECLARATION(UpdateTCPInterfaceLoop);
VE_THREAD_DECLARATION(ConnectionAttemptLoop);
}
#ifdef _MSC_VER
#pragma warning( push )
#endif

using namespace VENet;

STATIC_FACTORY_DEFINITIONS(TCPInterface,TCPInterface);

TCPInterface::TCPInterface()
{
    listenSocket=INVALID_SOCKET;
    remoteClients=0;
    remoteClientsLength=0;

    StringCompressor::AddReference();
    VENet::StringTable::AddReference();

#if OPEN_SSL_CLIENT_SUPPORT==1
    ctx=0;
    meth=0;
#endif

#ifdef _WIN32
    WSAStartupSingleton::AddRef();
#endif
}
TCPInterface::~TCPInterface()
{
    Stop();
#ifdef _WIN32
    WSAStartupSingleton::Deref();
#endif

    VENet::OP_DELETE_ARRAY(remoteClients,_FILE_AND_LINE_);

    StringCompressor::RemoveReference();
    VENet::StringTable::RemoveReference();
}
bool TCPInterface::Start(unsigned short port, unsigned short maxIncomingConnections, unsigned short maxConnections, int _threadPriority, unsigned short socketFamily)
{
#ifdef __native_client__
    return false;
#else
    (void) socketFamily;

    if (isStarted.GetValue()>0)
        return false;

    threadPriority=_threadPriority;

    if (threadPriority==-99999)
    {


#if   defined(_WIN32)
        threadPriority=0;


#else
    threadPriority=1000;
#endif
    }

    isStarted.Increment();
    if (maxConnections==0)
        maxConnections=maxIncomingConnections;
    if (maxConnections==0)
        maxConnections=1;
    remoteClientsLength=maxConnections;
    remoteClients=VENet::OP_NEW_ARRAY<RemoteClient>(maxConnections,_FILE_AND_LINE_);


#if VENET_SUPPORT_IPV6!=1
    if (maxIncomingConnections>0)
    {
        listenSocket = socket__(AF_INET, SOCK_STREAM, 0);
        if ((int)listenSocket ==-1)
            return false;

        struct sockaddr_in serverAddress;
        memset(&serverAddress,0,sizeof(sockaddr_in));
        serverAddress.sin_family = AF_INET;

        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        serverAddress.sin_port = htons(port);

        if (bind__(listenSocket,(struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0)
            return false;

        listen__(listenSocket, maxIncomingConnections);
    }
#else
    listenSocket=INVALID_SOCKET;
    if (maxIncomingConnections>0)
    {
        struct addrinfo hints;
        memset(&hints, 0, sizeof (addrinfo));
        hints.ai_family = socketFamily;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        struct addrinfo *servinfo=0, *aip;
        char portStr[32];
        Itoa(port,portStr,10);

        getaddrinfo(0, portStr, &hints, &servinfo);
        for (aip = servinfo; aip != NULL; aip = aip->ai_next)
        {
            listenSocket = socket__(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
            if (listenSocket != INVALID_SOCKET)
            {
                int ret = bind__( listenSocket, aip->ai_addr, (int) aip->ai_addrlen );
                if (ret>=0)
                {
                    break;
                }
                else
                {
                    closesocket__(listenSocket);
                    listenSocket=INVALID_SOCKET;
                }
            }
        }

        if (listenSocket==INVALID_SOCKET)
            return false;

        listen__(listenSocket, maxIncomingConnections);
    }
#endif
    int errorCode;
    errorCode = VENet::RakThread::Create(UpdateTCPInterfaceLoop, this, threadPriority);


    if (errorCode!=0)
        return false;

    while (threadRunning.GetValue()==0)
        VESleep(0);

    return true;
#endif
}
void TCPInterface::Stop(void)
{
#ifndef __native_client__
    if (isStarted.GetValue()==0)
        return;

    unsigned i;
#if OPEN_SSL_CLIENT_SUPPORT==1
    for (i=0; i < remoteClientsLength; i++)
        remoteClients[i].DisconnectSSL();
#endif

    isStarted.Decrement();

    if (listenSocket!=INVALID_SOCKET)
    {
#ifdef _WIN32
        shutdown__(listenSocket, SD_BOTH);

#else
        shutdown__(listenSocket, SHUT_RDWR);
#endif
        closesocket__(listenSocket);
    }

    blockingSocketListMutex.Lock();
    for (i=0; i < blockingSocketList.Size(); i++)
    {
        closesocket__(blockingSocketList[i]);
    }
    blockingSocketListMutex.Unlock();

    while ( threadRunning.GetValue()>0 )
        VESleep(15);

    VESleep(100);
    listenSocket=INVALID_SOCKET;

    for (i=0; i < (unsigned int) remoteClientsLength; i++)
    {
        closesocket__(remoteClients[i].socket);
#if OPEN_SSL_CLIENT_SUPPORT==1
        remoteClients[i].FreeSSL();
#endif
    }
    remoteClientsLength=0;
    VENet::OP_DELETE_ARRAY(remoteClients,_FILE_AND_LINE_);
    remoteClients=0;

    incomingMessages.Clear(_FILE_AND_LINE_);
    newIncomingConnections.Clear(_FILE_AND_LINE_);
    newRemoteClients.Clear(_FILE_AND_LINE_);
    lostConnections.Clear(_FILE_AND_LINE_);
    requestedCloseConnections.Clear(_FILE_AND_LINE_);
    failedConnectionAttempts.Clear(_FILE_AND_LINE_);
    completedConnectionAttempts.Clear(_FILE_AND_LINE_);
    failedConnectionAttempts.Clear(_FILE_AND_LINE_);
    for (i=0; i < headPush.Size(); i++)
        DeallocatePacket(headPush[i]);
    headPush.Clear(_FILE_AND_LINE_);
    for (i=0; i < tailPush.Size(); i++)
        DeallocatePacket(tailPush[i]);
    tailPush.Clear(_FILE_AND_LINE_);

#if OPEN_SSL_CLIENT_SUPPORT==1
    SSL_CTX_free (ctx);
    startSSL.Clear(_FILE_AND_LINE_);
    activeSSLConnections.Clear(false, _FILE_AND_LINE_);
#endif





#endif
}
SystemAddress TCPInterface::Connect(const char* host, unsigned short remotePort, bool block, unsigned short socketFamily)
{
    if (threadRunning.GetValue()==0)
        return UNASSIGNED_SYSTEM_ADDRESS;

    int newRemoteClientIndex=-1;
    for (newRemoteClientIndex=0; newRemoteClientIndex < remoteClientsLength; newRemoteClientIndex++)
    {
        remoteClients[newRemoteClientIndex].isActiveMutex.Lock();
        if (remoteClients[newRemoteClientIndex].isActive==false)
        {
            remoteClients[newRemoteClientIndex].SetActive(true);
            remoteClients[newRemoteClientIndex].isActiveMutex.Unlock();
            break;
        }
        remoteClients[newRemoteClientIndex].isActiveMutex.Unlock();
    }
    if (newRemoteClientIndex==-1)
        return UNASSIGNED_SYSTEM_ADDRESS;

    if (block)
    {
        SystemAddress systemAddress;
        systemAddress.FromString(host);
        systemAddress.SetPort(remotePort);
        systemAddress.systemIndex=(SystemIndex) newRemoteClientIndex;
        char buffout[128];
        systemAddress.ToString(false,buffout);

        SOCKET sockfd = SocketConnect(buffout, remotePort, socketFamily);
        if (sockfd==INVALID_SOCKET)
        {
            remoteClients[newRemoteClientIndex].isActiveMutex.Lock();
            remoteClients[newRemoteClientIndex].SetActive(false);
            remoteClients[newRemoteClientIndex].isActiveMutex.Unlock();

            failedConnectionAttemptMutex.Lock();
            failedConnectionAttempts.Push(systemAddress, _FILE_AND_LINE_ );
            failedConnectionAttemptMutex.Unlock();

            return UNASSIGNED_SYSTEM_ADDRESS;
        }

        remoteClients[newRemoteClientIndex].socket=sockfd;
        remoteClients[newRemoteClientIndex].systemAddress=systemAddress;

        completedConnectionAttemptMutex.Lock();
        completedConnectionAttempts.Push(remoteClients[newRemoteClientIndex].systemAddress, _FILE_AND_LINE_ );
        completedConnectionAttemptMutex.Unlock();

        return remoteClients[newRemoteClientIndex].systemAddress;
    }
    else
    {
        ThisPtrPlusSysAddr *s = VENet::OP_NEW<ThisPtrPlusSysAddr>( _FILE_AND_LINE_ );
        s->systemAddress.FromStringExplicitPort(host,remotePort);
        s->systemAddress.systemIndex=(SystemIndex) newRemoteClientIndex;
        s->tcpInterface=this;
        s->socketFamily=socketFamily;

        int errorCode;




        errorCode = VENet::RakThread::Create(ConnectionAttemptLoop, s, threadPriority);

        if (errorCode!=0)
        {
            VENet::OP_DELETE(s, _FILE_AND_LINE_);
            failedConnectionAttempts.Push(s->systemAddress, _FILE_AND_LINE_ );
        }
        return UNASSIGNED_SYSTEM_ADDRESS;
    }
}
#if OPEN_SSL_CLIENT_SUPPORT==1
void TCPInterface::StartSSLClient(SystemAddress systemAddress)
{
    if (ctx==0)
    {
        sharedSslMutex.Lock();
        SSLeay_add_ssl_algorithms();
        meth = (SSL_METHOD*) SSLv23_client_method();
        SSL_load_error_strings();
        ctx = SSL_CTX_new (meth);
        VEAssert(ctx!=0);
        sharedSslMutex.Unlock();
    }

    SystemAddress *id = startSSL.Allocate( _FILE_AND_LINE_ );
    *id=systemAddress;
    startSSL.Push(id);
    unsigned index = activeSSLConnections.GetIndexOf(systemAddress);
    if (index==(unsigned)-1)
        activeSSLConnections.Insert(systemAddress,_FILE_AND_LINE_);
}
bool TCPInterface::IsSSLActive(SystemAddress systemAddress)
{
    return activeSSLConnections.GetIndexOf(systemAddress)!=-1;
}
#endif
void TCPInterface::Send( const char *data, unsigned length, const SystemAddress &systemAddress, bool broadcast )
{
    SendList( &data, &length, 1, systemAddress,broadcast );
}
bool TCPInterface::SendList( const char **data, const unsigned int *lengths, const int numParameters, const SystemAddress &systemAddress, bool broadcast )
{
    if (isStarted.GetValue()==0)
        return false;
    if (data==0)
        return false;
    if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS && broadcast==false)
        return false;
    unsigned int totalLength=0;
    int i;
    for (i=0; i < numParameters; i++)
    {
        if (lengths[i]>0)
            totalLength+=lengths[i];
    }
    if (totalLength==0)
        return false;

    if (broadcast)
    {
        for (i=0; i < remoteClientsLength; i++)
        {
            if (remoteClients[i].systemAddress!=systemAddress)
            {
                remoteClients[i].SendOrBuffer(data, lengths, numParameters);
            }
        }
    }
    else
    {
        if (systemAddress.systemIndex<remoteClientsLength &&
                remoteClients[systemAddress.systemIndex].systemAddress==systemAddress)
        {
            remoteClients[systemAddress.systemIndex].SendOrBuffer(data, lengths, numParameters);
        }
        else
        {
            for (i=0; i < remoteClientsLength; i++)
            {
                if (remoteClients[i].systemAddress==systemAddress )
                {
                    remoteClients[i].SendOrBuffer(data, lengths, numParameters);
                }
            }
        }
    }


    return true;
}
bool TCPInterface::ReceiveHasPackets( void )
{
    return headPush.IsEmpty()==false || incomingMessages.IsEmpty()==false || tailPush.IsEmpty()==false;
}
Packet* TCPInterface::Receive( void )
{
    if (isStarted.GetValue()==0)
        return 0;
    if (headPush.IsEmpty()==false)
        return headPush.Pop();
    Packet *p = incomingMessages.PopInaccurate();
    if (p)
        return p;
    if (tailPush.IsEmpty()==false)
        return tailPush.Pop();
    return 0;
}
void TCPInterface::CloseConnection( SystemAddress systemAddress )
{
    if (isStarted.GetValue()==0)
        return;
    if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS)
        return;

    if (systemAddress.systemIndex<remoteClientsLength && remoteClients[systemAddress.systemIndex].systemAddress==systemAddress)
    {
        remoteClients[systemAddress.systemIndex].isActiveMutex.Lock();
        remoteClients[systemAddress.systemIndex].SetActive(false);
        remoteClients[systemAddress.systemIndex].isActiveMutex.Unlock();
    }
    else
    {
        for (int i=0; i < remoteClientsLength; i++)
        {
            remoteClients[i].isActiveMutex.Lock();
            if (remoteClients[i].isActive && remoteClients[i].systemAddress==systemAddress)
            {
                remoteClients[systemAddress.systemIndex].SetActive(false);
                remoteClients[i].isActiveMutex.Unlock();
                break;
            }
            remoteClients[i].isActiveMutex.Unlock();
        }
    }


#if OPEN_SSL_CLIENT_SUPPORT==1
    unsigned index = activeSSLConnections.GetIndexOf(systemAddress);
    if (index!=(unsigned)-1)
        activeSSLConnections.RemoveAtIndex(index);
#endif
}
void TCPInterface::DeallocatePacket( Packet *packet )
{
    if (packet==0)
        return;
    if (packet->deleteData)
    {
        veFree_Ex(packet->data, _FILE_AND_LINE_ );
        incomingMessages.Deallocate(packet, _FILE_AND_LINE_);
    }
    else
    {
        veFree_Ex(packet->data, _FILE_AND_LINE_ );
        VENet::OP_DELETE(packet, _FILE_AND_LINE_);
    }
}
Packet* TCPInterface::AllocatePacket(unsigned dataSize)
{
    Packet*p = VENet::OP_NEW<Packet>(_FILE_AND_LINE_);
    p->data=(unsigned char*) rakMalloc_Ex(dataSize,_FILE_AND_LINE_);
    p->length=dataSize;
    p->bitSize=BYTES_TO_BITS(dataSize);
    p->deleteData=false;
    p->guid=UNASSIGNED_VENET_GUID;
    p->systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
    p->systemAddress.systemIndex=(SystemIndex)-1;
    return p;
}
void TCPInterface::PushBackPacket( Packet *packet, bool pushAtHead )
{
    if (pushAtHead)
        headPush.Push(packet, _FILE_AND_LINE_ );
    else
        tailPush.Push(packet, _FILE_AND_LINE_ );
}
bool TCPInterface::WasStarted(void) const
{
    return threadRunning.GetValue()>0;
}
int TCPInterface::Base64Encoding(const char *inputData, int dataLength, char *outputData)
{
    int outputOffset, charCount;
    int write3Count;
    outputOffset=0;
    charCount=0;
    int j;

    write3Count=dataLength/3;
    for (j=0; j < write3Count; j++)
    {
        outputData[outputOffset++]=Base64Map()[inputData[j*3+0] >> 2];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]=Base64Map()[((inputData[j*3+0] << 4) | (inputData[j*3+1] >> 4)) & 63];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]=Base64Map()[((inputData[j*3+1] << 2) | (inputData[j*3+2] >> 6)) & 63]; // Third 6 bits
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]=Base64Map()[inputData[j*3+2] & 63];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }
    }

    if (dataLength % 3==1)
    {
        outputData[outputOffset++]=Base64Map()[inputData[j*3+0] >> 2];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]=Base64Map()[((inputData[j*3+0] << 4) | (inputData[j*3+1] >> 4)) & 63];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]='=';
        outputData[outputOffset++]='=';
    }
    else if (dataLength % 3==2)
    {
        outputData[outputOffset++]=Base64Map()[inputData[j*3+0] >> 2];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]=Base64Map()[((inputData[j*3+0] << 4) | (inputData[j*3+1] >> 4)) & 63];
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]=Base64Map()[(inputData[j*3+1] << 2) & 63]; // Third 6 bits
        if ((++charCount % 76)==0)
        {
            outputData[outputOffset++]='\r';
            outputData[outputOffset++]='\n';
            charCount=0;
        }

        outputData[outputOffset++]='=';
    }

    outputData[outputOffset++]='\r';
    outputData[outputOffset++]='\n';
    outputData[outputOffset]=0;

    return outputOffset;
}
SystemAddress TCPInterface::HasCompletedConnectionAttempt(void)
{
    SystemAddress sysAddr=UNASSIGNED_SYSTEM_ADDRESS;
    completedConnectionAttemptMutex.Lock();
    if (completedConnectionAttempts.IsEmpty()==false)
        sysAddr=completedConnectionAttempts.Pop();
    completedConnectionAttemptMutex.Unlock();
    return sysAddr;
}
SystemAddress TCPInterface::HasFailedConnectionAttempt(void)
{
    SystemAddress sysAddr=UNASSIGNED_SYSTEM_ADDRESS;
    failedConnectionAttemptMutex.Lock();
    if (failedConnectionAttempts.IsEmpty()==false)
        sysAddr=failedConnectionAttempts.Pop();
    failedConnectionAttemptMutex.Unlock();
    return sysAddr;
}
SystemAddress TCPInterface::HasNewIncomingConnection(void)
{
    SystemAddress *out, out2;
    out = newIncomingConnections.PopInaccurate();
    if (out)
    {
        out2=*out;
        newIncomingConnections.Deallocate(out, _FILE_AND_LINE_);
        return *out;
    }
    else
    {
        return UNASSIGNED_SYSTEM_ADDRESS;
    }
}
SystemAddress TCPInterface::HasLostConnection(void)
{
    SystemAddress *out, out2;
    out = lostConnections.PopInaccurate();
    if (out)
    {
        out2=*out;
        lostConnections.Deallocate(out, _FILE_AND_LINE_);
        return *out;
    }
    else
    {
        return UNASSIGNED_SYSTEM_ADDRESS;
    }
}
void TCPInterface::GetConnectionList( SystemAddress *remoteSystems, unsigned short *numberOfSystems ) const
{
    unsigned short systemCount=0;
    unsigned short maxToWrite=*numberOfSystems;
    for (int i=0; i < remoteClientsLength; i++)
    {
        if (remoteClients[i].isActive)
        {
            if (systemCount < maxToWrite)
                remoteSystems[systemCount]=remoteClients[i].systemAddress;
            systemCount++;
        }
    }
    *numberOfSystems=systemCount;
}
unsigned short TCPInterface::GetConnectionCount(void) const
{
    unsigned short systemCount=0;
    for (int i=0; i < remoteClientsLength; i++)
    {
        if (remoteClients[i].isActive)
            systemCount++;
    }
    return systemCount;
}

unsigned int TCPInterface::GetOutgoingDataBufferSize(SystemAddress systemAddress) const
{
    unsigned bytesWritten=0;
    if (systemAddress.systemIndex<remoteClientsLength &&
            remoteClients[systemAddress.systemIndex].isActive &&
            remoteClients[systemAddress.systemIndex].systemAddress==systemAddress)
    {
        remoteClients[systemAddress.systemIndex].outgoingDataMutex.Lock();
        bytesWritten=remoteClients[systemAddress.systemIndex].outgoingData.GetBytesWritten();
        remoteClients[systemAddress.systemIndex].outgoingDataMutex.Unlock();
        return bytesWritten;
    }

    for (int i=0; i < remoteClientsLength; i++)
    {
        if (remoteClients[i].isActive && remoteClients[i].systemAddress==systemAddress)
        {
            remoteClients[i].outgoingDataMutex.Lock();
            bytesWritten+=remoteClients[i].outgoingData.GetBytesWritten();
            remoteClients[i].outgoingDataMutex.Unlock();
        }
    }
    return bytesWritten;
}
SOCKET TCPInterface::SocketConnect(const char* host, unsigned short remotePort, unsigned short socketFamily)
{
#ifdef __native_client__
    return INVALID_SOCKET;
#else
    int connectResult;
    (void) socketFamily;

#if VENET_SUPPORT_IPV6!=1
    sockaddr_in serverAddress;


    struct hostent * server;
    server = gethostbyname(host);
    if (server == NULL)
        return INVALID_SOCKET;


    SOCKET sockfd = socket__(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return INVALID_SOCKET;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( remotePort );

    int sock_opt=1024*256;
    setsockopt__(sockfd, SOL_SOCKET, SO_RCVBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) );


    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)server->h_addr, server->h_length);






    blockingSocketListMutex.Lock();
    blockingSocketList.Insert(sockfd, _FILE_AND_LINE_);
    blockingSocketListMutex.Unlock();

    connectResult = connect__( sockfd, ( struct sockaddr * ) &serverAddress, sizeof( struct sockaddr ) );

#else


    struct addrinfo hints, *res;
    int sockfd;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = socketFamily;
    hints.ai_socktype = SOCK_STREAM;
    char portStr[32];
    Itoa(remotePort,portStr,10);
    getaddrinfo(host, portStr, &hints, &res);
    sockfd = socket__(res->ai_family, res->ai_socktype, res->ai_protocol);
    blockingSocketListMutex.Lock();
    blockingSocketList.Insert(sockfd, _FILE_AND_LINE_);
    blockingSocketListMutex.Unlock();
    connectResult=connect__(sockfd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
#endif

    if (connectResult==-1)
    {
        unsigned sockfdIndex;
        blockingSocketListMutex.Lock();
        sockfdIndex=blockingSocketList.GetIndexOf(sockfd);
        if (sockfdIndex!=(unsigned)-1)
            blockingSocketList.RemoveAtIndexFast(sockfdIndex);
        blockingSocketListMutex.Unlock();

        closesocket__(sockfd);
        return INVALID_SOCKET;
    }

    return sockfd;
#endif
}

VE_THREAD_DECLARATION(VENet::ConnectionAttemptLoop)
{



    TCPInterface::ThisPtrPlusSysAddr *s = (TCPInterface::ThisPtrPlusSysAddr *) arguments;



    SystemAddress systemAddress = s->systemAddress;
    TCPInterface *tcpInterface = s->tcpInterface;
    int newRemoteClientIndex=systemAddress.systemIndex;
    unsigned short socketFamily = s->socketFamily;
    VENet::OP_DELETE(s, _FILE_AND_LINE_);

    char str1[64];
    systemAddress.ToString(false, str1);
    SOCKET sockfd = tcpInterface->SocketConnect(str1, systemAddress.GetPort(), socketFamily);
    if (sockfd==INVALID_SOCKET)
    {
        tcpInterface->remoteClients[newRemoteClientIndex].isActiveMutex.Lock();
        tcpInterface->remoteClients[newRemoteClientIndex].SetActive(false);
        tcpInterface->remoteClients[newRemoteClientIndex].isActiveMutex.Unlock();

        tcpInterface->failedConnectionAttemptMutex.Lock();
        tcpInterface->failedConnectionAttempts.Push(systemAddress, _FILE_AND_LINE_ );
        tcpInterface->failedConnectionAttemptMutex.Unlock();
        return 0;
    }

    tcpInterface->remoteClients[newRemoteClientIndex].socket=sockfd;
    tcpInterface->remoteClients[newRemoteClientIndex].systemAddress=systemAddress;

    if (tcpInterface->threadRunning.GetValue()>0)
    {
        tcpInterface->completedConnectionAttemptMutex.Lock();
        tcpInterface->completedConnectionAttempts.Push(systemAddress, _FILE_AND_LINE_ );
        tcpInterface->completedConnectionAttemptMutex.Unlock();
    }




    return 0;

}

VE_THREAD_DECLARATION(VENet::UpdateTCPInterfaceLoop)
{
    TCPInterface * sts = ( TCPInterface * ) arguments;
	const unsigned int BUFF_SIZE=1048576;
//    char data[ BUFF_SIZE ];
    char * data = (char*) rakMalloc_Ex(BUFF_SIZE,_FILE_AND_LINE_);
    Packet *incomingMessage;
    fd_set readFD, exceptionFD, writeFD;
    sts->threadRunning.Increment();

#if VENET_SUPPORT_IPV6!=1
    sockaddr_in sockAddr;
    int sockAddrSize = sizeof(sockAddr);
#else
    struct sockaddr_storage sockAddr;
    socklen_t sockAddrSize = sizeof(sockAddr);
#endif

    int len;
    SOCKET newSock;
    int selectResult;


    timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=30000;


    while (sts->isStarted.GetValue()>0)
    {
#if OPEN_SSL_CLIENT_SUPPORT==1
        SystemAddress *sslSystemAddress;
        sslSystemAddress = sts->startSSL.PopInaccurate();
        if (sslSystemAddress)
        {
            if (sslSystemAddress->systemIndex>=0 &&
                    sslSystemAddress->systemIndex<sts->remoteClientsLength &&
                    sts->remoteClients[sslSystemAddress->systemIndex].systemAddress==*sslSystemAddress)
            {
                sts->remoteClients[sslSystemAddress->systemIndex].InitSSL(sts->ctx,sts->meth);
            }
            else
            {
                for (int i=0; i < sts->remoteClientsLength; i++)
                {
                    sts->remoteClients[i].isActiveMutex.Lock();
                    if (sts->remoteClients[i].isActive && sts->remoteClients[i].systemAddress==*sslSystemAddress)
                    {
                        if (sts->remoteClients[i].ssl==0)
                            sts->remoteClients[i].InitSSL(sts->ctx,sts->meth);
                    }
                    sts->remoteClients[i].isActiveMutex.Unlock();
                }
            }
            sts->startSSL.Deallocate(sslSystemAddress,_FILE_AND_LINE_);
        }
#endif


        SOCKET largestDescriptor=0;

        tv.tv_sec=0;
        tv.tv_usec=30000;


        while (1)
        {
            FD_ZERO(&readFD);
            FD_ZERO(&exceptionFD);
            FD_ZERO(&writeFD);
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
            largestDescriptor=0;
            if (sts->listenSocket!=INVALID_SOCKET)
            {
                FD_SET(sts->listenSocket, &readFD);
                FD_SET(sts->listenSocket, &exceptionFD);
                largestDescriptor = sts->listenSocket;
            }

            unsigned i;
            for (i=0; i < (unsigned int) sts->remoteClientsLength; i++)
            {
                sts->remoteClients[i].isActiveMutex.Lock();
                if (sts->remoteClients[i].isActive)
                {
                    SOCKET socketCopy = sts->remoteClients[i].socket;
                    if (socketCopy != INVALID_SOCKET)
                    {
                        FD_SET(socketCopy, &readFD);
                        FD_SET(socketCopy, &exceptionFD);
                        if (sts->remoteClients[i].outgoingData.GetBytesWritten()>0)
                            FD_SET(socketCopy, &writeFD);
                        if (socketCopy > largestDescriptor)
                            largestDescriptor = socketCopy;
                    }
                }
                sts->remoteClients[i].isActiveMutex.Unlock();
            }

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#endif
            selectResult=(int) select__(largestDescriptor+1, &readFD, &writeFD, &exceptionFD, &tv);

            if (selectResult<=0)
                break;

            if (sts->listenSocket!=INVALID_SOCKET && FD_ISSET(sts->listenSocket, &readFD))
            {
                newSock = accept__(sts->listenSocket, (sockaddr*)&sockAddr, (socklen_t*)&sockAddrSize);

                if (newSock != INVALID_SOCKET)
                {
                    int newRemoteClientIndex=-1;
                    for (newRemoteClientIndex=0; newRemoteClientIndex < sts->remoteClientsLength; newRemoteClientIndex++)
                    {
                        sts->remoteClients[newRemoteClientIndex].isActiveMutex.Lock();
                        if (sts->remoteClients[newRemoteClientIndex].isActive==false)
                        {
                            sts->remoteClients[newRemoteClientIndex].socket=newSock;

#if VENET_SUPPORT_IPV6!=1
                            sts->remoteClients[newRemoteClientIndex].systemAddress.address.addr4.sin_addr.s_addr=sockAddr.sin_addr.s_addr;
                            sts->remoteClients[newRemoteClientIndex].systemAddress.SetPortNetworkOrder( sockAddr.sin_port);
                            sts->remoteClients[newRemoteClientIndex].systemAddress.systemIndex=newRemoteClientIndex;
#else
                            if (sockAddr.ss_family==AF_INET)
                            {
                                memcpy(&sts->remoteClients[newRemoteClientIndex].systemAddress.address.addr4,(sockaddr_in *)&sockAddr,sizeof(sockaddr_in));
                            }
                            else
                            {
                                memcpy(&sts->remoteClients[newRemoteClientIndex].systemAddress.address.addr6,(sockaddr_in6 *)&sockAddr,sizeof(sockaddr_in6));
                            }

#endif
                            sts->remoteClients[newRemoteClientIndex].SetActive(true);
                            sts->remoteClients[newRemoteClientIndex].isActiveMutex.Unlock();


                            SystemAddress *newConnectionSystemAddress=sts->newIncomingConnections.Allocate( _FILE_AND_LINE_ );
                            *newConnectionSystemAddress=sts->remoteClients[newRemoteClientIndex].systemAddress;
                            sts->newIncomingConnections.Push(newConnectionSystemAddress);

                            break;
                        }
                        sts->remoteClients[newRemoteClientIndex].isActiveMutex.Unlock();
                    }
                    if (newRemoteClientIndex==-1)
                    {
                        closesocket__(sts->listenSocket);
                    }
                }
                else
                {
#ifdef _DO_PRINTF
                    VENET_DEBUG_PRINTF("Error: connection failed\n");
#endif
                }
            }
            else if (sts->listenSocket!=INVALID_SOCKET && FD_ISSET(sts->listenSocket, &exceptionFD))
            {
#ifdef _DO_PRINTF
                int err;
                int errlen = sizeof(err);
                getsockopt__(sts->listenSocket, SOL_SOCKET, SO_ERROR,(char*)&err, &errlen);
                VENET_DEBUG_PRINTF("Socket error %s on listening socket\n", err);
#endif
            }

            {
                i=0;
                while (i < (unsigned int) sts->remoteClientsLength)
                {
                    if (sts->remoteClients[i].isActive==false)
                    {
                        i++;
                        continue;
                    }
                    SOCKET socketCopy = sts->remoteClients[i].socket;
                    if (socketCopy == INVALID_SOCKET)
                    {
                        i++;
                        continue;
                    }

                    if (FD_ISSET(socketCopy, &exceptionFD))
                    {
                        SystemAddress *lostConnectionSystemAddress=sts->lostConnections.Allocate( _FILE_AND_LINE_ );
                        *lostConnectionSystemAddress=sts->remoteClients[i].systemAddress;
                        sts->lostConnections.Push(lostConnectionSystemAddress);
                        sts->remoteClients[i].isActiveMutex.Lock();
                        sts->remoteClients[i].SetActive(false);
                        sts->remoteClients[i].isActiveMutex.Unlock();
                    }
                    else
                    {
                        if (FD_ISSET(socketCopy, &readFD))
                        {
                            len = sts->remoteClients[i].Recv(data,BUFF_SIZE);

                            if (len>0)
                            {
                                incomingMessage=sts->incomingMessages.Allocate( _FILE_AND_LINE_ );
                                incomingMessage->data = (unsigned char*) rakMalloc_Ex( len+1, _FILE_AND_LINE_ );
                                memcpy(incomingMessage->data, data, len);
                                incomingMessage->data[len]=0; // Null terminate this so we can print it out as regular strings.  This is different from VENet which does not do this.
                                incomingMessage->length=len;
                                incomingMessage->deleteData=true; // actually means came from SPSC, rather than AllocatePacket
                                incomingMessage->systemAddress=sts->remoteClients[i].systemAddress;
                                sts->incomingMessages.Push(incomingMessage);
                            }
                            else
                            {
                                SystemAddress *lostConnectionSystemAddress=sts->lostConnections.Allocate( _FILE_AND_LINE_ );
                                *lostConnectionSystemAddress=sts->remoteClients[i].systemAddress;
                                sts->lostConnections.Push(lostConnectionSystemAddress);
                                sts->remoteClients[i].isActiveMutex.Lock();
                                sts->remoteClients[i].SetActive(false);
                                sts->remoteClients[i].isActiveMutex.Unlock();
                                continue;
                            }
                        }
                        if (FD_ISSET(socketCopy, &writeFD))
                        {
                            RemoteClient *rc = &sts->remoteClients[i];
                            unsigned int bytesInBuffer;
                            int bytesAvailable;
                            int bytesSent;
                            rc->outgoingDataMutex.Lock();
                            bytesInBuffer=rc->outgoingData.GetBytesWritten();
                            if (bytesInBuffer>0)
                            {
                                unsigned int contiguousLength;
                                char* contiguousBytesPointer = rc->outgoingData.PeekContiguousBytes(&contiguousLength);
                                if (contiguousLength < (unsigned int) BUFF_SIZE && contiguousLength<bytesInBuffer)
                                {
                                    if (bytesInBuffer > BUFF_SIZE)
                                        bytesAvailable=BUFF_SIZE;
                                    else
                                        bytesAvailable=bytesInBuffer;
                                    rc->outgoingData.ReadBytes(data,bytesAvailable,true);
                                    bytesSent=rc->Send(data,bytesAvailable);
                                }
                                else
                                {
                                    bytesSent=rc->Send(contiguousBytesPointer,contiguousLength);
                                }

                                rc->outgoingData.IncrementReadOffset(bytesSent);
                                bytesInBuffer=rc->outgoingData.GetBytesWritten();
                            }
                            rc->outgoingDataMutex.Unlock();
                        }

                        i++;
                    }
                }
            }
        }

        VESleep(30);
    }
    sts->threadRunning.Decrement();

    veFree_Ex(data,_FILE_AND_LINE_);

    return 0;

}

void RemoteClient::SetActive(bool a)
{
    if (isActive != a)
    {
        isActive=a;
        Reset();
        if (isActive==false && socket!=INVALID_SOCKET)
        {
            closesocket__(socket);
            socket=INVALID_SOCKET;
        }
    }
}
void RemoteClient::SendOrBuffer(const char **data, const unsigned int *lengths, const int numParameters)
{
    const bool ALLOW_SEND_FROM_USER_THREAD=false;

    int parameterIndex;
    if (isActive==false)
        return;
    parameterIndex=0;
    for (; parameterIndex < numParameters; parameterIndex++)
    {
        outgoingDataMutex.Lock();
        if (ALLOW_SEND_FROM_USER_THREAD && outgoingData.GetBytesWritten()==0)
        {
            outgoingDataMutex.Unlock();
            int bytesSent = Send(data[parameterIndex],lengths[parameterIndex]);
            if (bytesSent<(int) lengths[parameterIndex])
            {
                outgoingDataMutex.Lock();
                outgoingData.WriteBytes(data[parameterIndex]+bytesSent,lengths[parameterIndex]-bytesSent,_FILE_AND_LINE_);
                outgoingDataMutex.Unlock();
            }
        }
        else
        {
            outgoingData.WriteBytes(data[parameterIndex],lengths[parameterIndex],_FILE_AND_LINE_);
            outgoingDataMutex.Unlock();
        }
    }
}
#if OPEN_SSL_CLIENT_SUPPORT==1
void RemoteClient::InitSSL(SSL_CTX* ctx, SSL_METHOD *meth)
{
    (void) meth;

    ssl = SSL_new (ctx);
    VEAssert(ssl);
    int res;
    res = SSL_set_fd (ssl, socket);
    if (res!=1)
        printf("SSL_set_fd error: %s\n", ERR_reason_error_string(ERR_get_error()));
    VEAssert(res==1);
    res = SSL_connect (ssl);
    if (res<0)
    {
        printf("SSL_connect error: %s\n", ERR_reason_error_string(ERR_get_error()));
    }
    else if (res==0)
    {
        int err = SSL_get_error(ssl, res);
        switch (err)
        {
        case SSL_ERROR_NONE:
            printf("SSL_ERROR_NONE\n");
            break;
        case SSL_ERROR_ZERO_RETURN:
            printf("SSL_ERROR_ZERO_RETURN\n");
            break;
        case SSL_ERROR_WANT_READ:
            printf("SSL_ERROR_WANT_READ\n");
            break;
        case SSL_ERROR_WANT_WRITE:
            printf("SSL_ERROR_WANT_WRITE\n");
            break;
        case SSL_ERROR_WANT_CONNECT:
            printf("SSL_ERROR_WANT_CONNECT\n");
            break;
        case SSL_ERROR_WANT_ACCEPT:
            printf("SSL_ERROR_WANT_ACCEPT\n");
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            printf("SSL_ERROR_WANT_X509_LOOKUP\n");
            break;
        case SSL_ERROR_SYSCALL:
            printf("SSL_ERROR_SYSCALL\n");
            break;
        case SSL_ERROR_SSL:
            printf("SSL_ERROR_SSL\n");
            break;
        }

    }
    VEAssert(res==1);
}
void RemoteClient::DisconnectSSL(void)
{
    if (ssl)
        SSL_shutdown (ssl);  /* send SSL/TLS close_notify */
}
void RemoteClient::FreeSSL(void)
{
    if (ssl)
        SSL_free (ssl);
}
int RemoteClient::Send(const char *data, unsigned int length)
{
    if (ssl)
        return SSL_write (ssl, data, length);
    else
        return send__(socket, data, length, 0);
}
int RemoteClient::Recv(char *data, const int dataSize)
{
    if (ssl)
        return SSL_read (ssl, data, dataSize);
    else
        return recv__(socket, data, dataSize, 0);
}
#else
int RemoteClient::Send(const char *data, unsigned int length)
{
#ifdef __native_client__
    return -1;
#else
return send__(socket, data, length, 0);
#endif
}
int RemoteClient::Recv(char *data, const int dataSize)
{
#ifdef __native_client__
    return -1;
#else
return recv__(socket, data, dataSize, 0);
#endif
}
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
