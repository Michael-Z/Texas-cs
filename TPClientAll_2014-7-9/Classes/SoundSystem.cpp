#include "SoundSystem.h"

void SoundSystem::SetSwitch(bool bOpen)
{
	if(m_bSwitch != bOpen)
	{
		m_bSwitch = bOpen;
		UpdateMusicVolume();
	}
}

bool SoundSystem::GetSwitch()
{
	return m_bSwitch;
}

void SoundSystem::SetMusicVolume(VeFloat32 f32Volume)
{
	m_f32MusicVolume = f32Volume;
	UpdateMusicVolume();
}

VeFloat32 SoundSystem::GetMusicVolume()
{
	return m_f32MusicVolume;
}

#ifdef USE_COCOSDENSHION

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

SoundSystem::SoundSystem() : m_bSwitch(true)
{

}

SoundSystem::~SoundSystem()
{
	SimpleAudioEngine::end();
}

void SoundSystem::Pause()
{
	SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void SoundSystem::Resume()
{
	SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void SoundSystem::LoadEffect(VeUInt32 u32SoundID, const VeChar8* pcName)
{
	m_kSoundEffectMap[u32SoundID] = pcName;
	SimpleAudioEngine::sharedEngine()->preloadEffect(pcName);
}

void SoundSystem::PlayEffect(VeUInt32 u32SoundID)
{
	if(m_bSwitch)
	{
		VeFixedStringA* it = m_kSoundEffectMap.Find(u32SoundID);
		if(it)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(*it);
		}
	}
}

void SoundSystem::PlayMusic(const VeChar8* pcFilePath)
{
	if(m_kCurrentMusic == pcFilePath)
	{
		return;
	}
	else
	{
		StopMusic();
	}
	m_kCurrentMusic = pcFilePath;
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(m_kCurrentMusic, true);
}

void SoundSystem::StopMusic()
{
	if(m_kCurrentMusic != "")
	{
		SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
		m_kCurrentMusic = "";
	}
}

void SoundSystem::UpdateMusicVolume()
{
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(m_bSwitch ? m_f32MusicVolume : 0);
}

void SoundSystem::SetSoundVolume(VeFloat32 f32Volume)
{
	SimpleAudioEngine::sharedEngine()->setEffectsVolume(f32Volume);
}

VeFloat32 SoundSystem::GetSoundVolume()
{
	return SimpleAudioEngine::sharedEngine()->getEffectsVolume();
}

#endif

#ifdef USE_FMOD

SoundSystem::SoundSystem() : m_bSwitch(true), m_kThread(10)
	,m_pkSystem(NULL), m_u32Version(0), m_pkMusic(NULL), m_pkMusicChannel(NULL)
{
	VE_ASSERT_EQ(FMOD::System_Create(&m_pkSystem), VE_S_OK);
	VE_ASSERT(m_pkSystem);
	VE_ASSERT_EQ(m_pkSystem->getVersion(&m_u32Version), VE_S_OK);
	VE_ASSERT_EQ(m_pkSystem->init(32, FMOD_INIT_NORMAL, NULL), VE_S_OK);
	m_kThread.SetListener(this);
	m_kThread.Start();
	m_f32MusicVolume = 1.0f;
	m_f32SoundVolume = 1.0f;
}

SoundSystem::~SoundSystem()
{
	m_kThread.Stop();
	m_kMutex.Lock();
	if(m_pkSystem)
	{
		VE_ASSERT_EQ(m_pkSystem->close(), VE_S_OK);
		VE_ASSERT_EQ(m_pkSystem->release(), VE_S_OK);
		m_pkSystem = NULL;
	}
	m_kMutex.Unlock();
}

void SoundSystem::Pause()
{
	if(m_pkMusicChannel)
	{
		m_pkMusicChannel->setPaused(true);
	}
}

void SoundSystem::Resume()
{
	if(m_pkMusicChannel)
	{
		m_pkMusicChannel->setPaused(false);
	}
}

void SoundSystem::LoadEffect(VeUInt32 u32SoundID, const VeChar8* pcName)
{
	VE_ASSERT(m_pkSystem);
	FMOD::Sound* pkSound(NULL);
	m_kMutex.Lock();
	VeDebugOutputString(pcName);
	VE_ASSERT_EQ(m_pkSystem->createSound(pcName, FMOD_HARDWARE, 0, &pkSound), VE_S_OK);
	m_kMutex.Unlock();
	VE_ASSERT(pkSound);
	m_kSoundMap[u32SoundID] = m_kSoundArray.Size();
	m_kSoundArray.PushBack(pkSound);
}

void SoundSystem::PlayEffect(VeUInt32 u32SoundID)
{
	if(m_bSwitch)
	{
		VE_ASSERT(m_pkSystem);
		VeUInt32* it = m_kSoundMap.Find(u32SoundID);
		if(it)
		{
			m_kMutex.Lock();
			FMOD::Sound* pkSound = m_kSoundArray[*it];
			VE_ASSERT(pkSound);
			FMOD::Channel* pkChannel(NULL);
			VE_ASSERT_EQ(m_pkSystem->playSound(FMOD_CHANNEL_FREE, pkSound, false, &pkChannel), VE_S_OK);
			pkChannel->setVolume(m_f32SoundVolume);
			m_kMutex.Unlock();
		}
	}
}

void SoundSystem::Tick()
{
	m_kMutex.Lock();
	if(m_pkSystem)
		m_pkSystem->update();
	m_kMutex.Unlock();
}

void SoundSystem::PlayMusic(const VeChar8* pcFilePath)
{
	if(m_pkMusic)
	{
		if(m_kCurrentMusic == pcFilePath)
		{
			m_pkMusicChannel->setPosition(0, FMOD_TIMEUNIT_MS);
			return;
		}
		StopMusic();
	}
	m_kMutex.Lock();
	VE_ASSERT_EQ(m_pkSystem->createStream(pcFilePath, FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pkMusic), VE_S_OK);
	VE_ASSERT(m_pkMusic);
	m_kCurrentMusic = pcFilePath;
	VE_ASSERT_EQ(m_pkSystem->playSound(FMOD_CHANNEL_FREE, m_pkMusic, false, &m_pkMusicChannel), VE_S_OK);
	m_kMutex.Unlock();
	UpdateMusicVolume();
}

void SoundSystem::StopMusic()
{
	if(m_pkMusic)
	{
		m_kMutex.Lock();
		m_pkMusicChannel->stop();
		m_pkMusicChannel = NULL;
		m_pkMusic->release();
		m_pkMusic = NULL;
		m_kMutex.Unlock();
		m_kCurrentMusic = "";
	}
}

void SoundSystem::UpdateMusicVolume()
{
	if(m_pkMusicChannel)
	{
		m_kMutex.Lock();
		m_pkMusicChannel->setVolume(m_bSwitch ? m_f32MusicVolume : 0);
		m_kMutex.Unlock();
	}
}

void SoundSystem::SetSoundVolume(VeFloat32 f32Volume)
{
	m_f32SoundVolume = f32Volume;
}

VeFloat32 SoundSystem::GetSoundVolume()
{
	return m_f32SoundVolume;
}

#endif
