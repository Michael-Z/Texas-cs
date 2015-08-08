////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PacketLogger.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_PacketLogger==1

#pragma once

#include "VENetTypes.h"
#include "PluginInterface2.h"
#include "Export.h"

namespace VENet
{
class VEPeerInterface;

class VE_DLL_EXPORT PacketLogger : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(PacketLogger)

    PacketLogger();
    virtual ~PacketLogger();

    virtual void FormatLine(char* into, const char* dir, const char* type, unsigned int reliableMessageNumber, unsigned int frame
                            , unsigned char messageIdentifier, const BitSize_t bitLen, unsigned long long time, const SystemAddress& local, const SystemAddress& remote,
                            unsigned int splitPacketId, unsigned int splitPacketIndex, unsigned int splitPacketCount, unsigned int orderingIndex);
    virtual void FormatLine(char* into, const char* dir, const char* type, unsigned int reliableMessageNumber, unsigned int frame
                            , const char* idToPrint, const BitSize_t bitLen, unsigned long long time, const SystemAddress& local, const SystemAddress& remote,
                            unsigned int splitPacketId, unsigned int splitPacketIndex, unsigned int splitPacketCount, unsigned int orderingIndex);

    virtual void OnDirectSocketSend(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
    virtual void OnDirectSocketReceive(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);
    virtual void OnReliabilityLayerNotification(const char *errorMessage, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress, bool isError);
    virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, SystemAddress remoteSystemAddress, VENet::TimeMS time, int isSend);
    virtual void OnAck(unsigned int messageNumber, SystemAddress remoteSystemAddress, VENet::TimeMS time);
    virtual void OnPushBackPacket(const char *data, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress);

    virtual void LogHeader(void);

    virtual void WriteLog(const char *str);

    virtual void WriteMiscellaneous(const char *type, const char *msg);

    virtual void SetPrintID(bool print);
    virtual void SetPrintAcks(bool print);

    virtual void SetPrefix(const char *_prefix);

    virtual void SetSuffix(const char *_suffix);
    static const char* BaseIDTOString(unsigned char Id);

    void SetLogDirectMessages(bool send);
protected:

    virtual bool UsesReliabilityLayer(void) const
    {
        return true;
    }
    const char* IDTOString(unsigned char Id);
    virtual void AddToLog(const char *str);
    virtual const char* UserIDTOString(unsigned char Id);
    void GetLocalTime(char buffer[128]);
    bool logDirectMessages;

    bool printId, printAcks;
    char prefix[256];
    char suffix[256];
};

}

#endif