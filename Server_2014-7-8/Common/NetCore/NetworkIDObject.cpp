#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "VEAssert.h"
#include "VEAlloca.h"

using namespace VENet;

NetworkIDObject::NetworkIDObject()
{
    networkID=UNASSIGNED_NETWORK_ID;
    parent=0;
    networkIDManager=0;
    nextInstanceForNetworkIDManager=0;
}
NetworkIDObject::~NetworkIDObject()
{
    if (networkID!=UNASSIGNED_NETWORK_ID && networkIDManager)
        networkIDManager->StopTrackingNetworkIDObject(this);
}
void NetworkIDObject::SetNetworkIDManager( NetworkIDManager *manager)
{
    networkIDManager=manager;
}
NetworkIDManager * NetworkIDObject::GetNetworkIDManager( void )
{
    return networkIDManager;
}
NetworkID NetworkIDObject::GetNetworkID( void )
{
    if (networkID==UNASSIGNED_NETWORK_ID)
    {
        VEAssert(networkIDManager);
        networkID = networkIDManager->GetNewNetworkID();
        networkIDManager->TrackNetworkIDObject(this);
    }
    return networkID;
}
void NetworkIDObject::SetNetworkID( NetworkID id )
{
    if (id==networkID)
        return;

    if (networkID!=UNASSIGNED_NETWORK_ID)
        networkIDManager->StopTrackingNetworkIDObject(this);
    networkID = id;
    networkIDManager->TrackNetworkIDObject(this);
}
void NetworkIDObject::SetParent( void *_parent )
{
    parent=_parent;
}
void* NetworkIDObject::GetParent( void ) const
{
    return parent;
}
