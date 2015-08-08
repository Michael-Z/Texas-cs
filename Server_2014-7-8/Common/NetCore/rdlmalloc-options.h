#ifdef _VENET_SUPPORT_DL_MALLOC

#pragma once

#include "NativeTypes.h"
#include "Export.h"
#include <stdio.h>
#include <stdlib.h>

#define MSPACES 1
#define ONLY_MSPACES 1
#define USE_DL_PREFIX 1
#define NO_MALLINFO	1
#define USE_LOCKS 1

extern VE_DLL_EXPORT void * (*dlMallocMMap) (size_t size);
extern VE_DLL_EXPORT void * (*dlMallocDirectMMap) (size_t size);
extern VE_DLL_EXPORT int (*dlMallocMUnmap) (void* ptr, size_t size);

#define MMAP(s)         dlMallocMMap(s)
#define MUNMAP(a, s)    dlMallocMUnmap((a), (s))
#define DIRECT_MMAP(s)       dlMallocDirectMMap(s)

#endif