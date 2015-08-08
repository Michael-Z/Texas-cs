////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 HTTPConnection.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_HTTPConnection==1 && _VENET_SUPPORT_TCPInterface==1

#pragma once

#include "Export.h"
#include "VEString.h"
#include "VEMemoryOverride.h"
#include "VENetTypes.h"
#include "DS_Queue.h"

namespace VENet
{
class TCPInterface;
struct SystemAddress;

class VE_DLL_EXPORT HTTPConnection
{
public:
    STATIC_FACTORY_DECLARATIONS(HTTPConnection)

    HTTPConnection();
    virtual ~HTTPConnection();

    void Init(TCPInterface *_tcp, const char *host, unsigned short port=80);

    void Post(const char *path, const char *data, const char *_contentType="application/x-www-form-urlencoded");

    void Get(const char *path);

    bool HasRead(void) const;

    /// \pre HasResult must return true
    VENet::VEString Read(void);

    void Update(void);

    SystemAddress GetServerAddress(void) const;

    void ProcessTCPPacket(Packet *packet);

    enum ResponseCodes { NoBody=1001, OK=200, Deleted=1002 };

    HTTPConnection& operator=(const HTTPConnection& rhs)
    {
        (void) rhs;
        return *this;
    }

    struct BadResponse
    {
public:
        BadResponse()
        {
            code=0;
        }

        BadResponse(const unsigned char *_data, int _code)
                : data((const char *)_data), code(_code) {}

        BadResponse(const char *_data, int _code)
                : data(_data), code(_code) {}

        operator int () const
        {
            return code;
        }

        VENet::VEString data;
        int code;
    };

    bool HasBadResponse(int *code, VENet::VEString *data);

    bool IsBusy(void) const;

    int GetState(void) const;

    struct OutgoingCommand
    {
        VENet::VEString remotePath;
        VENet::VEString data;
        VENet::VEString contentType;
        bool isPost;
    };

    DataStructures::Queue<OutgoingCommand> outgoingCommand;
    OutgoingCommand currentProcessingCommand;

private:
    SystemAddress server;
    TCPInterface *tcp;
    VENet::VEString host;
    unsigned short port;
    DataStructures::Queue<BadResponse> badResponses;

    enum ConnectionState
    {
        CS_NONE,
        CS_DISCONNECTING,
        CS_CONNECTING,
        CS_CONNECTED,
        CS_PROCESSING,
    } connectionState;

    VENet::VEString incomingData;
    DataStructures::Queue<VENet::VEString> results;

    void CloseConnection();

};

}

#endif