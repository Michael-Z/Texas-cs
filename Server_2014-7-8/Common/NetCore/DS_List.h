////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_List.h
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

static const unsigned int MAX_UNSIGNED_LONG = 4294967295U;

namespace DataStructures
{
template <class list_type>
class VE_DLL_EXPORT List
{
public:
    List();
    ~List();

    List( const List& original_copy );

    List& operator= ( const List& original_copy );

    list_type& operator[] ( const unsigned int position ) const;

    list_type& Get ( const unsigned int position ) const;

    void Push(const list_type &input, const char *file, unsigned int line );

    list_type& Pop(void);

    void Insert( const list_type &input, const unsigned int position, const char *file, unsigned int line );

    void Insert( const list_type &input, const char *file, unsigned int line );

    void Replace( const list_type &input, const list_type filler, const unsigned int position, const char *file, unsigned int line );

    void Replace( const list_type &input );

    void RemoveAtIndex( const unsigned int position );

    void RemoveAtIndexFast( const unsigned int position );

    void RemoveFromEnd(const unsigned num=1);

    unsigned int GetIndexOf( const list_type &input ) const;

    unsigned int Size( void ) const;

    void Clear( bool doNotDeallocateSmallBlocks, const char *file, unsigned int line );

    void Preallocate( unsigned countNeeded, const char *file, unsigned int line );

    void Compress( const char *file, unsigned int line );

private:
    list_type* listArray;

    unsigned int list_size;

    unsigned int allocation_size;
};
template <class list_type>
List<list_type>::List()
{
    allocation_size = 0;
    listArray = 0;
    list_size = 0;
}

template <class list_type>
List<list_type>::~List()
{
    if (allocation_size>0)
        VENet::OP_DELETE_ARRAY(listArray, _FILE_AND_LINE_);
}


template <class list_type>
List<list_type>::List( const List& original_copy )
{

    if ( original_copy.list_size == 0 )
    {
        list_size = 0;
        allocation_size = 0;
    }
    else
    {
        listArray = VENet::OP_NEW_ARRAY<list_type >( original_copy.list_size , _FILE_AND_LINE_ );

        for ( unsigned int counter = 0; counter < original_copy.list_size; ++counter )
            listArray[ counter ] = original_copy.listArray[ counter ];

        list_size = allocation_size = original_copy.list_size;
    }
}

template <class list_type>
List<list_type>& List<list_type>::operator= ( const List& original_copy )
{
    if ( ( &original_copy ) != this )
    {
        Clear( false, _FILE_AND_LINE_ );

        if ( original_copy.list_size == 0 )
        {
            list_size = 0;
            allocation_size = 0;
        }

        else
        {
            listArray = VENet::OP_NEW_ARRAY<list_type >( original_copy.list_size , _FILE_AND_LINE_ );

            for ( unsigned int counter = 0; counter < original_copy.list_size; ++counter )
                listArray[ counter ] = original_copy.listArray[ counter ];

            list_size = allocation_size = original_copy.list_size;
        }
    }

    return *this;
}


template <class list_type>
inline list_type& List<list_type>::operator[] ( const unsigned int position ) const
{
#ifdef _DEBUG
    if (position>=list_size)
    {
        VEAssert ( position < list_size );
    }
#endif
    return listArray[ position ];
}

template <class list_type>
inline list_type& List<list_type>::Get ( const unsigned int position ) const
{
    return listArray[ position ];
}

template <class list_type>
void List<list_type>::Push(const list_type &input, const char *file, unsigned int line)
{
    Insert(input, file, line);
}

template <class list_type>
inline list_type& List<list_type>::Pop(void)
{
#ifdef _DEBUG
    VEAssert(list_size>0);
#endif
    --list_size;
    return listArray[list_size];
}

template <class list_type>
void List<list_type>::Insert( const list_type &input, const unsigned int position, const char *file, unsigned int line )
{
#ifdef _DEBUG
    if (position>list_size)
    {
        VEAssert( position <= list_size );
    }
#endif

    if ( list_size == allocation_size )
    {
        list_type * new_array;

        if ( allocation_size == 0 )
            allocation_size = 16;
        else
            allocation_size *= 2;

        new_array = VENet::OP_NEW_ARRAY<list_type >( allocation_size , file, line );

        for ( unsigned int counter = 0; counter < list_size; ++counter )
            new_array[ counter ] = listArray[ counter ];

        VENet::OP_DELETE_ARRAY(listArray, file, line);

        listArray = new_array;
    }

    for ( unsigned int counter = list_size; counter != position; counter-- )
        listArray[ counter ] = listArray[ counter - 1 ];

    listArray[ position ] = input;

    ++list_size;

}


template <class list_type>
void List<list_type>::Insert( const list_type &input, const char *file, unsigned int line )
{
    if ( list_size == allocation_size )
    {
        list_type * new_array;

        if ( allocation_size == 0 )
            allocation_size = 16;
        else
            allocation_size *= 2;

        new_array = VENet::OP_NEW_ARRAY<list_type >( allocation_size , file, line );

        if (listArray)
        {
            for ( unsigned int counter = 0; counter < list_size; ++counter )
                new_array[ counter ] = listArray[ counter ];

            VENet::OP_DELETE_ARRAY(listArray, file, line);
        }

        listArray = new_array;
    }

    listArray[ list_size ] = input;

    ++list_size;
}

template <class list_type>
inline void List<list_type>::Replace( const list_type &input, const list_type filler, const unsigned int position, const char *file, unsigned int line )
{
    if ( ( list_size > 0 ) && ( position < list_size ) )
    {
        listArray[ position ] = input;
    }
    else
    {
        if ( position >= allocation_size )
        {
            list_type * new_array;
            allocation_size = position + 1;

            new_array = VENet::OP_NEW_ARRAY<list_type >( allocation_size , file, line );

            for ( unsigned int counter = 0; counter < list_size; ++counter )
                new_array[ counter ] = listArray[ counter ];

            VENet::OP_DELETE_ARRAY(listArray, file, line);

            listArray = new_array;
        }

        while ( list_size < position )
            listArray[ list_size++ ] = filler;

        listArray[ list_size++ ] = input;

#ifdef _DEBUG

        VEAssert( list_size == position + 1 );

#endif

    }
}

template <class list_type>
inline void List<list_type>::Replace( const list_type &input )
{
    if ( list_size > 0 )
        listArray[ list_size - 1 ] = input;
}

template <class list_type>
void List<list_type>::RemoveAtIndex( const unsigned int position )
{
#ifdef _DEBUG
    if (position >= list_size)
    {
        VEAssert( position < list_size );
        return;
    }
#endif

    if ( position < list_size )
    {
        for ( unsigned int counter = position; counter < list_size - 1 ; ++counter )
            listArray[ counter ] = listArray[ counter + 1 ];
        RemoveFromEnd();
    }
}

template <class list_type>
void List<list_type>::RemoveAtIndexFast( const unsigned int position )
{
#ifdef _DEBUG
    if (position >= list_size)
    {
        VEAssert( position < list_size );
        return;
    }
#endif
    --list_size;
    listArray[position]=listArray[list_size];
}

template <class list_type>
inline void List<list_type>::RemoveFromEnd( const unsigned num )
{
#ifdef _DEBUG
    VEAssert(list_size>=num);
#endif
    list_size-=num;
}

template <class list_type>
unsigned int List<list_type>::GetIndexOf( const list_type &input ) const
{
    for ( unsigned int i = 0; i < list_size; ++i )
        if ( listArray[ i ] == input )
            return i;

    return MAX_UNSIGNED_LONG;
}

template <class list_type>
inline unsigned int List<list_type>::Size( void ) const
{
    return list_size;
}

template <class list_type>
void List<list_type>::Clear( bool doNotDeallocateSmallBlocks, const char *file, unsigned int line )
{
    if ( allocation_size == 0 )
        return;

    if (allocation_size>512 || doNotDeallocateSmallBlocks==false)
    {
        VENet::OP_DELETE_ARRAY(listArray, file, line);
        allocation_size = 0;
        listArray = 0;
    }
    list_size = 0;
}

template <class list_type>
void List<list_type>::Compress( const char *file, unsigned int line )
{
    list_type * new_array;

    if ( allocation_size == 0 )
        return ;

    new_array = VENet::OP_NEW_ARRAY<list_type >( allocation_size , file, line );

    for ( unsigned int counter = 0; counter < list_size; ++counter )
        new_array[ counter ] = listArray[ counter ];

    VENet::OP_DELETE_ARRAY(listArray, file, line);

    listArray = new_array;
}

template <class list_type>
void List<list_type>::Preallocate( unsigned countNeeded, const char *file, unsigned int line )
{
    unsigned amountToAllocate = allocation_size;
    if (allocation_size==0)
        amountToAllocate=16;
    while (amountToAllocate < countNeeded)
        amountToAllocate<<=1;

    if ( allocation_size < amountToAllocate)
    {
        list_type * new_array;

        allocation_size=amountToAllocate;

        new_array = VENet::OP_NEW_ARRAY< list_type >( allocation_size , file, line );

        if (listArray)
        {
            for ( unsigned int counter = 0; counter < list_size; ++counter )
                new_array[ counter ] = listArray[ counter ];

            VENet::OP_DELETE_ARRAY(listArray, file, line);
        }

        listArray = new_array;
    }
}

}