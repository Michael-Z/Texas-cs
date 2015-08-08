#pragma once

#include "UIWidget.h"

class UIInputBox : public UIWidget, public cocos2d::CCTextFieldDelegate
{
public:
	typedef VeMemberDelegate<UIInputBox, UIWidget&, VeUInt32> Delegate;

	UIInputBox(const VeChar8* pcPlaceHolder, const VeChar8* pcFont, VeFloat32 f32Size, VeUInt32 u32MaxLength = 12);

	virtual ~UIInputBox();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	virtual bool onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF *pSender);

	virtual bool onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF * pSender);

	virtual bool onTextFieldInsertText(cocos2d::CCTextFieldTTF * pSender, const char * text, int nLen);

	virtual bool onTextFieldDeleteBackward(cocos2d::CCTextFieldTTF * pSender, const char * delText, int nLen);

	void SetText(const VeChar8* pcText);

	const VeChar8* GetText();

protected:
	void UpdateTextToCenter();

	void UpdateTextToLeft();

	void UpdateCursor();

	void UpdateField();

	void OnEvent(UIWidget& kWidget, VeUInt32 u32State);

	Delegate m_kDelegate;
	cocos2d::CCNode* m_pkRoot;
	cocos2d::CCTextFieldTTF* m_pkTextField;
	cocos2d::CCSprite* m_pkCursorSprite;
	cocos2d::CCPoint m_kCursorPos;
	std::string m_kInputText;
	VeUInt32 m_u32MaxLength;

};
