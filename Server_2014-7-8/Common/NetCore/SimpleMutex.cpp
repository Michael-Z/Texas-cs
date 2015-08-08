#include "SimpleMutex.h"
#include "VEAssert.h"

using namespace VENet;

SimpleMutex::SimpleMutex()
{
    Init();
}

SimpleMutex::~SimpleMutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&criticalSection);
#else
    pthread_mutex_destroy(&hMutex);
#endif

}

#ifdef _WIN32
#ifdef _DEBUG
#include <stdio.h>
#endif
#endif

void SimpleMutex::Lock(void)
{

#ifdef _WIN32
    EnterCriticalSection(&criticalSection);
#else
    int error = pthread_mutex_lock(&hMutex);
    (void) error;
    VEAssert(error==0);
#endif
}

void SimpleMutex::Unlock(void)
{
#ifdef _WIN32
    LeaveCriticalSection(&criticalSection);
#else
    int error = pthread_mutex_unlock(&hMutex);
    (void) error;
    VEAssert(error==0);
#endif
}

void SimpleMutex::Init(void)
{
#ifdef _WIN32
    InitializeCriticalSection(&criticalSection);
#else
    int error = pthread_mutex_init(&hMutex, 0);
    (void) error;
    VEAssert(error==0);
#endif
}
