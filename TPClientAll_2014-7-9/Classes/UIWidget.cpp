#include "UIWidget.h"

USING_NS_CC;

UIWidget::UIWidget()
	: m_eType(UNKNOWN), m_pkParent(NULL), m_pkRange(NULL), m_bEnabled(true)
	, m_bVisible(true), m_bActive(false), m_bProcessInput(true), m_bClipByRange(false)
{
	m_tContent = this;
	m_kAnchorPoint.x = 0.5f;
	m_kAnchorPoint.y = 0.5f;
}

UIWidget::UIWidget(Type eType)
	: m_eType(eType), m_pkParent(NULL), m_pkRange(NULL), m_bEnabled(true)
	, m_bVisible(true), m_bActive(false), m_bProcessInput(true), m_bClipByRange(false)
{
	m_tContent = this;
	m_kAnchorPoint.x = 0.5f;
	m_kAnchorPoint.y = 0.5f;
}

UIWidget::~UIWidget()
{
	removeFromParent();
	m_kChildren.BeginIterator();
	while(!m_kChildren.IsEnd())
	{
		UIWidget* pkWidget = m_kChildren.GetIterNode()->m_tContent;
		m_kChildren.Next();
		VE_ASSERT(pkWidget);
		VE_DELETE(pkWidget);
	}
	m_kChildren.Clear();
	VE_ASSERT(m_kChildren.Size() == 0);
	VE_SAFE_DELETE(m_pkRange);
}

UIWidget::Type UIWidget::GetType()
{
	return m_eType;
}

void UIWidget::SetEnable(bool bEnable)
{
	if(m_bEnabled != bEnable)
	{
		m_bEnabled = bEnable;
		if(m_bEnabled)
		{
			m_kEvent.Callback(*this, ENABLE);
		}
		else
		{
			m_kEvent.Callback(*this, DISABLE);
		}
	}
}

bool UIWidget::GetEnable()
{
	return m_bEnabled;
}

void UIWidget::SetVisible(bool bVisible)
{
	if(m_bVisible != bVisible)
	{
		m_bVisible = bVisible;
		if(m_bVisible)
		{
			m_kEvent.Callback(*this, SHOW);
			setVisible(true);
		}
		else
		{
			m_kEvent.Callback(*this, HIDE);
			setVisible(false);
		}
	}
}

bool UIWidget::GetVisible()
{
	return m_bVisible;
}

void UIWidget::SetActive(bool bActive)
{
	if(m_bActive != bActive)
	{
		m_bActive = bActive;
		UILayer::SetActiveWidget(this);
		if(m_bActive)
		{
			m_kEvent.Callback(*this, ACTIVE);
		}
		else
		{
			m_kEvent.Callback(*this, INACTIVE);
		}
	}
}

bool UIWidget::GetActive()
{
	return m_bActive;
}

void UIWidget::SetProcessInput(bool bProcess)
{
	m_bProcessInput = bProcess;
}

bool UIWidget::GetProcessInput()
{
	return m_bProcessInput;
}

void UIWidget::SetClipByRange(bool bClip)
{
	m_bClipByRange = bClip; 
}

bool UIWidget::GetClipByRange()
{
	return m_bClipByRange;
}

bool UIWidget::IsRoot()
{
	return m_pkParent ? true : false;
}

UIWidget* UIWidget::GetParent()
{
	return m_pkParent;
}

VeUInt32 UIWidget::GetChildCount()
{
	return m_kChildren.Size();
}

UIWidget* UIWidget::GetChild(VeUInt32 u32Index)
{
	VE_ASSERT(u32Index < m_kChildren.Size());
	VeUInt32 u32Iter(0);
	UIWidget* pkRes(NULL);
	m_kChildren.BeginIterator();
	while(!m_kChildren.IsEnd())
	{
		if(u32Iter == u32Index)
		{
			pkRes = m_kChildren.GetIterNode()->m_tContent;
			break;
		}

		m_kChildren.Next();
		++u32Iter;
	}
	return pkRes;
}

void UIWidget::AddChild(UIWidget* pkWidget, VeInt32 i32ZOrder)
{
	VE_ASSERT(pkWidget);
	pkWidget->m_pkParent = this;
	m_kChildren.AttachBack(*pkWidget);
	addChild(pkWidget, i32ZOrder);
}

void UIWidget::EnableRange(VeFloat32 f32OffsetX, VeFloat32 f32OffsetY, VeFloat32 f32Width, VeFloat32 f32Height)
{
	if(m_pkRange)
	{
		m_pkRange->x = f32OffsetX;
		m_pkRange->y = f32OffsetY;
		m_pkRange->z = f32Width;
		m_pkRange->w = f32Height;
	}
	else
	{
		m_pkRange = VE_NEW VeVector4D(f32OffsetX, f32OffsetY, f32Width, f32Height);
	}
}

void UIWidget::DisableRange()
{
	VE_SAFE_DELETE(m_pkRange);
}

UIWidget* UIWidget::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
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

void UIWidget::EventRelease()
{

}

bool UIWidget::TestInput(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	CCPoint kPoint = pkTouch->getLocation();
	return IsIn(kPoint.x, kPoint.y);
}

bool UIWidget::IsIn(VeFloat32 x, VeFloat32 y)
{
    CCRect kRect = CCRectApplyAffineTransform(CCRectMake(0, 0, m_obContentSize.width, m_obContentSize.height), nodeToWorldTransform());
	kRect.origin.x -= kRect.size.width * m_kAnchorPoint.x;
	kRect.origin.y -= kRect.size.height * m_kAnchorPoint.y;
	return kRect.containsPoint(ccp(x,y));
}

bool UIWidget::IntersectWith(const cocos2d::CCRect kBounding)
{
	CCRect kBoundingCopy = kBounding;
	if(m_pkParent)
	{
		CCPoint kViewPos = m_pkParent->getPosition();
		kViewPos.x -= m_pkParent->getAnchorPointInPoints().x;
		kViewPos.y -= m_pkParent->getAnchorPointInPoints().y;
		kViewPos.x += getParent()->getPositionX();
		kViewPos.y += getParent()->getPositionY();

		kBoundingCopy.origin.x -= kViewPos.x;
		kBoundingCopy.origin.y -= kViewPos.y;
	}
	CCRect kRect = boundingBox();
	kRect.origin.x -= kRect.size.width * m_kAnchorPoint.x;
	kRect.origin.y -= kRect.size.height * m_kAnchorPoint.y;

	return !(kRect.getMaxX() <= kBoundingCopy.getMinX()
		|| kBoundingCopy.getMaxX() <= kRect.getMinX()
		|| kRect.getMaxY() <= kBoundingCopy.getMinY()
		|| kBoundingCopy.getMaxY() <= kRect.getMinY());
}

void UIWidget::RegisterListener(Delegate& kDelegate)
{
	m_kEvent.AddDelegate(kDelegate);
}

void UIWidget::UnregisterListener(Delegate& kDelegate)
{
	m_kEvent.DelDelegate(kDelegate);
}

void UIWidget::SetTouchAnchorPoint(const VeVector2D& kAnchorPoint)
{
	m_kAnchorPoint = kAnchorPoint;
}

const VeVector2D& UIWidget::GetTouchAnchorPoint()
{
	return m_kAnchorPoint;
}

void UIWidget::visit()
{
	if (!m_bVisible)
	{
		return;
	}
	if(m_bClipByRange)
	{
		glEnable(GL_SCISSOR_TEST);
		CCRect kRect = boundingBox();
		CCEGLView::sharedOpenGLView()->setScissorInPoints(kRect.origin.x, kRect.origin.y, kRect.size.width, kRect.size.height);

		VeVector<UIWidget*> kHideList;
		m_kChildren.BeginIterator();
		while(!m_kChildren.IsEnd())
		{
			UIWidget* pkWidget = m_kChildren.GetIterNode()->m_tContent;
			m_kChildren.Next();
			VE_ASSERT(pkWidget);
			if(pkWidget->isVisible() && (!pkWidget->IntersectWith(kRect)))
			{
				pkWidget->setVisible(false);
				kHideList.PushBack(pkWidget);
			}
		}

		CCNode::visit();

		for(VeVector<UIWidget*>::iterator it = kHideList.Begin(); it != kHideList.End(); ++it)
		{
			(*it)->setVisible(true);
		}

		glDisable(GL_SCISSOR_TEST);
	}
	else
	{
		CCNode::visit();
	}
}

void UIWidget::SetParent(UIWidget* pkParent)
{
	m_pkParent = pkParent;
}

VeRefList<UIWidget*>& UIWidget::GetChildren()
{
	return m_kChildren;
}

void UIWidget::ToTop()
{
	if(m_pkParent)
	{
		Detach();
		m_pkParent->m_kChildren.AttachFront(*this);
	}
}

void UIWidget::ToBottom()
{
	if(m_pkParent)
	{
		Detach();
		m_pkParent->m_kChildren.AttachBack(*this);
	}
}
