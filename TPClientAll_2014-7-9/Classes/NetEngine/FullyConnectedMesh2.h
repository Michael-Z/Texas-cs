////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 FullyConnectedMesh2.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_FullyConnectedMesh2==1

#pragma once

#include "PluginInterface2.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"
#include "DS_List.h"
#include "VEString.h"

typedef int64_t FCM2Guid;

namespace VENet
{
class VEPeerInterface;

class VE_DLL_EXPORT FullyConnectedMesh2 : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(FullyConnectedMesh2)

    FullyConnectedMesh2();
    virtual ~FullyConnectedMesh2();

    void SetConnectOnNewRemoteConnection(bool attemptConnection, VENet::VEString pw);

    VENetGUID GetConnectedHost(void) const;
    SystemAddress GetConnectedHostAddr(void) const;

    VENetGUID GetHostSystem(void) const;

    bool IsHostSystem(void) const;

    void GetHostOrder(DataStructures::List<VENetGUID> &hostList);

    bool IsConnectedHost(void) const;

    void SetAutoparticipateConnections(bool b);

    void ResetHostCalculation(void);

    void AddParticipant(VENetGUID VENetGuid);

    void GetParticipantList(DataStructures::List<VENetGUID> &participantList);

    bool HasParticipant(VENetGUID participantGuid);

    void ConnectToRemoteNewIncomingConnections(Packet *packet);

    void Clear(void);

    unsigned int GetParticipantCount(void) const;
    void GetParticipantCount(unsigned int *participantListSize) const;

    virtual void StartVerifiedJoin(VENetGUID client);

    virtual void RespondOnVerifiedJoinCapable(Packet *packet, bool accept, BitStream *additionalData);

    virtual void GetVerifiedJoinRequiredProcessingList(VENetGUID host, DataStructures::List<SystemAddress> &addresses, DataStructures::List<VENetGUID> &guids);

    virtual void GetVerifiedJoinAcceptedAdditionalData(Packet *packet, bool *thisSystemAccepted, DataStructures::List<VENetGUID> &systemsAccepted, BitStream *additionalData);

    virtual void GetVerifiedJoinRejectedAdditionalData(Packet *packet, BitStream *additionalData);

    VENet::TimeUS GetElapsedRuntime(void);

    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnRakPeerStartup(void);
    virtual void OnAttach(void);
    virtual void OnRakPeerShutdown(void);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);
    virtual void OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason);

    struct FCM2Participant
    {
        FCM2Participant() {}
        FCM2Participant(const FCM2Guid &_fcm2Guid, const VENetGUID &_VENetGuid) : fcm2Guid(_fcm2Guid), VENetGuid(_VENetGuid) {}

        FCM2Guid fcm2Guid;
        VENetGUID VENetGuid;
    };

    enum JoinInProgressState
    {
        JIPS_PROCESSING,
        JIPS_FAILED,
        JIPS_CONNECTED,
        JIPS_UNNECESSARY,
    };

    struct VerifiedJoinInProgressMember
    {
        SystemAddress systemAddress;
        VENetGUID guid;
        JoinInProgressState joinInProgressState;

        bool workingFlag;
    };

    struct VerifiedJoinInProgress
    {
        VENetGUID requester;
        DataStructures::List<VerifiedJoinInProgressMember> members;
        //bool sentResults;
    };

    unsigned int GetTotalConnectionCount(void) const;

protected:
    void PushNewHost(const VENetGUID &guid, VENetGUID oldHost);
    void SendOurFCMGuid(SystemAddress addr);
    void SendFCMGuidRequest(VENetGUID VENetGuid);
    void SendConnectionCountResponse(SystemAddress addr, unsigned int responseTotalConnectionCount);
    void OnRequestFCMGuid(Packet *packet);
    void OnRespondConnectionCount(Packet *packet);
    void OnInformFCMGuid(Packet *packet);
    void OnUpdateMinTotalConnectionCount(Packet *packet);
    void AssignOurFCMGuid(void);
    void CalculateHost(VENetGUID *VENetGuid, FCM2Guid *fcm2Guid);
    bool AddParticipantInternal( VENetGUID VENetGuid, FCM2Guid theirFCMGuid );
    void CalculateAndPushHost(void);
    bool ParticipantListComplete(void);
    void IncrementTotalConnectionCount(unsigned int i);
    PluginReceiveResult OnVerifiedJoinStart(Packet *packet);
    PluginReceiveResult OnVerifiedJoinCapable(Packet *packet);
    virtual void OnVerifiedJoinFailed(VENetGUID hostGuid);
    virtual void OnVerifiedJoinAccepted(Packet *packet);
    virtual void OnVerifiedJoinRejected(Packet *packet);
    unsigned int GetJoinsInProgressIndex(VENetGUID requester) const;
    void UpdateVerifiedJoinInProgressMember(const AddressOrGUID systemIdentifier, VENetGUID guidToAssign, JoinInProgressState newState);
    bool ProcessVerifiedJoinInProgressIfCompleted(VerifiedJoinInProgress *vjip);
    void ReadVerifiedJoinInProgressMember(VENet::BitStream *bsIn, VerifiedJoinInProgressMember *vjipm);
    unsigned int GetVerifiedJoinInProgressMemberIndex(const AddressOrGUID systemIdentifier, VerifiedJoinInProgress *vjip);
    void DecomposeJoinCapable(Packet *packet, VerifiedJoinInProgress *vjip);
    void CategorizeVJIP(VerifiedJoinInProgress *vjip,
                        DataStructures::List<VENetGUID> &participatingMembersOnClientSucceeded,
                        DataStructures::List<VENetGUID> &participatingMembersOnClientFailed,
                        DataStructures::List<VENetGUID> &participatingMembersNotOnClient,
                        DataStructures::List<VENetGUID> &clientMembersNotParticipatingSucceeded,
                        DataStructures::List<VENetGUID> &clientMembersNotParticipatingFailed);

    VENet::TimeUS startupTime;

    bool autoParticipateConnections;

    unsigned int totalConnectionCount;

    FCM2Guid ourFCMGuid;

    DataStructures::List<FCM2Participant> fcm2ParticipantList;

    VENetGUID lastPushedHost;

    VENetGUID hostVENetGuid;
    FCM2Guid hostFCM2Guid;

    VENet::VEString connectionPassword;
    bool connectOnNewRemoteConnections;

    DataStructures::List<VerifiedJoinInProgress*> joinsInProgress;
};

}

#endif
