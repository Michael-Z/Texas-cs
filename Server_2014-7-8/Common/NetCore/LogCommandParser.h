////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 LogCommandParser.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_LogCommandParser==1

#pragma once

#include "CommandParserInterface.h"
#include "Export.h"

namespace VENet
{
class VEPeerInterface;

class VE_DLL_EXPORT LogCommandParser : public CommandParserInterface
{
public:
    STATIC_FACTORY_DECLARATIONS(LogCommandParser)

    LogCommandParser();
    ~LogCommandParser();

    bool OnCommand(const char *command, unsigned numParameters, char **parameterList, TransportInterface *transport, const SystemAddress &systemAddress, const char *originalString);

    const char *GetName(void) const;

    void SendHelp(TransportInterface *transport, const SystemAddress &systemAddress);

    void AddChannel(const char *channelName);

    void WriteLog(const char *channelName, const char *format, ...);

    void OnNewIncomingConnection(const SystemAddress &systemAddress, TransportInterface *transport);

    void OnConnectionLost(const SystemAddress &systemAddress, TransportInterface *transport);

    void OnTransportChange(TransportInterface *transport);
protected:
    void PrintChannels(const SystemAddress &systemAddress, TransportInterface *transport) const;

    unsigned Unsubscribe(const SystemAddress &systemAddress, const char *channelName);

    unsigned Subscribe(const SystemAddress &systemAddress, const char *channelName);

    unsigned GetChannelIndexFromName(const char *channelName);

    struct SystemAddressAndChannel
    {
        SystemAddress systemAddress;

        unsigned channels;
    };

    DataStructures::List<SystemAddressAndChannel> remoteUsers;

    const char *channelNames[32];

    TransportInterface *trans;
};

}

#endif

