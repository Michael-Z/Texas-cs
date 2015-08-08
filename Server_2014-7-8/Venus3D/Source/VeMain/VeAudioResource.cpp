////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeAudioResource.cpp
//  Version:     v1.00
//  Created:     29/7/2013 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeAudioResource.inl"
#endif

//--------------------------------------------------------------------------
VeAudio::VeAudio() : m_f32Volume(1.0f)
{
	m_kUpdateNode.m_tContent = this;
}
//--------------------------------------------------------------------------
VeSimpleSound::VeSimpleSound(const VeChar8* pcName, const VeChar8* pcGroup)
	: VeResource(SIMPLE_SOUND, pcName, pcGroup)
{

}
//--------------------------------------------------------------------------
VeSimpleSound::~VeSimpleSound()
{
	Unload();
}
//--------------------------------------------------------------------------
VeResource::State VeSimpleSound::LoadImpl(const VeMemoryIStreamPtr& spStream)
{
	const VeChar8* pcExt = VeStrrchr(GetName(), '.');
	if(VeStricmp(pcExt, ".wav") == 0)
	{
		m_spData = VE_NEW VeAudioSampleData;
		m_spData->ParseWave(*spStream);
	}
	else
	{
		m_spData = NULL;
	}
	return m_spData ? LOADED : UNLOADED;
}
//--------------------------------------------------------------------------
void VeSimpleSound::UnloadImpl()
{
	m_spData = NULL;
}
//--------------------------------------------------------------------------
VeResource* VeSimpleSound::CreateResource(
	const VeChar8* pcName, const VeChar8* pcGroup)
{
	return VE_NEW VeSimpleSound(pcName, pcGroup);
}
//--------------------------------------------------------------------------
void VeSimpleSound::Play(VeAudioChannel* pkChannel, VeUInt32 u32LoopNum)
{
	if(GetState() == LOADED)
	{
		if(pkChannel->GetState() != VeAudioChannel::STOPPED)
		{
			return;
		}
		VE_ASSERT(!(pkChannel->m_pkTouchAudio));
		VE_ASSERT(pkChannel->m_kNode.IsAttach(g_pAudioSystem->m_kFreeChannelList));
		pkChannel->m_pkTouchAudio = this;
		pkChannel->m_eState = VeAudioChannel::PLAYING;
		pkChannel->m_f32Volume = m_f32Volume;
		pkChannel->m_u32LoopNum = u32LoopNum;
		pkChannel->m_pkTouchAudio = this;
		m_kPlayingChannelList.AttachBack(pkChannel->m_kNode);
		VE_ASSERT(m_spData);
		VeUInt32 u32ChannelNum = m_spData->GetChannelNum();
		VeUInt32 u32SampleRate = m_spData->GetSampleRate();
		VeUInt32 u32BitsPerSample = m_spData->GetBitsPerSample();
		void* pvData = *(m_spData->GetData());
		VeUInt32 u32Size = m_spData->GetData()->GetSize();
#		ifdef VE_USE_SLES
		VeUInt32 u32Mask = (u32ChannelNum > 1)
			? (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT)
			: SL_SPEAKER_FRONT_CENTER;

		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, u32ChannelNum, u32SampleRate * 1000,
			u32BitsPerSample, u32BitsPerSample,
			u32Mask, SL_BYTEORDER_LITTLEENDIAN};
		SLDataSource audioSrc = {&loc_bufq, &format_pcm};

		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, g_pAudioSystem->m_pkOutputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};

		const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
		const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
		VE_ASSERT_EQ((*g_pAudioSystem->m_pkEngine)->CreateAudioPlayer(g_pAudioSystem->m_pkEngine,
			&pkChannel->m_pkPlayerObj, &audioSrc, &audioSnk, 3, ids, req), SL_RESULT_SUCCESS);

		VE_ASSERT_EQ((*pkChannel->m_pkPlayerObj)->Realize(pkChannel->m_pkPlayerObj, SL_BOOLEAN_FALSE), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerObj)->GetInterface(pkChannel->m_pkPlayerObj, SL_IID_PLAY, &pkChannel->m_pkPlayerPlay), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerObj)->GetInterface(pkChannel->m_pkPlayerObj, SL_IID_BUFFERQUEUE, &pkChannel->m_pkPlayerBufferQueue), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerBufferQueue)->RegisterCallback(pkChannel->m_pkPlayerBufferQueue, VeAudioChannel::SimpleSoundCallback, pkChannel), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerObj)->GetInterface(pkChannel->m_pkPlayerObj, SL_IID_EFFECTSEND, &pkChannel->m_pkPlayerEffectSend), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerObj)->GetInterface(pkChannel->m_pkPlayerObj, SL_IID_VOLUME, &pkChannel->m_pkPlayerVolume), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerPlay)->SetPlayState(pkChannel->m_pkPlayerPlay, SL_PLAYSTATE_PLAYING), SL_RESULT_SUCCESS);
		VE_ASSERT_EQ((*pkChannel->m_pkPlayerBufferQueue)->Enqueue(pkChannel->m_pkPlayerBufferQueue, pvData, u32Size), SL_RESULT_SUCCESS);
#		endif
	}
}
//--------------------------------------------------------------------------
void VeSimpleSound::Stop(VeAudioChannel* pkChannel)
{
	if(pkChannel->m_kNode.IsAttach(m_kPlayingChannelList))
	{
		VE_ASSERT(static_cast<VeAudio*>(this) == pkChannel->m_pkTouchAudio);
		VE_ASSERT(pkChannel->m_eState != VeAudioChannel::STOPPED);
		pkChannel->m_eState = VeAudioChannel::STOPPED;
		pkChannel->m_f32Volume = 1.0f;
		pkChannel->m_u32LoopNum = 0;
#		ifdef VE_USE_SLES
		pkChannel->DestorySLPlayer();
#		endif
		g_pAudioSystem->m_kFreeChannelList.AttachBack(pkChannel->m_kNode);
		if(m_kPlayingChannelList.Empty())
		{
			m_kUpdateNode.Detach();
		}
	}
}
//--------------------------------------------------------------------------
void VeSimpleSound::Stop()
{

}
//--------------------------------------------------------------------------
void VeSimpleSound::Pause(VeAudioChannel* pkChannel)
{

}
//--------------------------------------------------------------------------
void VeSimpleSound::Pause()
{

}
//--------------------------------------------------------------------------
void VeSimpleSound::Resume(VeAudioChannel* pkChannel)
{

}
//--------------------------------------------------------------------------
void VeSimpleSound::Resume()
{

}
//--------------------------------------------------------------------------
void VeSimpleSound::Update()
{

}
//--------------------------------------------------------------------------
