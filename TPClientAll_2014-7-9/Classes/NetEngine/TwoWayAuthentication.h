#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TwoWayAuthentication==1

#pragma once

#define NONCE_TIMEOUT_MS 10000
#define CHALLENGE_MINIMUM_TIMEOUT 3000

#if LIBCAT_SECURITY==1
#define TWO_WAY_AUTHENTICATION_NONCE_LENGTH 32
#define HASHED_NONCE_AND_PW_LENGTH 32
#else
#include "SHA1.h"
#define TWO_WAY_AUTHENTICATION_NONCE_LENGTH 20
#define HASHED_NONCE_AND_PW_LENGTH SHA1_LENGTH
#endif

#include "PluginInterface2.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"
#include "VEString.h"
#include "DS_Hash.h"
#include "DS_Queue.h"

typedef int64_t FCM2Guid;

namespace VENet
{
/// Forward declarations
class VEPeerInterface;

class VE_DLL_EXPORT TwoWayAuthentication : public PluginInterface2
{
public:
    STATIC_FACTORY_DECLARATIONS(TwoWayAuthentication)

    TwoWayAuthentication();
    virtual ~TwoWayAuthentication();

    bool AddPassword(VENet::VEString identifier, VENet::VEString password);

    bool Challenge(VENet::VEString identifier, AddressOrGUID remoteSystem);

    void Clear(void);

    virtual void Update(void);
    virtual PluginReceiveResult OnReceive(Packet *packet);
    virtual void OnRakPeerShutdown(void);
    virtual void OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason );

    struct PendingChallenge
    {
        VENet::VEString identifier;
        AddressOrGUID remoteSystem;
        VENet::Time time;
        bool sentHash;
    };

    DataStructures::Queue<PendingChallenge> outgoingChallenges;

    struct NonceAndRemoteSystemRequest
    {
        char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
        VENet::AddressOrGUID remoteSystem;
        unsigned short requestId;
        VENet::Time whenGenerated;
    };
    struct VE_DLL_EXPORT NonceGenerator
    {
        NonceGenerator();
        ~NonceGenerator();
        void GetNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], unsigned short *requestId, VENet::AddressOrGUID remoteSystem);
        void GenerateNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH]);
        bool GetNonceById(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], unsigned short requestId, VENet::AddressOrGUID remoteSystem, bool popIfFound);
        void Clear(void);
        void ClearByAddress(VENet::AddressOrGUID remoteSystem);
        void Update(VENet::Time curTime);

        DataStructures::List<TwoWayAuthentication::NonceAndRemoteSystemRequest*> generatedNonces;
        unsigned short nextRequestId;
    };

protected:
    void PushToUser(MessageID messageId, VENet::VEString password, VENet::AddressOrGUID remoteSystem);
    DataStructures::Hash<VENet::VEString, VENet::VEString, 16, VENet::VEString::ToInteger > passwords;

    VENet::Time whenLastTimeoutCheck;

    NonceGenerator nonceGenerator;

    void OnNonceRequest(Packet *packet);
    void OnNonceReply(Packet *packet);
    PluginReceiveResult OnHashedNonceAndPassword(Packet *packet);
    void OnPasswordResult(Packet *packet);
    void Hash(char thierNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], VENet::VEString password, char out[HASHED_NONCE_AND_PW_LENGTH]);
};

}

#endif

