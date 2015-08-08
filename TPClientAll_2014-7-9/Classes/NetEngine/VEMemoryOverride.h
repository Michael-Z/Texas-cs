////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VEMemoryOverride.h
//  Version:     v1.00
//  Created:     29/10/2012 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Export.h"
#include "VENetDefines.h"
#include <new>
#include "VEAlloca.h"

extern VE_DLL_EXPORT void * (*rakMalloc) (size_t size);
extern VE_DLL_EXPORT void * (*rakRealloc) (void *p, size_t size);
extern VE_DLL_EXPORT void (*rakFree) (void *p);
extern VE_DLL_EXPORT void * (*rakMalloc_Ex) (size_t size, const char *file, unsigned int line);
extern VE_DLL_EXPORT void * (*rakRealloc_Ex) (void *p, size_t size, const char *file, unsigned int line);
extern VE_DLL_EXPORT void (*veFree_Ex) (void *p, const char *file, unsigned int line);
extern VE_DLL_EXPORT void (*notifyOutOfMemory) (const char *file, const long line);
extern VE_DLL_EXPORT void * (*dlMallocMMap) (size_t size);
extern VE_DLL_EXPORT void * (*dlMallocDirectMMap) (size_t size);
extern VE_DLL_EXPORT int (*dlMallocMUnmap) (void* ptr, size_t size);

void VE_DLL_EXPORT SetMalloc( void* (*userFunction)(size_t size) );
void VE_DLL_EXPORT SetRealloc( void* (*userFunction)(void *p, size_t size) );
void VE_DLL_EXPORT SetFree( void (*userFunction)(void *p) );
void VE_DLL_EXPORT SetMalloc_Ex( void* (*userFunction)(size_t size, const char *file, unsigned int line) );
void VE_DLL_EXPORT SetRealloc_Ex( void* (*userFunction)(void *p, size_t size, const char *file, unsigned int line) );
void VE_DLL_EXPORT SetFree_Ex( void (*userFunction)(void *p, const char *file, unsigned int line) );
void VE_DLL_EXPORT SetNotifyOutOfMemory( void (*userFunction)(const char *file, const long line) );
void VE_DLL_EXPORT SetDLMallocMMap( void* (*userFunction)(size_t size) );
void VE_DLL_EXPORT SetDLMallocDirectMMap( void* (*userFunction)(size_t size) );
void VE_DLL_EXPORT SetDLMallocMUnmap( int (*userFunction)(void* ptr, size_t size) );

extern VE_DLL_EXPORT void * (*GetMalloc()) (size_t size);
extern VE_DLL_EXPORT void * (*GetRealloc()) (void *p, size_t size);
extern VE_DLL_EXPORT void (*GetFree()) (void *p);
extern VE_DLL_EXPORT void * (*GetMalloc_Ex()) (size_t size, const char *file, unsigned int line);
extern VE_DLL_EXPORT void * (*GetRealloc_Ex()) (void *p, size_t size, const char *file, unsigned int line);
extern VE_DLL_EXPORT void (*GetFree_Ex()) (void *p, const char *file, unsigned int line);
extern VE_DLL_EXPORT void *(*GetDLMallocMMap())(size_t size);
extern VE_DLL_EXPORT void *(*GetDLMallocDirectMMap())(size_t size);
extern VE_DLL_EXPORT int (*GetDLMallocMUnmap())(void* ptr, size_t size);

namespace VENet
{

template <class Type>
VE_DLL_EXPORT Type* OP_NEW(const char *file, unsigned int line)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
    Type *t = new (buffer) Type;
    return t;
#else
    (void) file;
    (void) line;
    return new Type;
#endif
}

template <class Type, class P1>
VE_DLL_EXPORT Type* OP_NEW_1(const char *file, unsigned int line, const P1 &p1)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1);
    return t;
#else
    (void) file;
    (void) line;
    return new Type(p1);
#endif
}

template <class Type, class P1, class P2>
VE_DLL_EXPORT Type* OP_NEW_2(const char *file, unsigned int line, const P1 &p1, const P2 &p2)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1, p2);
    return t;
#else
    (void) file;
    (void) line;
    return new Type(p1, p2);
#endif
}

template <class Type, class P1, class P2, class P3>
VE_DLL_EXPORT Type* OP_NEW_3(const char *file, unsigned int line, const P1 &p1, const P2 &p2, const P3 &p3)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1, p2, p3);
    return t;
#else
    (void) file;
    (void) line;
    return new Type(p1, p2, p3);
#endif
}

template <class Type, class P1, class P2, class P3, class P4>
VE_DLL_EXPORT Type* OP_NEW_4(const char *file, unsigned int line, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1, p2, p3, p4);
    return t;
#else
    (void) file;
    (void) line;
    return new Type(p1, p2, p3, p4);
#endif
}


template <class Type>
VE_DLL_EXPORT Type* OP_NEW_ARRAY(const int count, const char *file, unsigned int line)
{
    if (count==0)
        return 0;

#if _USE_VE_MEMORY_OVERRIDE==1
    char *buffer = (char *) (GetMalloc_Ex())(sizeof(int)+sizeof(Type)*count, file, line);
    ((int*)buffer)[0]=count;
    for (int i=0; i<count; i++)
    {
        new(buffer+sizeof(int)+i*sizeof(Type)) Type;
    }
    return (Type *) (buffer+sizeof(int));
#else
    (void) file;
    (void) line;
    return new Type[count];
#endif

}

template <class Type>
VE_DLL_EXPORT void OP_DELETE(Type *buff, const char *file, unsigned int line)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    if (buff==0) return;
    buff->~Type();
    (GetFree_Ex())((char*)buff, file, line );
#else
    (void) file;
    (void) line;
    delete buff;
#endif

}

template <class Type>
VE_DLL_EXPORT void OP_DELETE_ARRAY(Type *buff, const char *file, unsigned int line)
{
#if _USE_VE_MEMORY_OVERRIDE==1
    if (buff==0)
        return;

    int count = ((int*)((char*)buff-sizeof(int)))[0];
    Type *t;
    for (int i=0; i<count; i++)
    {
        t = buff+i;
        t->~Type();
    }
    (GetFree_Ex())((char*)buff-sizeof(int), file, line );
#else
    (void) file;
    (void) line;
    delete [] buff;
#endif

}

void VE_DLL_EXPORT * _RakMalloc (size_t size);
void VE_DLL_EXPORT * _RakRealloc (void *p, size_t size);
void VE_DLL_EXPORT _RakFree (void *p);
void VE_DLL_EXPORT * _RakMalloc_Ex (size_t size, const char *file, unsigned int line);
void VE_DLL_EXPORT * _RakRealloc_Ex (void *p, size_t size, const char *file, unsigned int line);
void VE_DLL_EXPORT _RakFree_Ex (void *p, const char *file, unsigned int line);
void VE_DLL_EXPORT * _DLMallocMMap (size_t size);
void VE_DLL_EXPORT * _DLMallocDirectMMap (size_t size);
int VE_DLL_EXPORT _DLMallocMUnmap (void *p, size_t size);

}

void UseVENetFixedHeap(size_t initialCapacity,
                        void * (*yourMMapFunction) (size_t size) = VENet::_DLMallocMMap,
                        void * (*yourDirectMMapFunction) (size_t size) = VENet::_DLMallocDirectMMap,
                        int (*yourMUnmapFunction) (void *p, size_t size) = VENet::_DLMallocMUnmap);

void FreeVENetFixedHeap(void);
