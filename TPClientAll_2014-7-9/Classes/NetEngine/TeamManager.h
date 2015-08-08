#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TeamManager==1

#pragma once

#include "PluginInterface2.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"
#include "DS_List.h"
#include "VENetTypes.h"
#include "DS_Hash.h"
#include "DS_OrderedList.h"

namespace VENet
{
class VEPeerInterface;

typedef unsigned char NoTeamId;

typedef uint8_t WorldId;

typedef uint16_t TeamMemberLimit;

#define ALLOW_JOIN_ANY_AVAILABLE_TEAM (1<<0)
#define ALLOW_JOIN_SPECIFIC_TEAM (1<<1)
#define ALLOW_JOIN_REBALANCING (1<<2)

typedef uint8_t JoinPermissions;

class TM_Team;
class TM_TeamMember;
class TM_World;
class TeamManager;

enum JoinTeamType
{
    JOIN_ANY_AVAILABLE_TEAM,
    JOIN_SPECIFIC_TEAM,
    JOIN_NO_TEAM
};

enum TMTopology
{
    TM_PEER_TO_PEER,

    TM_CLIENT_SERVER,
};

struct TeamSelection
{
    TeamSelection();
    TeamSelection(JoinTeamType itt);
    TeamSelection(JoinTeamType itt, TM_Team *param);
    TeamSelection(JoinTeamType itt, NoTeamId param);
    JoinTeamType joinTeamType;

    union
    {
        TM_Team *specificTeamToJoin;
        NoTeamId noTeamSubcategory;
    } teamParameter;

    static TeamSelection AnyAvailable(void);
    static TeamSelection SpecificTeam(TM_Team *specificTeamToJoin);
    static TeamSelection NoTeam(NoTeamId noTeamSubcategory);
};

class VE_DLL_EXPORT TM_TeamMember
{
public:
    STATIC_FACTORY_DECLARATIONS(TM_TeamMember)

    TM_TeamMember();
    virtual ~TM_TeamMember();

    bool RequestTeam(TeamSelection teamSelection);

    bool RequestTeamSwitch(TM_Team *teamToJoin, TM_Team *teamToLeave);

    TeamSelection GetRequestedTeam(void) const;

    void GetRequestedSpecificTeams(DataStructures::List<TM_Team*> &requestedTeams) const;

    bool HasRequestedTeam(TM_Team *team) const;

    unsigned int GetRequestedTeamIndex(TM_Team *team) const;

    unsigned int GetRequestedTeamCount(void) const;

    bool CancelTeamRequest(TM_Team *specificTeamToCancel);

    bool LeaveTeam(TM_Team* team, NoTeamId _noTeamSubcategory);

    bool LeaveAllTeams(NoTeamId noTeamSubcategory);

    TM_Team* GetCurrentTeam(void) const;

    unsigned int GetCurrentTeamCount(void) const;

    TM_Team* GetCurrentTeamByIndex(unsigned int index);

    void GetCurrentTeams(DataStructures::List<TM_Team*> &_teams) const;

    void GetLastTeams(DataStructures::List<TM_Team*> &_teams) const;

    bool IsOnTeam(TM_Team *team) const;

    NetworkID GetNetworkID(void) const;

    TM_World* GetTM_World(void) const;

    void SerializeConstruction(BitStream *constructionBitstream);

    bool DeserializeConstruction(TeamManager *teamManager, BitStream *constructionBitstream);

    void SetOwner(void *o);

    void *GetOwner(void) const;

    NoTeamId GetNoTeamId(void) const;

    unsigned int GetWorldIndex(void) const;

    static unsigned long ToUint32( const NetworkID &g );

    struct RequestedTeam
    {
        VENet::Time whenRequested;
        unsigned int requestIndex;
        TM_Team *requested;
        bool isTeamSwitch;
        TM_Team *teamToLeave;
    };

protected:
    NetworkID networkId;
    TM_World* world;
    DataStructures::List<TM_Team*> teams;
    NoTeamId noTeamSubcategory;
    DataStructures::List<RequestedTeam> teamsRequested;
    JoinTeamType joinTeamType;
    DataStructures::List<TM_Team*> lastTeams;
    VENet::Time whenJoinAnyRequested;
    unsigned int joinAnyRequestIndex;
    void *owner;

    void UpdateListsToNoTeam(NoTeamId nti);
    bool JoinAnyTeamCheck(void) const;
    bool JoinSpecificTeamCheck(TM_Team *specificTeamToJoin, bool ignoreRequested) const;
    bool SwitchSpecificTeamCheck(TM_Team *teamToJoin, TM_Team *teamToLeave, bool ignoreRequested) const;
    bool LeaveTeamCheck(TM_Team *team) const;
    void UpdateTeamsRequestedToAny(void);
    void UpdateTeamsRequestedToNone(void);
    void AddToRequestedTeams(TM_Team *teamToJoin);
    void AddToRequestedTeams(TM_Team *teamToJoin, TM_Team *teamToLeave);
    bool RemoveFromRequestedTeams(TM_Team *team);
    void AddToTeamList(TM_Team *team);
    void RemoveFromSpecificTeamInternal(TM_Team *team);
    void RemoveFromAllTeamsInternal(void);
    void StoreLastTeams(void);

    friend class TM_World;
    friend class TM_Team;
    friend class TeamManager;
};

class VE_DLL_EXPORT TM_Team
{
public:
    STATIC_FACTORY_DECLARATIONS(TM_Team)

    TM_Team();
    virtual ~TM_Team();

    bool SetMemberLimit(TeamMemberLimit _teamMemberLimit, NoTeamId noTeamSubcategory);

    TeamMemberLimit GetMemberLimit(void) const;

    TeamMemberLimit GetMemberLimitSetting(void) const;

    bool SetJoinPermissions(JoinPermissions _joinPermissions);

    JoinPermissions GetJoinPermissions(void) const;

    void LeaveTeam(TM_TeamMember* teamMember, NoTeamId noTeamSubcategory);

    bool GetBalancingApplies(void) const;

    void GetTeamMembers(DataStructures::List<TM_TeamMember*> &_teamMembers) const;

    unsigned int GetTeamMembersCount(void) const;

    TM_TeamMember *GetTeamMemberByIndex(unsigned int index) const;

    NetworkID GetNetworkID(void) const;

    TM_World* GetTM_World(void) const;

    void SerializeConstruction(BitStream *constructionBitstream);

    bool DeserializeConstruction(TeamManager *teamManager, BitStream *constructionBitstream);

    void SetOwner(void *o);

    void *GetOwner(void) const;

    unsigned int GetWorldIndex(void) const;

    static unsigned long ToUint32( const NetworkID &g );

protected:
    NetworkID ID;
    TM_World* world;
    DataStructures::List<TM_TeamMember*> teamMembers;
    JoinPermissions joinPermissions;
    bool balancingApplies;
    TeamMemberLimit teamMemberLimit;
    void *owner;

    void RemoveFromTeamMemberList(TM_TeamMember *teamMember);

    unsigned int GetMemberWithRequestedSingleTeamSwitch(TM_Team *team);


    friend class TM_World;
    friend class TM_TeamMember;
    friend class TeamManager;
};

class TM_World
{
public:
    TM_World();
    virtual ~TM_World();

    TeamManager *GetTeamManager(void) const;

    void AddParticipant(VENetGUID VENetGuid);

    void RemoveParticipant(VENetGUID VENetGuid);

    void SetAutoManageConnections(bool autoAdd);

    void GetParticipantList(DataStructures::List<VENetGUID> &participantList);

    void ReferenceTeam(TM_Team *team, NetworkID networkId, bool applyBalancing);

    void DereferenceTeam(TM_Team *team, NoTeamId noTeamSubcategory);

    unsigned int GetTeamCount(void) const;

    TM_Team *GetTeamByIndex(unsigned int index) const;

    TM_Team *GetTeamByNetworkID(NetworkID teamId);

    unsigned int GetTeamIndex(const TM_Team *team) const;

    void ReferenceTeamMember(TM_TeamMember *teamMember, NetworkID networkId);

    void DereferenceTeamMember(TM_TeamMember *teamMember);

    unsigned int GetTeamMemberCount(void) const;

    TM_TeamMember *GetTeamMemberByIndex(unsigned int index) const;

    NetworkID GetTeamMemberIDByIndex(unsigned int index) const;

    TM_TeamMember *GetTeamMemberByNetworkID(NetworkID teamMemberId);

    unsigned int GetTeamMemberIndex(const TM_TeamMember *teamMember) const;

    bool SetBalanceTeams(bool balanceTeams, NoTeamId noTeamSubcategory);

    bool GetBalanceTeams(void) const;

    void SetHost(VENetGUID _hostGuid);

    VENetGUID GetHost(void) const;

    WorldId GetWorldId(void) const;

    void Clear(void);

    struct JoinRequestHelper
    {
        VENet::Time whenRequestMade;
        unsigned int teamMemberIndex;
        unsigned int indexIntoTeamsRequested;
        unsigned int requestIndex;
    };
    static int JoinRequestHelperComp(const TM_World::JoinRequestHelper &key, const TM_World::JoinRequestHelper &data);

protected:
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);

    void EnforceTeamBalance(NoTeamId noTeamSubcategory);
    void KickExcessMembers(NoTeamId noTeamSubcategory);
    void FillRequestedSlots(void);
    unsigned int GetAvailableTeamIndexWithFewestMembers(TeamMemberLimit secondaryLimit, JoinPermissions joinPermissions);

    void GetSortedJoinRequests(DataStructures::OrderedList<JoinRequestHelper, JoinRequestHelper, JoinRequestHelperComp> &joinRequests);


    void BroadcastToParticipants(VENet::BitStream *bsOut, VENetGUID exclusionGuid);
    void BroadcastToParticipants(unsigned char *data, const int length, VENetGUID exclusionGuid);

    TM_Team* JoinAnyTeam(TM_TeamMember *teamMember, int *resultCode);

    int JoinSpecificTeam(TM_TeamMember *teamMember, TM_Team *team, bool isTeamSwitch, TM_Team *teamToLeave, DataStructures::List<TM_Team*> &teamsWeAreLeaving);

    TeamMemberLimit GetBalancedTeamLimit(void) const;

    DataStructures::Hash<NetworkID, TM_Team*, 256, TM_Team::ToUint32> teamsHash;
    DataStructures::Hash<NetworkID, TM_TeamMember*, 256, TM_TeamMember::ToUint32> teamMembersHash;

    TeamManager *teamManager;
    DataStructures::List<VENetGUID> participants;
    DataStructures::List<TM_Team*> teams;
    DataStructures::List<TM_TeamMember*> teamMembers;
    bool balanceTeamsIsActive;
    VENetGUID hostGuid;
    WorldId worldId;
    bool autoAddParticipants;
    int teamRequestIndex;

    friend class TeamManager;
    friend class TM_TeamMember;
    friend class TM_Team;
};

class VE_DLL_EXPORT TeamManager : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(TeamManager)

    TeamManager();
    virtual ~TeamManager();

    TM_World* AddWorld(WorldId worldId);

    void RemoveWorld(WorldId worldId);

    unsigned int GetWorldCount(void) const;

    TM_World* GetWorldAtIndex(unsigned int index) const;

    TM_World* GetWorldWithId(WorldId worldId) const;

    void SetAutoManageConnections(bool autoAdd);

    void SetTopology(TMTopology _topology);

    void DecomposeTeamFull(Packet *packet,
                           TM_World **world, TM_TeamMember **teamMember, TM_Team **team,
                           uint16_t &currentMembers, uint16_t &memberLimitIncludingBalancing, bool &balancingIsActive, JoinPermissions &joinPermissions);

    void DecomposeTeamLocked(Packet *packet,
                             TM_World **world, TM_TeamMember **teamMember, TM_Team **team,
                             uint16_t &currentMembers, uint16_t &memberLimitIncludingBalancing, bool &balancingIsActive, JoinPermissions &joinPermissions);

    void Clear(void);

    void DecodeTeamAssigned(Packet *packet, TM_World **world, TM_TeamMember **teamMember);

    void DecodeTeamCancelled(Packet *packet, TM_World **world, TM_TeamMember **teamMember, TM_Team **teamCancelled);

protected:

    virtual void Update(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );
    virtual void OnNewConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, bool isIncoming);
    void Send( const VENet::BitStream * bitStream, const AddressOrGUID systemIdentifier, bool broadcast );

    void EncodeTeamFullOrLocked(VENet::BitStream *bitStream, TM_TeamMember *teamMember, TM_Team *team);
    void DecomposeTeamFullOrLocked(VENet::BitStream *bsIn, TM_World **world, TM_TeamMember **teamMember, TM_Team **team,
                                   uint16_t &currentMembers, uint16_t &memberLimitIncludingBalancing, bool &balancingIsActive, JoinPermissions &joinPermissions);
    void ProcessTeamAssigned(VENet::BitStream *bsIn);

    void EncodeTeamAssigned(VENet::BitStream *bitStream, TM_TeamMember *teamMember);
    void RemoveFromTeamsRequestedAndAddTeam(TM_TeamMember *teamMember, TM_Team *team, bool isTeamSwitch, TM_Team *teamToLeave);

    void PushTeamAssigned(TM_TeamMember *teamMember);
    void PushBitStream(VENet::BitStream *bitStream);
    void OnUpdateListsToNoTeam(Packet *packet, TM_World *world);
    void OnUpdateTeamsRequestedToAny(Packet *packet, TM_World *world);
    void OnJoinAnyTeam(Packet *packet, TM_World *world);
    void OnJoinRequestedTeam(Packet *packet, TM_World *world);
    void OnUpdateTeamsRequestedToNoneAndAddTeam(Packet *packet, TM_World *world);
    void OnRemoveFromTeamsRequestedAndAddTeam(Packet *packet, TM_World *world);
    void OnAddToRequestedTeams(Packet *packet, TM_World *world);
    bool OnRemoveFromRequestedTeams(Packet *packet, TM_World *world);
    void OnLeaveTeam(Packet *packet, TM_World *world);
    void OnSetMemberLimit(Packet *packet, TM_World *world);
    void OnSetJoinPermissions(Packet *packet, TM_World *world);
    void OnSetBalanceTeams(Packet *packet, TM_World *world);
    void OnSetBalanceTeamsInitial(Packet *packet, TM_World *world);


    void EncodeTeamFull(VENet::BitStream *bitStream, TM_TeamMember *teamMember, TM_Team *team);
    void EncodeTeamLocked(VENet::BitStream *bitStream, TM_TeamMember *teamMember, TM_Team *team);

    void DecodeTeamAssigned(VENet::BitStream *bsIn, TM_World **world, TM_TeamMember **teamMember, NoTeamId &noTeamSubcategory,
                            JoinTeamType &joinTeamType, DataStructures::List<TM_Team *> &newTeam,
                            DataStructures::List<TM_Team *> &teamsLeft, DataStructures::List<TM_Team *> &teamsJoined);

    TM_World *worldsArray[255];
    DataStructures::List<TM_World*> worldsList;
    bool autoAddParticipants;
    TMTopology topology;

    friend class TM_TeamMember;
    friend class TM_World;
    friend class TM_Team;
};

}

#endif
