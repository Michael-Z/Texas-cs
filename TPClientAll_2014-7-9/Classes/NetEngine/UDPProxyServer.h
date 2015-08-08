#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_UDPProxyServer==1 && _VENET_SUPPORT_UDPForwarder==1

#pragma once

#include "Export.h"
#include "VENetTypes.h"
#include "PluginInterface2.h"
#include "UDPForwarder.h"
#include "VEString.h"

namespace VENet
{
class UDPProxyServer;

struct UDPProxyServerResultHandler
{
    UDPProxyServerResultHandler() {}
    virtual ~UDPProxyServerResultHandler() {}

    virtual void OnLoginSuccess(VENet::VEString usedPassword, VENet::UDPProxyServer *proxyServerPlugin)=0;

    virtual void OnAlreadyLoggedIn(VENet::VEString usedPassword, VENet::UDPProxyServer *proxyServerPlugin)=0;

    virtual void OnNoPasswordSet(VENet::VEString usedPassword, VENet::UDPProxyServer *proxyServerPlugin)=0;

    virtual void OnWrongPassword(VENet::VEString usedPassword, VENet::UDPProxyServer *proxyServerPlugin)=0;
};

class VE_DLL_EXPORT UDPProxyServer : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(UDPProxyServer)

    UDPProxyServer();
    ~UDPProxyServer();

    void SetSocketFamily(unsigned short _socketFamily);

    void SetResultHandler(UDPProxyServerResultHandler *rh);

    bool LoginToCoordinator(VENet::VEString password, SystemAddress coordinatorAddress);

    void SetServerPublicIP(VEString ip);

    UDPForwarder udpForwarder;

    virtual void OnAttach(void);
    virtual void OnDetach(void);

    virtual void Update(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnRakPeerStartup(void);
    virtual void OnRakPeerShutdown(void);

protected:
    void OnForwardingRequestFromCoordinatorToServer(Packet *packet);

    DataStructures::OrderedList<SystemAddress, SystemAddress> loggingInCoordinators;
    DataStructures::OrderedList<SystemAddress, SystemAddress> loggedInCoordinators;

    UDPProxyServerResultHandler *resultHandler;
    unsigned short socketFamily;
    VEString serverPublicIp;

};

}

#endif
