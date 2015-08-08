////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PluginInterface2.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "NativeFeatureIncludes.h"
#include "VENetTypes.h"
#include "Export.h"
#include "PacketPriority.h"

namespace VENet
{

class VEPeerInterface;
class PacketizedTCP;
struct Packet;
struct InternalPacket;

enum PluginReceiveResult
{
    RR_STOP_PROCESSING_AND_DEALLOCATE=0,

    RR_CONTINUE_PROCESSING,

    RR_STOP_PROCESSING
};

enum PI2_LostConnectionReason
{
    LCR_CLOSED_BY_USER,

    LCR_DISCONNECTION_NOTIFICATION,

    LCR_CONNECTION_LOST
};

enum PI2_FailedConnectionAttemptReason
{
    FCAR_CONNECTION_ATTEMPT_FAILED,
    FCAR_ALREADY_CONNECTED,
    FCAR_NO_FREE_INCOMING_CONNECTIONS,
    FCAR_SECURITY_PUBLIC_KEY_MISMATCH,
    FCAR_CONNECTION_BANNED,
    FCAR_INVALID_PASSWORD,
    FCAR_INCOMPATIBLE_PROTOCOL,
    FCAR_IP_RECENTLY_CONNECTED,
    FCAR_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY,
    FCAR_OUR_SYSTEM_REQUIRES_SECURITY,
    FCAR_PUBLIC_KEY_MISMATCH
};

class VE_DLL_EXPORT PluginInterface2
{
public:
    PluginInterface2();
    virtual ~PluginInterface2();

    virtual void OnAttach(void) {}

    virtual void OnDetach(void) {}

    virtual void Update(void) {}

    virtual PluginReceiveResult OnReceive(Packet *packet)
    {
        (void) packet;
        return RR_CONTINUE_PROCESSING;
    }

    virtual void OnRakPeerStartup(void) {}

    virtual void OnVEPeerShutdown(void) {}

    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason )
    {
        (void) systemAddress;
        (void) VENetGuid;
        (void) lostConnectionReason;
    }

    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming)
    {
        (void) systemAddress;
        (void) VENetGuid;
        (void) isIncoming;
    }

    virtual void OnFailedConnectionAttempt(Packet *packet, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason)
    {
        (void) packet;
        (void) failedConnectionAttemptReason;
    }

    virtual bool UsesReliabilityLayer(void) const
    {
        return false;
    }

    virtual void OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
    {
        (void) data;
        (void) bitsUsed;
        (void) remoteSystemAddress;
    }

    virtual void OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
    {
        (void) data;
        (void) bitsUsed;
        (void) remoteSystemAddress;
    }

    virtual void OnReliabilityLayerNotification(const char *errorMessage, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress, bool isError)
    {
        (void) errorMessage;
        (void) bitsUsed;
        (void) remoteSystemAddress;
        (void) isError;
    }

    virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, VENet::TimeMS time, int isSend)
    {
        (void) internalPacket;
        (void) frameNumber;
        (void) remoteSystemAddress;
        (void) time;
        (void) isSend;
    }

    virtual void OnAck(unsigned int messageNumber, SystemAddress remoteSystemAddress, VENet::TimeMS time)
    {
        (void) messageNumber;
        (void) remoteSystemAddress;
        (void) time;
    }

    virtual void OnPushBackPacket(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
    {
        (void) data;
        (void) bitsUsed;
        (void) remoteSystemAddress;
    }

    VEPeerInterface *GetRakPeerInterface(void) const
    {
        return vePeerInterface;
    }

    VENetGUID GetMyGUIDUnified(void) const;

    void SetRakPeerInterface( VEPeerInterface *ptr );

#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    void SetPacketizedTCP( PacketizedTCP *ptr );
#endif

protected:
    void SendUnified( const VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast );
    void SendUnified( const char * data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast );
    bool SendListUnified( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast );

    Packet *AllocatePacketUnified(unsigned dataSize);
    void PushBackPacketUnified(Packet *packet, bool pushAtHead);
    void DeallocPacketUnified(Packet *packet);

    VEPeerInterface *vePeerInterface;
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    PacketizedTCP *packetizedTCP;
#endif
};

}
