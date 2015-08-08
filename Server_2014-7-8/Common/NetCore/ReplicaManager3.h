////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 ReplicaManager3.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_ReplicaManager3==1

#pragma once

#include "VENetTypes.h"
#include "VENetTime.h"
#include "BitStream.h"
#include "PacketPriority.h"
#include "PluginInterface2.h"
#include "NetworkIDObject.h"
#include "DS_OrderedList.h"
#include "DS_Queue.h"

namespace VENet
{
class Connection_RM3;
class Replica3;

typedef uint8_t WorldId;


struct PRO
{
    PacketPriority priority;

    PacketReliability reliability;

    char orderingChannel;

    uint32_t sendReceipt;

    bool operator==( const PRO& right ) const;
    bool operator!=( const PRO& right ) const;
};


class VE_DLL_EXPORT ReplicaManager3 : public PluginInterface2
{
public:
    ReplicaManager3();
    virtual ~ReplicaManager3();

    virtual Connection_RM3* AllocConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid) const=0;

    virtual void DeallocConnection(Connection_RM3 *connection) const=0;

    void SetAutoManageConnections(bool autoCreate, bool autoDestroy);

    bool GetAutoCreateConnections(void) const;

    bool GetAutoDestroyConnections(void) const;

    bool PushConnection(VENet::Connection_RM3 *newConnection, WorldId worldId=0);

    VENet::Connection_RM3 * PopConnection(VENetGUID guid, WorldId worldId=0);

    void Reference(VENet::Replica3 *replica3, WorldId worldId=0);

    void Dereference(VENet::Replica3 *replica3, WorldId worldId=0);

    void DereferenceList(DataStructures::List<Replica3*> &replicaListIn, WorldId worldId=0);

    void GetReplicasCreatedByGuid(VENetGUID guid, DataStructures::List<Replica3*> &replicaListOut, WorldId worldId=0);

    void GetReplicasCreatedByMe(DataStructures::List<Replica3*> &replicaListOut, WorldId worldId=0);

    void GetReferencedReplicaList(DataStructures::List<Replica3*> &replicaListOut, WorldId worldId=0);

    unsigned GetReplicaCount(WorldId worldId=0) const;

    Replica3 *GetReplicaAtIndex(unsigned index, WorldId worldId=0);

    unsigned int GetConnectionCount(WorldId worldId=0) const;

    Connection_RM3* GetConnectionAtIndex(unsigned index, WorldId worldId=0) const;

    Connection_RM3* GetConnectionBySystemAddress(const SystemAddress &sa, WorldId worldId=0) const;

    Connection_RM3* GetConnectionByGUID(VENetGUID guid, WorldId worldId=0) const;

    void SetDefaultOrderingChannel(char def);

    void SetDefaultPacketPriority(PacketPriority def);

    void SetDefaultPacketReliability(PacketReliability def);

    void SetAutoSerializeInterval(VENet::Time intervalMS);

    void GetConnectionsThatHaveReplicaConstructed(Replica3 *replica, DataStructures::List<Connection_RM3*> &connectionsThatHaveConstructedThisReplica, WorldId worldId=0);

    bool GetAllConnectionDownloadsCompleted(WorldId worldId=0) const;

    void AddWorld(WorldId worldId);

    void RemoveWorld(WorldId worldId);

    WorldId GetWorldIdAtIndex(unsigned int index);

    unsigned int GetWorldCount(void) const;

    void SetNetworkIDManager(NetworkIDManager *_networkIDManager, WorldId worldId=0);

    NetworkIDManager *GetNetworkIDManager(WorldId worldId=0) const;

    void BroadcastDestructionList(DataStructures::List<Replica3*> &replicaListSource, const SystemAddress &exclusionAddress, WorldId worldId=0);

    void BroadcastDestruction(Replica3 *replica, const SystemAddress &exclusionAddress);

    void Clear(bool deleteWorlds=false);

    PRO GetDefaultSendParameters(void) const;

    virtual void Update(void);

    struct RM3World
    {
        RM3World();
        void Clear(ReplicaManager3 *replicaManager3);

        DataStructures::List<Connection_RM3*> connectionList;
        DataStructures::List<Replica3*> userReplicaList;
        WorldId worldId;
        NetworkIDManager *networkIDManager;
    };
protected:
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);
    virtual void OnVEPeerShutdown(void);
    virtual void OnDetach(void);

    PluginReceiveResult OnConstruction(Packet *packet, unsigned char *packetData, int packetDataLength, VENetGUID senderGuid, unsigned char packetDataOffset, WorldId worldId);
    PluginReceiveResult OnSerialize(Packet *packet, unsigned char *packetData, int packetDataLength, VENetGUID senderGuid, VENet::Time timestamp, unsigned char packetDataOffset, WorldId worldId);
    PluginReceiveResult OnDownloadStarted(Packet *packet, unsigned char *packetData, int packetDataLength, VENetGUID senderGuid, unsigned char packetDataOffset, WorldId worldId);
    PluginReceiveResult OnDownloadComplete(Packet *packet, unsigned char *packetData, int packetDataLength, VENetGUID senderGuid, unsigned char packetDataOffset, WorldId worldId);

    void DeallocReplicaNoBroadcastDestruction(VENet::Connection_RM3 *connection, VENet::Replica3 *replica3);
    VENet::Connection_RM3 * PopConnection(unsigned int index, WorldId worldId);
    Replica3* GetReplicaByNetworkID(NetworkID networkId, WorldId worldId);
    unsigned int ReferenceInternal(VENet::Replica3 *replica3, WorldId worldId);

    PRO defaultSendParameters;
    VENet::Time autoSerializeInterval;
    VENet::Time lastAutoSerializeOccurance;
    bool autoCreateConnections, autoDestroyConnections;
    Replica3 *currentlyDeallocatingReplica;

    RM3World *worldsArray[255];
    DataStructures::List<RM3World *> worldsList;

    friend class Connection_RM3;
};

static const int RM3_NUM_OUTPUT_BITSTREAM_CHANNELS=16;

struct LastSerializationResultBS
{
    VENet::BitStream bitStream[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
    bool indicesToSend[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
};

struct LastSerializationResult
{
    LastSerializationResult();
    ~LastSerializationResult();

    VENet::Replica3 *replica;
    VENet::Time whenLastSerialized;

    void AllocBS(void);
    LastSerializationResultBS* lastSerializationResultBS;
};

struct SerializeParameters
{
    VENet::BitStream outputBitstream[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];

    VENet::BitStream* lastSentBitstream[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];

    VENet::Time messageTimestamp;

    PRO pro[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];

    VENet::Connection_RM3 *destinationConnection;

    BitSize_t bitsWrittenSoFar;

    VENet::Time whenLastSerialized;

    VENet::Time curTime;
};

struct DeserializeParameters
{
    VENet::BitStream serializationBitstream[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
    bool bitstreamWrittenTo[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS];
    VENet::Time timeStamp;
    VENet::Connection_RM3 *sourceConnection;
};

enum SendSerializeIfChangedResult
{
    SSICR_SENT_DATA,
    SSICR_DID_NOT_SEND_DATA,
    SSICR_NEVER_SERIALIZE,
};

class VE_DLL_EXPORT Connection_RM3
{
public:

    Connection_RM3(const SystemAddress &_systemAddress, VENetGUID _guid);
    virtual ~Connection_RM3();

    virtual Replica3 *AllocReplica(VENet::BitStream *allocationIdBitstream, ReplicaManager3 *replicaManager3)=0;

    virtual void GetConstructedReplicas(DataStructures::List<Replica3*> &objectsTheyDoHave);

    bool HasReplicaConstructed(VENet::Replica3 *replica);

    virtual void SerializeOnDownloadStarted(VENet::BitStream *bitStream)
    {
        (void) bitStream;
    }

    virtual void DeserializeOnDownloadStarted(VENet::BitStream *bitStream)
    {
        (void) bitStream;
    }

    virtual void SerializeOnDownloadComplete(VENet::BitStream *bitStream)
    {
        (void) bitStream;
    }

    virtual void DeserializeOnDownloadComplete(VENet::BitStream *bitStream)
    {
        (void) bitStream;
    }

    SystemAddress GetSystemAddress(void) const
    {
        return systemAddress;
    }

    VENetGUID GetVENetGuid(void) const
    {
        return guid;
    }

    bool GetDownloadWasCompleted(void) const
    {
        return gotDownloadComplete;
    }

    enum ConstructionMode
    {
        QUERY_REPLICA_FOR_CONSTRUCTION,

        QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION,

        QUERY_CONNECTION_FOR_REPLICA_LIST
    };

    virtual bool QueryGroupDownloadMessages(void) const
    {
        return false;
    }

    virtual ConstructionMode QueryConstructionMode(void) const
    {
        return QUERY_REPLICA_FOR_CONSTRUCTION_AND_DESTRUCTION;
    }

    virtual void QueryReplicaList(
        DataStructures::List<Replica3*> &newReplicasToCreate,
        DataStructures::List<Replica3*> &existingReplicasToDestroy)
    {
        (void) newReplicasToCreate;
        (void) existingReplicasToDestroy;
    }

    virtual bool QuerySerializationList(DataStructures::List<Replica3*> &replicasToSerialize)
    {
        (void) replicasToSerialize;
        return false;
    }

    virtual SendSerializeIfChangedResult SendSerialize(VENet::Replica3 *replica, bool indicesToSend[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS], VENet::BitStream serializationData[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS], VENet::Time timestamp, PRO sendParameters[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS], VENet::VEPeerInterface *rakPeer, unsigned char worldId, VENet::Time curTime);

    virtual SendSerializeIfChangedResult SendSerializeIfChanged(LastSerializationResult *lsr, SerializeParameters *sp, VENet::VEPeerInterface *rakPeer, unsigned char worldId, ReplicaManager3 *replicaManager, VENet::Time curTime);

    virtual void SendConstruction(DataStructures::List<Replica3*> &newObjects, DataStructures::List<Replica3*> &deletedObjects, PRO sendParameters, VENet::VEPeerInterface *rakPeer, unsigned char worldId, ReplicaManager3 *replicaManager3);

    void SendValidation(VENet::VEPeerInterface *rakPeer, WorldId worldId);

    void AutoConstructByQuery(ReplicaManager3 *replicaManager3, WorldId worldId);


    bool isValidated;
    bool isFirstConstruction;

    static int Replica3LSRComp( Replica3 * const &replica3, LastSerializationResult * const &data );

    void ClearDownloadGroup(VEPeerInterface *rakPeerInterface);
protected:

    SystemAddress systemAddress;
    VENetGUID guid;

    void OnLocalReference(Replica3* replica3, ReplicaManager3 *replicaManager);
    void OnDereference(Replica3* replica3, ReplicaManager3 *replicaManager);
    void OnDownloadFromThisSystem(Replica3* replica3, ReplicaManager3 *replicaManager);
    void OnDownloadFromOtherSystem(Replica3* replica3, ReplicaManager3 *replicaManager);
    void OnNeverConstruct(unsigned int queryToConstructIdx, ReplicaManager3 *replicaManager);
    void OnConstructToThisConnection(unsigned int queryToConstructIdx, ReplicaManager3 *replicaManager);
    void OnConstructToThisConnection(Replica3 *replica, ReplicaManager3 *replicaManager);
    void OnNeverSerialize(LastSerializationResult *lsr, ReplicaManager3 *replicaManager);
    void OnReplicaAlreadyExists(unsigned int queryToConstructIdx, ReplicaManager3 *replicaManager);
    void OnDownloadExisting(Replica3* replica3, ReplicaManager3 *replicaManager);
    void OnSendDestructionFromQuery(unsigned int queryToDestructIdx, ReplicaManager3 *replicaManager);
    void OnDoNotQueryDestruction(unsigned int queryToDestructIdx, ReplicaManager3 *replicaManager);
    void ValidateLists(ReplicaManager3 *replicaManager) const;
    void SendSerializeHeader(VENet::Replica3 *replica, VENet::Time timestamp, VENet::BitStream *bs, WorldId worldId);

    DataStructures::OrderedList<Replica3*, LastSerializationResult*, Connection_RM3::Replica3LSRComp> constructedReplicaList;

    DataStructures::List<LastSerializationResult*> queryToConstructReplicaList;

    DataStructures::List<LastSerializationResult*> queryToSerializeReplicaList;

    DataStructures::List<LastSerializationResult*> queryToDestructReplicaList;

    DataStructures::List<Replica3*> constructedReplicasCulled, destroyedReplicasCulled;

    bool groupConstructionAndSerialize;
    DataStructures::Queue<Packet*> downloadGroup;

    bool gotDownloadComplete;

    friend class ReplicaManager3;
private:
    Connection_RM3() {};

    ConstructionMode constructionMode;
};

enum RM3ConstructionState
{
    RM3CS_SEND_CONSTRUCTION,

    RM3CS_ALREADY_EXISTS_REMOTELY,

    RM3CS_ALREADY_EXISTS_REMOTELY_DO_NOT_CONSTRUCT,

    RM3CS_NEVER_CONSTRUCT,

    RM3CS_NO_ACTION,

    RM3CS_MAX,
};

enum RM3DestructionState
{
    RM3DS_SEND_DESTRUCTION,

    RM3DS_DO_NOT_QUERY_DESTRUCTION,

    RM3DS_NO_ACTION,

    RM3DS_MAX,
};

enum RM3SerializationResult
{
    RM3SR_BROADCAST_IDENTICALLY,

    RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION,

    RM3SR_SERIALIZED_UNIQUELY,

    RM3SR_SERIALIZED_ALWAYS,

    RM3SR_SERIALIZED_ALWAYS_IDENTICALLY,

    RM3SR_DO_NOT_SERIALIZE,

    RM3SR_NEVER_SERIALIZE_FOR_THIS_CONNECTION,

    RM3SR_MAX,
};

enum RM3QuerySerializationResult
{
    RM3QSR_CALL_SERIALIZE,
    RM3QSR_DO_NOT_CALL_SERIALIZE,
    RM3QSR_NEVER_CALL_SERIALIZE,
    RM3QSR_MAX,
};

enum RM3ActionOnPopConnection
{
    RM3AOPC_DO_NOTHING,
    RM3AOPC_DELETE_REPLICA,
    RM3AOPC_DELETE_REPLICA_AND_BROADCAST_DESTRUCTION,
    RM3AOPC_MAX,
};

enum Replica3P2PMode
{
    R3P2PM_SINGLE_OWNER,
    R3P2PM_MULTI_OWNER_CURRENTLY_AUTHORITATIVE,
    R3P2PM_MULTI_OWNER_NOT_CURRENTLY_AUTHORITATIVE,
    R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE,
    R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE,

};

class VE_DLL_EXPORT Replica3 : public NetworkIDObject
{
public:
    Replica3();

    virtual ~Replica3();

    virtual void WriteAllocationID(VENet::Connection_RM3 *destinationConnection, VENet::BitStream *allocationIdBitstream) const=0;

    virtual RM3ConstructionState QueryConstruction(VENet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)=0;

    virtual RM3DestructionState QueryDestruction(VENet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)
    {
        (void) destinationConnection;
        (void) replicaManager3;
        return RM3DS_DO_NOT_QUERY_DESTRUCTION;
    }

    virtual bool QueryRemoteConstruction(VENet::Connection_RM3 *sourceConnection)=0;

    virtual bool QueryRelayDestruction(Connection_RM3 *sourceConnection) const
    {
        (void) sourceConnection;
        return true;
    }

    virtual void SerializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)=0;

    virtual bool DeserializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)=0;

    virtual void SerializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        (void) constructionBitstream;
        (void) destinationConnection;
    };

    virtual void DeserializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        (void) constructionBitstream;
        (void) sourceConnection;
    };

    virtual void SerializeDestruction(VENet::BitStream *destructionBitstream, VENet::Connection_RM3 *destinationConnection)=0;

    virtual bool DeserializeDestruction(VENet::BitStream *destructionBitstream, VENet::Connection_RM3 *sourceConnection)=0;

    virtual VENet::RM3ActionOnPopConnection QueryActionOnPopConnection(VENet::Connection_RM3 *droppedConnection) const=0;

    virtual void OnPoppedConnection(VENet::Connection_RM3 *droppedConnection)
    {
        (void) droppedConnection;
    }

    virtual void DeallocReplica(VENet::Connection_RM3 *sourceConnection)=0;

    virtual VENet::RM3QuerySerializationResult QuerySerialization(VENet::Connection_RM3 *destinationConnection)=0;

    virtual void OnUserReplicaPreSerializeTick(void) {}

    virtual RM3SerializationResult Serialize(VENet::SerializeParameters *serializeParameters)=0;

    virtual void OnSerializeTransmission(VENet::BitStream *bitStream, VENet::Connection_RM3 *destinationConnection, BitSize_t bitsPerChannel[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS], VENet::Time curTime)
    {
        (void) bitStream;
        (void) destinationConnection;
        (void) bitsPerChannel;
        (void) curTime;
    }

    virtual void Deserialize(VENet::DeserializeParameters *deserializeParameters)=0;

    virtual void PostSerializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        (void) constructionBitstream;
        (void) destinationConnection;
    }

    virtual void PostDeserializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        (void) constructionBitstream;
        (void) sourceConnection;
    }

    virtual void PostSerializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        (void) constructionBitstream;
        (void) destinationConnection;
    }

    virtual void PostDeserializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        (void) constructionBitstream;
        (void) sourceConnection;
    }

    virtual void PreDestruction(VENet::Connection_RM3 *sourceConnection)
    {
        (void) sourceConnection;
    }

    virtual RM3ConstructionState QueryConstruction_ClientConstruction(VENet::Connection_RM3 *destinationConnection, bool isThisTheServer);

    virtual bool QueryRemoteConstruction_ClientConstruction(VENet::Connection_RM3 *sourceConnection, bool isThisTheServer);

    virtual RM3ConstructionState QueryConstruction_ServerConstruction(VENet::Connection_RM3 *destinationConnection, bool isThisTheServer);

    virtual bool QueryRemoteConstruction_ServerConstruction(VENet::Connection_RM3 *sourceConnection, bool isThisTheServer);

    virtual RM3ConstructionState QueryConstruction_PeerToPeer(VENet::Connection_RM3 *destinationConnection, Replica3P2PMode p2pMode=R3P2PM_SINGLE_OWNER);

    virtual bool QueryRemoteConstruction_PeerToPeer(VENet::Connection_RM3 *sourceConnection);

    virtual VENet::RM3QuerySerializationResult QuerySerialization_ClientSerializable(VENet::Connection_RM3 *destinationConnection, bool isThisTheServer);

    virtual VENet::RM3QuerySerializationResult QuerySerialization_ServerSerializable(VENet::Connection_RM3 *destinationConnection, bool isThisTheServer);

    virtual VENet::RM3QuerySerializationResult QuerySerialization_PeerToPeer(VENet::Connection_RM3 *destinationConnection, Replica3P2PMode p2pMode=R3P2PM_SINGLE_OWNER);

    virtual RM3ActionOnPopConnection QueryActionOnPopConnection_Client(VENet::Connection_RM3 *droppedConnection) const;
    virtual RM3ActionOnPopConnection QueryActionOnPopConnection_Server(VENet::Connection_RM3 *droppedConnection) const;
    virtual RM3ActionOnPopConnection QueryActionOnPopConnection_PeerToPeer(VENet::Connection_RM3 *droppedConnection) const;

    virtual void BroadcastDestruction(void);

    VENetGUID GetCreatingSystemGUID(void) const;

    bool WasReferenced(void) const
    {
        return replicaManager!=0;
    }

    VENetGUID creatingSystemGUID;
    VENetGUID deletingSystemGUID;

    ReplicaManager3 *replicaManager;

    LastSerializationResultBS lastSentSerialization;
    bool forceSendUntilNextUpdate;
    LastSerializationResult *lsr;
};

template <class parent_type>
class VE_DLL_EXPORT Replica3Composite : public Replica3
{
protected:
    parent_type *r3CompositeOwner;
public:
    void SetCompositeOwner(parent_type *p)
    {
        r3CompositeOwner=p;
    }
    parent_type* GetCompositeOwner(void) const
    {
        return r3CompositeOwner;
    };
    virtual void WriteAllocationID(VENet::Connection_RM3 *destinationConnection, VENet::BitStream *allocationIdBitstream) const
    {
        r3CompositeOwner->WriteAllocationID(destinationConnection, allocationIdBitstream);
    }
    virtual RM3ConstructionState QueryConstruction(VENet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)
    {
        return r3CompositeOwner->QueryConstruction(destinationConnection, replicaManager3);
    }
    virtual RM3DestructionState QueryDestruction(VENet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)
    {
        return r3CompositeOwner->QueryDestruction(destinationConnection, replicaManager3);
    }
    virtual bool QueryRemoteConstruction(VENet::Connection_RM3 *sourceConnection)
    {
        return r3CompositeOwner->QueryRemoteConstruction(sourceConnection);
    }
    virtual bool QueryRelayDestruction(Connection_RM3 *sourceConnection) const
    {
        return r3CompositeOwner->QueryRelayDestruction(sourceConnection);
    }
    virtual void SerializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        r3CompositeOwner->SerializeConstruction(constructionBitstream, destinationConnection);
    }
    virtual bool DeserializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        return r3CompositeOwner->DeserializeConstruction(constructionBitstream, sourceConnection);
    }
    virtual void SerializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        r3CompositeOwner->SerializeConstructionExisting(constructionBitstream, destinationConnection);
    }
    virtual void DeserializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        r3CompositeOwner->DeserializeConstructionExisting(constructionBitstream, sourceConnection);
    }
    virtual void SerializeDestruction(VENet::BitStream *destructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        r3CompositeOwner->SerializeDestruction(destructionBitstream, destinationConnection);
    }
    virtual bool DeserializeDestruction(VENet::BitStream *destructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        return r3CompositeOwner->DeserializeDestruction(destructionBitstream, sourceConnection);
    }
    virtual VENet::RM3ActionOnPopConnection QueryActionOnPopConnection(VENet::Connection_RM3 *droppedConnection) const
    {
        return r3CompositeOwner->QueryActionOnPopConnection(droppedConnection);
    }
    virtual void OnPoppedConnection(VENet::Connection_RM3 *droppedConnection)
    {
        r3CompositeOwner->OnPoppedConnection(droppedConnection);
    }
    virtual void DeallocReplica(VENet::Connection_RM3 *sourceConnection)
    {
        r3CompositeOwner->DeallocReplica(sourceConnection);
    }
    virtual VENet::RM3QuerySerializationResult QuerySerialization(VENet::Connection_RM3 *destinationConnection)
    {
        return r3CompositeOwner->QuerySerialization(destinationConnection);
    }
    virtual void OnUserReplicaPreSerializeTick(void)
    {
        r3CompositeOwner->OnUserReplicaPreSerializeTick();
    }
    virtual RM3SerializationResult Serialize(VENet::SerializeParameters *serializeParameters)
    {
        return r3CompositeOwner->Serialize(serializeParameters);
    }
    virtual void OnSerializeTransmission(VENet::BitStream *bitStream, VENet::Connection_RM3 *destinationConnection, BitSize_t bitsPerChannel[RM3_NUM_OUTPUT_BITSTREAM_CHANNELS], VENet::Time curTime)
    {
        r3CompositeOwner->OnSerializeTransmission(bitStream, destinationConnection, bitsPerChannel, curTime);
    }
    virtual void Deserialize(VENet::DeserializeParameters *deserializeParameters)
    {
        r3CompositeOwner->Deserialize(deserializeParameters);
    }
    virtual void PostSerializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        r3CompositeOwner->PostSerializeConstruction(constructionBitstream, destinationConnection);
    }
    virtual void PostDeserializeConstruction(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        r3CompositeOwner->PostDeserializeConstruction(constructionBitstream, sourceConnection);
    }
    virtual void PostSerializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *destinationConnection)
    {
        r3CompositeOwner->PostSerializeConstructionExisting(constructionBitstream, destinationConnection);
    }
    virtual void PostDeserializeConstructionExisting(VENet::BitStream *constructionBitstream, VENet::Connection_RM3 *sourceConnection)
    {
        r3CompositeOwner->PostDeserializeConstructionExisting(constructionBitstream, sourceConnection);
    }
    virtual void PreDestruction(VENet::Connection_RM3 *sourceConnection)
    {
        r3CompositeOwner->PreDestruction(sourceConnection);
    }
};

}


#endif
