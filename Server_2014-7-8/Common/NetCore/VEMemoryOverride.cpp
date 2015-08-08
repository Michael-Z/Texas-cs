#include "VEMemoryOverride.h"
#include "VEAssert.h"
#include <stdlib.h>

#ifdef _VENET_SUPPORT_DL_MALLOC
#include "rdlmalloc.h"
#endif


using namespace VENet;

#if _USE_VE_MEMORY_OVERRIDE==1
#if defined(malloc)
#pragma push_macro("malloc")
#undef malloc
#define RMO_MALLOC_UNDEF
#endif

#if defined(realloc)
#pragma push_macro("realloc")
#undef realloc
#define RMO_REALLOC_UNDEF
#endif

#if defined(free)
#pragma push_macro("free")
#undef free
#define RMO_FREE_UNDEF
#endif
#endif

void DefaultOutOfMemoryHandler(const char *file, const long line)
{
    (void) file;
    (void) line;
    VEAssert(0);
}

void * (*rakMalloc) (size_t size) = VENet::_RakMalloc;
void* (*rakRealloc) (void *p, size_t size) = VENet::_RakRealloc;
void (*rakFree) (void *p) = VENet::_RakFree;
void* (*rakMalloc_Ex) (size_t size, const char *file, unsigned int line) = VENet::_RakMalloc_Ex;
void* (*rakRealloc_Ex) (void *p, size_t size, const char *file, unsigned int line) = VENet::_RakRealloc_Ex;
void (*veFree_Ex) (void *p, const char *file, unsigned int line) = VENet::_RakFree_Ex;
void (*notifyOutOfMemory) (const char *file, const long line)=DefaultOutOfMemoryHandler;
void * (*dlMallocMMap) (size_t size) = VENet::_DLMallocMMap;
void * (*dlMallocDirectMMap) (size_t size) = VENet::_DLMallocDirectMMap;
int (*dlMallocMUnmap) (void* ptr, size_t size) = VENet::_DLMallocMUnmap;

void SetMalloc( void* (*userFunction)(size_t size) )
{
    rakMalloc=userFunction;
}
void SetRealloc( void* (*userFunction)(void *p, size_t size) )
{
    rakRealloc=userFunction;
}
void SetFree( void (*userFunction)(void *p) )
{
    rakFree=userFunction;
}
void SetMalloc_Ex( void* (*userFunction)(size_t size, const char *file, unsigned int line) )
{
    rakMalloc_Ex=userFunction;
}
void SetRealloc_Ex( void* (*userFunction)(void *p, size_t size, const char *file, unsigned int line) )
{
    rakRealloc_Ex=userFunction;
}
void SetFree_Ex( void (*userFunction)(void *p, const char *file, unsigned int line) )
{
    veFree_Ex=userFunction;
}
void SetNotifyOutOfMemory( void (*userFunction)(const char *file, const long line) )
{
    notifyOutOfMemory=userFunction;
}
void SetDLMallocMMap( void* (*userFunction)(size_t size) )
{
    dlMallocMMap=userFunction;
}
void SetDLMallocDirectMMap( void* (*userFunction)(size_t size) )
{
    dlMallocDirectMMap=userFunction;
}
void SetDLMallocMUnmap( int (*userFunction)(void* ptr, size_t size) )
{
    dlMallocMUnmap=userFunction;
}
void * (*GetMalloc()) (size_t size)
{
    return rakMalloc;
}
void * (*GetRealloc()) (void *p, size_t size)
{
    return rakRealloc;
}
void (*GetFree()) (void *p)
{
    return rakFree;
}
void * (*GetMalloc_Ex()) (size_t size, const char *file, unsigned int line)
{
    return rakMalloc_Ex;
}
void * (*GetRealloc_Ex()) (void *p, size_t size, const char *file, unsigned int line)
{
    return rakRealloc_Ex;
}
void (*GetFree_Ex()) (void *p, const char *file, unsigned int line)
{
    return veFree_Ex;
}
void *(*GetDLMallocMMap())(size_t size)
{
    return dlMallocMMap;
}
void *(*GetDLMallocDirectMMap())(size_t size)
{
    return dlMallocDirectMMap;
}
int (*GetDLMallocMUnmap())(void* ptr, size_t size)
{
    return dlMallocMUnmap;
}
void* VENet::_RakMalloc (size_t size)
{
    return malloc(size);
}

void* VENet::_RakRealloc (void *p, size_t size)
{
    return realloc(p,size);
}

void VENet::_RakFree (void *p)
{
    free(p);
}

void* VENet::_RakMalloc_Ex (size_t size, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    return malloc(size);
}

void* VENet::_RakRealloc_Ex (void *p, size_t size, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    return realloc(p,size);
}

void VENet::_RakFree_Ex (void *p, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    free(p);
}
#ifdef _VENET_SUPPORT_DL_MALLOC
void * VENet::_DLMallocMMap (size_t size)
{
    return VE_MMAP_DEFAULT(size);
}
void * VENet::_DLMallocDirectMMap (size_t size)
{
    return VE_DIRECT_MMAP_DEFAULT(size);
}
int VENet::_DLMallocMUnmap (void *p, size_t size)
{
    return VE_MUNMAP_DEFAULT(p,size);
}

static mspace veNetFixedHeapMSpace=0;

void* _DLMalloc(size_t size)
{
    return ve_mspace_malloc(veNetFixedHeapMSpace,size);
}

void* _DLRealloc(void *p, size_t size)
{
    return ve_mspace_realloc(veNetFixedHeapMSpace,p,size);
}

void _DLFree(void *p)
{
    if (p)
        ve_mspace_free(veNetFixedHeapMSpace,p);
}
void* _DLMalloc_Ex (size_t size, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    return ve_mspace_malloc(veNetFixedHeapMSpace,size);
}

void* _DLRealloc_Ex (void *p, size_t size, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    return ve_mspace_realloc(veNetFixedHeapMSpace,p,size);
}

void _DLFree_Ex (void *p, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    if (p)
        ve_mspace_free(veNetFixedHeapMSpace,p);
}

void UseVENetFixedHeap(size_t initialCapacity,
                        void * (*yourMMapFunction) (size_t size),
                        void * (*yourDirectMMapFunction) (size_t size),
                        int (*yourMUnmapFunction) (void *p, size_t size))
{
    SetDLMallocMMap(yourMMapFunction);
    SetDLMallocDirectMMap(yourDirectMMapFunction);
    SetDLMallocMUnmap(yourMUnmapFunction);
    SetMalloc(_DLMalloc);
    SetRealloc(_DLRealloc);
    SetFree(_DLFree);
    SetMalloc_Ex(_DLMalloc_Ex);
    SetRealloc_Ex(_DLRealloc_Ex);
    SetFree_Ex(_DLFree_Ex);

    veNetFixedHeapMSpace=rak_create_mspace(initialCapacity, 0);
}
void FreeVENetFixedHeap(void)
{
    if (veNetFixedHeapMSpace)
    {
        rak_destroy_mspace(veNetFixedHeapMSpace);
        veNetFixedHeapMSpace=0;
    }

    SetMalloc(_RakMalloc);
    SetRealloc(_RakRealloc);
    SetFree(_RakFree);
    SetMalloc_Ex(_RakMalloc_Ex);
    SetRealloc_Ex(_RakRealloc_Ex);
    SetFree_Ex(_RakFree_Ex);
}
#else
void * VENet::_DLMallocMMap (size_t size)
{
    (void) size;
    return 0;
}
void * VENet::_DLMallocDirectMMap (size_t size)
{
    (void) size;
    return 0;
}
int VENet::_DLMallocMUnmap (void *p, size_t size)
{
    (void) size;
    (void) p;
    return 0;
}
void* _DLMalloc(size_t size)
{
    (void) size;
    return 0;
}
void* _DLRealloc(void *p, size_t size)
{
    (void) p;
    (void) size;
    return 0;
}
void _DLFree(void *p)
{
    (void) p;
}
void* _DLMalloc_Ex (size_t size, const char *file, unsigned int line)
{
    (void) size;
    (void) file;
    (void) line;
    return 0;
}
void* _DLRealloc_Ex (void *p, size_t size, const char *file, unsigned int line)
{
    (void) p;
    (void) size;
    (void) file;
    (void) line;
    return 0;
}
void _DLFree_Ex (void *p, const char *file, unsigned int line)
{
    (void) p;
    (void) file;
    (void) line;
}

void UseVENetFixedHeap(size_t initialCapacity,
                        void * (*yourMMapFunction) (size_t size),
                        void * (*yourDirectMMapFunction) (size_t size),
                        int (*yourMUnmapFunction) (void *p, size_t size))
{
    (void) initialCapacity;
    (void) yourMMapFunction;
    (void) yourDirectMMapFunction;
    (void) yourMUnmapFunction;
}
void FreeVENetFixedHeap(void) {}
#endif

#if _USE_VE_MEMORY_OVERRIDE==1

#pragma pop_macro("malloc")
#undef RMO_MALLOC_UNDEF



#pragma pop_macro("realloc")
#undef RMO_REALLOC_UNDEF



#pragma pop_macro("free")
#undef RMO_FREE_UNDEF

#endif
