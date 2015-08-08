////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_BytePool.h
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
#include "DS_MemoryPool.h"
#include "Export.h"
#include "SimpleMutex.h"
#include "VEAssert.h"

namespace DataStructures
{
class VE_DLL_EXPORT BytePool
{
public:
    BytePool();
    ~BytePool();

    void SetPageSize(int size);
    unsigned char* Allocate(int bytesWanted, const char *file, unsigned int line);
    void Release(unsigned char *data, const char *file, unsigned int line);
    void Clear(const char *file, unsigned int line);
protected:
    MemoryPool<unsigned char[128]> pool128;
    MemoryPool<unsigned char[512]> pool512;
    MemoryPool<unsigned char[2048]> pool2048;
    MemoryPool<unsigned char[8192]> pool8192;
#ifdef _THREADSAFE_BYTE_POOL
    SimpleMutex mutex128;
    SimpleMutex mutex512;
    SimpleMutex mutex2048;
    SimpleMutex mutex8192;
#endif
};
}
