#pragma once

#include "VEMemoryOverride.h"


#if   defined(_WIN32)
#include "WindowsIncludes.h"


#else
#include <pthread.h>
#include <sys/types.h>
#endif
#include "Export.h"

namespace VENet
{
class VE_DLL_EXPORT SimpleMutex
{
public:

    SimpleMutex();

    ~SimpleMutex();

    void Lock(void);

    void Unlock(void);

private:
    void Init(void);
#ifdef _WIN32
    CRITICAL_SECTION criticalSection;


#else
    pthread_mutex_t hMutex;
#endif
};

}

