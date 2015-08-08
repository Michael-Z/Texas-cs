#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TelnetTransport==1

#include "VENetTransport2.h"

#include "VEPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "LinuxStrings.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

using namespace VENet;

STATIC_FACTORY_DEFINITIONS(VENetTransport2,VENetTransport2);

VENetTransport2::VENetTransport2()
{
}
VENetTransport2::~VENetTransport2()
{
    Stop();
}
bool VENetTransport2::Start(unsigned short port, bool serverMode)
{
    (void) port;
    (void) serverMode;
    return true;
}
void VENetTransport2::Stop(void)
{
    newConnections.Clear(_FILE_AND_LINE_);
    lostConnections.Clear(_FILE_AND_LINE_);
    for (unsigned int i=0; i < packetQueue.Size(); i++)
    {
        veFree_Ex(packetQueue[i]->data,_FILE_AND_LINE_);
        VENet::OP_DELETE(packetQueue[i],_FILE_AND_LINE_);
    }
    packetQueue.Clear(_FILE_AND_LINE_);
}
void VENetTransport2::Send( SystemAddress systemAddress, const char *data, ... )
{
    if (data==0 || data[0]==0) return;

    char text[REMOTE_MAX_TEXT_INPUT];
    va_list ap;
    va_start(ap, data);
    vsnprintf(text, REMOTE_MAX_TEXT_INPUT, data, ap);
    va_end(ap);
    text[REMOTE_MAX_TEXT_INPUT-1]=0;

    VENet::BitStream str;
    str.Write((MessageID)ID_TRANSPORT_STRING);
    str.Write(text, (int) strlen(text));
    str.Write((unsigned char) 0);
    vePeerInterface->Send(&str, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, (systemAddress==UNASSIGNED_SYSTEM_ADDRESS)!=0);
}
void VENetTransport2::CloseConnection( SystemAddress systemAddress )
{
    vePeerInterface->CloseConnection(systemAddress, true, 0);
}
Packet* VENetTransport2::Receive( void )
{
    if (packetQueue.Size()==0)
        return 0;
    return packetQueue.Pop();
}
SystemAddress VENetTransport2::HasNewIncomingConnection(void)
{
    if (newConnections.Size())
        return newConnections.Pop();
    return UNASSIGNED_SYSTEM_ADDRESS;
}
SystemAddress VENetTransport2::HasLostConnection(void)
{
    if (lostConnections.Size())
        return lostConnections.Pop();
    return UNASSIGNED_SYSTEM_ADDRESS;
}
void VENetTransport2::DeallocatePacket( Packet *packet )
{
    veFree_Ex(packet->data, _FILE_AND_LINE_ );
    VENet::OP_DELETE(packet, _FILE_AND_LINE_ );
}
PluginReceiveResult VENetTransport2::OnReceive(Packet *packet)
{
    switch (packet->data[0])
    {
    case ID_TRANSPORT_STRING:
    {
        if (packet->length==sizeof(MessageID))
            return RR_STOP_PROCESSING_AND_DEALLOCATE;

        Packet *p = VENet::OP_NEW<Packet>(_FILE_AND_LINE_);
        *p=*packet;
        p->bitSize-=8;
        p->length--;
        p->data=(unsigned char*) rakMalloc_Ex(p->length,_FILE_AND_LINE_);
        memcpy(p->data, packet->data+1, p->length);
        packetQueue.Push(p, _FILE_AND_LINE_ );

    }
    return RR_STOP_PROCESSING_AND_DEALLOCATE;
    }
    return RR_CONTINUE_PROCESSING;
}
void VENetTransport2::OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason )
{
    (void) VENetGuid;
    (void) lostConnectionReason;
    lostConnections.Push(systemAddress, _FILE_AND_LINE_ );
}
void VENetTransport2::OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming)
{
    (void) VENetGuid;
    (void) isIncoming;
    newConnections.Push(systemAddress, _FILE_AND_LINE_ );
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
