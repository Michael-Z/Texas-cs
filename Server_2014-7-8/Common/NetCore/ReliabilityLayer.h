////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 ReliabilityLayer.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VEMemoryOverride.h"
#include "MTUSize.h"
#include "DS_LinkedList.h"
#include "DS_List.h"
#include "SocketLayer.h"
#include "PacketPriority.h"
#include "DS_Queue.h"
#include "BitStream.h"
#include "InternalPacket.h"
#include "VENetStatistics.h"
#include "SHA1.h"
#include "DS_OrderedList.h"
#include "DS_RangeList.h"
#include "DS_BPlusTree.h"
#include "DS_MemoryPool.h"
#include "VENetDefines.h"
#include "DS_Heap.h"
#include "BitStream.h"
#include "NativeFeatureIncludes.h"
#include "SecureHandshake.h"
#include "PluginInterface2.h"
#include "Rand.h"

#if USE_SLIDING_WINDOW_CONGESTION_CONTROL!=1
#include "CCVENetUDT.h"
#define INCLUDE_TIMESTAMP_WITH_DATAGRAMS 1
#else
#include "CCVENetSlidingWindow.h"
#define INCLUDE_TIMESTAMP_WITH_DATAGRAMS 0
#endif

#define NUMBER_OF_ORDERED_STREAMS 32 // 2^5

#define RESEND_TREE_ORDER 32

namespace VENet
{

class PluginInterface2;
class VENetRandom;
typedef uint64_t reliabilityHeapWeightType;

struct SplitPacketChannel
{
    CCTimeType lastUpdateTime;

    DataStructures::List<InternalPacket*> splitPacketList;

#if PREALLOCATE_LARGE_MESSAGES==1
    InternalPacket *returnedPacket;
    bool gotFirstPacket;
    unsigned int stride;
    unsigned int splitPacketsArrived;
#else
    InternalPacket *firstPacket;
#endif

};
int VE_DLL_EXPORT SplitPacketChannelComp( SplitPacketIdType const &key, SplitPacketChannel* const &data );

struct BPSTracker
{
    BPSTracker();
    ~BPSTracker();
    void Reset(const char *file, unsigned int line);
    inline void Push1(CCTimeType time, uint64_t value1)
    {
        dataQueue.Push(TimeAndValue2(time,value1),_FILE_AND_LINE_);
        total1+=value1;
        lastSec1+=value1;
    }
    inline uint64_t GetBPS1(CCTimeType time)
    {
        (void) time;
        return lastSec1;
    }
    inline uint64_t GetBPS1Threadsafe(CCTimeType time)
    {
        (void) time;
        return lastSec1;
    }
    uint64_t GetTotal1(void) const;

    struct TimeAndValue2
    {
        TimeAndValue2();
        ~TimeAndValue2();
        TimeAndValue2(CCTimeType t, uint64_t v1);
        uint64_t value1;
        CCTimeType time;
    };

    uint64_t total1, lastSec1;
    DataStructures::Queue<TimeAndValue2> dataQueue;
    void ClearExpired1(CCTimeType time);
};

class ReliabilityLayer
{
public:
    ReliabilityLayer();

    ~ReliabilityLayer();

    void Reset( bool resetVariables, int MTUSize, bool _useSecurity );

    void SetTimeoutTime( VENet::TimeMS time );

    VENet::TimeMS GetTimeoutTime(void);

    bool HandleSocketReceiveFromConnectedPlayer(
        const char *buffer, unsigned int length, SystemAddress &systemAddress, DataStructures::List<PluginInterface2*> &messageHandlerList, int MTUSize,
        SOCKET s, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, CCTimeType timeRead, BitStream &updateBitStream);

    BitSize_t Receive( unsigned char**data );

    bool Send( char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, unsigned char orderingChannel, bool makeDataCopy, int MTUSize, CCTimeType currentTime, uint32_t receipt );

    void Update( SOCKET s, SystemAddress &systemAddress, int MTUSize, CCTimeType time,
                 unsigned bitsPerSecondLimit,
                 DataStructures::List<PluginInterface2*> &messageHandlerList,
                 VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, BitStream &updateBitStream);

    bool IsDeadConnection( void ) const;

    void KillConnection(void);

    VENetStatistics * GetStatistics( VENetStatistics *rns );

    bool IsOutgoingDataWaiting(void);
    bool AreAcksWaiting(void);

    void ApplyNetworkSimulator( double _maxSendBPS, VENet::TimeMS _minExtraPing, VENet::TimeMS _extraPingVariance );

    bool IsNetworkSimulatorActive( void );

    void SetSplitMessageProgressInterval(int interval);
    void SetUnreliableTimeout(VENet::TimeMS timeoutMS);
    bool AckTimeout(VENet::Time curTime);
    CCTimeType GetNextSendTime(void) const;
    CCTimeType GetTimeBetweenPackets(void) const;



    VENet::TimeMS GetTimeLastDatagramArrived(void) const
    {
        return timeLastDatagramArrived;
    }

private:
    void SendBitStream( SOCKET s, SystemAddress &systemAddress, VENet::BitStream *bitStream, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, CCTimeType currentTime);

    BitSize_t WriteToBitStreamFromInternalPacket( VENet::BitStream *bitStream, const InternalPacket *const internalPacket, CCTimeType curTime );

    InternalPacket* CreateInternalPacketFromBitStream( VENet::BitStream *bitStream, CCTimeType time );

    unsigned RemovePacketFromResendListAndDeleteOlderReliableSequenced( const MessageNumberType messageNumber, CCTimeType time, DataStructures::List<PluginInterface2*> &messageHandlerList, const SystemAddress &systemAddress );

    void SendAcknowledgementPacket( const DatagramSequenceNumberType messageNumber, CCTimeType time);

    bool IsSendThrottled( int MTUSize );

    void UpdateWindowFromPacketloss( CCTimeType time );

    void UpdateWindowFromAck( CCTimeType time );

    BitSize_t GetMaxMessageHeaderLengthBits( void );
    BitSize_t GetMessageHeaderLengthBits( const InternalPacket *const internalPacket );

    void GetSHA1( unsigned char * const buffer, unsigned int nbytes, char code[ SHA1_LENGTH ] );

    bool CheckSHA1( char code[ SHA1_LENGTH ], unsigned char * const buffer, unsigned int nbytes );
    bool IsOlderOrderedPacket( OrderingIndexType newPacketOrderingIndex, OrderingIndexType waitingForPacketOrderingIndex );

    void SplitPacket( InternalPacket *internalPacket );

    void InsertIntoSplitPacketList( InternalPacket * internalPacket, CCTimeType time );

    InternalPacket * BuildPacketFromSplitPacketList( SplitPacketIdType splitPacketId, CCTimeType time,
            SOCKET s, SystemAddress &systemAddress, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, BitStream &updateBitStream);
    InternalPacket * BuildPacketFromSplitPacketList( SplitPacketChannel *splitPacketChannel, CCTimeType time );

    InternalPacket * CreateInternalPacketCopy( InternalPacket *original, int dataByteOffset, int dataByteLength, CCTimeType time );

    void InsertPacketIntoResendList( InternalPacket *internalPacket, CCTimeType time, bool firstResend, bool modifyUnacknowledgedBytes );

    void FreeMemory( bool freeAllImmediately );

    void FreeThreadSafeMemory( void );

    void InitializeVariables( void );

    bool IsExpiredTime(unsigned int input, CCTimeType currentTime) const;

    void UpdateNextActionTime(void);

    unsigned int GetResendListDataSize(void) const;

    void UpdateThreadedMemory(void);

    void CalculateHistogramAckSize(void);

    DataStructures::Queue<InternalPacket*> outputQueue;
    int splitMessageProgressInterval;
    CCTimeType unreliableTimeout;

    struct MessageNumberNode
    {
        DatagramSequenceNumberType messageNumber;
        MessageNumberNode *next;
    };
    struct DatagramHistoryNode
    {
        DatagramHistoryNode() {}
        DatagramHistoryNode(MessageNumberNode *_head, CCTimeType ts
                           ) :
                head(_head), timeSent(ts)
        {}
        MessageNumberNode *head;
        CCTimeType timeSent;
    };
    DataStructures::Queue<DatagramHistoryNode> datagramHistory;
    DataStructures::MemoryPool<MessageNumberNode> datagramHistoryMessagePool;

    struct UnreliableWithAckReceiptNode
    {
        UnreliableWithAckReceiptNode() {}
        UnreliableWithAckReceiptNode(DatagramSequenceNumberType _datagramNumber, uint32_t _sendReceiptSerial, VENet::TimeUS _nextActionTime) :
                datagramNumber(_datagramNumber), sendReceiptSerial(_sendReceiptSerial), nextActionTime(_nextActionTime)
        {}
        DatagramSequenceNumberType datagramNumber;
        uint32_t sendReceiptSerial;
        VENet::TimeUS nextActionTime;
    };
    DataStructures::List<UnreliableWithAckReceiptNode> unreliableWithAckReceiptHistory;

    void RemoveFromDatagramHistory(DatagramSequenceNumberType index);
    MessageNumberNode* GetMessageNumberNodeByDatagramIndex(DatagramSequenceNumberType index, CCTimeType *timeSent);
    void AddFirstToDatagramHistory(DatagramSequenceNumberType datagramNumber, CCTimeType timeSent);
    MessageNumberNode* AddFirstToDatagramHistory(DatagramSequenceNumberType datagramNumber, DatagramSequenceNumberType messageNumber, CCTimeType timeSent);
    MessageNumberNode* AddSubsequentToDatagramHistory(MessageNumberNode *messageNumberNode, DatagramSequenceNumberType messageNumber);
    DatagramSequenceNumberType datagramHistoryPopCount;

    DataStructures::MemoryPool<InternalPacket> internalPacketPool;
    InternalPacket *resendBuffer[RESEND_BUFFER_ARRAY_LENGTH];
    InternalPacket *resendLinkedListHead;
    InternalPacket *unreliableLinkedListHead;
    void RemoveFromUnreliableLinkedList(InternalPacket *internalPacket);
    void AddToUnreliableLinkedList(InternalPacket *internalPacket);
    VENet::TimeMS timeLastDatagramArrived;

    DataStructures::Heap<reliabilityHeapWeightType, InternalPacket*, false> outgoingPacketBuffer;
    reliabilityHeapWeightType outgoingPacketBufferNextWeights[NUMBER_OF_PRIORITIES];
    void InitHeapWeights(void);
    reliabilityHeapWeightType GetNextWeight(int priorityLevel);

    DataStructures::OrderedList<SplitPacketIdType, SplitPacketChannel*, SplitPacketChannelComp> splitPacketChannelList;

    MessageNumberType sendReliableMessageNumberIndex;
    MessageNumberType internalOrderIndex;
    bool deadConnection, cheater;
    SplitPacketIdType splitPacketId;
    VENet::TimeMS timeoutTime;
    VENetStatistics statistics;

    OrderingIndexType orderedWriteIndex[NUMBER_OF_ORDERED_STREAMS];
    OrderingIndexType sequencedWriteIndex[NUMBER_OF_ORDERED_STREAMS];
    OrderingIndexType orderedReadIndex[NUMBER_OF_ORDERED_STREAMS];
    OrderingIndexType highestSequencedReadIndex[NUMBER_OF_ORDERED_STREAMS];
    DataStructures::Heap<reliabilityHeapWeightType, InternalPacket*, false> orderingHeaps[NUMBER_OF_ORDERED_STREAMS];
    OrderingIndexType heapIndexOffsets[NUMBER_OF_ORDERED_STREAMS];

    DataStructures::Queue<bool> hasReceivedPacketQueue;
    DatagramSequenceNumberType receivedPacketsBaseIndex;
    bool resetReceivedPackets;

    CCTimeType lastUpdateTime;
    CCTimeType timeBetweenPackets, nextSendTime;



    CCTimeType ackPingSum;
    unsigned char ackPingIndex;
    RemoteSystemTimeType remoteSystemTime;
    CCTimeType nextAllowedThroughputSample;
    bool bandwidthExceededStatistic;

    long long throughputCapCountdown;

    unsigned receivePacketCount;

#ifdef _DEBUG
    struct DataAndTime
    {
        SOCKET s;
        char data[ MAXIMUM_MTU_SIZE ];
        unsigned int length;
        VENet::TimeMS sendTime;
        unsigned short remotePortVENetWasStartedOn_PS3;
        unsigned int extraSocketOptions;
    };
    DataStructures::Queue<DataAndTime*> delayList;

    double packetloss;
    VENet::TimeMS minExtraPing, extraPingVariance;
#endif

    CCTimeType elapsedTimeSinceLastUpdate;

    CCTimeType nextAckTimeToSend;


#if USE_SLIDING_WINDOW_CONGESTION_CONTROL==1
    VENet::CCVENetSlidingWindow congestionManager;
#else
    VENet::CCVENetUDT congestionManager;
#endif


    uint32_t unacknowledgedBytes;

    bool ResendBufferOverflow(void) const;
    void ValidateResendList(void) const;
    void ResetPacketsAndDatagrams(void);
    void PushPacket(CCTimeType time, InternalPacket *internalPacket, bool isReliable);
    void PushDatagram(void);
    bool TagMostRecentPushAsSecondOfPacketPair(void);
    void ClearPacketsAndDatagrams(void);
    void MoveToListHead(InternalPacket *internalPacket);
    void RemoveFromList(InternalPacket *internalPacket, bool modifyUnacknowledgedBytes);
    void AddToListTail(InternalPacket *internalPacket, bool modifyUnacknowledgedBytes);
    void PopListHead(bool modifyUnacknowledgedBytes);
    bool IsResendQueueEmpty(void) const;
    void SortSplitPacketList(DataStructures::List<InternalPacket*> &data, unsigned int leftEdge, unsigned int rightEdge) const;
    void SendACKs(SOCKET s, SystemAddress &systemAddress, CCTimeType time, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, BitStream &updateBitStream);

    DataStructures::List<InternalPacket*> packetsToSendThisUpdate;
    DataStructures::List<bool> packetsToDeallocThisUpdate;
    DataStructures::List<unsigned int> packetsToSendThisUpdateDatagramBoundaries;
    DataStructures::List<bool> datagramsToSendThisUpdateIsPair;
    DataStructures::List<unsigned int> datagramSizesInBytes;
    BitSize_t datagramSizeSoFar;
    BitSize_t allDatagramSizesSoFar;
    double totalUserDataBytesAcked;
    CCTimeType timeOfLastContinualSend;
    CCTimeType timeToNextUnreliableCull;

    DataStructures::RangeList<DatagramSequenceNumberType> incomingAcks;

    int countdownToNextPacketPair;
    InternalPacket* AllocateFromInternalPacketPool(void);
    void ReleaseToInternalPacketPool(InternalPacket *ip);

    DataStructures::RangeList<DatagramSequenceNumberType> acknowlegements;
    DataStructures::RangeList<DatagramSequenceNumberType> NAKs;
    bool remoteSystemNeedsBAndAS;

    unsigned int GetMaxDatagramSizeExcludingMessageHeaderBytes(void);
    BitSize_t GetMaxDatagramSizeExcludingMessageHeaderBits(void);

    void AllocInternalPacketData(InternalPacket *internalPacket, InternalPacketRefCountedData **refCounter, unsigned char *externallyAllocatedPtr, unsigned char *ourOffset);
    void AllocInternalPacketData(InternalPacket *internalPacket, unsigned char *externallyAllocatedPtr);
    void AllocInternalPacketData(InternalPacket *internalPacket, unsigned int numBytes, bool allowStack, const char *file, unsigned int line);
    void FreeInternalPacketData(InternalPacket *internalPacket, const char *file, unsigned int line);
    DataStructures::MemoryPool<InternalPacketRefCountedData> refCountedDataPool;

    BPSTracker bpsMetrics[RNS_PER_SECOND_METRICS_COUNT];
    CCTimeType lastBpsClear;

#if LIBCAT_SECURITY==1
public:
    cat::AuthenticatedEncryption* GetAuthenticatedEncryption(void)
    {
        return &auth_enc;
    }

protected:
    cat::AuthenticatedEncryption auth_enc;
    bool useSecurity;
#endif // LIBCAT_SECURITY
};

}