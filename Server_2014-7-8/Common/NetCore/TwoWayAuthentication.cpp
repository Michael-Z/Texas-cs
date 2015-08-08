#include "NativeFeatureIncludes.h"
#if _VENET_SUPPORT_TwoWayAuthentication==1

#include "TwoWayAuthentication.h"
#include "Rand.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "VEPeerInterface.h"

#if LIBCAT_SECURITY==1
static const int HASH_BITS = 256;
static const int HASH_BYTES = HASH_BITS / 8;
static const int STRENGTHENING_FACTOR = 256;
#include <cat/crypt/hash/Skein.hpp>
#endif

using namespace VENet;

enum NegotiationIdentifiers
{
    ID_NONCE_REQUEST,
    ID_NONCE_REPLY,
    ID_HASHED_NONCE_AND_PASSWORD,
};

TwoWayAuthentication::NonceGenerator::NonceGenerator()
{
    nextRequestId=0;
}
TwoWayAuthentication::NonceGenerator::~NonceGenerator()
{
    Clear();
}
void TwoWayAuthentication::NonceGenerator::GetNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], unsigned short *requestId, VENet::AddressOrGUID remoteSystem)
{
    TwoWayAuthentication::NonceAndRemoteSystemRequest *narsr = VENet::OP_NEW<TwoWayAuthentication::NonceAndRemoteSystemRequest>(_FILE_AND_LINE_);
    narsr->remoteSystem=remoteSystem;
    GenerateNonce(narsr->nonce);
    narsr->requestId=nextRequestId++;
    *requestId=narsr->requestId;
    memcpy(nonce,narsr->nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    narsr->whenGenerated=VENet::GetTime();
    generatedNonces.Push(narsr,_FILE_AND_LINE_);
}
void TwoWayAuthentication::NonceGenerator::GenerateNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH])
{
    fillBufferMT(nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
}
bool TwoWayAuthentication::NonceGenerator::GetNonceById(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], unsigned short requestId, VENet::AddressOrGUID remoteSystem, bool popIfFound)
{
    unsigned int i;
    for (i=0; i < generatedNonces.Size(); i++)
    {
        if (generatedNonces[i]->requestId==requestId)
        {
            if (remoteSystem==generatedNonces[i]->remoteSystem)
            {
                memcpy(nonce,generatedNonces[i]->nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
                if (popIfFound)
                {
                    VENet::OP_DELETE(generatedNonces[i],_FILE_AND_LINE_);
                    generatedNonces.RemoveAtIndex(i);
                }
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}
void TwoWayAuthentication::NonceGenerator::Clear(void)
{
    unsigned int i;
    for (i=0; i < generatedNonces.Size(); i++)
        VENet::OP_DELETE(generatedNonces[i],_FILE_AND_LINE_);
    generatedNonces.Clear(true,_FILE_AND_LINE_);
}
void TwoWayAuthentication::NonceGenerator::ClearByAddress(VENet::AddressOrGUID remoteSystem)
{
    unsigned int i=0;
    while (i < generatedNonces.Size())
    {
        if (generatedNonces[i]->remoteSystem==remoteSystem)
        {
            VENet::OP_DELETE(generatedNonces[i],_FILE_AND_LINE_);
            generatedNonces.RemoveAtIndex(i);
        }
        else
        {
            i++;
        }
    }
}
void TwoWayAuthentication::NonceGenerator::Update(VENet::Time curTime)
{
    if (generatedNonces.Size()>0 && GreaterThan(curTime-5000, generatedNonces[0]->whenGenerated))
    {
        VENet::OP_DELETE(generatedNonces[0], _FILE_AND_LINE_);
        generatedNonces.RemoveAtIndex(0);
    }
}
TwoWayAuthentication::TwoWayAuthentication()
{
    whenLastTimeoutCheck=VENet::GetTime();
    seedMT(VENet::GetTimeMS());
}
TwoWayAuthentication::~TwoWayAuthentication()
{
    Clear();
}
bool TwoWayAuthentication::AddPassword(VENet::VEString identifier, VENet::VEString password)
{
    if (password.IsEmpty())
        return false;

    if (identifier.IsEmpty())
        return false;

    if (password==identifier)
        return false;

    if (passwords.GetIndexOf(identifier.C_String()).IsInvalid()==false)
        return false;

    passwords.Push(identifier, password,_FILE_AND_LINE_);
    return true;
}
bool TwoWayAuthentication::Challenge(VENet::VEString identifier, AddressOrGUID remoteSystem)
{
    DataStructures::HashIndex skhi = passwords.GetIndexOf(identifier.C_String());
    if (skhi.IsInvalid())
        return false;

    VENet::BitStream bsOut;
    bsOut.Write((MessageID)ID_TWO_WAY_AUTHENTICATION_NEGOTIATION);
    bsOut.Write((MessageID)ID_NONCE_REQUEST);
    SendUnified(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,remoteSystem,false);

    PendingChallenge pc;
    pc.identifier=identifier;
    pc.remoteSystem=remoteSystem;
    pc.time=VENet::GetTime();
    pc.sentHash=false;
    outgoingChallenges.Push(pc,_FILE_AND_LINE_);

    return true;
}
void TwoWayAuthentication::Update(void)
{
    VENet::Time curTime = VENet::GetTime();
    nonceGenerator.Update(curTime);
    if (GreaterThan(curTime - CHALLENGE_MINIMUM_TIMEOUT, whenLastTimeoutCheck))
    {
        while (outgoingChallenges.Size() && GreaterThan(curTime - CHALLENGE_MINIMUM_TIMEOUT, outgoingChallenges.Peek().time))
        {
            PendingChallenge pc = outgoingChallenges.Pop();

            PushToUser(ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT, pc.identifier, pc.remoteSystem);
        }

        whenLastTimeoutCheck=curTime+CHALLENGE_MINIMUM_TIMEOUT;
    }
}
PluginReceiveResult TwoWayAuthentication::OnReceive(Packet *packet)
{
    switch (packet->data[0])
    {
    case ID_TWO_WAY_AUTHENTICATION_NEGOTIATION:
    {
        if (packet->length>=sizeof(MessageID)*2)
        {
            switch (packet->data[sizeof(MessageID)])
            {
            case ID_NONCE_REQUEST:
            {
                OnNonceRequest(packet);
            }
            break;
            case ID_NONCE_REPLY:
            {
                OnNonceReply(packet);
            }
            break;
            case ID_HASHED_NONCE_AND_PASSWORD:
            {
                return OnHashedNonceAndPassword(packet);
            }
            break;
            }
        }
        return RR_STOP_PROCESSING_AND_DEALLOCATE;
    }
    case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE:
    case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS:
    {
        if (packet->wasGeneratedLocally==false)
        {
            OnPasswordResult(packet);
            return RR_STOP_PROCESSING_AND_DEALLOCATE;
        }
        else
            break;
    }
    break;
    case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS:
    case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE:
    case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT:
        if (packet->wasGeneratedLocally==false)
            return RR_STOP_PROCESSING_AND_DEALLOCATE;
        break;
    }

    return RR_CONTINUE_PROCESSING;
}
void TwoWayAuthentication::OnRakPeerShutdown(void)
{
    Clear();
}
void TwoWayAuthentication::OnClosedConnection(const SystemAddress &systemAddress, VENetGUID VENetGuid, PI2_LostConnectionReason lostConnectionReason )
{
    (void) lostConnectionReason;

    unsigned int i=0;
    while (i < outgoingChallenges.Size())
    {
        if ((VENetGuid!=UNASSIGNED_VENET_GUID && outgoingChallenges[i].remoteSystem.veNetGuid==VENetGuid) ||
                (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS && outgoingChallenges[i].remoteSystem.systemAddress==systemAddress))
        {
            outgoingChallenges.RemoveAtIndex(i);
        }
        else
        {
            i++;
        }
    }

    if (VENetGuid!=UNASSIGNED_VENET_GUID)
        nonceGenerator.ClearByAddress(VENetGuid);
    else
        nonceGenerator.ClearByAddress(systemAddress);
}
void TwoWayAuthentication::Clear(void)
{
    outgoingChallenges.Clear(_FILE_AND_LINE_);
    passwords.Clear(_FILE_AND_LINE_);
    nonceGenerator.Clear();
}
void TwoWayAuthentication::PushToUser(MessageID messageId, VENet::VEString password, VENet::AddressOrGUID remoteSystem)
{
    VENet::BitStream output;
    output.Write(messageId);
    if (password.IsEmpty()==false)
        output.Write(password);
    Packet *p = AllocatePacketUnified(output.GetNumberOfBytesUsed());
    p->systemAddress=remoteSystem.systemAddress;
    p->systemAddress.systemIndex=(SystemIndex)-1;
    p->guid=remoteSystem.veNetGuid;
    p->wasGeneratedLocally=true;
    memcpy(p->data, output.GetData(), output.GetNumberOfBytesUsed());
    vePeerInterface->PushBackPacket(p, true);
}
void TwoWayAuthentication::OnNonceRequest(Packet *packet)
{
    VENet::BitStream bsIn(packet->data, packet->length, false);
    bsIn.IgnoreBytes(sizeof(MessageID)*2);

    char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
    unsigned short requestId;
    nonceGenerator.GetNonce(nonce,&requestId,packet);

    VENet::BitStream bsOut;
    bsOut.Write((MessageID)ID_TWO_WAY_AUTHENTICATION_NEGOTIATION);
    bsOut.Write((MessageID)ID_NONCE_REPLY);
    bsOut.Write(requestId);
    bsOut.WriteAlignedBytes((const unsigned char*) nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    SendUnified(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet,false);
}
void TwoWayAuthentication::OnNonceReply(Packet *packet)
{
    VENet::BitStream bsIn(packet->data, packet->length, false);
    bsIn.IgnoreBytes(sizeof(MessageID)*2);

    char thierNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
    unsigned short requestId;
    bsIn.Read(requestId);
    bsIn.ReadAlignedBytes((unsigned char *) thierNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);

    AddressOrGUID aog(packet);
    unsigned int i;
    for (i=0; i < outgoingChallenges.Size(); i++)
    {
        if (outgoingChallenges[i].remoteSystem==aog && outgoingChallenges[i].sentHash==false)
        {
            outgoingChallenges[i].sentHash=true;

            DataStructures::HashIndex skhi = passwords.GetIndexOf(outgoingChallenges[i].identifier.C_String());
            if (skhi.IsInvalid()==false)
            {
                VENet::VEString password = passwords.ItemAtIndex(skhi);

                char hashedNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
                Hash(thierNonce, password, hashedNonceAndPw);

                VENet::BitStream bsOut;
                bsOut.Write((MessageID)ID_TWO_WAY_AUTHENTICATION_NEGOTIATION);
                bsOut.Write((MessageID)ID_HASHED_NONCE_AND_PASSWORD);
                bsOut.Write(requestId);
                bsOut.Write(outgoingChallenges[i].identifier);
                bsOut.WriteAlignedBytes((const unsigned char*) hashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);
                SendUnified(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet,false);
            }

            return;
        }
    }
}
PluginReceiveResult TwoWayAuthentication::OnHashedNonceAndPassword(Packet *packet)
{
    VENet::BitStream bsIn(packet->data, packet->length, false);
    bsIn.IgnoreBytes(sizeof(MessageID)*2);

    char remoteHashedNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
    unsigned short requestId;
    bsIn.Read(requestId);
    VENet::VEString passwordIdentifier;
    bsIn.Read(passwordIdentifier);
    bsIn.ReadAlignedBytes((unsigned char *) remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);

    char usedNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
    if (nonceGenerator.GetNonceById(usedNonce, requestId, packet, true)==false)
        return RR_STOP_PROCESSING_AND_DEALLOCATE;

    DataStructures::HashIndex skhi = passwords.GetIndexOf(passwordIdentifier.C_String());
    if (skhi.IsInvalid()==false)
    {
        char hashedThisNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
        Hash(usedNonce, passwords.ItemAtIndex(skhi), hashedThisNonceAndPw);
        if (memcmp(hashedThisNonceAndPw, remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH)==0)
        {
            VENet::BitStream bsOut;
            bsOut.Write((MessageID)ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS);
            bsOut.WriteAlignedBytes((const unsigned char*) usedNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
            bsOut.WriteAlignedBytes((const unsigned char*) remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);
            bsOut.Write(passwordIdentifier);
            SendUnified(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet,false);

            PushToUser(ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS, passwordIdentifier, packet);

            return RR_STOP_PROCESSING_AND_DEALLOCATE;
        }
    }

    packet->data[0]=(MessageID) ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE;

    VENet::BitStream bsOut;
    bsOut.Write((MessageID)ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE);
    bsOut.WriteAlignedBytes((const unsigned char*) usedNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    bsOut.WriteAlignedBytes((const unsigned char*) remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);
    bsOut.Write(passwordIdentifier);
    SendUnified(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet,false);

    return RR_CONTINUE_PROCESSING;
}
void TwoWayAuthentication::OnPasswordResult(Packet *packet)
{
    VENet::BitStream bsIn(packet->data, packet->length, false);
    bsIn.IgnoreBytes(sizeof(MessageID)*1);
    char usedNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
    bsIn.ReadAlignedBytes((unsigned char *)usedNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    char hashedNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
    bsIn.ReadAlignedBytes((unsigned char *)hashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);
    VENet::VEString passwordIdentifier;
    bsIn.Read(passwordIdentifier);

    DataStructures::HashIndex skhi = passwords.GetIndexOf(passwordIdentifier.C_String());
    if (skhi.IsInvalid()==false)
    {
        VENet::VEString password = passwords.ItemAtIndex(skhi);
        char testHash[HASHED_NONCE_AND_PW_LENGTH];
        Hash(usedNonce, password, testHash);
        if (memcmp(testHash,hashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH)==0)
        {
            // Lookup the outgoing challenge and remove it from the list
            unsigned int i;
            AddressOrGUID aog(packet);
            for (i=0; i < outgoingChallenges.Size(); i++)
            {
                if (outgoingChallenges[i].identifier==passwordIdentifier &&
                        outgoingChallenges[i].remoteSystem==aog &&
                        outgoingChallenges[i].sentHash==true)
                {
                    outgoingChallenges.RemoveAtIndex(i);

                    PushToUser(packet->data[0], passwordIdentifier, packet);
                    return;
                }
            }
        }
    }
}
void TwoWayAuthentication::Hash(char thierNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], VENet::VEString password, char out[HASHED_NONCE_AND_PW_LENGTH])
{
#if LIBCAT_SECURITY==1
    cat::Skein hash;
    if (!hash.BeginKey(HASH_BITS)) return;
    hash.Crunch(thierNonce, TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    hash.Crunch(password.C_String(), (int) password.GetLength());
    hash.End();
    hash.Generate(out, HASH_BYTES, STRENGTHENING_FACTOR);
#else
    CSHA1 sha1;
    sha1.Update((unsigned char *) thierNonce, TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    sha1.Update((unsigned char *) password.C_String(), (unsigned int) password.GetLength());
    sha1.Final();
    sha1.GetHash((unsigned char *) out);
#endif
}

#endif
