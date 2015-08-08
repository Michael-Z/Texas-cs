////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_Queue.h
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
#include "Export.h"
#include "VEMemoryOverride.h"

namespace DataStructures
{
template <class queue_type>
class VE_DLL_EXPORT Queue
{
public:
    Queue();
    ~Queue();
    Queue( Queue& original_copy );
    bool operator= ( const Queue& original_copy );
    void Push( const queue_type& input, const char *file, unsigned int line );
    void PushAtHead( const queue_type& input, unsigned index, const char *file, unsigned int line );
    queue_type& operator[] ( unsigned int position ) const;
    void RemoveAtIndex( unsigned int position );
    inline queue_type Peek( void ) const;
    inline queue_type PeekTail( void ) const;
    inline queue_type Pop( void );
    inline queue_type PopTail( void );
    inline queue_type PopDeref( void );
    inline unsigned int Size( void ) const;
    inline bool IsEmpty(void) const;
    inline unsigned int AllocationSize( void ) const;
    inline void Clear( const char *file, unsigned int line );
    void Compress( const char *file, unsigned int line );
    bool Find ( queue_type q );
    void ClearAndForceAllocation( int size, const char *file, unsigned int line );

private:
    queue_type* array;
    unsigned int head;
    unsigned int tail;
    unsigned int allocation_size;
};


template <class queue_type>
inline unsigned int Queue<queue_type>::Size( void ) const
{
    if ( head <= tail )
        return tail -head;
    else
        return allocation_size -head + tail;
}

template <class queue_type>
inline bool Queue<queue_type>::IsEmpty(void) const
{
    return head==tail;
}

template <class queue_type>
inline unsigned int Queue<queue_type>::AllocationSize( void ) const
{
    return allocation_size;
}

template <class queue_type>
Queue<queue_type>::Queue()
{
    allocation_size = 0;
    array=0;
    head = 0;
    tail = 0;
}

template <class queue_type>
Queue<queue_type>::~Queue()
{
    if (allocation_size>0)
        VENet::OP_DELETE_ARRAY(array, _FILE_AND_LINE_);
}

template <class queue_type>
inline queue_type Queue<queue_type>::Pop( void )
{
#ifdef _DEBUG
    VEAssert( head != tail);
#endif
    if ( ++head == allocation_size )
        head = 0;

    if ( head == 0 )
        return ( queue_type ) array[ allocation_size -1 ];

    return ( queue_type ) array[ head -1 ];
}

template <class queue_type>
inline queue_type Queue<queue_type>::PopTail( void )
{
#ifdef _DEBUG
    VEAssert( head != tail );
#endif
    if (tail!=0)
    {
        --tail;
        return ( queue_type ) array[ tail ];
    }
    else
    {
        tail=allocation_size-1;
        return ( queue_type ) array[ tail ];
    }
}

template <class queue_type>
inline queue_type Queue<queue_type>::PopDeref( void )
{
    if ( ++head == allocation_size )
        head = 0;

    queue_type q;
    if ( head == 0 )
    {
        q=array[ allocation_size -1 ];
        array[ allocation_size -1 ]=0;
        return q;
    }

    q=array[ head -1 ];
    array[ head -1 ]=0;
    return q;
}

template <class queue_type>
void Queue<queue_type>::PushAtHead( const queue_type& input, unsigned index, const char *file, unsigned int line )
{
    VEAssert(index <= Size());

    Push(input, file, line );

    if (Size()==1)
        return;

    unsigned writeIndex, readIndex, trueWriteIndex, trueReadIndex;
    writeIndex=Size()-1;
    readIndex=writeIndex-1;
    while (readIndex >= index)
    {
        if ( head + writeIndex >= allocation_size )
            trueWriteIndex = head + writeIndex - allocation_size;
        else
            trueWriteIndex = head + writeIndex;

        if ( head + readIndex >= allocation_size )
            trueReadIndex = head + readIndex - allocation_size;
        else
            trueReadIndex = head + readIndex;

        array[trueWriteIndex]=array[trueReadIndex];

        if (readIndex==0)
            break;
        writeIndex--;
        readIndex--;
    }

    if ( head + index >= allocation_size )
        trueWriteIndex = head + index - allocation_size;
    else
        trueWriteIndex = head + index;

    array[trueWriteIndex]=input;
}


template <class queue_type>
inline queue_type Queue<queue_type>::Peek( void ) const
{
#ifdef _DEBUG
    VEAssert( head != tail );
#endif

    return ( queue_type ) array[ head ];
}

template <class queue_type>
inline queue_type Queue<queue_type>::PeekTail( void ) const
{
#ifdef _DEBUG
    VEAssert( head != tail );
#endif
    if (tail!=0)
        return ( queue_type ) array[ tail-1 ];
    else
        return ( queue_type ) array[ allocation_size-1 ];
}

template <class queue_type>
void Queue<queue_type>::Push( const queue_type& input, const char *file, unsigned int line )
{
    if ( allocation_size == 0 )
    {
        array = VENet::OP_NEW_ARRAY<queue_type>(16, file, line );
        head = 0;
        tail = 1;
        array[ 0 ] = input;
        allocation_size = 16;
        return ;
    }

    array[ tail++ ] = input;

    if ( tail == allocation_size )
        tail = 0;

    if ( tail == head )
    {
        queue_type * new_array;
        new_array = VENet::OP_NEW_ARRAY<queue_type>(allocation_size * 2, file, line );
#ifdef _DEBUG
        VEAssert( new_array );
#endif
        if (new_array==0)
            return;

        for ( unsigned int counter = 0; counter < allocation_size; ++counter )
            new_array[ counter ] = array[ ( head + counter ) % ( allocation_size ) ];

        head = 0;

        tail = allocation_size;

        allocation_size *= 2;

        VENet::OP_DELETE_ARRAY(array, file, line);

        array = new_array;
    }

}

template <class queue_type>
Queue<queue_type>::Queue( Queue& original_copy )
{
    if ( original_copy.Size() == 0 )
    {
        allocation_size = 0;
    }

    else
    {
        array = VENet::OP_NEW_ARRAY<queue_type >( original_copy.Size() + 1 , _FILE_AND_LINE_ );

        for ( unsigned int counter = 0; counter < original_copy.Size(); ++counter )
            array[ counter ] = original_copy.array[ ( original_copy.head + counter ) % ( original_copy.allocation_size ) ];

        head = 0;

        tail = original_copy.Size();

        allocation_size = original_copy.Size() + 1;
    }
}

template <class queue_type>
bool Queue<queue_type>::operator= ( const Queue& original_copy )
{
    if ( ( &original_copy ) == this )
        return false;

    Clear(_FILE_AND_LINE_);

    if ( original_copy.Size() == 0 )
    {
        allocation_size = 0;
    }

    else
    {
        array = VENet::OP_NEW_ARRAY<queue_type >( original_copy.Size() + 1 , _FILE_AND_LINE_ );

        for ( unsigned int counter = 0; counter < original_copy.Size(); ++counter )
            array[ counter ] = original_copy.array[ ( original_copy.head + counter ) % ( original_copy.allocation_size ) ];

        head = 0;

        tail = original_copy.Size();

        allocation_size = original_copy.Size() + 1;
    }

    return true;
}

template <class queue_type>
inline void Queue<queue_type>::Clear ( const char *file, unsigned int line )
{
    if ( allocation_size == 0 )
        return ;

    if (allocation_size > 32)
    {
        VENet::OP_DELETE_ARRAY(array, file, line);
        allocation_size = 0;
    }

    head = 0;
    tail = 0;
}

template <class queue_type>
void Queue<queue_type>::Compress ( const char *file, unsigned int line )
{
    queue_type* new_array;
    unsigned int newAllocationSize;
    if (allocation_size==0)
        return;

    newAllocationSize=1;
    while (newAllocationSize <= Size())
        newAllocationSize<<=1;

    new_array = VENet::OP_NEW_ARRAY<queue_type >(newAllocationSize, file, line );

    for (unsigned int counter=0; counter < Size(); ++counter)
        new_array[counter] = array[(head + counter)%(allocation_size)];

    tail=Size();
    allocation_size=newAllocationSize;
    head=0;

    VENet::OP_DELETE_ARRAY(array, file, line);
    array=new_array;
}

template <class queue_type>
bool Queue<queue_type>::Find ( queue_type q )
{
    if ( allocation_size == 0 )
        return false;

    unsigned int counter = head;

    while ( counter != tail )
    {
        if ( array[ counter ] == q )
            return true;

        counter = ( counter + 1 ) % allocation_size;
    }

    return false;
}

template <class queue_type>
void Queue<queue_type>::ClearAndForceAllocation( int size, const char *file, unsigned int line )
{
    VENet::OP_DELETE_ARRAY(array, file, line);
    if (size>0)
        array = VENet::OP_NEW_ARRAY<queue_type>(size, file, line );
    else
        array=0;
    allocation_size = size;
    head = 0;
    tail = 0;
}

template <class queue_type>
inline queue_type& Queue<queue_type>::operator[] ( unsigned int position ) const
{
#ifdef _DEBUG
    VEAssert( position < Size() );
#endif
    if ( head + position >= allocation_size )
        return array[ head + position - allocation_size ];
    else
        return array[ head + position ];
}

template <class queue_type>
void Queue<queue_type>::RemoveAtIndex( unsigned int position )
{
#ifdef _DEBUG
    VEAssert( position < Size() );
    VEAssert( head != tail );
#endif

    if ( head == tail || position >= Size() )
        return ;

    unsigned int index;

    unsigned int next;

    if ( head + position >= allocation_size )
        index = head + position - allocation_size;
    else
        index = head + position;

    next = index + 1;

    if ( next == allocation_size )
        next = 0;

    while ( next != tail )
    {
        array[ index ] = array[ next ];
        index = next;

        if ( ++next == allocation_size )
            next = 0;
    }

    if ( tail == 0 )
        tail = allocation_size - 1;
    else
        --tail;
}
}
