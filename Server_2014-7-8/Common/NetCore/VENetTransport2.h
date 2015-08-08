////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 VENetTransport2.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TelnetTransport==1

#pragma once

#include "TransportInterface.h"
#include "DS_Queue.h"
#include "CommandParserInterface.h"
#include "PluginInterface2.h"
#include "Export.h"

namespace VENet
{
class BitStream;
class VEPeerInterface;
class VENetTransport;

class VE_DLL_EXPORT VENetTransport2 : public TransportInterface, public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(VENetTransport2)

    VENetTransport2();
    virtual ~VENetTransport2();

    bool Start(unsigned short port, bool serverMode);

    void Stop(void);

    void Send( SystemAddress systemAddress, const char *data, ... );

    void CloseConnection( SystemAddress systemAddress );

    Packet* Receive( void );

    void DeallocatePacket( Packet *packet );

    SystemAddress HasNewIncomingConnection(void);

    SystemAddress HasLostConnection(void);

    virtual CommandParserInterface* GetCommandParser(void)
    {
        return 0;
    }

    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);
protected:
    DataStructures::Queue<SystemAddress> newConnections, lostConnections;
    DataStructures::Queue<Packet*> packetQueue;
};

}

#endif