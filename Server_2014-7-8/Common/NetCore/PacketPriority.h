////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 PacketPriority.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

enum PacketPriority
{
	IMMEDIATE_PRIORITY,

	HIGH_PRIORITY,

	MEDIUM_PRIORITY,   

	LOW_PRIORITY,

	NUMBER_OF_PRIORITIES
};

enum PacketReliability
{
	UNRELIABLE,

	UNRELIABLE_SEQUENCED,

	RELIABLE,

	RELIABLE_ORDERED,

	RELIABLE_SEQUENCED,

	UNRELIABLE_WITH_ACK_RECEIPT,

	RELIABLE_WITH_ACK_RECEIPT,

	RELIABLE_ORDERED_WITH_ACK_RECEIPT,

	NUMBER_OF_RELIABILITIES
};