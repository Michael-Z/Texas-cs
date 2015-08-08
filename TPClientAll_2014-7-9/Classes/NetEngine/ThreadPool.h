#pragma once

#include "VEMemoryOverride.h"
#include "DS_Queue.h"
#include "SimpleMutex.h"
#include "Export.h"
#include "VEThread.h"
#include "SignaledEvent.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

class ThreadDataInterface
{
public:
    ThreadDataInterface() {}
    virtual ~ThreadDataInterface() {}

    virtual void* PerThreadFactory(void *context)=0;
    virtual void PerThreadDestructor(void* factoryResult, void *context)=0;
};
template <class InputType, class OutputType>
struct VE_DLL_EXPORT ThreadPool
{
    ThreadPool();
    ~ThreadPool();

    bool StartThreads(int numThreads, int stackSize, void* (*_perThreadInit)()=0, void (*_perThreadDeinit)(void*)=0);

    void SetThreadDataInterface(ThreadDataInterface *tdi, void *context);

    void StopThreads(void);

    void AddInput(OutputType (*workerThreadCallback)(InputType, bool *returnOutput, void* perThreadData), InputType inputData);

    void AddOutput(OutputType outputData);

    bool HasOutput(void);

    bool HasOutputFast(void);

    bool HasInput(void);

    bool HasInputFast(void);

    OutputType GetOutput(void);

    void Clear(void);

    void LockInput(void);

    void UnlockInput(void);

    unsigned InputSize(void);

    InputType GetInputAtIndex(unsigned index);

    void RemoveInputAtIndex(unsigned index);

    void LockOutput(void);

    void UnlockOutput(void);

    unsigned OutputSize(void);

    OutputType GetOutputAtIndex(unsigned index);

    void RemoveOutputAtIndex(unsigned index);

    void ClearInput(void);

    void ClearOutput(void);

    bool IsWorking(void);

    int NumThreadsWorking(void);

    bool WasStarted(void);

    bool Pause(void);

    void Resume(void);

protected:
    VENet::SimpleMutex inputQueueMutex, outputQueueMutex, workingThreadCountMutex, runThreadsMutex;

    void* (*perThreadDataFactory)();
    void (*perThreadDataDestructor)(void*);

    DataStructures::Queue<OutputType (*)(InputType, bool *, void*)> inputFunctionQueue;
    DataStructures::Queue<InputType> inputQueue;
    DataStructures::Queue<OutputType> outputQueue;

    ThreadDataInterface *threadDataInterface;
    void *tdiContext;


    template <class ThreadInputType, class ThreadOutputType>
    friend RAK_THREAD_DECLARATION(WorkerThread);

    bool runThreads;
    int numThreadsRunning;
    int numThreadsWorking;
    VENet::SimpleMutex numThreadsRunningMutex;

    VENet::SignaledEvent quitAndIncomingDataEvents;
};

#include "ThreadPool.h"
#include "VESleep.h"
#ifdef _WIN32

#else
#include <unistd.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4127)
#pragma warning( disable : 4701 )
#endif

template <class ThreadInputType, class ThreadOutputType>
VE_THREAD_DECLARATION(WorkerThread)
{



    ThreadPool<ThreadInputType, ThreadOutputType> *threadPool = (ThreadPool<ThreadInputType, ThreadOutputType>*) arguments;


    bool returnOutput;
    ThreadOutputType (*userCallback)(ThreadInputType, bool *, void*);
    ThreadInputType inputData;
    ThreadOutputType callbackOutput;

    userCallback=0;

    void *perThreadData;
    if (threadPool->perThreadDataFactory)
        perThreadData=threadPool->perThreadDataFactory();
    else if (threadPool->threadDataInterface)
        perThreadData=threadPool->threadDataInterface->PerThreadFactory(threadPool->tdiContext);
    else
        perThreadData=0;

    threadPool->numThreadsRunningMutex.Lock();
    ++threadPool->numThreadsRunning;
    threadPool->numThreadsRunningMutex.Unlock();

    while (1)
    {
        if (userCallback==0)
        {
            threadPool->quitAndIncomingDataEvents.WaitOnEvent(1000);
        }

        threadPool->runThreadsMutex.Lock();
        if (threadPool->runThreads==false)
        {
            threadPool->runThreadsMutex.Unlock();
            break;
        }
        threadPool->runThreadsMutex.Unlock();

        threadPool->workingThreadCountMutex.Lock();
        ++threadPool->numThreadsWorking;
        threadPool->workingThreadCountMutex.Unlock();

        userCallback=0;
        threadPool->inputQueueMutex.Lock();
        if (threadPool->inputFunctionQueue.Size())
        {
            userCallback=threadPool->inputFunctionQueue.Pop();
            inputData=threadPool->inputQueue.Pop();
        }
        threadPool->inputQueueMutex.Unlock();

        if (userCallback)
        {
            callbackOutput=userCallback(inputData, &returnOutput,perThreadData);
            if (returnOutput)
            {
                threadPool->outputQueueMutex.Lock();
                threadPool->outputQueue.Push(callbackOutput, _FILE_AND_LINE_ );
                threadPool->outputQueueMutex.Unlock();
            }
        }

        threadPool->workingThreadCountMutex.Lock();
        --threadPool->numThreadsWorking;
        threadPool->workingThreadCountMutex.Unlock();
    }

    threadPool->numThreadsRunningMutex.Lock();
    --threadPool->numThreadsRunning;
    threadPool->numThreadsRunningMutex.Unlock();

    if (threadPool->perThreadDataDestructor)
        threadPool->perThreadDataDestructor(perThreadData);
    else if (threadPool->threadDataInterface)
        threadPool->threadDataInterface->PerThreadDestructor(perThreadData, threadPool->tdiContext);

    return 0;

}
template <class InputType, class OutputType>
ThreadPool<InputType, OutputType>::ThreadPool()
{
    runThreads=false;
    numThreadsRunning=0;
    threadDataInterface=0;
    tdiContext=0;
    numThreadsWorking=0;

}
template <class InputType, class OutputType>
ThreadPool<InputType, OutputType>::~ThreadPool()
{
    StopThreads();
    Clear();
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::StartThreads(int numThreads, int stackSize, void* (*_perThreadDataFactory)(), void (*_perThreadDataDestructor)(void *))
{
    (void) stackSize;

    runThreadsMutex.Lock();
    if (runThreads==true)
    {
        runThreadsMutex.Unlock();
        return false;
    }
    runThreadsMutex.Unlock();

    quitAndIncomingDataEvents.InitEvent();

    perThreadDataFactory=_perThreadDataFactory;
    perThreadDataDestructor=_perThreadDataDestructor;

    runThreadsMutex.Lock();
    runThreads=true;
    runThreadsMutex.Unlock();

    numThreadsWorking=0;
    unsigned threadId = 0;
    (void) threadId;
    int i;
    for (i=0; i < numThreads; i++)
    {
        int errorCode;

        errorCode = VENet::RakThread::Create(WorkerThread<InputType, OutputType>, this);

        if (errorCode!=0)
        {
            StopThreads();
            return false;
        }
    }
    bool done=false;
    while (done==false)
    {
        VESleep(50);
        numThreadsRunningMutex.Lock();
        if (numThreadsRunning==numThreads)
            done=true;
        numThreadsRunningMutex.Unlock();
    }

    return true;
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::SetThreadDataInterface(ThreadDataInterface *tdi, void *context)
{
    threadDataInterface=tdi;
    tdiContext=context;
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::StopThreads(void)
{
    runThreadsMutex.Lock();
    if (runThreads==false)
    {
        runThreadsMutex.Unlock();
        return;
    }

    runThreads=false;
    runThreadsMutex.Unlock();

    bool done=false;
    while (done==false)
    {
        quitAndIncomingDataEvents.SetEvent();

        VESleep(50);
        numThreadsRunningMutex.Lock();
        if (numThreadsRunning==0)
            done=true;
        numThreadsRunningMutex.Unlock();
    }

    quitAndIncomingDataEvents.CloseEvent();

}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::AddInput(OutputType (*workerThreadCallback)(InputType, bool *returnOutput, void* perThreadData), InputType inputData)
{
    inputQueueMutex.Lock();
    inputQueue.Push(inputData, _FILE_AND_LINE_ );
    inputFunctionQueue.Push(workerThreadCallback, _FILE_AND_LINE_ );
    inputQueueMutex.Unlock();

    quitAndIncomingDataEvents.SetEvent();
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::AddOutput(OutputType outputData)
{
    outputQueueMutex.Lock();
    outputQueue.Push(outputData, _FILE_AND_LINE_ );
    outputQueueMutex.Unlock();
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::HasOutputFast(void)
{
    return outputQueue.IsEmpty()==false;
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::HasOutput(void)
{
    bool res;
    outputQueueMutex.Lock();
    res=outputQueue.IsEmpty()==false;
    outputQueueMutex.Unlock();
    return res;
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::HasInputFast(void)
{
    return inputQueue.IsEmpty()==false;
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::HasInput(void)
{
    bool res;
    inputQueueMutex.Lock();
    res=inputQueue.IsEmpty()==false;
    inputQueueMutex.Unlock();
    return res;
}
template <class InputType, class OutputType>
OutputType ThreadPool<InputType, OutputType>::GetOutput(void)
{
    OutputType output;
    outputQueueMutex.Lock();
    output=outputQueue.Pop();
    outputQueueMutex.Unlock();
    return output;
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::Clear(void)
{
    runThreadsMutex.Lock();
    if (runThreads)
    {
        runThreadsMutex.Unlock();
        inputQueueMutex.Lock();
        inputFunctionQueue.Clear(_FILE_AND_LINE_);
        inputQueue.Clear(_FILE_AND_LINE_);
        inputQueueMutex.Unlock();

        outputQueueMutex.Lock();
        outputQueue.Clear(_FILE_AND_LINE_);
        outputQueueMutex.Unlock();
    }
    else
    {
        inputFunctionQueue.Clear(_FILE_AND_LINE_);
        inputQueue.Clear(_FILE_AND_LINE_);
        outputQueue.Clear(_FILE_AND_LINE_);
    }
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::LockInput(void)
{
    inputQueueMutex.Lock();
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::UnlockInput(void)
{
    inputQueueMutex.Unlock();
}
template <class InputType, class OutputType>
unsigned ThreadPool<InputType, OutputType>::InputSize(void)
{
    return inputQueue.Size();
}
template <class InputType, class OutputType>
InputType ThreadPool<InputType, OutputType>::GetInputAtIndex(unsigned index)
{
    return inputQueue[index];
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::RemoveInputAtIndex(unsigned index)
{
    inputQueue.RemoveAtIndex(index);
    inputFunctionQueue.RemoveAtIndex(index);
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::LockOutput(void)
{
    outputQueueMutex.Lock();
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::UnlockOutput(void)
{
    outputQueueMutex.Unlock();
}
template <class InputType, class OutputType>
unsigned ThreadPool<InputType, OutputType>::OutputSize(void)
{
    return outputQueue.Size();
}
template <class InputType, class OutputType>
OutputType ThreadPool<InputType, OutputType>::GetOutputAtIndex(unsigned index)
{
    return outputQueue[index];
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::RemoveOutputAtIndex(unsigned index)
{
    outputQueue.RemoveAtIndex(index);
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::ClearInput(void)
{
    inputQueue.Clear(_FILE_AND_LINE_);
    inputFunctionQueue.Clear(_FILE_AND_LINE_);
}

template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::ClearOutput(void)
{
    outputQueue.Clear(_FILE_AND_LINE_);
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::IsWorking(void)
{
    bool isWorking;
    if (HasOutputFast() && HasOutput())
        return true;

    if (HasInputFast() && HasInput())
        return true;

    workingThreadCountMutex.Lock();
    isWorking=numThreadsWorking!=0;
    workingThreadCountMutex.Unlock();

    return isWorking;
}

template <class InputType, class OutputType>
int ThreadPool<InputType, OutputType>::NumThreadsWorking(void)
{
    return numThreadsWorking;
}

template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::WasStarted(void)
{
    bool b;
    runThreadsMutex.Lock();
    b = runThreads;
    runThreadsMutex.Unlock();
    return b;
}
template <class InputType, class OutputType>
bool ThreadPool<InputType, OutputType>::Pause(void)
{
    if (WasStarted()==false)
        return false;

    workingThreadCountMutex.Lock();
    while (numThreadsWorking>0)
    {
        VESleep(30);
    }
    return true;
}
template <class InputType, class OutputType>
void ThreadPool<InputType, OutputType>::Resume(void)
{
    workingThreadCountMutex.Unlock();
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

