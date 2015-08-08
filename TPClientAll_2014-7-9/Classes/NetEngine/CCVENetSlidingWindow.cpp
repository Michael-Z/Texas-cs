#include "CCVENetSlidingWindow.h"

#if USE_SLIDING_WINDOW_CONGESTION_CONTROL==1

static const double UNSET_TIME_US=-1;

#if CC_TIME_TYPE_BYTES==4
static const CCTimeType SYN=10;
#else
static const CCTimeType SYN=10000;
#endif

#include "MTUSize.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "VEAssert.h"
#include "VEAlloca.h"

using namespace VENet;

// ****************************************************** PUBLIC METHODS ******************************************************

CCVENetSlidingWindow::CCVENetSlidingWindow()
{
}
// ----------------------------------------------------------------------------------------------------------------------------
CCVENetSlidingWindow::~CCVENetSlidingWindow()
{

}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::Init(CCTimeType curTime, uint32_t maxDatagramPayload)
{
    (void) curTime;

    lastRtt=estimatedRTT=deviationRtt=UNSET_TIME_US;
    MAXIMUM_MTU_INCLUDING_UDP_HEADER=maxDatagramPayload;
    cwnd=maxDatagramPayload;
    ssThresh=0.0;
    oldestUnsentAck=0;
    nextDatagramSequenceNumber=0;
    nextCongestionControlBlock=0;
    backoffThisBlock=speedUpThisBlock=false;
    expectedNextSequenceNumber=0;
    _isContinuousSend=false;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::Update(CCTimeType curTime, bool hasDataToSendOrResend)
{
    (void) curTime;
    (void) hasDataToSendOrResend;
}
// ----------------------------------------------------------------------------------------------------------------------------
int CCVENetSlidingWindow::GetRetransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend)
{
    (void) curTime;
    (void) isContinuousSend;
    (void) timeSinceLastTick;

    return unacknowledgedBytes;
}
// ----------------------------------------------------------------------------------------------------------------------------
int CCVENetSlidingWindow::GetTransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend)
{
    (void) curTime;
    (void) timeSinceLastTick;

    _isContinuousSend=isContinuousSend;

    if (unacknowledgedBytes<=cwnd)
        return (int) (cwnd-unacknowledgedBytes);
    else
        return 0;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetSlidingWindow::ShouldSendACKs(CCTimeType curTime, CCTimeType estimatedTimeToNextTick)
{
    CCTimeType rto = GetSenderRTOForACK();
    (void) estimatedTimeToNextTick;

    // iphone crashes on comparison between double and int64 http://www.jenkinssoftware.com/forum/index.php?topic=2717.0
    if (rto==(CCTimeType) UNSET_TIME_US)
    {
        // Unknown how long until the remote system will retransmit, so better send right away
        return true;
    }

    return curTime >= oldestUnsentAck + SYN;
}
// ----------------------------------------------------------------------------------------------------------------------------
DatagramSequenceNumberType CCVENetSlidingWindow::GetNextDatagramSequenceNumber(void)
{
    return nextDatagramSequenceNumber;
}
// ----------------------------------------------------------------------------------------------------------------------------
DatagramSequenceNumberType CCVENetSlidingWindow::GetAndIncrementNextDatagramSequenceNumber(void)
{
    DatagramSequenceNumberType dsnt=nextDatagramSequenceNumber;
    nextDatagramSequenceNumber++;
    return dsnt;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnSendBytes(CCTimeType curTime, uint32_t numBytes)
{
    (void) curTime;
    (void) numBytes;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber, uint32_t sizeInBytes, CCTimeType curTime)
{
    (void) curTime;
    (void) sizeInBytes;
    (void) datagramSequenceNumber;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetSlidingWindow::OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber, bool isContinuousSend, CCTimeType curTime, uint32_t sizeInBytes, uint32_t *skippedMessageCount)
{
    (void) curTime;
    (void) sizeInBytes;
    (void) isContinuousSend;

    if (oldestUnsentAck==0)
        oldestUnsentAck=curTime;

    if (datagramSequenceNumber==expectedNextSequenceNumber)
    {
        *skippedMessageCount=0;
        expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
    }
    else if (GreaterThan(datagramSequenceNumber, expectedNextSequenceNumber))
    {
        *skippedMessageCount=datagramSequenceNumber-expectedNextSequenceNumber;
        // Sanity check, just use timeout resend if this was really valid
        if (*skippedMessageCount>1000)
        {
            // During testing, the nat punchthrough server got 51200 on the first packet. I have no idea where this comes from, but has happened twice
            if (*skippedMessageCount>(uint32_t)50000)
                return false;
            *skippedMessageCount=1000;
        }
        expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
    }
    else
    {
        *skippedMessageCount=0;
    }

    return true;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnResend(CCTimeType curTime, VENet::TimeUS nextActionTime)
{
    (void) curTime;
    (void) nextActionTime;

    if (_isContinuousSend && backoffThisBlock==false && cwnd>MAXIMUM_MTU_INCLUDING_UDP_HEADER*2)
    {
        ssThresh=cwnd/2;
        if (ssThresh<MAXIMUM_MTU_INCLUDING_UDP_HEADER)
            ssThresh=MAXIMUM_MTU_INCLUDING_UDP_HEADER;
        cwnd=MAXIMUM_MTU_INCLUDING_UDP_HEADER;

        nextCongestionControlBlock=nextDatagramSequenceNumber;
        backoffThisBlock=true;
    }
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnNAK(CCTimeType curTime, DatagramSequenceNumberType nakSequenceNumber)
{
    (void) nakSequenceNumber;
    (void) curTime;

    if (_isContinuousSend && backoffThisBlock==false)
    {
        ssThresh=cwnd/2;
    }
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnAck(CCTimeType curTime, CCTimeType rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, double totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber )
{
    (void) _B;
    (void) totalUserDataBytesAcked;
    (void) _AS;
    (void) hasBAndAS;
    (void) curTime;
    (void) rtt;

    lastRtt=(double) rtt;
    if (estimatedRTT==UNSET_TIME_US)
    {
        estimatedRTT=(double) rtt;
        deviationRtt=(double)rtt;
    }
    else
    {
        double d = .05;
        double difference = rtt - estimatedRTT;
        estimatedRTT = estimatedRTT + d * difference;
        deviationRtt = deviationRtt + d * (abs(difference) - deviationRtt);
    }

    _isContinuousSend=isContinuousSend;

    if (isContinuousSend==false)
        return;

    bool isNewCongestionControlPeriod;
    isNewCongestionControlPeriod = GreaterThan(sequenceNumber, nextCongestionControlBlock);

    if (isNewCongestionControlPeriod)
    {
        backoffThisBlock=false;
        speedUpThisBlock=false;
        nextCongestionControlBlock=nextDatagramSequenceNumber;
    }

    if (IsInSlowStart())
    {
        cwnd+=MAXIMUM_MTU_INCLUDING_UDP_HEADER;
        if (cwnd > ssThresh && ssThresh!=0)
            cwnd = ssThresh + MAXIMUM_MTU_INCLUDING_UDP_HEADER*MAXIMUM_MTU_INCLUDING_UDP_HEADER/cwnd;
    }
    else if (isNewCongestionControlPeriod)
    {
        cwnd+=MAXIMUM_MTU_INCLUDING_UDP_HEADER*MAXIMUM_MTU_INCLUDING_UDP_HEADER/cwnd;
    }
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnDuplicateAck( CCTimeType curTime, DatagramSequenceNumberType sequenceNumber )
{
    (void) curTime;
    (void) sequenceNumber;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnSendAckGetBAndAS(CCTimeType curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS)
{
    (void) curTime;
    (void) _B;
    (void) _AS;

    *hasBAndAS=false;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnSendAck(CCTimeType curTime, uint32_t numBytes)
{
    (void) curTime;
    (void) numBytes;

    oldestUnsentAck=0;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::OnSendNACK(CCTimeType curTime, uint32_t numBytes)
{
    (void) curTime;
    (void) numBytes;

}
// ----------------------------------------------------------------------------------------------------------------------------
CCTimeType CCVENetSlidingWindow::GetRTOForRetransmission(unsigned char timesSent) const
{
    (void) timesSent;

#if CC_TIME_TYPE_BYTES==4
    const CCTimeType maxThreshold=2000;
    //const CCTimeType minThreshold=100;
    const CCTimeType additionalVariance=30;
#else
    const CCTimeType maxThreshold=2000000;
    const CCTimeType additionalVariance=30000;
#endif


    if (estimatedRTT==UNSET_TIME_US)
        return maxThreshold;

    double u=2.0f;
    double q=4.0f;
    return (CCTimeType) (u * estimatedRTT + q * deviationRtt) + additionalVariance;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetSlidingWindow::SetMTU(uint32_t bytes)
{
    MAXIMUM_MTU_INCLUDING_UDP_HEADER=bytes;
}
// ----------------------------------------------------------------------------------------------------------------------------
uint32_t CCVENetSlidingWindow::GetMTU(void) const
{
    return MAXIMUM_MTU_INCLUDING_UDP_HEADER;
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCVENetSlidingWindow::GetLocalReceiveRate(CCTimeType currentTime) const
{
    (void) currentTime;

    return 0; // TODO
}
// ----------------------------------------------------------------------------------------------------------------------------
double CCVENetSlidingWindow::GetRTT(void) const
{
    if (lastRtt==UNSET_TIME_US)
        return 0.0;
    return lastRtt;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetSlidingWindow::GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
{
    const DatagramSequenceNumberType halfSpan =(DatagramSequenceNumberType) (((DatagramSequenceNumberType)(const uint32_t)-1)/(DatagramSequenceNumberType)2);
    return b!=a && b-a>halfSpan;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetSlidingWindow::LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
{
    const DatagramSequenceNumberType halfSpan = ((DatagramSequenceNumberType)(const uint32_t)-1)/(DatagramSequenceNumberType)2;
    return b!=a && b-a<halfSpan;
}
// ----------------------------------------------------------------------------------------------------------------------------
uint64_t CCVENetSlidingWindow::GetBytesPerSecondLimitByCongestionControl(void) const
{
    return 0; // TODO
}
// ----------------------------------------------------------------------------------------------------------------------------
CCTimeType CCVENetSlidingWindow::GetSenderRTOForACK(void) const
{
    if (lastRtt==UNSET_TIME_US)
        return (CCTimeType) UNSET_TIME_US;
    return (CCTimeType)(lastRtt + SYN);
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetSlidingWindow::IsInSlowStart(void) const
{
    return cwnd <= ssThresh || ssThresh==0;
}
// ----------------------------------------------------------------------------------------------------------------------------
#endif
