#pragma once

#include "cocos2d.h"
#include "Venus3D.h"

#ifdef VE_PLATFORM_ANDROID
#	define USE_COCOSDENSHION
#else
#	define USE_FMOD
#endif

#ifdef USE_FMOD
#   include <fmod.hpp>
#endif

class SoundSystem : public VeSingleton<SoundSystem>
#ifdef USE_FMOD
    , public VeTickThread::TickListener
#endif
{
public:
	SoundSystem();

	virtual ~SoundSystem();

	void Pause();

	void Resume();

	void LoadEffect(VeUInt32 u32SoundID, const VeChar8* pcName);

	void PlayEffect(VeUInt32 u32SoundID);

	void SetSwitch(bool bOpen);

	bool GetSwitch();

	void PlayMusic(const VeChar8* pcFilePath);

	void StopMusic();

	void SetMusicVolume(VeFloat32 f32Volume);

	VeFloat32 GetMusicVolume();

	void UpdateMusicVolume();

	void SetSoundVolume(VeFloat32 f32Volume);

	VeFloat32 GetSoundVolume();

protected:
	bool m_bSwitch;
	VeFloat32 m_f32MusicVolume;
#ifdef USE_COCOSDENSHION
	VeHashMap<VeUInt32, VeFixedStringA> m_kSoundEffectMap;
	VeString m_kCurrentMusic;
#endif

#ifdef USE_FMOD
    virtual void Tick();
    
    VeTickThread m_kThread;
	FMOD::System* m_pkSystem;
	VeUInt32 m_u32Version;
    VeThread::Mutex m_kMutex;
    VeHashMap<VeUInt32,VeUInt32> m_kSoundMap;
	VeVector<FMOD::Sound*> m_kSoundArray;
	VeFloat32 m_f32SoundVolume;

	VeString m_kCurrentMusic;
	FMOD::Sound* m_pkMusic;
	FMOD::Channel* m_pkMusicChannel;

#endif

	

};

#define g_pSoundSystem SoundSystem::GetSingletonPtr()
