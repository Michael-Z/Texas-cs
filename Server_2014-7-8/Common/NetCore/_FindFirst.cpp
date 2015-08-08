#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(_WIN32)
#include "_FindFirst.h"
#include "DS_List.h"

#include <sys/stat.h>

#include <fnmatch.h>


static DataStructures::List< _findinfo_t* > fileInfo;

#include "VEMemoryOverride.h"
#include "VEAssert.h"

long _findfirst(const char *name, _finddata_t *f)
{
    VENet::VEString nameCopy = name;
    VENet::VEString filter;

    const char* lastSep = strrchr(name,'/');
    if (!lastSep)
    {
        filter = nameCopy;
        nameCopy = ".";
    }
    else
    {
        filter = lastSep+1;
        unsigned sepIndex = lastSep - name;
        nameCopy.Erase(sepIndex+1, nameCopy.GetLength() - sepIndex-1);
    }

    DIR* dir = opendir(nameCopy);

    if (!dir) return -1;

    _findinfo_t* fi = VENet::OP_NEW<_findinfo_t>( _FILE_AND_LINE_ );
    fi->filter    = filter;
    fi->dirName   = nameCopy;
    fi->openedDir = dir;
    fileInfo.Insert(fi, _FILE_AND_LINE_);

    long ret = fileInfo.Size()-1;

    if (_findnext(ret, f) == -1) return -1;
    else return ret;
}

int _findnext(long h, _finddata_t *f)
{
    VEAssert(h >= 0 && h < (long)fileInfo.Size());
    if (h < 0 || h >= (long)fileInfo.Size()) return -1;

    _findinfo_t* fi = fileInfo[h];

    while (true)
    {
        dirent* entry = readdir(fi->openedDir);
        if (entry == 0) return -1;

        if (fnmatch(fi->filter, entry->d_name, FNM_PATHNAME) != 0) continue;

        struct stat filestat;
        VENet::VEString fullPath = fi->dirName + entry->d_name;
        if (stat(fullPath, &filestat) != 0)
        {
            VENET_DEBUG_PRINTF("Cannot stat %s\n", fullPath.C_String());
            continue;
        }

        if (S_ISREG(filestat.st_mode))
        {
            f->attrib = _A_NORMAL;
        }
        else if (S_ISDIR(filestat.st_mode))
        {
            f->attrib = _A_SUBDIR;
        }
        else continue;

        f->size = filestat.st_size;
        strncpy(f->name, entry->d_name, STRING_BUFFER_SIZE);

        return 0;
    }

    return -1;
}

int _findclose(long h)
{
    if (h==-1) return 0;

    if (h < 0 || h >= (long)fileInfo.Size())
    {
        VEAssert(false);
        return -1;
    }

    _findinfo_t* fi = fileInfo[h];
    closedir(fi->openedDir);
    fileInfo.RemoveAtIndex(h);
    VENet::OP_DELETE(fi, _FILE_AND_LINE_);
    return 0;
}
#endif
