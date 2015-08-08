#include "UILayerManager.h"

UILayerManager::UILayerManager()
	: m_kCurrentMainLayer(NULL, NULL)
{
	m_bLocked = false;
}

UILayerManager::~UILayerManager()
{
	ClearLayers();
	removeFromParent();
}

void UILayerManager::ClearLayers()
{
	PopAllLayers();
	SetMainLayer(NULL, false);
	m_kLayerMap.Clear();
	for(VeVector<UILayer*>::iterator it = m_kLayerSet.Begin(); it != m_kLayerSet.End(); ++it)
	{
		VE_SAFE_DELETE(*it);
	}
	m_kLayerSet.Clear();
}

void UILayerManager::AddLayer(UILayer* pkLayer, bool bSet, const UIUserDataPtr& spUserData, bool bNeedPath)
{
	if(pkLayer)
	{
		m_kLayerMap[pkLayer->GetName()] = m_kLayerSet.Size();
		m_kLayerSet.PushBack(pkLayer);

		if(bSet)
		{
			SetMainLayer(pkLayer, spUserData, bNeedPath);
		}
	}
}

void UILayerManager::DelLayer(const VeChar8* pcName)
{
	if(pcName)
	{
		VeUInt32* it = m_kLayerMap.Find(pcName);
		if(it)
		{
			VeUInt32 u32Index = *it;
			if(u32Index < (m_kLayerSet.Size() - 1))
			{
				UILayer* pkLayer = m_kLayerSet[u32Index];
				UILayer* pkMove = m_kLayerSet.Back();
				m_kLayerMap[pkMove->GetName()] = u32Index;
				m_kLayerSet[u32Index] = pkMove;
				m_kLayerSet.Back() = pkLayer;
			}
			m_kLayerMap.Remove(pcName);
			VE_SAFE_DELETE(m_kLayerSet.Back());
			m_kLayerSet.PopBack();
		}
	}
}

UILayer* UILayerManager::GetLayer(const VeChar8* pcName)
{
	if(pcName)
	{
		VeUInt32* it = m_kLayerMap.Find(pcName);
		if(it)
		{
			VE_ASSERT((*it) < m_kLayerSet.Size());
			return m_kLayerSet[*it];
		}
	}
	
	return NULL;
}

UILayer* UILayerManager::GetCurrentMainLayer()
{
	return m_kCurrentMainLayer.m_tFirst;
}

void UILayerManager::SetMainLayer(const VeChar8* pcName, bool bNeedPath)
{
	SetMainLayer(pcName, NULL, bNeedPath);
}

void UILayerManager::SetMainLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData, bool bNeedPath)
{
	SetMainLayer(GetLayer(pcName), spUserData, bNeedPath);
}

void UILayerManager::SetMainLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData, bool bNeedPath)
{
	if(GetCurrentMainLayer())
	{
		if(bNeedPath)
		{
			m_kPreLayers.PushBack(m_kCurrentMainLayer);
		}
		removeChild(m_kCurrentMainLayer.m_tFirst, false);
		m_kCurrentMainLayer.m_tFirst->SetActive(false, m_kCurrentMainLayer.m_tSecond);
	}
	m_kCurrentMainLayer.m_tFirst = pkLayer;
	m_kCurrentMainLayer.m_tSecond = spUserData;
	if(pkLayer)
	{
		addChild(pkLayer, 0);
		pkLayer->SetActive(true, spUserData);
	}
	else
	{
		m_kPreLayers.Clear();
		m_kNextLayers.Clear();
	}
}

void UILayerManager::PushLayer(const VeChar8* pcName)
{
	PushLayer(pcName, NULL);
}

void UILayerManager::PushLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData)
{
	PushLayer(GetLayer(pcName), spUserData);
}

void UILayerManager::PushLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData)
{
	if(pkLayer)
	{
		addChild(pkLayer, 2);
		pkLayer->SetActive(true, spUserData);
		m_kLayerStack.PushBack(Layer(pkLayer, spUserData));
	}
}

void UILayerManager::PopLayer()
{
	if(m_kLayerStack.Size())
	{
		Layer& kLayer = m_kLayerStack.Back();
		VE_ASSERT(kLayer.m_tFirst);
		removeChild(kLayer.m_tFirst, false);
		m_kLayerStack.PopBack();
		kLayer.m_tFirst->SetActive(false, NULL);
	}
}

void UILayerManager::PopAllLayers()
{
	while(m_kLayerStack.Size())
	{
		PopLayer();
	}
}

VeUInt32 UILayerManager::GetLayersNum()
{
	return m_kLayerStack.Size();
}

VeUInt32 UILayerManager::GetMiddleLayersNum()
{
	return m_kMiddleLayerStack.Size();
}

void UILayerManager::PushMiddleLayer(const VeChar8* pcName)
{
	PushMiddleLayer(pcName, NULL);
}

void UILayerManager::PushMiddleLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData)
{
	PushMiddleLayer(GetLayer(pcName), spUserData);
}

void UILayerManager::PushMiddleLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData)
{
	if(pkLayer)
	{
		addChild(pkLayer, 1);
		pkLayer->SetActive(true, spUserData);
		m_kMiddleLayerStack.PushBack(Layer(pkLayer, spUserData));
	}
}

void UILayerManager::PopMiddleLayer()
{
	if(m_kMiddleLayerStack.Size())
	{
		Layer& kLayer = m_kMiddleLayerStack.Back();
		VE_ASSERT(kLayer.m_tFirst);
		removeChild(kLayer.m_tFirst, false);
		m_kMiddleLayerStack.PopBack();
	}
}

void UILayerManager::PopAllMiddleLayers()
{
	while(m_kMiddleLayerStack.Size())
	{
		PopMiddleLayer();
	}
}

void UILayerManager::PushTopLayer(const VeChar8* pcName)
{
	PushTopLayer(pcName, NULL);
}

void UILayerManager::PushTopLayer(const VeChar8* pcName, const UIUserDataPtr& spUserData)
{
	PushTopLayer(GetLayer(pcName), spUserData);
}

void UILayerManager::PushTopLayer(UILayer* pkLayer, const UIUserDataPtr& spUserData)
{
	if(pkLayer)
	{
		addChild(pkLayer, 3);
		m_kTopLayerStack.PushBack(Layer(pkLayer, spUserData));
		pkLayer->SetActive(true, spUserData);
	}
}

void UILayerManager::PopTopLayer()
{
	if(m_kTopLayerStack.Size())
	{
		Layer& kLayer = m_kTopLayerStack.Back();
		VE_ASSERT(kLayer.m_tFirst);
		removeChild(kLayer.m_tFirst, false);
		m_kTopLayerStack.PopBack();
	}
}

void UILayerManager::PopTopLayers()
{
	while(m_kTopLayerStack.Size())
	{
		PopTopLayer();
	}
}

void UILayerManager::PopTopLayerAndDeavtive(const UIUserDataPtr& spUserData)
{
	if(m_kTopLayerStack.Size())
	{
		Layer& kLayer = m_kTopLayerStack.Back();
		VE_ASSERT(kLayer.m_tFirst);
		removeChild(kLayer.m_tFirst, false);
		m_kTopLayerStack.PopBack();
		kLayer.m_tFirst->SetActive(false, spUserData);
	}
}

bool UILayerManager::Back()
{
	if(m_bLocked) return true;
	if(m_kLayerStack.Size())
	{
		Layer& kLayer = m_kLayerStack.Back();
		VE_ASSERT(kLayer.m_tFirst);
		if(kLayer.m_tFirst->OnBack())
		{
			removeChild(kLayer.m_tFirst, false);
			m_kLayerStack.PopBack();
			kLayer.m_tFirst->SetActive(false, kLayer.m_tSecond);
			
		}
		return true;
	}
	else if(m_kPreLayers.Size() && GetCurrentMainLayer())
	{
		Layer& kLayer = m_kPreLayers.Back();
		VE_ASSERT(kLayer.m_tFirst);
		if(m_kCurrentMainLayer.m_tFirst->OnBack())
		{
			m_kNextLayers.PushBack(m_kCurrentMainLayer);
			SetMainLayer(kLayer.m_tFirst, kLayer.m_tSecond, false);
			m_kPreLayers.PopBack();
		}
		return true;
	}
	else if(GetCurrentMainLayer())
	{
		if(m_kCurrentMainLayer.m_tFirst->OnBack())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool UILayerManager::Next()
{
	if(m_bLocked) return true;
	if(m_kNextLayers.Size())
	{
		Layer& kLayer = m_kNextLayers.Back();
		VE_ASSERT(kLayer.m_tFirst);
		if(m_kCurrentMainLayer.m_tFirst->OnNext())
		{
			SetMainLayer(kLayer.m_tFirst, kLayer.m_tSecond, true);
			m_kNextLayers.PopBack();
			
		}
		return true;
	}
	return false;
}

void UILayerManager::Menu()
{
	if(GetCurrentMainLayer())
	{
		m_kCurrentMainLayer.m_tFirst->OnMenu();
	}
}

void UILayerManager::Lock()
{
	m_bLocked = true;
}

void UILayerManager::Unlock()
{
	m_bLocked = false;
}
