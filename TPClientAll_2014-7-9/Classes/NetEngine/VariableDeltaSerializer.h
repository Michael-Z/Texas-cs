#pragma once

#include "VariableListDeltaTracker.h"
#include "DS_MemoryPool.h"
#include "NativeTypes.h"
#include "BitStream.h"
#include "PacketPriority.h"
#include "DS_OrderedList.h"

namespace VENet
{
class VE_DLL_EXPORT VariableDeltaSerializer
{
protected:
    struct RemoteSystemVariableHistory;
    struct ChangedVariablesList;

public:
    VariableDeltaSerializer();
    ~VariableDeltaSerializer();

    struct SerializationContext
    {
        SerializationContext();
        ~SerializationContext();

        VENetGUID guid;
        BitStream *bitStream;
        uint32_t rakPeerSendReceipt;
        RemoteSystemVariableHistory *variableHistory;
        RemoteSystemVariableHistory *variableHistoryIdentical;
        RemoteSystemVariableHistory *variableHistoryUnique;
        ChangedVariablesList *changedVariables;
        uint32_t sendReceipt;
        PacketReliability serializationMode;
        bool anyVariablesWritten;
        bool newSystemSend;
    };

    struct DeserializationContext
    {
        BitStream *bitStream;
    };

    void BeginUnreliableAckedSerialize(SerializationContext *context, VENetGUID _guid, BitStream *_bitStream, uint32_t _sendReceipt);

    void BeginUniqueSerialize(SerializationContext *context, VENetGUID _guid, BitStream *_bitStream);

    void BeginIdenticalSerialize(SerializationContext *context, bool _isFirstSerializeToThisSystem, BitStream *_bitStream);

    void EndSerialize(SerializationContext *context);

    void BeginDeserialize(DeserializationContext *context, BitStream *_bitStream);

    void EndDeserialize(DeserializationContext *context);

    void AddRemoteSystemVariableHistory(VENetGUID guid);

    void RemoveRemoteSystemVariableHistory(VENetGUID guid);

    void OnPreSerializeTick(void);

    void OnMessageReceipt(VENetGUID guid, uint32_t receiptId, bool messageArrived);

    template <class VarType>
    void SerializeVariable(SerializationContext *context, const VarType &variable)
    {
        if (context->newSystemSend)
        {
            if (context->variableHistory->variableListDeltaTracker.IsPastEndOfList()==false)
            {
                context->bitStream->Write(true);
                context->bitStream->Write(variable);
                context->anyVariablesWritten=true;
            }
            else
            {
                context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream);
                context->anyVariablesWritten=true;
            }
        }
        else if (context->serializationMode==UNRELIABLE_WITH_ACK_RECEIPT)
        {
            context->anyVariablesWritten|=
                context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream, context->changedVariables->bitField, context->changedVariables->bitWriteIndex++);
        }
        else
        {
            if (context->variableHistoryIdentical)
            {
                if (didComparisonThisTick==false)
                    context->anyVariablesWritten|=
                        context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream);
            }
            else
            {
                context->anyVariablesWritten|=
                    context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream);
            }
        }
    }

    template <class VarType>
    bool DeserializeVariable(DeserializationContext *context, VarType &variable)
    {
        return VariableListDeltaTracker::ReadVarFromBitstream(variable, context->bitStream);
    }



protected:

    struct ChangedVariablesList
    {
        uint32_t sendReceipt;
        unsigned short bitWriteIndex;
        unsigned char bitField[56];
    };


    static int UpdatedVariablesListPtrComp( const uint32_t &key, ChangedVariablesList* const &data );

    struct RemoteSystemVariableHistory
    {
        VENetGUID guid;
        VariableListDeltaTracker variableListDeltaTracker;
        DataStructures::OrderedList<uint32_t,ChangedVariablesList*,VariableDeltaSerializer::UpdatedVariablesListPtrComp> updatedVariablesHistory;
    };
    DataStructures::List<RemoteSystemVariableHistory*> remoteSystemVariableHistoryList;

    DataStructures::MemoryPool<ChangedVariablesList> updatedVariablesMemoryPool;

    bool didComparisonThisTick;
    VENet::BitStream identicalSerializationBs;

    void FreeVarsAssociatedWithReceipt(VENetGUID guid, uint32_t receiptId);
    void DirtyAndFreeVarsAssociatedWithReceipt(VENetGUID guid, uint32_t receiptId);
    unsigned int GetVarsWrittenPerRemoteSystemListIndex(VENetGUID guid);
    void RemoveRemoteSystemVariableHistory(void);

    RemoteSystemVariableHistory* GetRemoteSystemVariableHistory(VENetGUID guid);

    ChangedVariablesList *AllocChangedVariablesList(void);
    void FreeChangedVariablesList(ChangedVariablesList *changedVariables);
    void StoreChangedVariablesList(RemoteSystemVariableHistory *variableHistory, ChangedVariablesList *changedVariables, uint32_t sendReceipt);

    RemoteSystemVariableHistory *StartVariableHistoryWrite(VENetGUID guid);
    unsigned int GetRemoteSystemHistoryListIndex(VENetGUID guid);

};

}