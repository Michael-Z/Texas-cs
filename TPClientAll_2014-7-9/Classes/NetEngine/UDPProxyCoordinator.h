#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_UDPProxyCoordinator==1 && _VENET_SUPPORT_UDPForwarder==1

#pragma once

#include "Export.h"
#include "VENetTypes.h"
#include "PluginInterface2.h"
#include "VEString.h"
#include "BitStream.h"
#include "DS_Queue.h"
#include "DS_OrderedList.h"

namespace VENet
{
class VE_DLL_EXPORT UDPProxyCoordinator : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(UDPProxyCoordinator)

    UDPProxyCoordinator();
    virtual ~UDPProxyCoordinator();

    void SetRemoteLoginPassword(VENet::VEString password);

    virtual void Update(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );

    struct SenderAndTargetAddress
    {
        SystemAddress senderClientAddress;
        VENetGUID senderClientGuid;
        SystemAddress targetClientAddress;
        VENetGUID targetClientGuid;
    };

    struct ServerWithPing
    {
        unsigned short ping;
        SystemAddress serverAddress;
    };

    struct ForwardingRequest
    {
        VENet::TimeMS timeoutOnNoDataMS;
        VENet::TimeMS timeoutAfterSuccess;
        SenderAndTargetAddress sata;
        SystemAddress requestingAddress;
        SystemAddress currentlyAttemptedServerAddress;
        DataStructures::Queue<SystemAddress> remainingServersToTry;
        VENet::BitStream serverSelectionBitstream;

        DataStructures::List<ServerWithPing> sourceServerPings, targetServerPings;
        VENet::TimeMS timeRequestedPings;
        void OrderRemainingServersToTry(void);

    };
protected:

    static int ServerWithPingComp( const unsigned short &key, const UDPProxyCoordinator::ServerWithPing &data );
    static int ForwardingRequestComp( const SenderAndTargetAddress &key, ForwardingRequest* const &data);

    void OnForwardingRequestFromClientToCoordinator(Packet *packet);
    void OnLoginRequestFromServerToCoordinator(Packet *packet);
    void OnForwardingReplyFromServerToCoordinator(Packet *packet);
    void OnPingServersReplyFromClientToCoordinator(Packet *packet);
    void TryNextServer(SenderAndTargetAddress sata, ForwardingRequest *fw);
    void SendAllBusy(SystemAddress senderClientAddress, SystemAddress targetClientAddress, VENetGUID targetClientGuid, SystemAddress requestingAddress);
    void Clear(void);

    void SendForwardingRequest(SystemAddress sourceAddress, SystemAddress targetAddress, SystemAddress serverAddress, VENet::TimeMS timeoutOnNoDataMS);

    DataStructures::List<SystemAddress> serverList;

    DataStructures::OrderedList<SenderAndTargetAddress, ForwardingRequest*, ForwardingRequestComp> forwardingRequestList;

    VENet::VEString remoteLoginPassword;

};

}

#endif
