////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeAudioSystem.cpp
//  Version:     v1.00
//  Created:     28/7/2013 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#ifdef VE_NO_INLINE
#	include "VeAudioSystem.inl"
#endif

//--------------------------------------------------------------------------
VeAudioChannel::VeAudioChannel() : m_u32Index(0), m_eState(STOPPED)
	, m_f32Volume(1.0f), m_u32LoopNum(0), m_pkTouchAudio(NULL)
{
	m_kNode.m_tContent = this;
#	ifdef VE_USE_SLES
	m_pkPlayerObj = NULL;;
	m_pkPlayerPlay = NULL;
	m_pkPlayerBufferQueue = NULL;
	m_pkPlayerEffectSend = NULL;;
	m_pkPlayerVolume = NULL;
#	endif
}
//--------------------------------------------------------------------------
VeAudioChannel::~VeAudioChannel()
{
	m_u32Index = 0;
	m_eState = STOPPED;
	m_f32Volume = 1.0f;
	m_pkTouchAudio = NULL;
}
//--------------------------------------------------------------------------
#ifdef VE_USE_SLES
//--------------------------------------------------------------------------
void VeAudioChannel::DestorySLPlayer()
{
	if(m_pkPlayerObj)
	{
		(*m_pkPlayerObj)->Destroy(m_pkPlayerObj);
		m_pkPlayerObj = NULL;
		m_pkPlayerPlay = NULL;
		m_pkPlayerBufferQueue = NULL;
		m_pkPlayerEffectSend = NULL;
		m_pkPlayerVolume = NULL;
	}
}
//--------------------------------------------------------------------------
void VeAudioChannel::SimpleSoundCallback(SLAndroidSimpleBufferQueueItf pkQueue,
	void* pvContext)
{
	VE_AUTO_LOCK_MUTEX(g_pAudioSystem->m_kMutex);
	VeAudioChannel* pkChannel = (VeAudioChannel*)pvContext;
	bool bContinue = true;
	if(pkChannel->m_u32LoopNum)
		bContinue = (--(pkChannel->m_u32LoopNum)) > 0;
	if(bContinue)
	{
		VeSimpleSound* pkAudio = static_cast<VeSimpleSound*>(pkChannel->m_pkTouchAudio);
		void* pvData = *(pkAudio->m_spData->GetData());
		VeUInt32 u32Size = pkAudio->m_spData->GetData()->GetSize();

		VE_ASSERT_EQ((*pkChannel->m_pkPlayerBufferQueue)->Enqueue(pkChannel->m_pkPlayerBufferQueue, pvData, u32Size), SL_RESULT_SUCCESS);
	}
	else
	{
		g_pAudioSystem->m_kChannelNeedStop.PushBack(pkChannel);
	}
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
#ifdef VE_USE_SLES
const SLEnvironmentalReverbSettings VeAudioSystem::ms_kReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
#endif
//--------------------------------------------------------------------------
VeAudioSystem::VeAudioSystem()
{
#	ifdef VE_USE_SLES
	VE_ASSERT_EQ(slCreateEngine(&m_pkEngineObj, 0, NULL, 0, NULL, NULL), SL_RESULT_SUCCESS);
	VE_ASSERT_EQ((*m_pkEngineObj)->Realize(m_pkEngineObj, SL_BOOLEAN_FALSE), SL_RESULT_SUCCESS);
	VE_ASSERT_EQ((*m_pkEngineObj)->GetInterface(m_pkEngineObj, SL_IID_ENGINE, &m_pkEngine), SL_RESULT_SUCCESS);
	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	VE_ASSERT_EQ((*m_pkEngine)->CreateOutputMix(m_pkEngine, &m_pkOutputMixObject, 1, ids, req), SL_RESULT_SUCCESS);
	VE_ASSERT_EQ((*m_pkOutputMixObject)->Realize(m_pkOutputMixObject, SL_BOOLEAN_FALSE), SL_RESULT_SUCCESS);
	if((*m_pkOutputMixObject)->GetInterface(m_pkOutputMixObject, SL_IID_ENVIRONMENTALREVERB, &m_pkOutputMixEnvironmentalReverb) == SL_RESULT_SUCCESS)
	{
		VE_ASSERT_EQ((*m_pkOutputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(m_pkOutputMixEnvironmentalReverb, &ms_kReverbSettings), SL_RESULT_SUCCESS);
	}
#	endif
	InitGlobal();
}
//--------------------------------------------------------------------------
VeAudioSystem::~VeAudioSystem()
{
	TermGlobal();
#	ifdef VE_USE_SLES
	if(m_pkOutputMixObject)
	{
		(*m_pkOutputMixObject)->Destroy(m_pkOutputMixObject);
		m_pkOutputMixObject = NULL;
		m_pkOutputMixEnvironmentalReverb = NULL;
	}

	if(m_pkEngineObj != NULL)
	{
		(*m_pkEngineObj)->Destroy(m_pkEngineObj);
		m_pkEngineObj = NULL;
		m_pkEngine = NULL;
	}
#	endif
}
//--------------------------------------------------------------------------
void VeAudioSystem::InitGlobal()
{
	g_pResourceManager->SetResourceCreateFunction(
		VeResource::SIMPLE_SOUND, VeSimpleSound::CreateResource);
	for(VeUInt32 i(0); i < VE_MAX_CHANNEL_NUM; ++i)
	{
		VeAudioChannel& kChannel = m_akChannels[i];
		kChannel.m_u32Index = i;
		m_kFreeChannelList.AttachBack(kChannel.m_kNode);
	}
}
//--------------------------------------------------------------------------
void VeAudioSystem::TermGlobal()
{
	for(VeUInt32 i(0); i < VE_MAX_CHANNEL_NUM; ++i)
	{
		m_akChannels[i].Stop();
	}
	g_pResourceManager->SetResourceCreateFunction(
		VeResource::SIMPLE_SOUND, NULL);
}
//--------------------------------------------------------------------------
void VeAudioSystem::Update()
{
	VE_AUTO_LOCK_MUTEX(g_pAudioSystem->m_kMutex);
	for(VeUInt32 i(0); i < m_kChannelNeedStop.Size(); ++i)
	{
		m_kChannelNeedStop[i]->Stop();
	}
	m_kChannelNeedStop.Clear();
	m_kPlayingAudioList.BeginIterator();
	while(!m_kPlayingAudioList.IsEnd())
	{
		VeAudio* pkAudio = m_kPlayingAudioList.GetIterNode()->m_tContent;
		m_kPlayingAudioList.Next();
		pkAudio->Update();
	}
}
//--------------------------------------------------------------------------
VeAudioChannel* VeAudioSystem::GetChannel(VeUInt32 u32Index)
{
	VE_ASSERT(u32Index < VE_MAX_CHANNEL_NUM);
	return m_akChannels + u32Index;
}
//--------------------------------------------------------------------------
VeAudioChannel* VeAudioSystem::GetFreeChannel()
{
	return m_kFreeChannelList.Size() ?
		m_kFreeChannelList.GetHeadNode()->m_tContent : NULL;
}
//--------------------------------------------------------------------------
VeAudioChannel* VeAudioSystem::Play(VeAudio* pkAudio, VeUInt32 u32LoopNum)
{
	VeAudioChannel* pkTarget = GetFreeChannel();
	if(pkTarget)
	{
		pkAudio->Play(pkTarget, u32LoopNum);
		m_kPlayingAudioList.AttachBack(pkAudio->m_kUpdateNode);
	}
	return pkTarget;
}
//--------------------------------------------------------------------------
