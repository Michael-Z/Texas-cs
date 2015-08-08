#pragma once

#include "UIWidget.h"

class UIScrollView : public UIWidget
{
public:
	UIScrollView(VeFloat32 f32Width, VeFloat32 f32Height, const VeChar8* pcBean, const VeChar8* pcBeanExt, VeFloat32 f32BarBias);

	virtual ~UIScrollView();

	virtual void AddChild(UIWidget* pkWidget, VeInt32 i32ZOrder = 3);

	virtual void UpdateBounding();

	void SetScrollBarColor(const cocos2d::ccColor3B& value);

	void SetScrollBarOpacity(GLubyte value);

	void Update(VeFloat32 f32Delta);

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	virtual bool IsIn(VeFloat32 x, VeFloat32 y);

	void ScrollToTop();

	void ScroolToBottom();

protected:
	void UpdateScrollBar();

	void OnRelease();

	cocos2d::CCNode* m_pkInnerNode;
	cocos2d::CCNode* m_pkScrollBarV;
	cocos2d::CCSprite* m_apkScrollBarV[3];
	VeFloat32 m_f32MinX;
	VeFloat32 m_f32MaxX;
	VeFloat32 m_f32MinY;
	VeFloat32 m_f32MaxY;
	VeFloat32 m_f32MoveLeft;
	VeFloat32 m_f32MoveRight;
	VeFloat32 m_f32MoveBottom;
	VeFloat32 m_f32MoveTop;
	VeFloat32 m_f32ScrollBarLen;
	VeFloat32 m_f32Velocity;
	bool m_bOnTouch;
	bool m_bRollBack;
	VeFloat32 m_f32RollBackTime;
	VeFloat32 m_f32RollBackPosY;
	GLubyte m_ubyOpacity;
	bool m_bShowScrollBar;
	VeFloat32 m_fHideScrollBarPass;
	bool m_bCanScroll;

};
