////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DataCompressor.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "VEMemoryOverride.h"
#include "DS_HuffmanEncodingTree.h"
#include "Export.h"

namespace VENet
{
class VE_DLL_EXPORT DataCompressor
{
public:
    STATIC_FACTORY_DECLARATIONS(DataCompressor)

    static void Compress( unsigned char *userData, unsigned sizeInBytes, VENet::BitStream * output );
    static unsigned DecompressAndAllocate( VENet::BitStream * input, unsigned char **output );
};

}
