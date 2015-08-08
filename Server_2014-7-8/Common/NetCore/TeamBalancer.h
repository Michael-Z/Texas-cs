#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TeamBalancer==1

#pragma once

#include "PluginInterface2.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"
#include "DS_List.h"
#include "VEString.h"

namespace VENet
{
class VEPeerInterface;

typedef unsigned char TeamId;

#define UNASSIGNED_TEAM_ID 255

class VE_DLL_EXPORT TeamBalancer : public PluginInterface2
{
public:
	STATIC_FACTORY_DECLARATIONS(TeamBalancer)

	TeamBalancer();
	virtual ~TeamBalancer();

	void SetTeamSizeLimit(TeamId team, unsigned short limit);

	enum DefaultAssigmentAlgorithm
	{
		SMALLEST_TEAM,
		FILL_IN_ORDER
	};
	void SetDefaultAssignmentAlgorithm(DefaultAssigmentAlgorithm daa);

	void SetForceEvenTeams(bool force);

	void SetLockTeams(bool lock);

	void RequestSpecificTeam(NetworkID memberId, TeamId desiredTeam);

	void CancelRequestSpecificTeam(NetworkID memberId);

	void RequestAnyTeam(NetworkID memberId);

	TeamId GetMyTeam(NetworkID memberId) const;

	void DeleteMember(NetworkID memberId);

	struct TeamMember
	{
		VENetGUID memberGuid;
		NetworkID memberId;
		TeamId currentTeam;
		TeamId requestedTeam;
	};
	struct MyTeamMembers
	{
		NetworkID memberId;
		TeamId currentTeam;
		TeamId requestedTeam;
	};

protected:

	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
	void OnAttach(void);

	void OnStatusUpdateToNewHost(Packet *packet);
	void OnCancelTeamRequest(Packet *packet);
	void OnRequestAnyTeam(Packet *packet);
	void OnRequestSpecificTeam(Packet *packet);

	VENetGUID hostGuid;
	DefaultAssigmentAlgorithm defaultAssigmentAlgorithm;
	bool forceTeamsToBeEven;
	bool lockTeams;
	DataStructures::List<MyTeamMembers> myTeamMembers;

	DataStructures::List<unsigned short> teamLimits;
	DataStructures::List<unsigned short> teamMemberCounts;
	DataStructures::List<TeamMember> teamMembers;
	unsigned int GetMemberIndex(NetworkID memberId, VENetGUID guid) const;
	unsigned int AddTeamMember(const TeamMember &tm);
	void RemoveTeamMember(unsigned int index);
	void EvenTeams(void);
	unsigned int GetMemberIndexToSwitchTeams(const DataStructures::List<TeamId> &sourceTeamNumbers, TeamId targetTeamNumber);
	void GetOverpopulatedTeams(DataStructures::List<TeamId> &overpopulatedTeams, int maxTeamSize);
	void SwitchMemberTeam(unsigned int teamMemberIndex, TeamId destinationTeam);
	void NotifyTeamAssigment(unsigned int teamMemberIndex);
	bool WeAreHost(void) const;
	PluginReceiveResult OnTeamAssigned(Packet *packet);
	PluginReceiveResult OnRequestedTeamChangePending(Packet *packet);
	PluginReceiveResult OnTeamsLocked(Packet *packet);
	void GetMinMaxTeamMembers(int &minMembersOnASingleTeam, int &maxMembersOnASingleTeam);
	TeamId GetNextDefaultTeam(void);
	bool TeamWouldBeOverpopulatedOnAddition(TeamId teamId, unsigned int teamMemberSize);
	bool TeamWouldBeUnderpopulatedOnLeave(TeamId teamId, unsigned int teamMemberSize);
	TeamId GetSmallestNonFullTeam(void) const;
	TeamId GetFirstNonFullTeam(void) const;
	void MoveMemberThatWantsToJoinTeam(TeamId teamId);
	TeamId MoveMemberThatWantsToJoinTeamInternal(TeamId teamId);
	void NotifyTeamsLocked(VENetGUID target, TeamId requestedTeam);
	void NotifyTeamSwitchPending(VENetGUID target, TeamId requestedTeam, NetworkID memberId);
	void NotifyNoTeam(NetworkID memberId, VENetGUID target);
	void SwapTeamMembersByRequest(unsigned int memberIndex1, unsigned int memberIndex2);
	void RemoveByGuid(VENetGUID VENetGuid);
	bool TeamsWouldBeEvenOnSwitch(TeamId t1, TeamId t2);

};

}

#endif

