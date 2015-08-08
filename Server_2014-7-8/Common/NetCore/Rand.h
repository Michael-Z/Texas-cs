////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 Rand.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Export.h"

extern void VE_DLL_EXPORT seedMT( unsigned int seed );

extern unsigned int VE_DLL_EXPORT reloadMT( void );

extern unsigned int VE_DLL_EXPORT randomMT( void );

extern float VE_DLL_EXPORT frandomMT( void );

extern void VE_DLL_EXPORT fillBufferMT( void *buffer, unsigned int bytes );

namespace VENet
{

class VE_DLL_EXPORT VENetRandom
{
public:
    VENetRandom();
    ~VENetRandom();
    void SeedMT( unsigned int seed );
    unsigned int ReloadMT( void );
    unsigned int RandomMT( void );
    float FrandomMT( void );
    void FillBufferMT( void *buffer, unsigned int bytes );

protected:
    unsigned int state[ 624 + 1 ];
    unsigned int *next;
    int left;
};

}
