#pragma once

#include "VENetTypes.h"
#include "Export.h"
#include "VEMemoryOverride.h"

#define REMOTE_MAX_TEXT_INPUT 2048

namespace VENet
{

class CommandParserInterface;


class VE_DLL_EXPORT TransportInterface
{
public:
    TransportInterface() {}
    virtual ~TransportInterface() {}

    virtual bool Start(unsigned short port, bool serverMode)=0;

    virtual void Stop(void)=0;

    virtual void Send( SystemAddress systemAddress, const char *data, ... )=0;

    virtual void CloseConnection( SystemAddress systemAddress )=0;

    virtual Packet* Receive( void )=0;

    virtual void DeallocatePacket( Packet *packet )=0;

    virtual SystemAddress HasNewIncomingConnection(void)=0;

    virtual SystemAddress HasLostConnection(void)=0;

    virtual CommandParserInterface* GetCommandParser(void)=0;
protected:
};

}
