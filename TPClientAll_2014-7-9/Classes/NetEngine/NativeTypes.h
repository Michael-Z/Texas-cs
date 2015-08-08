////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 NativeTypes.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(__GNUC__) || defined(__GCCXML__) || defined(__SNC__) || defined(__S3E__)
#include <stdint.h>
#elif !defined(_STDINT_H) && !defined(_SN_STDINT_H) && !defined(_SYS_STDINT_H_) && !defined(_STDINT) && !defined(_MACHTYPES_H_) && !defined(_STDINT_H_)
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned __int32    uint32_t;
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef __int32				int32_t;
#if defined(_MSC_VER) && _MSC_VER < 1300
typedef unsigned __int64    uint64_t;
typedef signed __int64   	int64_t;
#else
typedef unsigned long long int    uint64_t;
typedef signed long long   	int64_t;
#endif
#endif
