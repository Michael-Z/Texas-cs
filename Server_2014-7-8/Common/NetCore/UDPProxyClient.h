#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_UDPProxyClient==1

#pragma once

#include "Export.h"
#include "VENetTypes.h"
#include "PluginInterface2.h"
#include "DS_List.h"

namespace VENet
{
class UDPProxyClient;

struct UDPProxyClientResultHandler
{
    UDPProxyClientResultHandler() {}
    virtual ~UDPProxyClientResultHandler() {}

    virtual void OnForwardingSuccess(const char *proxyIPAddress, unsigned short proxyPort,
                                     SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, VENetGUID targetGuid, VENet::UDPProxyClient *proxyClientPlugin)=0;

    virtual void OnForwardingNotification(const char *proxyIPAddress, unsigned short proxyPort,
                                          SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, VENetGUID targetGuid, VENet::UDPProxyClient *proxyClientPlugin)=0;

    virtual void OnNoServersOnline(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, VENetGUID targetGuid, VENet::UDPProxyClient *proxyClientPlugin)=0;

    virtual void OnRecipientNotConnected(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, VENetGUID targetGuid, VENet::UDPProxyClient *proxyClientPlugin)=0;

    virtual void OnAllServersBusy(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, VENetGUID targetGuid, VENet::UDPProxyClient *proxyClientPlugin)=0;

    virtual void OnForwardingInProgress(const char *proxyIPAddress, unsigned short proxyPort, SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, VENetGUID targetGuid, VENet::UDPProxyClient *proxyClientPlugin)=0;
};


class VE_DLL_EXPORT UDPProxyClient : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(UDPProxyClient)

    UDPProxyClient();
    ~UDPProxyClient();

    void SetResultHandler(UDPProxyClientResultHandler *rh);

    bool RequestForwarding(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddressAsSeenFromCoordinator, VENet::TimeMS timeoutOnNoDataMS, VENet::BitStream *serverSelectionBitstream=0);

    bool RequestForwarding(SystemAddress proxyCoordinator, SystemAddress sourceAddress, VENetGUID targetGuid, VENet::TimeMS timeoutOnNoDataMS, VENet::BitStream *serverSelectionBitstream=0);

    virtual void Update(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnRakPeerShutdown(void);

    struct ServerWithPing
    {
        unsigned short ping;
        SystemAddress serverAddress;
    };
    struct SenderAndTargetAddress
    {
        SystemAddress senderClientAddress;
        SystemAddress targetClientAddress;
    };
    struct PingServerGroup
    {
        SenderAndTargetAddress sata;
        VENet::TimeMS startPingTime;
        SystemAddress coordinatorAddressForPings;
        DataStructures::List<ServerWithPing> serversToPing;
        bool AreAllServersPinged(void) const;
        void SendPingedServersToCoordinator(VEPeerInterface *rakPeerInterface);
    };
    DataStructures::List<PingServerGroup*> pingServerGroups;
protected:

    void OnPingServers(Packet *packet);
    void Clear(void);
    UDPProxyClientResultHandler *resultHandler;

};

}

#endif
