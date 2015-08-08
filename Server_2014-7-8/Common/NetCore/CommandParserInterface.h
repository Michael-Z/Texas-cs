////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 CommandParserInterface.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma

#include "VEMemoryOverride.h"
#include "VENetTypes.h"
#include "DS_OrderedList.h"
#include "Export.h"

namespace VENet
{
class TransportInterface;

struct VE_DLL_EXPORT RegisteredCommand
{
    const char *command;
    const char *commandHelp;
    unsigned char parameterCount;
};

int VE_DLL_EXPORT RegisteredCommandComp( const char* const & key, const RegisteredCommand &data );

class VE_DLL_EXPORT CommandParserInterface
{
public:
    CommandParserInterface();
    virtual ~CommandParserInterface();

    virtual const char *GetName(void) const=0;

    virtual void  OnNewIncomingConnection(const SystemAddress &systemAddress, TransportInterface *transport);

    virtual void OnConnectionLost(const SystemAddress &systemAddress, TransportInterface *transport);

    virtual void SendHelp(TransportInterface *transport, const SystemAddress &systemAddress)=0;

    virtual bool OnCommand(const char *command, unsigned numParameters, char **parameterList, TransportInterface *transport, const SystemAddress &systemAddress, const char *originalString)=0;

    virtual void OnTransportChange(TransportInterface *transport);

    virtual bool GetRegisteredCommand(const char *command, RegisteredCommand *rc);

    static void ParseConsoleString(char *str, const char delineator, unsigned char delineatorToggle, unsigned *numParameters, char **parameterList, unsigned parameterListLength);

    virtual void SendCommandList(TransportInterface *transport, const SystemAddress &systemAddress);

    static const unsigned char VARIABLE_NUMBER_OF_PARAMETERS;

    virtual void RegisterCommand(unsigned char parameterCount, const char *command, const char *commandHelp);

    virtual void ReturnResult(bool res, const char *command, TransportInterface *transport, const SystemAddress &systemAddress);
    virtual void ReturnResult(char *res, const char *command, TransportInterface *transport, const SystemAddress &systemAddress);
    virtual void ReturnResult(SystemAddress res, const char *command, TransportInterface *transport, const SystemAddress &systemAddress);
    virtual void ReturnResult(int res, const char *command,TransportInterface *transport, const SystemAddress &systemAddress);

    virtual void ReturnResult(const char *command,TransportInterface *transport, const SystemAddress &systemAddress);

protected:
    DataStructures::OrderedList<const char*, RegisteredCommand, RegisteredCommandComp> commandList;
};

}

