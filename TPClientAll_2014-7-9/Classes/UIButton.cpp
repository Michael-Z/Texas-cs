#include "UIButton.h"
#include "SoundSystem.h"

USING_NS_CC;

VeUInt32 UIButton::ms_u32PressedSound = 0;

UIButton::UIButton(const VeChar8* pcNormal, const VeChar8* pcPressed, const VeChar8* pcDisabled)
	: UIWidget(BUTTON), m_eState(NORMAL), m_u32PressedSound(0)
{
	m_kDelegate.Set(this, &UIButton::OnEvent);
	RegisterListener(m_kDelegate);
	if(pcNormal)
	{
		m_pkNormal = CCSprite::createWithSpriteFrameName(pcNormal);
		m_pkNormal->setAnchorPoint(ccp(0.5f, 0.5f));
		addChild(m_pkNormal, 1);
	}
	else
	{
		m_pkNormal = NULL;
	}
	
	if(pcPressed)
	{
		m_pkPressed = CCSprite::createWithSpriteFrameName(pcPressed);
		m_pkPressed->setAnchorPoint(ccp(0.5f, 0.5f));
		addChild(m_pkPressed, 1);
		m_pkPressed->setVisible(false);
	}
	else
	{
		m_pkPressed = NULL;
	}	

	if(pcDisabled)
	{
		m_pkDisabled = CCSprite::createWithSpriteFrameName(pcDisabled);
		m_pkDisabled->setAnchorPoint(ccp(0.5f, 0.5f));
		addChild(m_pkDisabled, 1);
		m_pkDisabled->setVisible(false);
	}
	else
	{
		m_pkDisabled = NULL;
	}

	if(m_pkNormal)
		setContentSize(m_pkNormal->getContentSize());
}

UIButton::UIButton(CCNode* pkNormal, CCNode* pkPressed, CCNode* pkDisabled)
	: UIWidget(BUTTON), m_eState(NORMAL), m_u32PressedSound(0)
{
	m_kDelegate.Set(this, &UIButton::OnEvent);
	RegisterListener(m_kDelegate);
	if(pkNormal)
	{
		addChild(pkNormal, 1);
		m_pkNormal = pkNormal;
	}
	else
	{
		m_pkNormal = NULL;
	}

	if(pkPressed)
	{
		addChild(pkPressed, 1);
		m_pkPressed = pkPressed;
		m_pkPressed->setVisible(false);
	}
	else
	{
		m_pkPressed = NULL;
	}	

	if(pkDisabled)
	{
		addChild(pkDisabled, 1);
		m_pkDisabled = pkDisabled;
		m_pkDisabled->setVisible(false);
	}
	else
	{
		m_pkDisabled = NULL;
	}

	if(m_pkNormal)
		setContentSize(m_pkNormal->getContentSize());
}

UIButton::~UIButton()
{

}

void UIButton::SetEnable(bool bEnable)
{
	UIWidget::SetEnable(bEnable);
	UpdateAnimation();
}

UIWidget* UIButton::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			if(m_eState != PRESS)
			{
				m_eState = PRESS;
				UpdateAnimation();
				m_kEvent.Callback(*this, PRESSED);
			}
			break;
		case UILayer::TOUCH_MOVED:
			/*if(m_eState != PRESS)
			{
				m_eState = PRESS;
				UpdateAnimation();
				m_kEvent.Callback(*this, MOVE_IN);
			}*/
			break;
		case UILayer::TOUCH_ENDED:
		case UILayer::TOUCH_CANCELLED:
			if(m_eState != NORMAL)
			{
				m_eState = NORMAL;
				UpdateAnimation();
				m_kEvent.Callback(*this, RELEASED);
			}
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void UIButton::EventRelease()
{
	if(m_eState == PRESS)
	{
		m_eState = NORMAL;
		UpdateAnimation();
		m_kEvent.Callback(*this, MOVE_OUT);
	}
}

void UIButton::UpdateAnimation()
{
	if(GetEnable())
	{
		if(m_eState == NORMAL)
		{
			if(m_pkNormal)
				m_pkNormal->setVisible(true);
			if(m_pkPressed)
				m_pkPressed->setVisible(false);
		}
		else
		{
			if(m_pkNormal)
				m_pkNormal->setVisible(false);
			if(m_pkPressed)
				m_pkPressed->setVisible(true);
		}
		if(m_pkDisabled)
			m_pkDisabled->setVisible(false);
	}
	else
	{
		if(m_pkNormal)
			m_pkNormal->setVisible(false);
		if(m_pkPressed)
			m_pkPressed->setVisible(false);
		if(m_pkDisabled)
			m_pkDisabled->setVisible(true);
	}
}

cocos2d::CCNode* UIButton::Normal()
{
	return m_pkNormal;
}

cocos2d::CCNode* UIButton::Pressed()
{
	return m_pkPressed;
}

cocos2d::CCNode* UIButton::Disabled()
{
	return m_pkDisabled;
}

void UIButton::OnEvent(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == PRESSED || u32State == MOVE_IN)
	{
		VeUInt32 u32Sound = GetPressedSound();
		if(u32Sound)
		{
			g_pSoundSystem->PlayEffect(u32Sound);
		}
	}
}

VeUInt32 UIButton::GetPressedSound()
{
	if(m_u32PressedSound == VE_INFINITE)
	{
		return 0;
	}
	else if(m_u32PressedSound == 0)
	{
		return ms_u32PressedSound;
	}
	else
	{
		return m_u32PressedSound;
	}
}

void UIButton::SetPressedSound(VeUInt32 u32SoundID)
{
	m_u32PressedSound = u32SoundID;
}

void UIButton::SetPressedSoundGlobal(VeUInt32 u32SoundID)
{
	ms_u32PressedSound = u32SoundID;
}

UIChooseButton::UIChooseButton(const VeChar8* pcNormal, const VeChar8* pcPressed, const VeChar8* pcDisabled)
	: UIButton(pcNormal, pcPressed, pcDisabled)
{

}

UIWidget* UIChooseButton::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			SetChoose(true);
			break;
		default:
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void UIChooseButton::SetChoose(bool bChoose)
{
	State eState = bChoose ? PRESS : NORMAL;
	if(m_eState != eState)
	{
		m_eState = eState;
		UpdateAnimation();
		if(bChoose)
		{
			m_kEvent.Callback(*this, PRESSED);
		}
		else
		{
			m_kEvent.Callback(*this, RELEASED);
		}
	}
}

UIChooseButtonArea::UIChooseButtonArea() : m_pkCurrent(NULL)
{

}

void UIChooseButtonArea::AddButton(UIChooseButton* pkButton)
{
	pkButton->setTag(m_kButtons.Size());
	m_kButtons.Resize(m_kButtons.Size() + 1);
	m_kButtons.Back().m_tFirst.Set(this, &UIChooseButtonArea::OnEvent);
	m_kButtons.Back().m_tSecond = pkButton;
	AddChild(pkButton);
	pkButton->SetChoose(false);
	pkButton->RegisterListener(m_kButtons.Back().m_tFirst);
}

void UIChooseButtonArea::Choose(VeUInt32 u32Index)
{
	UIChooseButton* pkButton(NULL);
	if(u32Index < m_kButtons.Size())
	{
		pkButton = m_kButtons[u32Index].m_tSecond;
	}
	if(pkButton != m_pkCurrent)
	{
		if(m_pkCurrent)
		{
			m_pkCurrent->SetChoose(false);
		}
		if(pkButton)
		{
			pkButton->SetChoose(true);
		}
		m_pkCurrent = pkButton;
		m_kEvent.Callback(*this, VALUE_CHANGE);
	}
	m_kEvent.Callback(*this, PRESSED);
}

UIChooseButton* UIChooseButtonArea::GetCurrent()
{
	return m_pkCurrent;
}

VeUInt32 UIChooseButtonArea::GetCurrentIndex()
{
	for(VeUInt32 i(0); i < m_kButtons.Size(); ++i)
	{
		if(m_pkCurrent == m_kButtons[i].m_tSecond)
		{
			return i;
		}
	}
	return VE_INFINITE;
}

VeUInt32 UIChooseButtonArea::GetButtonsNum()
{
	return m_kButtons.Size();
}

void UIChooseButtonArea::OnEvent(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == PRESSED)
	{
		Choose(kWidget.getTag());
	}
}

UIWidget* UIChooseButtonArea::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(!m_bProcessInput) return this;
	if(!m_bVisible) return NULL;

	m_kChildren.BeginIterator();
	while(!m_kChildren.IsEnd())
	{
		UIWidget* pkWidget = m_kChildren.GetIterNode()->m_tContent;
		m_kChildren.Next();
		VE_ASSERT(pkWidget);
		if(pkWidget->TestInput(eType, pkTouch))
		{
			UIWidget* pkRes = pkWidget->ProcessTouch(eType, pkTouch);
			if(pkRes)
			{
				m_kChildren.EndIterator();
				return pkRes;
			}
		}
	}
	return NULL;
}

bool UIChooseButtonArea::TestInput(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	return true;
}

UICheckBox::UICheckBox(const VeChar8* pcNormal, const VeChar8* pcChecked, const VeChar8* pcDisabled)
	: UIButton(pcNormal, pcChecked, pcDisabled), m_bIsCheck(false)
{

}

UIWidget* UICheckBox::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			Change();
			break;
        default:
            break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void UICheckBox::SetCheck(bool bIsCheck)
{
	if(bIsCheck != m_bIsCheck)
	{
		Change();
	}
}

bool UICheckBox::IsCheck()
{
	return m_bIsCheck;
}

void UICheckBox::Change()
{
	m_bIsCheck = !m_bIsCheck;
	m_eState = m_bIsCheck ? PRESS : NORMAL;
	UpdateAnimation();
	m_kEvent.Callback(*this, VALUE_CHANGE);
}
