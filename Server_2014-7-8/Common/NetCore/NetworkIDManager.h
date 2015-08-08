////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 NetworkIDManager.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VENetTypes.h"
#include "Export.h"
#include "VEMemoryOverride.h"
#include "NetworkIDObject.h"
#include "Rand.h"

namespace VENet
{

#define NETWORK_ID_MANAGER_HASH_LENGTH 1024

class VE_DLL_EXPORT NetworkIDManager
{
public:
    STATIC_FACTORY_DECLARATIONS(NetworkIDManager)

    NetworkIDManager();
    virtual ~NetworkIDManager(void);

    template <class returnType>
    returnType GET_OBJECT_FROM_ID(NetworkID x)
    {
        NetworkIDObject *nio = GET_BASE_OBJECT_FROM_ID(x);
        if (nio==0)
            return 0;
        if (nio->GetParent())
            return (returnType) nio->GetParent();
        return (returnType) nio;
    }

    NetworkIDObject *GET_BASE_OBJECT_FROM_ID(NetworkID x);

    void TrackNetworkIDObject(NetworkIDObject *networkIdObject);
    void StopTrackingNetworkIDObject(NetworkIDObject *networkIdObject);

protected:
    friend class NetworkIDObject;

    NetworkIDObject *networkIdHash[NETWORK_ID_MANAGER_HASH_LENGTH];
    unsigned int NetworkIDToHashIndex(NetworkID networkId);
    uint64_t startingOffset;
    NetworkID GetNewNetworkID(void);

};

}