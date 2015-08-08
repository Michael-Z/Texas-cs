#include "UIScrollView.h"
#include "TexasPoker.h"

USING_NS_CC;

UIScrollView::UIScrollView(VeFloat32 f32Width, VeFloat32 f32Height,
	const VeChar8* pcBean, const VeChar8* pcBeanExt, VeFloat32 f32BarBias)
	: m_ubyOpacity(255), m_bShowScrollBar(false), m_bCanScroll(false)
{
	schedule(schedule_selector(UIScrollView::Update), 0.0f);
	setContentSize(CCSize(f32Width, f32Height));
	SetClipByRange(true);
	m_pkInnerNode = CCNode::create();
	addChild(m_pkInnerNode);

	m_apkScrollBarV[0] = CCSprite::createWithSpriteFrameName(pcBean);
	m_apkScrollBarV[0]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkScrollBarV[0]->setScale(ui_main_scale);
	m_apkScrollBarV[1] = CCSprite::createWithSpriteFrameName(pcBean);
	m_apkScrollBarV[1]->setAnchorPoint(ccp(1.0f, 0.0f));
	m_apkScrollBarV[1]->setScaleX(ui_main_scale);
	m_apkScrollBarV[1]->setScaleY(-ui_main_scale);

	m_apkScrollBarV[2] = CCSprite::createWithSpriteFrameName(pcBeanExt);
	m_apkScrollBarV[2]->setAnchorPoint(ccp(1.0f, 1.0f));
	m_apkScrollBarV[2]->setScaleX(ui_main_scale);
	m_apkScrollBarV[2]->setScaleY(0);

	m_pkScrollBarV = CCNode::create();
	m_pkScrollBarV->addChild(m_apkScrollBarV[0]);
	m_pkScrollBarV->addChild(m_apkScrollBarV[1]);
	m_pkScrollBarV->addChild(m_apkScrollBarV[2]);
	m_pkScrollBarV->setPositionX(f32Width - f32BarBias);
	addChild(m_pkScrollBarV, 6);

	m_f32Velocity = 0;
	m_bOnTouch = false;

	m_apkScrollBarV[0]->setOpacity(0);
	m_apkScrollBarV[1]->setOpacity(0);
	m_apkScrollBarV[2]->setOpacity(0);

	m_pkScrollBarV->setVisible(false);

	m_f32MinX = VE_MAXFLOAT_F;
	m_f32MaxX = -VE_MAXFLOAT_F;
	m_f32MinY = VE_MAXFLOAT_F;
	m_f32MaxY = -VE_MAXFLOAT_F;
	m_f32MoveLeft = 0;
	m_f32MoveRight = 0;
	m_f32MoveBottom = 0;
	m_f32MoveTop = 0;
	m_f32ScrollBarLen = 0;
	m_f32Velocity = 0;
	m_bRollBack = false;
	m_fHideScrollBarPass = 0;
}

UIScrollView::~UIScrollView()
{

}

void UIScrollView::AddChild(UIWidget* pkWidget, VeInt32 i32ZOrder)
{
	VE_ASSERT(pkWidget);
	pkWidget->SetParent(this);
	m_kChildren.AttachBack(*pkWidget);
	m_pkInnerNode->addChild(pkWidget, i32ZOrder);
	UpdateBounding();
}

void UIScrollView::UpdateBounding()
{
	CCRect kRect = boundingBox();
	if(m_kChildren.Size())
	{
		m_f32MinX = VE_MAXFLOAT_F;
		m_f32MaxX = -VE_MAXFLOAT_F;
		m_f32MinY = VE_MAXFLOAT_F;
		m_f32MaxY = -VE_MAXFLOAT_F;
		m_kChildren.BeginIterator();
		while(!m_kChildren.IsEnd())
		{
			UIWidget* pkWidget = m_kChildren.GetIterNode()->m_tContent;
			m_kChildren.Next();
			VE_ASSERT(pkWidget);
			if(pkWidget->GetVisible())
			{
				CCRect kRectWidget = pkWidget->boundingBox();
				kRectWidget.origin.x -= kRectWidget.size.width * pkWidget->GetTouchAnchorPoint().x;
				kRectWidget.origin.y -= kRectWidget.size.height * pkWidget->GetTouchAnchorPoint().y;
				m_f32MinX = VE_MIN(kRectWidget.getMinX(), m_f32MinX);
				m_f32MaxX = VE_MAX(kRectWidget.getMaxX(), m_f32MaxX);
				m_f32MinY = VE_MIN(kRectWidget.getMinY(), m_f32MinY);
				m_f32MaxY = VE_MAX(kRectWidget.getMaxY(), m_f32MaxY);
			}
		}
		m_f32MinY = VE_MIN(m_f32MinY, m_f32MaxY - kRect.size.height);
	}
	else
	{
		m_f32MinX = 0;
		m_f32MaxX = kRect.size.width;
		m_f32MinY = 0;
		m_f32MaxY = kRect.size.height;
	}

	m_f32MoveLeft = m_f32MinX;
	m_f32MoveRight = m_f32MaxX - kRect.size.width;
	m_f32MoveBottom = m_f32MinY;
	m_f32MoveTop = m_f32MaxY - kRect.size.height;

	VeFloat32 f32Rate = m_f32MaxY > m_f32MinY ? kRect.size.height / (m_f32MaxY - m_f32MinY) : 1.0f;
	f32Rate = VE_MIN(f32Rate, 1.0f);
	m_f32ScrollBarLen = VeFloorf(VeFloat32(kRect.size.height) * f32Rate);

	m_bCanScroll = f32Rate < 1.0f;

	UpdateScrollBar();
}

void UIScrollView::UpdateScrollBar()
{
	VeFloat32 f32Len = m_f32ScrollBarLen;
	VeFloat32 f32PosY = -m_pkInnerNode->getPositionY();
	CCRect kRect = m_apkScrollBarV[0]->getTextureRect();
	VeFloat32 f32ScaleLen = VeFloorf(f32Len - kRect.size.height * 2.4f * ui_main_scale);
	f32ScaleLen = VE_MAX(f32ScaleLen, 0);
	if(f32PosY < m_f32MoveBottom)
	{
		f32ScaleLen = VE_MAX(f32ScaleLen - m_f32MoveBottom + f32PosY, 0);
		VeFloat32 f32PosYMin = VeFloorf(kRect.size.height * ui_main_scale * 1.2f) + f32ScaleLen;
		m_pkScrollBarV->setPositionY(f32PosYMin);
	}
	else if(f32PosY > m_f32MoveTop)
	{
		f32ScaleLen = VE_MAX(f32ScaleLen - f32PosY + m_f32MoveTop, 0);
		VeFloat32 f32PosYMax = getContentSize().height - VeFloorf(kRect.size.height * ui_main_scale * 1.2f);
		m_pkScrollBarV->setPositionY(f32PosYMax);
	}
	else
	{
		VeFloat32 f32PosYMin = VeFloorf(kRect.size.height * ui_main_scale * 1.2f) + f32ScaleLen;
		VeFloat32 f32PosYMax = getContentSize().height - VeFloorf(kRect.size.height * ui_main_scale * 1.2f);
		if(f32PosYMin != f32PosYMax)
		{
			VeFloat32 f32PosYRate = (f32PosY - m_f32MoveBottom) / (m_f32MoveTop - m_f32MoveBottom);
			m_pkScrollBarV->setPositionY(VeFloorf(VeLerp(f32PosYRate, f32PosYMin, f32PosYMax)));
		}
		else
		{
			m_pkScrollBarV->setPositionY(VeFloorf(f32PosYMin));
		}
	}
	m_apkScrollBarV[1]->setPositionY(-f32ScaleLen);
	m_apkScrollBarV[2]->setScaleY(f32ScaleLen);
}

void UIScrollView::SetScrollBarColor(const ccColor3B& value)
{
	m_apkScrollBarV[0]->setColor(value);
	m_apkScrollBarV[1]->setColor(value);
	m_apkScrollBarV[2]->setColor(value);
}

void UIScrollView::SetScrollBarOpacity(GLubyte value)
{
	m_ubyOpacity = value;
}

void UIScrollView::Update(VeFloat32 f32Delta)
{
	if(!m_bOnTouch)
	{
		if(m_f32Velocity > 0)
		{
			VeFloat32 f32PosY = m_pkInnerNode->getPositionY();
			VeFloat32 f32Acce = FRICTION_PER_SEC * ui_main_scale;
			if(m_f32MoveBottom > (-f32PosY))
			{
				f32Acce += (m_f32MoveBottom + f32PosY) * DAMPING;
			}
			m_f32Velocity = VE_MAX(m_f32Velocity - f32Acce * f32Delta, 0);
			if(m_f32Velocity > 0)
			{
				m_pkInnerNode->setPositionY(VeFloorf(f32PosY + m_f32Velocity * f32Delta));
			}
			else
			{
				m_bRollBack = true;
				m_f32RollBackTime = ROLLBACK_TIME;
				m_f32RollBackPosY = f32PosY;
			}
			UpdateScrollBar();
		}
		else if(m_f32Velocity < 0)
		{
			VeFloat32 f32PosY = m_pkInnerNode->getPositionY();
			VeFloat32 f32Acce = -FRICTION_PER_SEC * ui_main_scale;
			if((-f32PosY) > m_f32MoveTop)
			{
				f32Acce += (m_f32MoveTop + f32PosY) * DAMPING;
			}
			m_f32Velocity = VE_MIN(m_f32Velocity - f32Acce * f32Delta, 0);
			if(m_f32Velocity < 0)
			{
				m_pkInnerNode->setPositionY(VeFloorf(f32PosY + m_f32Velocity * f32Delta));
			}
			else
			{
				m_bRollBack = true;
				m_f32RollBackTime = ROLLBACK_TIME;
				m_f32RollBackPosY = f32PosY;
			}
			UpdateScrollBar();
		}
		else if(m_bRollBack)
		{
			if(m_f32RollBackTime <= 0)
			{
				m_bRollBack = false;
				m_f32RollBackTime = 0;
			}
			else if(m_f32MoveBottom > (-m_f32RollBackPosY))
			{
				m_f32RollBackTime -= f32Delta;
				if(m_f32RollBackTime > 0)
				{
					VeFloat32 f32Offset = VeCosf((m_f32RollBackTime / ROLLBACK_TIME) * VE_MATH_PI_2_F);
					m_pkInnerNode->setPositionY(VeFloorf(VeLerp(f32Offset, m_f32RollBackPosY, -m_f32MoveBottom)));
				}
				else
				{
					m_f32RollBackTime = 0;
					m_bRollBack = false;
					m_pkInnerNode->setPositionY(VeFloorf(-m_f32MoveBottom));
				}
			}
			else if(((-m_f32RollBackPosY) > m_f32MoveTop))
			{
				m_f32RollBackTime -= f32Delta;
				if(m_f32RollBackTime > 0)
				{
					VeFloat32 f32Offset = VeCosf((m_f32RollBackTime / ROLLBACK_TIME) * VE_MATH_PI_2_F);
					m_pkInnerNode->setPositionY(VeFloorf(VeLerp(f32Offset, m_f32RollBackPosY, -m_f32MoveTop)));
				}
				else
				{
					m_f32RollBackTime = 0;
					m_bRollBack = false;
					m_pkInnerNode->setPositionY(VeFloorf(-m_f32MoveTop));
				}
			}
			else
			{
				m_bRollBack = false;
				m_f32RollBackTime = 0;
			}
			UpdateScrollBar();
		}
		else if(m_bShowScrollBar)
		{
			if(m_fHideScrollBarPass < SCROLLBAR_HIDE)
			{
				m_fHideScrollBarPass = VE_MIN(m_fHideScrollBarPass + f32Delta, SCROLLBAR_HIDE);
				VeFloat32 f32Rate = 1.0f - (m_fHideScrollBarPass / SCROLLBAR_HIDE);
				m_apkScrollBarV[0]->setOpacity(m_ubyOpacity * f32Rate);
				m_apkScrollBarV[1]->setOpacity(m_ubyOpacity * f32Rate);
				m_apkScrollBarV[2]->setOpacity(m_ubyOpacity * f32Rate);
			}
			else
			{
				m_pkScrollBarV->setVisible(false);
				m_bShowScrollBar = false;
			}
		}
	}
}

UIWidget* UIScrollView::ProcessTouch(UILayer::TouchType eType, cocos2d::CCTouch* pkTouch)
{
	static CCPoint s_kBeganPos;
	static bool s_bScroll;
	static VeFloat32 s_f32InnerNodeStart;

	if(m_bCanScroll)
	{
		if(m_bEnabled && m_bVisible)
		{
			switch(eType)
			{
			case UILayer::TOUCH_BEGAN:
				m_pkScrollBarV->setVisible(false);
				m_bShowScrollBar = false;
				m_bOnTouch = true;
				s_bScroll = false;
				m_bRollBack = false;
				s_kBeganPos = pkTouch->getLocation();
				m_f32Velocity = 0.0f;
				{
					UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
					if(pkWidget) return pkWidget;
				}
				break;
			case UILayer::TOUCH_MOVED:
				m_bOnTouch = true;
				if(s_bScroll)
				{
					VeFloat32 f32DeltaY = pkTouch->getLocation().y - pkTouch->getPreviousLocation().y;
					if(((-s_f32InnerNodeStart) < m_f32MoveBottom) || ((-s_f32InnerNodeStart) > m_f32MoveTop))
					{
						f32DeltaY *= 0.3f;
						m_f32Velocity = 0;
					}
					else
					{
						m_f32Velocity = f32DeltaY * EVENT_RATE;
						m_f32Velocity = VE_MIN(m_f32Velocity, VELOCITYMAX);
						m_f32Velocity = VE_MAX(m_f32Velocity, -VELOCITYMAX);
					}
					s_f32InnerNodeStart += f32DeltaY;
					m_pkInnerNode->setPositionY(VeFloorf(s_f32InnerNodeStart));
					UpdateScrollBar();
				}
				else
				{
					if(VeFabsf(pkTouch->getLocation().y - s_kBeganPos.y) > (10.0f * ui_main_scale))
					{
						m_bShowScrollBar = true;
						m_fHideScrollBarPass = 0;
						m_pkScrollBarV->setVisible(true);
						m_apkScrollBarV[0]->setOpacity(m_ubyOpacity);
						m_apkScrollBarV[1]->setOpacity(m_ubyOpacity);
						m_apkScrollBarV[2]->setOpacity(m_ubyOpacity);
						s_bScroll = true;
						s_f32InnerNodeStart = m_pkInnerNode->getPositionY();
					}
					else
					{
						UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
						if(pkWidget) return pkWidget;
					}
				}
				break;
			case UILayer::TOUCH_ENDED:
			case UILayer::TOUCH_CANCELLED:
				{
					UIWidget* pkWidget = UIWidget::ProcessTouch(eType, pkTouch);
					if(pkWidget)
					{
						return pkWidget;
					}
					else
					{
						OnRelease();
					}
				}
				break;
			default:
				break;
			}
			return this;
		}
		else
		{
			m_bOnTouch = false;
			return NULL;
		}
	}
	else
	{
		return UIWidget::ProcessTouch(eType, pkTouch);
	}
}

void UIScrollView::EventRelease()
{
	OnRelease();
}

bool UIScrollView::IsIn(VeFloat32 x, VeFloat32 y)
{
	CCRect kRect = boundingBox();
	return kRect.containsPoint(ccp(x,y));
}

void UIScrollView::OnRelease()
{
	m_bOnTouch = false;
	VeFloat32 f32PosY = m_pkInnerNode->getPositionY();

	if(((-f32PosY) < m_f32MoveBottom) || ((-f32PosY) > m_f32MoveTop))
	{
		m_bRollBack = true;
		m_f32RollBackTime = ROLLBACK_TIME;
		m_f32RollBackPosY = f32PosY;
	}
}

void UIScrollView::ScrollToTop()
{
	m_pkInnerNode->setPositionY(VeFloorf(-m_f32MoveTop));
	UpdateScrollBar();
	m_f32Velocity = 0;
	m_bOnTouch = false;
}

void UIScrollView::ScroolToBottom()
{
	m_pkInnerNode->setPositionY(VeFloorf(-m_f32MoveBottom));
	UpdateScrollBar();
}
