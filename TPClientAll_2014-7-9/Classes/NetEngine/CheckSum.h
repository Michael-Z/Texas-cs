////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 CheckSum.h
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

class CheckSum
{

public:

    CheckSum()
    {
        Clear();
    }

    void Clear()
    {
        sum = 0;
        r = 55665;
        c1 = 52845;
        c2 = 22719;
    }

    void Add ( unsigned int w );


    void Add ( unsigned short w );

    void Add ( unsigned char* b, unsigned int length );

    void Add ( unsigned char b );

    unsigned int Get ()
    {
        return sum;
    }

protected:
    unsigned short r;
    unsigned short c1;
    unsigned short c2;
    unsigned int sum;
};

