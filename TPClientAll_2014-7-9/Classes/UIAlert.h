#pragma once

#include "UIWidget.h"

class UIPanel : public cocos2d::CCNode
{
public:
	virtual ~UIPanel();

	void SetSize(VeFloat32 f32Width, VeFloat32 f32Height);

	static UIPanel* createWithCltBlBt(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height);

	static UIPanel* createWithCltClbBtBbBl(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height);

	static UIPanel* createWithCltClbCrtCrbBtBbBlBr(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height);

	void Show(VeFloat32 f32FadeIn, VeFloat32 f32Delay, VeFloat32 f32FadeOut);

	void PanelFadeIn(VeFloat32 f32FadeIn, VeFloat32 f32Delay);

	void setOpacity(GLubyte value);

	void SetMiddleVisible(bool bVisible);

protected:
	UIPanel();

	void InitWithCltBtBl(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height);

	void InitWithCltClbBtBbBl(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height);

	void InitWithCltClbCrtCrbBtBbBlBr(const VeChar8* pcFileName, VeFloat32 f32Width, VeFloat32 f32Height);

	cocos2d::CCSprite* m_apkCorner[4];
	cocos2d::CCSprite* m_apkBar[4];
	cocos2d::CCSprite* m_pkFill;
	cocos2d::CCSize m_kMinSize;
	cocos2d::CCSize m_kSize;
	cocos2d::CCSpriteBatchNode* m_pkRoot;

};

class UIBarH : public cocos2d::CCNode
{
public:
	virtual ~UIBarH();

	void SetLength(VeFloat32 f32Length);

	static UIBarH* createWithEl(const VeChar8* pcFileName, VeFloat32 f32Length);

	static UIBarH* createWithElEr(const VeChar8* pcFileName, VeFloat32 f32Length);

protected:
	UIBarH();

	void InitWithEl(const VeChar8* pcFileName, VeFloat32 f32Length);

	void InitWithElEr(const VeChar8* pcFileName, VeFloat32 f32Length);

	cocos2d::CCSprite* m_apkEndpoint[2];
	cocos2d::CCSprite* m_pkFill;
	cocos2d::CCSpriteBatchNode* m_pkRoot;
	VeFloat32 m_f32MinLength;
	VeFloat32 m_f32Length;

};



class UIAlert : public UILayerModalExt<UIAlert>
{
public:
	class Callback : public UIUserData
	{
		VeDeclareRTTI;
	public:
		virtual void OnConfirm() {};

		virtual void OnCancel() {};

		VeStringA m_kTitle;
		VeStringA m_kContent;
		VeStringA m_kEnter;
		VeStringA m_kCancel;

	};

	typedef VePointer<Callback> CallbackPtr;

	UIAlert();

	virtual ~UIAlert();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	VE_CREATE_FUNC(UIAlert);

protected:
	void SetText(const VeChar8* pcTitle, const VeChar8* pcContent, const VeChar8* pcEnter, const VeChar8* pcCancel);

	UIDeclareDelegate(OnEnter);
	UIDeclareDelegate(OnCancel);

	UIPanel* m_pkPanel;
	UIBarH* m_pkTitle;
	cocos2d::CCLabelTTF* m_pkTitleText;
	cocos2d::CCLabelTTF* m_pkContentText;
	cocos2d::CCLabelTTF* m_pkEnterText;
	cocos2d::CCLabelTTF* m_pkCancelText;
	CallbackPtr m_spCallback;

};

class UIAlertConfirm : public UILayerModalExt<UIAlertConfirm>
{
public:
	class Callback : public UIUserData
	{
		VeDeclareRTTI;
	public:
		virtual void OnConfirm() {};

		VeStringA m_kTitle;
		VeStringA m_kContent;
		VeStringA m_kEnter;
	};

	typedef VePointer<Callback> CallbackPtr;

	UIAlertConfirm();

	virtual ~UIAlertConfirm();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(UIAlertConfirm);

protected:
	void SetText(const VeChar8* pcTitle, const VeChar8* pcContent, const VeChar8* pcEnter);

	UIDeclareDelegate(OnEnter);

	UIPanel* m_pkPanel;
	UIBarH* m_pkTitle;
	cocos2d::CCLabelTTF* m_pkTitleText;
	cocos2d::CCLabelTTF* m_pkContentText;
	cocos2d::CCLabelTTF* m_pkEnterText;
	CallbackPtr m_spCallback;
};

class UILoading : public UILayerModalExt<UILoading>
{
public:
	class Data : public UIUserData
	{
		VeDeclareRTTI;
	public:
		VeStringA m_kContent;

	};

	UILoading();

	virtual ~UILoading();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(UILoading);

protected:
	void SetText(const VeChar8* pcContent);

	UIPanel* m_pkPanel;
	cocos2d::CCSprite* m_pkLoading;
	cocos2d::CCLabelTTF* m_pkContentText;

};

class UINoticeBoard : public UILayerModalExt<UINoticeBoard>
{
public:
	class Callback : public UIUserData
	{
		VeDeclareRTTI;
	public:
		virtual void OnClose() {};

		VeStringA m_kTitle;
		VeStringA m_kCancel;
		VeStringA m_kContent;		

	};

	typedef VePointer<Callback> CallbackPtr;

	UINoticeBoard();

	virtual ~UINoticeBoard();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	VE_CREATE_FUNC(UINoticeBoard);

protected:
	void SetText(const VeChar8* pcTitle, const VeChar8* pcContent, const VeChar8* pcCancel);

	UIDeclareDelegate(OnCancel);

	UIPanel* m_pkPanel;
	UIPanel* m_pkPanelInner;
	UIBarH* m_pkTitle;
	cocos2d::CCLabelTTF* m_pkTitleText;
	cocos2d::CCLabelTTF* m_pkCancelText;
	cocos2d::CCLabelTTF* m_pkContentText;
	CallbackPtr m_spCallback;

};

class UIToast : public UILayerExt<UIToast>
{
public:
	class Callback : public UIUserData
	{
		VeDeclareRTTI;
	public:
		virtual void OnClose() {}

		VeStringA m_kContent;		

	};

	typedef VePointer<Callback> CallbackPtr;

	UIToast();

	virtual ~UIToast();

	virtual bool init();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	VE_CREATE_FUNC(UIToast);

	void SetText(const VeChar8* pcContent);

protected:
	void Update(VeFloat32 f32Delta);

	void UpdateBlend();

	UIPanel* m_pkPanel;
	cocos2d::CCLabelTTF* m_pkContentText;
	CallbackPtr m_spCallback;
	VeFloat32 m_f32Blend;
	VeFloat32 m_f32ShowTime;

};
