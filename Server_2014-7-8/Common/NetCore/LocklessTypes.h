////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 LocklessTypes.h
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
#include "NativeTypes.h"
#include "WindowsIncludes.h"
#if defined(ANDROID) || defined(__S3E__) || defined(__APPLE__)
#include "SimpleMutex.h"
#endif

namespace VENet
{

class VE_DLL_EXPORT LocklessUint32_t
{
public:
    LocklessUint32_t();
    explicit LocklessUint32_t(uint32_t initial);
    uint32_t Increment(void);
    uint32_t Decrement(void);
    uint32_t GetValue(void) const
    {
        return value;
    }

protected:
#ifdef _WIN32
    volatile LONG value;
#elif defined(ANDROID) || defined(__S3E__) || defined(__APPLE__)
    SimpleMutex mutex;
    uint32_t value;
#else
    volatile uint32_t value;
#endif
};

}