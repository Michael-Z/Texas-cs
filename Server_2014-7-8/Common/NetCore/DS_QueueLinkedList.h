////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_QueueLinkedList.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DS_LinkedList.h"
#include "Export.h"
#include "VEMemoryOverride.h"

namespace DataStructures
{
template <class QueueType>
class VE_DLL_EXPORT QueueLinkedList
{

public:
    QueueLinkedList();
    QueueLinkedList( const QueueLinkedList& original_copy );
    bool operator= ( const QueueLinkedList& original_copy );
    QueueType Pop( void );
    QueueType& Peek( void );
    QueueType& EndPeek( void );
    void Push( const QueueType& input );
    unsigned int Size( void );
    void Clear( void );
    void Compress( void );

private:
    LinkedList<QueueType> data;
};

template <class QueueType>
QueueLinkedList<QueueType>::QueueLinkedList()
{
}

template <class QueueType>
inline unsigned int QueueLinkedList<QueueType>::Size()
{
    return data.Size();
}

template <class QueueType>
inline QueueType QueueLinkedList<QueueType>::Pop( void )
{
    data.Beginning();
    return ( QueueType ) data.Pop();
}

template <class QueueType>
inline QueueType& QueueLinkedList<QueueType>::Peek( void )
{
    data.Beginning();
    return ( QueueType ) data.Peek();
}

template <class QueueType>
inline QueueType& QueueLinkedList<QueueType>::EndPeek( void )
{
    data.End();
    return ( QueueType ) data.Peek();
}

template <class QueueType>
void QueueLinkedList<QueueType>::Push( const QueueType& input )
{
    data.End();
    data.Add( input );
}

template <class QueueType>
QueueLinkedList<QueueType>::QueueLinkedList( const QueueLinkedList& original_copy )
{
    data = original_copy.data;
}

template <class QueueType>
bool QueueLinkedList<QueueType>::operator= ( const QueueLinkedList& original_copy )
{
    if ( ( &original_copy ) == this )
        return false;

    data = original_copy.data;
}

template <class QueueType>
void QueueLinkedList<QueueType>::Clear ( void )
{
    data.Clear();
}
}