#pragma once

#include "UIWidget.h"

class UISlider : public UIWidget
{
public:
	UISlider(const VeChar8* pcHandle, VeFloat32 f32Range, const VeChar8* pcBar = NULL, const VeChar8* pcBarLight = NULL);

	virtual ~UISlider();

	void SetValue(VeFloat32 f32Value);

	VeFloat32 GetValue();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

protected:
	virtual void Update();

	void SetValueWithEvent(VeFloat32 f32Value);

	cocos2d::CCSprite* m_pkHandle;
	const VeFloat32 m_f32Range;
	VeFloat32 m_f32Value;
	cocos2d::CCProgressTimer* m_pkBarLight;
	VeFloat32 m_f32BarWidth;
	bool m_bChange;

};

class UISliderV : public UIWidget
{
public:
	UISliderV(const VeChar8* pcHandle, VeFloat32 f32Range, const VeChar8* pcBar = NULL, const VeChar8* pcBarLight = NULL);

	virtual ~UISliderV();

	void SetValue(VeFloat32 f32Value);

	VeFloat32 GetValue();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

protected:
	virtual void Update();

	void SetValueWithEvent(VeFloat32 f32Value);

	cocos2d::CCSprite* m_pkHandle;
	const VeFloat32 m_f32Range;
	VeFloat32 m_f32Value;
	cocos2d::CCProgressTimer* m_pkBarLight;
	VeFloat32 m_f32BarHeight;
	bool m_bChange;
};
