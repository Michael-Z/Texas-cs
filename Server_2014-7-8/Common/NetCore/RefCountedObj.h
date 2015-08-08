////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 RefCountedObj.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VEMemoryOverride.h"

class RefCountedObj
{
	public:
		RefCountedObj() {refCount=1;}
		virtual ~RefCountedObj() {}
		void AddRef(void) {refCount++;}
		void Deref(void) {if (--refCount==0) VENet::OP_DELETE(this, _FILE_AND_LINE_);}
		int refCount;
};