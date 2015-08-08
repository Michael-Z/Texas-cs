#include "Venus3D.h"
#include "VeSoundSystem.h"
#include "Client.h"

#ifdef WIN32
#	pragma comment(lib,"VePower.lib")
#endif

void VenusInit()
{
	VeGlobalStringTableA::Create();
	VE_NEW VeAllocatorManager();
	VE_NEW VeStringParser();
	VeLogger::SetLog(VeLogger::OUTPUT_DEBUGOUTPUT);
	VE_NEW VeResourceManager();
	VE_NEW VeURL();
	//VE_NEW VeSoundSystem();
	VE_NEW VeTime();
	VE_NEW Client();
}

void VenusTerm()
{
	Client::Destory();
	VeTime::Destory();
	//VeSoundSystem::Destory();
	VeURL::Destory();
	VeResourceManager::Destory();
	VeLogger::SetLog(VeLogger::OUTPUT_MAX);
	VeStringParser::Destory();
	VeAllocatorManager::Destory();
	VeGlobalStringTableA::Destory();
#ifdef VE_MEM_DEBUG
	_VeMemoryExit();
#endif
}

void VenusUpdate()
{
	g_pTime->Update();
	g_pURL->Update();
	g_pResourceManager->Update();
}
