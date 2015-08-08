////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_MemoryPool.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef __APPLE__
#include <stdlib.h>
#endif
#include "VEAssert.h"
#include "Export.h"

#include "VEMemoryOverride.h"

#define DS_MEMORY_POOL_MAX_FREE_PAGES 4

namespace DataStructures
{
template <class MemoryBlockType>
class VE_DLL_EXPORT MemoryPool
{
public:
    struct Page;
    struct MemoryWithPage
    {
        MemoryBlockType userMemory;
        Page *parentPage;
    };
    struct Page
    {
        MemoryWithPage** availableStack;
        int availableStackSize;
        MemoryWithPage* block;
        Page *next, *prev;
    };

    MemoryPool();
    ~MemoryPool();
    void SetPageSize(int size);
    MemoryBlockType *Allocate(const char *file, unsigned int line);
    void Release(MemoryBlockType *m, const char *file, unsigned int line);
    void Clear(const char *file, unsigned int line);

    int GetAvailablePagesSize(void) const
    {
        return availablePagesSize;
    }
    int GetUnavailablePagesSize(void) const
    {
        return unavailablePagesSize;
    }
    int GetMemoryPoolPageSize(void) const
    {
        return memoryPoolPageSize;
    }
protected:
    int BlocksPerPage(void) const;
    void AllocateFirst(void);
    bool InitPage(Page *page, Page *prev, const char *file, unsigned int line);

    Page *availablePages, *unavailablePages;
    int availablePagesSize, unavailablePagesSize;
    int memoryPoolPageSize;
};

template<class MemoryBlockType>
MemoryPool<MemoryBlockType>::MemoryPool()
{
#ifndef _DISABLE_MEMORY_POOL
    availablePagesSize=0;
    unavailablePagesSize=0;
    memoryPoolPageSize=16384;
#endif
}
template<class MemoryBlockType>
MemoryPool<MemoryBlockType>::~MemoryPool()
{
#ifndef _DISABLE_MEMORY_POOL
    Clear(_FILE_AND_LINE_);
#endif
}

template<class MemoryBlockType>
void MemoryPool<MemoryBlockType>::SetPageSize(int size)
{
    memoryPoolPageSize=size;
}

template<class MemoryBlockType>
MemoryBlockType* MemoryPool<MemoryBlockType>::Allocate(const char *file, unsigned int line)
{
#ifdef _DISABLE_MEMORY_POOL
    return (MemoryBlockType*) rakMalloc_Ex(sizeof(MemoryBlockType), file, line);
#else

    if (availablePagesSize>0)
    {
        MemoryBlockType *retVal;
        Page *curPage;
        curPage=availablePages;
        retVal = (MemoryBlockType*) curPage->availableStack[--(curPage->availableStackSize)];
        if (curPage->availableStackSize==0)
        {
            --availablePagesSize;
            availablePages=curPage->next;
            VEAssert(availablePagesSize==0 || availablePages->availableStackSize>0);
            curPage->next->prev=curPage->prev;
            curPage->prev->next=curPage->next;

            if (unavailablePagesSize++==0)
            {
                unavailablePages=curPage;
                curPage->next=curPage;
                curPage->prev=curPage;
            }
            else
            {
                curPage->next=unavailablePages;
                curPage->prev=unavailablePages->prev;
                unavailablePages->prev->next=curPage;
                unavailablePages->prev=curPage;
            }
        }

        VEAssert(availablePagesSize==0 || availablePages->availableStackSize>0);
        return retVal;
    }

    availablePages = (Page *) rakMalloc_Ex(sizeof(Page), file, line);
    if (availablePages==0)
        return 0;
    availablePagesSize=1;
    if (InitPage(availablePages, availablePages, file, line)==false)
        return 0;
    VEAssert(availablePages->availableStackSize>1);

    return (MemoryBlockType *) availablePages->availableStack[--availablePages->availableStackSize];
#endif
}
template<class MemoryBlockType>
void MemoryPool<MemoryBlockType>::Release(MemoryBlockType *m, const char *file, unsigned int line)
{
#ifdef _DISABLE_MEMORY_POOL
    veFree_Ex(m, file, line);
    return;
#else
    Page *curPage;
    MemoryWithPage *memoryWithPage = (MemoryWithPage*)m;
    curPage=memoryWithPage->parentPage;

    if (curPage->availableStackSize==0)
    {
        curPage->availableStack[curPage->availableStackSize++]=memoryWithPage;
        unavailablePagesSize--;

        curPage->next->prev=curPage->prev;
        curPage->prev->next=curPage->next;

        if (unavailablePagesSize>0 && curPage==unavailablePages)
            unavailablePages=unavailablePages->next;

        if (availablePagesSize++==0)
        {
            availablePages=curPage;
            curPage->next=curPage;
            curPage->prev=curPage;
        }
        else
        {
            curPage->next=availablePages;
            curPage->prev=availablePages->prev;
            availablePages->prev->next=curPage;
            availablePages->prev=curPage;
        }
    }
    else
    {
        curPage->availableStack[curPage->availableStackSize++]=memoryWithPage;

        if (curPage->availableStackSize==BlocksPerPage() &&
                availablePagesSize>=DS_MEMORY_POOL_MAX_FREE_PAGES)
        {
            if (curPage==availablePages)
            {
                availablePages=curPage->next;
                VEAssert(availablePages->availableStackSize>0);
            }
            curPage->prev->next=curPage->next;
            curPage->next->prev=curPage->prev;
            availablePagesSize--;
            veFree_Ex(curPage->availableStack, file, line );
            veFree_Ex(curPage->block, file, line );
            veFree_Ex(curPage, file, line );
        }
    }
#endif
}
template<class MemoryBlockType>
void MemoryPool<MemoryBlockType>::Clear(const char *file, unsigned int line)
{
#ifdef _DISABLE_MEMORY_POOL
    return;
#else
    Page *cur, *freed;

    if (availablePagesSize>0)
    {
        cur = availablePages;
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
        while (true)
        {
            veFree_Ex(cur->availableStack, file, line );
            veFree_Ex(cur->block, file, line );
            freed=cur;
            cur=cur->next;
            if (cur==availablePages)
            {
                veFree_Ex(freed, file, line );
                break;
            }
            veFree_Ex(freed, file, line );
        }
    }

    if (unavailablePagesSize>0)
    {
        cur = unavailablePages;
        while (1)
        {
            veFree_Ex(cur->availableStack, file, line );
            veFree_Ex(cur->block, file, line );
            freed=cur;
            cur=cur->next;
            if (cur==unavailablePages)
            {
                veFree_Ex(freed, file, line );
                break;
            }
            veFree_Ex(freed, file, line );
        }
    }

    availablePagesSize=0;
    unavailablePagesSize=0;
#endif
}
template<class MemoryBlockType>
int MemoryPool<MemoryBlockType>::BlocksPerPage(void) const
{
    return memoryPoolPageSize / sizeof(MemoryWithPage);
}
template<class MemoryBlockType>
bool MemoryPool<MemoryBlockType>::InitPage(Page *page, Page *prev, const char *file, unsigned int line)
{
    int i=0;
    const int bpp = BlocksPerPage();
    page->block=(MemoryWithPage*) rakMalloc_Ex(memoryPoolPageSize, file, line);
    if (page->block==0)
        return false;
    page->availableStack=(MemoryWithPage**)rakMalloc_Ex(sizeof(MemoryWithPage*)*bpp, file, line);
    if (page->availableStack==0)
    {
        veFree_Ex(page->block, file, line );
        return false;
    }
    MemoryWithPage *curBlock = page->block;
    MemoryWithPage **curStack = page->availableStack;
    while (i < bpp)
    {
        curBlock->parentPage=page;
        curStack[i]=curBlock++;
        i++;
    }
    page->availableStackSize=bpp;
    page->next=availablePages;
    page->prev=prev;
    return true;
}
}
