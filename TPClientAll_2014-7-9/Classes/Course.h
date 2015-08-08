#pragma once

#include "UILayer.h"
#include "UIAlert.h"

class UIButton;

class Course : public UILayerExt<Course>
{
public:
	typedef VeMemberDelegate<Course> FadeDelegate;

	Course();

	virtual ~Course();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	void HelpOKMusicEffect();

	void SetButtonEnable(CCNode* pTarget, void* data);

	void CourseUpDate(VeFloat32 f32Delta);

	void ScrollCourseText();

	void HideNotice123();

	void HideNotice456();

	VE_CREATE_FUNC(Course);

protected:
	void EnterGame();

	void OnFadeOut();
	FadeDelegate m_kOnFadeOut;

	UIPanel* pkTest1;
	cocos2d::CCSprite* pkPrompt1;
	cocos2d::CCLabelTTF* m_pkCourseText1;
	UIPanel* pkTest2;
	cocos2d::CCSprite* pkPrompt2;
	cocos2d::CCLabelTTF* m_pkCourseText2;
	UIPanel* pkTest3;
	cocos2d::CCSprite* pkPrompt3;
	cocos2d::CCLabelTTF* m_pkCourseText3;
	UIPanel* pkTest4;
	cocos2d::CCSprite* pkPrompt4;
	cocos2d::CCLabelTTF* m_pkCourseText4;
	UIPanel* pkTest5;
	cocos2d::CCSprite* pkPrompt5;
	cocos2d::CCSprite* pkDrinkLogo;
	cocos2d::CCLabelTTF* m_pkCourseText5;
	UIPanel* pkTest6;
	cocos2d::CCSprite* pkPrompt6;
	cocos2d::CCLabelTTF* m_pkCourseText6;

	UIDeclareDelegate(OnUnderstand);
	UIButton* pkUnderstand;

};
