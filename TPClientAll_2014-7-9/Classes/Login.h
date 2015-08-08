#pragma once

#include "UILayer.h"

class Login : public UILayerExt<Login>
{
public:
	enum
	{
		LOGIN_TEX_BACKGROUND,
		LOGIN_TEX_GAMELOGO_0,
		LOGIN_TEX_GAMELOGO_1,
		LOGIN_TEX_STARTFRAME,
		LOGIN_TEX_BOTTOMLOGO,
		LOGIN_TEX_STARTTEXT,
		LOGIN_TEX_MAX
	};

	typedef VeMemberDelegate<Login> FadeDelegate;

	Login();

	virtual ~Login();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	VE_CREATE_FUNC(Login);

	void ShowStart();

	void HideStart();

	void ShowNotice(const VeChar8* pcNotice);

	void IntoHall();

	static const VeChar8* GetName();

protected:
	void WaitLoading(VeFloat32 f32Delta);

	void OnFadeIn();
	FadeDelegate m_kOnFadeIn;

	void OnFadeOut();
	FadeDelegate m_kOnFadeOut;	

	cocos2d::CCSprite* m_pkStartText;
	
};
