////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_ThreadsafeAllocatingQueue.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DS_Queue.h"
#include "SimpleMutex.h"
#include "DS_MemoryPool.h"

namespace DataStructures
{

template <class structureType>
class VE_DLL_EXPORT ThreadsafeAllocatingQueue
{
public:
    void Push(structureType *s);
    structureType *PopInaccurate(void);
    structureType *Pop(void);
    void SetPageSize(int size);
    bool IsEmpty(void);
    structureType * operator[] ( unsigned int position );
    void RemoveAtIndex( unsigned int position );
    unsigned int Size( void );

    structureType *Allocate(const char *file, unsigned int line);
    void Deallocate(structureType *s, const char *file, unsigned int line);
    void Clear(const char *file, unsigned int line);
protected:

    MemoryPool<structureType> memoryPool;
    VENet::SimpleMutex memoryPoolMutex;
    Queue<structureType*> queue;
    VENet::SimpleMutex queueMutex;
};

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::Push(structureType *s)
{
    queueMutex.Lock();
    queue.Push(s, _FILE_AND_LINE_ );
    queueMutex.Unlock();
}

template <class structureType>
structureType *ThreadsafeAllocatingQueue<structureType>::PopInaccurate(void)
{
    structureType *s;
    if (queue.IsEmpty())
        return 0;
    queueMutex.Lock();
    if (queue.IsEmpty()==false)
        s=queue.Pop();
    else
        s=0;
    queueMutex.Unlock();
    return s;
}

template <class structureType>
structureType *ThreadsafeAllocatingQueue<structureType>::Pop(void)
{
    structureType *s;
    queueMutex.Lock();
    if (queue.IsEmpty())
    {
        queueMutex.Unlock();
        return 0;
    }
    s=queue.Pop();
    queueMutex.Unlock();
    return s;
}

template <class structureType>
structureType *ThreadsafeAllocatingQueue<structureType>::Allocate(const char *file, unsigned int line)
{
    structureType *s;
    memoryPoolMutex.Lock();
    s=memoryPool.Allocate(file, line);
    memoryPoolMutex.Unlock();
    s = new ((void*)s) structureType;
    return s;
}
template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::Deallocate(structureType *s, const char *file, unsigned int line)
{
    s->~structureType();
    memoryPoolMutex.Lock();
    memoryPool.Release(s, file, line);
    memoryPoolMutex.Unlock();
}

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::Clear(const char *file, unsigned int line)
{
    memoryPoolMutex.Lock();
    for (unsigned int i=0; i < queue.Size(); i++)
    {
        queue[i]->~structureType();
        memoryPool.Release(queue[i], file, line);
    }
    queue.Clear(file, line);
    memoryPoolMutex.Unlock();
    memoryPoolMutex.Lock();
    memoryPool.Clear(file, line);
    memoryPoolMutex.Unlock();
}

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::SetPageSize(int size)
{
    memoryPool.SetPageSize(size);
}

template <class structureType>
bool ThreadsafeAllocatingQueue<structureType>::IsEmpty(void)
{
    bool isEmpty;
    queueMutex.Lock();
    isEmpty=queue.IsEmpty();
    queueMutex.Unlock();
    return isEmpty;
}

template <class structureType>
structureType * ThreadsafeAllocatingQueue<structureType>::operator[] ( unsigned int position )
{
    structureType *s;
    queueMutex.Lock();
    s=queue[position];
    queueMutex.Unlock();
    return s;
}

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::RemoveAtIndex( unsigned int position )
{
    queueMutex.Lock();
    queue.RemoveAtIndex(position);
    queueMutex.Unlock();
}

template <class structureType>
unsigned int ThreadsafeAllocatingQueue<structureType>::Size( void )
{
    unsigned int s;
    queueMutex.Lock();
    s=queue.Size();
    queueMutex.Unlock();
    return s;
}

}
