////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_HuffmanEncodingTreeFactory.h
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

namespace VENet
{
class HuffmanEncodingTree;

class HuffmanEncodingTreeFactory
{
public:
    HuffmanEncodingTreeFactory();
    void Reset( void );

    void AddToFrequencyTable( unsigned char *array, int size );

    void GetFrequencyTable( unsigned int _frequency[ 256 ] );

    unsigned int * GetFrequencyTable( void );

    HuffmanEncodingTree * GenerateTree( void );

private:

    unsigned int frequency[ 256 ];
};

}
