////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 VENetStatistics.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "PacketPriority.h"
#include "Export.h"
#include "VENetTypes.h"

namespace VENet
{

enum RNSPerSecondMetrics
{
    USER_MESSAGE_BYTES_PUSHED,

    USER_MESSAGE_BYTES_SENT,

    USER_MESSAGE_BYTES_RESENT,

    USER_MESSAGE_BYTES_RECEIVED_PROCESSED,

    USER_MESSAGE_BYTES_RECEIVED_IGNORED,

    ACTUAL_BYTES_SENT,

    ACTUAL_BYTES_RECEIVED,

    RNS_PER_SECOND_METRICS_COUNT
};

struct VE_DLL_EXPORT VENetStatistics
{
    uint64_t valueOverLastSecond[RNS_PER_SECOND_METRICS_COUNT];

    uint64_t runningTotal[RNS_PER_SECOND_METRICS_COUNT];

    VENet::TimeUS connectionStartTime;

    bool isLimitedByCongestionControl;

    uint64_t BPSLimitByCongestionControl;

    bool isLimitedByOutgoingBandwidthLimit;

    uint64_t BPSLimitByOutgoingBandwidthLimit;

    unsigned int messageInSendBuffer[NUMBER_OF_PRIORITIES];

    double bytesInSendBuffer[NUMBER_OF_PRIORITIES];

    unsigned int messagesInResendBuffer;

    uint64_t bytesInResendBuffer;

    float packetlossLastSecond;

    float packetlossTotal;

    VENetStatistics& operator +=(const VENetStatistics& other)
    {
        unsigned i;
        for (i=0; i < NUMBER_OF_PRIORITIES; i++)
        {
            messageInSendBuffer[i]+=other.messageInSendBuffer[i];
            bytesInSendBuffer[i]+=other.bytesInSendBuffer[i];
        }

        for (i=0; i < RNS_PER_SECOND_METRICS_COUNT; i++)
        {
            valueOverLastSecond[i]+=other.valueOverLastSecond[i];
            runningTotal[i]+=other.runningTotal[i];
        }

        return *this;
    }
};

void VE_DLL_EXPORT StatisticsToString( VENetStatistics *s, char *buffer, int verbosityLevel );

}
