#pragma once

#include <cocos2d.h>
#include "Venus3D.h"
#include "GameSaveData.h"
#include "NativeInterface.h"

#define MAIN_WIDTH_HD (640)
#define MAIN_HEIGHT_HD (960)
#define MAIN_WIDTH (320)
#define MAIN_HEIGHT (480)
#define MAX_ASPECT_RATIO (768.0f / 1024.0f)
#define MIN_ASPECT_RATIO (640.0f / 1136.0f)

#ifdef WIN32
#	define TTF_NAME "Î¢ÈíÑÅºÚ"
#else
#	define TTF_NAME "Arial"
#endif

#define RES_ROOT "Res"
#define IMAGE_PATH "image/png"
#define IMAGE_PATH_HD "image/hd_png"

#define EN_US "en_US"
#define ZH_CN "zh_CN"
#define ZH_TW "zh_TW"
#ifdef VERSION_EN
#	define MAIN_LAN EN_US
#	define MAIN_LAN_ENUM en_US
#else
#	define MAIN_LAN ZH_CN
#	define MAIN_LAN_ENUM zh_CN
#endif

#define LOGIN_MUSIC "Res/music/start.ogg"
#define HALL_MUSIC "Res/music/hall.ogg"
#define GAME_MUSIC "Res/music/game.ogg"
#define SLOT_MUSIC "Res/music/slots.ogg"
#define SLOT_LOOP_EFF "Res/audio/slots_reelLoop.ogg"
#define BACC_MUSIC "Res/music/Baccarat.ogg"

#define FADE_IN_TIME (0.5f)
#define FADE_OUT_TIME (0.2f)

#include "UILayerManager.h"

enum SoundEffect
{
	SE_NULL,
	SE_HELPOK,
	SE_BANK_UP,
	SE_GOLD,
	SE_BTN_CLICK,
	SE_SLOT_CARD,
	SE_DEAL_CARD,
	SE_GET_CHIPS,
	SE_SLOT_ARMPULL,
	SE_SLOT_ARMRELEASE,
	SE_SLOT_LOOP,
	SE_SLOT_REELSTOP,
	SE_SLOT_WINTYPE0,
	SE_SLOT_WINTYPE1,
	SE_SLOT_WINTYPE2,
	SE_POKER_ON_CLOCK,
	SE_POKER_ON_MYTURN,
	SE_POKER_WIN_0,
	SE_POKER_WIN_1,
	SE_POKER_WIN_2,
	SE_WIN_CATE_0,
	SE_WIN_CATE_1,
	SE_WIN_CATE_2,
	SE_WIN_CATE_3,
	SE_WIN_CATE_4,
	SE_WIN_CATE_5,
	SE_WIN_CATE_6,
	SE_WIN_CATE_7,
	SE_WIN_CATE_8,
	SE_WIN_CATE_9,
	SE_POKER_ON_ADD,
	SE_POKER_ON_ALLIN,
	SE_POKER_ON_CALL,
	SE_POKER_ON_CHECK,
	SE_POKER_ON_DISS,
	SE_POKER_ON_FLOP,
	SE_POKER_ON_RIVER,
	SE_POKER_ON_START,
	SE_POKER_ON_TURN,
	SE_POKER_ON_WELCOME,
	SE_BET,
	SE_CHIPS_WIN,
	SE_BANKERWIN,
	SE_PLAYERWIN,
	SE_DRAWGAME,
	SE_GLOBAL,
	SE_RIGHT,
	SE_MAX
};

enum Shader
{
	SHADER_PERSPECTIVE_SPRITE,
	SHADER_MAX
};

enum PurchaseType
{
	PURCHASE_TYPE_DOLLAR,
	PURCHASE_TYPE_RMB,
	PURCHASE_TYPE_MAX
};

#define SHOW_LOADING (0.3f)

VeSmartPointer(ToastCallback);

class TexasPoker : public cocos2d::CCScene
{
public:
	enum Language
	{
		zh_CN,
		zh_TW,
		en_US
	};

	struct PurchaseInfo
	{
		VeStringA m_kInfo;
		VeUInt32 m_u32Price;
		PurchaseType m_eType;
		VeUInt32 m_u32Chips;
		VeUInt32 m_u32Icon;
		bool m_bVip;
	};

	typedef VeVector<PurchaseInfo> PurchaseInfoList;

	class KeypadDelegateLayer : public cocos2d::CCLayer
	{
	public:
		virtual void keyBackClicked();

		virtual void keyMenuClicked();

		CREATE_FUNC(KeypadDelegateLayer);
	};

	typedef VeEvent<> FadeEvent;

	typedef VeAbstractDelegate<> FadeDelegate;

	TexasPoker();

	virtual ~TexasPoker();

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

	virtual void onEnterTransitionDidFinish();

	void Restore();

	virtual void visit();

	Language GetLanguage();

	const VeChar8* GetString(const VeChar8* pcName);

	const VeChar8* GetNumber(VeUInt64 u64Number, VeUInt32 u32Show);

	const VeChar8* GetNumberLimit(VeUInt64 u64Number, VeUInt32 u32Limit);

	VeFloat32 GetScaleHD();

	cocos2d::CCTexture2D* GetLoginTexture(VeUInt32 u32Index);

	CREATE_FUNC(TexasPoker);

	void ShowLoading(bool bMiddle = true);

	UILayerManager* GetLayerManager();

	VeStringA LoadImage(const VeChar8* pcName, bool bLanguage = true);

	VeStringA GetImagePath(const VeChar8* pcName, bool bLanguage = true);

	VeUInt32 GetNumTexInLoading();

	bool IsFading();

	void FadeOut(VeFloat32 f32Time, FadeDelegate* kDelegate = NULL);

	void FadeIn(VeFloat32 f32Time, FadeDelegate* kDelegate = NULL);

	void Toast(const VeChar8* pcContent, bool bInsert = true);

	VeString GetDeviceID();

	VeString GetSysLanguage();

	void ShowFeedBack();

	void ConnectToHallServer();

	void DisconnectFromHallServer();

	void LoginToHallServer();

	void ShowAlert(const UIUserDataPtr& spUserData);

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	static const VeChar8* jstringTostring2(JNIEnv* env, jstring jstr);
#endif

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	VeString GetWindowsDeviceID();
#endif

	VeString GetClientName();

	void UpdateServerTime(VeUInt64 u64Time);

	VeUInt64 GetServerTime();

	void ShowKenoResult();

	void RefreshServerNotice();

	void OnServerNoticeUpdate(const VeChar8* pcResult);

	const VeVector< VePair<VeStringA,VeStringA> >& GetLeaderboard();

	void ShowExit();

	void StartRefreshPurchase();

	void EndRefreshPurchase();

	void TickRefreshPurchase(VeFloat32 f32Delta);

	void OnPurchaseEnd(VeInt32 i32State);

	void VerifyPurchase(const VeChar8* pcValue);

	void VerifyIOSPurchase(const VeChar8* pcValue, const VeChar8* pcPostData);

	void UpdatePurchaseList();

	void ShowPurchaseAlert(bool bSuccess, const VeChar8* pcContent = NULL);

	void NeedStopLoading();

	void ShowNotice(const VeChar8* strNotice, VeUInt16 u16Show, VeUInt16 u16Interval);

	void ClearAlwaysNotice();

	void RestartNoticeAlways();

protected:
	friend class ToastCallback;

	void FadeUpdate(VeFloat32 f32Delta);

	void FadeOutUpdate(VeFloat32 f32Delta);

	void UpdateToast(VeFloat32 f32Delta);

protected:
	friend class Login;

	void LoadSoundEffect(SoundEffect eSound, const VeChar8* pcName, bool bLanguage = false);

	void LoadImages();

	void LoadSoundEffects();

	void LoadShaders();

	void LoadLogin();

	void UnloadLogin();

	void ReleaseShaders();

	void RestoreShaders();

	UILayerManager* m_pkManager;

protected:
	Version m_kVersion;
	Language m_eLanguate;
	const VeChar8* m_pcLanguage;
	const VeChar8* m_pcImagePath;
	VeStringMap<VeFixedStringA> m_kStringMap;
	VeFloat32 m_f32ScaleHD;
	bool m_bNeedRestore;

public:
	cocos2d::CCGLProgram* GetShader(Shader eShader);

	VeUInt32 GetShaderParams(Shader eShader);

protected:
	cocos2d::CCGLProgram* m_apkShaders[SHADER_MAX];
	VeUInt32 m_au32Params[SHADER_MAX];

protected:
	void Update(VeFloat32 f32Delta);

	void UpdateVenus3D(VeFloat32 f32Delta);

	void UpdateSyncServerTime(VeFloat32 f32Delta);

	void LoadPng(const VeChar8* pcName, bool bLanguage = true);

	void LoadingCallBack(CCObject* pkObject);

	void LoadingPvrcczCallBack(CCObject* pkObject);

	void UpdateVolume();

	cocos2d::CCSprite* m_pkLogo;
	VeUInt32 m_u32TexturesInLoading;
	VeVector<cocos2d::CCTexture2D*> m_kLoginTextureArray;
	VeVector<VeStringA> m_kLoading;
	VeVector<CCObject*> m_kObjects;
	UIUserDataPtr m_spExitCallback;
	UIUserDataPtr m_spLoadingData;
	ToastCallbackPtr m_spToastCallback;

private:
	cocos2d::CCSprite* m_pkFadeQuad;
	VeFloat32 m_f32FadeFactor;
	VeFloat32 m_f32FadeTime;
	VeFloat32 m_f32FadePassTime;
	VeFloat32 m_f32FadeType;
	VeUInt32 m_u32WaitTick;
	FadeEvent m_kFadeEvent;

	VeUInt64 m_u64ServerTime;
	VeFloat64 m_f64ServerSyncTime;

private:
	VeStringA m_kHttpServer;
	VeStringA m_kHallServerIp;
	VeUInt32 m_u32HallServerPort;
	VeBackgroundTaskQueue m_kCurlQueue;
	VeStringA m_kServerNotice;
	VeVector< VePair<VeStringA,VeStringA> > m_kLeaderboard;

private:
	VeList<VeStringA> m_kNoticeCache;
	VeList<VeStringA> m_kNoticeAlways;

	void OnNoticeCache(VeFloat32 f32Delta);

	void OnNoticeAlways(VeFloat32 f32Delta);

private:
	volatile bool m_bNeedStopLoading;

};

extern TexasPoker* g_pkGame;

#define TT(text) (g_pkGame->GetString(text))
#define NT(num,max) (g_pkGame->GetNumber(num,max))
#define NTL(num,lim) (g_pkGame->GetNumberLimit(num,lim))
#define g_f32ScaleHD (g_pkGame->GetScaleHD())
#define g_pLayerManager (g_pkGame->GetLayerManager())
#define GetUILayer(class,name) ((class*)(g_pLayerManager->GetLayer(name)))

#define EVENT_RATE (30.0f * g_f32ScaleHD)
#define FRICTION_PER_SEC (1000.0f * g_f32ScaleHD)
#define DAMPING (200.0f * g_f32ScaleHD)
#define ROLLBACK_TIME 0.5f
#define VELOCITYMAX (1000.0f * g_f32ScaleHD)
#define SCROLLBAR_HIDE 0.3f
