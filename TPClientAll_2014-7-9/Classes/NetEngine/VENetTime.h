#pragma once

#include "NativeTypes.h"
#include "VENetDefines.h"

namespace VENet {

#if __GET_TIME_64BIT==1
typedef uint64_t Time;
typedef uint32_t TimeMS;
typedef uint64_t TimeUS;
#else
typedef uint32_t Time;
typedef uint32_t TimeMS;
typedef uint64_t TimeUS;
#endif

}