////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_RangeList.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DS_OrderedList.h"
#include "BitStream.h"
#include "VEMemoryOverride.h"
#include "VEAssert.h"

namespace DataStructures
{
template <class range_type>
struct RangeNode
{
    RangeNode() {}
    ~RangeNode() {}
    RangeNode(range_type min, range_type max)
    {
        minIndex=min;
        maxIndex=max;
    }
    range_type minIndex;
    range_type maxIndex;
};


template <class range_type>
int RangeNodeComp(const range_type &a, const RangeNode<range_type> &b)
{
    if (a<b.minIndex)
        return -1;
    if (a==b.minIndex)
        return 0;
    return 1;
}

template <class range_type>
class VE_DLL_EXPORT RangeList
{
public:
    RangeList();
    ~RangeList();
    void Insert(range_type index);
    void Clear(void);
    unsigned Size(void) const;
    unsigned RangeSum(void) const;
    VENet::BitSize_t Serialize(VENet::BitStream *in, VENet::BitSize_t maxBits, bool clearSerialized);
    bool Deserialize(VENet::BitStream *out);

    DataStructures::OrderedList<range_type, RangeNode<range_type> , RangeNodeComp<range_type> > ranges;
};

template <class range_type>
VENet::BitSize_t RangeList<range_type>::Serialize(VENet::BitStream *in, VENet::BitSize_t maxBits, bool clearSerialized)
{
    VEAssert(ranges.Size() < (unsigned short)-1);
    VENet::BitStream tempBS;
    VENet::BitSize_t bitsWritten;
    unsigned short countWritten;
    unsigned i;
    countWritten=0;
    bitsWritten=0;
    for (i=0; i < ranges.Size(); i++)
    {
        if ((int)sizeof(unsigned short)*8+bitsWritten+(int)sizeof(range_type)*8*2+1>maxBits)
            break;
        unsigned char minEqualsMax;
        if (ranges[i].minIndex==ranges[i].maxIndex)
            minEqualsMax=1;
        else
            minEqualsMax=0;
        tempBS.Write(minEqualsMax);
        tempBS.Write(ranges[i].minIndex);
        bitsWritten+=sizeof(range_type)*8+8;
        if (ranges[i].minIndex!=ranges[i].maxIndex)
        {
            tempBS.Write(ranges[i].maxIndex);
            bitsWritten+=sizeof(range_type)*8;
        }
        countWritten++;
    }

    in->AlignWriteToByteBoundary();
    VENet::BitSize_t before=in->GetWriteOffset();
    in->Write(countWritten);
    bitsWritten+=in->GetWriteOffset()-before;
    in->Write(&tempBS, tempBS.GetNumberOfBitsUsed());

    if (clearSerialized && countWritten)
    {
        unsigned rangeSize=ranges.Size();
        for (i=0; i < rangeSize-countWritten; i++)
        {
            ranges[i]=ranges[i+countWritten];
        }
        ranges.RemoveFromEnd(countWritten);
    }

    return bitsWritten;
}
template <class range_type>
bool RangeList<range_type>::Deserialize(VENet::BitStream *out)
{
    ranges.Clear(true, _FILE_AND_LINE_);
    unsigned short count;
    out->AlignReadToByteBoundary();
    out->Read(count);
    unsigned short i;
    range_type min,max;
    unsigned char maxEqualToMin=0;

    for (i=0; i < count; i++)
    {
        out->Read(maxEqualToMin);
        if (out->Read(min)==false)
            return false;
        if (maxEqualToMin==false)
        {
            if (out->Read(max)==false)
                return false;
            if (max<min)
                return false;
        }
        else
            max=min;


        ranges.InsertAtEnd(RangeNode<range_type>(min,max), _FILE_AND_LINE_);
    }
    return true;
}

template <class range_type>
RangeList<range_type>::RangeList()
{
    RangeNodeComp<range_type>(0, RangeNode<range_type>());
}

template <class range_type>
RangeList<range_type>::~RangeList()
{
    Clear();
}

template <class range_type>
void RangeList<range_type>::Insert(range_type index)
{
    if (ranges.Size()==0)
    {
        ranges.Insert(index, RangeNode<range_type>(index, index), true, _FILE_AND_LINE_);
        return;
    }

    bool objectExists;
    unsigned insertionIndex=ranges.GetIndexFromKey(index, &objectExists);
    if (insertionIndex==ranges.Size())
    {
        if (index == ranges[insertionIndex-1].maxIndex+(range_type)1)
            ranges[insertionIndex-1].maxIndex++;
        else if (index > ranges[insertionIndex-1].maxIndex+(range_type)1)
        {
            ranges.Insert(index, RangeNode<range_type>(index, index), true, _FILE_AND_LINE_);
        }

        return;
    }

    if (index < ranges[insertionIndex].minIndex-(range_type)1)
    {
        ranges.InsertAtIndex(RangeNode<range_type>(index, index), insertionIndex, _FILE_AND_LINE_);

        return;
    }
    else if (index == ranges[insertionIndex].minIndex-(range_type)1)
    {
        ranges[insertionIndex].minIndex--;
        if (insertionIndex>0 && ranges[insertionIndex-1].maxIndex+(range_type)1==ranges[insertionIndex].minIndex)
        {
            ranges[insertionIndex-1].maxIndex=ranges[insertionIndex].maxIndex;
            ranges.RemoveAtIndex(insertionIndex);
        }

        return;
    }
    else if (index >= ranges[insertionIndex].minIndex && index <= ranges[insertionIndex].maxIndex)
    {
        return;
    }
    else if (index == ranges[insertionIndex].maxIndex+(range_type)1)
    {
        ranges[insertionIndex].maxIndex++;
        if (insertionIndex<ranges.Size()-1 && ranges[insertionIndex+(range_type)1].minIndex==ranges[insertionIndex].maxIndex+(range_type)1)
        {
            ranges[insertionIndex+1].minIndex=ranges[insertionIndex].minIndex;
            ranges.RemoveAtIndex(insertionIndex);
        }

        return;
    }
}

template <class range_type>
void RangeList<range_type>::Clear(void)
{
    ranges.Clear(true, _FILE_AND_LINE_);
}

template <class range_type>
unsigned RangeList<range_type>::Size(void) const
{
    return ranges.Size();
}

template <class range_type>
unsigned RangeList<range_type>::RangeSum(void) const
{
    unsigned sum=0,i;
    for (i=0; i < ranges.Size(); i++)
        sum+=ranges[i].maxIndex-ranges[i].minIndex+1;
    return sum;
}

}
