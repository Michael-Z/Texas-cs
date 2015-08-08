#pragma once

#include <cocos2d.h>
#include "Venus3D.h"

#define g_pkDirector (CCDirector::sharedDirector())

#define VE_CREATE_FUNC(__TYPE__) \
	static __TYPE__* create() \
	{ \
		__TYPE__ *pRet = VE_NEW __TYPE__(); \
		if (pRet && pRet->init()) \
		{ \
			return pRet; \
		} \
		else \
		{ \
			VE_SAFE_DELETE(pRet); \
			return NULL; \
		} \
	}

class UIWidget;

class UIUserData : public VeSharedRefObject
{
	VeDeclareRootRTTI(UIUserData);
public:
	virtual ~UIUserData();

};

VeSmartPointer(UIUserData);

class UILayerChangeParams : public UIUserData
{
	VeDeclareRTTI;
public:
	UILayerChangeParams(VeUInt32 u32Active, VeUInt32 u32Deactive);

	VeUInt32 m_u32ActiveParam;
	VeUInt32 m_u32DeactiveParam;

};

#define WIDGET(class, name) class& name = GetWidget<class>(#name)

class UILayer : public cocos2d::CCLayer, public VeMemObject
{
public:
	enum TouchType
	{
		TOUCH_BEGAN,
		TOUCH_MOVED,
		TOUCH_ENDED,
		TOUCH_CANCELLED
	};

	UILayer(const VeChar8* pcName);

	virtual ~UILayer();

	const VeChar8* GetName();

	static void SetMainResolution(VeUInt32 u32Width, VeUInt32 u32Height, VeFloat32 f32AspectRatioMax, VeFloat32 f32AspectRatioMin);

	virtual bool init();

	virtual void term();

	virtual void onEnter();

	virtual void onExit();

	virtual void onEnterTransitionDidFinish();

	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	virtual void OnForceUpdate() {}

	virtual void OnPushNotice(const VeChar8* pcNotice) {};

	void EnableTouch();

	void DisableTouch();

	bool IsTouchEnabled();

	bool IsLastTouchProcess();

	void SetActive(bool bActive);

	void SetActive(bool bActive, const UIUserDataPtr& spData);

	bool IsActive();

	virtual void OnActive(const UIUserDataPtr& spData) {};

	virtual void OnDeactive(const UIUserDataPtr& spData) {};

	virtual bool OnBack() { return true; }

	virtual bool OnNext() { return true; }

	virtual void OnMenu() {}

	UIWidget* LastTouchWidget();

	void AddWidget(const VeChar8* pcName, UIWidget* pkWidget, VeInt32 i32ZOrder = 0);

	static VeUInt32 MainWidth();

	static VeUInt32 MainHeight();

	static VeFloat32 Width();

	static VeFloat32 Height();

	static VeFloat32 WidthScale();

	static VeFloat32 HeightScale();

	static VeFloat32 MainScale();

	static void SetActiveWidget(UIWidget* pkWidget);

	template <class T>
	T& GetWidget(const VeChar8* pcName)
	{
		UIWidget** it = m_kWidgetMap.Find(pcName);
		VE_ASSERT(it && (*it));
		return *(T*)(*it);
	}

	static void SetModalTextureName(const VeChar8* pcName);

	static const VeChar8* GetModalTextureName();

protected:
	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	VeRefList<UIWidget*> m_kWidgets;
	VeStringMap<UIWidget*> m_kWidgetMap;
	bool m_bNeedProcessTouch;
	bool m_bActive;

	VeFixedStringA m_kName;
	UIWidget* m_pkLastProcessWidget;

	static VeUInt32 ms_u32MainWidth;
	static VeUInt32 ms_u32MainHeight;
	static VeFloat32 ms_f32Width;
	static VeFloat32 ms_f32Height;
	static VeFloat32 ms_f32WidthScale;
	static VeFloat32 ms_f32HeightScale;
	static VeFloat32 ms_f32MainScale;

	static UIWidget* m_pkActiveWidget;
	static std::string ms_kModalTexture;

};

template <class T>
class UILayerExt : public UILayer
{
public:
    typedef VeMemberDelegate<T, UIWidget&, VeUInt32> UIDelegate;

	UILayerExt(const VeChar8* pcName) : UILayer(pcName) {}
    
};

class UILayerModal : public UILayer
{
public:
	UILayerModal(const VeChar8* pcName) : UILayer(pcName) {}

	virtual void onEnter();

	virtual bool ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch);

	void SetModal(bool bEnable);

	bool IsModal();

protected:
	bool m_bModal;

};

template <class T>
class UILayerModalExt : public UILayerModal
{
public:
    typedef VeMemberDelegate<T, UIWidget&, VeUInt32> UIDelegate;

    UILayerModalExt(const VeChar8* pcName) : UILayerModal(pcName) {}

};

#define ui_width_scale UILayer::WidthScale()
#define ui_height_scale UILayer::HeightScale()
#define ui_main_scale UILayer::MainScale()
#define ui_w(w) VeFloorf(UILayer::Width()*w)
#define ui_h(h) VeFloorf(UILayer::Height()*h)
#define ui_w_ex(w,x) VeFloorf(UILayer::Width()*w+x*ui_width_scale)
#define ui_h_ex(h,y) VeFloorf(UILayer::Height()*h+y*ui_height_scale)
#define ui_ccp(w,h) ccp(ui_w(w),ui_h(h))
#define ui_ccp_ex(w,x,h,y) ccp( ui_w_ex(w,x),ui_h_ex(h,y) )

#define UIDeclareDelegate(name) \
	void name(UIWidget&,VeUInt32); \
	UIDelegate m_k##name

#define UIInitDelegate(class,name) \
	m_k##name.Set(this,&class::name)

#define UIImplementDelegate(class,name,widget,state) \
	void class::name(UIWidget& widget, VeUInt32 state)

#define UILinkDelegate(widget,name) \
	widget->RegisterListener(m_k##name)
