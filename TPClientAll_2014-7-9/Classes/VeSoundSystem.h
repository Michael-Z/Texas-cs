#pragma once

#include "Venus3D.h"
#include "VeSoundData.h"
#include "cocos2d.h"

#ifdef VE_PLATFORM_ANDROID
#	define VE_USE_SLES
#else
#	define USE_OAL
#endif

#ifdef VE_USE_SLES
#	include <SLES/OpenSLES.h>
#	include <SLES/OpenSLES_Android.h>
#endif

class VeSoundSystem : public VeSingleton<VeSoundSystem>
{
public:
	VeSoundSystem();

	virtual ~VeSoundSystem();

	void TestLoad();

	void TestPlay();

protected:
#	ifdef VE_USE_SLES
	SLObjectItf m_pkEngineObj;
	SLEngineItf m_pkEngine;
	SLObjectItf m_pkOutputMixObject;
	SLEnvironmentalReverbItf m_pkOutputMixEnvironmentalReverb;
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
	SLEffectSendItf bqPlayerEffectSend;
	SLMuteSoloItf bqPlayerMuteSolo;
	SLVolumeItf bqPlayerVolume;
#	endif
	VeSoundData m_kData;

};

#define g_pSoundSystemTest VeSoundSystem::GetSingletonPtr()
