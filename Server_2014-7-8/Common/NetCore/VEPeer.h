////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 VEPeer.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ReliabilityLayer.h"
#include "VEPeerInterface.h"
#include "BitStream.h"
#include "SingleProducerConsumer.h"
#include "SimpleMutex.h"
#include "DS_OrderedList.h"
#include "Export.h"
#include "VEString.h"
#include "VEThread.h"
#include "VENetSocket.h"
#include "VENetSmartPtr.h"
#include "DS_ThreadsafeAllocatingQueue.h"
#include "SignaledEvent.h"
#include "NativeFeatureIncludes.h"
#include "SecureHandshake.h"
#include "LocklessTypes.h"

namespace VENet
{
class HuffmanEncodingTree;
class PluginInterface2;

struct RemoteSystemIndex
{
    unsigned index;
    RemoteSystemIndex *next;
};

class VE_DLL_EXPORT VEPeer : public VEPeerInterface
{
public:
    VEPeer();

    virtual ~VEPeer();

    StartupResult Startup( unsigned short maxConnections, SocketDescriptor *socketDescriptors, unsigned socketDescriptorCount, int threadPriority=-99999 );

    bool InitializeSecurity( const char *publicKey, const char *privateKey, bool bRequireClientKey = false );

    void DisableSecurity( void );

    void AddToSecurityExceptionList(const char *ip);

    void RemoveFromSecurityExceptionList(const char *ip);

    bool IsInSecurityExceptionList(const char *ip);

    void SetMaximumIncomingConnections( unsigned short numberAllowed );

    unsigned short GetMaximumIncomingConnections( void ) const;

    unsigned short NumberOfConnections(void) const;

    void SetIncomingPassword( const char* passwordData, int passwordDataLength );

    void GetIncomingPassword( char* passwordData, int *passwordDataLength  );

    ConnectionAttemptResult Connect( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey=0, unsigned connectionSocketIndex=0, unsigned sendConnectionAttemptCount=6, unsigned timeBetweenSendConnectionAttemptsMS=1000, VENet::TimeMS timeoutTime=0 );

    virtual ConnectionAttemptResult ConnectWithSocket(const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, VENetSmartPtr<VENetSocket> socket, PublicKey *publicKey=0, unsigned sendConnectionAttemptCount=6, unsigned timeBetweenSendConnectionAttemptsMS=1000, VENet::TimeMS timeoutTime=0);

    void Shutdown( unsigned int blockDuration, unsigned char orderingChannel=0, PacketPriority disconnectionNotificationPriority=LOW_PRIORITY );

    bool IsActive( void ) const;

    bool GetConnectionList( SystemAddress *remoteSystems, unsigned short *numberOfSystems ) const;

    virtual uint32_t GetNextSendReceipt(void);

    virtual uint32_t IncrementNextSendReceipt(void);

    uint32_t Send( const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber=0 );

    void SendLoopback( const char *data, const int length );

    uint32_t Send( const VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber=0 );

    uint32_t SendList( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber=0 );

    Packet* Receive( void );

    void DeallocatePacket( Packet *packet );

    unsigned short GetMaximumNumberOfPeers( void ) const;

    void CloseConnection( const AddressOrGUID target, bool sendDisconnectionNotification, unsigned char orderingChannel=0, PacketPriority disconnectionNotificationPriority=LOW_PRIORITY );

    void CancelConnectionAttempt( const SystemAddress target );

    ConnectionState GetConnectionState(const AddressOrGUID systemIdentifier);

    int GetIndexFromSystemAddress( const SystemAddress systemAddress ) const;

    SystemAddress GetSystemAddressFromIndex( int index );

    VENetGUID GetGUIDFromIndex( int index );

    void GetSystemList(DataStructures::List<SystemAddress> &addresses, DataStructures::List<VENetGUID> &guids) const;

    void AddToBanList( const char *IP, VENet::TimeMS milliseconds=0 );

    void RemoveFromBanList( const char *IP );

    void ClearBanList( void );

    bool IsBanned( const char *IP );

    void SetLimitIPConnectionFrequency(bool b);

    void Ping( const SystemAddress target );

    bool Ping( const char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections, unsigned connectionSocketIndex=0 );

    int GetAveragePing( const AddressOrGUID systemIdentifier );

    int GetLastPing( const AddressOrGUID systemIdentifier ) const;

    int GetLowestPing( const AddressOrGUID systemIdentifier ) const;

    void SetOccasionalPing( bool doPing );

    void SetOfflinePingResponse( const char *data, const unsigned int length );

    void GetOfflinePingResponse( char **data, unsigned int *length );

    SystemAddress GetInternalID( const SystemAddress systemAddress=UNASSIGNED_SYSTEM_ADDRESS, const int index=0 ) const;

    SystemAddress GetExternalID( const SystemAddress target ) const;

    const VENetGUID GetMyGUID(void) const;

    SystemAddress GetMyBoundAddress(const int socketIndex=0);

    const VENetGUID& GetGuidFromSystemAddress( const SystemAddress input ) const;

    SystemAddress GetSystemAddressFromGuid( const VENetGUID input ) const;

    bool GetClientPublicKeyFromSystemAddress( const SystemAddress input, char *client_public_key ) const;

    void SetTimeoutTime( VENet::TimeMS timeMS, const SystemAddress target );

    VENet::TimeMS GetTimeoutTime( const SystemAddress target );

    int GetMTUSize( const SystemAddress target ) const;

    unsigned GetNumberOfAddresses( void );

    const char* GetLocalIP( unsigned int index );

    bool IsLocalIP( const char *ip );

    void AllowConnectionResponseIPMigration( bool allow );

    bool AdvertiseSystem( const char *host, unsigned short remotePort, const char *data, int dataLength, unsigned connectionSocketIndex=0 );

    void SetSplitMessageProgressInterval(int interval);

    int GetSplitMessageProgressInterval(void) const;

    void SetUnreliableTimeout(VENet::TimeMS timeoutMS);

    void SendTTL( const char* host, unsigned short remotePort, int ttl, unsigned connectionSocketIndex=0 );

    void AttachPlugin( PluginInterface2 *plugin );

    void DetachPlugin( PluginInterface2 *messageHandler );

    void PushBackPacket( Packet *packet, bool pushAtHead );

    void ChangeSystemAddress(VENetGUID guid, const SystemAddress &systemAddress);

    Packet* AllocatePacket(unsigned dataSize);

    virtual VENetSmartPtr<VENetSocket> GetSocket( const SystemAddress target );

    virtual void GetSockets( DataStructures::List<VENetSmartPtr<VENetSocket> > &sockets );
    virtual void ReleaseSockets( DataStructures::List<VENetSmartPtr<VENetSocket> > &sockets );

    virtual void WriteOutOfBandHeader(VENet::BitStream *bitStream);

    virtual void SetUserUpdateThread(void (*_userUpdateThreadPtr)(VEPeerInterface *, void *), void *_userUpdateThreadData);

    virtual void ApplyNetworkSimulator( float packetloss, unsigned short minExtraPing, unsigned short extraPingVariance);

    virtual void SetPerConnectionOutgoingBandwidthLimit( unsigned maxBitsPerSecond );

    virtual bool IsNetworkSimulatorActive( void );

    VENetStatistics * GetStatistics( const SystemAddress systemAddress, VENetStatistics *rns=0 );

    bool GetStatistics( const int index, VENetStatistics *rns );

    virtual void GetStatisticsList(DataStructures::List<SystemAddress> &addresses, DataStructures::List<VENetGUID> &guids, DataStructures::List<VENetStatistics> &statistics);

    virtual unsigned int GetReceiveBufferSize(void);

    bool RunUpdateCycle( BitStream &updateBitStream );

    bool RunRecvFromOnce( SOCKET s, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions );

    bool SendOutOfBand(const char *host, unsigned short remotePort, const char *data, BitSize_t dataLength, unsigned connectionSocketIndex=0 );

    struct PingAndClockDifferential
    {
        unsigned short pingTime;
        VENet::Time clockDifferential;
    };

    struct RemoteSystemStruct
    {
        bool isActive;
        SystemAddress systemAddress;
        SystemAddress myExternalSystemAddress;
        SystemAddress theirInternalSystemAddress[MAXIMUM_NUMBER_OF_INTERNAL_IDS];
        ReliabilityLayer reliabilityLayer;
        bool weInitiatedTheConnection;
        PingAndClockDifferential pingAndClockDifferential[ PING_TIMES_ARRAY_SIZE ];
        VENet::Time pingAndClockDifferentialWriteIndex;
        unsigned short lowestPing;
        VENet::Time nextPingTime;
        VENet::Time lastReliableSend;
        VENet::Time connectionTime;
        VENetGUID guid;
        int MTUSize;
        VENetSmartPtr<VENetSocket> VENetSocket;
        SystemIndex remoteSystemIndex;

#if LIBCAT_SECURITY==1
        char answer[cat::EasyHandshake::ANSWER_BYTES];

        char client_public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
#endif

        enum ConnectMode {NO_ACTION, DISCONNECT_ASAP, DISCONNECT_ASAP_SILENTLY, DISCONNECT_ON_NO_ACK, REQUESTED_CONNECTION, HANDLING_CONNECTION_REQUEST, UNVERIFIED_SENDER, CONNECTED} connectMode;
    };

    void ProcessChromePacket(SOCKET s, const char *buffer, int dataSize, const SystemAddress& recvFromAddress, VENet::TimeUS timeRead);
protected:

    friend VE_THREAD_DECLARATION(UpdateNetworkLoop);
    friend VE_THREAD_DECLARATION(RecvFromLoop);
    friend VE_THREAD_DECLARATION(UDTConnect);

    friend bool ProcessOfflineNetworkPacket( SystemAddress systemAddress, const char *data, const int length, VEPeer *rakPeer, VENetSmartPtr<VENetSocket> VENetSocket, bool *isOfflineMessage, VENet::TimeUS timeRead );
    friend void ProcessNetworkPacket( const SystemAddress systemAddress, const char *data, const int length, VEPeer *rakPeer, VENet::TimeUS timeRead, BitStream &updateBitStream );
    friend void ProcessNetworkPacket( const SystemAddress systemAddress, const char *data, const int length, VEPeer *rakPeer, VENetSmartPtr<VENetSocket> VENetSocket, VENet::TimeUS timeRead, BitStream &updateBitStream );

    int GetIndexFromSystemAddress( const SystemAddress systemAddress, bool calledFromNetworkThread ) const;
    int GetIndexFromGuid( const VENetGUID guid );

    ConnectionAttemptResult SendConnectionRequest( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned int extraData, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, VENet::TimeMS timeoutTime, VENetSmartPtr<VENetSocket> socket );
    ConnectionAttemptResult SendConnectionRequest( const char* host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned int extraData, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, VENet::TimeMS timeoutTime );
    RemoteSystemStruct *GetRemoteSystemFromSystemAddress( const SystemAddress systemAddress, bool calledFromNetworkThread, bool onlyActive ) const;
    VEPeer::RemoteSystemStruct *GetRemoteSystem( const AddressOrGUID systemIdentifier, bool calledFromNetworkThread, bool onlyActive ) const;
    void ValidateRemoteSystemLookup(void) const;
    RemoteSystemStruct *GetRemoteSystemFromGUID( const VENetGUID guid, bool onlyActive ) const;
    void ParseConnectionRequestPacket( VEPeer::RemoteSystemStruct *remoteSystem, const SystemAddress &systemAddress, const char *data, int byteSize);
    void OnConnectionRequest( VEPeer::RemoteSystemStruct *remoteSystem, VENet::Time incomingTimestamp );
    void NotifyAndFlagForShutdown( const SystemAddress systemAddress, bool performImmediate, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority );
    unsigned short GetNumberOfRemoteInitiatedConnections( void ) const;
    RemoteSystemStruct * AssignSystemAddressToRemoteSystemList( const SystemAddress systemAddress, RemoteSystemStruct::ConnectMode connectionMode, VENetSmartPtr<VENetSocket> incomingVENetSocket, bool *thisIPConnectedRecently, SystemAddress bindingAddress, int incomingMTU, VENetGUID guid, bool useSecurity );
    void ShiftIncomingTimestamp( unsigned char *data, const SystemAddress &systemAddress ) const;
    VENet::Time GetBestClockDifferential( const SystemAddress systemAddress ) const;

    bool IsLoopbackAddress(const AddressOrGUID &systemIdentifier, bool matchPort) const;
    SystemAddress GetLoopbackAddress(void) const;

    volatile bool endThreads;
    volatile bool isMainLoopThreadActive;

    VENet::LocklessUint32_t isRecvFromLoopThreadActive;


    bool occasionalPing;
    unsigned short maximumNumberOfPeers;
    unsigned short maximumIncomingConnections;
    VENet::BitStream offlinePingResponse;
    char incomingPassword[256];
    unsigned char incomingPasswordLength;

    RemoteSystemStruct* remoteSystemList;
    RemoteSystemStruct** activeSystemList;
    unsigned int activeSystemListSize;

    RemoteSystemIndex **remoteSystemLookup;
    unsigned int RemoteSystemLookupHashIndex(const SystemAddress &sa) const;
    void ReferenceRemoteSystem(const SystemAddress &sa, unsigned int remoteSystemListIndex);
    void DereferenceRemoteSystem(const SystemAddress &sa);
    RemoteSystemStruct* GetRemoteSystem(const SystemAddress &sa) const;
    unsigned int GetRemoteSystemIndex(const SystemAddress &sa) const;
    void ClearRemoteSystemLookup(void);
    DataStructures::MemoryPool<RemoteSystemIndex> remoteSystemIndexPool;

    void AddToActiveSystemList(unsigned int remoteSystemListIndex);
    void RemoveFromActiveSystemList(const SystemAddress &sa);

    enum
    {
        requestedConnectionList_Mutex,
        offlinePingResponse_Mutex,
        NUMBER_OF_RAKPEER_MUTEXES
    };
    SimpleMutex vePeerMutexes[ NUMBER_OF_RAKPEER_MUTEXES ];

    bool updateCycleIsRunning;

    unsigned int bytesSentPerSecond, bytesReceivedPerSecond;
    unsigned int validationInteger;
    SimpleMutex incomingQueueMutex, banListMutex;

    struct BanStruct
    {
        char *IP;
        VENet::TimeMS timeout;
    };

    struct RequestedConnectionStruct
    {
        SystemAddress systemAddress;
        VENet::Time nextRequestTime;
        unsigned char requestsMade;
        char *data;
        unsigned short dataLength;
        char outgoingPassword[256];
        unsigned char outgoingPasswordLength;
        unsigned socketIndex;
        unsigned int extraData;
        unsigned sendConnectionAttemptCount;
        unsigned timeBetweenSendConnectionAttemptsMS;
        VENet::TimeMS timeoutTime;
        PublicKeyMode publicKeyMode;
        VENetSmartPtr<VENetSocket> socket;
        enum {CONNECT=1, /*PING=2, PING_OPEN_CONNECTIONS=4,*/ /*ADVERTISE_SYSTEM=2*/} actionToTake;

#if LIBCAT_SECURITY==1
        char handshakeChallenge[cat::EasyHandshake::CHALLENGE_BYTES];
        cat::ClientEasyHandshake *client_handshake;
        char remote_public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
#endif
    };
#if LIBCAT_SECURITY==1
    bool GenerateConnectionRequestChallenge(RequestedConnectionStruct *rcs,PublicKey *publicKey);
#endif

    DataStructures::List<BanStruct*> banList;
    DataStructures::List<PluginInterface2*> pluginListTS, pluginListNTS;

    DataStructures::Queue<RequestedConnectionStruct*> requestedConnectionQueue;
    SimpleMutex requestedConnectionQueueMutex;


    struct BufferedCommandStruct
    {
        BitSize_t numberOfBitsToSend;
        PacketPriority priority;
        PacketReliability reliability;
        char orderingChannel;
        AddressOrGUID systemIdentifier;
        bool broadcast;
        RemoteSystemStruct::ConnectMode connectionMode;
        NetworkID networkID;
        bool blockingCommand;
        char *data;
        bool haveVENetCloseSocket;
        unsigned connectionSocketIndex;
        unsigned short remotePortVENetWasStartedOn_PS3;
        unsigned int extraSocketOptions;
        SOCKET socket;
        unsigned short port;
        uint32_t receipt;
        enum {BCS_SEND, BCS_CLOSE_CONNECTION, BCS_GET_SOCKET, BCS_CHANGE_SYSTEM_ADDRESS,/* BCS_USE_USER_SOCKET, BCS_REBIND_SOCKET_ADDRESS, BCS_RPC, BCS_RPC_SHIFT,*/ BCS_DO_NOTHING} command;
    };


    DataStructures::ThreadsafeAllocatingQueue<BufferedCommandStruct> bufferedCommands;


    struct RecvFromStruct
    {



        char data[MAXIMUM_MTU_SIZE];

        int bytesRead;
        SystemAddress systemAddress;
        VENet::TimeUS timeRead;
        SOCKET s;
        unsigned short remotePortVENetWasStartedOn_PS3;
        unsigned int extraSocketOptions;
    };


    DataStructures::ThreadsafeAllocatingQueue<RecvFromStruct> bufferedPackets;


    struct SocketQueryOutput
    {
        SocketQueryOutput() {}
        ~SocketQueryOutput() {}
        DataStructures::List<VENetSmartPtr<VENetSocket> > sockets;
    };

    DataStructures::ThreadsafeAllocatingQueue<SocketQueryOutput> socketQueryOutput;


    bool AllowIncomingConnections(void) const;

    void PingInternal( const SystemAddress target, bool performImmediate, PacketReliability reliability );
    void CloseConnectionInternal( const AddressOrGUID& systemIdentifier, bool sendDisconnectionNotification, bool performImmediate, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority );
    void SendBuffered( const char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, RemoteSystemStruct::ConnectMode connectionMode, uint32_t receipt );
    void SendBufferedList( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, RemoteSystemStruct::ConnectMode connectionMode, uint32_t receipt );
    bool SendImmediate( char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, bool useCallerDataAllocation, VENet::TimeUS currentTime, uint32_t receipt );
    void ClearBufferedCommands(void);
    void ClearBufferedPackets(void);
    void ClearSocketQueryOutput(void);
    void ClearRequestedConnectionList(void);
    void AddPacketToProducer(VENet::Packet *p);
    unsigned int GenerateSeedFromGuid(void);
    SimpleMutex securityExceptionMutex;

    int defaultMTUSize;
    bool trackFrequencyTable;

    DataStructures::List<VENetSmartPtr<VENetSocket> > socketList;
    void DerefAllSockets(void);
    unsigned int GetVENetSocketFromUserConnectionSocketIndex(unsigned int userIndex) const;
    VENet::BitStream *replyFromTargetBS;
    SystemAddress replyFromTargetPlayer;
    bool replyFromTargetBroadcast;

    VENet::TimeMS defaultTimeoutTime;

    void GenerateGUID(void);
    unsigned int GetSystemIndexFromGuid( const VENetGUID input ) const;
    VENetGUID myGuid;

    unsigned maxOutgoingBPS;

#ifdef _DEBUG
    double _packetloss;
    unsigned short _minExtraPing, _extraPingVariance;
#endif

    bool allowConnectionResponseIPMigration;

    SystemAddress firstExternalID;
    int splitMessageProgressInterval;
    VENet::TimeMS unreliableTimeout;

    DataStructures::List<VENet::VEString> securityExceptionList;

    SystemAddress ipList[ MAXIMUM_NUMBER_OF_INTERNAL_IDS ];

    bool allowInternalRouting;

    void (*userUpdateThreadPtr)(VEPeerInterface *, void *);
    void *userUpdateThreadData;


    SignaledEvent quitAndDataEvents;
    bool limitConnectionFrequencyFromTheSameIP;

    SimpleMutex packetAllocationPoolMutex;
    DataStructures::MemoryPool<Packet> packetAllocationPool;

    SimpleMutex packetReturnMutex;
    DataStructures::Queue<Packet*> packetReturnQueue;
    Packet *AllocPacket(unsigned dataSize, const char *file, unsigned int line);
    Packet *AllocPacket(unsigned dataSize, unsigned char *data, const char *file, unsigned int line);

    SimpleMutex sendReceiptSerialMutex;
    uint32_t sendReceiptSerial;
    void ResetSendReceipt(void);
    void OnConnectedPong(VENet::Time sendPingTime, VENet::Time sendPongTime, RemoteSystemStruct *remoteSystem);
    void CallPluginCallbacks(DataStructures::List<PluginInterface2*> &pluginList, Packet *packet);

#if LIBCAT_SECURITY==1
    bool _using_security, _require_client_public_key;
    char my_public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
    cat::ServerEasyHandshake *_server_handshake;
    cat::CookieJar *_cookie_jar;
    bool InitializeClientSecurity(RequestedConnectionStruct *rcs, const char *public_key);
#endif

};

}