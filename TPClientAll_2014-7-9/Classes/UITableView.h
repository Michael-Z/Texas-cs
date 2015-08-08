#pragma once

#include "UIWidget.h"

class UIChooseButtonArea;
class UIChooseButton;

class UIPage : public UIWidget
{
public:
	virtual bool TestInput(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch);

};

class UIPageView : public UIPage
{
public:
	UIPageView();

	virtual ~UIPageView();

	void AddPage(UIWidget* pkWidget, VeInt32 i32ZOrder = 3);

	virtual void Change(VeUInt32 u32Index);

	VeUInt32 GetCurrent();

protected:
	VeVector<UIWidget*> m_kWidgets;
	VeUInt32 m_u32Current;

};

class UITableView : public UIPageView
{
public:
	typedef VeMemberDelegate<UITableView, UIWidget&, VeUInt32> UIDelegate;

	UITableView(VeInt32 i32ZOrder = 3);

	virtual ~UITableView();

	virtual void Change(VeUInt32 u32Index);

	void AddButton(UIChooseButton* pkButton);

protected:
	void OnChange(UIWidget& kWidget, VeUInt32 u32State);

	UIDelegate m_kDelegate;
	UIChooseButtonArea* m_pkTables;

};
