////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_ByteQueue.h
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
#include "Export.h"

namespace DataStructures
{
class ByteQueue
{
public:
    ByteQueue();
    ~ByteQueue();
    void WriteBytes(const char *in, unsigned length, const char *file, unsigned int line);
    bool ReadBytes(char *out, unsigned maxLengthToRead, bool peek);
    unsigned GetBytesWritten(void) const;
    char* PeekContiguousBytes(unsigned int *outLength) const;
    void IncrementReadOffset(unsigned length);
    void DecrementReadOffset(unsigned length);
    void Clear(const char *file, unsigned int line);
    void Print(void);

protected:
    char *data;
    unsigned readOffset, writeOffset, lengthAllocated;
};
}