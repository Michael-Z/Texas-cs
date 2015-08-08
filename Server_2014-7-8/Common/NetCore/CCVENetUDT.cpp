#include "CCVENetUDT.h"

#if USE_SLIDING_WINDOW_CONGESTION_CONTROL!=1

#include "Rand.h"
#include "MTUSize.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//#include <memory.h>
#include "VEAssert.h"
#include "VEAlloca.h"

using namespace VENet;

static const double UNSET_TIME_US=-1;
static const double CWND_MIN_THRESHOLD=2.0;
static const double UNDEFINED_TRANSFER_RATE=0.0;

#if CC_TIME_TYPE_BYTES==4
static const CCTimeType SYN=10;
#else
static const CCTimeType SYN=10000;
#endif

#if CC_TIME_TYPE_BYTES==4
#define MAX_RTT 1000
#define RTT_TOLERANCE 30
#else
#define MAX_RTT 1000000
#define RTT_TOLERANCE 30000
#endif


double RTTVarMultiple=4.0;


// ****************************************************** PUBLIC METHODS ******************************************************

CCVENetUDT::CCVENetUDT()
{
}

// ----------------------------------------------------------------------------------------------------------------------------

CCVENetUDT::~CCVENetUDT()
{
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::Init(CCTimeType curTime, uint32_t maxDatagramPayload)
{
    (void) curTime;

    nextSYNUpdate=0;
    packetPairRecieptHistoryWriteIndex=0;
    packetArrivalHistoryWriteIndex=0;
    packetArrivalHistoryWriteCount=0;
    RTT=UNSET_TIME_US;
    //	RTTVar=UNSET_TIME_US;
    isInSlowStart=true;
    NAKCount=1000;
    AvgNAKNum=1;
    DecInterval=1;
    DecCount=0;
    nextDatagramSequenceNumber=0;
    lastPacketPairPacketArrivalTime=0;
    lastPacketPairSequenceNumber=(DatagramSequenceNumberType)(const uint32_t)-1;
    lastPacketArrivalTime=0;
    CWND=CWND_MIN_THRESHOLD;
    lastUpdateWindowSizeAndAck=0;
    lastTransmitOfBAndAS=0;
    ExpCount=1.0;
    totalUserDataBytesSent=0;
    oldestUnsentAck=0;
    MAXIMUM_MTU_INCLUDING_UDP_HEADER=maxDatagramPayload;
    CWND_MAX_THRESHOLD=RESEND_BUFFER_ARRAY_LENGTH;
#if CC_TIME_TYPE_BYTES==4
    const BytesPerMicrosecond DEFAULT_TRANSFER_RATE=(BytesPerMicrosecond) 3.6;
#else
    const BytesPerMicrosecond DEFAULT_TRANSFER_RATE=(BytesPerMicrosecond) .0036;
#endif

#if CC_TIME_TYPE_BYTES==4
    lastRttOnIncreaseSendRate=1000;
#else
    lastRttOnIncreaseSendRate=1000000;
#endif
    nextCongestionControlBlock=0;
    lastRtt=0;

    //	B=DEFAULT_TRANSFER_RATE;
    AS=UNDEFINED_TRANSFER_RATE;
    const MicrosecondsPerByte DEFAULT_BYTE_INTERVAL=(MicrosecondsPerByte) (1.0/DEFAULT_TRANSFER_RATE);
    SND=DEFAULT_BYTE_INTERVAL;
    expectedNextSequenceNumber=0;
    sendBAndASCount=0;
    packetArrivalHistoryContinuousGapsIndex=0;
    //packetPairRecipetHistoryGapsIndex=0;
    hasWrittenToPacketPairReceiptHistory=false;
    InitPacketArrivalHistory();

    estimatedLinkCapacityBytesPerSecond=0;
    bytesCanSendThisTick=0;
    hadPacketlossThisBlock=false;
    pingsLastInterval.Clear(__FILE__,__LINE__);
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::SetMTU(uint32_t bytes)
{
    MAXIMUM_MTU_INCLUDING_UDP_HEADER=bytes;
}
// ----------------------------------------------------------------------------------------------------------------------------
uint32_t CCVENetUDT::GetMTU(void) const
{
    return MAXIMUM_MTU_INCLUDING_UDP_HEADER;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::Update(CCTimeType curTime, bool hasDataToSendOrResend)
{
    (void) hasDataToSendOrResend;
    (void) curTime;

    return;
}
// ----------------------------------------------------------------------------------------------------------------------------
int CCVENetUDT::GetRetransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend)
{
    (void) curTime;

    if (isInSlowStart)
    {
        uint32_t CWNDLimit = (uint32_t) (CWND*MAXIMUM_MTU_INCLUDING_UDP_HEADER);
        return CWNDLimit;
    }
    return GetTransmissionBandwidth(curTime,timeSinceLastTick,unacknowledgedBytes,isContinuousSend);
}
// ----------------------------------------------------------------------------------------------------------------------------
int CCVENetUDT::GetTransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend)
{
    (void) curTime;

    if (isInSlowStart)
    {
        uint32_t CWNDLimit = (uint32_t) (CWND*MAXIMUM_MTU_INCLUDING_UDP_HEADER-unacknowledgedBytes);
        return CWNDLimit;
    }
    if (bytesCanSendThisTick>0)
        bytesCanSendThisTick=0;

#if CC_TIME_TYPE_BYTES==4
    if (isContinuousSend==false && timeSinceLastTick>100)
        timeSinceLastTick=100;
#else
    if (isContinuousSend==false && timeSinceLastTick>100000)
        timeSinceLastTick=100000;
#endif

    bytesCanSendThisTick=(int)((double)timeSinceLastTick*((double)1.0/SND)+(double)bytesCanSendThisTick);
    if (bytesCanSendThisTick>0)
        return bytesCanSendThisTick;
    return 0;
}
uint64_t CCVENetUDT::GetBytesPerSecondLimitByCongestionControl(void) const
{
    if (isInSlowStart)
        return 0;
#if CC_TIME_TYPE_BYTES==4
    return (uint64_t) ((double)1.0/(SND*1000.0));
#else
    return (uint64_t) ((double)1.0/(SND*1000000.0));
#endif
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetUDT::ShouldSendACKs(CCTimeType curTime, CCTimeType estimatedTimeToNextTick)
{
    CCTimeType rto = GetSenderRTOForACK();

    if (rto==(CCTimeType) UNSET_TIME_US)
    {
        return true;
    }

    return curTime >= oldestUnsentAck + SYN ||
           estimatedTimeToNextTick+curTime < oldestUnsentAck+rto-RTT;
}
// ----------------------------------------------------------------------------------------------------------------------------
DatagramSequenceNumberType CCVENetUDT::GetNextDatagramSequenceNumber(void)
{
    return nextDatagramSequenceNumber;
}
// ----------------------------------------------------------------------------------------------------------------------------
DatagramSequenceNumberType CCVENetUDT::GetAndIncrementNextDatagramSequenceNumber(void)
{
    DatagramSequenceNumberType dsnt=nextDatagramSequenceNumber;
    nextDatagramSequenceNumber++;
    return dsnt;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnSendBytes(CCTimeType curTime, uint32_t numBytes)
{
    (void) curTime;

    totalUserDataBytesSent+=numBytes;
    if (isInSlowStart==false)
        bytesCanSendThisTick-=numBytes;
}

// ****************************************************** PROTECTED METHODS ******************************************************

void CCVENetUDT::SetNextSYNUpdate(CCTimeType currentTime)
{
    nextSYNUpdate+=SYN;
    if (nextSYNUpdate < currentTime)
        nextSYNUpdate=currentTime+SYN;
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCVENetUDT::ReceiverCalculateDataArrivalRate(CCTimeType curTime) const
{
    (void) curTime;
    if (packetArrivalHistoryWriteCount<CC_VENET_UDT_PACKET_HISTORY_LENGTH)
        return UNDEFINED_TRANSFER_RATE;

    BytesPerMicrosecond median = ReceiverCalculateDataArrivalRateMedian();
    int i;
    const BytesPerMicrosecond oneEighthMedian=median*(1.0/8.0);
    const BytesPerMicrosecond eightTimesMedian=median*8.0f;
    BytesPerMicrosecond medianListLength=0.0;
    BytesPerMicrosecond sum=0.0;
    for (i=0; i < CC_VENET_UDT_PACKET_HISTORY_LENGTH; i++)
    {
        if (packetArrivalHistory[i]>=oneEighthMedian &&
                packetArrivalHistory[i]<eightTimesMedian)
        {
            medianListLength=medianListLength+1.0;
            sum+=packetArrivalHistory[i];
        }
    }
    if (medianListLength==0.0)
        return UNDEFINED_TRANSFER_RATE;
    return sum/medianListLength;
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCVENetUDT::ReceiverCalculateDataArrivalRateMedian(void) const
{
    return CalculateListMedianRecursive(packetArrivalHistory, CC_VENET_UDT_PACKET_HISTORY_LENGTH, 0, 0);
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCVENetUDT::CalculateListMedianRecursive(const BytesPerMicrosecond inputList[CC_VENET_UDT_PACKET_HISTORY_LENGTH], int inputListLength, int lessThanSum, int greaterThanSum)
{
    BytesPerMicrosecond lessThanMedian[CC_VENET_UDT_PACKET_HISTORY_LENGTH], greaterThanMedian[CC_VENET_UDT_PACKET_HISTORY_LENGTH];
    int lessThanMedianListLength=0, greaterThanMedianListLength=0;
    BytesPerMicrosecond median=inputList[0];
    int i;
    for (i=1; i < inputListLength; i++)
    {
        if (inputList[i]<median || ((i&1)==0 && inputList[i]==median))
            lessThanMedian[lessThanMedianListLength++]=inputList[i];
        else
            greaterThanMedian[greaterThanMedianListLength++]=inputList[i];
    }
    VEAssert(CC_VENET_UDT_PACKET_HISTORY_LENGTH%2==0);
    if (lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum+1 ||
            lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum-1)
        return median;

    if (lessThanMedianListLength+lessThanSum < greaterThanMedianListLength+greaterThanSum)
    {
        lessThanMedian[lessThanMedianListLength++]=median;
        return CalculateListMedianRecursive(greaterThanMedian, greaterThanMedianListLength, lessThanMedianListLength+lessThanSum, greaterThanSum);
    }
    else
    {
        greaterThanMedian[greaterThanMedianListLength++]=median;
        return CalculateListMedianRecursive(lessThanMedian, lessThanMedianListLength, lessThanSum, greaterThanMedianListLength+greaterThanSum);
    }
}

// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetUDT::GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
{
    const DatagramSequenceNumberType halfSpan =(DatagramSequenceNumberType) (((DatagramSequenceNumberType)(const uint32_t)-1)/(DatagramSequenceNumberType)2);
    return b!=a && b-a>halfSpan;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetUDT::LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
{
    const DatagramSequenceNumberType halfSpan = ((DatagramSequenceNumberType)(const uint32_t)-1)/(DatagramSequenceNumberType)2;
    return b!=a && b-a<halfSpan;
}
// ----------------------------------------------------------------------------------------------------------------------------
CCTimeType CCVENetUDT::GetSenderRTOForACK(void) const
{
    if (RTT==UNSET_TIME_US)
        return (CCTimeType) UNSET_TIME_US;
    double RTTVar = maxRTT-minRTT;
    return (CCTimeType)(RTT + RTTVarMultiple * RTTVar + SYN);
}
// ----------------------------------------------------------------------------------------------------------------------------
CCTimeType CCVENetUDT::GetRTOForRetransmission(unsigned char timesSent) const
{
#if CC_TIME_TYPE_BYTES==4
    const CCTimeType maxThreshold=10000;
    const CCTimeType minThreshold=100;
#else
    const CCTimeType maxThreshold=1000000;
    const CCTimeType minThreshold=100000;
#endif

    if (RTT==UNSET_TIME_US)
    {
        return (CCTimeType) maxThreshold;
    }

    CCTimeType ret = lastRttOnIncreaseSendRate*2;

    if (ret<minThreshold)
        return minThreshold;
    if (ret>maxThreshold)
        return maxThreshold;
    return ret;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnResend(CCTimeType curTime, VENet::TimeUS nextActionTime)
{
    (void) curTime;

    if (isInSlowStart)
    {
        if (AS!=UNDEFINED_TRANSFER_RATE)
            EndSlowStart();
        return;
    }

    if (hadPacketlossThisBlock==false)
    {
        IncreaseTimeBetweenSends();
        hadPacketlossThisBlock=true;
    }
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnNAK(CCTimeType curTime, DatagramSequenceNumberType nakSequenceNumber)
{
    (void) nakSequenceNumber;
    (void) curTime;

    if (isInSlowStart)
    {
        if (AS!=UNDEFINED_TRANSFER_RATE)
            EndSlowStart();
        return;
    }

    if (hadPacketlossThisBlock==false)
    {
        if (pingsLastInterval.Size()>10)
        {
            for (int i=0; i < 10; i++)
                printf("%i, ", pingsLastInterval[pingsLastInterval.Size()-1-i]/1000);
        }
        printf("\n");
        IncreaseTimeBetweenSends();

        hadPacketlossThisBlock=true;
    }
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::EndSlowStart(void)
{
    VEAssert(isInSlowStart==true);
    VEAssert(AS!=UNDEFINED_TRANSFER_RATE);

    estimatedLinkCapacityBytesPerSecond=AS * 1000000.0;

    isInSlowStart=false;
    SND=1.0/AS;
    CapMinSnd(_FILE_AND_LINE_);

#if CC_TIME_TYPE_BYTES==4
    //	printf("Initial SND=%f Kilobytes per second\n", 1.0/SND);
#else
    //	printf("Initial SND=%f Megabytes per second\n", 1.0/SND);
#endif
    if (SND > .1)
        PrintLowBandwidthWarning();
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber, uint32_t sizeInBytes, CCTimeType curTime)
{
    (void) datagramSequenceNumber;
    (void) sizeInBytes;
    (void) curTime;

}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCVENetUDT::OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber, bool isContinuousSend, CCTimeType curTime, uint32_t sizeInBytes, uint32_t *skippedMessageCount)
{
    CC_DEBUG_PRINTF_2("R%i ",datagramSequenceNumber.val);

    if (datagramSequenceNumber==expectedNextSequenceNumber)
    {
        *skippedMessageCount=0;
        expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
    }
    else if (GreaterThan(datagramSequenceNumber, expectedNextSequenceNumber))
    {
        *skippedMessageCount=datagramSequenceNumber-expectedNextSequenceNumber;
        if (*skippedMessageCount>1000)
        {
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

    if (curTime>lastPacketArrivalTime)
    {
        CCTimeType interval = curTime-lastPacketArrivalTime;

        if (isContinuousSend)
        {
            continuousBytesReceived+=sizeInBytes;
            if (continuousBytesReceivedStartTime==0)
                continuousBytesReceivedStartTime=lastPacketArrivalTime;


            mostRecentPacketArrivalHistory=(BytesPerMicrosecond)sizeInBytes/(BytesPerMicrosecond)interval;

            packetArrivalHistoryContinuousGaps[packetArrivalHistoryContinuousGapsIndex++]=(int) interval;
            packetArrivalHistoryContinuousGapsIndex&=(CC_VENET_UDT_PACKET_HISTORY_LENGTH-1);

            packetArrivalHistoryWriteCount++;
            packetArrivalHistory[packetArrivalHistoryWriteIndex++]=mostRecentPacketArrivalHistory;
            packetArrivalHistoryWriteIndex&=(CC_VENET_UDT_PACKET_HISTORY_LENGTH-1);
        }
        else
        {
            continuousBytesReceivedStartTime=0;
            continuousBytesReceived=0;
        }

        lastPacketArrivalTime=curTime;
    }
    return true;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnAck(CCTimeType curTime, CCTimeType rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, double totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber )
{
#if CC_TIME_TYPE_BYTES==4
    VEAssert(rtt < 10000);
#else
    VEAssert(rtt < 10000000);
#endif
    (void) _B;

    if (hasBAndAS)
    {
        VEAssert(_AS!=UNDEFINED_TRANSFER_RATE);
        AS=_AS;
        CC_DEBUG_PRINTF_4("ArrivalRate=%f linkCap=%f incomingLinkCap=%f\n", _AS,B,_B);
    }

    if (oldestUnsentAck==0)
        oldestUnsentAck=curTime;

    if (isInSlowStart==true)
    {
        nextCongestionControlBlock=nextDatagramSequenceNumber;
        lastRttOnIncreaseSendRate=rtt;
        UpdateWindowSizeAndAckOnAckPreSlowStart(totalUserDataBytesAcked);
    }
    else
    {
        UpdateWindowSizeAndAckOnAckPerSyn(curTime, rtt, isContinuousSend, sequenceNumber);
    }

    lastUpdateWindowSizeAndAck=curTime;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnSendAckGetBAndAS(CCTimeType curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS)
{
    if (curTime>lastTransmitOfBAndAS+SYN)
    {
        *_B=0;
        *_AS=ReceiverCalculateDataArrivalRate(curTime);

        if (*_AS==UNDEFINED_TRANSFER_RATE)
        {
            *hasBAndAS=false;
        }
        else
        {
            *hasBAndAS=true;
        }
    }
    else
    {
        *hasBAndAS=false;
    }
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnSendAck(CCTimeType curTime, uint32_t numBytes)
{
    (void) numBytes;
    (void) curTime;
    oldestUnsentAck=0;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::OnSendNACK(CCTimeType curTime, uint32_t numBytes)
{
    (void) numBytes;
    (void) curTime;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::UpdateWindowSizeAndAckOnAckPreSlowStart(double totalUserDataBytesAcked)
{
    CC_DEBUG_PRINTF_3("CWND increasing from %f to %f\n", CWND, (double) ((double)totalUserDataBytesAcked/(double)MAXIMUM_MTU_INCLUDING_UDP_HEADER));
    CWND=(double) ((double)totalUserDataBytesAcked/(double)MAXIMUM_MTU_INCLUDING_UDP_HEADER);
    if (CWND>=CWND_MAX_THRESHOLD)
    {
        CWND=CWND_MAX_THRESHOLD;

        if (AS!=UNDEFINED_TRANSFER_RATE)
            EndSlowStart();
    }
    if (CWND<CWND_MIN_THRESHOLD)
        CWND=CWND_MIN_THRESHOLD;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::UpdateWindowSizeAndAckOnAckPerSyn(CCTimeType curTime, CCTimeType rtt, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber)
{
    (void) curTime;
    (void) sequenceNumber;
    if (isContinuousSend==false)
    {
        nextCongestionControlBlock=nextDatagramSequenceNumber;
        pingsLastInterval.Clear(__FILE__,__LINE__);
        return;
    }

    pingsLastInterval.Push(rtt,__FILE__,__LINE__);
    static const int intervalSize=33;
    if (pingsLastInterval.Size()>intervalSize)
        pingsLastInterval.Pop();
    if (GreaterThan(sequenceNumber, nextCongestionControlBlock) &&
            sequenceNumber-nextCongestionControlBlock>=intervalSize &&
            pingsLastInterval.Size()==intervalSize)
    {
        double slopeSum=0.0;
        double average=(double) pingsLastInterval[0];
        int sampleSize=pingsLastInterval.Size();
        for (int i=1; i < sampleSize; i++)
        {
            slopeSum+=(double)pingsLastInterval[i]-(double)pingsLastInterval[i-1];
            average+=pingsLastInterval[i];
        }
        average/=sampleSize;

        if (hadPacketlossThisBlock==true)
        {
        }
        else if (slopeSum < -.10*average)
        {
        }
        else if (slopeSum > .10*average)
        {
            IncreaseTimeBetweenSends();
        }
        else
        {
            lastRttOnIncreaseSendRate=rtt;
            DecreaseTimeBetweenSends();
        }

        pingsLastInterval.Clear(__FILE__,__LINE__);
        hadPacketlossThisBlock=false;
        nextCongestionControlBlock=nextDatagramSequenceNumber;
    }

    lastRtt=rtt;
}

// ----------------------------------------------------------------------------------------------------------------------------
double CCVENetUDT::BytesPerMicrosecondToPacketsPerMillisecond(BytesPerMicrosecond in)
{
#if CC_TIME_TYPE_BYTES==4
    const BytesPerMicrosecond factor = 1.0 / (BytesPerMicrosecond) MAXIMUM_MTU_INCLUDING_UDP_HEADER;
#else
    const BytesPerMicrosecond factor = 1000.0 / (BytesPerMicrosecond) MAXIMUM_MTU_INCLUDING_UDP_HEADER;
#endif
    return in * factor;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::InitPacketArrivalHistory(void)
{
    unsigned int i;
    for (i=0; i < CC_VENET_UDT_PACKET_HISTORY_LENGTH; i++)
    {
        packetArrivalHistory[i]=UNDEFINED_TRANSFER_RATE;
        packetArrivalHistoryContinuousGaps[i]=0;
    }

    packetArrivalHistoryWriteCount=0;
    continuousBytesReceived=0;
    continuousBytesReceivedStartTime=0;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCVENetUDT::PrintLowBandwidthWarning(void)
{

}
BytesPerMicrosecond CCVENetUDT::GetLocalReceiveRate(CCTimeType currentTime) const
{
    return ReceiverCalculateDataArrivalRate(currentTime);
}
double CCVENetUDT::GetRTT(void) const
{
    if (RTT==UNSET_TIME_US)
        return 0.0;
    return RTT;
}
void CCVENetUDT::CapMinSnd(const char *file, int line)
{
    (void) file;
    (void) line;

    if (SND > 500)
    {
        SND=500;
        CC_DEBUG_PRINTF_3("%s:%i LIKELY BUG: SND has gotten above 500 microseconds between messages (28.8 modem)\nReport to rakkar@jenkinssoftware.com with file and line number\n", file, line);
    }
}
void CCVENetUDT::IncreaseTimeBetweenSends(void)
{
    double increment;
    increment = .02 * ((SND+1.0) * (SND+1.0)) / (501.0*501.0) ;
    SND*=(1.02 - increment);
    CapMinSnd(__FILE__,__LINE__);
}
void CCVENetUDT::DecreaseTimeBetweenSends(void)
{
    double increment;
    increment = .01 * ((SND+1.0) * (SND+1.0)) / (501.0*501.0) ;
    SND*=(.99 - increment);
}

#endif
