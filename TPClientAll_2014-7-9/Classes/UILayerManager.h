#pragma once

#include "UILayer.h"

class UILayerManager : public cocos2d::CCNode, public VeMemObject
{
public:
	typedef VePair<UILayer*, UIUserDataPtr> Layer;

	UILayerManager();

	virtual ~UILayerManager();

	void ClearLayers();

	void AddLayer(UILayer* pkLayer, bool bSet = false, const UIUserDataPtr& spUserData = NULL, bool bNeedPath = true);

	void DelLayer(const VeChar8* pcName);

	UILayer* GetLayer(const VeChar8* pcName);

	UILayer* GetCurrentMainLayer();

	template<class T>
	T* GetCurrentMainLayer()
	{
		UILayer* pkLayer = GetCurrentMainLayer();
		VE_ASSERT(!VeStrcmp(pkLayer->GetName(), T::GetName()));
		return (T*)pkLayer;
	}

	void SetMainLayer(const VeChar8* pcName, bool bNeedPath = true);

	void SetMainLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData, bool bNeedPath = true);

	void SetMainLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData, bool bNeedPath = true);

	void PushLayer(const VeChar8* pcName);

	void PushLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData);

	void PushLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData);

	void PopLayer();

	void PopAllLayers();

	VeUInt32 GetLayersNum();

	VeUInt32 GetMiddleLayersNum();

	void PushMiddleLayer(const VeChar8* pcName);

	void PushMiddleLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData);

	void PushMiddleLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData);

	void PopMiddleLayer();

	void PopAllMiddleLayers();

	void PushTopLayer(const VeChar8* pcName);

	void PushTopLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData);

	void PushTopLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData);

	void PopTopLayer();

	void PopTopLayers();

	void PopTopLayerAndDeavtive(const UIUserDataPtr& spUserData = NULL);

	bool Back();

	bool Next();

	void Menu();

	void Lock();

	void Unlock();

protected:
	VeStringMap<VeUInt32> m_kLayerMap;
	VeVector<UILayer*> m_kLayerSet;
	Layer m_kCurrentMainLayer;
	VeVector<Layer> m_kPreLayers;
	VeVector<Layer> m_kNextLayers;
	VeVector<Layer> m_kLayerStack;
	VeVector<Layer> m_kMiddleLayerStack;
	VeVector<Layer> m_kTopLayerStack;
	bool m_bLocked;

};
