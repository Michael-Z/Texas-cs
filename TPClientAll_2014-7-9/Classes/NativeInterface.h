#pragma once

#include "Venus3D.h"

struct Version
{
	VeUInt8 m_au8Version[4];
};

Version NativeGetVersion();

void NativeOnEvent(const VeChar8* pcEvent);

void NativeOnEvent(const VeChar8* pcEvent, const VeVector< VePair<VeStringA,VeStringA> >& kValue);

void NativeOnEventBegin(const VeChar8* pcEvent);

void NativeOnEventEnd(const VeChar8* pcEvent);

void NativeOnPageStart(const VeChar8* pcPage);

void NativeOnPageEnd(const VeChar8* pcPage);
