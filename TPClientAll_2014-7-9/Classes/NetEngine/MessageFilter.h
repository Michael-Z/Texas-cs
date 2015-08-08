////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 MessageFilter.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_MessageFilter==1

#pragma once

#include "VENetTypes.h"
#include "PluginInterface2.h"
#include "DS_OrderedList.h"
#include "DS_Hash.h"
#include "Export.h"

#define MESSAGE_FILTER_MAX_MESSAGE_ID 256

namespace VENet
{
class VEPeerInterface;

int VE_DLL_EXPORT MessageFilterStrComp( char *const &key,char *const &data );

struct FilterSet
{
    bool banOnFilterTimeExceed;
    bool kickOnDisallowedMessage;
    bool banOnDisallowedMessage;
    VENet::TimeMS disallowedMessageBanTimeMS;
    VENet::TimeMS timeExceedBanTimeMS;
    VENet::TimeMS maxMemberTimeMS;
    void (*invalidMessageCallback)(VEPeerInterface *peer, AddressOrGUID systemAddress, int filterSetID, void *userData, unsigned char messageID);
    void *disallowedCallbackUserData;
    void (*timeoutCallback)(VEPeerInterface *peer, AddressOrGUID systemAddress, int filterSetID, void *userData);
    void *timeoutUserData;
    int filterSetID;
    bool allowedIDs[MESSAGE_FILTER_MAX_MESSAGE_ID];
    DataStructures::OrderedList<VENet::VEString,VENet::VEString> allowedRPC4;
};

int VE_DLL_EXPORT FilterSetComp( const int &key, FilterSet * const &data );

struct FilteredSystem
{
    FilterSet *filter;
    VENet::TimeMS timeEnteredThisSet;
};

class VE_DLL_EXPORT MessageFilter : public PluginInterface2
{
public:

    STATIC_FACTORY_DECLARATIONS(MessageFilter)

    MessageFilter();
    virtual ~MessageFilter();

    void SetAutoAddNewConnectionsToFilter(int filterSetID);

    void SetAllowMessageID(bool allow, int messageIDStart, int messageIDEnd,int filterSetID);

    void SetAllowRPC4(bool allow, const char* uniqueID, int filterSetID);

    void SetActionOnDisallowedMessage(bool kickOnDisallowed, bool banOnDisallowed, VENet::TimeMS banTimeMS, int filterSetID);

    void SetDisallowedMessageCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(VEPeerInterface *peer, AddressOrGUID addressOrGUID, int filterSetID, void *userData, unsigned char messageID));

    void SetTimeoutCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(VEPeerInterface *peer, AddressOrGUID addressOrGUID, int filterSetID, void *userData));

    void SetFilterMaxTime(int allowedTimeMS, bool banOnExceed, VENet::TimeMS banTimeMS, int filterSetID);

    int GetSystemFilterSet(AddressOrGUID addressOrGUID);

    void SetSystemFilterSet(AddressOrGUID addressOrGUID, int filterSetID);

    unsigned GetSystemCount(int filterSetID) const;

    unsigned GetFilterSetCount(void) const;

    int GetFilterSetIDByIndex(unsigned index);

    void DeleteFilterSet(int filterSetID);


    virtual void Update(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );

protected:

    void Clear(void);
    void DeallocateFilterSet(FilterSet *filterSet);
    FilterSet* GetFilterSetByID(int filterSetID);
    void OnInvalidMessage(FilterSet *filterSet, AddressOrGUID systemAddress, unsigned char messageID);

    DataStructures::OrderedList<int, FilterSet*, FilterSetComp> filterList;
    DataStructures::Hash<AddressOrGUID, FilteredSystem, 2048, AddressOrGUID::ToInteger> systemList;

    int autoAddNewConnectionsToFilter;
    VENet::Time whenLastTimeoutCheck;
};

}

#endif

