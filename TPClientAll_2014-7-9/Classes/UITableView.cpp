#include "UITableView.h"
#include "UIButton.h"

USING_NS_CC;

bool UIPage::TestInput(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	return true;
}

UIPageView::UIPageView() : m_u32Current(VE_INFINITE)
{

}

UIPageView::~UIPageView()
{

}

void UIPageView::AddPage(UIWidget* pkWidget, VeInt32 i32ZOrder)
{
	pkWidget->setTag(m_kWidgets.Size());
	m_kWidgets.PushBack(pkWidget);
	AddChild(pkWidget, i32ZOrder);
	pkWidget->SetVisible(false);
}

void UIPageView::Change(VeUInt32 u32Index)
{
	u32Index = u32Index < m_kWidgets.Size() ? u32Index : VE_INFINITE;
	if(u32Index != m_u32Current)
	{
		if(m_u32Current < m_kWidgets.Size())
		{
			m_kWidgets[m_u32Current]->SetVisible(false);
		}
		if(u32Index < m_kWidgets.Size())
		{
			m_kWidgets[u32Index]->SetVisible(true);
		}
		m_u32Current = u32Index;
		m_kEvent.Callback(*this, VALUE_CHANGE);
	}
}

VeUInt32 UIPageView::GetCurrent()
{
	return m_u32Current;
}

UITableView::UITableView(VeInt32 i32ZOrder)
{
	m_kDelegate.Set(this, &UITableView::OnChange);
	m_pkTables = VE_NEW UIChooseButtonArea;
	AddChild(m_pkTables, i32ZOrder);
	m_pkTables->RegisterListener(m_kDelegate);
}

UITableView::~UITableView()
{

}

void UITableView::OnChange(UIWidget& kWidget, VeUInt32 u32State)
{
	if(u32State == UIWidget::VALUE_CHANGE)
	{
		VeUInt32 u32Index = m_pkTables->GetCurrent()->getTag();
		Change(u32Index);
	}
}

void UITableView::AddButton(UIChooseButton* pkButton)
{
	m_pkTables->AddButton(pkButton);
}

void UITableView::Change(VeUInt32 u32Index)
{
	UIPageView::Change(u32Index);
	m_pkTables->Choose(u32Index);
}
