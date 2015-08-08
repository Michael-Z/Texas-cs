#include "SignaledEvent.h"
#include "VEAssert.h"
#include "VESleep.h"

#if defined(__GNUC__)
#include <sys/time.h>
#include <unistd.h>
#endif

using namespace VENet;


SignaledEvent::SignaledEvent()
{
#ifdef _WIN32
    eventList=INVALID_HANDLE_VALUE;


#else
    isSignaled=false;
#endif
}
SignaledEvent::~SignaledEvent()
{
}

void SignaledEvent::InitEvent(void)
{
#ifdef _WIN32
    eventList=CreateEvent(0, false, false, 0);
#else

#if !defined(ANDROID)
    pthread_condattr_init( &condAttr );
    pthread_cond_init(&eventList, &condAttr);
#else
    pthread_cond_init(&eventList, 0);
#endif
    pthread_mutexattr_init( &mutexAttr	);
    pthread_mutex_init(&hMutex, &mutexAttr);
#endif
}

void SignaledEvent::CloseEvent(void)
{
#ifdef _WIN32
    if (eventList!=INVALID_HANDLE_VALUE)
    {
        CloseHandle(eventList);
        eventList=INVALID_HANDLE_VALUE;
    }
#else
    pthread_cond_destroy(&eventList);
    pthread_mutex_destroy(&hMutex);
#if !defined(ANDROID)
    pthread_condattr_destroy( &condAttr );
#endif
    pthread_mutexattr_destroy( &mutexAttr );
#endif
}

void SignaledEvent::SetEvent(void)
{
#ifdef _WIN32
    ::SetEvent(eventList);
#else
    isSignaledMutex.Lock();
    isSignaled=true;
    isSignaledMutex.Unlock();

    pthread_cond_broadcast(&eventList);
#endif
}

void SignaledEvent::WaitOnEvent(int timeoutMs)
{
#ifdef _WIN32
    WaitForSingleObject(eventList,timeoutMs);
#else

    isSignaledMutex.Lock();
    if (isSignaled==true)
    {
        isSignaled=false;
        isSignaledMutex.Unlock();
        return;
    }
    isSignaledMutex.Unlock();


    struct timespec   ts;

    int rc;
    struct timeval    tp;
    rc =  gettimeofday(&tp, NULL);
    ts.tv_sec  = tp.tv_sec;
    ts.tv_nsec = tp.tv_usec * 1000;


    while (timeoutMs > 30)
    {
        ts.tv_nsec += 30*1000000;
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
        }

        pthread_mutex_lock(&hMutex);
        pthread_cond_timedwait(&eventList, &hMutex, &ts);
        pthread_mutex_unlock(&hMutex);

        timeoutMs-=30;

        isSignaledMutex.Lock();
        if (isSignaled==true)
        {
            isSignaled=false;
            isSignaledMutex.Unlock();
            return;
        }
        isSignaledMutex.Unlock();
    }

    ts.tv_nsec += timeoutMs*1000000;
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    pthread_mutex_lock(&hMutex);
    pthread_cond_timedwait(&eventList, &hMutex, &ts);
    pthread_mutex_unlock(&hMutex);

    isSignaledMutex.Lock();
    isSignaled=false;
    isSignaledMutex.Unlock();
#endif
}
