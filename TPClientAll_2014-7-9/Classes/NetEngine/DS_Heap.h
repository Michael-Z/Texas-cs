////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_Heap.h
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
#include "DS_List.h"
#include "Export.h"
#include "VEAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

namespace DataStructures
{
template <class weight_type, class data_type, bool isMaxHeap>
class VE_DLL_EXPORT Heap
{
public:
    struct HeapNode
    {
        HeapNode() {}
        HeapNode(const weight_type &w, const data_type &d) : weight(w), data(d) {}
        weight_type weight;
        data_type data;
    };

    Heap();
    ~Heap();
    void Push(const weight_type &weight, const data_type &data, const char *file, unsigned int line);
    void StartSeries(void)
    {
        optimizeNextSeriesPush=false;
    }
    void PushSeries(const weight_type &weight, const data_type &data, const char *file, unsigned int line);
    data_type Pop(const unsigned startingIndex);
    data_type Peek(const unsigned startingIndex=0) const;
    weight_type PeekWeight(const unsigned startingIndex=0) const;
    void Clear(bool doNotDeallocateSmallBlocks, const char *file, unsigned int line);
    data_type& operator[] ( const unsigned int position ) const;
    unsigned Size(void) const;

protected:
    unsigned LeftChild(const unsigned i) const;
    unsigned RightChild(const unsigned i) const;
    unsigned Parent(const unsigned i) const;
    void Swap(const unsigned i, const unsigned j);
    DataStructures::List<HeapNode> heap;
    bool optimizeNextSeriesPush;
};

template  <class weight_type, class data_type, bool isMaxHeap>
Heap<weight_type, data_type, isMaxHeap>::Heap()
{
    optimizeNextSeriesPush=false;
}

template  <class weight_type, class data_type, bool isMaxHeap>
Heap<weight_type, data_type, isMaxHeap>::~Heap()
{
}

template  <class weight_type, class data_type, bool isMaxHeap>
void Heap<weight_type, data_type, isMaxHeap>::PushSeries(const weight_type &weight, const data_type &data, const char *file, unsigned int line)
{
    if (optimizeNextSeriesPush==false)
    {
        unsigned currentIndex = heap.Size();
        unsigned parentIndex;
        if (currentIndex>0)
        {
            for (parentIndex = Parent(currentIndex); parentIndex < currentIndex; parentIndex++)
            {
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
                if (isMaxHeap)
                {
                    if (weight>heap[parentIndex].weight)
                    {
                        Push(weight,data,file,line);
                        return;
                    }
                }
                else
                {
                    if (weight<heap[parentIndex].weight)
                    {
                        Push(weight,data,file,line);
                        return;
                    }
                }
            }
        }

        heap.Insert(HeapNode(weight, data), file, line);
        optimizeNextSeriesPush=true;
    }
    else
    {
        heap.Insert(HeapNode(weight, data), file, line);
    }
}

template  <class weight_type, class data_type, bool isMaxHeap>
void Heap<weight_type, data_type, isMaxHeap>::Push(const weight_type &weight, const data_type &data, const char *file, unsigned int line)
{
    unsigned currentIndex = heap.Size();
    unsigned parentIndex;
    heap.Insert(HeapNode(weight, data), file, line);
    while (currentIndex!=0)
    {
        parentIndex = Parent(currentIndex);
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
        if (isMaxHeap)
        {
            if (heap[parentIndex].weight < weight)
            {
                Swap(currentIndex, parentIndex);
                currentIndex=parentIndex;
            }
            else
                break;
        }
        else
        {
            if (heap[parentIndex].weight > weight)
            {
                Swap(currentIndex, parentIndex);
                currentIndex=parentIndex;
            }
            else
                break;
        }
    }
}

template  <class weight_type, class data_type, bool isMaxHeap>
data_type Heap<weight_type, data_type, isMaxHeap>::Pop(const unsigned startingIndex)
{
    data_type returnValue=heap[startingIndex].data;

    heap[startingIndex]=heap[heap.Size()-1];

    unsigned currentIndex,leftChild,rightChild;
    weight_type currentWeight;
    currentIndex=startingIndex;
    currentWeight=heap[startingIndex].weight;
    heap.RemoveFromEnd();

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
    while (1)
    {
        leftChild=LeftChild(currentIndex);
        rightChild=RightChild(currentIndex);
        if (leftChild >= heap.Size())
        {
            return returnValue;
        }
        if (rightChild >= heap.Size())
        {
            if ((isMaxHeap==true && currentWeight < heap[leftChild].weight) ||
                    (isMaxHeap==false && currentWeight > heap[leftChild].weight))
                Swap(leftChild, currentIndex);

            return returnValue;
        }
        else
        {
            if (isMaxHeap)
            {
                if (heap[leftChild].weight <= currentWeight && heap[rightChild].weight <= currentWeight)
                    return returnValue;

                if (heap[leftChild].weight > heap[rightChild].weight)
                {
                    Swap(leftChild, currentIndex);
                    currentIndex=leftChild;
                }
                else
                {
                    Swap(rightChild, currentIndex);
                    currentIndex=rightChild;
                }
            }
            else
            {
                if (heap[leftChild].weight >= currentWeight && heap[rightChild].weight >= currentWeight)
                    return returnValue;

                if (heap[leftChild].weight < heap[rightChild].weight)
                {
                    Swap(leftChild, currentIndex);
                    currentIndex=leftChild;
                }
                else
                {
                    Swap(rightChild, currentIndex);
                    currentIndex=rightChild;
                }
            }
        }
    }
}

template  <class weight_type, class data_type, bool isMaxHeap>
inline data_type Heap<weight_type, data_type, isMaxHeap>::Peek(const unsigned startingIndex) const
{
    return heap[startingIndex].data;
}

template  <class weight_type, class data_type, bool isMaxHeap>
inline weight_type Heap<weight_type, data_type, isMaxHeap>::PeekWeight(const unsigned startingIndex) const
{
    return heap[startingIndex].weight;
}

template  <class weight_type, class data_type, bool isMaxHeap>
void Heap<weight_type, data_type, isMaxHeap>::Clear(bool doNotDeallocateSmallBlocks, const char *file, unsigned int line)
{
    heap.Clear(doNotDeallocateSmallBlocks, file, line);
}

template <class weight_type, class data_type, bool isMaxHeap>
inline data_type& Heap<weight_type, data_type, isMaxHeap>::operator[] ( const unsigned int position ) const
{
    return heap[position].data;
}
template <class weight_type, class data_type, bool isMaxHeap>
unsigned Heap<weight_type, data_type, isMaxHeap>::Size(void) const
{
    return heap.Size();
}

template <class weight_type, class data_type, bool isMaxHeap>
inline unsigned Heap<weight_type, data_type, isMaxHeap>::LeftChild(const unsigned i) const
{
    return i*2+1;
}

template <class weight_type, class data_type, bool isMaxHeap>
inline unsigned Heap<weight_type, data_type, isMaxHeap>::RightChild(const unsigned i) const
{
    return i*2+2;
}

template <class weight_type, class data_type, bool isMaxHeap>
inline unsigned Heap<weight_type, data_type, isMaxHeap>::Parent(const unsigned i) const
{
#ifdef _DEBUG
    VEAssert(i!=0);
#endif
    return (i-1)/2;
}

template <class weight_type, class data_type, bool isMaxHeap>
void Heap<weight_type, data_type, isMaxHeap>::Swap(const unsigned i, const unsigned j)
{
    HeapNode temp;
    temp=heap[i];
    heap[i]=heap[j];
    heap[j]=temp;
}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
