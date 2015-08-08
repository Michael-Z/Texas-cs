////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 VENetCommandParser.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_VENetCommandParser==1

#pragma once

#include "CommandParserInterface.h"
#include "Export.h"

namespace VENet
{
class VEPeerInterface;

class VE_DLL_EXPORT VENetCommandParser : public CommandParserInterface
{
public:
    STATIC_FACTORY_DECLARATIONS(VENetCommandParser)

    VENetCommandParser();
    ~VENetCommandParser();

    bool OnCommand(const char *command, unsigned numParameters, char **parameterList, TransportInterface *transport, const SystemAddress &systemAddress, const char *originalString);

    const char *GetName(void) const;

    void SendHelp(TransportInterface *transport, const SystemAddress &systemAddress);

    void SetRakPeerInterface(VENet::VEPeerInterface *vePeer);
protected:

    VEPeerInterface *peer;
};

}

#endif
