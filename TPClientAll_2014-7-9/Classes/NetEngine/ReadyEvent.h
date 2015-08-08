////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 Rand.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_ReadyEvent==1

#pragma once

#include "PluginInterface2.h"
#include "DS_OrderedList.h"

namespace VENet
{

class VEPeerInterface;


enum ReadyEventSystemStatus
{
    RES_NOT_WAITING,
    RES_WAITING,
    RES_READY,
    RES_ALL_READY,
    RES_UNKNOWN_EVENT,
};


class ReadyEvent : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(ReadyEvent)

    ReadyEvent();

    virtual ~ReadyEvent();

    bool SetEvent(int eventId, bool isReady);

    void ForceCompletion(int eventId);

    bool DeleteEvent(int eventId);

    bool IsEventSet(int eventId);

    bool IsEventCompletionProcessing(int eventId) const;

    bool IsEventCompleted(int eventId) const;

    bool HasEvent(int eventId);

    unsigned GetEventListSize(void) const;

    int GetEventAtIndex(unsigned index) const;

    bool AddToWaitList(int eventId, SystemAddress address);

    bool RemoveFromWaitList(int eventId, SystemAddress address);

    bool IsInWaitList(int eventId, SystemAddress address);

    unsigned GetRemoteWaitListSize(int eventId) const;

    SystemAddress GetFromWaitListAtIndex(int eventId, unsigned index) const;

    ReadyEventSystemStatus GetReadyStatus(int eventId, SystemAddress address);

    void SetSendChannel(unsigned char newChannel);

    struct RemoteSystem
    {
        MessageID lastSentStatus, lastReceivedStatus;
        SystemAddress systemAddress;
    };
    static int RemoteSystemCompBySystemAddress( const SystemAddress &key, const RemoteSystem &data );
    struct ReadyEventNode
    {
        int eventId;
        MessageID eventStatus;
        DataStructures::OrderedList<SystemAddress,RemoteSystem,ReadyEvent::RemoteSystemCompBySystemAddress> systemList;
    };
    static int ReadyEventNodeComp( const int &key, ReadyEvent::ReadyEventNode * const &data );


protected:
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnRakPeerShutdown(void);

    void Clear(void);

    void SendReadyStateQuery(unsigned eventId, SystemAddress address);
    void SendReadyUpdate(unsigned eventIndex, unsigned systemIndex, bool forceIfNotDefault);
    void BroadcastReadyUpdate(unsigned eventIndex, bool forceIfNotDefault);
    void RemoveFromAllLists(SystemAddress address);
    void OnReadyEventQuery(Packet *packet);
    void PushCompletionPacket(unsigned eventId);
    bool AddToWaitListInternal(unsigned eventIndex, SystemAddress address);
    void OnReadyEventForceAllSet(Packet *packet);
    void OnReadyEventPacketUpdate(Packet *packet);
    void UpdateReadyStatus(unsigned eventIndex);
    bool IsEventCompletedByIndex(unsigned eventIndex) const;
    unsigned CreateNewEvent(int eventId, bool isReady);
    bool SetEventByIndex(int eventIndex, bool isReady);

    DataStructures::OrderedList<int, ReadyEventNode*, ReadyEvent::ReadyEventNodeComp> readyEventNodeList;
    unsigned char channel;
};

}

#endif

