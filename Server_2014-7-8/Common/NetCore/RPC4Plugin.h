////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 RPC4Plugin.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_RPC4Plugin==1

#pragma once

#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "VENetTypes.h"
#include "BitStream.h"
#include "VEString.h"
#include "NetworkIDObject.h"
#include "DS_Hash.h"
#include "DS_OrderedList.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

namespace VENet
{
class VEPeerInterface;
class NetworkIDManager;

enum RPCErrorCodes
{
    RPC_ERROR_FUNCTION_NOT_REGISTERED,
};

class VE_DLL_EXPORT RPC4GlobalRegistration
{
public:
    RPC4GlobalRegistration(const char* uniqueID, void ( *functionPointer ) ( VENet::BitStream *userData, Packet *packet ));

    RPC4GlobalRegistration(const char* uniqueID, void ( *functionPointer ) ( VENet::BitStream *userData, Packet *packet ), int callPriority);

    RPC4GlobalRegistration(const char* uniqueID, void ( *functionPointer ) ( VENet::BitStream *userData, VENet::BitStream *returnData, Packet *packet ));

    RPC4GlobalRegistration(const char* uniqueID, MessageID messageId);
};

class VE_DLL_EXPORT RPC4 : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(RPC4)

    RPC4();

    virtual ~RPC4();

    bool RegisterFunction(const char* uniqueID, void ( *functionPointer ) ( VENet::BitStream *userData, Packet *packet ));

    void RegisterSlot(const char *sharedIdentifier, void ( *functionPointer ) ( VENet::BitStream *userData, Packet *packet ), int callPriority);

    bool RegisterBlockingFunction(const char* uniqueID, void ( *functionPointer ) ( VENet::BitStream *userData, VENet::BitStream *returnData, Packet *packet ));

    void RegisterLocalCallback(const char* uniqueID, MessageID messageId);

    bool UnregisterFunction(const char* uniqueID);

    bool UnregisterBlockingFunction(const char* uniqueID);

    bool UnregisterLocalCallback(const char* uniqueID, MessageID messageId);

    bool UnregisterSlot(const char* sharedIdentifier);

    void CallLoopback( const char* uniqueID, VENet::BitStream * bitStream );

    void Call( const char* uniqueID, VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast );

    bool CallBlocking( const char* uniqueID, VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, VENet::BitStream *returnData );

    void Signal(const char *sharedIdentifier, VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, bool invokeLocal);

    void InterruptSignal(void);

    struct LocalCallback
    {
        MessageID messageId;
        DataStructures::OrderedList<VENet::VEString,VENet::VEString> functions;
    };
    static int LocalCallbackComp(const MessageID &key, LocalCallback* const &data );

    struct LocalSlotObject
    {
        LocalSlotObject() {}
        LocalSlotObject(unsigned int _registrationCount,int _callPriority, void ( *_functionPointer ) ( VENet::BitStream *userData, Packet *packet ))
        {
            registrationCount=_registrationCount;
            callPriority=_callPriority;
            functionPointer=_functionPointer;
        }
        ~LocalSlotObject() {}

        unsigned int registrationCount;
        int callPriority;
        void ( *functionPointer ) ( VENet::BitStream *userData, Packet *packet );
    };

    static int LocalSlotObjectComp( const LocalSlotObject &key, const LocalSlotObject &data );

    struct LocalSlot
    {
        DataStructures::OrderedList<LocalSlotObject,LocalSlotObject,LocalSlotObjectComp> slotObjects;
    };
    DataStructures::Hash<VENet::VEString, LocalSlot*,256, VENet::VEString::ToInteger> localSlots;

protected:
    virtual void OnAttach(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);

    DataStructures::Hash<VENet::VEString, void ( * ) ( VENet::BitStream *, Packet * ),64, VENet::VEString::ToInteger> registeredNonblockingFunctions;
    DataStructures::Hash<VENet::VEString, void ( * ) ( VENet::BitStream *, VENet::BitStream *, Packet * ),64, VENet::VEString::ToInteger> registeredBlockingFunctions;
    DataStructures::OrderedList<MessageID,LocalCallback*,RPC4::LocalCallbackComp> localCallbacks;

    VENet::BitStream blockingReturnValue;
    bool gotBlockingReturnValue;

    DataStructures::HashIndex GetLocalSlotIndex(const char *sharedIdentifier);

    unsigned int nextSlotRegistrationCount;

    bool interruptSignal;

    void InvokeSignal(DataStructures::HashIndex functionIndex, VENet::BitStream *serializedParameters, Packet *packet);
};

}

#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

