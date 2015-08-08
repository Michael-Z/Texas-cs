////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _FindFirst.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#if (defined(__GNUC__) || defined(__ARMCC_VERSION) || defined(__GCCXML__) || defined(__S3E__) ) && !defined(__WIN32)

#include <dirent.h>

#include "VEString.h"

#define _A_NORMAL 		0x00
#define _A_RDONLY 		0x01
#define _A_HIDDEN 		0x02
#define _A_SYSTEM 		0x04
#define _A_VOLID 		0x08
#define _A_SUBDIR 		0x10
#define _A_ARCH 		0x20
#define FA_NORMAL 		0x00
#define FA_RDONLY 		0x01
#define FA_HIDDEN 		0x02
#define FA_SYSTEM 		0x04
#define FA_LABEL 		0x08
#define FA_DIREC 		0x10
#define FA_ARCH 		0x20


const unsigned STRING_BUFFER_SIZE = 512;

typedef struct _finddata_t
{
    char            name[STRING_BUFFER_SIZE];
    int            attrib;
    unsigned long   size;
} _finddata;

typedef struct _findinfo_t
{
    DIR*	openedDir;
    VENet::VEString filter;
    VENet::VEString dirName;
} _findinfo;

long _findfirst(const char *name, _finddata_t *f);
int _findnext(long h, _finddata_t *f);
int _findclose(long h);

#endif