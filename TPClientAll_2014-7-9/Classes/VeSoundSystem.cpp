#include "VeSoundSystem.h"

#ifdef VE_USE_SLES

VeSoundSystem::VeSoundSystem()
{
	VE_ASSERT_EQ(slCreateEngine(&m_pkEngineObj, 0, NULL, 0, NULL, NULL), SL_RESULT_SUCCESS);
	VE_ASSERT_EQ((*m_pkEngineObj)->Realize(m_pkEngineObj, SL_BOOLEAN_FALSE), SL_RESULT_SUCCESS);
	VE_ASSERT_EQ((*m_pkEngineObj)->GetInterface(m_pkEngineObj, SL_IID_ENGINE, &m_pkEngine), SL_RESULT_SUCCESS);
	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	VE_ASSERT_EQ((*m_pkEngine)->CreateOutputMix(m_pkEngine, &m_pkOutputMixObject, 1, ids, req), SL_RESULT_SUCCESS);
	SLresult eRes;
	eRes = (*m_pkOutputMixObject)->GetInterface(m_pkOutputMixObject, SL_IID_ENVIRONMENTALREVERB, &m_pkOutputMixEnvironmentalReverb);
	if(eRes == SL_RESULT_SUCCESS)
	{
		const SLEnvironmentalReverbSettings eReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
		(*m_pkOutputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(m_pkOutputMixEnvironmentalReverb, &eReverbSettings);
	}
	VeDebugOutputString("Create");
}

VeSoundSystem::~VeSoundSystem()
{
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
}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{

}

#else

VeSoundSystem::VeSoundSystem()
{
	
}

VeSoundSystem::~VeSoundSystem()
{

}

#endif

#if defined(VE_PLATFORM_ANDROID)
#	define DATA_DIR "asset#/Resources"
#else
#	define DATA_DIR "file#."
#endif

#define WAVE_NAME "Footsteps.wav"

USING_NS_CC;

void VeSoundSystem::TestLoad()
{
	std::string pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(WAVE_NAME);
	unsigned char* pbyBuffer = NULL;
	unsigned long ulSize = 0;
	pbyBuffer = CCFileUtils::sharedFileUtils()->getFileData(pathKey.c_str(), "rb", &ulSize);

	VE_ASSERT(ulSize);
	VeBlobPtr spBlob = VE_NEW VeBlob(pbyBuffer, ulSize);
	/*VeDirectory* pkDir = g_pResourceManager->CreateDir(DATA_DIR);
	VE_ASSERT(pkDir);
	VE_ASSERT(pkDir->HasFile(WAVE_NAME));*/
	//VeBinaryIStreamPtr spInput = pkDir->OpenSync(WAVE_NAME);
	VeMemoryIStream kStream(spBlob);
	m_kData.ParseWave(kStream);
	//g_pResourceManager->DestoryDir(pkDir);
	delete [] pbyBuffer;
	pbyBuffer = NULL;


	//SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
	//SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_44_1,
	//	SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
	//	SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
	//SLDataSource audioSrc = {&loc_bufq, &format_pcm};

	//SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, m_pkOutputMixObject};
	//SLDataSink audioSnk = {&loc_outmix, NULL};

	//SLresult result;
	//    // create audio player
	//const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
	//	/*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
	//const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
	//	/*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};

	//VeDebugOutputString("Create1");
	//result = (*m_pkEngine)->CreateAudioPlayer(m_pkEngine, &bqPlayerObject, &audioSrc, &audioSnk,
	//            3, ids, req);
	//assert(SL_RESULT_SUCCESS == result);
	//VeDebugOutputString("Create2");
	    // realize the player
	/*result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	    // get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
	assert(SL_RESULT_SUCCESS == result);

	    // get the buffer queue interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
		&bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result);

	    // register callback on the buffer queue
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
	assert(SL_RESULT_SUCCESS == result);

	    // get the effect send interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
		&bqPlayerEffectSend);
	assert(SL_RESULT_SUCCESS == result);

	    // get the volume interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
	assert(SL_RESULT_SUCCESS == result);

	    // set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);*/

	//(*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, *m_kData.GetData(), m_kData.GetData()->GetSize());

}

void VeSoundSystem::TestPlay()
{

}
