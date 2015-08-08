////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_Multilist.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VEAssert.h"
#include <string.h>
#include "Export.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"


#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4512 )
#endif

enum MultilistType
{
    ML_UNORDERED_LIST,
    ML_STACK,
    ML_QUEUE,
    ML_ORDERED_LIST,
    ML_VARIABLE_DURING_RUNTIME
};

namespace DataStructures
{
template <class templateType>
void DeletePtr_VENet(templateType &ptr, const char *file, unsigned int line )
{
    VENet::OP_DELETE(ptr, file, line);
}

template <class templateType>
void DeletePtr(templateType &ptr)
{
    delete ptr;
}

template < class templateType >
class MLKeyRef
{
public:
    MLKeyRef(const templateType& input) : val(input) {}
    const templateType &Get(void) const
    {
        return val;
    }
    bool operator<( const templateType &right )
    {
        return val < right;
    }
    bool operator>( const templateType &right )
    {
        return val > right;
    }
    bool operator==( const templateType &right )
    {
        return val == right;
    }
protected:
    const templateType &val;
};

#define DEFINE_MULTILIST_PTR_TO_MEMBER_COMPARISONS( _CLASS_NAME_, _KEY_TYPE_, _MEMBER_VARIABLE_NAME_ ) \
	bool operator<( const DataStructures::MLKeyRef<_KEY_TYPE_> &inputKey, const _CLASS_NAME_ *cls ) {return inputKey.Get() < cls->_MEMBER_VARIABLE_NAME_;} \
	bool operator>( const DataStructures::MLKeyRef<_KEY_TYPE_> &inputKey, const _CLASS_NAME_ *cls ) {return inputKey.Get() > cls->_MEMBER_VARIABLE_NAME_;} \
	bool operator==( const DataStructures::MLKeyRef<_KEY_TYPE_> &inputKey, const _CLASS_NAME_ *cls ) {return inputKey.Get() == cls->_MEMBER_VARIABLE_NAME_;}

typedef uint32_t DefaultIndexType;

template <const MultilistType _MultilistType, class _DataType, class _KeyType=_DataType, class _IndexType=DefaultIndexType>
class VE_DLL_EXPORT Multilist
{
public:
    Multilist();
    ~Multilist();
    Multilist( const Multilist& source );
    Multilist& operator= ( const Multilist& source );
    _DataType& operator[] ( const _IndexType position ) const;
    void Push(const _DataType &d, const char *file=__FILE__, unsigned int line=__LINE__ );
    void Push(const _DataType &d, const _KeyType &key, const char *file=__FILE__, unsigned int line=__LINE__ );

    _DataType &Pop(const char *file=__FILE__, unsigned int line=__LINE__);
    _DataType &Peek(void) const;

    void PushOpposite(const _DataType &d, const char *file=__FILE__, unsigned int line=__LINE__ );
    void PushOpposite(const _DataType &d, const _KeyType &key, const char *file=__FILE__, unsigned int line=__LINE__ );

    _DataType &PopOpposite(const char *file=__FILE__, unsigned int line=__LINE__);
    _DataType &PeekOpposite(void) const;

    void InsertAtIndex(const _DataType &d, _IndexType index, const char *file=__FILE__, unsigned int line=__LINE__);

    void RemoveAtIndex(_IndexType position, const char *file=__FILE__, unsigned int line=__LINE__);

    bool RemoveAtKey(_KeyType key, bool assertIfDoesNotExist, const char *file=__FILE__, unsigned int line=__LINE__);

    _IndexType GetIndexOf(_KeyType key) const;

    _IndexType GetInsertionIndex(_KeyType key) const;

    _DataType GetPtr(_KeyType key) const;

    void ForEach(void (*func)(_DataType &item, const char *file, unsigned int line), const char *file, unsigned int line);
    void ForEach(void (*func)(_DataType &item));

    bool IsEmpty(void) const;

    _IndexType GetSize(void) const;

    void Clear( bool deallocateSmallBlocks=true, const char *file=__FILE__, unsigned int line=__LINE__ );

    void ClearPointers( bool deallocateSmallBlocks=true, const char *file=__FILE__, unsigned int line=__LINE__ );

    void ClearPointer( _KeyType key, const char *file=__FILE__, unsigned int line=__LINE__ );

    void ReverseList(void);

    void Reallocate(_IndexType size, const char *file=__FILE__, unsigned int line=__LINE__);

    void Sort(bool force);

    void TagSorted(void);

    void SetSortOrder(bool ascending);

    bool GetSortOrder(void) const;

    bool IsSorted(void) const;

    MultilistType GetMultilistType(void) const;

    void SetMultilistType(MultilistType newType);

    static void FindIntersection(
        Multilist& source1,
        Multilist& source2,
        Multilist& intersection,
        Multilist& uniqueToSource1,
        Multilist& uniqueToSource2);

protected:
    void ReallocateIfNeeded(const char *file, unsigned int line);
    void DeallocateIfNeeded(const char *file, unsigned int line);
    void ReallocToSize(_IndexType newAllocationSize, const char *file, unsigned int line);
    void ReverseListInternal(void);
    void InsertInOrderedList(const _DataType &d, const _KeyType &key);
    _IndexType GetIndexFromKeyInSortedList(const _KeyType &key, bool *objectExists) const;
    void InsertShiftArrayRight(const _DataType &d, _IndexType index);
    void DeleteShiftArrayLeft(_IndexType index);
    void QSortAscending(_IndexType left, _IndexType right);
    void QSortDescending(_IndexType left, _IndexType right);
    void CopySource( const Multilist& source );

    _DataType* data;

    _IndexType dataSize;

    _IndexType allocationSize;

    _IndexType queueHead;

    _IndexType queueTail;

    _IndexType preallocationSize;

    enum
    {
        ML_UNSORTED,
        ML_SORTED_ASCENDING,
        ML_SORTED_DESCENDING
    } sortState;

    bool ascendingSort;

    MultilistType variableMultilistType;
};

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Multilist()
{
    data=0;
    dataSize=0;
    allocationSize=0;
    ascendingSort=true;
    sortState=ML_UNSORTED;
    queueHead=0;
    queueTail=0;
    preallocationSize=0;

    if (_MultilistType==ML_ORDERED_LIST)
        sortState=ML_SORTED_ASCENDING;
    else
        sortState=ML_UNSORTED;

    if (_MultilistType==ML_VARIABLE_DURING_RUNTIME)
        variableMultilistType=ML_UNORDERED_LIST;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::~Multilist()
{
    if (data!=0)
        VENet::OP_DELETE_ARRAY(data, _FILE_AND_LINE_);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Multilist( const Multilist& source )
{
    CopySource(source);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
Multilist<_MultilistType, _DataType, _KeyType, _IndexType>& Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::operator= ( const Multilist& source )
{
    Clear(true);
    CopySource(source);
    return *this;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::CopySource( const Multilist& source )
{
    dataSize=source.GetSize();
    ascendingSort=source.ascendingSort;
    sortState=source.sortState;
    queueHead=0;
    queueTail=dataSize;
    preallocationSize=source.preallocationSize;
    variableMultilistType=source.variableMultilistType;
    if (source.data==0)
    {
        data=0;
        allocationSize=0;
    }
    else
    {
        allocationSize=dataSize;
        data = VENet::OP_NEW_ARRAY<_DataType>(dataSize,_FILE_AND_LINE_);
        _IndexType i;
        for (i=0; i < dataSize; i++)
            data[i]=source[i];
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_DataType& Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::operator[] ( const _IndexType position ) const
{
    VEAssert(position<GetSize());

    if (GetMultilistType()==ML_QUEUE)
    {
        if ( queueHead + position >= allocationSize )
            return data[ queueHead + position - allocationSize ];
        else
            return data[ queueHead + position ];
    }

    return data[position];
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Push(const _DataType &d, const char *file, unsigned int line )
{
    Push(d,d,file,line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Push(const _DataType &d, const _KeyType &key, const char *file, unsigned int line )
{
    ReallocateIfNeeded(file,line);

    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK)
    {
        data[dataSize]=d;
        dataSize++;
    }
    else if (GetMultilistType()==ML_QUEUE)
    {
        data[queueTail++] = d;

        if ( queueTail == allocationSize )
            queueTail = 0;
        dataSize++;
    }
    else
    {
        VEAssert(GetMultilistType()==ML_ORDERED_LIST);
        InsertInOrderedList(d,key);
    }

    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_QUEUE)
    {
        if (sortState!=ML_UNSORTED && dataSize>1)
        {
            if (ascendingSort)
            {
                if ( MLKeyRef<_KeyType>(key) < operator[](dataSize-2) )
                    sortState=ML_UNSORTED;
            }
            else
            {
                if ( MLKeyRef<_KeyType>(key) > operator[](dataSize-2) )
                    sortState=ML_UNSORTED;
            }

            sortState=ML_UNSORTED;
        }
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_DataType &Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Pop(const char *file, unsigned int line)
{
    VEAssert(IsEmpty()==false);
    DeallocateIfNeeded(file,line);
    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_ORDERED_LIST)
    {
        dataSize--;
        return data[dataSize];
    }
    else
    {
        VEAssert(GetMultilistType()==ML_QUEUE);

        if ( ++queueHead == allocationSize )
            queueHead = 0;

        if ( queueHead == 0 )
            return data[ allocationSize -1 ];

        dataSize--;
        return data[ queueHead -1 ];
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_DataType &Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Peek(void) const
{
    VEAssert(IsEmpty()==false);
    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_ORDERED_LIST)
    {
        return data[dataSize-1];
    }
    else
    {
        VEAssert(GetMultilistType()==ML_QUEUE);
        return data[ queueHead ];
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::PushOpposite(const _DataType &d, const char *file, unsigned int line )
{
    PushOpposite(d,d,file,line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::PushOpposite(const _DataType &d, const _KeyType &key, const char *file, unsigned int line )
{
    ReallocateIfNeeded(file,line);

    if (GetMultilistType()==ML_UNORDERED_LIST)
    {
        data[dataSize]=d;
        dataSize++;
    }
    else if (GetMultilistType()==ML_STACK)
    {
        InsertAtIndex(d,0,file,line);
    }
    else if (GetMultilistType()==ML_QUEUE)
    {
        InsertAtIndex(d,0,file,line);
    }
    else
    {
        VEAssert(GetMultilistType()==ML_ORDERED_LIST);
        InsertInOrderedList(d,key);
    }

    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_QUEUE)
    {
        if (sortState!=ML_UNSORTED && dataSize>1)
        {
            if (ascendingSort)
            {
                if ( MLKeyRef<_KeyType>(key) > operator[](1) )
                    sortState=ML_UNSORTED;
            }
            else
            {
                if ( MLKeyRef<_KeyType>(key) < operator[](1) )
                    sortState=ML_UNSORTED;
            }
        }
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_DataType &Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::PopOpposite(const char *file, unsigned int line)
{
    VEAssert(IsEmpty()==false);
    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_ORDERED_LIST)
    {
        ReallocateIfNeeded(file,line);
        data[dataSize]=data[0];
        DeleteShiftArrayLeft(0);
        --dataSize;
        DeallocateIfNeeded(file,line);
        return data[dataSize+1];
    }
    else
    {
        VEAssert(GetMultilistType()==ML_QUEUE);
        DeallocateIfNeeded(file,line);
        dataSize--;

        if (queueTail==0)
            queueTail=allocationSize-1;
        else
            --queueTail;

        return data[queueTail];
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_DataType &Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::PeekOpposite(void) const
{
    VEAssert(IsEmpty()==false);
    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_ORDERED_LIST)
    {
        return data[0];
    }
    else
    {
        VEAssert(GetMultilistType()==ML_QUEUE);
        _IndexType priorIndex;
        if (queueTail==0)
            priorIndex=allocationSize-1;
        else
            priorIndex=queueTail-1;

        return data[priorIndex];
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::InsertAtIndex(const _DataType &d, _IndexType index, const char *file, unsigned int line)
{
    ReallocateIfNeeded(file,line);

    if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK || GetMultilistType()==ML_ORDERED_LIST)
    {
        if (index>=dataSize)
        {
            data[dataSize]=d;

            dataSize++;
        }
        else
        {
            InsertShiftArrayRight(d,index);
        }
    }
    else
    {
        data[queueTail++] = d;

        if ( queueTail == allocationSize )
            queueTail = 0;

        ++dataSize;

        if (dataSize==1)
            return;

        _IndexType writeIndex, readIndex, trueWriteIndex, trueReadIndex;
        writeIndex=dataSize-1;
        readIndex=writeIndex-1;
        while (readIndex >= index)
        {
            if ( queueHead + writeIndex >= allocationSize )
                trueWriteIndex = queueHead + writeIndex - allocationSize;
            else
                trueWriteIndex = queueHead + writeIndex;

            if ( queueHead + readIndex >= allocationSize )
                trueReadIndex = queueHead + readIndex - allocationSize;
            else
                trueReadIndex = queueHead + readIndex;

            data[trueWriteIndex]=data[trueReadIndex];

            if (readIndex==0)
                break;
            writeIndex--;
            readIndex--;
        }

        if ( queueHead + index >= allocationSize )
            trueWriteIndex = queueHead + index - allocationSize;
        else
            trueWriteIndex = queueHead + index;

        data[trueWriteIndex]=d;
    }

    if (_MultilistType!=ML_ORDERED_LIST)
        sortState=ML_UNSORTED;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::RemoveAtIndex(_IndexType position, const char *file, unsigned int line)
{
    VEAssert(position < dataSize);
    VEAssert(IsEmpty()==false);

    if (GetMultilistType()==ML_UNORDERED_LIST)
    {
        data[position]=data[dataSize-1];
    }
    else if (GetMultilistType()==ML_STACK || GetMultilistType()==ML_ORDERED_LIST)
    {
        DeleteShiftArrayLeft(position);
    }
    else
    {
        VEAssert(GetMultilistType()==ML_QUEUE);

        _IndexType index, next;

        if ( queueHead + position >= allocationSize )
            index = queueHead + position - allocationSize;
        else
            index = queueHead + position;

        next = index + 1;

        if ( next == allocationSize )
            next = 0;

        while ( next != queueTail )
        {
            data[ index ] = data[ next ];
            index = next;

            if ( ++next == allocationSize )
                next = 0;
        }

        if ( queueTail == 0 )
            queueTail = allocationSize - 1;
        else
            --queueTail;
    }


    dataSize--;
    DeallocateIfNeeded(file,line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
bool Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::RemoveAtKey(_KeyType key, bool assertIfDoesNotExist, const char *file, unsigned int line)
{
    _IndexType index = GetIndexOf(key);
    if (index==(_IndexType)-1)
    {
        VEAssert(assertIfDoesNotExist==false && "RemoveAtKey element not found");
        return false;
    }
    RemoveAtIndex(index,file,line);
    return true;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_IndexType Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetIndexOf(_KeyType key) const
{
    _IndexType i;
    if (IsSorted())
    {
        bool objectExists;
        i=GetIndexFromKeyInSortedList(key, &objectExists);
        if (objectExists)
            return i;
        return (_IndexType)-1;
    }
    else if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK)
    {
        for (i=0; i < dataSize; i++)
        {
            if (MLKeyRef<_KeyType>(key)==data[i])
                return i;
        }
        return (_IndexType)-1;
    }
    else
    {
        VEAssert( GetMultilistType()==ML_QUEUE );

        for (i=0; i < dataSize; i++)
        {
            if (MLKeyRef<_KeyType>(key)==operator[](i))
                return i;
        }
        return (_IndexType)-1;
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_IndexType Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetInsertionIndex(_KeyType key) const
{
    _IndexType i;
    if (IsSorted())
    {
        bool objectExists;
        i=GetIndexFromKeyInSortedList(key, &objectExists);
        if (objectExists)
            return (_IndexType)-1;
        return i;
    }
    else if (GetMultilistType()==ML_UNORDERED_LIST || GetMultilistType()==ML_STACK)
    {
        for (i=0; i < dataSize; i++)
        {
            if (MLKeyRef<_KeyType>(key)==data[i])
                return (_IndexType)-1;
        }
        return dataSize;
    }
    else
    {
        VEAssert( GetMultilistType()==ML_QUEUE );

        for (i=0; i < dataSize; i++)
        {
            if (MLKeyRef<_KeyType>(key)==operator[](i))
                return (_IndexType)-1;
        }
        return dataSize;
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_DataType Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetPtr(_KeyType key) const
{
    _IndexType i = GetIndexOf(key);
    if (i==(_IndexType)-1)
        return 0;
    return data[i];
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ForEach(void (*func)(_DataType &item, const char *file, unsigned int line), const char *file, unsigned int line)
{
    _IndexType i;
    for (i=0; i < dataSize; i++)
        func(operator[](i), file, line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ForEach(void (*func)(_DataType &item))
{
    _IndexType i;
    for (i=0; i < dataSize; i++)
        func(operator[](i));
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
bool Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::IsEmpty(void) const
{
    return dataSize==0;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_IndexType Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetSize(void) const
{
    return dataSize;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Clear( bool deallocateSmallBlocks, const char *file, unsigned int line )
{
    dataSize=0;
    if (GetMultilistType()==ML_ORDERED_LIST)
        if (ascendingSort)
            sortState=ML_SORTED_ASCENDING;
        else
            sortState=ML_SORTED_DESCENDING;
    else
        sortState=ML_UNSORTED;
    queueHead=0;
    queueTail=0;

    if (deallocateSmallBlocks && allocationSize < 128 && data)
    {
        VENet::OP_DELETE_ARRAY(data,file,line);
        data=0;
        allocationSize=0;
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ClearPointers( bool deallocateSmallBlocks, const char *file, unsigned int line )
{
    _IndexType i;
    for (i=0; i < dataSize; i++)
        VENet::OP_DELETE(operator[](i), file, line);
    Clear(deallocateSmallBlocks, file, line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ClearPointer( _KeyType key, const char *file, unsigned int line )
{
    _IndexType i;
    i = GetIndexOf(key);
    if (i!=-1)
    {
        VENet::OP_DELETE(operator[](i), file, line);
        RemoveAtIndex(i);
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ReverseList(void)
{
    if (IsSorted())
        ascendingSort=!ascendingSort;

    ReverseListInternal();
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Reallocate(_IndexType size, const char *file, unsigned int line)
{
    _IndexType newAllocationSize;
    if (size < dataSize)
        newAllocationSize=dataSize;
    else
        newAllocationSize=size;
    preallocationSize=size;
    ReallocToSize(newAllocationSize,file,line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::Sort(bool force)
{
    if (IsSorted() && force==false)
        return;

    if (dataSize>1)
    {
        if (ascendingSort)
            QSortAscending(0,dataSize-1);
        else
            QSortDescending(0,dataSize-1);
    }

    TagSorted();
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::TagSorted(void)
{
    if (ascendingSort)
        sortState=ML_SORTED_ASCENDING;
    else
        sortState=ML_SORTED_DESCENDING;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::QSortAscending(_IndexType leftEdge, _IndexType rightEdge)
{
    _DataType temp;
    _IndexType left=leftEdge;
    _IndexType right=rightEdge;
    _IndexType pivotIndex=left++;

    while (left<right)
    {
        if (data[left] <= data[pivotIndex])
        {
            ++left;
        }
        else
        {
            temp=data[left];
            data[left]=data[right];
            data[right]=temp;
            --right;
        }
    }

    temp=data[pivotIndex];

    if (data[left] > data[pivotIndex])
    {
        --left;

        data[pivotIndex]=data[left];
        data[left]=temp;
    }
    else
    {
        data[pivotIndex]=data[left];
        data[left]=temp;

        --left;
    }

    if (left!=leftEdge)
        QSortAscending(leftEdge, left);

    if (right!=rightEdge)
        QSortAscending(right, rightEdge);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::QSortDescending(_IndexType leftEdge, _IndexType rightEdge)
{
    _DataType temp;
    _IndexType left=leftEdge;
    _IndexType right=rightEdge;
    _IndexType pivotIndex=left++;

    while (left<right)
    {
        if (data[left] >= data[pivotIndex])
        {
            ++left;
        }
        else
        {
            temp=data[left];
            data[left]=data[right];
            data[right]=temp;
            --right;
        }
    }

    temp=data[pivotIndex];

    if (data[left] < data[pivotIndex])
    {
        --left;

        data[pivotIndex]=data[left];
        data[left]=temp;
    }
    else
    {
        data[pivotIndex]=data[left];
        data[left]=temp;

        --left;
    }

    if (left!=leftEdge)
        QSortDescending(leftEdge, left);

    if (right!=rightEdge)
        QSortDescending(right, rightEdge);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::SetSortOrder(bool ascending)
{
    if (ascendingSort!=ascending && IsSorted())
    {
        ascendingSort=ascending;
        ReverseListInternal();
    }
    else
        ascendingSort=ascending;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
bool Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetSortOrder(void) const
{
    return ascendingSort;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
bool Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::IsSorted(void) const
{
    return GetMultilistType()==ML_ORDERED_LIST || sortState!=ML_UNSORTED;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
MultilistType Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetMultilistType(void) const
{
    if (_MultilistType==ML_VARIABLE_DURING_RUNTIME)
        return variableMultilistType;
    return _MultilistType;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::SetMultilistType(MultilistType newType)
{
    VEAssert(_MultilistType==ML_VARIABLE_DURING_RUNTIME);
    switch (variableMultilistType)
    {
    case ML_UNORDERED_LIST:
        switch (newType)
        {
        case ML_UNORDERED_LIST:
            break;
        case ML_STACK:
            break;
        case ML_QUEUE:
            queueHead=0;
            queueTail=dataSize;
            break;
        case ML_ORDERED_LIST:
            Sort(false);
            break;
        }
        break;
    case ML_STACK:
        switch (newType)
        {
        case ML_UNORDERED_LIST:
            break;
        case ML_STACK:
            break;
        case ML_QUEUE:
            queueHead=0;
            queueTail=dataSize;
            break;
        case ML_ORDERED_LIST:
            Sort(false);
            break;
        }
        break;
    case ML_QUEUE:
        switch (newType)
        {
        case ML_UNORDERED_LIST:
        case ML_STACK:
        case ML_ORDERED_LIST:
            if (queueTail < queueHead)
            {
                ReallocToSize(dataSize, _FILE_AND_LINE_);
            }
            else
            {
                _IndexType i;
                for (i=0; i < dataSize; i++)
                    data[i]=operator[](i);
            }
            if (newType==ML_ORDERED_LIST)
                Sort(false);
            break;
        case ML_QUEUE:
            break;
        }
        break;
    case ML_ORDERED_LIST:
        switch (newType)
        {
        case ML_UNORDERED_LIST:
        case ML_STACK:
        case ML_QUEUE:
            if (ascendingSort)
                sortState=ML_SORTED_ASCENDING;
            else
                sortState=ML_SORTED_DESCENDING;
            if (newType==ML_QUEUE)
            {
                queueHead=0;
                queueTail=dataSize;
            }
            break;
        case ML_ORDERED_LIST:
            break;
        }
        break;
    }

    variableMultilistType=newType;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::FindIntersection(
    Multilist& source1,
    Multilist& source2,
    Multilist& intersection,
    Multilist& uniqueToSource1,
    Multilist& uniqueToSource2)
{
    _IndexType index1=0, index2=0;
    source1.SetSortOrder(true);
    source2.SetSortOrder(true);
    source1.Sort(false);
    source2.Sort(false);
    intersection.Clear(true,_FILE_AND_LINE_);
    uniqueToSource1.Clear(true,_FILE_AND_LINE_);
    uniqueToSource2.Clear(true,_FILE_AND_LINE_);

    while (index1 < source1.GetSize() && index2 < source2.GetSize())
    {
        if (source1[index1]<source2[index2])
        {
            uniqueToSource1.Push(source1[index1],_FILE_AND_LINE_);
            index1++;
        }
        else if (source1[index1]==source2[index2])
        {
            intersection.Push(source1[index1],_FILE_AND_LINE_);
            index1++;
            index2++;
        }
        else
        {
            uniqueToSource2.Push(source2[index2],_FILE_AND_LINE_);
            index2++;
        }
    }
    while (index1 < source1.GetSize())
    {
        uniqueToSource1.Push(source1[index1],_FILE_AND_LINE_);
        index1++;
    }
    while (index2 < source2.GetSize())
    {
        uniqueToSource2.Push(source2[index2],_FILE_AND_LINE_);
        index2++;
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ReallocateIfNeeded(const char *file, unsigned int line)
{
    if (dataSize<allocationSize)
        return;

    _IndexType newAllocationSize;
    if (allocationSize<16)
        newAllocationSize=32;
    else if (allocationSize>65536)
        newAllocationSize=allocationSize+65536;
    else
    {
        newAllocationSize=allocationSize<<1;
        if (newAllocationSize < allocationSize)
            newAllocationSize=allocationSize+65536;
    }

    ReallocToSize(newAllocationSize,file,line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::DeallocateIfNeeded(const char *file, unsigned int line)
{
    if (allocationSize<512)
        return;
    if (dataSize >= allocationSize/3 )
        return;
    if (dataSize <= preallocationSize )
        return;

    _IndexType newAllocationSize = dataSize<<1;

    ReallocToSize(newAllocationSize,file,line);
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ReallocToSize(_IndexType newAllocationSize, const char *file, unsigned int line)
{
    _DataType* newData = VENet::OP_NEW_ARRAY<_DataType>(newAllocationSize,file,line);
    _IndexType i;
    for (i=0; i < dataSize; i++)
        newData[i]=operator[](i);
    if (dataSize>0)
    {
        VENet::OP_DELETE_ARRAY(data,file,line);
        if (GetMultilistType()==ML_QUEUE)
        {
            queueHead=0;
            queueTail=dataSize;
        }
    }
    data=newData;
    allocationSize=newAllocationSize;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::ReverseListInternal(void)
{
    _DataType temp;
    _IndexType i;
    for (i=0; i < dataSize/2; i++)
    {
        temp=operator[](i);
        operator[](i)=operator[](dataSize-1-i);
        operator[](dataSize-1-i)=temp;
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::InsertInOrderedList(const _DataType &d, const _KeyType &key)
{
    VEAssert(GetMultilistType()==ML_ORDERED_LIST);

    bool objectExists;
    _IndexType index;
    index = GetIndexFromKeyInSortedList(key, &objectExists);

    if (index>=dataSize)
    {
        data[dataSize]=d;
        dataSize++;
    }
    else
    {
        InsertShiftArrayRight(d,index);
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
_IndexType Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::GetIndexFromKeyInSortedList(const _KeyType &key, bool *objectExists) const
{
    VEAssert(IsSorted());
    _IndexType index, upperBound, lowerBound;

    if (dataSize==0)
    {
        *objectExists=false;
        return 0;
    }

    upperBound=dataSize-1;
    lowerBound=0;
    index = dataSize/2;

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
    while (1)
    {
        if (MLKeyRef<_KeyType>(key) > operator[](index) )
        {
            if (ascendingSort)
                lowerBound=index+1;
            else
                upperBound=index-1;
        }
        else if (MLKeyRef<_KeyType>(key) < operator[](index) )
        {
            if (ascendingSort)
                upperBound=index-1;
            else
                lowerBound=index+1;
        }
        else
        {
            *objectExists=true;
            return index;
        }

        index=lowerBound+(upperBound-lowerBound)/2;

        if (lowerBound>upperBound || upperBound==(_IndexType)-1)
        {
            *objectExists=false;
            return lowerBound;
        }
    }
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::InsertShiftArrayRight(const _DataType &d, _IndexType index)
{
    VEAssert(_MultilistType!=ML_QUEUE);

    _IndexType i;
    for ( i = dataSize; i != index; i-- )
        data[ i ] = data[ i - 1 ];

    data[ index ] = d;

    ++dataSize;
}

template <const MultilistType _MultilistType, class _DataType, class _KeyType, class _IndexType>
void Multilist<_MultilistType, _DataType, _KeyType, _IndexType>::DeleteShiftArrayLeft( _IndexType index )
{
    VEAssert(index < dataSize);
    VEAssert(_MultilistType!=ML_QUEUE);

    _IndexType i;
    for ( i = index; i < dataSize-1; i++ )
        data[i]=data[i+1];
}
};
