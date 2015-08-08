////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PacketFileLogger.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
#pragma once

#include "TCPInterface.h"
#include "DS_ByteQueue.h"
#include "PluginInterface2.h"
#include "DS_Map.h"

namespace VENet
{

class VE_DLL_EXPORT PacketizedTCP : public TCPInterface
{
public:
    STATIC_FACTORY_DECLARATIONS(PacketizedTCP)

    PacketizedTCP();
    virtual ~PacketizedTCP();

    bool Start(unsigned short port, unsigned short maxIncomingConnections, int threadPriority=-99999, unsigned short socketFamily=AF_INET);

    void Stop(void);

    void Send( const char *data, unsigned length, const SystemAddress &systemAddress, bool broadcast );

    bool SendList( const char **data, const int *lengths, const int numParameters, const SystemAddress &systemAddress, bool broadcast );

    Packet* Receive( void );

    void CloseConnection( SystemAddress systemAddress );

    SystemAddress HasCompletedConnectionAttempt(void);

    SystemAddress HasFailedConnectionAttempt(void);

    SystemAddress HasNewIncomingConnection(void);

    SystemAddress HasLostConnection(void);

    void AttachPlugin( PluginInterface2 *plugin );
    void DetachPlugin( PluginInterface2 *plugin );

protected:
    void ClearAllConnections(void);
    void RemoveFromConnectionList(const SystemAddress &sa);
    void AddToConnectionList(const SystemAddress &sa);
    void PushNotificationsToQueues(void);
    Packet *ReturnOutgoingPacket(void);

    DataStructures::List<PluginInterface2*> messageHandlerList;
    DataStructures::Queue<Packet*> waitingPackets;
    DataStructures::Map<SystemAddress, DataStructures::ByteQueue *> connections;

    DataStructures::Queue<SystemAddress> _newIncomingConnections, _lostConnections, _failedConnectionAttempts, _completedConnectionAttempts;
};

}

#endif
