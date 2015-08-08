////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_OrderedChannelHeap.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DS_Heap.h"
#include "DS_Map.h"
#include "DS_Queue.h"
#include "Export.h"
#include "VEAssert.h"
#include "Rand.h"

namespace DataStructures
{
template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)=defaultMapKeyComparison<channel_key_type> >
class VE_DLL_EXPORT OrderedChannelHeap
{
public:
    static void IMPLEMENT_DEFAULT_COMPARISON(void)
    {
        DataStructures::defaultMapKeyComparison<channel_key_type>(channel_key_type(),channel_key_type());
    }

    OrderedChannelHeap();
    ~OrderedChannelHeap();
    void Push(const channel_key_type &channelID, const heap_data_type &data);
    void PushAtHead(const unsigned index, const channel_key_type &channelID, const heap_data_type &data);
    heap_data_type Pop(const unsigned startingIndex=0);
    heap_data_type Peek(const unsigned startingIndex) const;
    void AddChannel(const channel_key_type &channelID, const double weight);
    void RemoveChannel(channel_key_type channelID);
    void Clear(void);
    heap_data_type& operator[] ( const unsigned int position ) const;
    unsigned ChannelSize(const channel_key_type &channelID);
    unsigned Size(void) const;

    struct QueueAndWeight
    {
        DataStructures::Queue<double> randResultQueue;
        double weight;
        bool signalDeletion;
    };

    struct HeapChannelAndData
    {
        HeapChannelAndData() {}
        HeapChannelAndData(const channel_key_type &_channel, const heap_data_type &_data) : data(_data), channel(_channel) {}
        heap_data_type data;
        channel_key_type channel;
    };

protected:
    DataStructures::Map<channel_key_type, QueueAndWeight*, channel_key_comparison_func> map;
    DataStructures::Heap<double, HeapChannelAndData, true> heap;
    void GreatestRandResult(void);
};

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::OrderedChannelHeap()
{
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::~OrderedChannelHeap()
{
    Clear();
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Push(const channel_key_type &channelID, const heap_data_type &data)
{
    PushAtHead(MAX_UNSIGNED_LONG, channelID, data);
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::GreatestRandResult(void)
{
    double greatest;
    unsigned i;
    greatest=0.0;
    for (i=0; i < map.Size(); i++)
    {
        if (map[i]->randResultQueue.Size() && map[i]->randResultQueue[0]>greatest)
            greatest=map[i]->randResultQueue[0];
    }
    return greatest;
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::PushAtHead(const unsigned index, const channel_key_type &channelID, const heap_data_type &data)
{
    QueueAndWeight *queueAndWeight=map.Get(channelID);
    double maxRange, minRange, rnd;
    if (queueAndWeight->randResultQueue.Size()==0)
    {
        maxRange=GreatestRandResult();
        if (maxRange==0.0)
            maxRange=1.0;
        minRange=0.0;
    }
    else if (index >= queueAndWeight->randResultQueue.Size())
    {
        maxRange=queueAndWeight->randResultQueue[queueAndWeight->randResultQueue.Size()-1]*.99999999;
        minRange=0.0;
    }
    else
    {
        if (index==0)
        {
            maxRange=GreatestRandResult();
            if (maxRange==queueAndWeight->randResultQueue[0])
                maxRange=1.0;
        }
        else if (index >= queueAndWeight->randResultQueue.Size())
            maxRange=queueAndWeight->randResultQueue[queueAndWeight->randResultQueue.Size()-1]*.99999999;
        else
            maxRange=queueAndWeight->randResultQueue[index-1]*.99999999;

        minRange=maxRange=queueAndWeight->randResultQueue[index]*1.00000001;
    }

#ifdef _DEBUG
    VEAssert(maxRange!=0.0);
#endif
    rnd=frandomMT() * (maxRange - minRange);
    if (rnd==0.0)
        rnd=maxRange/2.0;

    if (index >= queueAndWeight->randResultQueue.Size())
        queueAndWeight->randResultQueue.Push(rnd);
    else
        queueAndWeight->randResultQueue.PushAtHead(rnd, index);

    heap.Push(rnd*queueAndWeight->weight, HeapChannelAndData(channelID, data));
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
heap_data_type OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Pop(const unsigned startingIndex)
{
    VEAssert(startingIndex < heap.Size());

    QueueAndWeight *queueAndWeight=map.Get(heap[startingIndex].channel);
    if (startingIndex!=0)
    {
        unsigned indiceCount=0;
        unsigned i;
        for (i=0; i < startingIndex; i++)
            if (channel_key_comparison_func(heap[i].channel,heap[startingIndex].channel)==0)
                indiceCount++;
        queueAndWeight->randResultQueue.RemoveAtIndex(indiceCount);
    }
    else
    {
        queueAndWeight->randResultQueue.Pop();
    }

    if (queueAndWeight->signalDeletion)
        RemoveChannel(heap[startingIndex].channel);

    return heap.Pop(startingIndex).data;
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
heap_data_type OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Peek(const unsigned startingIndex) const
{
    HeapChannelAndData heapChannelAndData = heap.Peek(startingIndex);
    return heapChannelAndData.data;
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::AddChannel(const channel_key_type &channelID, const double weight)
{
    QueueAndWeight *qaw = VENet::OP_NEW<QueueAndWeight>( _FILE_AND_LINE_ );
    qaw->weight=weight;
    qaw->signalDeletion=false;
    map.SetNew(channelID, qaw);
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::RemoveChannel(channel_key_type channelID)
{
    if (map.Has(channelID))
    {
        unsigned i;
        i=map.GetIndexAtKey(channelID);
        if (map[i]->randResultQueue.Size()==0)
        {
            VENet::OP_DELETE(map[i], _FILE_AND_LINE_);
            map.RemoveAtIndex(i);
        }
        else
        {
            map[i]->signalDeletion=true;
        }
    }
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
unsigned OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Size(void) const
{
    return heap.Size();
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
heap_data_type& OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::operator[]( const unsigned int position ) const
{
    return heap[position].data;
}


template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
unsigned OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::ChannelSize(const channel_key_type &channelID)
{
    QueueAndWeight *queueAndWeight=map.Get(channelID);
    return queueAndWeight->randResultQueue.Size();
}

template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Clear(void)
{
    unsigned i;
    for (i=0; i < map.Size(); i++)
        VENet::OP_DELETE(map[i], _FILE_AND_LINE_);
    map.Clear(_FILE_AND_LINE_);
    heap.Clear(_FILE_AND_LINE_);
}
}
