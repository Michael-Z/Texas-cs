#include "cocos2d.h"
#include "CCEGLView.h"
#include "AppDelegate.h"
#include "TexasPoker.h"
#include "Venus3D.h"
#include "SoundSystem.h"
#include "GameSaveData.h"
#include "Hall.h"
#include "Login.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate()
{
	SoundSystem::Destory();
	GameSaveData ::Destory();
	VenusTerm();
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
	pDirector->setProjection(kCCDirectorProjection2D);

    // turn on display FPS
    //pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    //pDirector->setAnimationInterval(1.0 / 60);

	VenusInit();
	VE_NEW GameSaveData;
	VE_NEW SoundSystem;
	// create a scene. it's an autorelease object    
	TexasPoker* pkGame = TexasPoker::create();

    // run
    pDirector->runWithScene(pkGame);
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
    if(g_pSoundSystem)
    	g_pSoundSystem->Pause();
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(VeStrcmp(pkMainLayer->GetName(), "login"))
	{
		g_pkGame->DisconnectFromHallServer();
	}
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();

    if(g_pSoundSystem)
    	g_pSoundSystem->Resume();

#	ifdef VE_PLATFORM_ANDROID
    if(g_pkGame)
    	g_pkGame->Restore();
#	endif

#	ifdef VE_PLATFORM_MOBILE_SIM
	static bool s_bFirst = true;
	if(s_bFirst)
	{
		s_bFirst = false;
		return;
	}
#	endif
	UILayer* pkMainLayer = g_pLayerManager->GetCurrentMainLayer();
	if(VeStrcmp(pkMainLayer->GetName(), "login"))
	{
		g_pkGame->ConnectToHallServer();
	}
	
}
