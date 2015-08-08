#include "UILayer.h"
#include "UIWidget.h"

USING_NS_CC;

VeUInt32 UILayer::ms_u32MainWidth = 1024;

VeUInt32 UILayer::ms_u32MainHeight = 768;

VeFloat32 UILayer::ms_f32Width = 1024;

VeFloat32 UILayer::ms_f32Height = 768;

VeFloat32 UILayer::ms_f32WidthScale = 1.0f;

VeFloat32 UILayer::ms_f32HeightScale = 1.0f;

VeFloat32 UILayer::ms_f32MainScale = 1.0f;

UIWidget* UILayer::m_pkActiveWidget = NULL;

VeImplementRootRTTI(UIUserData);

UIUserData::~UIUserData()
{

}

VeImplementRTTI(UILayerChangeParams, UIUserData);

UILayerChangeParams::UILayerChangeParams(VeUInt32 u32Active, VeUInt32 u32Deactive)
{
	m_u32ActiveParam = u32Active;
	m_u32DeactiveParam = u32Deactive;
}

UILayer::UILayer(const VeChar8* pcName) : m_bNeedProcessTouch(true), m_bActive(false)
	, m_kName(pcName), m_pkLastProcessWidget(NULL)
{
	
}

UILayer::~UILayer()
{

}

const VeChar8* UILayer::GetName()
{
	return m_kName;
}

void UILayer::SetMainResolution(VeUInt32 u32Width, VeUInt32 u32Height, VeFloat32 f32AspectRatioMax, VeFloat32 f32AspectRatioMin)
{
	ms_u32MainWidth = u32Width;
	ms_u32MainHeight = u32Height;
	VeFloat32 f32MainW = u32Width;
	VeFloat32 f32MainH = u32Height;
	CCSize kScreenSize = g_pkDirector->getVisibleSize();
	ms_f32Width = kScreenSize.width;
	ms_f32Height = kScreenSize.height;
	ms_f32WidthScale = kScreenSize.width / f32MainW;
	ms_f32HeightScale = kScreenSize.height / f32MainH;
	VeFloat32 f32AspectRatioMain = f32MainW / f32MainH;
	VeFloat32 f32AspectRatioScreen = kScreenSize.width / kScreenSize.height;
	if(f32AspectRatioScreen > f32AspectRatioMain)
	{
		ms_f32MainScale = ms_f32HeightScale;
		if(f32AspectRatioScreen > f32AspectRatioMax)
		{
			ms_f32WidthScale = ms_f32Height * f32AspectRatioMax / f32MainW;
		}
	}
	else
	{
		ms_f32MainScale = ms_f32WidthScale;
		if(f32AspectRatioScreen < f32AspectRatioMin)
		{
			ms_f32HeightScale = ms_f32Width / f32AspectRatioMin / f32MainH;
		}
	}
}

bool UILayer::init()
{
	return CCLayer::init();
}

void UILayer::term()
{
	removeFromParent();
	m_kWidgetMap.Clear();
	m_kWidgets.BeginIterator();
	while(!m_kWidgets.IsEnd())
	{
		UIWidget* pkWidget = m_kWidgets.GetIterNode()->m_tContent;
		m_kWidgets.Next();
		VE_ASSERT(pkWidget);
		VE_DELETE(pkWidget);
	}
	m_kWidgets.Clear();
}

void UILayer::onEnter()
{
	CCLayer::onEnter();
	g_pkDirector->getTouchDispatcher()->addTargetedDelegate(this, 1, false);
}

void UILayer::onExit()
{
	g_pkDirector->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

void UILayer::onEnterTransitionDidFinish()
{
	CCLayer::onEnterTransitionDidFinish();
}

bool UILayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	ProcessTouch(TOUCH_BEGAN, pTouch);
	return true;
}

void UILayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	ProcessTouch(TOUCH_MOVED, pTouch);
}

void UILayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	ProcessTouch(TOUCH_ENDED, pTouch);
}

void UILayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	ProcessTouch(TOUCH_CANCELLED, pTouch);
}

void UILayer::EnableTouch()
{
	m_bNeedProcessTouch = true;
}

void UILayer::DisableTouch()
{
	m_bNeedProcessTouch = false;
}

bool UILayer::IsTouchEnabled()
{
	return m_bNeedProcessTouch;
}

bool UILayer::IsLastTouchProcess()
{
	return m_pkLastProcessWidget ? true : false;
}

void UILayer::SetActive(bool bActive)
{
	return SetActive(bActive, NULL);
}

void UILayer::SetActive(bool bActive, const UIUserDataPtr& spData)
{
	if(bActive != m_bActive)
	{
		if(bActive)
		{
			OnActive(spData);
		}
		else
		{
			OnDeactive(spData);
		}
		m_bActive = bActive;
	}
}

bool UILayer::IsActive()
{
	return m_bActive;
}

UIWidget* UILayer::LastTouchWidget()
{
	return m_pkLastProcessWidget;
}

bool UILayer::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(m_bNeedProcessTouch)
	{
		m_kWidgets.BeginIterator();
		while(!m_kWidgets.IsEnd())
		{
			UIWidget* pkWidget = m_kWidgets.GetIterNode()->m_tContent;
			m_kWidgets.Next();
			VE_ASSERT(pkWidget);
			if(pkWidget->TestInput(eType, pkTouch))
			{
				UIWidget* pkRes = pkWidget->ProcessTouch(eType, pkTouch);
				if(pkRes)
				{
					if(pkRes != m_pkLastProcessWidget)
					{
						if(m_pkLastProcessWidget)
						{
							m_pkLastProcessWidget->EventRelease();
						}
						m_pkLastProcessWidget = pkRes;
					}
					return true;
				}
			}
		}

		if(m_pkLastProcessWidget)
		{
			m_pkLastProcessWidget->EventRelease();
			m_pkLastProcessWidget = NULL;
		}
	}
	return false;
}

void UILayer::AddWidget(const VeChar8* pcName, UIWidget* pkWidget, VeInt32 i32ZOrder)
{
	m_kWidgets.AttachBack(*pkWidget);
	m_kWidgetMap[pcName] = pkWidget;
	addChild(pkWidget, i32ZOrder);
}

VeUInt32 UILayer::MainWidth()
{
	return ms_u32MainWidth;
}

VeUInt32 UILayer::MainHeight()
{
	return ms_u32MainHeight;
}

VeFloat32 UILayer::Width()
{
	return ms_f32Width;
}

VeFloat32 UILayer::Height()
{
	return ms_f32Height;
}

VeFloat32 UILayer::WidthScale()
{
	return ms_f32WidthScale;
}

VeFloat32 UILayer::HeightScale()
{
	return ms_f32HeightScale;
}

VeFloat32 UILayer::MainScale()
{
	return ms_f32MainScale;
}

void UILayer::SetActiveWidget(UIWidget* pkWidget)
{
	if(m_pkActiveWidget && m_pkActiveWidget != pkWidget)
	{
		m_pkActiveWidget->SetActive(false);
	}
	m_pkActiveWidget = pkWidget;
}

void UILayer::SetModalTextureName(const VeChar8* pcName)
{
	ms_kModalTexture = pcName;
}

const VeChar8* UILayer::GetModalTextureName()
{
	return ms_kModalTexture.c_str();
}

std::string UILayer::ms_kModalTexture;

void UILayerModal::onEnter()
{
	CCLayer::onEnter();
	g_pkDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool UILayerModal::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(UILayer::ProcessTouch(eType, pkTouch))
	{
		return true;
	}
	return m_bModal;
}

void UILayerModal::SetModal(bool bEnable)
{
	m_bModal = bEnable;
}

bool UILayerModal::IsModal()
{
	return m_bModal;
}
