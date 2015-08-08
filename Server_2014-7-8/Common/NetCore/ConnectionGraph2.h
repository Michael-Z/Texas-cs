////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DataCompressor.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_ConnectionGraph2==1

#pragma once

#include "VEMemoryOverride.h"
#include "VENetTypes.h"
#include "PluginInterface2.h"
#include "DS_List.h"
#include "DS_WeightedGraph.h"
#include "GetTime.h"
#include "Export.h"

namespace VENet
{
class VEPeerInterface;

class VE_DLL_EXPORT ConnectionGraph2 : public PluginInterface2
{
public:

	STATIC_FACTORY_DECLARATIONS(ConnectionGraph2)

	ConnectionGraph2();
	~ConnectionGraph2();

	bool GetConnectionListForRemoteSystem(VENetGUID remoteSystemGuid, SystemAddress *saOut, VENetGUID *guidOut, unsigned int *outLength);

	bool ConnectionExists(VENetGUID g1, VENetGUID g2);

	uint16_t GetPingBetweenSystems(VENetGUID g1, VENetGUID g2) const;

	VENetGUID GetLowestAveragePingSystem(void) const;

	void SetAutoProcessNewConnections(bool b);

	bool GetAutoProcessNewConnections(void) const;

	void AddParticipant(const SystemAddress &systemAddress, VENetGUID VENetGuid);

	void GetParticipantList(DataStructures::OrderedList<VENetGUID, VENetGUID> &participantList);

	struct SystemAddressAndGuid
	{
		SystemAddress systemAddress;
		VENetGUID guid;
		uint16_t sendersPingToThatSystem;
	};
	static int SystemAddressAndGuidComp( const SystemAddressAndGuid &key, const SystemAddressAndGuid &data );

	struct RemoteSystem
	{
		DataStructures::OrderedList<SystemAddressAndGuid,SystemAddressAndGuid,ConnectionGraph2::SystemAddressAndGuidComp> remoteConnections;
		VENetGUID guid;
	};
	static int RemoteSystemComp( const VENetGUID &key, RemoteSystem * const &data );
	
protected:
	virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
	virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);
	virtual PluginReceiveResult OnReceive(Packet *packet);
	DataStructures::OrderedList<VENetGUID, RemoteSystem*, ConnectionGraph2::RemoteSystemComp> remoteSystems;

	bool autoProcessNewConnections;

};

}

#endif
