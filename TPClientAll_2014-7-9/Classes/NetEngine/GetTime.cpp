#if defined(_WIN32)
#include "WindowsIncludes.h"

#pragma comment(lib, "Winmm.lib")
#endif

#include "GetTime.h"


#if defined(_WIN32)
DWORD mProcMask;
DWORD mSysMask;
HANDLE mThread;

#else
#include <sys/time.h>
#include <unistd.h>
VENet::TimeUS initialTime;
#endif

static bool initialized=false;

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
#include "SimpleMutex.h"
VENet::TimeUS lastNormalizedReturnedValue=0;
VENet::TimeUS lastNormalizedInputValue=0;
VENet::TimeUS NormalizeTime(VENet::TimeUS timeIn)
{
    VENet::TimeUS diff, lastNormalizedReturnedValueCopy;
    static VENet::SimpleMutex mutex;

    mutex.Lock();
    if (timeIn>=lastNormalizedInputValue)
    {
        diff = timeIn-lastNormalizedInputValue;
        if (diff > GET_TIME_SPIKE_LIMIT)
            lastNormalizedReturnedValue+=GET_TIME_SPIKE_LIMIT;
        else
            lastNormalizedReturnedValue+=diff;
    }
    else
        lastNormalizedReturnedValue+=GET_TIME_SPIKE_LIMIT;

    lastNormalizedInputValue=timeIn;
    lastNormalizedReturnedValueCopy=lastNormalizedReturnedValue;
    mutex.Unlock();

    return lastNormalizedReturnedValueCopy;
}
#endif
VENet::Time VENet::GetTime( void )
{
    return (VENet::Time)(GetTimeUS()/1000);
}
VENet::TimeMS VENet::GetTimeMS( void )
{
    return (VENet::TimeMS)(GetTimeUS()/1000);
}
















































#if   defined(_WIN32)
VENet::TimeUS GetTimeUS_Windows( void )
{
    if ( initialized == false)
    {
        initialized = true;

        // Save the current process
#if !defined(_WIN32_WCE)
        HANDLE mProc = GetCurrentProcess();

        // Get the current Affinity
#if _MSC_VER >= 1400 && defined (_M_X64)
        GetProcessAffinityMask(mProc, (PDWORD_PTR)&mProcMask, (PDWORD_PTR)&mSysMask);
#else
        GetProcessAffinityMask(mProc, &mProcMask, &mSysMask);
#endif
        mThread = GetCurrentThread();

#endif // _WIN32_WCE
    }

    // 9/26/2010 In China running LuDaShi, QueryPerformanceFrequency has to be called every time because CPU clock speeds can be different
    VENet::TimeUS curTime;
    LARGE_INTEGER PerfVal;
    LARGE_INTEGER yo1;

    QueryPerformanceFrequency( &yo1 );
    QueryPerformanceCounter( &PerfVal );

    __int64 quotient, remainder;
    quotient=((PerfVal.QuadPart) / yo1.QuadPart);
    remainder=((PerfVal.QuadPart) % yo1.QuadPart);
    curTime = (VENet::TimeUS) quotient*(VENet::TimeUS)1000000 + (remainder*(VENet::TimeUS)1000000 / yo1.QuadPart);

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
    return NormalizeTime(curTime);
#else
    return curTime;
#endif
}
#elif defined(__GNUC__)  || defined(__GCCXML__) ||  defined(__S3E__)
VENet::TimeUS GetTimeUS_Linux( void )
{
    timeval tp;
    if ( initialized == false)
    {
        gettimeofday( &tp, 0 );
        initialized=true;
        initialTime = ( tp.tv_sec ) * (VENet::TimeUS) 1000000 + ( tp.tv_usec );
    }

    // GCC
    VENet::TimeUS curTime;
    gettimeofday( &tp, 0 );

    curTime = ( tp.tv_sec ) * (VENet::TimeUS) 1000000 + ( tp.tv_usec );

#if defined(GET_TIME_SPIKE_LIMIT) && GET_TIME_SPIKE_LIMIT>0
    return NormalizeTime(curTime - initialTime);
#else
return curTime - initialTime;
#endif
}
#endif

VENet::TimeUS VENet::GetTimeUS( void )
{






#if   defined(_WIN32)
    return GetTimeUS_Windows();
#else
    return GetTimeUS_Linux();
#endif
}
bool VENet::GreaterThan(VENet::Time a, VENet::Time b)
{
    const VENet::Time halfSpan =(VENet::Time) (((VENet::Time)(const VENet::Time)-1)/(VENet::Time)2);
    return b!=a && b-a>halfSpan;
}
bool VENet::LessThan(VENet::Time a, VENet::Time b)
{
    const VENet::Time halfSpan = ((VENet::Time)(const VENet::Time)-1)/(VENet::Time)2;
    return b!=a && b-a<halfSpan;
}
