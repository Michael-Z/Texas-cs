#pragma once

#include "UILayer.h"
#include "TexasPokerHallClient.h"

class RoomChoose;
class EventList;
class OptionView;
class RankList;
class UIBarH;

class Hall : public UILayerExt<Hall>
{
public:
	typedef VeMemberDelegate<Hall> FadeDelegate;

	Hall();

	virtual ~Hall();

	virtual bool init();

	virtual void term();

	virtual void OnActive(const UIUserDataPtr& spData);

	virtual void OnDeactive(const UIUserDataPtr& spData);

	virtual bool OnBack();

	virtual void OnMenu();

	VE_CREATE_FUNC(Hall);

	void UpdateRoomList(const VeVector<RoomInfo>& tRoomList);

	void UpdateRoomEnable();

	void UpdateProperties();

	void UpdateOnline(VeUInt32 u32Number);

	void OnForceUpdate();

	void TurnGame();

	void OnLeaderboardUpdate(const VeVector< VePair<VeStringA,VeStringA> >& kLeaderboard);

	void SetEventNumber(VeUInt32 u32Number);

	virtual void OnPushNotice(const VeChar8* pcNotice);

protected:
	void Tick(VeFloat32 f32Delta);

	void UpdateLeaderboard(VeFloat32 f32Delta);

	void OnFadeOut();
	FadeDelegate m_kOnFadeOut;

	void UpdateForLoading(VeFloat32 f32Delta);

	void AddRoom();

	void OnFadeOutMini();
	FadeDelegate m_kOnFadeOutMini;

	void UpdateForLoadingMini(VeFloat32 f32Delta);

	void TurnMiniGame();

	void TurnLoadingMiniGame(VeUInt32 u32Index);

	UIDeclareDelegate(OnPageChange);
	UIDeclareDelegate(OnRoomClicked);
	UIDeclareDelegate(OnGetChipsClicked);
	UIDeclareDelegate(OnBankClicked);

	UIDeclareDelegate(OnMini0);
	UIDeclareDelegate(OnMini1);
	UIDeclareDelegate(OnMini2);

	UIDeclareDelegate(OnEventAction);

	cocos2d::CCSprite* m_pkLight;
	cocos2d::CCPoint m_akLightPos[3];
	RoomChoose* m_pkRoomChoose;
	EventList* m_pkEventList;
	OptionView* m_pkOptionView;
	bool m_bIsGameCreate;
	bool m_bIsMiniGameCreate;
	VeUInt32 m_u32MiniGameNeeded;
	VeVector<RoomInfo> m_kRoomList;
	RankList* m_pkRankList;
	UIBarH* m_pkNotify;
	cocos2d::CCLabelTTF* m_pkEventNumber;

};
