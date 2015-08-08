#pragma once

#include "Venus3D.h"

class VeSoundData : public VeRefObject
{
public:
	VeSoundData();

	~VeSoundData();

	void ParseWave(VeBinaryIStream& kInput);

	const VeBlobPtr& GetData();

protected:
	VeUInt32 m_u32Channel;
	VeUInt32 m_u32SampleRate;
	VeUInt32 m_u32BitsPerSample;
	VeBlobPtr m_spData;

};

VeSmartPointer(VeSoundData);
