#include "CommandParserInterface.h"
#include "TransportInterface.h"
#include <string.h>
#include "VEAssert.h"
#include <stdio.h>


#if   defined(_WIN32)
#include <winsock2.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "LinuxStrings.h"

using namespace VENet;

#ifdef _MSC_VER
#pragma warning( push )
#endif

const unsigned char CommandParserInterface::VARIABLE_NUMBER_OF_PARAMETERS=255;

int VENet::RegisteredCommandComp( const char* const & key, const RegisteredCommand &data )
{
    return _stricmp(key,data.command);
}

CommandParserInterface::CommandParserInterface() {}
CommandParserInterface::~CommandParserInterface() {}

void CommandParserInterface::ParseConsoleString(char *str, const char delineator, unsigned char delineatorToggle, unsigned *numParameters, char **parameterList, unsigned parameterListLength)
{
    unsigned strIndex, parameterListIndex;
    unsigned strLen;
    bool replaceDelineator=true;

    strLen = (unsigned) strlen(str);

    for (strIndex=0; strIndex < strLen; strIndex++)
    {
        if (str[strIndex]==delineator && replaceDelineator)
            str[strIndex]=0;

        if (str[strIndex]=='\n' || str[strIndex]=='\r')
            str[strIndex]=0;

        if (str[strIndex]==delineatorToggle)
        {
            str[strIndex]=0;
            replaceDelineator=!replaceDelineator;
        }
    }

    for (strIndex=0, parameterListIndex=0; strIndex < strLen; )
    {
        if (str[strIndex]!=0)
        {
            parameterList[parameterListIndex]=str+strIndex;
            parameterListIndex++;
            VEAssert(parameterListIndex < parameterListLength);
            if (parameterListIndex >= parameterListLength)
                break;

            strIndex++;
            while (str[strIndex]!=0 && strIndex < strLen)
                strIndex++;
        }
        else
            strIndex++;
    }

    parameterList[parameterListIndex]=0;
    *numParameters=parameterListIndex;
}
void CommandParserInterface::SendCommandList(TransportInterface *transport, const SystemAddress &systemAddress)
{
    unsigned i;
    if (commandList.Size())
    {
        for (i=0; i < commandList.Size(); i++)
        {
            transport->Send(systemAddress, "%s", commandList[i].command);
            if (i < commandList.Size()-1)
                transport->Send(systemAddress, ", ");
        }
        transport->Send(systemAddress, "\r\n");
    }
    else
        transport->Send(systemAddress, "No registered commands\r\n");
}
void CommandParserInterface::RegisterCommand(unsigned char parameterCount, const char *command, const char *commandHelp)
{
    RegisteredCommand rc;
    rc.command=command;
    rc.commandHelp=commandHelp;
    rc.parameterCount=parameterCount;
    commandList.Insert( command, rc, true, _FILE_AND_LINE_);
}
bool CommandParserInterface::GetRegisteredCommand(const char *command, RegisteredCommand *rc)
{
    bool objectExists;
    unsigned index;
    index=commandList.GetIndexFromKey(command, &objectExists);
    if (objectExists)
        *rc=commandList[index];
    return objectExists;
}
void CommandParserInterface::OnTransportChange(TransportInterface *transport)
{
    (void) transport;
}
void CommandParserInterface::OnNewIncomingConnection(const SystemAddress &systemAddress, TransportInterface *transport)
{
    (void) systemAddress;
    (void) transport;
}
void CommandParserInterface::OnConnectionLost(const SystemAddress &systemAddress, TransportInterface *transport)
{
    (void) systemAddress;
    (void) transport;
}
void CommandParserInterface::ReturnResult(bool res, const char *command,TransportInterface *transport, const SystemAddress &systemAddress)
{
    if (res)
        transport->Send(systemAddress, "%s returned true.\r\n", command);
    else
        transport->Send(systemAddress, "%s returned false.\r\n", command);
}
void CommandParserInterface::ReturnResult(int res, const char *command,TransportInterface *transport, const SystemAddress &systemAddress)
{
    transport->Send(systemAddress, "%s returned %i.\r\n", command, res);
}
void CommandParserInterface::ReturnResult(const char *command, TransportInterface *transport, const SystemAddress &systemAddress)
{
    transport->Send(systemAddress, "Successfully called %s.\r\n", command);
}
void CommandParserInterface::ReturnResult(char *res, const char *command, TransportInterface *transport, const SystemAddress &systemAddress)
{
    transport->Send(systemAddress, "%s returned %s.\r\n", command, res);
}
void CommandParserInterface::ReturnResult(SystemAddress res, const char *command, TransportInterface *transport, const SystemAddress &systemAddress)
{
    char addr[128];
    systemAddress.ToString(false,addr);
    char addr2[128];
    res.ToString(false,addr2);
    transport->Send(systemAddress, "%s returned %s %s:%i\r\n", command,addr,addr2,res.GetPort());
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
