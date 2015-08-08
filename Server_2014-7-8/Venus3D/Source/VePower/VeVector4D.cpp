////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeVector3D.cpp
//  Version:     v1.00
//  Created:     13/9/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VePowerPCH.h"
#ifdef VE_NO_INLINE
#	include "VeVector4D.inl"
#endif

//--------------------------------------------------------------------------
const VeVector4D VeVector4D::ZERO = VeVector4D(0, 0, 0, 0);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::UNIT_X = VeVector4D(1, 0, 0, 0);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::UNIT_Y = VeVector4D(0, 1, 0, 0);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::UNIT_Z = VeVector4D(0, 0, 1, 0);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::ZERO_POINT = VeVector4D(0, 0, 0, 1);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::UNIT_X_POINT = VeVector4D(1, 0, 0, 1);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::UNIT_Y_POINT = VeVector4D(0, 1, 0, 1);
//--------------------------------------------------------------------------
const VeVector4D VeVector4D::UNIT_Z_POINT = VeVector4D(0, 0, 1, 1);
//--------------------------------------------------------------------------
