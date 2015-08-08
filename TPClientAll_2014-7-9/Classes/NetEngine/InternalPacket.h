////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 InternalPacket.h
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
#include "VENetTypes.h"
#include "VEMemoryOverride.h"
#include "VENetDefines.h"
#include "NativeTypes.h"
#include "VENetDefines.h"
#if USE_SLIDING_WINDOW_CONGESTION_CONTROL!=1
#include "CCVENetUDT.h"
#else
#include "CCVENetSlidingWindow.h"
#endif

namespace VENet
{

typedef uint16_t SplitPacketIdType;
typedef uint32_t SplitPacketIndexType;

typedef uint24_t MessageNumberType;

typedef MessageNumberType OrderingIndexType;

typedef VENet::TimeUS RemoteSystemTimeType;

struct InternalPacketFixedSizeTransmissionHeader
{
    MessageNumberType reliableMessageNumber;
    OrderingIndexType orderingIndex;
    OrderingIndexType sequencingIndex;
    unsigned char orderingChannel;
    SplitPacketIdType splitPacketId;
    SplitPacketIndexType splitPacketIndex;
    SplitPacketIndexType splitPacketCount;;
    BitSize_t dataBitLength;
    PacketReliability reliability;
};

struct InternalPacketRefCountedData
{
    unsigned char *sharedDataBlock;
    unsigned int refCount;
};

struct InternalPacket : public InternalPacketFixedSizeTransmissionHeader
{
    MessageNumberType messageInternalOrder;
    bool messageNumberAssigned;
    VENet::TimeUS creationTime;
    VENet::TimeUS nextActionTime;
    VENet::TimeUS retransmissionTime;
    BitSize_t headerLength;
    unsigned char *data;
    enum AllocationScheme
    {
        NORMAL,

        REF_COUNTED,

        STACK
    } allocationScheme;
    InternalPacketRefCountedData *refCountedData;
    unsigned char timesSent;
    PacketPriority priority;
    uint32_t sendReceiptSerial;

    InternalPacket *resendPrev, *resendNext,*unreliablePrev,*unreliableNext;

    unsigned char stackData[128];
};

}

