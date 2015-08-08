////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 NetworkIDObject.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#if !defined(__NETWORK_ID_GENERATOR)
#define      __NETWORK_ID_GENERATOR

#include "VENetTypes.h"
#include "VEMemoryOverride.h"
#include "Export.h"

namespace VENet
{
class NetworkIDManager;

typedef uint32_t NetworkIDType;

class VE_DLL_EXPORT NetworkIDObject
{
public:
    NetworkIDObject();

    virtual ~NetworkIDObject();

    virtual void SetNetworkIDManager( NetworkIDManager *manager);

    virtual NetworkIDManager * GetNetworkIDManager( void );

    virtual NetworkID GetNetworkID( void );

    virtual void SetNetworkID( NetworkID id );

    virtual void SetParent( void *_parent );

    virtual void* GetParent( void ) const;

protected:
    NetworkID networkID;

    NetworkIDManager *networkIDManager;

    void *parent;

    friend class NetworkIDManager;
    NetworkIDObject *nextInstanceForNetworkIDManager;
};

}

#endif
