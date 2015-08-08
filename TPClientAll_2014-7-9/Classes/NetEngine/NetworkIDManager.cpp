#include "NetworkIDManager.h"
#include "NetworkIDObject.h"
#include "VEAssert.h"
#include "GetTime.h"
#include "VESleep.h"
#include "SuperFastHash.h"
#include "VEPeerInterface.h"

using namespace VENet;

STATIC_FACTORY_DEFINITIONS(NetworkIDManager,NetworkIDManager)

NetworkIDManager::NetworkIDManager()
{
    startingOffset = VEPeerInterface::Get64BitUniqueRandomNumber();
    memset(networkIdHash,0,sizeof(networkIdHash));
}
NetworkIDManager::~NetworkIDManager(void)
{

}
NetworkIDObject *NetworkIDManager::GET_BASE_OBJECT_FROM_ID(NetworkID x)
{
    unsigned int hashIndex=NetworkIDToHashIndex(x);
    NetworkIDObject *nio=networkIdHash[hashIndex];
    while (nio)
    {
        if (nio->GetNetworkID()==x)
            return nio;
        nio=nio->nextInstanceForNetworkIDManager;
    }
    return 0;
}
NetworkID NetworkIDManager::GetNewNetworkID(void)
{
    while (GET_BASE_OBJECT_FROM_ID(++startingOffset))
        ;
    return startingOffset;
}
unsigned int NetworkIDManager::NetworkIDToHashIndex(NetworkID networkId)
{
    return (unsigned int) (networkId % NETWORK_ID_MANAGER_HASH_LENGTH);
}
void NetworkIDManager::TrackNetworkIDObject(NetworkIDObject *networkIdObject)
{
    VEAssert(networkIdObject->GetNetworkID()!=UNASSIGNED_NETWORK_ID);
    unsigned int hashIndex=NetworkIDToHashIndex(networkIdObject->GetNetworkID());
    if (networkIdHash[hashIndex]==0)
    {
        networkIdHash[hashIndex]=networkIdObject;
        return;
    }
    NetworkIDObject *nio=networkIdHash[hashIndex];
    VEAssert(nio!=networkIdObject);
    VEAssert(nio->GetNetworkID()!=networkIdObject->GetNetworkID());

    while (nio->nextInstanceForNetworkIDManager!=0)
    {
        nio=nio->nextInstanceForNetworkIDManager;

        VEAssert(nio!=networkIdObject);
        VEAssert(nio->GetNetworkID()!=networkIdObject->GetNetworkID());
    }

    networkIdObject->nextInstanceForNetworkIDManager=0;
    nio->nextInstanceForNetworkIDManager=networkIdObject;
}
void NetworkIDManager::StopTrackingNetworkIDObject(NetworkIDObject *networkIdObject)
{
    VEAssert(networkIdObject->GetNetworkID()!=UNASSIGNED_NETWORK_ID);
    unsigned int hashIndex=NetworkIDToHashIndex(networkIdObject->GetNetworkID());
    NetworkIDObject *nio=networkIdHash[hashIndex];
    if (nio==0)
    {
        VEAssert("NetworkIDManager::StopTrackingNetworkIDObject didn't find object" && 0);
        return;
    }
    if (nio==networkIdObject)
    {
        networkIdHash[hashIndex]=nio->nextInstanceForNetworkIDManager;
        return;
    }

    while (nio)
    {
        if (nio->nextInstanceForNetworkIDManager==networkIdObject)
        {
            nio->nextInstanceForNetworkIDManager=networkIdObject->nextInstanceForNetworkIDManager;
            return;
        }
        nio=nio->nextInstanceForNetworkIDManager;
    }

    VEAssert("NetworkIDManager::StopTrackingNetworkIDObject didn't find object" && 0);
}
