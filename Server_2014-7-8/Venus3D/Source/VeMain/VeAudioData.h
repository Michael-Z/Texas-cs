////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeAudioData.h
//  Version:     v1.00
//  Created:     28/7/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_MAIN_API VeAudioSampleData : public VeRefObject
{
public:
	VeAudioSampleData();

	~VeAudioSampleData();

	void ParseWave(VeBinaryIStream& kInput);

	VE_INLINE VeUInt32 GetChannelNum();

	VE_INLINE VeUInt32 GetSampleRate();

	VE_INLINE VeUInt32 GetBitsPerSample();

	VE_INLINE const VeBlobPtr& GetData();

protected:
	VeUInt32 m_u32ChannelNum;
	VeUInt32 m_u32SampleRate;
	VeUInt32 m_u32BitsPerSample;
	VeBlobPtr m_spData;

};

VeSmartPointer(VeAudioSampleData);

#ifndef VE_NO_INLINE
#	include "VeAudioData.inl"
#endif
