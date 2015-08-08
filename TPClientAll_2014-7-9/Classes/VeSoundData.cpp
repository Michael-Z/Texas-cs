#include "VeSoundData.h"

VeSoundData::VeSoundData()
	: m_u32Channel(0), m_u32SampleRate(0), m_u32BitsPerSample(0)
{

}

VeSoundData::~VeSoundData()
{
	m_u32Channel = 0;
	m_u32SampleRate = 0;
	m_u32BitsPerSample = 0;
	m_spData = NULL;
}

typedef struct
{
	VeChar8 m_ac8Riff[4];
	VeUInt32 m_u32RiffSize;
	VeChar8 m_ac8Wave[4];
} VeWaveFileHeader;

typedef struct
{
	VeChar8 m_ac8ChunkName[4];
	VeUInt32 m_u32ChunkSize;
} VeRiffChunk;

typedef struct
{
	VeUInt16 m_u16FormatTag;
	VeUInt16 m_u16Channels;
	VeUInt32 m_u32SamplesPerSec;
	VeUInt32 m_u32AvgBytesPerSec;
	VeUInt16 m_u16BlockAlign;
	VeUInt16 m_u16BitsPerSample;
	VeUInt16 m_u16Extra; 
} VeWaveFormat;


void VeSoundData::ParseWave(VeBinaryIStream& kInput)
{
	VeWaveFileHeader kHeader;
	VE_ASSERT_EQ(kInput.Read(&kHeader, 12), 12);
	VE_ASSERT((!VeStrnicmp(kHeader.m_ac8Riff, "RIFF", 4)) && (!VeStrnicmp(kHeader.m_ac8Wave, "WAVE", 4)));
	VeRiffChunk kChunk;
	while(kInput.Read(&kChunk, 8) == 8)
	{
		if(!VeStrnicmp(kChunk.m_ac8ChunkName, "fmt ", 4))
		{
			VeWaveFormat kFormat;
			VE_ASSERT_EQ(kInput.Read(&kFormat, kChunk.m_u32ChunkSize), VeInt32(kChunk.m_u32ChunkSize));
			VE_ASSERT(kFormat.m_u16FormatTag == 1);
			m_u32Channel = kFormat.m_u16Channels;
			m_u32SampleRate = kFormat.m_u32SamplesPerSec;
			m_u32BitsPerSample = kFormat.m_u16BitsPerSample;
		}
		else if(!VeStrnicmp(kChunk.m_ac8ChunkName, "data", 4))
		{
			m_spData = VE_NEW VeBlob(kChunk.m_u32ChunkSize);
			VE_ASSERT_EQ(kInput.Read(*m_spData, kChunk.m_u32ChunkSize), VeInt32(kChunk.m_u32ChunkSize));
		}
		else
		{
			kInput.Skip(kChunk.m_u32ChunkSize);
		}
		if (kChunk.m_u32ChunkSize & 1)
		{
			kInput.Skip(1);
		}
	}
}

const VeBlobPtr& VeSoundData::GetData()
{
	return m_spData;
}
