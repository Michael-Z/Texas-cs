////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:  CCVENetSlidingWindow.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VENetDefines.h"

#if USE_SLIDING_WINDOW_CONGESTION_CONTROL==1

#include "NativeTypes.h"
#include "VENetTime.h"
#include "VENetTypes.h"
#include "DS_Queue.h"

#define UDP_HEADER_SIZE 28

#define CC_DEBUG_PRINTF_1(x)
#define CC_DEBUG_PRINTF_2(x,y)
#define CC_DEBUG_PRINTF_3(x,y,z)
#define CC_DEBUG_PRINTF_4(x,y,z,a)
#define CC_DEBUG_PRINTF_5(x,y,z,a,b)

/// Set to 4 if you are using the iPod Touch TG.
#define CC_TIME_TYPE_BYTES 8

typedef VENet::TimeUS CCTimeType;

typedef VENet::uint24_t DatagramSequenceNumberType;
typedef double BytesPerMicrosecond;
typedef double BytesPerSecond;
typedef double MicrosecondsPerByte;

namespace VENet
{

class CCVENetSlidingWindow
{
public:

    CCVENetSlidingWindow();
    ~CCVENetSlidingWindow();

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
    void OnDuplicateAck( CCTimeType curTime, DatagramSequenceNumberType sequenceNumber );

    void OnSendAckGetBAndAS(CCTimeType curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS);

    void OnSendAck(CCTimeType curTime, uint32_t numBytes);

    void OnSendNACK(CCTimeType curTime, uint32_t numBytes);

    CCTimeType GetRTOForRetransmission(unsigned char timesSent) const;

    void SetMTU(uint32_t bytes);

    uint32_t GetMTU(void) const;

    BytesPerMicrosecond GetLocalSendRate(void) const
    {
        return 0;
    }
    BytesPerMicrosecond GetLocalReceiveRate(CCTimeType currentTime) const;
    BytesPerMicrosecond GetRemoveReceiveRate(void) const
    {
        return 0;
    }
    BytesPerMicrosecond GetEstimatedBandwidth(void) const
    {
        return GetLinkCapacityBytesPerSecond()*1000000.0;
    }
    double GetLinkCapacityBytesPerSecond(void) const
    {
        return 0;
    }

    double GetRTT(void) const;

    bool GetIsInSlowStart(void) const
    {
        return IsInSlowStart();
    }
    uint32_t GetCWNDLimit(void) const
    {
        return (uint32_t) 0;
    }

    static bool GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);

    static bool LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);

    uint64_t GetBytesPerSecondLimitByCongestionControl(void) const;

protected:

    uint32_t MAXIMUM_MTU_INCLUDING_UDP_HEADER;

    double cwnd;
    double ssThresh;

    CCTimeType oldestUnsentAck;

    CCTimeType GetSenderRTOForACK(void) const;

    DatagramSequenceNumberType nextDatagramSequenceNumber;
    DatagramSequenceNumberType nextCongestionControlBlock;
    bool backoffThisBlock, speedUpThisBlock;

    DatagramSequenceNumberType expectedNextSequenceNumber;

    bool _isContinuousSend;

    bool IsInSlowStart(void) const;

    double lastRtt, estimatedRTT, deviationRtt;

};

}

#endif