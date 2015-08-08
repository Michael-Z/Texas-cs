#include "PluginInterface2.h"
#include "PacketizedTCP.h"
#include "VEPeerInterface.h"
#include "BitStream.h"

using namespace VENet;

PluginInterface2::PluginInterface2()
{
    vePeerInterface=0;
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    packetizedTCP=0;
#endif
}
PluginInterface2::~PluginInterface2()
{

}
void PluginInterface2::SendUnified( const VENet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast )
{
    if (vePeerInterface)
    {
        vePeerInterface->Send(bitStream,priority,reliability,orderingChannel,systemIdentifier,broadcast);
        return;
    }
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    else if (packetizedTCP)
    {
        packetizedTCP->Send((const char*) bitStream->GetData(), bitStream->GetNumberOfBytesUsed(), systemIdentifier.systemAddress, broadcast);
        return;
    }
#endif

    if (broadcast==false && systemIdentifier.veNetGuid==GetMyGUIDUnified())
    {
        Packet packet;
        packet.bitSize=bitStream->GetNumberOfBitsUsed();
        packet.data=bitStream->GetData();
        packet.deleteData=false;
        packet.guid=UNASSIGNED_VENET_GUID;
        packet.length=bitStream->GetNumberOfBytesUsed();
        packet.systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
        packet.wasGeneratedLocally=false;
        OnReceive(&packet);
        Update();
    }
}
void PluginInterface2::SendUnified( const char * data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast )
{
    if (vePeerInterface)
    {
        vePeerInterface->Send(data, length, priority,reliability,orderingChannel,systemIdentifier,broadcast);
        return;
    }
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    else if (packetizedTCP)
    {
        packetizedTCP->Send(data, length, systemIdentifier.systemAddress, broadcast);
        return;
    }
#endif

    if (broadcast==false && systemIdentifier.veNetGuid==GetMyGUIDUnified())
    {
        Packet packet;
        packet.bitSize=BYTES_TO_BITS(length);
        packet.data=(unsigned char*) data;
        packet.deleteData=false;
        packet.guid=UNASSIGNED_VENET_GUID;
        packet.length=length;
        packet.systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
        packet.wasGeneratedLocally=false;
        OnReceive(&packet);
        Update();
    }
}
Packet *PluginInterface2::AllocatePacketUnified(unsigned dataSize)
{
    if (vePeerInterface)
    {
        return vePeerInterface->AllocatePacket(dataSize);
    }
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    else if (packetizedTCP)
    {
        return packetizedTCP->AllocatePacket(dataSize);
    }
#endif

    Packet *packet = VENet::OP_NEW<Packet>(_FILE_AND_LINE_);
    packet->data = (unsigned char*) rakMalloc_Ex(dataSize, _FILE_AND_LINE_);
    packet->bitSize=BYTES_TO_BITS(dataSize);
    packet->deleteData=true;
    packet->guid=UNASSIGNED_VENET_GUID;
    packet->systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
    packet->wasGeneratedLocally=false;
    return packet;
}
void PluginInterface2::PushBackPacketUnified(Packet *packet, bool pushAtHead)
{
    if (vePeerInterface)
    {
        vePeerInterface->PushBackPacket(packet,pushAtHead);
        return;
    }
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    else if (packetizedTCP)
    {
        packetizedTCP->PushBackPacket(packet,pushAtHead);
        return;
    }
#endif

    OnReceive(packet);
    Update();
}
void PluginInterface2::DeallocPacketUnified(Packet *packet)
{
    if (vePeerInterface)
    {
        vePeerInterface->DeallocatePacket(packet);
        return;
    }
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    else if (packetizedTCP)
    {
        packetizedTCP->DeallocatePacket(packet);
        return;
    }
#endif

    veFree_Ex(packet->data, _FILE_AND_LINE_);
    VENet::OP_DELETE(packet, _FILE_AND_LINE_);
}
bool PluginInterface2::SendListUnified( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast )
{
    if (vePeerInterface)
    {
        return vePeerInterface->SendList(data,lengths,numParameters,priority,reliability,orderingChannel,systemIdentifier,broadcast)!=0;
    }
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
    else if (packetizedTCP)
    {
        return packetizedTCP->SendList(data,lengths,numParameters,systemIdentifier.systemAddress,broadcast );
    }
#endif

    if (broadcast==false && systemIdentifier.veNetGuid==GetMyGUIDUnified())
    {

        unsigned int totalLength=0;
        unsigned int lengthOffset;
        int i;
        for (i=0; i < numParameters; i++)
        {
            if (lengths[i]>0)
                totalLength+=lengths[i];
        }
        if (totalLength==0)
            return false;

        char *dataAggregate;
        dataAggregate = (char*) rakMalloc_Ex( (size_t) totalLength, _FILE_AND_LINE_ );
        if (dataAggregate==0)
        {
            notifyOutOfMemory(_FILE_AND_LINE_);
            return false;
        }
        for (i=0, lengthOffset=0; i < numParameters; i++)
        {
            if (lengths[i]>0)
            {
                memcpy(dataAggregate+lengthOffset, data[i], lengths[i]);
                lengthOffset+=lengths[i];
            }
        }

        SendUnified(dataAggregate, totalLength, priority, reliability,orderingChannel, systemIdentifier, broadcast);
        veFree_Ex(dataAggregate, _FILE_AND_LINE_);
        return true;
    }

    return false;
}
void PluginInterface2::SetRakPeerInterface( VEPeerInterface *ptr )
{
    vePeerInterface=ptr;
}
#if _VENET_SUPPORT_PacketizedTCP==1 && _VENET_SUPPORT_TCPInterface==1
void PluginInterface2::SetPacketizedTCP( PacketizedTCP *ptr )
{
    packetizedTCP=ptr;
}
#endif
VENetGUID PluginInterface2::GetMyGUIDUnified(void) const
{
    if (vePeerInterface)
        return vePeerInterface->GetMyGUID();
    return UNASSIGNED_VENET_GUID;
}