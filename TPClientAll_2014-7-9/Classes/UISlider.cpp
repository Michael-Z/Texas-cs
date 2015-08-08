#include "UISlider.h"

USING_NS_CC;

UISlider::UISlider(const VeChar8* pcHandle, VeFloat32 f32Range, const VeChar8* pcBar, const VeChar8* pcBarLight)
	: m_f32Range(f32Range), m_f32Value(0.0f), m_pkBarLight(NULL), m_bChange(false)
{
	m_pkHandle = CCSprite::createWithSpriteFrameName(pcHandle);
	addChild(m_pkHandle, 3);

	if(pcBar)
	{
		CCSprite* pkBar = CCSprite::createWithSpriteFrameName(pcBar);
		addChild(pkBar, 0);
		m_f32BarWidth = pkBar->getContentSize().width;
		if(pcBarLight)
		{
			m_pkBarLight = CCProgressTimer::create(CCSprite::createWithSpriteFrameName(pcBarLight));
			m_pkBarLight->setType(kCCProgressTimerTypeBar);
			m_pkBarLight->setMidpoint(ccp(0,0));
			m_pkBarLight->setBarChangeRate(ccp(1, 0));
			addChild(m_pkBarLight, 0);
		}
		else
		{
			m_pkBarLight = NULL;
		}
	}


	Update();
	CCSize kSize = m_pkHandle->getContentSize();
	kSize.width += m_f32Range * 2.0f;
	setContentSize(kSize);
}

UISlider::~UISlider()
{

}

void UISlider::Update()
{
	VeFloat32 f32Pos = VeFloorf(VeLerp(m_f32Value, -m_f32Range, m_f32Range));
	m_pkHandle->setPositionX(f32Pos);
	if(m_pkBarLight)
	{
		VeFloat32 f32Percentage = (m_f32BarWidth * 0.5f + f32Pos) * 100.0f / m_f32BarWidth;
		f32Percentage = VE_MIN(f32Percentage, 100.0f);
		f32Percentage = VE_MAX(f32Percentage, 0.0f);
		m_pkBarLight->setPercentage(f32Percentage);
	}
}

void UISlider::SetValue(VeFloat32 f32Value)
{
	if(f32Value != m_f32Value)
	{
		m_f32Value = f32Value;
		Update();
		m_kEvent.Callback(*this, VALUE_CHANGE);
	}
}

void UISlider::SetValueWithEvent(VeFloat32 f32Value)
{
	if(f32Value != m_f32Value)
	{
		m_f32Value = f32Value;
		Update();
		m_kEvent.Callback(*this, VALUE_CHANGE);
		m_bChange = true;
	}
}

VeFloat32 UISlider::GetValue()
{
	return m_f32Value;
}

UIWidget* UISlider::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
		case UILayer::TOUCH_MOVED:
			{
				VeFloat32 f32WidgetX = getPositionX();
				VeFloat32 f32Touch = pkTouch->getLocation().x;
				VeFloat32 f32Min = f32WidgetX - m_f32Range * ui_main_scale;
				VeFloat32 f32Max = f32WidgetX + m_f32Range * ui_main_scale;
				f32Touch = VE_MAX(f32Min, f32Touch);
				f32Touch = VE_MIN(f32Max, f32Touch);
				SetValueWithEvent((f32Touch - f32Min) / (f32Max - f32Min));
			}
			break;
		case UILayer::TOUCH_ENDED:
		case UILayer::TOUCH_CANCELLED:
			EventRelease();
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void UISlider::EventRelease()
{
	if(m_bChange)
	{
		m_kEvent.Callback(*this, VALUE_CHANGED);
		m_bChange = false;
	}
}

UISliderV::UISliderV(const VeChar8* pcHandle, VeFloat32 f32Range, const VeChar8* pcBar, const VeChar8* pcBarLight)
	: m_f32Range(f32Range), m_f32Value(0.0f), m_pkBarLight(NULL), m_bChange(false)
{
	m_pkHandle = CCSprite::createWithSpriteFrameName(pcHandle);
	addChild(m_pkHandle, 3);

	if(pcBar)
	{
		CCSprite* pkBar = CCSprite::createWithSpriteFrameName(pcBar);
		addChild(pkBar, 0);
		m_f32BarHeight = pkBar->getContentSize().height;
		if(pcBarLight)
		{
			m_pkBarLight = CCProgressTimer::create(CCSprite::createWithSpriteFrameName(pcBarLight));
			m_pkBarLight->setType(kCCProgressTimerTypeBar);
			m_pkBarLight->setMidpoint(ccp(0,0));
			m_pkBarLight->setBarChangeRate(ccp(0, 1));
			addChild(m_pkBarLight, 0);
		}
		else
		{
			m_pkBarLight = NULL;
		}
	}

	Update();
	CCSize kSize = m_pkHandle->getContentSize();
	kSize.height += m_f32Range * 2.0f;
	setContentSize(kSize);
}

UISliderV::~UISliderV()
{

}

void UISliderV::Update()
{
	VeFloat32 f32Pos = VeFloorf(VeLerp(m_f32Value, -m_f32Range, m_f32Range));
	m_pkHandle->setPositionY(f32Pos);
	VeFloat32 f32Percentage = (m_f32BarHeight * 0.5f + f32Pos) * 100.0f / m_f32BarHeight;
	f32Percentage = VE_MIN(f32Percentage, 100.0f);
	f32Percentage = VE_MAX(f32Percentage, 0.0f);
	if(m_pkBarLight)
		m_pkBarLight->setPercentage(f32Percentage);
}

void UISliderV::SetValue(VeFloat32 f32Value)
{
	if(f32Value != m_f32Value)
	{
		m_f32Value = f32Value;
		Update();
		m_kEvent.Callback(*this, VALUE_CHANGE);
	}
}

void UISliderV::SetValueWithEvent(VeFloat32 f32Value)
{
	if(f32Value != m_f32Value)
	{
		m_f32Value = f32Value;
		Update();
		m_kEvent.Callback(*this, VALUE_CHANGE);
		m_bChange = true;
	}
}

VeFloat32 UISliderV::GetValue()
{
	return m_f32Value;
}

UIWidget* UISliderV::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
		case UILayer::TOUCH_MOVED:
			{
				VeFloat32 f32WidgetY = getPositionY();
				VeFloat32 f32Touch = pkTouch->getLocation().y;
				VeFloat32 f32Min = f32WidgetY - m_f32Range * ui_main_scale;
				VeFloat32 f32Max = f32WidgetY + m_f32Range * ui_main_scale;
				f32Touch = VE_MAX(f32Min, f32Touch);
				f32Touch = VE_MIN(f32Max, f32Touch);
				SetValueWithEvent((f32Touch - f32Min) / (f32Max - f32Min));
			}
			break;
		case UILayer::TOUCH_ENDED:
		case UILayer::TOUCH_CANCELLED:
			EventRelease();
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void UISliderV::EventRelease()
{
	if(m_bChange)
	{
		m_kEvent.Callback(*this, VALUE_CHANGED);
		m_bChange = false;
	}
}
