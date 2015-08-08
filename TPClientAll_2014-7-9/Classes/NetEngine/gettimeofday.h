////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 gettimeofday.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#if defined(_WIN32) && !defined(__GNUC__)  &&!defined(__GCCXML__)
#include < time.h >
struct timezone
{
  int  tz_minuteswest;
  int  tz_dsttime;
};
int gettimeofday(struct timeval *tv, struct timezone *tz);


#else


#include <sys/time.h>

#include <unistd.h>


#endif
