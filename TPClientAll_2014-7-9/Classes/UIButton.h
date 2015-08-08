#pragma once

#include "UIWidget.h"

class UIButton : public UIWidget
{
public:
	enum State
	{
		NORMAL,
		PRESS
	};

	typedef VeMemberDelegate<UIButton, UIWidget&, VeUInt32> Delegate;

	UIButton(const VeChar8* pcNormal, const VeChar8* pcPressed, const VeChar8* pcDisabled = NULL);

	UIButton(cocos2d::CCNode* pkNormal, cocos2d::CCNode* pkPressed, cocos2d::CCNode* pkDisabled = NULL);

	virtual ~UIButton();

	virtual void SetEnable(bool bEnable);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	cocos2d::CCNode* Normal();

	cocos2d::CCNode* Pressed();

	cocos2d::CCNode* Disabled();

	VeUInt32 GetPressedSound();

	void SetPressedSound(VeUInt32 u32SoundID);

	static void SetPressedSoundGlobal(VeUInt32 u32SoundID);

protected:
	void UpdateAnimation();

	virtual void OnEvent(UIWidget& kWidget, VeUInt32 u32State);

	Delegate m_kDelegate;
	State m_eState;
	cocos2d::CCNode* m_pkNormal;
	cocos2d::CCNode* m_pkPressed;
	cocos2d::CCNode* m_pkDisabled;
	VeUInt32 m_u32PressedSound;

	static VeUInt32 ms_u32PressedSound;

};

class UIChooseButton : public UIButton
{
public:
	UIChooseButton(const VeChar8* pcNormal, const VeChar8* pcPressed, const VeChar8* pcDisabled = NULL);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease() {}

	void SetChoose(bool bChoose);

};

class UIChooseButtonArea : public UIWidget
{
public:
	typedef VeMemberDelegate<UIChooseButtonArea, UIWidget&, VeUInt32> UIDelegate;

	UIChooseButtonArea();

	void AddButton(UIChooseButton* pkButton);

	void Choose(VeUInt32 u32Index);

	UIChooseButton* GetCurrent();

	VeUInt32 GetCurrentIndex();

	VeUInt32 GetButtonsNum();

	void OnEvent(UIWidget& kWidget, VeUInt32 u32State);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual bool TestInput(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease() {}

protected:
	VeVector< VePair<UIDelegate,UIChooseButton*> > m_kButtons;
	UIChooseButton* m_pkCurrent;

};

class UICheckBox : public UIButton
{
public:
	UICheckBox(const VeChar8* pcNormal, const VeChar8* pcChecked, const VeChar8* pcDisabled = NULL);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease() {}

	void SetCheck(bool bIsCheck);

	bool IsCheck();

	void Change();

protected:
	bool m_bIsCheck;


};