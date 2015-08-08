////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 CCVENetUDT.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "VENetDefines.h"

#if USE_SLIDING_WINDOW_CONGESTION_CONTROL!=1

#include "NativeTypes.h"
#include "VENetTime.h"
#include "VENetTypes.h"
#include "DS_Queue.h"

/// Set to 4 if you are using the iPod Touch TG.
#define CC_TIME_TYPE_BYTES 8

namespace VENet
{

typedef uint64_t CCTimeType;

typedef uint24_t DatagramSequenceNumberType;
typedef double BytesPerMicrosecond;
typedef double BytesPerSecond;
typedef double MicrosecondsPerByte;

#define CC_VENET_UDT_PACKET_HISTORY_LENGTH 64
#define RTT_HISTORY_LENGTH 64

#define UDP_HEADER_SIZE 28

#define CC_DEBUG_PRINTF_1(x)
#define CC_DEBUG_PRINTF_2(x,y)
#define CC_DEBUG_PRINTF_3(x,y,z)
#define CC_DEBUG_PRINTF_4(x,y,z,a)
#define CC_DEBUG_PRINTF_5(x,y,z,a,b)

class CCVENetUDT
{
public:

    CCVENetUDT();
    ~CCVENetUDT();

    void Init(CCTimeType curTime, uint32_t maxDatagramPayload);

    void Update(CCTimeType curTime, bool hasDataToSendOrResend);

    int GetRetransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend);
    int GetTransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend);

    bool ShouldSendACKs(CCTimeType curTime, CCTimeType estimatedTimeToNextTick);

    DatagramSequenceNumberType GetAndIncrementNextDatagramSequenceNumber(void);
    DatagramSequenceNumberType GetNextDatagramSequenceNumber(void);

    void OnSendBytes(CCTimeType curTime, uint32_t numBytes);

    void OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber, uint32_t sizeInBytes, CCTimeType curTime);

    bool OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber, bool isContinuousSend, CCTimeType curTime, uint32_t sizeInBytes, uint32_t *skippedMessageCount);

    void OnResend(CCTimeType curTime, VENet::TimeUS nextActionTime);
    void OnNAK(CCTimeType curTime, DatagramSequenceNumberType nakSequenceNumber);

    void OnAck(CCTimeType curTime, CCTimeType rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, double totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber );
    void OnDuplicateAck( CCTimeType curTime, DatagramSequenceNumberType sequenceNumber ) {}

    void OnSendAckGetBAndAS(CCTimeType curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS);

    void OnSendAck(CCTimeType curTime, uint32_t numBytes);

    void OnSendNACK(CCTimeType curTime, uint32_t numBytes);

    CCTimeType GetRTOForRetransmission(unsigned char timesSent) const;

    void SetMTU(uint32_t bytes);

    uint32_t GetMTU(void) const;

    BytesPerMicrosecond GetLocalSendRate(void) const
    {
        return 1.0 / SND;
    }
    BytesPerMicrosecond GetLocalReceiveRate(CCTimeType currentTime) const;
    BytesPerMicrosecond GetRemoveReceiveRate(void) const
    {
        return AS;
    }
    BytesPerMicrosecond GetEstimatedBandwidth(void) const
    {
        return GetLinkCapacityBytesPerSecond()*1000000.0;
    }
    double GetLinkCapacityBytesPerSecond(void) const
    {
        return estimatedLinkCapacityBytesPerSecond;
    };

    double GetRTT(void) const;

    bool GetIsInSlowStart(void) const
    {
        return isInSlowStart;
    }
    uint32_t GetCWNDLimit(void) const
    {
        return (uint32_t) (CWND*MAXIMUM_MTU_INCLUDING_UDP_HEADER);
    }

    static bool GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);
    static bool LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);
    uint64_t GetBytesPerSecondLimitByCongestionControl(void) const;

protected:
    MicrosecondsPerByte SND;

    double CWND;

    CCTimeType nextSYNUpdate;

    int packetPairRecieptHistoryWriteIndex;

    double RTT;

    double minRTT, maxRTT;

    BytesPerMicrosecond packetArrivalHistory[CC_VENET_UDT_PACKET_HISTORY_LENGTH];
    BytesPerMicrosecond packetArrivalHistoryContinuousGaps[CC_VENET_UDT_PACKET_HISTORY_LENGTH];
    unsigned char packetArrivalHistoryContinuousGapsIndex;
    uint64_t continuousBytesReceived;
    CCTimeType continuousBytesReceivedStartTime;
    unsigned int packetArrivalHistoryWriteCount;

    int packetArrivalHistoryWriteIndex;

    CCTimeType lastPacketArrivalTime;

    BytesPerMicrosecond AS;

    CCTimeType lastTransmitOfBAndAS;

    bool isInSlowStart;

    uint32_t NAKCount;

    uint32_t AvgNAKNum;

    uint32_t DecCount;

    uint32_t DecInterval;

    DatagramSequenceNumberType nextDatagramSequenceNumber;

    CCTimeType lastPacketPairPacketArrivalTime;

    DatagramSequenceNumberType lastPacketPairSequenceNumber;

    CCTimeType lastUpdateWindowSizeAndAck;

    double ExpCount;

    uint64_t totalUserDataBytesSent;

    CCTimeType oldestUnsentAck;

    uint32_t MAXIMUM_MTU_INCLUDING_UDP_HEADER;

    double CWND_MAX_THRESHOLD;

    DatagramSequenceNumberType expectedNextSequenceNumber;

    uint32_t sendBAndASCount;

    BytesPerMicrosecond mostRecentPacketArrivalHistory;

    bool hasWrittenToPacketPairReceiptHistory;

    double estimatedLinkCapacityBytesPerSecond;

    void SetNextSYNUpdate(CCTimeType currentTime);

    BytesPerMicrosecond ReceiverCalculateDataArrivalRate(CCTimeType curTime) const;

    BytesPerMicrosecond ReceiverCalculateDataArrivalRateMedian(void) const;

    static BytesPerMicrosecond CalculateListMedianRecursive(const BytesPerMicrosecond inputList[CC_VENET_UDT_PACKET_HISTORY_LENGTH], int inputListLength, int lessThanSum, int greaterThanSum);

    CCTimeType GetSenderRTOForACK(void) const;

    void EndSlowStart(void);

    inline double BytesPerMicrosecondToPacketsPerMillisecond(BytesPerMicrosecond in);

    void UpdateWindowSizeAndAckOnAckPreSlowStart(double totalUserDataBytesAcked);

    void UpdateWindowSizeAndAckOnAckPerSyn(CCTimeType curTime, CCTimeType rtt, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber);

    void ResetOnDataArrivalHalveSNDOnNoDataTime(CCTimeType curTime);

    void InitPacketArrivalHistory(void);

    void PrintLowBandwidthWarning(void);

    void CapMinSnd(const char *file, int line);

    void DecreaseTimeBetweenSends(void);
    void IncreaseTimeBetweenSends(void);

    int bytesCanSendThisTick;

    CCTimeType lastRttOnIncreaseSendRate;
    CCTimeType lastRtt;

    DatagramSequenceNumberType nextCongestionControlBlock;
    bool hadPacketlossThisBlock;
    DataStructures::Queue<CCTimeType> pingsLastInterval;
};

}

#endif
