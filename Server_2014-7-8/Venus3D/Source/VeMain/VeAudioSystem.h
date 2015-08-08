////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeAudioSystem.h
//  Version:     v1.00
//  Created:     28/7/2013 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef VE_PLATFORM_ANDROID
#	define VE_USE_SLES
#else
#	define USE_OAL
#endif

#ifdef VE_USE_SLES
#	include <SLES/OpenSLES.h>
#	include <SLES/OpenSLES_Android.h>
#endif

#define VE_MAX_CHANNEL_NUM (32)

class VE_MAIN_API VeAudioChannel : public VeMemObject
{
public:
	enum State
	{
		STOPPED,
		PLAYING,
		PAUSED
	};

	VeAudioChannel();

	~VeAudioChannel();

	VE_INLINE VeUInt32 GetIndex();

	VE_INLINE State GetState();

	VE_INLINE void SetVolume(VeFloat32 f32Volume);

	VE_INLINE VeFloat32 GetVolume();

	VE_INLINE void Stop();

	VE_INLINE void Pause();

	VE_INLINE void Resume();

protected:
	friend class VeAudioSystem;
	friend class VeSimpleSound;
	VeUInt32 m_u32Index;
	State m_eState;
	VeFloat32 m_f32Volume;
	VeUInt32 m_u32LoopNum;
	VeAudio* m_pkTouchAudio;
	VeRefNode<VeAudioChannel*> m_kNode;

#	ifdef VE_USE_SLES
	SLObjectItf m_pkPlayerObj;
	SLPlayItf m_pkPlayerPlay;
	SLAndroidSimpleBufferQueueItf m_pkPlayerBufferQueue;
	SLEffectSendItf m_pkPlayerEffectSend;
	SLVolumeItf m_pkPlayerVolume;
	void DestorySLPlayer();
	static void SimpleSoundCallback(SLAndroidSimpleBufferQueueItf pkQueue, void* pvContext);
#	endif

};

class VE_MAIN_API VeAudioSystem : public VeSingleton<VeAudioSystem>
{
public:
	VeAudioSystem();

	virtual ~VeAudioSystem();

	void Update();

	VeAudioChannel* GetChannel(VeUInt32 u32Index);

	VeAudioChannel* GetFreeChannel();

	VeAudioChannel* Play(VeAudio* pkAudio, VeUInt32 u32LoopNum = 1);

protected:
	friend class VeSimpleSound;
	friend class VeAudioChannel;

	void InitGlobal();

	void TermGlobal();

	VeThread::Mutex m_kMutex;
	VeAudioChannel m_akChannels[VE_MAX_CHANNEL_NUM];
	VeRefList<VeAudioChannel*> m_kFreeChannelList;
	VeRefList<VeAudio*> m_kPlayingAudioList;
	VeVector<VeAudioChannel*> m_kChannelNeedStop;

#	ifdef VE_USE_SLES
	SLObjectItf m_pkEngineObj;
	SLEngineItf m_pkEngine;
	SLObjectItf m_pkOutputMixObject;
	SLEnvironmentalReverbItf m_pkOutputMixEnvironmentalReverb;
	static const SLEnvironmentalReverbSettings ms_kReverbSettings;
#	endif

};

#define g_pAudioSystem VeAudioSystem::GetSingletonPtr()

#ifndef VE_NO_INLINE
#	include "VeAudioSystem.inl"
#endif
