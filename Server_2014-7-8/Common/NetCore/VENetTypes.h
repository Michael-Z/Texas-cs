////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 VENetTypes.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VENetDefines.h"
#include "NativeTypes.h"
#include "VENetTime.h"
#include "Export.h"
#include "SocketIncludes.h"
#include "WindowsIncludes.h"


namespace VENet
{
class VEPeerInterface;
class BitStream;
struct Packet;

enum StartupResult
{
    VENET_STARTED,
    VENET_ALREADY_STARTED,
    INVALID_SOCKET_DESCRIPTORS,
    INVALID_MAX_CONNECTIONS,
    SOCKET_FAMILY_NOT_SUPPORTED,
    SOCKET_PORT_ALREADY_IN_USE,
    SOCKET_FAILED_TO_BIND,
    SOCKET_FAILED_TEST_SEND,
    PORT_CANNOT_BE_ZERO,
    FAILED_TO_CREATE_NETWORK_THREAD,
    STARTUP_OTHER_FAILURE
};


enum ConnectionAttemptResult
{
    CONNECTION_ATTEMPT_STARTED,
    INVALID_PARAMETER,
    CANNOT_RESOLVE_DOMAIN_NAME,
    ALREADY_CONNECTED_TO_ENDPOINT,
    CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS,
    SECURITY_INITIALIZATION_FAILED
};

enum ConnectionState
{
    IS_PENDING,
    IS_CONNECTING,
    IS_CONNECTED,
    IS_DISCONNECTING,
    IS_SILENTLY_DISCONNECTING,
    IS_DISCONNECTED,
    IS_NOT_CONNECTED
};

#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

typedef unsigned char UniqueIDType;
typedef unsigned short SystemIndex;
typedef unsigned char RPCIndex;
const int MAX_RPC_MAP_SIZE=((RPCIndex)-1)-1;
const int UNDEFINED_RPC_INDEX=((RPCIndex)-1);

typedef unsigned char MessageID;

typedef uint32_t BitSize_t;

#if defined(_MSC_VER) && _MSC_VER > 0
#define PRINTF_64_BIT_MODIFIER "I64"
#else
#define PRINTF_64_BIT_MODIFIER "ll"
#endif

enum PublicKeyMode
{
    PKM_INSECURE_CONNECTION,

    PKM_ACCEPT_ANY_PUBLIC_KEY,

    PKM_USE_KNOWN_PUBLIC_KEY,

    PKM_USE_TWO_WAY_AUTHENTICATION
};

struct VE_DLL_EXPORT PublicKey
{
    PublicKeyMode publicKeyMode;

    char *remoteServerPublicKey;

    char *myPublicKey;

    char *myPrivateKey;
};

struct VE_DLL_EXPORT SocketDescriptor
{
    SocketDescriptor();
    SocketDescriptor(unsigned short _port, const char *_hostAddress);

    unsigned short port;

    char hostAddress[32];

    short socketFamily;

    unsigned short remotePortVENetWasStartedOn_PS3_PSP2;

    _PP_Instance_ chromeInstance;

    bool blockingSocket;

    unsigned int extraSocketOptions;
};

extern bool NonNumericHostString( const char *host );

struct VE_DLL_EXPORT SystemAddress
{
    SystemAddress();
    SystemAddress(const char *str);
    SystemAddress(const char *str, unsigned short port);


    union
    {
#if VENET_SUPPORT_IPV6==1
        struct sockaddr_in6 addr6;
#endif

        struct sockaddr_in addr4;
    } address;

    unsigned short debugPort;

    static int size(void);

    static unsigned long ToInteger( const SystemAddress &sa );

    unsigned char GetIPVersion(void) const;

    unsigned int GetIPPROTO(void) const;

    void SetToLoopback(void);

    void SetToLoopback(unsigned char ipVersion);

    bool IsLoopback(void) const;

    const char *ToString(bool writePort=true, char portDelineator='|') const;

    void ToString(bool writePort, char *dest, char portDelineator='|') const;

    bool FromString(const char *str, char portDelineator='|', int ipVersion=0);

    bool FromStringExplicitPort(const char *str, unsigned short port, int ipVersion=0);

    void CopyPort( const SystemAddress& right );

    bool EqualsExcludingPort( const SystemAddress& right ) const;

    unsigned short GetPort(void) const;

    unsigned short GetPortNetworkOrder(void) const;

    void SetPort(unsigned short s);

    void SetPortNetworkOrder(unsigned short s);

    void SetBinaryAddress(const char *str, char portDelineator=':');
    void ToString_Old(bool writePort, char *dest, char portDelineator=':') const;

    void FixForIPVersion(const SystemAddress &boundAddressToSocket);

    SystemAddress& operator = ( const SystemAddress& input );
    bool operator==( const SystemAddress& right ) const;
    bool operator!=( const SystemAddress& right ) const;
    bool operator > ( const SystemAddress& right ) const;
    bool operator < ( const SystemAddress& right ) const;

    SystemIndex systemIndex;

private:

#if VENET_SUPPORT_IPV6==1
    void ToString_New(bool writePort, char *dest, char portDelineator) const;
#endif
};

struct VE_DLL_EXPORT VENetGUID
{
    VENetGUID();
    explicit VENetGUID(uint64_t _g)
    {
        g=_g;
        systemIndex=(SystemIndex)-1;
    }
    uint64_t g;

    const char *ToString(void) const;

    void ToString(char *dest) const;

    bool FromString(const char *source);

    static unsigned long ToUint32( const VENetGUID &g );

    VENetGUID& operator = ( const VENetGUID& input )
    {
        g=input.g;
        systemIndex=input.systemIndex;
        return *this;
    }

    SystemIndex systemIndex;
    static int size()
    {
        return (int) sizeof(uint64_t);
    }

    bool operator==( const VENetGUID& right ) const;
    bool operator!=( const VENetGUID& right ) const;
    bool operator > ( const VENetGUID& right ) const;
    bool operator < ( const VENetGUID& right ) const;
};

#ifndef SWIG
const SystemAddress UNASSIGNED_SYSTEM_ADDRESS;
const VENetGUID UNASSIGNED_VENET_GUID((uint64_t)-1);
#endif

struct VE_DLL_EXPORT AddressOrGUID
{
    VENetGUID veNetGuid;
    SystemAddress systemAddress;

    SystemIndex GetSystemIndex(void) const
    {
        if (veNetGuid!=UNASSIGNED_VENET_GUID) return veNetGuid.systemIndex;
        else return systemAddress.systemIndex;
    }
    bool IsUndefined(void) const
    {
        return veNetGuid==UNASSIGNED_VENET_GUID && systemAddress==UNASSIGNED_SYSTEM_ADDRESS;
    }
    void SetUndefined(void)
    {
        veNetGuid=UNASSIGNED_VENET_GUID;
        systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
    }
    static unsigned long ToInteger( const AddressOrGUID &aog );
    const char *ToString(bool writePort=true) const;
    void ToString(bool writePort, char *dest) const;

    AddressOrGUID() {}
    AddressOrGUID( const AddressOrGUID& input )
    {
        veNetGuid=input.veNetGuid;
        systemAddress=input.systemAddress;
    }
    AddressOrGUID( const SystemAddress& input )
    {
        veNetGuid=UNASSIGNED_VENET_GUID;
        systemAddress=input;
    }
    AddressOrGUID( Packet *packet );
    AddressOrGUID( const VENetGUID& input )
    {
        veNetGuid=input;
        systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
    }
    AddressOrGUID& operator = ( const AddressOrGUID& input )
    {
        veNetGuid=input.veNetGuid;
        systemAddress=input.systemAddress;
        return *this;
    }

    AddressOrGUID& operator = ( const SystemAddress& input )
    {
        veNetGuid=UNASSIGNED_VENET_GUID;
        systemAddress=input;
        return *this;
    }

    AddressOrGUID& operator = ( const VENetGUID& input )
    {
        veNetGuid=input;
        systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
        return *this;
    }

    inline bool operator==( const AddressOrGUID& right ) const
    {
        return (veNetGuid!=UNASSIGNED_VENET_GUID && veNetGuid==right.veNetGuid) || (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS && systemAddress==right.systemAddress);
    }
};

typedef uint64_t NetworkID;

struct Packet
{
    SystemAddress systemAddress;

    VENetGUID guid;

    unsigned int length;

    BitSize_t bitSize;

    unsigned char* data;

    bool deleteData;

    bool wasGeneratedLocally;
};

const SystemIndex UNASSIGNED_PLAYER_INDEX = 65535;

const NetworkID UNASSIGNED_NETWORK_ID = (uint64_t) -1;

const int PING_TIMES_ARRAY_SIZE = 5;

struct VE_DLL_EXPORT uint24_t
{
    uint32_t val;

    uint24_t() {}
    inline operator uint32_t()
    {
        return val;
    }
    inline operator uint32_t() const
    {
        return val;
    }

    inline uint24_t(const uint24_t& a)
    {
        val=a.val;
    }
    inline uint24_t operator++()
    {
        ++val;
        val&=0x00FFFFFF;
        return *this;
    }
    inline uint24_t operator--()
    {
        --val;
        val&=0x00FFFFFF;
        return *this;
    }
    inline uint24_t operator++(int)
    {
        uint24_t temp(val);
        ++val;
        val&=0x00FFFFFF;
        return temp;
    }
    inline uint24_t operator--(int)
    {
        uint24_t temp(val);
        --val;
        val&=0x00FFFFFF;
        return temp;
    }
    inline uint24_t operator&(const uint24_t& a)
    {
        return uint24_t(val&a.val);
    }
    inline uint24_t& operator=(const uint24_t& a)
    {
        val=a.val;
        return *this;
    }
    inline uint24_t& operator+=(const uint24_t& a)
    {
        val+=a.val;
        val&=0x00FFFFFF;
        return *this;
    }
    inline uint24_t& operator-=(const uint24_t& a)
    {
        val-=a.val;
        val&=0x00FFFFFF;
        return *this;
    }
    inline bool operator==( const uint24_t& right ) const
    {
        return val==right.val;
    }
    inline bool operator!=( const uint24_t& right ) const
    {
        return val!=right.val;
    }
    inline bool operator > ( const uint24_t& right ) const
    {
        return val>right.val;
    }
    inline bool operator < ( const uint24_t& right ) const
    {
        return val<right.val;
    }
    inline const uint24_t operator+( const uint24_t &other ) const
    {
        return uint24_t(val+other.val);
    }
    inline const uint24_t operator-( const uint24_t &other ) const
    {
        return uint24_t(val-other.val);
    }
    inline const uint24_t operator/( const uint24_t &other ) const
    {
        return uint24_t(val/other.val);
    }
    inline const uint24_t operator*( const uint24_t &other ) const
    {
        return uint24_t(val*other.val);
    }

    inline uint24_t(const uint32_t& a)
    {
        val=a;
        val&=0x00FFFFFF;
    }
    inline uint24_t operator&(const uint32_t& a)
    {
        return uint24_t(val&a);
    }
    inline uint24_t& operator=(const uint32_t& a)
    {
        val=a;
        val&=0x00FFFFFF;
        return *this;
    }
    inline uint24_t& operator+=(const uint32_t& a)
    {
        val+=a;
        val&=0x00FFFFFF;
        return *this;
    }
    inline uint24_t& operator-=(const uint32_t& a)
    {
        val-=a;
        val&=0x00FFFFFF;
        return *this;
    }
    inline bool operator==( const uint32_t& right ) const
    {
        return val==(right&0x00FFFFFF);
    }
    inline bool operator!=( const uint32_t& right ) const
    {
        return val!=(right&0x00FFFFFF);
    }
    inline bool operator > ( const uint32_t& right ) const
    {
        return val>(right&0x00FFFFFF);
    }
    inline bool operator < ( const uint32_t& right ) const
    {
        return val<(right&0x00FFFFFF);
    }
    inline const uint24_t operator+( const uint32_t &other ) const
    {
        return uint24_t(val+other);
    }
    inline const uint24_t operator-( const uint32_t &other ) const
    {
        return uint24_t(val-other);
    }
    inline const uint24_t operator/( const uint32_t &other ) const
    {
        return uint24_t(val/other);
    }
    inline const uint24_t operator*( const uint32_t &other ) const
    {
        return uint24_t(val*other);
    }
};

}