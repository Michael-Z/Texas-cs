#pragma once

#include "VEPeerInterface.h"
#include "VENetStatistics.h"
#include "VENetTypes.h"
#include "BitStream.h"
#include "VESleep.h"
#include "PacketLogger.h"
#include "PacketFileLogger.h"
#include "MessageIdentifiers.h"
#ifdef VE_PLATFORM_IOS
#define _NEW_
#include "VePower.h"
VE_POWER_API void* operator new(VeSizeT stSize, void* pvMemory) throw();
#endif
#include <VePowerPCH.h>

using namespace VENet;

enum ServerEvent
{
	ID_SERVER_START = ID_USER_PACKET_ENUM,
	ID_SERVER_GLOBAL_RPC,
	ID_SERVER_ENTITY_RPC,
	ID_SERVER_REQ_ENTITY_UPDATE
};

enum ClientEvent
{
	ID_CLIENT_START = ID_USER_PACKET_ENUM,
	ID_CLIENT_GLOBAL_RPC,
	ID_CLIENT_ENTITY_RPC,
	ID_CLIENT_ENTITY_UPDATE,
	ID_CLIENT_ENTITY_DESTORY,
	ID_CLIENT_FORCE_DISCONNECT
};

enum PropertyType
{
	TYPE_VeUInt8,
	TYPE_VeUInt16,
	TYPE_VeUInt32,
	TYPE_VeUInt64,
	TYPE_VeInt8,
	TYPE_VeInt16,
	TYPE_VeInt32,
	TYPE_VeInt64,
	TYPE_VeStringA
};

void StreamErrorClear();

void StreamError();

bool IsStreamError();

template<class T>
BitStream& operator << (BitStream& kOut, const T& tData)
{
	kOut.Write(tData);
	return kOut;
}

template<class T>
BitStream& operator >> (BitStream& kIn, T& tData)
{
	bool bRes = kIn.Read(tData);
	if(!bRes)
	{
		StreamError();
	}
	return kIn;
}

template<class T>
BitStream& operator << (BitStream& kOut, const VeVector<T>& kData)
{
	VeUInt32 u32Num = kData.Size();
	kOut << u32Num;
	for(VeUInt32 i(0); i < u32Num; ++i)
	{
		kOut << kData[i];
	}
	return kOut;
}

template<class T>
BitStream& operator >> (BitStream& kIn, VeVector<T>& kData)
{
	VeUInt32 u32Num;
	kIn >> u32Num;
	kData.Resize(u32Num);
	for(VeUInt32 i(0); i < u32Num; ++i)
	{
		kIn >> kData[i];
	}
	return kIn;
}

BitStream& operator << (BitStream& kOut, const VeChar8* pcStr);

BitStream& operator << (BitStream& kOut, const VeStringA& kStr);

BitStream& operator >> (BitStream& kIn, VeStringA& kStr);
