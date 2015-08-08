#include "General.h"

//--------------------------------------------------------------------------
BitStream& operator<<(BitStream& kOut, const VeChar8* pcStr)
{
	VeUInt16 u16Len = (VeUInt16)VeStrlen(pcStr);
	kOut.Write(u16Len);
	kOut.WriteAlignedBytes((const VeUInt8*)pcStr, u16Len);
	return kOut;
}
//--------------------------------------------------------------------------
BitStream& operator << (BitStream& kOut, const VeStringA& kStr)
{
	kOut << ((const VeChar8*)kStr);
	return kOut;
}
//--------------------------------------------------------------------------
BitStream& operator>>(BitStream& kIn, VeStringA& kStr)
{
	VEString kString;
	kIn >> kString;
	kStr = kString;
	return kIn;
}
//--------------------------------------------------------------------------
static bool s_bStreamError = false;
//--------------------------------------------------------------------------
void StreamErrorClear()
{
	s_bStreamError = false;
}
//--------------------------------------------------------------------------
void StreamError()
{
	s_bStreamError = true;
}
//--------------------------------------------------------------------------
bool IsStreamError()
{
	return s_bStreamError;
}
//--------------------------------------------------------------------------
