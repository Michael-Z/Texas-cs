#pragma once

#include "VEAssert.h"

static const int MINIMUM_LIST_SIZE=8;

#include "VEMemoryOverride.h"
#include "Export.h"

namespace DataStructures
{
template <class SingleProducerConsumerType>
class VE_DLL_EXPORT SingleProducerConsumer
{
public:
    SingleProducerConsumer();

    ~SingleProducerConsumer();

    SingleProducerConsumerType* WriteLock(void);

    void CancelWriteLock(SingleProducerConsumerType* cancelToLocation);

    void WriteUnlock(void);

    SingleProducerConsumerType* ReadLock(void);

    void CancelReadLock(SingleProducerConsumerType* cancelToLocation);

    void ReadUnlock(void);

    void Clear(void);

    int Size(void) const;

    bool CheckReadUnlockOrder(const SingleProducerConsumerType* data) const;

    bool ReadIsLocked(void) const;

private:
    struct DataPlusPtr
    {
        DataPlusPtr ()
        {
            readyToRead=false;
        }
        SingleProducerConsumerType object;

        volatile bool readyToRead;
        volatile DataPlusPtr *next;
    };
    volatile DataPlusPtr *readAheadPointer;
    volatile DataPlusPtr *writeAheadPointer;
    volatile DataPlusPtr *readPointer;
    volatile DataPlusPtr *writePointer;
    unsigned readCount, writeCount;
};

template <class SingleProducerConsumerType>
SingleProducerConsumer<SingleProducerConsumerType>::SingleProducerConsumer()
{
    readPointer = VENet::OP_NEW<DataPlusPtr>( _FILE_AND_LINE_ );
    writePointer=readPointer;
    readPointer->next = VENet::OP_NEW<DataPlusPtr>( _FILE_AND_LINE_ );
    int listSize;
#ifdef _DEBUG
    VEAssert(MINIMUM_LIST_SIZE>=3);
#endif
    for (listSize=2; listSize < MINIMUM_LIST_SIZE; listSize++)
    {
        readPointer=readPointer->next;
        readPointer->next = VENet::OP_NEW<DataPlusPtr>( _FILE_AND_LINE_ );
    }
    readPointer->next->next=writePointer;
    readPointer=writePointer;
    readAheadPointer=readPointer;
    writeAheadPointer=writePointer;
    readCount=writeCount=0;
}

template <class SingleProducerConsumerType>
SingleProducerConsumer<SingleProducerConsumerType>::~SingleProducerConsumer()
{
    volatile DataPlusPtr *next;
    readPointer=writeAheadPointer->next;
    while (readPointer!=writeAheadPointer)
    {
        next=readPointer->next;
        VENet::OP_DELETE((char*) readPointer, _FILE_AND_LINE_);
        readPointer=next;
    }
    VENet::OP_DELETE((char*) readPointer, _FILE_AND_LINE_);
}

template <class SingleProducerConsumerType>
SingleProducerConsumerType* SingleProducerConsumer<SingleProducerConsumerType>::WriteLock( void )
{
    if (writeAheadPointer->next==readPointer ||
            writeAheadPointer->next->readyToRead==true)
    {
        volatile DataPlusPtr *originalNext=writeAheadPointer->next;
        writeAheadPointer->next=VENet::OP_NEW<DataPlusPtr>(_FILE_AND_LINE_);
        VEAssert(writeAheadPointer->next);
        writeAheadPointer->next->next=originalNext;
    }

    volatile DataPlusPtr *last;
    last=writeAheadPointer;
    writeAheadPointer=writeAheadPointer->next;

    return (SingleProducerConsumerType*) last;
}

template <class SingleProducerConsumerType>
void SingleProducerConsumer<SingleProducerConsumerType>::CancelWriteLock( SingleProducerConsumerType* cancelToLocation )
{
    writeAheadPointer=(DataPlusPtr *)cancelToLocation;
}

template <class SingleProducerConsumerType>
void SingleProducerConsumer<SingleProducerConsumerType>::WriteUnlock( void )
{
#ifdef _DEBUG
    VEAssert(writePointer->next!=readPointer);
    VEAssert(writePointer!=writeAheadPointer);
#endif

    writeCount++;
    writePointer->readyToRead=true;
    writePointer=writePointer->next;
}

template <class SingleProducerConsumerType>
SingleProducerConsumerType* SingleProducerConsumer<SingleProducerConsumerType>::ReadLock( void )
{
    if (readAheadPointer==writePointer ||
            readAheadPointer->readyToRead==false)
    {
        return 0;
    }

    volatile DataPlusPtr *last;
    last=readAheadPointer;
    readAheadPointer=readAheadPointer->next;
    return (SingleProducerConsumerType*)last;
}

template <class SingleProducerConsumerType>
void SingleProducerConsumer<SingleProducerConsumerType>::CancelReadLock( SingleProducerConsumerType* cancelToLocation )
{
#ifdef _DEBUG
    VEAssert(readPointer!=writePointer);
#endif
    readAheadPointer=(DataPlusPtr *)cancelToLocation;
}

template <class SingleProducerConsumerType>
void SingleProducerConsumer<SingleProducerConsumerType>::ReadUnlock( void )
{
#ifdef _DEBUG
    VEAssert(readAheadPointer!=readPointer);
    VEAssert(readPointer!=writePointer);
#endif
    readCount++;

    readPointer->readyToRead=false;
    readPointer=readPointer->next;
}

template <class SingleProducerConsumerType>
void SingleProducerConsumer<SingleProducerConsumerType>::Clear( void )
{
    volatile DataPlusPtr *next;
    writePointer=readPointer->next;

    int listSize=1;
    next=readPointer->next;
    while (next!=readPointer)
    {
        listSize++;
        next=next->next;
    }

    while (listSize-- > MINIMUM_LIST_SIZE)
    {
        next=writePointer->next;
#ifdef _DEBUG
        VEAssert(writePointer!=readPointer);
#endif
        VENet::OP_DELETE((char*) writePointer, _FILE_AND_LINE_);
        writePointer=next;
    }

    readPointer->next=writePointer;
    writePointer=readPointer;
    readAheadPointer=readPointer;
    writeAheadPointer=writePointer;
    readCount=writeCount=0;
}

template <class SingleProducerConsumerType>
int SingleProducerConsumer<SingleProducerConsumerType>::Size( void ) const
{
    return writeCount-readCount;
}

template <class SingleProducerConsumerType>
bool SingleProducerConsumer<SingleProducerConsumerType>::CheckReadUnlockOrder(const SingleProducerConsumerType* data) const
{
    return const_cast<const SingleProducerConsumerType *>(&readPointer->object) == data;
}


template <class SingleProducerConsumerType>
bool SingleProducerConsumer<SingleProducerConsumerType>::ReadIsLocked(void) const
{
    return readAheadPointer!=readPointer;
}
}
