#include "UIInputBox.h"
#include "TexasPoker.h"

USING_NS_CC;

UIInputBox::UIInputBox(const VeChar8* pcPlaceHolder, const VeChar8* pcFont, VeFloat32 f32Size, VeUInt32 u32MaxLength)
{
	m_u32MaxLength = u32MaxLength;
	m_pkRoot = CCNode::create();
	m_pkTextField = CCTextFieldTTF::textFieldWithPlaceHolder(pcPlaceHolder, pcFont, f32Size);
	VE_ASSERT(m_pkTextField);
	m_pkTextField->setDelegate(this);
	m_pkRoot->addChild(m_pkTextField);
	addChild(m_pkRoot, 2);
	m_pkTextField->setAnchorPoint(ccp(0,0));
	m_kDelegate.Set(this, &UIInputBox::OnEvent);
	RegisterListener(m_kDelegate);

	m_pkCursorSprite = CCSprite::createWithSpriteFrameName("point.png");
	m_pkCursorSprite->setScaleX(2 * g_f32ScaleHD);
	m_pkCursorSprite->setScaleY(VeFloorf(f32Size * 1.2f));
	CCSize kFieldSize = m_pkTextField->getContentSize();
	m_kCursorPos = ccp(0, kFieldSize.height / 2);
	m_pkCursorSprite->setPosition(m_kCursorPos);
	m_pkRoot->addChild(m_pkCursorSprite);
	m_pkCursorSprite->runAction(CCRepeatForever::create((CCActionInterval *) CCSequence::create(CCFadeOut::create(0.25f), CCFadeIn::create(0.25f), NULL)));
	m_pkCursorSprite->setVisible(false);
	UpdateTextToCenter();
}

UIInputBox::~UIInputBox()
{

}

UIWidget* UIInputBox::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
	if(pkWidget) return pkWidget;

	if(m_bEnabled && m_bVisible)
	{
		switch(eType)
		{
		case UILayer::TOUCH_BEGAN:
			if(!GetActive())
			{
				SetActive(true);
			}
			else
			{
				m_pkTextField->attachWithIME();
			}
			break;
		case UILayer::TOUCH_MOVED:
		case UILayer::TOUCH_ENDED:
		case UILayer::TOUCH_CANCELLED:
			break;
		}

		return this;
	}
	else
	{
		return NULL;
	}
}

void UIInputBox::EventRelease()
{
	
}

void UIInputBox::UpdateTextToCenter()
{
	const CCSize& kSize = m_pkTextField->getContentSize();
	m_pkRoot->setPosition(ccp(-VeFloorf(kSize.width * 0.5f), -VeFloorf(kSize.height * 0.5f)));
	UpdateCursor();
}

void UIInputBox::UpdateTextToLeft()
{
	const CCSize& kSize = getContentSize();
	const CCSize& kFieldSize = m_pkTextField->getContentSize();
	m_pkRoot->setPosition(ccp(-VeFloorf(kSize.width * 0.5f), -VeFloorf(kFieldSize.height * 0.5f)));
	UpdateCursor();
}

void UIInputBox::UpdateCursor()
{
	if(m_kInputText.empty())
	{
		m_pkCursorSprite->setPositionX(0);
	}
	else
	{
		m_pkCursorSprite->setPositionX(m_pkTextField->getContentSize().width);
	}
}

void UIInputBox::UpdateField()
{
	if(m_kInputText.empty())
	{
		m_pkTextField->setVisible(false);
	}
	else
	{
		m_pkTextField->setVisible(true);
	}
}

void UIInputBox::OnEvent(UIWidget& kWidget, VeUInt32 u32State)
{
	switch(u32State)
	{
	case ACTIVE:
		{
			m_pkTextField->attachWithIME();
		}
		break;
	case INACTIVE:
		{
			m_pkTextField->detachWithIME();
		}
		break;
	default:
		break;
	}
}

bool UIInputBox::onTextFieldAttachWithIME(CCTextFieldTTF *pSender)
{
	if(!GetActive())
	{
		SetActive(true);
		return true;
	}
	else
	{
		m_pkCursorSprite->setVisible(true);
		UpdateTextToLeft();
		UpdateField();
		return false;
	}
}

bool UIInputBox::onTextFieldDetachWithIME(CCTextFieldTTF * pSender)
{
	if(GetActive())
	{
		SetActive(false);
		return true;
	}
	else
	{
		m_pkCursorSprite->setVisible(false);
		UpdateTextToCenter();
		m_pkTextField->setVisible(true);
		return false;
	}
}

bool UIInputBox::onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen)
{
	if((*text) == '\n')
	{
		return false;
	}
	else
	{
		if(nLen + m_kInputText.size() <= m_u32MaxLength)
		{
			m_kInputText += text;
			m_pkTextField->setString(m_kInputText.c_str());
			UpdateCursor();
			UpdateField();
			m_kEvent.Callback(*this, VALUE_CHANGE);
			return true;
		}
		return true;
	}
}

bool UIInputBox::onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen)
{
	m_kInputText.resize(m_kInputText.size() - nLen);
	m_pkTextField->setString(m_kInputText.c_str());
	UpdateCursor();
	UpdateField();
	m_kEvent.Callback(*this, VALUE_CHANGE);
	return true;
}

void UIInputBox::SetText(const VeChar8* pcText)
{
	m_kInputText = pcText;
	m_pkTextField->setString(m_kInputText.c_str());
	if(m_pkCursorSprite->isVisible())
	{
		UpdateTextToLeft();
	}
	else
	{
		UpdateTextToCenter();
	}
}

const VeChar8* UIInputBox::GetText()
{
	return m_kInputText.c_str();
}
