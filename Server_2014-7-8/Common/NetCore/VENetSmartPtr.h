////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 VENetSmartPtr.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VEMemoryOverride.h"
#include "Export.h"

namespace VENet
{

class VE_DLL_EXPORT ReferenceCounter
{
private:
    int refCount;

public:
    ReferenceCounter()
    {
        refCount=0;
    }
    ~ReferenceCounter() {}
    void AddRef()
    {
        refCount++;
    }
    int Release()
    {
        return --refCount;
    }
    int GetRefCount(void) const
    {
        return refCount;
    }
};

template < typename T > class VE_DLL_EXPORT VENetSmartPtr
{
private:
    T*    ptr;
    ReferenceCounter* reference;

public:
    VENetSmartPtr() : ptr(0), reference(0)
    {
    }

    VENetSmartPtr(T* pValue) : ptr(pValue)
    {
        reference = VENet::OP_NEW<ReferenceCounter>(_FILE_AND_LINE_);
        reference->AddRef();

    }

    VENetSmartPtr(const VENetSmartPtr<T>& sp) : ptr(sp.ptr), reference(sp.reference)
    {
        if (reference)
            reference->AddRef();
    }

    ~VENetSmartPtr()
    {
        if (reference && reference->Release() == 0)
        {
            VENet::OP_DELETE(ptr, _FILE_AND_LINE_);
            VENet::OP_DELETE(reference, _FILE_AND_LINE_);

        }
    }

    bool IsNull(void) const
    {
        return ptr==0;
    }

    void SetNull(void)
    {
        if (reference && reference->Release() == 0)
        {
            VENet::OP_DELETE(ptr, _FILE_AND_LINE_);
            VENet::OP_DELETE(reference, _FILE_AND_LINE_);

        }
        ptr=0;
        reference=0;
    }

    bool IsUnique(void) const
    {
        return reference->GetRefCount()==1;
    }

    void Clone(bool copyContents)
    {
        if (IsUnique()==false)
        {
            reference->Release();

            reference = VENet::OP_NEW<ReferenceCounter>(_FILE_AND_LINE_);
            reference->AddRef();
            T* oldPtr=ptr;
            ptr=VENet::OP_NEW<T>(_FILE_AND_LINE_);
            if (copyContents)
                *ptr=*oldPtr;
        }
    }

    int GetRefCount(void) const
    {
        return reference->GetRefCount();
    }

    T& operator* ()
    {
        return *ptr;
    }

    const T& operator* () const
    {
        return *ptr;
    }

    T* operator-> ()
    {
        return ptr;
    }

    const T* operator-> () const
    {
        return ptr;
    }

    bool operator == (const VENetSmartPtr<T>& sp)
    {
        return ptr == sp.ptr;
    }
    bool operator<( const VENetSmartPtr<T> &right )
    {
        return ptr < right.ptr;
    }
    bool operator>( const VENetSmartPtr<T> &right )
    {
        return ptr > right.ptr;
    }

    bool operator != (const VENetSmartPtr<T>& sp)
    {
        return ptr != sp.ptr;
    }

    VENetSmartPtr<T>& operator = (const VENetSmartPtr<T>& sp)
    {

        if (this != &sp)
        {
            if (reference && reference->Release() == 0)
            {
                VENet::OP_DELETE(ptr, _FILE_AND_LINE_);
                VENet::OP_DELETE(reference, _FILE_AND_LINE_);

            }

            ptr = sp.ptr;
            reference = sp.reference;
            if (reference)
                reference->AddRef();
        }
        return *this;
    }


};

}
