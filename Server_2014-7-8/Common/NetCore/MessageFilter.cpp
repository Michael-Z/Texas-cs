#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_MessageFilter==1

#include "MessageFilter.h"
#include "VEAssert.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "VEAssert.h"
#include "VEPeerInterface.h"
#include "PacketizedTCP.h"
#include "BitStream.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

using namespace VENet;

int VENet::MessageFilterStrComp( char *const &key,char *const &data )
{
    return strcmp(key,data);
}

int VENet::FilterSetComp( const int &key, FilterSet * const &data )
{
    if (key < data->filterSetID)
        return -1;
    else if (key==data->filterSetID)
        return 0;
    else
        return 1;
}
STATIC_FACTORY_DEFINITIONS(MessageFilter,MessageFilter);

MessageFilter::MessageFilter()
{
    whenLastTimeoutCheck=VENet::GetTime();
}
MessageFilter::~MessageFilter()
{
    Clear();
}
void MessageFilter::SetAutoAddNewConnectionsToFilter(int filterSetID)
{
    autoAddNewConnectionsToFilter=filterSetID;
}
void MessageFilter::SetAllowMessageID(bool allow, int messageIDStart, int messageIDEnd,int filterSetID)
{
    VEAssert(messageIDStart <= messageIDEnd);
    FilterSet *filterSet = GetFilterSetByID(filterSetID);
    int i;
    for (i=messageIDStart; i <= messageIDEnd; ++i)
        filterSet->allowedIDs[i]=allow;
}
void MessageFilter::SetAllowRPC4(bool allow, const char* uniqueID, int filterSetID)
{
    FilterSet *filterSet = GetFilterSetByID(filterSetID);
    bool objectExists;
    unsigned int idx = filterSet->allowedRPC4.GetIndexFromKey(uniqueID, &objectExists);
    if (allow)
    {
        if (objectExists==false)
        {
            filterSet->allowedRPC4.InsertAtIndex(uniqueID, idx, _FILE_AND_LINE_);
            filterSet->allowedIDs[ID_RPC_PLUGIN]=true;
        }
    }
    else
    {
        if (objectExists==true)
        {
            filterSet->allowedRPC4.RemoveAtIndex(idx);
            if (filterSet->allowedRPC4.Size()==0)
            {
                filterSet->allowedIDs[ID_RPC_PLUGIN]=false;
            }
        }
    }
}
void MessageFilter::SetActionOnDisallowedMessage(bool kickOnDisallowed, bool banOnDisallowed, VENet::TimeMS banTimeMS, int filterSetID)
{
    FilterSet *filterSet = GetFilterSetByID(filterSetID);
    filterSet->kickOnDisallowedMessage=kickOnDisallowed;
    filterSet->disallowedMessageBanTimeMS=banTimeMS;
    filterSet->banOnDisallowedMessage=banOnDisallowed;
}
void MessageFilter::SetDisallowedMessageCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(VEPeerInterface *peer, AddressOrGUID systemAddress, int filterSetID, void *userData, unsigned char messageID))
{
    FilterSet *filterSet = GetFilterSetByID(filterSetID);
    filterSet->invalidMessageCallback=invalidMessageCallback;
    filterSet->disallowedCallbackUserData=userData;
}
void MessageFilter::SetTimeoutCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(VEPeerInterface *peer, AddressOrGUID systemAddress, int filterSetID, void *userData))
{
    FilterSet *filterSet = GetFilterSetByID(filterSetID);
    filterSet->timeoutCallback=invalidMessageCallback;
    filterSet->timeoutUserData=userData;
}
void MessageFilter::SetFilterMaxTime(int allowedTimeMS, bool banOnExceed, VENet::TimeMS banTimeMS, int filterSetID)
{
    FilterSet *filterSet = GetFilterSetByID(filterSetID);
    filterSet->maxMemberTimeMS=allowedTimeMS;
    filterSet->banOnFilterTimeExceed=banOnExceed;
    filterSet->timeExceedBanTimeMS=banTimeMS;
}
int MessageFilter::GetSystemFilterSet(AddressOrGUID systemAddress)
{
    DataStructures::HashIndex index = systemList.GetIndexOf(systemAddress);
    if (index.IsInvalid())
        return -1;
    else
        return systemList.ItemAtIndex(index).filter->filterSetID;
}
void MessageFilter::SetSystemFilterSet(AddressOrGUID addressOrGUID, int filterSetID)
{
    VEAssert(addressOrGUID.IsUndefined()==false);
    DataStructures::HashIndex index = systemList.GetIndexOf(addressOrGUID);
    if (index.IsInvalid())
    {
        if (filterSetID<0)
            return;

        FilteredSystem filteredSystem;
        filteredSystem.filter = GetFilterSetByID(filterSetID);
        filteredSystem.timeEnteredThisSet=VENet::GetTimeMS();
        systemList.Push(addressOrGUID,filteredSystem,_FILE_AND_LINE_);
    }
    else
    {
        if (filterSetID>=0)
        {
            FilterSet *filterSet = GetFilterSetByID(filterSetID);
            systemList.ItemAtIndex(index).timeEnteredThisSet=VENet::GetTimeMS();
            systemList.ItemAtIndex(index).filter=filterSet;
        }
        else
        {
            systemList.RemoveAtIndex(index, _FILE_AND_LINE_);
        }
    }
}
unsigned MessageFilter::GetSystemCount(int filterSetID) const
{
    if (filterSetID==-1)
    {
        return systemList.Size();
    }
    else
    {
        unsigned i;
        unsigned count=0;
        DataStructures::List< FilteredSystem > itemList;
        DataStructures::List< AddressOrGUID > keyList;
        systemList.GetAsList(itemList, keyList, _FILE_AND_LINE_);
        for (i=0; i < itemList.Size(); i++)
            if (itemList[i].filter->filterSetID==filterSetID)
                ++count;
        return count;
    }
}
unsigned MessageFilter::GetFilterSetCount(void) const
{
    return filterList.Size();
}
int MessageFilter::GetFilterSetIDByIndex(unsigned index)
{
    return filterList[index]->filterSetID;
}
void MessageFilter::DeleteFilterSet(int filterSetID)
{
    FilterSet *filterSet;
    bool objectExists;
    unsigned i,index;
    index = filterList.GetIndexFromKey(filterSetID, &objectExists);
    if (objectExists)
    {
        filterSet=filterList[index];
        DeallocateFilterSet(filterSet);
        filterList.RemoveAtIndex(index);

        DataStructures::List< FilteredSystem > itemList;
        DataStructures::List< AddressOrGUID > keyList;
        systemList.GetAsList(itemList, keyList, _FILE_AND_LINE_);
        for (i=0; i < itemList.Size(); i++)
        {
            if (itemList[i].filter==filterSet)
            {
                systemList.Remove(keyList[i], _FILE_AND_LINE_);
            }
        }
    }
}
void MessageFilter::Clear(void)
{
    unsigned i;
    systemList.Clear(_FILE_AND_LINE_);
    for (i=0; i < filterList.Size(); i++)
        DeallocateFilterSet(filterList[i]);
    filterList.Clear(false, _FILE_AND_LINE_);
}
void MessageFilter::DeallocateFilterSet(FilterSet* filterSet)
{
    VENet::OP_DELETE(filterSet, _FILE_AND_LINE_);
}
FilterSet* MessageFilter::GetFilterSetByID(int filterSetID)
{
    VEAssert(filterSetID>=0);
    bool objectExists;
    unsigned index;
    index = filterList.GetIndexFromKey(filterSetID, &objectExists);
    if (objectExists)
        return filterList[index];
    else
    {
        FilterSet *newFilterSet = VENet::OP_NEW<FilterSet>( _FILE_AND_LINE_ );
        memset(newFilterSet->allowedIDs, 0, MESSAGE_FILTER_MAX_MESSAGE_ID * sizeof(bool));
        newFilterSet->banOnFilterTimeExceed=false;
        newFilterSet->kickOnDisallowedMessage=false;
        newFilterSet->banOnDisallowedMessage=false;
        newFilterSet->disallowedMessageBanTimeMS=0;
        newFilterSet->timeExceedBanTimeMS=0;
        newFilterSet->maxMemberTimeMS=0;
        newFilterSet->filterSetID=filterSetID;
        newFilterSet->invalidMessageCallback=0;
        newFilterSet->timeoutCallback=0;
        newFilterSet->timeoutUserData=0;
        filterList.Insert(filterSetID, newFilterSet, true, _FILE_AND_LINE_);
        return newFilterSet;
    }
}
void MessageFilter::OnInvalidMessage(FilterSet *filterSet, AddressOrGUID systemAddress, unsigned char messageID)
{
    if (filterSet->invalidMessageCallback)
        filterSet->invalidMessageCallback(vePeerInterface, systemAddress, filterSet->filterSetID, filterSet->disallowedCallbackUserData, messageID);
    if (filterSet->banOnDisallowedMessage && vePeerInterface)
    {
        char str1[64];
        systemAddress.systemAddress.ToString(false, str1);
        vePeerInterface->AddToBanList(str1, filterSet->disallowedMessageBanTimeMS);
    }
    if (filterSet->kickOnDisallowedMessage)
    {
        if (vePeerInterface)
            vePeerInterface->CloseConnection(systemAddress, true, 0);
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
        else
            packetizedTCP->CloseConnection(systemAddress.systemAddress);
#endif
    }
}
void MessageFilter::Update(void)
{
    VENet::Time curTime = VENet::GetTime();
    if (GreaterThan(curTime - 1000, whenLastTimeoutCheck))
    {
        DataStructures::List< FilteredSystem > itemList;
        DataStructures::List< AddressOrGUID > keyList;
        systemList.GetAsList(itemList, keyList, _FILE_AND_LINE_);

        unsigned int index;
        for (index=0; index < itemList.Size(); index++)
        {
            if (itemList[index].filter &&
                    itemList[index].filter->maxMemberTimeMS>0 &&
                    curTime-itemList[index].timeEnteredThisSet >= itemList[index].filter->maxMemberTimeMS)
            {
                if (itemList[index].filter->timeoutCallback)
                    itemList[index].filter->timeoutCallback(vePeerInterface, keyList[index], itemList[index].filter->filterSetID, itemList[index].filter->timeoutUserData);

                if (itemList[index].filter->banOnFilterTimeExceed && vePeerInterface)
                {
                    char str1[64];
                    keyList[index].ToString(false, str1);
                    vePeerInterface->AddToBanList(str1, itemList[index].filter->timeExceedBanTimeMS);
                }
                if (vePeerInterface)
                    vePeerInterface->CloseConnection(keyList[index], true, 0);
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
                else
                    packetizedTCP->CloseConnection(keyList[index].systemAddress);
#endif

                systemList.Remove(keyList[index], _FILE_AND_LINE_);
            }
        }

        whenLastTimeoutCheck=curTime+1000;
    }
}
void MessageFilter::OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming)
{
    (void) systemAddress;
    (void) VENetGuid;
    (void) isIncoming;

    AddressOrGUID aog;
    aog.veNetGuid=VENetGuid;
    aog.systemAddress=systemAddress;

    if (autoAddNewConnectionsToFilter>=0 && systemList.HasData(aog)==false)
        SetSystemFilterSet(aog, autoAddNewConnectionsToFilter);
}
void MessageFilter::OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason )
{
    (void) VENetGuid;
    (void) lostConnectionReason;

    AddressOrGUID aog;
    aog.veNetGuid=VENetGuid;
    aog.systemAddress=systemAddress;

    systemList.Remove(aog, _FILE_AND_LINE_);
}
PluginReceiveResult MessageFilter::OnReceive(Packet *packet)
{
    DataStructures::HashIndex index;
    unsigned char messageId;

    switch (packet->data[0])
    {
    case ID_NEW_INCOMING_CONNECTION:
    case ID_CONNECTION_REQUEST_ACCEPTED:
    case ID_CONNECTION_LOST:
    case ID_DISCONNECTION_NOTIFICATION:
    case ID_CONNECTION_ATTEMPT_FAILED:
    case ID_NO_FREE_INCOMING_CONNECTIONS:
    case ID_IP_RECENTLY_CONNECTED:
    case ID_CONNECTION_BANNED:
    case ID_INVALID_PASSWORD:
    case ID_UNCONNECTED_PONG:
    case ID_ALREADY_CONNECTED:
    case ID_ADVERTISE_SYSTEM:
    case ID_REMOTE_DISCONNECTION_NOTIFICATION:
    case ID_REMOTE_CONNECTION_LOST:
    case ID_REMOTE_NEW_INCOMING_CONNECTION:
    case ID_DOWNLOAD_PROGRESS:
        break;
    default:
        if (packet->data[0]==ID_TIMESTAMP)
        {
            if (packet->length<sizeof(MessageID) + sizeof(VENet::TimeMS))
                return RR_STOP_PROCESSING_AND_DEALLOCATE;
            messageId=packet->data[sizeof(MessageID) + sizeof(VENet::TimeMS)];
        }
        else
            messageId=packet->data[0];
        index = systemList.GetIndexOf(packet);
        if (index.IsInvalid())
            break;
        if (systemList.ItemAtIndex(index).filter->allowedIDs[messageId]==false)
        {
            OnInvalidMessage(systemList.ItemAtIndex(index).filter, packet, packet->data[0]);
            return RR_STOP_PROCESSING_AND_DEALLOCATE;
        }
        if (packet->data[0]==ID_RPC_PLUGIN)
        {
            VENet::BitStream bsIn(packet->data,packet->length,false);
            bsIn.IgnoreBytes(2);
            VENet::VEString functionName;
            bsIn.ReadCompressed(functionName);
            if (systemList.ItemAtIndex(index).filter->allowedRPC4.HasData(functionName)==false)
            {
                OnInvalidMessage(systemList.ItemAtIndex(index).filter, packet, packet->data[0]);
                return RR_STOP_PROCESSING_AND_DEALLOCATE;
            }
        }

        break;
    }

    return RR_CONTINUE_PROCESSING;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
