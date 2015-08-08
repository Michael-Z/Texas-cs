#include "ReliabilityLayer.h"
#include "GetTime.h"
#include "SocketLayer.h"
#include "PluginInterface2.h"
#include "VEAssert.h"
#include "Rand.h"
#include "MessageIdentifiers.h"
#ifdef USE_THREADED_SEND
#include "SendToThread.h"
#endif
#include <math.h>

using namespace VENet;

double Ceil(double d)
{
    if (((double)((int)d))==d) return d;
    return (int) (d+1.0);
}


#if CC_TIME_TYPE_BYTES==4
static const CCTimeType MAX_TIME_BETWEEN_PACKETS= 350;
static const CCTimeType HISTOGRAM_RESTART_CYCLE=10000;
#else
static const CCTimeType MAX_TIME_BETWEEN_PACKETS= 350000;
#endif
static const int DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE=512;
static const CCTimeType STARTING_TIME_BETWEEN_PACKETS=MAX_TIME_BETWEEN_PACKETS;

typedef uint32_t BitstreamLengthEncoding;

#ifdef _MSC_VER
#pragma warning( push )
#endif

#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
static unsigned int packetNumber=0;
static FILE *fp=0;
#endif


BPSTracker::TimeAndValue2::TimeAndValue2() {}
BPSTracker::TimeAndValue2::~TimeAndValue2() {}
BPSTracker::TimeAndValue2::TimeAndValue2(VENet::TimeUS t, uint64_t v1) : value1(v1), time(t) {}
BPSTracker::BPSTracker()
{
    Reset(_FILE_AND_LINE_);
}
BPSTracker::~BPSTracker() {}
void BPSTracker::Reset(const char *file, unsigned int line)
{
    total1=lastSec1=0;
    dataQueue.Clear(file,line);
}
uint64_t BPSTracker::GetTotal1(void) const
{
    return total1;
}

void BPSTracker::ClearExpired1(VENet::TimeUS time)
{
    while (dataQueue.IsEmpty()==false &&
#if CC_TIME_TYPE_BYTES==8
            dataQueue.Peek().time+1000000 < time
#else
            dataQueue.Peek().time+1000 < time
#endif
          )
    {
        lastSec1-=dataQueue.Peek().value1;
        dataQueue.Pop();
    }
}

struct DatagramHeaderFormat
{
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
    CCTimeType sourceSystemTime;
#endif
    DatagramSequenceNumberType datagramNumber;

    float AS;
    bool isACK;
    bool isNAK;
    bool isPacketPair;
    bool hasBAndAS;
    bool isContinuousSend;
    bool needsBAndAs;
    bool isValid;

    static BitSize_t GetDataHeaderBitLength()
    {
        return BYTES_TO_BITS(GetDataHeaderByteLength());
    }

    static unsigned int GetDataHeaderByteLength()
    {
        return 2 + 3 +
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
               sizeof(VENetTimeMS) +
#endif
               sizeof(float)*1;
    }

    void Serialize(VENet::BitStream *b)
    {
        b->Write(true);
        if (isACK)
        {
            b->Write(true);
            b->Write(hasBAndAS);
            b->AlignWriteToByteBoundary();
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
            VENet::TimeMS timeMSLow=(VENet::TimeMS) sourceSystemTime&0xFFFFFFFF;
            b->Write(timeMSLow);
#endif
            if (hasBAndAS)
            {
                b->Write(AS);
            }
        }
        else if (isNAK)
        {
            b->Write(false);
            b->Write(true);
        }
        else
        {
            b->Write(false);
            b->Write(false);
            b->Write(isPacketPair);
            b->Write(isContinuousSend);
            b->Write(needsBAndAs);
            b->AlignWriteToByteBoundary();
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
            VENet::TimeMS timeMSLow=(VENet::TimeMS) sourceSystemTime&0xFFFFFFFF;
            b->Write(timeMSLow);
#endif
            b->Write(datagramNumber);
        }
    }
    void Deserialize(VENet::BitStream *b)
    {
        b->Read(isValid);
        b->Read(isACK);
        if (isACK)
        {
            isNAK=false;
            isPacketPair=false;
            b->Read(hasBAndAS);
            b->AlignReadToByteBoundary();
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
            VENet::TimeMS timeMS;
            b->Read(timeMS);
            sourceSystemTime=(CCTimeType) timeMS;
#endif
            if (hasBAndAS)
            {
                b->Read(AS);
            }
        }
        else
        {
            b->Read(isNAK);
            if (isNAK)
            {
                isPacketPair=false;
            }
            else
            {
                b->Read(isPacketPair);
                b->Read(isContinuousSend);
                b->Read(needsBAndAs);
                b->AlignReadToByteBoundary();
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
                VENet::TimeMS timeMS;
                b->Read(timeMS);
                sourceSystemTime=(CCTimeType) timeMS;
#endif
                b->Read(datagramNumber);
            }
        }
    }
};

#if   !defined(__GNUC__) && !defined(__ARMCC)
#pragma warning(disable:4702)
#endif

#ifdef _WIN32
#ifdef _DEBUG_LOGGER
#include "WindowsIncludes.h"
#endif
#endif

#if defined (DEBUG_SPLIT_PACKET_PROBLEMS)
static int waitFlag=-1;
#endif

using namespace VENet;

int VENet::SplitPacketChannelComp( SplitPacketIdType const &key, SplitPacketChannel* const &data )
{
#if PREALLOCATE_LARGE_MESSAGES==1
    if (key < data->returnedPacket->splitPacketId)
        return -1;
    if (key == data->returnedPacket->splitPacketId)
        return 0;
#else
    if (key < data->splitPacketList[0]->splitPacketId)
        return -1;
    if (key == data->splitPacketList[0]->splitPacketId)
        return 0;
#endif
    return 1;
}

int SplitPacketIndexComp( SplitPacketIndexType const &key, InternalPacket* const &data )
{
    if (key < data->splitPacketIndex)
        return -1;
    if (key == data->splitPacketIndex)
        return 0;
    return 1;
}

ReliabilityLayer::ReliabilityLayer()
{

#ifdef _DEBUG
    timeoutTime=30000;
#else
    timeoutTime=10000;
#endif

#ifdef _DEBUG
    minExtraPing=extraPingVariance=0;
    packetloss=(double) minExtraPing;
#endif


#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
    if (fp==0 && 0)
    {
        fp = fopen("reliableorderedoutput.txt", "wt");
    }
#endif

    InitializeVariables();
    datagramHistoryMessagePool.SetPageSize(sizeof(MessageNumberNode)*128);
    internalPacketPool.SetPageSize(sizeof(InternalPacket)*INTERNAL_PACKET_PAGE_SIZE);
    refCountedDataPool.SetPageSize(sizeof(InternalPacketRefCountedData)*32);
}

ReliabilityLayer::~ReliabilityLayer()
{
    FreeMemory( true );
}

void ReliabilityLayer::Reset( bool resetVariables, int MTUSize, bool _useSecurity )
{

    FreeMemory( true );
    if (resetVariables)
    {
        InitializeVariables();

#if LIBCAT_SECURITY==1
        useSecurity = _useSecurity;

        if (_useSecurity)
            MTUSize -= cat::AuthenticatedEncryption::OVERHEAD_BYTES;
#else
        (void) _useSecurity;
#endif
        congestionManager.Init(VENet::GetTimeUS(), MTUSize - UDP_HEADER_SIZE);
    }
}

void ReliabilityLayer::SetTimeoutTime( VENet::TimeMS time )
{
    timeoutTime=time;
}

VENet::TimeMS ReliabilityLayer::GetTimeoutTime(void)
{
    return timeoutTime;
}

void ReliabilityLayer::InitializeVariables( void )
{
    memset( orderedWriteIndex, 0, NUMBER_OF_ORDERED_STREAMS * sizeof(OrderingIndexType));
    memset( sequencedWriteIndex, 0, NUMBER_OF_ORDERED_STREAMS * sizeof(OrderingIndexType) );
    memset( orderedReadIndex, 0, NUMBER_OF_ORDERED_STREAMS * sizeof(OrderingIndexType) );
    memset( highestSequencedReadIndex, 0, NUMBER_OF_ORDERED_STREAMS * sizeof(OrderingIndexType) );
    memset( &statistics, 0, sizeof( statistics ) );
    memset( &heapIndexOffsets, 0, sizeof( heapIndexOffsets ) );

    statistics.connectionStartTime = VENet::GetTimeUS();
    splitPacketId = 0;
    elapsedTimeSinceLastUpdate=0;
    throughputCapCountdown=0;
    sendReliableMessageNumberIndex = 0;
    internalOrderIndex=0;
    timeToNextUnreliableCull=0;
    unreliableLinkedListHead=0;
    lastUpdateTime= VENet::GetTimeUS();
    bandwidthExceededStatistic=false;
    remoteSystemTime=0;
    unreliableTimeout=0;
    lastBpsClear=0;

    countdownToNextPacketPair=15;

    nextAllowedThroughputSample=0;
    deadConnection = cheater = false;
    timeOfLastContinualSend=0;

    timeLastDatagramArrived=VENet::GetTimeMS();
    statistics.messagesInResendBuffer=0;
    statistics.bytesInResendBuffer=0;

    receivedPacketsBaseIndex=0;
    resetReceivedPackets=true;
    receivePacketCount=0;

    timeBetweenPackets=STARTING_TIME_BETWEEN_PACKETS;

    ackPingIndex=0;
    ackPingSum=(CCTimeType)0;

    nextSendTime=lastUpdateTime;
    unacknowledgedBytes=0;
    resendLinkedListHead=0;
    totalUserDataBytesAcked=0;

    datagramHistoryPopCount=0;

    InitHeapWeights();
    for (int i=0; i < NUMBER_OF_PRIORITIES; i++)
    {
        statistics.messageInSendBuffer[i]=0;
        statistics.bytesInSendBuffer[i]=0.0;
    }

    for (int i=0; i < RNS_PER_SECOND_METRICS_COUNT; i++)
    {
        bpsMetrics[i].Reset(_FILE_AND_LINE_);
    }
}

void ReliabilityLayer::FreeMemory( bool freeAllImmediately )
{
    (void) freeAllImmediately;
    FreeThreadSafeMemory();
}

void ReliabilityLayer::FreeThreadSafeMemory( void )
{
    unsigned i,j;
    InternalPacket *internalPacket;

    ClearPacketsAndDatagrams();

    for (i=0; i < splitPacketChannelList.Size(); i++)
    {
        for (j=0; j < splitPacketChannelList[i]->splitPacketList.Size(); j++)
        {
            FreeInternalPacketData(splitPacketChannelList[i]->splitPacketList[j], _FILE_AND_LINE_ );
            ReleaseToInternalPacketPool( splitPacketChannelList[i]->splitPacketList[j] );
        }
#if PREALLOCATE_LARGE_MESSAGES==1
        if (splitPacketChannelList[i]->returnedPacket)
        {
            FreeInternalPacketData(splitPacketChannelList[i]->returnedPacket, __FILE__, __LINE__ );
            ReleaseToInternalPacketPool( splitPacketChannelList[i]->returnedPacket );
        }
#endif
        VENet::OP_DELETE(splitPacketChannelList[i], __FILE__, __LINE__);
    }
    splitPacketChannelList.Clear(false, _FILE_AND_LINE_);

    while ( outputQueue.Size() > 0 )
    {
        internalPacket = outputQueue.Pop();
        FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
        ReleaseToInternalPacketPool( internalPacket );
    }

    outputQueue.ClearAndForceAllocation( 32, _FILE_AND_LINE_ );

    for (i=0; i < NUMBER_OF_ORDERED_STREAMS; i++)
    {
        for (j=0; j < orderingHeaps[i].Size(); j++)
        {
            FreeInternalPacketData(orderingHeaps[i][j], _FILE_AND_LINE_ );
            ReleaseToInternalPacketPool( orderingHeaps[i][j] );
        }
        orderingHeaps[i].Clear(true, _FILE_AND_LINE_);
    }

    memset(resendBuffer, 0, sizeof(resendBuffer));
    statistics.messagesInResendBuffer=0;
    statistics.bytesInResendBuffer=0;

    if (resendLinkedListHead)
    {
        InternalPacket *prev;
        InternalPacket *iter = resendLinkedListHead;
        while (1)
        {
            if (iter->data)
                FreeInternalPacketData(iter, _FILE_AND_LINE_ );
            prev=iter;
            iter=iter->resendNext;
            if (iter==resendLinkedListHead)
            {
                ReleaseToInternalPacketPool(prev);
                break;
            }
            ReleaseToInternalPacketPool(prev);
        }
        resendLinkedListHead=0;
    }
    unacknowledgedBytes=0;

    for ( j=0 ; j < outgoingPacketBuffer.Size(); j++ )
    {
        if ( outgoingPacketBuffer[ j ]->data)
            FreeInternalPacketData( outgoingPacketBuffer[ j ], _FILE_AND_LINE_ );
        ReleaseToInternalPacketPool( outgoingPacketBuffer[ j ] );
    }

    outgoingPacketBuffer.Clear(true, _FILE_AND_LINE_);

#ifdef _DEBUG
    for (unsigned i = 0; i < delayList.Size(); i++ )
        VENet::OP_DELETE(delayList[ i ], __FILE__, __LINE__);
    delayList.Clear(__FILE__, __LINE__);
#endif

    unreliableWithAckReceiptHistory.Clear(false, _FILE_AND_LINE_);

    packetsToSendThisUpdate.Clear(false, _FILE_AND_LINE_);
    packetsToSendThisUpdate.Preallocate(512, _FILE_AND_LINE_);
    packetsToDeallocThisUpdate.Clear(false, _FILE_AND_LINE_);
    packetsToDeallocThisUpdate.Preallocate(512, _FILE_AND_LINE_);
    packetsToSendThisUpdateDatagramBoundaries.Clear(false, _FILE_AND_LINE_);
    packetsToSendThisUpdateDatagramBoundaries.Preallocate(128, _FILE_AND_LINE_);
    datagramSizesInBytes.Clear(false, _FILE_AND_LINE_);
    datagramSizesInBytes.Preallocate(128, _FILE_AND_LINE_);

    internalPacketPool.Clear(_FILE_AND_LINE_);

    refCountedDataPool.Clear(_FILE_AND_LINE_);

    while (datagramHistory.Size())
    {
        RemoveFromDatagramHistory(datagramHistoryPopCount);
        datagramHistory.Pop();
        datagramHistoryPopCount++;
    }
    datagramHistoryMessagePool.Clear(_FILE_AND_LINE_);
    datagramHistoryPopCount=0;

    acknowlegements.Clear();
    NAKs.Clear();

    unreliableLinkedListHead=0;
}

bool ReliabilityLayer::HandleSocketReceiveFromConnectedPlayer(
    const char *buffer, unsigned int length, SystemAddress &systemAddress, DataStructures::List<PluginInterface2*> &messageHandlerList, int MTUSize,
    SOCKET s, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, CCTimeType timeRead,
    BitStream &updateBitStream)
{
#ifdef _DEBUG
    VEAssert( !( buffer == 0 ) );
#endif

#if CC_TIME_TYPE_BYTES==4
    timeRead/=1000;
#endif


    bpsMetrics[(int) ACTUAL_BYTES_RECEIVED].Push1(timeRead,length);

    (void) MTUSize;

    if ( length <= 2 || buffer == 0 )
    {
        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("length <= 2 || buffer == 0", BYTES_TO_BITS(length), systemAddress, true);
        return true;
    }

    timeLastDatagramArrived=VENet::GetTimeMS();

    DatagramSequenceNumberType holeCount;
    unsigned i;

#if LIBCAT_SECURITY==1
    if (useSecurity)
    {
        unsigned int received = length;

        if (!auth_enc.Decrypt((cat::u8*)buffer, received))
            return false;

        length = received;
    }
#endif

    VENet::BitStream socketData( (unsigned char*) buffer, length, false );


    DatagramHeaderFormat dhf;
    dhf.Deserialize(&socketData);
    if (dhf.isValid==false)
    {
        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("dhf.isValid==false", BYTES_TO_BITS(length), systemAddress, true);

        return true;
    }
    if (dhf.isACK)
    {
        DatagramSequenceNumberType datagramNumber;

#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
        VENet::TimeMS timeMSLow=(VENet::TimeMS) timeRead&0xFFFFFFFF;
        CCTimeType rtt = timeMSLow-dhf.sourceSystemTime;
#if CC_TIME_TYPE_BYTES==4
        if (rtt > 10000)
#else
        if (rtt > 10000000)
#endif
        {
            rtt=(CCTimeType) congestionManager.GetRTT();
        }
        ackPing=rtt;
#endif

#ifdef _DEBUG
        if (dhf.hasBAndAS==false)
        {
            dhf.AS=0;
        }
#endif
        incomingAcks.Clear();
        if (incomingAcks.Deserialize(&socketData)==false)
        {
            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("incomingAcks.Deserialize failed", BYTES_TO_BITS(length), systemAddress, true);

            return false;
        }
        for (i=0; i<incomingAcks.ranges.Size(); i++)
        {
            if (incomingAcks.ranges[i].minIndex>incomingAcks.ranges[i].maxIndex)
            {
                VEAssert(incomingAcks.ranges[i].minIndex<=incomingAcks.ranges[i].maxIndex);

                for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                    messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("incomingAcks minIndex > maxIndex", BYTES_TO_BITS(length), systemAddress, true);
                return false;
            }
            for (datagramNumber=incomingAcks.ranges[i].minIndex; datagramNumber >= incomingAcks.ranges[i].minIndex && datagramNumber <= incomingAcks.ranges[i].maxIndex; datagramNumber++)
            {
                CCTimeType whenSent;

                if (unreliableWithAckReceiptHistory.Size()>0)
                {
                    unsigned int k=0;
                    while (k < unreliableWithAckReceiptHistory.Size())
                    {
                        if (unreliableWithAckReceiptHistory[k].datagramNumber == datagramNumber)
                        {
                            InternalPacket *ackReceipt = AllocateFromInternalPacketPool();
                            AllocInternalPacketData(ackReceipt, 5,  false, _FILE_AND_LINE_ );
                            ackReceipt->dataBitLength=BYTES_TO_BITS(5);
                            ackReceipt->data[0]=(MessageID)ID_SND_RECEIPT_ACKED;
                            memcpy(ackReceipt->data+sizeof(MessageID), &unreliableWithAckReceiptHistory[k].sendReceiptSerial, sizeof(uint32_t));
                            outputQueue.Push(ackReceipt, _FILE_AND_LINE_ );

                            unreliableWithAckReceiptHistory.RemoveAtIndex(k);
                        }
                        else
                            k++;
                    }
                }

                MessageNumberNode *messageNumberNode = GetMessageNumberNodeByDatagramIndex(datagramNumber, &whenSent);
                if (messageNumberNode)
                {
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
                    congestionManager.OnAck(timeRead, rtt, dhf.hasBAndAS, 0, dhf.AS, totalUserDataBytesAcked, bandwidthExceededStatistic, datagramNumber );
#else
                    CCTimeType ping;
                    if (timeRead>whenSent)
                        ping=timeRead-whenSent;
                    else
                        ping=0;
                    congestionManager.OnAck(timeRead, ping, dhf.hasBAndAS, 0, dhf.AS, totalUserDataBytesAcked, bandwidthExceededStatistic, datagramNumber );
#endif
                    while (messageNumberNode)
                    {
                        RemovePacketFromResendListAndDeleteOlderReliableSequenced( messageNumberNode->messageNumber, timeRead, messageHandlerList, systemAddress );
                        messageNumberNode=messageNumberNode->next;
                    }

                    RemoveFromDatagramHistory(datagramNumber);
                }
            }
        }
    }
    else if (dhf.isNAK)
    {
        DatagramSequenceNumberType messageNumber;
        DataStructures::RangeList<DatagramSequenceNumberType> incomingNAKs;
        if (incomingNAKs.Deserialize(&socketData)==false)
        {
            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("incomingNAKs.Deserialize failed", BYTES_TO_BITS(length), systemAddress, true);

            return false;
        }
        for (i=0; i<incomingNAKs.ranges.Size(); i++)
        {
            if (incomingNAKs.ranges[i].minIndex>incomingNAKs.ranges[i].maxIndex)
            {
                VEAssert(incomingNAKs.ranges[i].minIndex<=incomingNAKs.ranges[i].maxIndex);

                for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                    messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("incomingNAKs minIndex>maxIndex", BYTES_TO_BITS(length), systemAddress, true);

                return false;
            }
            for (messageNumber=incomingNAKs.ranges[i].minIndex; messageNumber >= incomingNAKs.ranges[i].minIndex && messageNumber <= incomingNAKs.ranges[i].maxIndex; messageNumber++)
            {
                congestionManager.OnNAK(timeRead, messageNumber);

                CCTimeType timeSent;
                MessageNumberNode *messageNumberNode = GetMessageNumberNodeByDatagramIndex(messageNumber, &timeSent);
                while (messageNumberNode)
                {
                    InternalPacket *internalPacket = resendBuffer[messageNumberNode->messageNumber & (uint32_t) RESEND_BUFFER_ARRAY_MASK];
                    if (internalPacket)
                    {
                        if (internalPacket->nextActionTime!=0)
                        {
                            internalPacket->nextActionTime=timeRead;
                        }
                    }

                    messageNumberNode=messageNumberNode->next;
                }
            }
        }
    }
    else
    {
        uint32_t skippedMessageCount;
        if (!congestionManager.OnGotPacket(dhf.datagramNumber, dhf.isContinuousSend, timeRead, length, &skippedMessageCount))
        {
            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("congestionManager.OnGotPacket failed", BYTES_TO_BITS(length), systemAddress, true);

            return true;
        }
        if (dhf.isPacketPair)
            congestionManager.OnGotPacketPair(dhf.datagramNumber, length, timeRead);

        DatagramHeaderFormat dhfNAK;
        dhfNAK.isNAK=true;
        uint32_t skippedMessageOffset;
        for (skippedMessageOffset=skippedMessageCount; skippedMessageOffset > 0; skippedMessageOffset--)
        {
            NAKs.Insert(dhf.datagramNumber-skippedMessageOffset);
        }
        remoteSystemNeedsBAndAS=dhf.needsBAndAs;

#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
        SendAcknowledgementPacket( dhf.datagramNumber, dhf.sourceSystemTime);
#else
        SendAcknowledgementPacket( dhf.datagramNumber, 0);
#endif

        InternalPacket* internalPacket = CreateInternalPacketFromBitStream( &socketData, timeRead );
        if (internalPacket==0)
        {
            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("CreateInternalPacketFromBitStream failed", BYTES_TO_BITS(length), systemAddress, true);

            return true;
        }

        while ( internalPacket )
        {
            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
            {
#if CC_TIME_TYPE_BYTES==4
                messageHandlerList[messageHandlerIndex]->OnInternalPacket(internalPacket, receivePacketCount, systemAddress, timeRead, false);
#else
                messageHandlerList[messageHandlerIndex]->OnInternalPacket(internalPacket, receivePacketCount, systemAddress, (VENet::TimeMS)(timeRead/(CCTimeType)1000), false);
#endif
            }

            {
                if (resetReceivedPackets)
                {
                    hasReceivedPacketQueue.ClearAndForceAllocation(DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE, _FILE_AND_LINE_);
                    receivedPacketsBaseIndex=0;
                    resetReceivedPackets=false;
                }

                if (
                    internalPacket->reliability == RELIABLE_SEQUENCED ||
                    internalPacket->reliability == UNRELIABLE_SEQUENCED ||
                    internalPacket->reliability == RELIABLE_ORDERED
                )
                {
                    if ( internalPacket->orderingChannel >= NUMBER_OF_ORDERED_STREAMS )
                    {
                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("internalPacket->orderingChannel >= NUMBER_OF_ORDERED_STREAMS", BYTES_TO_BITS(length), systemAddress, true);

                        bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));

                        FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
                        ReleaseToInternalPacketPool( internalPacket );
                        goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                    }
                }

                if (internalPacket->reliability == RELIABLE || internalPacket->reliability == RELIABLE_SEQUENCED || internalPacket->reliability == RELIABLE_ORDERED )
                {
                    holeCount = (DatagramSequenceNumberType)(internalPacket->reliableMessageNumber-receivedPacketsBaseIndex);
                    const DatagramSequenceNumberType typeRange = (DatagramSequenceNumberType)(const uint32_t)-1;

                    if (holeCount==(DatagramSequenceNumberType) 0)
                    {
                        if (hasReceivedPacketQueue.Size())
                            hasReceivedPacketQueue.Pop();
                        ++receivedPacketsBaseIndex;
                    }
                    else if (holeCount > typeRange/(DatagramSequenceNumberType) 2)
                    {
                        bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));

                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("holeCount > typeRange/(DatagramSequenceNumberType) 2", BYTES_TO_BITS(length), systemAddress, false);

                        FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
                        ReleaseToInternalPacketPool( internalPacket );

                        goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                    }
                    else if ((unsigned int) holeCount<hasReceivedPacketQueue.Size())
                    {
                        if (hasReceivedPacketQueue[holeCount]!=false)
                        {
#ifdef LOG_TRIVIAL_NOTIFICATIONS
                            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("Higher count pushed to hasReceivedPacketQueue", BYTES_TO_BITS(length), systemAddress, false);
#endif
                            hasReceivedPacketQueue[holeCount]=false;
                        }
                        else
                        {
                            bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));

#ifdef LOG_TRIVIAL_NOTIFICATIONS
                            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("Duplicate packet ignored", BYTES_TO_BITS(length), systemAddress, false);
#endif
                            FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
                            ReleaseToInternalPacketPool( internalPacket );

                            goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                        }
                    }
                    else
                    {
                        if (holeCount > (DatagramSequenceNumberType) 1000000)
                        {
                            VEAssert("Hole count too high. See ReliabilityLayer.h" && 0);

                            for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                                messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("holeCount > 1000000", BYTES_TO_BITS(length), systemAddress, true);

                            bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));

                            FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
                            ReleaseToInternalPacketPool( internalPacket );

                            goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                        }

#ifdef LOG_TRIVIAL_NOTIFICATIONS
                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("Adding to hasReceivedPacketQueue later ordered message", BYTES_TO_BITS(length), systemAddress, false);
#endif

                        while ((unsigned int)(holeCount) > hasReceivedPacketQueue.Size())
                            hasReceivedPacketQueue.Push(true, _FILE_AND_LINE_ ); // time+(CCTimeType)60 * (CCTimeType)1000 * (CCTimeType)1000); // Didn't get this packet - set the time to give up waiting
                        hasReceivedPacketQueue.Push(false, _FILE_AND_LINE_ ); // Got the packet
#ifdef _DEBUG
                        VEAssert(hasReceivedPacketQueue.Size() < (unsigned int)((DatagramSequenceNumberType)(const uint32_t)(-1)));
#endif
                    }

                    while ( hasReceivedPacketQueue.Size()>0 && hasReceivedPacketQueue.Peek()==false )
                    {
                        hasReceivedPacketQueue.Pop();
                        ++receivedPacketsBaseIndex;
                    }
                }

                if (hasReceivedPacketQueue.AllocationSize() > (unsigned int) DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE && hasReceivedPacketQueue.AllocationSize() > hasReceivedPacketQueue.Size() * 3)
                    hasReceivedPacketQueue.Compress(_FILE_AND_LINE_);

                if ( internalPacket->splitPacketCount > 0 )
                {
                    if ( internalPacket->reliability != RELIABLE_ORDERED && internalPacket->reliability!=RELIABLE_SEQUENCED && internalPacket->reliability!=UNRELIABLE_SEQUENCED)
                        internalPacket->orderingChannel = 255;

                    InsertIntoSplitPacketList( internalPacket, timeRead );

                    internalPacket = BuildPacketFromSplitPacketList( internalPacket->splitPacketId, timeRead,
                                     s, systemAddress, rnr, remotePortVENetWasStartedOn_PS3, extraSocketOptions, updateBitStream);

                    if ( internalPacket == 0 )
                    {
#ifdef LOG_TRIVIAL_NOTIFICATIONS
                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("BuildPacketFromSplitPacketList did not return anything.", BYTES_TO_BITS(length), systemAddress, false);
#endif
                        goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                    }
                }

#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                unsigned char packetId;
                char *type="UNDEFINED";
#endif
                if (internalPacket->reliability == RELIABLE_SEQUENCED ||
                        internalPacket->reliability == UNRELIABLE_SEQUENCED ||
                        internalPacket->reliability == RELIABLE_ORDERED)
                {
#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST

                    // ___________________
                    BitStream bitStream(internalPacket->data, BITS_TO_BYTES(internalPacket->dataBitLength), false);
                    unsigned int receivedPacketNumber;
                    VENet::Time receivedTime;
                    unsigned char streamNumber;
                    PacketReliability reliability;
                    // ___________________


                    bitStream.IgnoreBits(8);
                    bitStream.Read(receivedTime);
                    bitStream.Read(packetId);
                    bitStream.Read(receivedPacketNumber);
                    bitStream.Read(streamNumber);
                    bitStream.Read(reliability);
                    if (packetId==ID_USER_PACKET_ENUM+1)
                    {

                        if (reliability==UNRELIABLE_SEQUENCED)
                            type="UNRELIABLE_SEQUENCED";
                        else if (reliability==RELIABLE_ORDERED)
                            type="RELIABLE_ORDERED";
                        else
                            type="RELIABLE_SEQUENCED";
                    }
                    // ___________________
#endif


                    if (internalPacket->orderingIndex==orderedReadIndex[internalPacket->orderingChannel])
                    {
                        if (internalPacket->reliability == RELIABLE_SEQUENCED ||
                                internalPacket->reliability == UNRELIABLE_SEQUENCED)
                        {
                            if (IsOlderOrderedPacket(internalPacket->sequencingIndex,highestSequencedReadIndex[internalPacket->orderingChannel])==false)
                            {
#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                                {
                                    fprintf(fp, "Returning %i, %s by fallthrough. OI=%i. SI=%i.\n", receivedPacketNumber, type, internalPacket->orderingIndex.val, internalPacket->sequencingIndex);
                                    fflush(fp);
                                }

                                if (packetId==ID_USER_PACKET_ENUM+1)
                                {
                                    if (receivedPacketNumber<packetNumber)
                                    {
                                        if (fp)
                                        {
                                            fprintf(fp, "Out of order packet from fallthrough! Expecting %i got %i\n", receivedPacketNumber, packetNumber);
                                            fflush(fp);
                                        }
                                    }
                                    packetNumber=receivedPacketNumber+1;
                                }
#endif
                                highestSequencedReadIndex[internalPacket->orderingChannel] = internalPacket->sequencingIndex+(OrderingIndexType)1;

                            }
                            else
                            {
#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                                {
                                    fprintf(fp, "Discarding %i, %s late sequenced. OI=%i. SI=%i.\n", receivedPacketNumber, type, internalPacket->orderingIndex.val, internalPacket->sequencingIndex);
                                    fflush(fp);
                                }
#endif

#ifdef LOG_TRIVIAL_NOTIFICATIONS
                                for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                                    messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("Sequenced rejected: lower than highest known value", BYTES_TO_BITS(length), systemAddress, false);
#endif
                                FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
                                ReleaseToInternalPacketPool( internalPacket );

                                goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                            }
                        }
                        else
                        {
                            bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_PROCESSED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));
                            outputQueue.Push( internalPacket, _FILE_AND_LINE_  );

#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                            if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                            {
                                fprintf(fp, "outputting immediate %i, %s. OI=%i. SI=%i.", receivedPacketNumber, type, internalPacket->orderingIndex.val, internalPacket->sequencingIndex);
                                if (orderingHeaps[internalPacket->orderingChannel].Size()==0)
                                    fprintf(fp, "heap empty\n");
                                else
                                    fprintf(fp, "heap head=%i\n", orderingHeaps[internalPacket->orderingChannel].Peek()->orderingIndex.val);

                                if (receivedPacketNumber<packetNumber)
                                {
                                    if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                                    {
                                        fprintf(fp, "Out of order packet arrived! Expecting %i got %i\n", receivedPacketNumber, packetNumber);
                                        fflush(fp);
                                    }
                                }
                                packetNumber=receivedPacketNumber+1;

                                fflush(fp);
                            }
#endif

                            orderedReadIndex[internalPacket->orderingChannel]++;
                            highestSequencedReadIndex[internalPacket->orderingChannel] = 0;

                            while (orderingHeaps[internalPacket->orderingChannel].Size()>0 &&
                                    orderingHeaps[internalPacket->orderingChannel].Peek()->orderingIndex==orderedReadIndex[internalPacket->orderingChannel])
                            {
                                internalPacket = orderingHeaps[internalPacket->orderingChannel].Pop(0);

#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                BitStream bitStream2(internalPacket->data, BITS_TO_BYTES(internalPacket->dataBitLength), false);
                                bitStream2.IgnoreBits(8);
                                bitStream2.Read(receivedTime);
                                bitStream2.IgnoreBits(8);
                                bitStream2.Read(receivedPacketNumber);
                                bitStream2.Read(streamNumber);
                                bitStream2.Read(reliability);
                                char *type="UNDEFINED";
                                if (reliability==UNRELIABLE_SEQUENCED)
                                    type="UNRELIABLE_SEQUENCED";
                                else if (reliability==RELIABLE_ORDERED)
                                    type="RELIABLE_ORDERED";

                                if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                                {
                                    fprintf(fp, "Heap pop %i, %s. OI=%i. SI=%i.\n", receivedPacketNumber, type, internalPacket->orderingIndex.val, internalPacket->sequencingIndex);
                                    fflush(fp);

                                    if (receivedPacketNumber<packetNumber)
                                    {
                                        if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                                        {
                                            fprintf(fp, "Out of order packet from heap! Expecting %i got %i\n", receivedPacketNumber, packetNumber);
                                            fflush(fp);
                                        }
                                    }
                                    packetNumber=receivedPacketNumber+1;
                                }
#endif

                                bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_PROCESSED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));
                                outputQueue.Push( internalPacket, _FILE_AND_LINE_  );

                                if (internalPacket->reliability == RELIABLE_ORDERED)
                                {
                                    orderedReadIndex[internalPacket->orderingChannel]++;
                                }
                                else
                                {
                                    highestSequencedReadIndex[internalPacket->orderingChannel] = internalPacket->sequencingIndex;
                                }
                            }

                            goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                        }
                    }
                    else if (IsOlderOrderedPacket(internalPacket->orderingIndex,orderedReadIndex[internalPacket->orderingChannel])==false)
                    {
                        if (orderingHeaps[internalPacket->orderingChannel].Size()==0)
                            heapIndexOffsets[internalPacket->orderingChannel]=orderedReadIndex[internalPacket->orderingChannel];

                        reliabilityHeapWeightType orderedHoleCount = internalPacket->orderingIndex-heapIndexOffsets[internalPacket->orderingChannel];
                        reliabilityHeapWeightType weight = orderedHoleCount*1048576;
                        if (internalPacket->reliability == RELIABLE_SEQUENCED ||
                                internalPacket->reliability == UNRELIABLE_SEQUENCED)
                            weight+=internalPacket->sequencingIndex;
                        else
                            weight+=(1048576-1);
                        orderingHeaps[internalPacket->orderingChannel].Push(weight, internalPacket, _FILE_AND_LINE_);

#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                        if (packetId==ID_USER_PACKET_ENUM+1 && fp)
                        {
                            fprintf(fp, "Heap push %i, %s, weight=%"PRINTF_64_BIT_MODIFIER"u. OI=%i. waiting on %i. SI=%i.\n", receivedPacketNumber, type, weight, internalPacket->orderingIndex.val, orderedReadIndex[internalPacket->orderingChannel].val, internalPacket->sequencingIndex);
                            fflush(fp);
                        }
#endif

#ifdef LOG_TRIVIAL_NOTIFICATIONS
                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("Larger number ordered packet leaving holes", BYTES_TO_BITS(length), systemAddress, false);
#endif
                        goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                    }
                    else
                    {
                        FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
                        ReleaseToInternalPacketPool( internalPacket );

#ifdef LOG_TRIVIAL_NOTIFICATIONS
                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                            messageHandlerList[messageHandlerIndex]->OnReliabilityLayerNotification("Rejected older resend", BYTES_TO_BITS(length), systemAddress, false);
#endif

                        goto CONTINUE_SOCKET_DATA_PARSE_LOOP;
                    }

                }

                bpsMetrics[(int) USER_MESSAGE_BYTES_RECEIVED_PROCESSED].Push1(timeRead,BITS_TO_BYTES(internalPacket->dataBitLength));

                outputQueue.Push( internalPacket, _FILE_AND_LINE_  );

                internalPacket = 0;
            }

CONTINUE_SOCKET_DATA_PARSE_LOOP:
            internalPacket = CreateInternalPacketFromBitStream( &socketData, timeRead );
        }

    }


    receivePacketCount++;

    return true;
}

BitSize_t ReliabilityLayer::Receive( unsigned char **data )
{
    InternalPacket * internalPacket;

    if ( outputQueue.Size() > 0 )
    {
        internalPacket = outputQueue.Pop();

        BitSize_t bitLength;
        *data = internalPacket->data;
        bitLength = internalPacket->dataBitLength;
        ReleaseToInternalPacketPool( internalPacket );
        return bitLength;
    }

    else
    {
        return 0;
    }

}

bool ReliabilityLayer::Send( char *data, BitSize_t numberOfBitsToSend, PacketPriority priority, PacketReliability reliability, unsigned char orderingChannel, bool makeDataCopy, int MTUSize, CCTimeType currentTime, uint32_t receipt )
{
#ifdef _DEBUG
    VEAssert( !( reliability >= NUMBER_OF_RELIABILITIES || reliability < 0 ) );
    VEAssert( !( priority > NUMBER_OF_PRIORITIES || priority < 0 ) );
    VEAssert( !( orderingChannel >= NUMBER_OF_ORDERED_STREAMS ) );
    VEAssert( numberOfBitsToSend > 0 );
#endif

#if CC_TIME_TYPE_BYTES==4
    currentTime/=1000;
#endif

    (void) MTUSize;


    if ( reliability > RELIABLE_ORDERED_WITH_ACK_RECEIPT || reliability < 0 )
        reliability = RELIABLE;

    if ( priority > NUMBER_OF_PRIORITIES || priority < 0 )
        priority = HIGH_PRIORITY;

    if ( orderingChannel >= NUMBER_OF_ORDERED_STREAMS )
        orderingChannel = 0;

    unsigned int numberOfBytesToSend=(unsigned int) BITS_TO_BYTES(numberOfBitsToSend);
    if ( numberOfBitsToSend == 0 )
    {
        return false;
    }
    InternalPacket * internalPacket = AllocateFromInternalPacketPool();
    if (internalPacket==0)
    {
        notifyOutOfMemory(_FILE_AND_LINE_);
        return false;
    }

    bpsMetrics[(int) USER_MESSAGE_BYTES_PUSHED].Push1(currentTime,numberOfBytesToSend);

    internalPacket->creationTime = currentTime;

    if ( makeDataCopy )
    {
        AllocInternalPacketData(internalPacket, numberOfBytesToSend, true, _FILE_AND_LINE_ );
        memcpy( internalPacket->data, data, numberOfBytesToSend );
    }
    else
    {
        AllocInternalPacketData(internalPacket, (unsigned char*) data );
    }

    internalPacket->dataBitLength = numberOfBitsToSend;
    internalPacket->messageInternalOrder = internalOrderIndex++;
    internalPacket->priority = priority;
    internalPacket->reliability = reliability;
    internalPacket->sendReceiptSerial=receipt;


    unsigned int maxDataSizeBytes = GetMaxDatagramSizeExcludingMessageHeaderBytes() - BITS_TO_BYTES(GetMaxMessageHeaderLengthBits());

    bool splitPacket = numberOfBytesToSend > maxDataSizeBytes;

    if ( splitPacket )
    {
        if (internalPacket->reliability==UNRELIABLE)
            internalPacket->reliability=RELIABLE;
        else if (internalPacket->reliability==UNRELIABLE_WITH_ACK_RECEIPT)
            internalPacket->reliability=RELIABLE_WITH_ACK_RECEIPT;
        else if (internalPacket->reliability==UNRELIABLE_SEQUENCED)
            internalPacket->reliability=RELIABLE_SEQUENCED;
    }


    if ( internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == UNRELIABLE_SEQUENCED
       )
    {
        internalPacket->orderingChannel = orderingChannel;
        internalPacket->orderingIndex = orderedWriteIndex[ orderingChannel ];
        internalPacket->sequencingIndex = sequencedWriteIndex[ orderingChannel ]++;
    }
    else if ( internalPacket->reliability == RELIABLE_ORDERED || internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT )
    {
        internalPacket->orderingChannel = orderingChannel;
        internalPacket->orderingIndex = orderedWriteIndex[ orderingChannel ] ++;
        sequencedWriteIndex[ orderingChannel ]=0;
    }

    if ( splitPacket )
    {
        SplitPacket( internalPacket );
        return true;
    }

    VEAssert(internalPacket->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
    AddToUnreliableLinkedList(internalPacket);

    VEAssert(internalPacket->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
    VEAssert(internalPacket->messageNumberAssigned==false);
    outgoingPacketBuffer.Push( GetNextWeight(internalPacket->priority), internalPacket, _FILE_AND_LINE_  );
    VEAssert(outgoingPacketBuffer.Size()==0 || outgoingPacketBuffer.Peek()->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
    statistics.messageInSendBuffer[(int)internalPacket->priority]++;
    statistics.bytesInSendBuffer[(int)internalPacket->priority]+=(double) BITS_TO_BYTES(internalPacket->dataBitLength);

    return true;
}

void ReliabilityLayer::Update( SOCKET s, SystemAddress &systemAddress, int MTUSize, CCTimeType time,
                               unsigned bitsPerSecondLimit,
                               DataStructures::List<PluginInterface2*> &messageHandlerList,
                               VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions,
                               BitStream &updateBitStream)

{
    (void) MTUSize;

    VENet::TimeMS timeMs;
#if CC_TIME_TYPE_BYTES==4
    time/=1000;
    timeMs=time;
#else
    timeMs=(VENet::TimeMS) (time/(CCTimeType)1000);
#endif

#ifdef _DEBUG
    while (delayList.Size())
    {
        if (delayList.Peek()->sendTime <= timeMs)
        {
            DataAndTime *dat = delayList.Pop();
            SocketLayer::SendTo( dat->s, dat->data, dat->length, systemAddress, dat->remotePortVENetWasStartedOn_PS3, dat->extraSocketOptions, __FILE__, __LINE__  );
            VENet::OP_DELETE(dat,__FILE__,__LINE__);
        }
        else
        {
            break;
        }
    }
#endif

    if (time <= lastUpdateTime)
    {
        lastUpdateTime=time;
        return;
    }

    CCTimeType timeSinceLastTick = time - lastUpdateTime;
    lastUpdateTime=time;
#if CC_TIME_TYPE_BYTES==4
    if (timeSinceLastTick>100)
        timeSinceLastTick=100;
#else
    if (timeSinceLastTick>100000)
        timeSinceLastTick=100000;
#endif

    if (unreliableTimeout>0)
    {
        if (timeSinceLastTick>=timeToNextUnreliableCull)
        {
            if (unreliableLinkedListHead)
            {
                InternalPacket *cur = unreliableLinkedListHead;
                InternalPacket *end = unreliableLinkedListHead->unreliablePrev;
                while (1)
                {
                    if (time > cur->creationTime+(CCTimeType)unreliableTimeout)
                    {
                        FreeInternalPacketData(cur, _FILE_AND_LINE_ );
                        cur->data=0;
                        InternalPacket *next = cur->unreliableNext;
                        RemoveFromUnreliableLinkedList(cur);

                        if (cur==end)
                            break;

                        cur=next;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            timeToNextUnreliableCull=unreliableTimeout/(CCTimeType)2;
        }
        else
        {
            timeToNextUnreliableCull-=timeSinceLastTick;
        }
    }


#if CC_TIME_TYPE_BYTES==4
    if ( statistics.messagesInResendBuffer!=0 && AckTimeout(time) )
#else
    if ( statistics.messagesInResendBuffer!=0 && AckTimeout(VENet::TimeMS(time/(CCTimeType)1000)) )
#endif
    {
        deadConnection = true;
        return;
    }

    if (congestionManager.ShouldSendACKs(time,timeSinceLastTick))
    {
        SendACKs(s, systemAddress, time, rnr, remotePortVENetWasStartedOn_PS3, extraSocketOptions, updateBitStream);
    }

    if (NAKs.Size()>0)
    {
        updateBitStream.Reset();
        DatagramHeaderFormat dhfNAK;
        dhfNAK.isNAK=true;
        dhfNAK.isACK=false;
        dhfNAK.isPacketPair=false;
        dhfNAK.Serialize(&updateBitStream);
        NAKs.Serialize(&updateBitStream, GetMaxDatagramSizeExcludingMessageHeaderBits(), true);
        SendBitStream( s, systemAddress, &updateBitStream, rnr, remotePortVENetWasStartedOn_PS3, extraSocketOptions, time );
    }

    DatagramHeaderFormat dhf;
    dhf.needsBAndAs=congestionManager.GetIsInSlowStart();
    dhf.isContinuousSend=bandwidthExceededStatistic;
    bandwidthExceededStatistic=outgoingPacketBuffer.Size()>0;

    const bool hasDataToSendOrResend = IsResendQueueEmpty()==false || bandwidthExceededStatistic;
    VEAssert(NUMBER_OF_PRIORITIES==4);
    congestionManager.Update(time, hasDataToSendOrResend);

    statistics.BPSLimitByOutgoingBandwidthLimit = BITS_TO_BYTES(bitsPerSecondLimit);
    statistics.BPSLimitByCongestionControl = congestionManager.GetBytesPerSecondLimitByCongestionControl();

    unsigned int i;
    if (time > lastBpsClear+
#if CC_TIME_TYPE_BYTES==4
            100
#else
            100000
#endif
       )
    {
        for (i=0; i < RNS_PER_SECOND_METRICS_COUNT; i++)
        {
            bpsMetrics[i].ClearExpired1(time);
        }

        lastBpsClear=time;
    }

    if (unreliableWithAckReceiptHistory.Size()>0)
    {
        i=0;
        while (i < unreliableWithAckReceiptHistory.Size())
        {
            if (time - unreliableWithAckReceiptHistory[i].nextActionTime < (((CCTimeType)-1)/2) )
            {
                InternalPacket *ackReceipt = AllocateFromInternalPacketPool();
                AllocInternalPacketData(ackReceipt, 5,  false, _FILE_AND_LINE_ );
                ackReceipt->dataBitLength=BYTES_TO_BITS(5);
                ackReceipt->data[0]=(MessageID)ID_SND_RECEIPT_LOSS;
                memcpy(ackReceipt->data+sizeof(MessageID), &unreliableWithAckReceiptHistory[i].sendReceiptSerial, sizeof(uint32_t));
                outputQueue.Push(ackReceipt, _FILE_AND_LINE_ );

                unreliableWithAckReceiptHistory.RemoveAtIndex(i);
            }
            else
                i++;
        }
    }

    if (hasDataToSendOrResend==true)
    {
        InternalPacket *internalPacket;
        bool pushedAnything;
        BitSize_t nextPacketBitLength;
        dhf.isACK=false;
        dhf.isNAK=false;
        dhf.hasBAndAS=false;
        ResetPacketsAndDatagrams();

        int transmissionBandwidth = congestionManager.GetTransmissionBandwidth(time, timeSinceLastTick, unacknowledgedBytes,dhf.isContinuousSend);
        int retransmissionBandwidth = congestionManager.GetRetransmissionBandwidth(time, timeSinceLastTick, unacknowledgedBytes,dhf.isContinuousSend);
        if (retransmissionBandwidth>0 || transmissionBandwidth>0)
        {
            statistics.isLimitedByCongestionControl=false;

            allDatagramSizesSoFar=0;

            while ((int)BITS_TO_BYTES(allDatagramSizesSoFar)<retransmissionBandwidth)
            {
                pushedAnything=false;

                while ( IsResendQueueEmpty()==false )
                {
                    internalPacket = resendLinkedListHead;
                    VEAssert(internalPacket->messageNumberAssigned==true);

                    if ( time - internalPacket->nextActionTime < (((CCTimeType)-1)/2) )
                    {
                        nextPacketBitLength = internalPacket->headerLength + internalPacket->dataBitLength;
                        if ( datagramSizeSoFar + nextPacketBitLength > GetMaxDatagramSizeExcludingMessageHeaderBits() )
                        {
                            PushDatagram();
                            break;
                        }

                        PopListHead(false);

                        CC_DEBUG_PRINTF_2("Rs %i ", internalPacket->reliableMessageNumber.val);

                        bpsMetrics[(int) USER_MESSAGE_BYTES_RESENT].Push1(time,BITS_TO_BYTES(internalPacket->dataBitLength));

                        PushPacket(time,internalPacket,true);
                        internalPacket->timesSent++;
                        congestionManager.OnResend(time, internalPacket->nextActionTime);
                        internalPacket->retransmissionTime = congestionManager.GetRTOForRetransmission(internalPacket->timesSent);
                        internalPacket->nextActionTime = internalPacket->retransmissionTime+time;

                        pushedAnything=true;

                        for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                        {
#if CC_TIME_TYPE_BYTES==4
                            messageHandlerList[messageHandlerIndex]->OnInternalPacket(internalPacket, packetsToSendThisUpdateDatagramBoundaries.Size()+congestionManager.GetNextDatagramSequenceNumber(), systemAddress, (VENet::TimeMS) time, true);
#else
                            messageHandlerList[messageHandlerIndex]->OnInternalPacket(internalPacket, packetsToSendThisUpdateDatagramBoundaries.Size()+congestionManager.GetNextDatagramSequenceNumber(), systemAddress, (VENet::TimeMS)(time/(CCTimeType)1000), true);
#endif
                        }

                        InsertPacketIntoResendList( internalPacket, time, false, false );

                    }
                    else
                    {
                        PushDatagram();
                        break;
                    }
                }

                if (pushedAnything==false)
                    break;
            }
        }
        else
        {
            statistics.isLimitedByCongestionControl=true;
        }

        if ((int)BITS_TO_BYTES(allDatagramSizesSoFar)<transmissionBandwidth)
        {
            allDatagramSizesSoFar=0;

            while (
                ResendBufferOverflow()==false &&
                ((int)BITS_TO_BYTES(allDatagramSizesSoFar)<transmissionBandwidth ||
                 (countdownToNextPacketPair==0 &&
                  datagramsToSendThisUpdateIsPair.Size()==1))
            )
            {
                pushedAnything=false;

                statistics.isLimitedByOutgoingBandwidthLimit=bitsPerSecondLimit!=0 && BITS_TO_BYTES(bitsPerSecondLimit) < bpsMetrics[USER_MESSAGE_BYTES_SENT].GetBPS1(time);


                while (outgoingPacketBuffer.Size() &&
                        statistics.isLimitedByOutgoingBandwidthLimit==false)
                {
                    internalPacket=outgoingPacketBuffer.Peek();
                    VEAssert(internalPacket->messageNumberAssigned==false);
                    VEAssert(outgoingPacketBuffer.Size()==0 || outgoingPacketBuffer.Peek()->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));

                    if (internalPacket->data==0)
                    {
                        outgoingPacketBuffer.Pop(0);
                        VEAssert(outgoingPacketBuffer.Size()==0 || outgoingPacketBuffer.Peek()->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
                        statistics.messageInSendBuffer[(int)internalPacket->priority]--;
                        statistics.bytesInSendBuffer[(int)internalPacket->priority]-=(double) BITS_TO_BYTES(internalPacket->dataBitLength);
                        ReleaseToInternalPacketPool( internalPacket );
                        continue;
                    }

                    internalPacket->headerLength=GetMessageHeaderLengthBits(internalPacket);
                    nextPacketBitLength = internalPacket->headerLength + internalPacket->dataBitLength;
                    if ( datagramSizeSoFar + nextPacketBitLength > GetMaxDatagramSizeExcludingMessageHeaderBits() )
                    {
                        VEAssert(datagramSizeSoFar!=0);
                        VEAssert(internalPacket->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
                        break;
                    }

                    bool isReliable;
                    if ( internalPacket->reliability == RELIABLE ||
                            internalPacket->reliability == RELIABLE_SEQUENCED ||
                            internalPacket->reliability == RELIABLE_ORDERED ||
                            internalPacket->reliability == RELIABLE_WITH_ACK_RECEIPT  ||
                            internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
                       )
                        isReliable = true;
                    else
                        isReliable = false;

                    outgoingPacketBuffer.Pop(0);
                    VEAssert(outgoingPacketBuffer.Size()==0 || outgoingPacketBuffer.Peek()->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
                    VEAssert(internalPacket->messageNumberAssigned==false);
                    statistics.messageInSendBuffer[(int)internalPacket->priority]--;
                    statistics.bytesInSendBuffer[(int)internalPacket->priority]-=(double) BITS_TO_BYTES(internalPacket->dataBitLength);
                    if (isReliable)
                    {
                        internalPacket->messageNumberAssigned=true;
                        internalPacket->reliableMessageNumber=sendReliableMessageNumberIndex;
                        internalPacket->retransmissionTime = congestionManager.GetRTOForRetransmission(internalPacket->timesSent+1);
                        internalPacket->nextActionTime = internalPacket->retransmissionTime+time;
#if CC_TIME_TYPE_BYTES==4
                        const CCTimeType threshhold = 10000;
#else
                        const CCTimeType threshhold = 10000000;
#endif
                        if (internalPacket->nextActionTime-time > threshhold)
                        {
                            //VEAssert(time-internalPacket->nextActionTime < threshhold);
                        }
                        if (resendBuffer[internalPacket->reliableMessageNumber & (uint32_t) RESEND_BUFFER_ARRAY_MASK]!=0)
                        {
                            VEAssert(0);
                        }
                        resendBuffer[internalPacket->reliableMessageNumber & (uint32_t) RESEND_BUFFER_ARRAY_MASK] = internalPacket;
                        statistics.messagesInResendBuffer++;
                        statistics.bytesInResendBuffer+=BITS_TO_BYTES(internalPacket->dataBitLength);

                        InsertPacketIntoResendList( internalPacket, time, true, isReliable);

                        sendReliableMessageNumberIndex++;
                    }
                    else if (internalPacket->reliability == UNRELIABLE_WITH_ACK_RECEIPT)
                    {
                        unreliableWithAckReceiptHistory.Push(UnreliableWithAckReceiptNode(
                                                                 congestionManager.GetNextDatagramSequenceNumber() + packetsToSendThisUpdateDatagramBoundaries.Size(),
                                                                 internalPacket->sendReceiptSerial,
                                                                 congestionManager.GetRTOForRetransmission(internalPacket->timesSent+1)+time
                                                             ), _FILE_AND_LINE_);
                    }

                    bpsMetrics[(int) USER_MESSAGE_BYTES_SENT].Push1(time,BITS_TO_BYTES(internalPacket->dataBitLength));

                    PushPacket(time,internalPacket, isReliable);
                    internalPacket->timesSent++;

                    for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
                    {
#if CC_TIME_TYPE_BYTES==4
                        messageHandlerList[messageHandlerIndex]->OnInternalPacket(internalPacket, packetsToSendThisUpdateDatagramBoundaries.Size()+congestionManager.GetNextDatagramSequenceNumber(), systemAddress, (VENet::TimeMS)time, true);
#else
                        messageHandlerList[messageHandlerIndex]->OnInternalPacket(internalPacket, packetsToSendThisUpdateDatagramBoundaries.Size()+congestionManager.GetNextDatagramSequenceNumber(), systemAddress, (VENet::TimeMS)(time/(CCTimeType)1000), true);
#endif
                    }
                    pushedAnything=true;

                    if (ResendBufferOverflow())
                        break;
                }
                if (datagramSizeSoFar==0)
                    break;

                PushDatagram();
            }
        }


        for (unsigned int datagramIndex=0; datagramIndex < packetsToSendThisUpdateDatagramBoundaries.Size(); datagramIndex++)
        {
            if (datagramIndex>0)
                dhf.isContinuousSend=true;
            MessageNumberNode* messageNumberNode = 0;
            dhf.datagramNumber=congestionManager.GetAndIncrementNextDatagramSequenceNumber();
            dhf.isPacketPair=datagramsToSendThisUpdateIsPair[datagramIndex];


            bool isSecondOfPacketPair=dhf.isPacketPair && datagramIndex>0 &&  datagramsToSendThisUpdateIsPair[datagramIndex-1];
            unsigned int msgIndex, msgTerm;
            if (datagramIndex==0)
            {
                msgIndex=0;
                msgTerm=packetsToSendThisUpdateDatagramBoundaries[0];
            }
            else
            {
                msgIndex=packetsToSendThisUpdateDatagramBoundaries[datagramIndex-1];
                msgTerm=packetsToSendThisUpdateDatagramBoundaries[datagramIndex];
            }

#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
            dhf.sourceSystemTime=VENet::GetTimeUS();
#endif
            updateBitStream.Reset();
            dhf.Serialize(&updateBitStream);
            CC_DEBUG_PRINTF_2("S%i ",dhf.datagramNumber.val);

            while (msgIndex < msgTerm)
            {
                if ( packetsToSendThisUpdate[msgIndex]->reliability != UNRELIABLE &&
                        packetsToSendThisUpdate[msgIndex]->reliability != UNRELIABLE_SEQUENCED
                   )
                {
                    if (messageNumberNode==0)
                    {
                        messageNumberNode = AddFirstToDatagramHistory(dhf.datagramNumber, packetsToSendThisUpdate[msgIndex]->reliableMessageNumber, time);
                    }
                    else
                    {
                        messageNumberNode = AddSubsequentToDatagramHistory(messageNumberNode, packetsToSendThisUpdate[msgIndex]->reliableMessageNumber);
                    }
                }

                VEAssert(updateBitStream.GetNumberOfBytesUsed()<=MAXIMUM_MTU_SIZE-UDP_HEADER_SIZE);
                WriteToBitStreamFromInternalPacket( &updateBitStream, packetsToSendThisUpdate[msgIndex], time );
                VEAssert(updateBitStream.GetNumberOfBytesUsed()<=MAXIMUM_MTU_SIZE-UDP_HEADER_SIZE);
                msgIndex++;
            }

            if (isSecondOfPacketPair)
            {
                VEAssert(updateBitStream.GetNumberOfBytesUsed()<=MAXIMUM_MTU_SIZE-UDP_HEADER_SIZE);
                updateBitStream.PadWithZeroToByteLength(datagramSizesInBytes[datagramIndex-1]);
                VEAssert(updateBitStream.GetNumberOfBytesUsed()<=MAXIMUM_MTU_SIZE-UDP_HEADER_SIZE);
            }

            if (messageNumberNode==0)
            {
                AddFirstToDatagramHistory(dhf.datagramNumber, time);
            }

            congestionManager.OnSendBytes(time,UDP_HEADER_SIZE+DatagramHeaderFormat::GetDataHeaderByteLength());

            SendBitStream( s, systemAddress, &updateBitStream, rnr, remotePortVENetWasStartedOn_PS3, extraSocketOptions, time );

            bandwidthExceededStatistic=outgoingPacketBuffer.Size()>0;


            if (bandwidthExceededStatistic==true)
                timeOfLastContinualSend=time;
            else
                timeOfLastContinualSend=0;
        }

        ClearPacketsAndDatagrams();

        bandwidthExceededStatistic=outgoingPacketBuffer.Size()>0;
    }

}

void ReliabilityLayer::SendBitStream( SOCKET s, SystemAddress &systemAddress, VENet::BitStream *bitStream, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, CCTimeType currentTime)
{
    (void) systemAddress;
    (void) rnr;

    unsigned int length;

    length = (unsigned int) bitStream->GetNumberOfBytesUsed();


#ifdef _DEBUG
    if (packetloss > 0.0)
    {
        if (frandomMT() < packetloss)
            return;
    }

    if (minExtraPing > 0 || extraPingVariance > 0)
    {
#ifdef FLIP_SEND_ORDER_TEST
        DataAndTime *dat = VENet::OP_NEW<DataAndTime>(__FILE__,__LINE__);
        memcpy(dat->data, ( char* ) bitStream->GetData(), length );
        dat->s=s;
        dat->length=length;
        dat->sendTime = 0;
        dat->remotePortVENetWasStartedOn_PS3=remotePortVENetWasStartedOn_PS3;
        dat->extraSocketOptions=extraSocketOptions;
        delayList.PushAtHead(dat, 0, _FILE_AND_LINE_);
#else
        VENet::TimeMS delay = minExtraPing;
        if (extraPingVariance>0)
            delay += (randomMT() % extraPingVariance);
        if (delay > 0)
        {
            DataAndTime *dat = VENet::OP_NEW<DataAndTime>(__FILE__,__LINE__);
            memcpy(dat->data, ( char* ) bitStream->GetData(), length );
            dat->s=s;
            dat->length=length;
            dat->sendTime = VENet::GetTimeMS() + delay;
            dat->remotePortVENetWasStartedOn_PS3=remotePortVENetWasStartedOn_PS3;
            dat->extraSocketOptions=extraSocketOptions;
            for (unsigned int i=0; i < delayList.Size(); i++)
            {
                if (dat->sendTime < delayList[i]->sendTime)
                {
                    delayList.PushAtHead(dat, i, __FILE__, __LINE__);
                    dat=0;
                    break;
                }
            }
            if (dat!=0)
                delayList.Push(dat,__FILE__,__LINE__);
            return;
        }
#endif
    }
#endif

#if LIBCAT_SECURITY==1
    if (useSecurity)
    {
        unsigned char *buffer = reinterpret_cast<unsigned char*>( bitStream->GetData() );

        int buffer_size = bitStream->GetNumberOfBitsAllocated() / 8;

        bool success = auth_enc.Encrypt(buffer, buffer_size, length);
        VEAssert(success);
    }
#endif

    bpsMetrics[(int) ACTUAL_BYTES_SENT].Push1(currentTime,length);

    VEAssert(length <= congestionManager.GetMTU());

#ifdef USE_THREADED_SEND
    SendToThread::SendToThreadBlock *block =  SendToThread::AllocateBlock();
    memcpy(block->data, bitStream->GetData(), length);
    block->dataWriteOffset=length;
    block->extraSocketOptions=extraSocketOptions;
    block->remotePortVENetWasStartedOn_PS3=remotePortVENetWasStartedOn_PS3;
    block->s=s;
    block->systemAddress=systemAddress;
    SendToThread::ProcessBlock(block);
#else
    SocketLayer::SendTo( s, ( char* ) bitStream->GetData(), length, systemAddress, remotePortVENetWasStartedOn_PS3, extraSocketOptions, __FILE__, __LINE__  );
#endif
}

bool ReliabilityLayer::IsOutgoingDataWaiting(void)
{
    if (outgoingPacketBuffer.Size()>0)
        return true;

    return
        statistics.messagesInResendBuffer!=0;
}
bool ReliabilityLayer::AreAcksWaiting(void)
{
    return acknowlegements.Size() > 0;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::ApplyNetworkSimulator( double _packetloss, VENet::TimeMS _minExtraPing, VENet::TimeMS _extraPingVariance )
{
#ifdef _DEBUG
    packetloss=_packetloss;
    minExtraPing=_minExtraPing;
    extraPingVariance=_extraPingVariance;
#endif
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::SetSplitMessageProgressInterval(int interval)
{
    splitMessageProgressInterval=interval;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::SetUnreliableTimeout(VENet::TimeMS timeoutMS)
{
#if CC_TIME_TYPE_BYTES==4
    unreliableTimeout=timeoutMS;
#else
    unreliableTimeout=(CCTimeType)timeoutMS*(CCTimeType)1000;
#endif
}

bool ReliabilityLayer::IsSendThrottled( int MTUSize )
{
    (void) MTUSize;

    return false;
}

void ReliabilityLayer::UpdateWindowFromPacketloss( CCTimeType time )
{
    (void) time;
}

void ReliabilityLayer::UpdateWindowFromAck( CCTimeType time )
{
    (void) time;
}

unsigned ReliabilityLayer::RemovePacketFromResendListAndDeleteOlderReliableSequenced( const MessageNumberType messageNumber, CCTimeType time, DataStructures::List<PluginInterface2*> &messageHandlerList, const SystemAddress &systemAddress )
{
    (void) time;
    (void) messageNumber;
    InternalPacket * internalPacket;

    for (unsigned int messageHandlerIndex=0; messageHandlerIndex < messageHandlerList.Size(); messageHandlerIndex++)
    {
#if CC_TIME_TYPE_BYTES==4
        messageHandlerList[messageHandlerIndex]->OnAck(messageNumber, systemAddress, time);
#else
        messageHandlerList[messageHandlerIndex]->OnAck(messageNumber, systemAddress, (VENet::TimeMS)(time/(CCTimeType)1000));
#endif
    }

    internalPacket = resendBuffer[messageNumber & RESEND_BUFFER_ARRAY_MASK];
    if (internalPacket && internalPacket->reliableMessageNumber==messageNumber)
    {
        resendBuffer[messageNumber & RESEND_BUFFER_ARRAY_MASK]=0;
        CC_DEBUG_PRINTF_2("AckRcv %i ", messageNumber);

        statistics.messagesInResendBuffer--;
        statistics.bytesInResendBuffer-=BITS_TO_BYTES(internalPacket->dataBitLength);

        totalUserDataBytesAcked+=(double) BITS_TO_BYTES(internalPacket->headerLength+internalPacket->dataBitLength);

        if (internalPacket->reliability>=RELIABLE_WITH_ACK_RECEIPT &&
                (internalPacket->splitPacketCount==0 || internalPacket->splitPacketIndex+1==internalPacket->splitPacketCount)
           )
        {
            InternalPacket *ackReceipt = AllocateFromInternalPacketPool();
            AllocInternalPacketData(ackReceipt, 5,  false, _FILE_AND_LINE_ );
            ackReceipt->dataBitLength=BYTES_TO_BITS(5);
            ackReceipt->data[0]=(MessageID)ID_SND_RECEIPT_ACKED;
            memcpy(ackReceipt->data+sizeof(MessageID), &internalPacket->sendReceiptSerial, sizeof(internalPacket->sendReceiptSerial));
            outputQueue.Push(ackReceipt, _FILE_AND_LINE_ );
        }

        bool isReliable;
        if ( internalPacket->reliability == RELIABLE ||
                internalPacket->reliability == RELIABLE_SEQUENCED ||
                internalPacket->reliability == RELIABLE_ORDERED ||
                internalPacket->reliability == RELIABLE_WITH_ACK_RECEIPT  ||
                internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
           )
            isReliable = true;
        else
            isReliable = false;

        RemoveFromList(internalPacket, isReliable);
        FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
        ReleaseToInternalPacketPool( internalPacket );


        return 0;
    }
    else
    {

    }

    return (unsigned)-1;
}

void ReliabilityLayer::SendAcknowledgementPacket( const DatagramSequenceNumberType messageNumber, CCTimeType time )
{
    nextAckTimeToSend=time;
    acknowlegements.Insert(messageNumber);


    CC_DEBUG_PRINTF_2("AckPush %i ", messageNumber);

}

BitSize_t ReliabilityLayer::GetMaxMessageHeaderLengthBits( void )
{
    InternalPacket ip;
    ip.reliability=RELIABLE_SEQUENCED;
    ip.splitPacketCount=1;
    return GetMessageHeaderLengthBits(&ip);
}
//-------------------------------------------------------------------------------------------------------
BitSize_t ReliabilityLayer::GetMessageHeaderLengthBits( const InternalPacket *const internalPacket )
{
    BitSize_t bitLength;

    bitLength = 8*1;

    bitLength += 8*2;

    if ( internalPacket->reliability == RELIABLE ||
            internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_ORDERED ||
            internalPacket->reliability == RELIABLE_WITH_ACK_RECEIPT ||
            internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
       )
        bitLength += 8*3;

    if ( internalPacket->reliability == UNRELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_SEQUENCED
       )
    {
        bitLength += 8*3;;
    }

    if ( internalPacket->reliability == UNRELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_ORDERED ||
            internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
       )
    {
        bitLength += 8*3;
        bitLength += 8*1;
    }
    if (internalPacket->splitPacketCount>0)
    {
        bitLength += 8*4;
        bitLength += 8*sizeof(SplitPacketIdType);
        bitLength += 8*4;
    }

    return bitLength;
}

BitSize_t ReliabilityLayer::WriteToBitStreamFromInternalPacket( VENet::BitStream *bitStream, const InternalPacket *const internalPacket, CCTimeType curTime )
{
    (void) curTime;

    BitSize_t start = bitStream->GetNumberOfBitsUsed();
    unsigned char tempChar;

    bitStream->AlignWriteToByteBoundary();
    if (internalPacket->reliability==UNRELIABLE_WITH_ACK_RECEIPT)
        tempChar=UNRELIABLE;
    else if (internalPacket->reliability==RELIABLE_WITH_ACK_RECEIPT)
        tempChar=RELIABLE;
    else if (internalPacket->reliability==RELIABLE_ORDERED_WITH_ACK_RECEIPT)
        tempChar=RELIABLE_ORDERED;
    else
        tempChar=(unsigned char)internalPacket->reliability;

    bitStream->WriteBits( (const unsigned char *)&tempChar, 3, true );

    bool hasSplitPacket = internalPacket->splitPacketCount>0;
    bitStream->Write(hasSplitPacket);
    bitStream->AlignWriteToByteBoundary();
    VEAssert(internalPacket->dataBitLength < 65535);
    unsigned short s;
    s = (unsigned short) internalPacket->dataBitLength;
    bitStream->WriteAlignedVar16((const char*)& s);
    if ( internalPacket->reliability == RELIABLE ||
            internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_ORDERED ||
            internalPacket->reliability == RELIABLE_WITH_ACK_RECEIPT ||
            internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
       )
        bitStream->Write(internalPacket->reliableMessageNumber);
    bitStream->AlignWriteToByteBoundary();

    if ( internalPacket->reliability == UNRELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_SEQUENCED
       )
    {
        bitStream->Write(internalPacket->sequencingIndex);
    }

    if ( internalPacket->reliability == UNRELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_ORDERED ||
            internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
       )
    {
        bitStream->Write(internalPacket->orderingIndex);
        tempChar=internalPacket->orderingChannel;
        bitStream->WriteAlignedVar8((const char*)& tempChar);
    }

    if (internalPacket->splitPacketCount>0)
    {
        bitStream->WriteAlignedVar32((const char*)& internalPacket->splitPacketCount);
        VEAssert(sizeof(SplitPacketIndexType)==4);
        bitStream->WriteAlignedVar16((const char*)& internalPacket->splitPacketId);
        VEAssert(sizeof(SplitPacketIdType)==2);
        bitStream->WriteAlignedVar32((const char*)& internalPacket->splitPacketIndex);
    }

    bitStream->WriteAlignedBytes( ( unsigned char* ) internalPacket->data, BITS_TO_BYTES( internalPacket->dataBitLength ) );

    return bitStream->GetNumberOfBitsUsed() - start;
}

InternalPacket* ReliabilityLayer::CreateInternalPacketFromBitStream( VENet::BitStream *bitStream, CCTimeType time )
{
    bool bitStreamSucceeded;
    InternalPacket* internalPacket;
    unsigned char tempChar;
    bool hasSplitPacket=false;
    bool readSuccess;

    if ( bitStream->GetNumberOfUnreadBits() < (int) sizeof( internalPacket->reliableMessageNumber ) * 8 )
        return 0;

    internalPacket = AllocateFromInternalPacketPool();
    if (internalPacket==0)
    {
        VEAssert(0);
        return 0;
    }
    internalPacket->creationTime = time;

    bitStream->AlignReadToByteBoundary();
    bitStream->ReadBits( ( unsigned char* ) ( &( tempChar ) ), 3 );
    internalPacket->reliability = ( const PacketReliability ) tempChar;
    readSuccess=bitStream->Read(hasSplitPacket);
    bitStream->AlignReadToByteBoundary();
    unsigned short s;
    bitStream->ReadAlignedVar16((char*)&s);
    internalPacket->dataBitLength=s;
    if ( internalPacket->reliability == RELIABLE ||
            internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_ORDERED
       )
        bitStream->Read(internalPacket->reliableMessageNumber);
    else
        internalPacket->reliableMessageNumber=(MessageNumberType)(const uint32_t)-1;
    bitStream->AlignReadToByteBoundary();

    if ( internalPacket->reliability == UNRELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_SEQUENCED
       )
    {
        bitStream->Read(internalPacket->sequencingIndex);
    }

    if ( internalPacket->reliability == UNRELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_SEQUENCED ||
            internalPacket->reliability == RELIABLE_ORDERED ||
            internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT
       )
    {
        bitStream->Read(internalPacket->orderingIndex);
        readSuccess=bitStream->ReadAlignedVar8((char*)& internalPacket->orderingChannel);
    }
    else
        internalPacket->orderingChannel=0;

    if (hasSplitPacket)
    {
        bitStream->ReadAlignedVar32((char*)& internalPacket->splitPacketCount);
        bitStream->ReadAlignedVar16((char*)& internalPacket->splitPacketId);
        readSuccess=bitStream->ReadAlignedVar32((char*)& internalPacket->splitPacketIndex);
        VEAssert(readSuccess);

    }
    else
    {
        internalPacket->splitPacketCount=0;
    }

    if (readSuccess==false ||
            internalPacket->dataBitLength==0 ||
            internalPacket->reliability>=NUMBER_OF_RELIABILITIES ||
            internalPacket->orderingChannel>=32 ||
            (hasSplitPacket && (internalPacket->splitPacketIndex >= internalPacket->splitPacketCount)))
    {
        VEAssert("Encoding is garbage" && 0);
        ReleaseToInternalPacketPool( internalPacket );
        return 0;
    }

    AllocInternalPacketData(internalPacket, BITS_TO_BYTES( internalPacket->dataBitLength ), false, _FILE_AND_LINE_ );
    VEAssert(BITS_TO_BYTES( internalPacket->dataBitLength )<MAXIMUM_MTU_SIZE);

    if (internalPacket->data == 0)
    {
        VEAssert("Out of memory in ReliabilityLayer::CreateInternalPacketFromBitStream" && 0);
        notifyOutOfMemory(_FILE_AND_LINE_);
        ReleaseToInternalPacketPool( internalPacket );
        return 0;
    }

    internalPacket->data[ BITS_TO_BYTES( internalPacket->dataBitLength ) - 1 ] = 0;

    bitStreamSucceeded = bitStream->ReadAlignedBytes( ( unsigned char* ) internalPacket->data, BITS_TO_BYTES( internalPacket->dataBitLength ) );

    if ( bitStreamSucceeded == false )
    {
        VEAssert("Couldn't read all the data"  && 0);

        FreeInternalPacketData(internalPacket, _FILE_AND_LINE_ );
        ReleaseToInternalPacketPool( internalPacket );
        return 0;
    }

    return internalPacket;
}

void ReliabilityLayer::GetSHA1( unsigned char * const buffer, unsigned int
                                nbytes, char code[ SHA1_LENGTH ] )
{
    CSHA1 sha1;

    sha1.Reset();
    sha1.Update( ( unsigned char* ) buffer, nbytes );
    sha1.Final();
    memcpy( code, sha1.GetHash(), SHA1_LENGTH );
}

bool ReliabilityLayer::CheckSHA1( char code[ SHA1_LENGTH ], unsigned char *
                                  const buffer, unsigned int nbytes )
{
    char code2[ SHA1_LENGTH ];
    GetSHA1( buffer, nbytes, code2 );

    for ( int i = 0; i < SHA1_LENGTH; i++ )
        if ( code[ i ] != code2[ i ] )
            return false;

    return true;
}

bool ReliabilityLayer::IsOlderOrderedPacket( OrderingIndexType newPacketOrderingIndex, OrderingIndexType waitingForPacketOrderingIndex )
{
    OrderingIndexType maxRange = (OrderingIndexType) (const uint32_t)-1;

    if ( waitingForPacketOrderingIndex > maxRange/(OrderingIndexType)2 )
    {
        if ( newPacketOrderingIndex >= waitingForPacketOrderingIndex - maxRange/(OrderingIndexType)2+(OrderingIndexType)1 && newPacketOrderingIndex < waitingForPacketOrderingIndex )
        {
            return true;
        }
    }

    else if ( newPacketOrderingIndex >= ( OrderingIndexType ) ( waitingForPacketOrderingIndex - (( OrderingIndexType ) maxRange/(OrderingIndexType)2+(OrderingIndexType)1) ) ||
              newPacketOrderingIndex < waitingForPacketOrderingIndex )
    {
        return true;
    }

    return false;
}

void ReliabilityLayer::SplitPacket( InternalPacket *internalPacket )
{
    internalPacket->splitPacketCount = 1;
    unsigned int headerLength = (unsigned int) BITS_TO_BYTES( GetMessageHeaderLengthBits( internalPacket ) );
    unsigned int dataByteLength = (unsigned int) BITS_TO_BYTES( internalPacket->dataBitLength );
    int maximumSendBlockBytes, byteOffset, bytesToSend;
    SplitPacketIndexType splitPacketIndex;
    int i;
    InternalPacket **internalPacketArray;

    maximumSendBlockBytes = GetMaxDatagramSizeExcludingMessageHeaderBytes() - BITS_TO_BYTES(GetMaxMessageHeaderLengthBits());

    internalPacket->splitPacketCount = ( ( dataByteLength - 1 ) / ( maximumSendBlockBytes ) + 1 );

    bool usedAlloca=false;

    if (sizeof( InternalPacket* ) * internalPacket->splitPacketCount < MAX_ALLOCA_STACK_ALLOCATION)
    {
        internalPacketArray = ( InternalPacket** ) alloca( sizeof( InternalPacket* ) * internalPacket->splitPacketCount );
        usedAlloca=true;
    }
    else

        internalPacketArray = (InternalPacket**) rakMalloc_Ex( sizeof(InternalPacket*) * internalPacket->splitPacketCount, _FILE_AND_LINE_ );

    for ( i = 0; i < ( int ) internalPacket->splitPacketCount; i++ )
    {
        internalPacketArray[ i ] = AllocateFromInternalPacketPool();

        *internalPacketArray[ i ]=*internalPacket;
        internalPacketArray[ i ]->messageNumberAssigned=false;

        if (i!=0)
            internalPacket->messageInternalOrder = internalOrderIndex++;
    }

    splitPacketIndex = 0;

    InternalPacketRefCountedData *refCounter=0;

    do
    {
        byteOffset = splitPacketIndex * maximumSendBlockBytes;
        bytesToSend = dataByteLength - byteOffset;

        if ( bytesToSend > maximumSendBlockBytes )
            bytesToSend = maximumSendBlockBytes;

        AllocInternalPacketData(internalPacketArray[ splitPacketIndex ], &refCounter, internalPacket->data, internalPacket->data + byteOffset);

        if ( bytesToSend != maximumSendBlockBytes )
            internalPacketArray[ splitPacketIndex ]->dataBitLength = internalPacket->dataBitLength - splitPacketIndex * ( maximumSendBlockBytes << 3 );
        else
            internalPacketArray[ splitPacketIndex ]->dataBitLength = bytesToSend << 3;

        internalPacketArray[ splitPacketIndex ]->splitPacketIndex = splitPacketIndex;
        internalPacketArray[ splitPacketIndex ]->splitPacketId = splitPacketId;
        internalPacketArray[ splitPacketIndex ]->splitPacketCount = internalPacket->splitPacketCount;
        VEAssert(internalPacketArray[ splitPacketIndex ]->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
    }
    while ( ++splitPacketIndex < internalPacket->splitPacketCount );

    splitPacketId++;

    VEAssert(outgoingPacketBuffer.Size()==0 || outgoingPacketBuffer.Peek()->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
    outgoingPacketBuffer.StartSeries();

    for ( i = 0; i < ( int ) internalPacket->splitPacketCount; i++ )
    {
        internalPacketArray[ i ]->headerLength=headerLength;
        VEAssert(internalPacketArray[ i ]->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
        AddToUnreliableLinkedList(internalPacketArray[ i ]);
        VEAssert(internalPacketArray[ i ]->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
        VEAssert(internalPacketArray[ i ]->messageNumberAssigned==false);
        outgoingPacketBuffer.PushSeries(GetNextWeight(internalPacketArray[ i ]->priority), internalPacketArray[ i ], _FILE_AND_LINE_);
        VEAssert(outgoingPacketBuffer.Size()==0 || outgoingPacketBuffer.Peek()->dataBitLength<BYTES_TO_BITS(MAXIMUM_MTU_SIZE));
        statistics.messageInSendBuffer[(int)internalPacketArray[ i ]->priority]++;
        statistics.bytesInSendBuffer[(int)(int)internalPacketArray[ i ]->priority]+=(double) BITS_TO_BYTES(internalPacketArray[ i ]->dataBitLength);
    }

    ReleaseToInternalPacketPool( internalPacket );

    if (usedAlloca==false)
        veFree_Ex(internalPacketArray, _FILE_AND_LINE_ );
}

void ReliabilityLayer::InsertIntoSplitPacketList( InternalPacket * internalPacket, CCTimeType time )
{
    bool objectExists;
    unsigned index;
    index=splitPacketChannelList.GetIndexFromKey(internalPacket->splitPacketId, &objectExists);
    if (objectExists==false)
    {
        SplitPacketChannel *newChannel = VENet::OP_NEW<SplitPacketChannel>( __FILE__, __LINE__ );
#if PREALLOCATE_LARGE_MESSAGES==1
        index=splitPacketChannelList.Insert(internalPacket->splitPacketId, newChannel, true, __FILE__,__LINE__);
        newChannel->returnedPacket=CreateInternalPacketCopy( internalPacket, 0, 0, time );
        newChannel->gotFirstPacket=false;
        newChannel->splitPacketsArrived=0;
        AllocInternalPacketData(newChannel->returnedPacket, BITS_TO_BYTES( internalPacket->dataBitLength*internalPacket->splitPacketCount ),  false, __FILE__, __LINE__ );
        VEAssert(newChannel->returnedPacket->data);
#else
        newChannel->firstPacket=0;
        index=splitPacketChannelList.Insert(internalPacket->splitPacketId, newChannel, true, __FILE__,__LINE__);
        newChannel->splitPacketList.Preallocate(internalPacket->splitPacketCount, __FILE__,__LINE__);

#endif
    }

#if PREALLOCATE_LARGE_MESSAGES==1
    splitPacketChannelList[index]->lastUpdateTime=time;
    splitPacketChannelList[index]->splitPacketsArrived++;
    splitPacketChannelList[index]->returnedPacket->dataBitLength+=internalPacket->dataBitLength;

    bool dealloc;
    if (internalPacket->splitPacketIndex==0)
    {
        splitPacketChannelList[index]->gotFirstPacket=true;
        splitPacketChannelList[index]->stride=BITS_TO_BYTES(internalPacket->dataBitLength);

        for (unsigned int j=0; j < splitPacketChannelList[index]->splitPacketList.Size(); j++)
        {
            memcpy(splitPacketChannelList[index]->returnedPacket->data+internalPacket->splitPacketIndex*splitPacketChannelList[index]->stride, internalPacket->data, (size_t) BITS_TO_BYTES(internalPacket->dataBitLength));
            FreeInternalPacketData(splitPacketChannelList[index]->splitPacketList[j], __FILE__, __LINE__ );
            ReleaseToInternalPacketPool(splitPacketChannelList[index]->splitPacketList[j]);
        }

        memcpy(splitPacketChannelList[index]->returnedPacket->data, internalPacket->data, (size_t) BITS_TO_BYTES(internalPacket->dataBitLength));
        splitPacketChannelList[index]->splitPacketList.Clear(true,__FILE__,__LINE__);
        dealloc=true;
    }
    else
    {
        if (splitPacketChannelList[index]->gotFirstPacket==true)
        {
            memcpy(splitPacketChannelList[index]->returnedPacket->data+internalPacket->splitPacketIndex*splitPacketChannelList[index]->stride, internalPacket->data, (size_t) BITS_TO_BYTES(internalPacket->dataBitLength));
            dealloc=true;
        }
        else
        {
            splitPacketChannelList[index]->splitPacketList.Push(internalPacket,__FILE__,__LINE__);
            dealloc=false;
        }
    }

    if (splitPacketChannelList[index]->gotFirstPacket==true &&
            splitMessageProgressInterval &&
            splitPacketChannelList[index]->gotFirstPacket &&
            splitPacketChannelList[index]->splitPacketsArrived!=splitPacketChannelList[index]->returnedPacket->splitPacketCount &&
            (splitPacketChannelList[index]->splitPacketsArrived%splitMessageProgressInterval)==0
       )
    {
        InternalPacket *progressIndicator = AllocateFromInternalPacketPool();
        unsigned int l = (unsigned int) splitPacketChannelList[index]->stride;
        const unsigned int len = sizeof(MessageID) + sizeof(unsigned int)*2 + sizeof(unsigned int) + l;
        AllocInternalPacketData(progressIndicator, len,  false, __FILE__, __LINE__ );
        progressIndicator->dataBitLength=BYTES_TO_BITS(len);
        progressIndicator->data[0]=(MessageID)ID_DOWNLOAD_PROGRESS;
        unsigned int temp;
        temp=splitPacketChannelList[index]->splitPacketsArrived;
        memcpy(progressIndicator->data+sizeof(MessageID), &temp, sizeof(unsigned int));
        temp=(unsigned int)internalPacket->splitPacketCount;
        memcpy(progressIndicator->data+sizeof(MessageID)+sizeof(unsigned int)*1, &temp, sizeof(unsigned int));
        temp=(unsigned int) BITS_TO_BYTES(l);
        memcpy(progressIndicator->data+sizeof(MessageID)+sizeof(unsigned int)*2, &temp, sizeof(unsigned int));
        memcpy(progressIndicator->data+sizeof(MessageID)+sizeof(unsigned int)*3, splitPacketChannelList[index]->returnedPacket->data, (size_t) BITS_TO_BYTES(l));
    }

    if (dealloc)
    {
        FreeInternalPacketData(internalPacket, __FILE__, __LINE__ );
        ReleaseToInternalPacketPool(internalPacket);
    }
#else
    splitPacketChannelList[index]->splitPacketList.Insert(internalPacket, __FILE__, __LINE__ );
    splitPacketChannelList[index]->lastUpdateTime=time;

    if (internalPacket->splitPacketIndex==0)
        splitPacketChannelList[index]->firstPacket=internalPacket;

    if (splitMessageProgressInterval &&
            splitPacketChannelList[index]->firstPacket &&
            splitPacketChannelList[index]->splitPacketList.Size()!=splitPacketChannelList[index]->firstPacket->splitPacketCount &&
            (splitPacketChannelList[index]->splitPacketList.Size()%splitMessageProgressInterval)==0)
    {
        InternalPacket *progressIndicator = AllocateFromInternalPacketPool();
        unsigned int length = sizeof(MessageID) + sizeof(unsigned int)*2 + sizeof(unsigned int) + (unsigned int) BITS_TO_BYTES(splitPacketChannelList[index]->firstPacket->dataBitLength);
        AllocInternalPacketData(progressIndicator, length,  false, __FILE__, __LINE__ );
        progressIndicator->dataBitLength=BYTES_TO_BITS(length);
        progressIndicator->data[0]=(MessageID)ID_DOWNLOAD_PROGRESS;
        unsigned int temp;
        temp=splitPacketChannelList[index]->splitPacketList.Size();
        memcpy(progressIndicator->data+sizeof(MessageID), &temp, sizeof(unsigned int));
        temp=(unsigned int)internalPacket->splitPacketCount;
        memcpy(progressIndicator->data+sizeof(MessageID)+sizeof(unsigned int)*1, &temp, sizeof(unsigned int));
        temp=(unsigned int) BITS_TO_BYTES(splitPacketChannelList[index]->firstPacket->dataBitLength);
        memcpy(progressIndicator->data+sizeof(MessageID)+sizeof(unsigned int)*2, &temp, sizeof(unsigned int));

        memcpy(progressIndicator->data+sizeof(MessageID)+sizeof(unsigned int)*3, splitPacketChannelList[index]->firstPacket->data, (size_t) BITS_TO_BYTES(splitPacketChannelList[index]->firstPacket->dataBitLength));
        outputQueue.Push(progressIndicator, __FILE__, __LINE__ );
    }

#endif
}

InternalPacket * ReliabilityLayer::BuildPacketFromSplitPacketList( SplitPacketChannel *splitPacketChannel, CCTimeType time )
{
#if PREALLOCATE_LARGE_MESSAGES==1
    InternalPacket *returnedPacket=splitPacketChannel->returnedPacket;
    VENet::OP_DELETE(splitPacketChannel, __FILE__, __LINE__);
    (void) time;
    return returnedPacket;
#else
    unsigned int j;
    InternalPacket * internalPacket, *splitPacket;
    int splitPacketPartLength;

    internalPacket = CreateInternalPacketCopy( splitPacketChannel->splitPacketList[0], 0, 0, time );
    internalPacket->dataBitLength=0;
    for (j=0; j < splitPacketChannel->splitPacketList.Size(); j++)
        internalPacket->dataBitLength+=splitPacketChannel->splitPacketList[j]->dataBitLength;
    splitPacketPartLength=BITS_TO_BYTES(splitPacketChannel->firstPacket->dataBitLength);

    internalPacket->data = (unsigned char*) rakMalloc_Ex( (size_t) BITS_TO_BYTES( internalPacket->dataBitLength ), _FILE_AND_LINE_ );
    internalPacket->allocationScheme=InternalPacket::NORMAL;

    for (j=0; j < splitPacketChannel->splitPacketList.Size(); j++)
    {
        splitPacket=splitPacketChannel->splitPacketList[j];
        memcpy(internalPacket->data+splitPacket->splitPacketIndex*splitPacketPartLength, splitPacket->data, (size_t) BITS_TO_BYTES(splitPacketChannel->splitPacketList[j]->dataBitLength));
    }

    for (j=0; j < splitPacketChannel->splitPacketList.Size(); j++)
    {
        FreeInternalPacketData(splitPacketChannel->splitPacketList[j], _FILE_AND_LINE_ );
        ReleaseToInternalPacketPool(splitPacketChannel->splitPacketList[j]);
    }
    VENet::OP_DELETE(splitPacketChannel, __FILE__, __LINE__);

    return internalPacket;
#endif
}
//-------------------------------------------------------------------------------------------------------
InternalPacket * ReliabilityLayer::BuildPacketFromSplitPacketList( SplitPacketIdType splitPacketId, CCTimeType time,
        SOCKET s, SystemAddress &systemAddress, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions,
        BitStream &updateBitStream)
{
    unsigned int i;
    bool objectExists;
    SplitPacketChannel *splitPacketChannel;
    InternalPacket * internalPacket;

    i=splitPacketChannelList.GetIndexFromKey(splitPacketId, &objectExists);
    splitPacketChannel=splitPacketChannelList[i];

#if PREALLOCATE_LARGE_MESSAGES==1
    if (splitPacketChannel->splitPacketsArrived==splitPacketChannel->returnedPacket->splitPacketCount)
#else
    if (splitPacketChannel->splitPacketList.Size()==splitPacketChannel->splitPacketList[0]->splitPacketCount)
#endif
    {
        SendACKs(s, systemAddress, time, rnr, remotePortVENetWasStartedOn_PS3, extraSocketOptions, updateBitStream);
        internalPacket=BuildPacketFromSplitPacketList(splitPacketChannel,time);
        splitPacketChannelList.RemoveAtIndex(i);
        return internalPacket;
    }
    else
    {
        return 0;
    }
}

InternalPacket * ReliabilityLayer::CreateInternalPacketCopy( InternalPacket *original, int dataByteOffset, int dataByteLength, CCTimeType time )
{
    InternalPacket * copy = AllocateFromInternalPacketPool();
#ifdef _DEBUG
    memset( copy, 255, sizeof( InternalPacket ) );
#endif
    if ( dataByteLength > 0 )
    {
        AllocInternalPacketData(copy, BITS_TO_BYTES(dataByteLength ),  false, _FILE_AND_LINE_ );
        memcpy( copy->data, original->data + dataByteOffset, dataByteLength );
    }
    else
        copy->data = 0;

    copy->dataBitLength = dataByteLength << 3;
    copy->creationTime = time;
    copy->nextActionTime = 0;
    copy->orderingIndex = original->orderingIndex;
    copy->sequencingIndex = original->sequencingIndex;
    copy->orderingChannel = original->orderingChannel;
    copy->reliableMessageNumber = original->reliableMessageNumber;
    copy->priority = original->priority;
    copy->reliability = original->reliability;
#if PREALLOCATE_LARGE_MESSAGES==1
    copy->splitPacketCount = original->splitPacketCount;
    copy->splitPacketId = original->splitPacketId;
    copy->splitPacketIndex = original->splitPacketIndex;
#endif

    return copy;
}

void ReliabilityLayer::InsertPacketIntoResendList( InternalPacket *internalPacket, CCTimeType time, bool firstResend, bool modifyUnacknowledgedBytes )
{
    (void) firstResend;
    (void) time;
    (void) internalPacket;

    AddToListTail(internalPacket, modifyUnacknowledgedBytes);
    VEAssert(internalPacket->nextActionTime!=0);

}

bool ReliabilityLayer::IsDeadConnection( void ) const
{
    return deadConnection;
}

void ReliabilityLayer::KillConnection( void )
{
    deadConnection=true;
}

VENetStatistics * ReliabilityLayer::GetStatistics( VENetStatistics *rns )
{
    unsigned i;
    VENet::TimeUS time = VENet::GetTimeUS();
    uint64_t uint64Denominator;
    double doubleDenominator;

    for (i=0; i < RNS_PER_SECOND_METRICS_COUNT; i++)
    {
        statistics.valueOverLastSecond[i]=bpsMetrics[i].GetBPS1Threadsafe(time);
        statistics.runningTotal[i]=bpsMetrics[i].GetTotal1();
    }

    memcpy(rns, &statistics, sizeof(statistics));

    if (rns->valueOverLastSecond[USER_MESSAGE_BYTES_SENT]+rns->valueOverLastSecond[USER_MESSAGE_BYTES_RESENT]>0)
        rns->packetlossLastSecond=(float)((double) rns->valueOverLastSecond[USER_MESSAGE_BYTES_RESENT]/((double) rns->valueOverLastSecond[USER_MESSAGE_BYTES_SENT]+(double) rns->valueOverLastSecond[USER_MESSAGE_BYTES_RESENT]));
    else
        rns->packetlossLastSecond=0.0f;

    rns->packetlossTotal=0.0f;
    uint64Denominator=(rns->runningTotal[USER_MESSAGE_BYTES_SENT]+rns->runningTotal[USER_MESSAGE_BYTES_RESENT]);
    if (uint64Denominator!=0&&rns->runningTotal[USER_MESSAGE_BYTES_SENT]/uint64Denominator>0)
    {
        doubleDenominator=((double) rns->runningTotal[USER_MESSAGE_BYTES_SENT]+(double) rns->runningTotal[USER_MESSAGE_BYTES_RESENT]);
        if (doubleDenominator!=0)
        {
            rns->packetlossTotal=(float)((double) rns->runningTotal[USER_MESSAGE_BYTES_RESENT]/doubleDenominator);
        }
    }

    rns->isLimitedByCongestionControl=statistics.isLimitedByCongestionControl;
    rns->BPSLimitByCongestionControl=statistics.BPSLimitByCongestionControl;
    rns->isLimitedByOutgoingBandwidthLimit=statistics.isLimitedByOutgoingBandwidthLimit;
    rns->BPSLimitByOutgoingBandwidthLimit=statistics.BPSLimitByOutgoingBandwidthLimit;

    return rns;
}

unsigned int ReliabilityLayer::GetResendListDataSize(void) const
{
    return statistics.messagesInResendBuffer;
}

//-------------------------------------------------------------------------------------------------------
bool ReliabilityLayer::AckTimeout(VENet::Time curTime)
{
    return (timeLastDatagramArrived-curTime)>10000 && curTime-timeLastDatagramArrived>timeoutTime;
}
//-------------------------------------------------------------------------------------------------------
CCTimeType ReliabilityLayer::GetNextSendTime(void) const
{
    return nextSendTime;
}
//-------------------------------------------------------------------------------------------------------
CCTimeType ReliabilityLayer::GetTimeBetweenPackets(void) const
{
    return timeBetweenPackets;
}
//-------------------------------------------------------------------------------------------------------
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
CCTimeType ReliabilityLayer::GetAckPing(void) const
{
    return ackPing;
}
#endif
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::ResetPacketsAndDatagrams(void)
{
    packetsToSendThisUpdate.Clear(true, _FILE_AND_LINE_);
    packetsToDeallocThisUpdate.Clear(true, _FILE_AND_LINE_);
    packetsToSendThisUpdateDatagramBoundaries.Clear(true, _FILE_AND_LINE_);
    datagramsToSendThisUpdateIsPair.Clear(true, _FILE_AND_LINE_);
    datagramSizesInBytes.Clear(true, _FILE_AND_LINE_);
    datagramSizeSoFar=0;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::PushPacket(CCTimeType time, InternalPacket *internalPacket, bool isReliable)
{
    BitSize_t bitsForThisPacket=BYTES_TO_BITS(BITS_TO_BYTES(internalPacket->dataBitLength)+BITS_TO_BYTES(internalPacket->headerLength));
    datagramSizeSoFar+=bitsForThisPacket;
    VEAssert(BITS_TO_BYTES(datagramSizeSoFar)<MAXIMUM_MTU_SIZE-UDP_HEADER_SIZE);
    allDatagramSizesSoFar+=bitsForThisPacket;
    packetsToSendThisUpdate.Push(internalPacket, _FILE_AND_LINE_ );
    packetsToDeallocThisUpdate.Push(isReliable==false, _FILE_AND_LINE_ );
    VEAssert(internalPacket->headerLength==GetMessageHeaderLengthBits(internalPacket));

    congestionManager.OnSendBytes(time, BITS_TO_BYTES(internalPacket->dataBitLength)+BITS_TO_BYTES(internalPacket->headerLength));
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::PushDatagram(void)
{
    if (datagramSizeSoFar>0)
    {
        packetsToSendThisUpdateDatagramBoundaries.Push(packetsToSendThisUpdate.Size(), _FILE_AND_LINE_ );
        datagramsToSendThisUpdateIsPair.Push(false, _FILE_AND_LINE_ );
        VEAssert(BITS_TO_BYTES(datagramSizeSoFar)<MAXIMUM_MTU_SIZE-UDP_HEADER_SIZE);
        datagramSizesInBytes.Push(BITS_TO_BYTES(datagramSizeSoFar), _FILE_AND_LINE_ );
        datagramSizeSoFar=0;
    }
}
//-------------------------------------------------------------------------------------------------------
bool ReliabilityLayer::TagMostRecentPushAsSecondOfPacketPair(void)
{
    if (datagramsToSendThisUpdateIsPair.Size()>=2)
    {
        datagramsToSendThisUpdateIsPair[datagramsToSendThisUpdateIsPair.Size()-2]=true;
        datagramsToSendThisUpdateIsPair[datagramsToSendThisUpdateIsPair.Size()-1]=true;
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::ClearPacketsAndDatagrams(void)
{
    unsigned int i;
    for (i=0; i < packetsToDeallocThisUpdate.Size(); i++)
    {
        if (packetsToDeallocThisUpdate[i])
        {
            RemoveFromUnreliableLinkedList(packetsToSendThisUpdate[i]);
            FreeInternalPacketData(packetsToSendThisUpdate[i], _FILE_AND_LINE_ );
            ReleaseToInternalPacketPool( packetsToSendThisUpdate[i] );
        }
    }
    packetsToDeallocThisUpdate.Clear(true, _FILE_AND_LINE_);
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::MoveToListHead(InternalPacket *internalPacket)
{
    if ( internalPacket == resendLinkedListHead )
        return;
    if (resendLinkedListHead==0)
    {
        internalPacket->resendNext=internalPacket;
        internalPacket->resendPrev=internalPacket;
        resendLinkedListHead=internalPacket;
        return;
    }
    internalPacket->resendPrev->resendNext = internalPacket->resendNext;
    internalPacket->resendNext->resendPrev = internalPacket->resendPrev;
    internalPacket->resendNext=resendLinkedListHead;
    internalPacket->resendPrev=resendLinkedListHead->resendPrev;
    internalPacket->resendPrev->resendNext=internalPacket;
    resendLinkedListHead->resendPrev=internalPacket;
    resendLinkedListHead=internalPacket;
    VEAssert(internalPacket->headerLength+internalPacket->dataBitLength>0);
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::RemoveFromList(InternalPacket *internalPacket, bool modifyUnacknowledgedBytes)
{
    InternalPacket *newPosition;
    internalPacket->resendPrev->resendNext = internalPacket->resendNext;
    internalPacket->resendNext->resendPrev = internalPacket->resendPrev;
    newPosition = internalPacket->resendNext;
    if ( internalPacket == resendLinkedListHead )
        resendLinkedListHead = newPosition;
    if (resendLinkedListHead==internalPacket)
        resendLinkedListHead=0;

    if (modifyUnacknowledgedBytes)
    {
        VEAssert(unacknowledgedBytes>=BITS_TO_BYTES(internalPacket->headerLength+internalPacket->dataBitLength));
        unacknowledgedBytes-=BITS_TO_BYTES(internalPacket->headerLength+internalPacket->dataBitLength);
    }
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::AddToListTail(InternalPacket *internalPacket, bool modifyUnacknowledgedBytes)
{
    if (modifyUnacknowledgedBytes)
    {
        unacknowledgedBytes+=BITS_TO_BYTES(internalPacket->headerLength+internalPacket->dataBitLength);
    }

    if (resendLinkedListHead==0)
    {
        internalPacket->resendNext=internalPacket;
        internalPacket->resendPrev=internalPacket;
        resendLinkedListHead=internalPacket;
        return;
    }
    internalPacket->resendNext=resendLinkedListHead;
    internalPacket->resendPrev=resendLinkedListHead->resendPrev;
    internalPacket->resendPrev->resendNext=internalPacket;
    resendLinkedListHead->resendPrev=internalPacket;

}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::PopListHead(bool modifyUnacknowledgedBytes)
{
    VEAssert(resendLinkedListHead!=0);
    RemoveFromList(resendLinkedListHead, modifyUnacknowledgedBytes);
}
//-------------------------------------------------------------------------------------------------------
bool ReliabilityLayer::IsResendQueueEmpty(void) const
{
    return resendLinkedListHead==0;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::SendACKs(SOCKET s, SystemAddress &systemAddress, CCTimeType time, VENetRandom *rnr, unsigned short remotePortVENetWasStartedOn_PS3, unsigned int extraSocketOptions, BitStream &updateBitStream)
{
    BitSize_t maxDatagramPayload = GetMaxDatagramSizeExcludingMessageHeaderBits();

    while (acknowlegements.Size()>0)
    {
        updateBitStream.Reset();
        DatagramHeaderFormat dhf;
        dhf.isACK=true;
        dhf.isNAK=false;
        dhf.isPacketPair=false;
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
        dhf.sourceSystemTime=time;
#endif
        double B;
        double AS;
        bool hasBAndAS;
        if (remoteSystemNeedsBAndAS)
        {
            congestionManager.OnSendAckGetBAndAS(time, &hasBAndAS,&B,&AS);
            dhf.AS=(float)AS;
            dhf.hasBAndAS=hasBAndAS;
        }
        else
            dhf.hasBAndAS=false;
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
        dhf.sourceSystemTime=nextAckTimeToSend;
#endif
        updateBitStream.Reset();
        dhf.Serialize(&updateBitStream);
        CC_DEBUG_PRINTF_1("AckSnd ");
        acknowlegements.Serialize(&updateBitStream, maxDatagramPayload, true);
        SendBitStream( s, systemAddress, &updateBitStream, rnr, remotePortVENetWasStartedOn_PS3, extraSocketOptions, time );
        congestionManager.OnSendAck(time,updateBitStream.GetNumberOfBytesUsed());
    }
}

InternalPacket* ReliabilityLayer::AllocateFromInternalPacketPool(void)
{
    InternalPacket *ip = internalPacketPool.Allocate( _FILE_AND_LINE_ );
    ip->reliableMessageNumber = (MessageNumberType) (const uint32_t)-1;
    ip->messageNumberAssigned=false;
    ip->nextActionTime = 0;
    ip->splitPacketCount = 0;
    ip->splitPacketIndex = 0;
    ip->splitPacketId = 0;
    ip->allocationScheme=InternalPacket::NORMAL;
    ip->data=0;
    ip->timesSent=0;
    return ip;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::ReleaseToInternalPacketPool(InternalPacket *ip)
{
    internalPacketPool.Release(ip, _FILE_AND_LINE_);
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::RemoveFromUnreliableLinkedList(InternalPacket *internalPacket)
{
    if (internalPacket->reliability==UNRELIABLE ||
            internalPacket->reliability==UNRELIABLE_SEQUENCED ||
            internalPacket->reliability==UNRELIABLE_WITH_ACK_RECEIPT
       )
    {
        InternalPacket *newPosition;
        internalPacket->unreliablePrev->unreliableNext = internalPacket->unreliableNext;
        internalPacket->unreliableNext->unreliablePrev = internalPacket->unreliablePrev;
        newPosition = internalPacket->unreliableNext;
        if ( internalPacket == unreliableLinkedListHead )
            unreliableLinkedListHead = newPosition;
        if (unreliableLinkedListHead==internalPacket)
            unreliableLinkedListHead=0;
    }
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::AddToUnreliableLinkedList(InternalPacket *internalPacket)
{
    if (internalPacket->reliability==UNRELIABLE ||
            internalPacket->reliability==UNRELIABLE_SEQUENCED ||
            internalPacket->reliability==UNRELIABLE_WITH_ACK_RECEIPT
       )
    {
        if (unreliableLinkedListHead==0)
        {
            internalPacket->unreliableNext=internalPacket;
            internalPacket->unreliablePrev=internalPacket;
            unreliableLinkedListHead=internalPacket;
            return;
        }
        internalPacket->unreliableNext=unreliableLinkedListHead;
        internalPacket->unreliablePrev=unreliableLinkedListHead->unreliablePrev;
        internalPacket->unreliablePrev->unreliableNext=internalPacket;
        unreliableLinkedListHead->unreliablePrev=internalPacket;
    }
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::ValidateResendList(void) const
{

}
//-------------------------------------------------------------------------------------------------------
bool ReliabilityLayer::ResendBufferOverflow(void) const
{
    int index1 = sendReliableMessageNumberIndex & (uint32_t) RESEND_BUFFER_ARRAY_MASK;
    VEAssert(index1<RESEND_BUFFER_ARRAY_LENGTH);
    return resendBuffer[index1]!=0;
}
//-------------------------------------------------------------------------------------------------------
ReliabilityLayer::MessageNumberNode* ReliabilityLayer::GetMessageNumberNodeByDatagramIndex(DatagramSequenceNumberType index, CCTimeType *timeSent)
{
    if (datagramHistory.IsEmpty())
        return 0;

    if (congestionManager.LessThan(index, datagramHistoryPopCount))
        return 0;

    DatagramSequenceNumberType offsetIntoList = index - datagramHistoryPopCount;
    if (offsetIntoList >= datagramHistory.Size())
        return 0;

    *timeSent=datagramHistory[offsetIntoList].timeSent;
    return datagramHistory[offsetIntoList].head;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::RemoveFromDatagramHistory(DatagramSequenceNumberType index)
{
    DatagramSequenceNumberType offsetIntoList = index - datagramHistoryPopCount;
    MessageNumberNode *mnm = datagramHistory[offsetIntoList].head;
    MessageNumberNode *next;
    while (mnm)
    {
        next=mnm->next;
        datagramHistoryMessagePool.Release(mnm, _FILE_AND_LINE_);
        mnm=next;
    }
    datagramHistory[offsetIntoList].head=0;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::AddFirstToDatagramHistory(DatagramSequenceNumberType datagramNumber, CCTimeType timeSent)
{
    (void) datagramNumber;
    if (datagramHistory.Size()>DATAGRAM_MESSAGE_ID_ARRAY_LENGTH)
    {
        RemoveFromDatagramHistory(datagramHistoryPopCount);
        datagramHistory.Pop();
        datagramHistoryPopCount++;
    }

    datagramHistory.Push(DatagramHistoryNode(0, timeSent), _FILE_AND_LINE_);
}
//-------------------------------------------------------------------------------------------------------
ReliabilityLayer::MessageNumberNode* ReliabilityLayer::AddFirstToDatagramHistory(DatagramSequenceNumberType datagramNumber, DatagramSequenceNumberType messageNumber, CCTimeType timeSent)
{
    (void) datagramNumber;
    if (datagramHistory.Size()>DATAGRAM_MESSAGE_ID_ARRAY_LENGTH)
    {
        RemoveFromDatagramHistory(datagramHistoryPopCount);
        datagramHistory.Pop();
        datagramHistoryPopCount++;
    }

    MessageNumberNode *mnm = datagramHistoryMessagePool.Allocate(_FILE_AND_LINE_);
    mnm->next=0;
    mnm->messageNumber=messageNumber;
    datagramHistory.Push(DatagramHistoryNode(mnm, timeSent), _FILE_AND_LINE_);
    return mnm;
}
//-------------------------------------------------------------------------------------------------------
ReliabilityLayer::MessageNumberNode* ReliabilityLayer::AddSubsequentToDatagramHistory(MessageNumberNode *messageNumberNode, DatagramSequenceNumberType messageNumber)
{
    messageNumberNode->next=datagramHistoryMessagePool.Allocate(_FILE_AND_LINE_);
    messageNumberNode->next->messageNumber=messageNumber;
    messageNumberNode->next->next=0;
    return messageNumberNode->next;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::AllocInternalPacketData(InternalPacket *internalPacket, InternalPacketRefCountedData **refCounter, unsigned char *externallyAllocatedPtr, unsigned char *ourOffset)
{
    internalPacket->allocationScheme=InternalPacket::REF_COUNTED;
    internalPacket->data=ourOffset;
    if (*refCounter==0)
    {
        *refCounter = refCountedDataPool.Allocate(_FILE_AND_LINE_);
        (*refCounter)->refCount=1;
        (*refCounter)->sharedDataBlock=externallyAllocatedPtr;
    }
    else
        (*refCounter)->refCount++;
    internalPacket->refCountedData=(*refCounter);
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::AllocInternalPacketData(InternalPacket *internalPacket, unsigned char *externallyAllocatedPtr)
{
    internalPacket->allocationScheme=InternalPacket::NORMAL;
    internalPacket->data=externallyAllocatedPtr;
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::AllocInternalPacketData(InternalPacket *internalPacket, unsigned int numBytes, bool allowStack, const char *file, unsigned int line)
{
    if (allowStack && numBytes <= sizeof(internalPacket->stackData))
    {
        internalPacket->allocationScheme=InternalPacket::STACK;
        internalPacket->data=internalPacket->stackData;
    }
    else
    {
        internalPacket->allocationScheme=InternalPacket::NORMAL;
        internalPacket->data=(unsigned char*) rakMalloc_Ex(numBytes,file,line);
    }
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::FreeInternalPacketData(InternalPacket *internalPacket, const char *file, unsigned int line)
{
    if (internalPacket==0)
        return;

    if (internalPacket->allocationScheme==InternalPacket::REF_COUNTED)
    {
        if (internalPacket->refCountedData==0)
            return;

        internalPacket->refCountedData->refCount--;
        if (internalPacket->refCountedData->refCount==0)
        {
            veFree_Ex(internalPacket->refCountedData->sharedDataBlock, file, line );
            internalPacket->refCountedData->sharedDataBlock=0;
            refCountedDataPool.Release(internalPacket->refCountedData,file, line);
            internalPacket->refCountedData=0;
        }
    }
    else if (internalPacket->allocationScheme==InternalPacket::NORMAL)
    {
        if (internalPacket->data==0)
            return;

        veFree_Ex(internalPacket->data, file, line );
        internalPacket->data=0;
    }
    else
    {
        internalPacket->data=0;
    }
}
//-------------------------------------------------------------------------------------------------------
unsigned int ReliabilityLayer::GetMaxDatagramSizeExcludingMessageHeaderBytes(void)
{
    unsigned int val = congestionManager.GetMTU() - DatagramHeaderFormat::GetDataHeaderByteLength();

#if LIBCAT_SECURITY==1
    if (useSecurity)
        val -= cat::AuthenticatedEncryption::OVERHEAD_BYTES;
#endif

    return val;
}
//-------------------------------------------------------------------------------------------------------
BitSize_t ReliabilityLayer::GetMaxDatagramSizeExcludingMessageHeaderBits(void)
{
    return BYTES_TO_BITS(GetMaxDatagramSizeExcludingMessageHeaderBytes());
}
//-------------------------------------------------------------------------------------------------------
void ReliabilityLayer::InitHeapWeights(void)
{
    for (int priorityLevel=0; priorityLevel < NUMBER_OF_PRIORITIES; priorityLevel++)
        outgoingPacketBufferNextWeights[priorityLevel]=(1<<priorityLevel)*priorityLevel+priorityLevel;
}
//-------------------------------------------------------------------------------------------------------
reliabilityHeapWeightType ReliabilityLayer::GetNextWeight(int priorityLevel)
{
    uint64_t next = outgoingPacketBufferNextWeights[priorityLevel];
    if (outgoingPacketBuffer.Size()>0)
    {
        int peekPL = outgoingPacketBuffer.Peek()->priority;
        reliabilityHeapWeightType weight = outgoingPacketBuffer.PeekWeight();
        reliabilityHeapWeightType min = weight - (1<<peekPL)*peekPL+peekPL;
        if (next<min)
            next=min + (1<<priorityLevel)*priorityLevel+priorityLevel;
        outgoingPacketBufferNextWeights[priorityLevel]=next+(1<<priorityLevel)*(priorityLevel+1)+priorityLevel;
    }
    else
    {
        InitHeapWeights();
    }
    return next;
}


#ifdef _MSC_VER
#pragma warning( pop )
#endif
