////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_LinkedList.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Export.h"
#include "VEMemoryOverride.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

namespace DataStructures
{
template <class LinkedListType>
class VE_DLL_EXPORT LinkedList;

template <class CircularLinkedListType>

class CircularLinkedList
{

public:

    struct node
    {
        CircularLinkedListType item;

        node* previous;
        node* next;
    };

    CircularLinkedList();
    ~CircularLinkedList();
    CircularLinkedList( const CircularLinkedList& original_copy );
    bool operator= ( const CircularLinkedList& original_copy );
    CircularLinkedList& operator++();
    CircularLinkedList& operator++( int );
    CircularLinkedList& operator--();
    CircularLinkedList& operator--( int );
    bool IsIn( const CircularLinkedListType& input );
    bool Find( const CircularLinkedListType& input );
    void Insert( const CircularLinkedListType& input );

    CircularLinkedListType& Add ( const CircularLinkedListType& input );

    void Replace( const CircularLinkedListType& input );

    void Del( void );

    unsigned int Size( void );

    CircularLinkedListType& Peek( void );

    CircularLinkedListType Pop( void );

    void Clear( void );

    void Sort( void );

    void Beginning( void );

    void End( void );

    void Concatenate( const CircularLinkedList& L );

protected:
    unsigned int list_size;

    node *root;

    node *position;

    node* FindPointer( const CircularLinkedListType& input );

private:
    CircularLinkedList Merge( CircularLinkedList L1, CircularLinkedList L2 );

    CircularLinkedList Mergesort( const CircularLinkedList& L );
};

template <class LinkedListType>

class LinkedList : public CircularLinkedList<LinkedListType>
{

public:
    LinkedList()
    {}

    LinkedList( const LinkedList& original_copy );
    ~LinkedList();
    bool operator= ( const LinkedList<LinkedListType>& original_copy );
    LinkedList& operator++();
    LinkedList& operator++( int );
    LinkedList& operator--();
    LinkedList& operator--( int );

private:
    LinkedList Merge( LinkedList L1, LinkedList L2 );
    LinkedList Mergesort( const LinkedList& L );

};


template <class CircularLinkedListType>
inline void CircularLinkedList<CircularLinkedListType>::Beginning( void )
{
    if ( this->root )
        this->position = this->root;
}

template <class CircularLinkedListType>
inline void CircularLinkedList<CircularLinkedListType>::End( void )
{
    if ( this->root )
        this->position = this->root->previous;
}

template <class LinkedListType>
bool LinkedList<LinkedListType>::operator= ( const LinkedList<LinkedListType>& original_copy )
{
    typename LinkedList::node * original_copy_pointer, *last, *save_position;

    if ( ( &original_copy ) != this )
    {

        this->Clear();


        if ( original_copy.list_size == 0 )
        {
            this->root = 0;
            this->position = 0;
            this->list_size = 0;
        }

        else if ( original_copy.list_size == 1 )
        {
            this->root = VENet::OP_NEW<typename LinkedList::node>( _FILE_AND_LINE_ );
            this->root->next = this->root;
            this->root->previous = this->root;
            this->list_size = 1;
            this->position = this->root;
            this->root->item = original_copy.root->item;
        }

        else
        {
            original_copy_pointer = original_copy.root;
            this->root = VENet::OP_NEW<typename LinkedList::node>( _FILE_AND_LINE_ );
            this->position = this->root;
            this->root->item = original_copy.root->item;

            if ( original_copy_pointer == original_copy.position )
                save_position = this->position;

            do
            {
                last = this->position;

                original_copy_pointer = original_copy_pointer->next;

                this->position = VENet::OP_NEW<typename LinkedList::node>( _FILE_AND_LINE_ );
                this->position->item = original_copy_pointer->item;

                if ( original_copy_pointer == original_copy.position )
                    save_position = this->position;


                ( this->position->previous ) = last;

                ( last->next ) = this->position;
            }

            while ( ( original_copy_pointer->next ) != ( original_copy.root ) );

            this->position->next = this->root;

            this->root->previous = this->position;

            this->list_size = original_copy.list_size;

            this->position = save_position;
        }
    }

    return true;
}


template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>::CircularLinkedList()
{
    this->root = 0;
    this->position = 0;
    this->list_size = 0;
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>::~CircularLinkedList()
{
    this->Clear();
}

template <class LinkedListType>
LinkedList<LinkedListType>::~LinkedList()
{
    this->Clear();
}

template <class LinkedListType>
LinkedList<LinkedListType>::LinkedList( const LinkedList& original_copy )
{
    typename LinkedList::node * original_copy_pointer, *last, *save_position;

    if ( original_copy.list_size == 0 )
    {
        this->root = 0;
        this->position = 0;
        this->list_size = 0;
        return ;
    }

    else if ( original_copy.list_size == 1 )
    {
        this->root = VENet::OP_NEW<typename LinkedList::node>( _FILE_AND_LINE_ );
        this->root->next = this->root;
        this->root->previous = this->root;
        this->list_size = 1;
        this->position = this->root;
        this->root->item = original_copy.root->item;
    }

    else
    {
        original_copy_pointer = original_copy.root;
        this->root = VENet::OP_NEW<typename LinkedList::node>( _FILE_AND_LINE_ );
        this->position = this->root;
        this->root->item = original_copy.root->item;

        if ( original_copy_pointer == original_copy.position )
            save_position = this->position;

        do
        {
            last = this->position;

            original_copy_pointer = original_copy_pointer->next;

            this->position = VENet::OP_NEW<typename LinkedList::node>( _FILE_AND_LINE_ );
            this->position->item = original_copy_pointer->item;

            if ( original_copy_pointer == original_copy.position )
                save_position = this->position;

            ( this->position->previous ) = last;

            ( last->next ) = this->position;

        }

        while ( ( original_copy_pointer->next ) != ( original_copy.root ) );

        this->position->next = this->root;

        this->root->previous = this->position;

        this->list_size = original_copy.list_size;

        this->position = save_position;
    }
}

#ifdef _MSC_VER
#pragma warning( disable : 4701 )
#endif
template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>::CircularLinkedList( const CircularLinkedList& original_copy )
{
    node * original_copy_pointer;
    node *last;
    node *save_position;

    if ( original_copy.list_size == 0 )
    {
        this->root = 0;
        this->position = 0;
        this->list_size = 0;
        return ;
    }

    else if ( original_copy.list_size == 1 )
    {
        this->root = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
        this->root->next = this->root;
        this->root->previous = this->root;
        this->list_size = 1;
        this->position = this->root;
        this->root->item = original_copy.root->item;
    }

    else
    {
        original_copy_pointer = original_copy.root;
        this->root = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
        this->position = this->root;
        this->root->item = original_copy.root->item;

        if ( original_copy_pointer == original_copy.position )
            save_position = this->position;

        do
        {
            last = this->position;

            original_copy_pointer = original_copy_pointer->next;

            this->position = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );

            this->position->item = original_copy_pointer->item;

            if ( original_copy_pointer == original_copy.position )
                save_position = position;

            ( this->position->previous ) = last;

            ( last->next ) = this->position;

        }

        while ( ( original_copy_pointer->next ) != ( original_copy.root ) );

        this->position->next = this->root;

        this->root->previous = position;

        this->list_size = original_copy.list_size;

        this->position = save_position;
    }
}

#ifdef _MSC_VER
#pragma warning( disable : 4701 )
#endif
template <class CircularLinkedListType>
bool CircularLinkedList<CircularLinkedListType>::operator= ( const CircularLinkedList& original_copy )
{
    node * original_copy_pointer;
    node *last;
    node *save_position;

    if ( ( &original_copy ) != this )
    {

        this->Clear();


        if ( original_copy.list_size == 0 )
        {
            this->root = 0;
            this->position = 0;
            this->list_size = 0;
        }

        else if ( original_copy.list_size == 1 )
        {
            this->root = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
            this->root->next = this->root;
            this->root->previous = this->root;
            this->list_size = 1;
            this->position = this->root;
            this->root->item = original_copy.root->item;
        }

        else
        {
            original_copy_pointer = original_copy.root;
            this->root = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
            this->position = this->root;
            this->root->item = original_copy.root->item;

            if ( original_copy_pointer == original_copy.position )
                save_position = this->position;

            do
            {
                last = this->position;

                original_copy_pointer = original_copy_pointer->next;

                this->position = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );

                this->position->item = original_copy_pointer->item;

                if ( original_copy_pointer == original_copy.position )
                    save_position = this->position;

                ( this->position->previous ) = last;

                ( last->next ) = this->position;

            }

            while ( ( original_copy_pointer->next ) != ( original_copy.root ) );

            this->position->next = this->root;

            this->root->previous = this->position;

            this->list_size = original_copy.list_size;

            this->position = save_position;
        }
    }

    return true;
}

template <class CircularLinkedListType>
void CircularLinkedList<CircularLinkedListType>::Insert( const CircularLinkedListType& input )
{
    node * new_node;

    if ( list_size == 0 )
    {
        this->root = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
        this->root->item = input;
        this->root->next = this->root;
        this->root->previous = this->root;
        this->list_size = 1;
        this->position = this->root;
    }

    else if ( list_size == 1 )
    {
        this->position = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
        this->root->next = this->position;
        this->root->previous = this->position;
        this->position->previous = this->root;
        this->position->next = this->root;
        this->position->item = input;
        this->root = this->position;
        this->list_size = 2;
    }

    else
    {
        new_node = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );

        new_node->item = input;

        ( this->position->previous ) ->next = new_node;

        new_node->previous = this->position->previous;

        this->position->previous = new_node;

        new_node->next = this->position;

        if ( this->position == this->root )
        {
            this->root = new_node;
            this->position = this->root;
        }

        this->list_size++;
    }
}

template <class CircularLinkedListType>
CircularLinkedListType& CircularLinkedList<CircularLinkedListType>::Add ( const CircularLinkedListType& input )
{
    node * new_node;

    if ( this->list_size == 0 )
    {
        this->root = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
        this->root->item = input;
        this->root->next = this->root;
        this->root->previous = this->root;
        this->list_size = 1;
        this->position = this->root;
        return this->position->item;
    }

    else if ( list_size == 1 )
    {
        this->position = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );
        this->root->next = this->position;
        this->root->previous = this->position;
        this->position->previous = this->root;
        this->position->next = this->root;
        this->position->item = input;
        this->list_size = 2;
        this->position = this->root;
        return this->position->item;
    }

    else
    {
        new_node = VENet::OP_NEW<typename CircularLinkedList::node>( _FILE_AND_LINE_ );

        new_node->item = input;

        new_node->previous = this->position;

        new_node->next = ( this->position->next );

        ( this->position->next ) ->previous = new_node;

        ( this->position->next ) = new_node;

        this->list_size++;

        return new_node->item;
    }
}

template <class CircularLinkedListType>
inline void CircularLinkedList<CircularLinkedListType>::Replace( const CircularLinkedListType& input )
{
    if ( this->list_size > 0 )
        this->position->item = input;
}

template <class CircularLinkedListType>
void CircularLinkedList<CircularLinkedListType>::Del()
{
    node * new_position;

    if ( this->list_size == 0 )
        return ;

    else if ( this->list_size == 1 )
    {
        VENet::OP_DELETE(this->root, _FILE_AND_LINE_);
        this->root = this->position = 0;
        this->list_size = 0;
    }

    else
    {
        ( this->position->previous ) ->next = this->position->next;
        ( this->position->next ) ->previous = this->position->previous;
        new_position = this->position->next;

        if ( this->position == this->root )
            this->root = new_position;

        VENet::OP_DELETE(this->position, _FILE_AND_LINE_);

        this->position = new_position;

        this->list_size--;
    }
}

template <class CircularLinkedListType>
bool CircularLinkedList<CircularLinkedListType>::IsIn(const CircularLinkedListType& input )
{
    node * return_value, *old_position;

    old_position = this->position;

    return_value = FindPointer( input );
    this->position = old_position;

    if ( return_value != 0 )
        return true;
    else
        return false;
}

template <class CircularLinkedListType>
bool CircularLinkedList<CircularLinkedListType>::Find( const CircularLinkedListType& input )
{
    node * return_value;

    return_value = FindPointer( input );

    if ( return_value != 0 )
    {
        this->position = return_value;
        return true;
    }

    else
        return false;
}

template <class CircularLinkedListType>
typename CircularLinkedList<CircularLinkedListType>::node* CircularLinkedList<CircularLinkedListType>::FindPointer( const CircularLinkedListType& input )
{
    node * current;

    if ( this->list_size == 0 )
        return 0;

    current = this->root;

    do
    {
        if ( current->item == input )
            return current;

        current = current->next;
    }

    while ( current != this->root );

    return 0;

}

template <class CircularLinkedListType>
inline unsigned int CircularLinkedList<CircularLinkedListType>::Size( void )
{
    return this->list_size;
}

template <class CircularLinkedListType>
inline CircularLinkedListType& CircularLinkedList<CircularLinkedListType>::Peek( void )
{
    return this->position->item;
}

template <class CircularLinkedListType>
CircularLinkedListType CircularLinkedList<CircularLinkedListType>::Pop( void )
{
    CircularLinkedListType element;
    element = Peek();
    Del();
    return CircularLinkedListType( element );
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>& CircularLinkedList<CircularLinkedListType>::operator++()
{
    if ( this->list_size != 0 )
        position = position->next;

    return *this;
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>& CircularLinkedList<CircularLinkedListType>::operator++( int )
{
    return this->operator++();
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>& CircularLinkedList<CircularLinkedListType>::operator--()
{
    if ( this->list_size != 0 )
        this->position = this->position->previous;

    return *this;
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType>& CircularLinkedList<CircularLinkedListType>::operator--( int )
{
    return this->operator--();
}

template <class CircularLinkedListType>
void CircularLinkedList<CircularLinkedListType>::Clear( void )
{
    if ( this->list_size == 0 )
        return ;
    else if ( this->list_size == 1 )
    {
        VENet::OP_DELETE(this->root, _FILE_AND_LINE_);
    }

    else
    {
        node* current;
        node* temp;

        current = this->root;

        do
        {
            temp = current;
            current = current->next;
            VENet::OP_DELETE(temp, _FILE_AND_LINE_);
        }

        while ( current != this->root );
    }

    this->list_size = 0;
    this->root = 0;
    this->position = 0;
}

template <class CircularLinkedListType>
inline void CircularLinkedList<CircularLinkedListType>::Concatenate( const CircularLinkedList<CircularLinkedListType>& L )
{
    unsigned int counter;
    node* ptr;

    if ( L.list_size == 0 )
        return ;

    if ( this->list_size == 0 )
        * this = L;

    ptr = L.root;

    this->position = this->root->previous;

    for ( counter = 0; counter < L.list_size; counter++ )
    {
        Add ( ptr->item );

        ptr = ptr->next;

        this->position = this->position->next;
    }
}

template <class CircularLinkedListType>
inline void CircularLinkedList<CircularLinkedListType>::Sort( void )
{
    if ( this->list_size <= 1 )
        return ;

    *this = Mergesort( *this );

    this->position = this->root;
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType> CircularLinkedList<CircularLinkedListType>::Mergesort( const CircularLinkedList& L )
{
    unsigned int counter;
    node* location;
    CircularLinkedList<CircularLinkedListType> L1;
    CircularLinkedList<CircularLinkedListType> L2;

    location = L.root;

    for ( counter = 0; counter < L.list_size / 2; counter++ )
    {
        L1.Add ( location->item );
        location = location->next;
    }

    for ( ; counter < L.list_size; counter++ )
    {
        L2.Add ( location->item );
        location = location->next;
    }

    if ( L1.list_size > 1 )
        L1 = Mergesort( L1 );

    if ( L2.list_size > 1 )
        L2 = Mergesort( L2 );

    return Merge( L1, L2 );
}

template <class CircularLinkedListType>
CircularLinkedList<CircularLinkedListType> CircularLinkedList<CircularLinkedListType>::Merge( CircularLinkedList L1, CircularLinkedList L2 )
{
    CircularLinkedList<CircularLinkedListType> X;
    CircularLinkedListType element;
    L1.position = L1.root;
    L2.position = L2.root;

    while ( ( L1.list_size != 0 ) && ( L2.list_size != 0 ) )
    {
        if ( ( ( L1.root ) ->item ) < ( ( L2.root ) ->item ) )
        {
            element = ( L1.root ) ->item;
            L1.Del();
        }
        else
        {
            element = ( L2.root ) ->item;
            L2.Del();
        }

        X.Add( element );

        X++;
    }

    if ( L1.list_size != 0 )
        X.Concatenate( L1 );
    else
        X.Concatenate( L2 );

    return X;
}

template <class LinkedListType>
LinkedList<LinkedListType> LinkedList<LinkedListType>::Mergesort( const LinkedList& L )
{
    unsigned int counter;
    typename LinkedList::node* location;
    LinkedList<LinkedListType> L1;
    LinkedList<LinkedListType> L2;

    location = L.root;

    for ( counter = 0; counter < L.LinkedList_size / 2; counter++ )
    {
        L1.Add ( location->item );
        location = location->next;
    }

    for ( ; counter < L.LinkedList_size; counter++ )
    {
        L2.Add ( location->item );
        location = location->next;
    }

    if ( L1.list_size > 1 )
        L1 = Mergesort( L1 );

    if ( L2.list_size > 1 )
        L2 = Mergesort( L2 );

    return Merge( L1, L2 );
}

template <class LinkedListType>
LinkedList<LinkedListType> LinkedList<LinkedListType>::Merge( LinkedList L1, LinkedList L2 )
{
    LinkedList<LinkedListType> X;
    LinkedListType element;
    L1.position = L1.root;
    L2.position = L2.root;

    while ( ( L1.LinkedList_size != 0 ) && ( L2.LinkedList_size != 0 ) )
    {
        if ( ( ( L1.root ) ->item ) < ( ( L2.root ) ->item ) )
        {
            element = ( L1.root ) ->item;
            L1.Del();
        }
        else
        {
            element = ( L2.root ) ->item;
            L2.Del();
        }

        X.Add( element );
    }

    if ( L1.LinkedList_size != 0 )
        X.concatenate( L1 );
    else
        X.concatenate( L2 );

    return X;
}


template <class LinkedListType>
LinkedList<LinkedListType>& LinkedList<LinkedListType>::operator++()
{
    if ( ( this->list_size != 0 ) && ( this->position->next != this->root ) )
        this->position = this->position->next;

    return *this;
}

template <class LinkedListType>
LinkedList<LinkedListType>& LinkedList<LinkedListType>::operator++( int )
{
    return this->operator++();
}

template <class LinkedListType>
LinkedList<LinkedListType>& LinkedList<LinkedListType>::operator--()
{
    if ( ( this->list_size != 0 ) && ( this->position != this->root ) )
        this->position = this->position->previous;

    return *this;
}

template <class LinkedListType>
LinkedList<LinkedListType>& LinkedList<LinkedListType>::operator--( int )
{
    return this->operator--();
}

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif