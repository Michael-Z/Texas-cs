////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_OrderedList.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#include "DS_List.h"
#include "VEMemoryOverride.h"
#include "Export.h"

#pragma once

namespace DataStructures
{
template <class key_type, class data_type>
int defaultOrderedListComparison(const key_type &a, const data_type &b)
{
    if (a<b) return -1;
    if (a==b) return 0;
    return 1;
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)=defaultOrderedListComparison<key_type, data_type> >
class VE_DLL_EXPORT OrderedList
{
public:
    static void IMPLEMENT_DEFAULT_COMPARISON(void)
    {
        DataStructures::defaultOrderedListComparison<key_type, data_type>(key_type(),data_type());
    }

    OrderedList();
    ~OrderedList();
    OrderedList( const OrderedList& original_copy );
    OrderedList& operator= ( const OrderedList& original_copy );

    bool HasData(const key_type &key, int (*cf)(const key_type&, const data_type&)=default_comparison_function) const;
    unsigned GetIndexFromKey(const key_type &key, bool *objectExists, int (*cf)(const key_type&, const data_type&)=default_comparison_function) const;
    data_type GetElementFromKey(const key_type &key, int (*cf)(const key_type&, const data_type&)=default_comparison_function) const;
    bool GetElementFromKey(const key_type &key, data_type &element, int (*cf)(const key_type&, const data_type&)=default_comparison_function) const;
    unsigned Insert(const key_type &key, const data_type &data, bool assertOnDuplicate, const char *file, unsigned int line, int (*cf)(const key_type&, const data_type&)=default_comparison_function);
    unsigned Remove(const key_type &key, int (*cf)(const key_type&, const data_type&)=default_comparison_function);
    unsigned RemoveIfExists(const key_type &key, int (*cf)(const key_type&, const data_type&)=default_comparison_function);
    data_type& operator[] ( const unsigned int position ) const;
    void RemoveAtIndex(const unsigned index);
    void InsertAtIndex(const data_type &data, const unsigned index, const char *file, unsigned int line);
    void InsertAtEnd(const data_type &data, const char *file, unsigned int line);
    void RemoveFromEnd(const unsigned num=1);
    void Clear(bool doNotDeallocate, const char *file, unsigned int line);
    unsigned Size(void) const;

protected:
    DataStructures::List<data_type> orderedList;
};

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
OrderedList<key_type, data_type, default_comparison_function>::OrderedList()
{
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
OrderedList<key_type, data_type, default_comparison_function>::~OrderedList()
{
    Clear(false, _FILE_AND_LINE_);
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
OrderedList<key_type, data_type, default_comparison_function>::OrderedList( const OrderedList& original_copy )
{
    orderedList=original_copy.orderedList;
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
OrderedList<key_type, data_type, default_comparison_function>& OrderedList<key_type, data_type, default_comparison_function>::operator= ( const OrderedList& original_copy )
{
    orderedList=original_copy.orderedList;
    return *this;
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
bool OrderedList<key_type, data_type, default_comparison_function>::HasData(const key_type &key, int (*cf)(const key_type&, const data_type&)) const
{
    bool objectExists;
    GetIndexFromKey(key, &objectExists, cf);
    return objectExists;
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
data_type OrderedList<key_type, data_type, default_comparison_function>::GetElementFromKey(const key_type &key, int (*cf)(const key_type&, const data_type&)) const
{
    bool objectExists;
    unsigned index;
    index = GetIndexFromKey(key, &objectExists, cf);
    VEAssert(objectExists);
    return orderedList[index];
}
template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
bool OrderedList<key_type, data_type, default_comparison_function>::GetElementFromKey(const key_type &key, data_type &element, int (*cf)(const key_type&, const data_type&)) const
{
    bool objectExists;
    unsigned index;
    index = GetIndexFromKey(key, &objectExists, cf);
    if (objectExists)
        element = orderedList[index];
    return objectExists;
}
template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
unsigned OrderedList<key_type, data_type, default_comparison_function>::GetIndexFromKey(const key_type &key, bool *objectExists, int (*cf)(const key_type&, const data_type&)) const
{
    int index, upperBound, lowerBound;
    int res;

    if (orderedList.Size()==0)
    {
        *objectExists=false;
        return 0;
    }

    upperBound=(int)orderedList.Size()-1;
    lowerBound=0;
    index = (int)orderedList.Size()/2;

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
    while (1)
    {
        res = cf(key,orderedList[index]);
        if (res==0)
        {
            *objectExists=true;
            return index;
        }
        else if (res<0)
        {
            upperBound=index-1;
        }
        else
        {
            lowerBound=index+1;
        }

        index=lowerBound+(upperBound-lowerBound)/2;

        if (lowerBound>upperBound)
        {
            *objectExists=false;
            return lowerBound;
        }
    }
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
unsigned OrderedList<key_type, data_type, default_comparison_function>::Insert(const key_type &key, const data_type &data, bool assertOnDuplicate, const char *file, unsigned int line, int (*cf)(const key_type&, const data_type&))
{
    (void) assertOnDuplicate;
    bool objectExists;
    unsigned index;
    index = GetIndexFromKey(key, &objectExists, cf);

    if (objectExists)
    {
        VEAssert(assertOnDuplicate==false);
        return (unsigned)-1;
    }

    if (index>=orderedList.Size())
    {
        orderedList.Insert(data, file, line);
        return orderedList.Size()-1;
    }
    else
    {
        orderedList.Insert(data,index, file, line);
        return index;
    }
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
unsigned OrderedList<key_type, data_type, default_comparison_function>::Remove(const key_type &key, int (*cf)(const key_type&, const data_type&))
{
    bool objectExists;
    unsigned index;
    index = GetIndexFromKey(key, &objectExists, cf);

    if (objectExists==false)
    {
        VEAssert(objectExists==true);
        return 0;
    }

    orderedList.RemoveAtIndex(index);
    return index;
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
unsigned OrderedList<key_type, data_type, default_comparison_function>::RemoveIfExists(const key_type &key, int (*cf)(const key_type&, const data_type&))
{
    bool objectExists;
    unsigned index;
    index = GetIndexFromKey(key, &objectExists, cf);

    if (objectExists==false)
        return 0;

    orderedList.RemoveAtIndex(index);
    return index;
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
void OrderedList<key_type, data_type, default_comparison_function>::RemoveAtIndex(const unsigned index)
{
    orderedList.RemoveAtIndex(index);
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
void OrderedList<key_type, data_type, default_comparison_function>::InsertAtIndex(const data_type &data, const unsigned index, const char *file, unsigned int line)
{
    orderedList.Insert(data, index, file, line);
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
void OrderedList<key_type, data_type, default_comparison_function>::InsertAtEnd(const data_type &data, const char *file, unsigned int line)
{
    orderedList.Insert(data, file, line);
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
void OrderedList<key_type, data_type, default_comparison_function>::RemoveFromEnd(const unsigned num)
{
    orderedList.RemoveFromEnd(num);
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
void OrderedList<key_type, data_type, default_comparison_function>::Clear(bool doNotDeallocate, const char *file, unsigned int line)
{
    orderedList.Clear(doNotDeallocate, file, line);
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
data_type& OrderedList<key_type, data_type, default_comparison_function>::operator[]( const unsigned int position ) const
{
    return orderedList[position];
}

template <class key_type, class data_type, int (*default_comparison_function)(const key_type&, const data_type&)>
unsigned OrderedList<key_type, data_type, default_comparison_function>::Size(void) const
{
    return orderedList.Size();
}
}
