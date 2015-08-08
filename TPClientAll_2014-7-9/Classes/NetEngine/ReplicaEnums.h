////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 ReplicaEnums.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

enum
{
	REPLICA_RECEIVE_DESTRUCTION=1<<0,
	REPLICA_RECEIVE_SERIALIZE=1<<1,
	REPLICA_RECEIVE_SCOPE_CHANGE=1<<2,
	REPLICA_SEND_CONSTRUCTION=1<<3,
	REPLICA_SEND_DESTRUCTION=1<<4,
	REPLICA_SEND_SCOPE_CHANGE=1<<5,
	REPLICA_SEND_SERIALIZE=1<<6,
	REPLICA_SET_ALL = 0xFF
};

enum ReplicaReturnResult
{
	REPLICA_PROCESS_LATER,
	REPLICA_PROCESSING_DONE,
	REPLICA_CANCEL_PROCESS,
	REPLICA_PROCESS_AGAIN,
	REPLICA_PROCESS_IMPLICIT
};
