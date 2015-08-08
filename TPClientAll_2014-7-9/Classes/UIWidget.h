#pragma once

#include "UILayer.h"

class UIWidget : public cocos2d::CCNode, public VeRefNode<UIWidget*>
{
public:
	enum Type
	{
		UNKNOWN,
		BUTTON,
		TYPE_NUM
	};

	enum UIEvent
	{
		ENABLE,
		DISABLE,
		SHOW,
		HIDE,
		ACTIVE,
		INACTIVE,

		PRESSED,
		RELEASED,
		MOVE_IN,
		MOVE_OUT,

		CHANGE_INDEX,
		MOVE,
		STOP,

		VALUE_CHANGE,
		VALUE_CHANGED
	};

	typedef VeEvent<UIWidget&,VeUInt32> Event;

	typedef VeAbstractDelegate<UIWidget&,VeUInt32> Delegate;

	UIWidget();

	virtual ~UIWidget();

	Type GetType();

	virtual void SetEnable(bool bEnable);

	bool GetEnable();

	virtual void SetVisible(bool bVisible);

	bool GetVisible();

	virtual void SetActive(bool bActive);

	bool GetActive();

	virtual void SetProcessInput(bool bProcess);

	bool GetProcessInput();

	virtual void SetClipByRange(bool bClip);

	bool GetClipByRange();

	bool IsRoot();

	UIWidget* GetParent();

	VeUInt32 GetChildCount();

	UIWidget* GetChild(VeUInt32 u32Index);

	VeRefList<UIWidget*>& GetChildren();

	virtual void AddChild(UIWidget* pkWidget, VeInt32 i32ZOrder = 3);

	void EnableRange(VeFloat32 f32OffsetX, VeFloat32 f32OffsetY, VeFloat32 f32Width, VeFloat32 f32Height);

	void DisableRange();

	virtual UIWidget* ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual void EventRelease();

	virtual bool TestInput(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

	virtual bool IsIn(VeFloat32 x, VeFloat32 y);

	virtual bool IntersectWith(const cocos2d::CCRect kBounding);

	void RegisterListener(Delegate& kDelegate);

	void UnregisterListener(Delegate& kDelegate);

	void SetTouchAnchorPoint(const VeVector2D& kAnchorPoint);

	const VeVector2D& GetTouchAnchorPoint();

	virtual void visit();

	void SetParent(UIWidget* pkParent);

	void ToTop();

	void ToBottom();

protected:
	UIWidget(Type eType);

	const Type m_eType;
	UIWidget* m_pkParent;
	VeRefList<UIWidget*> m_kChildren;
	VeVector4D* m_pkRange;
	Event m_kEvent;
	VeVector2D m_kAnchorPoint;
	bool m_bEnabled;
	bool m_bVisible;
	bool m_bActive;
	bool m_bProcessInput;
	bool m_bClipByRange;

};
