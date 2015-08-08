#pragma once

#include "UIScrollView.h"
#include "UITableView.h"
#include "Knickknacks.h"
#include "Card.h"

class EventList;
class TexasPokerHallPlayerC;

class OptionView : public UIPageView
{
public:
	enum Page
	{
		PG_INFO,
		PG_SETUP,
		PG_HELP,
		PG_MAX
	};

	typedef VeMemberDelegate<OptionView, UIWidget&, VeUInt32> UIDelegate;
	typedef VeVector< VePair<CardNum,CardSuit> > CardType;

	OptionView();

	virtual ~OptionView();

	void Back();

	bool OnBack();

	void SetName(const VeChar8* pcName);

	void SetTitle(const VeChar8* pcTitle);

	void SetBankDesc(const VeChar8* pkBankDesc);

	void SetChipsGold(VeUInt32 u32Chips, VeUInt32 u32Gold);

	void SetMedal(VeUInt32 u32Gold, VeUInt32 u32Sliver, VeUInt32 u32Copper);

	void SetObject(VeUInt32 u32Diamond, VeUInt32 u32Antique, VeUInt32 u32Car, VeUInt32 u32Plane);

	void SetGameData(VeUInt32 u32WinGames, VeUInt32 u32MaxWinBet, VeFloat32 f32WinRate);

	void SetBestType(const CardType& kType, bool bVip);

	void UpdateProperties(TexasPokerHallPlayerC* pkPlayer);

protected:
	void CreateRoot(UIWidget* pkParent);

	void CreateContent(UIWidget* pkParent);

	void CreateInfo(UIWidget* pkParent);

	void CreateSetup(UIWidget* pkParent);

	void CreateHelp(UIWidget* pkParent);

	UIPageView* m_pkContentView;

	UIDeclareDelegate(OnPlayerInfo);
	UIDeclareDelegate(OnEnvSetup);
	UIDeclareDelegate(OnGameHelp);
	UIDeclareDelegate(OnBugFeedback);
	UIDeclareDelegate(OnBack);
	UIDeclareDelegate(OnEditInfo);
	UIDeclareDelegate(OnMusicVolume);
	UIDeclareDelegate(OnSoundVolume);
	UIDeclareDelegate(OnSilenceCheck);
	UIDeclareDelegate(OnLeftEvent);
	UIDeclareDelegate(OnMidEvent);
	UIDeclareDelegate(OnRightEvent);
	UIDeclareDelegate(OnLowLeftEvent);
	UIDeclareDelegate(OnLowMidEvent);
	UIDeclareDelegate(OnLowRightEvent);

	HeadArea* m_pkHeadArea;

	cocos2d::CCLabelTTF* m_pkName;
	cocos2d::CCLabelTTF* m_pkTitle;
	cocos2d::CCSprite* m_pkVipCard;
	cocos2d::CCLabelTTF* m_pkBankDesc;
	cocos2d::CCLabelTTF* m_pkChips;
	cocos2d::CCLabelTTF* m_pkGold;
	cocos2d::CCLabelTTF* m_apkMedal[3];
	cocos2d::CCLabelTTF* m_apkObject[4];
	cocos2d::CCLabelTTF* m_pkWinGames;
	cocos2d::CCLabelTTF* m_pkMaxWinBet;
	cocos2d::CCLabelTTF* m_pkBestType;
	MiniCard* m_apkCardType[5];
	UIPanel* pkNormalLeft;
	UIPanel* pkPressedLeft;
	UIButton* pkEventLeft;
	UIPanel* pkNormalMid;
	UIPanel* pkPressedMid;
	UIButton* pkEventMid;
	UIPanel* pkNormalRight;
	UIPanel* pkPressedRight;
	UIButton* pkEventRight;
	CCNode* pkHelpNodeL;
	CCNode* pkHelpNodeM;
	CCNode* pkHelpNodeR;
	UIScrollView* pkHelpScrollView;

	UIPanel* pkNormalLowLeft;
	UIPanel* pkPressedLowLeft;
	UIButton* pkEventLowLeft;
	UIPanel* pkNormalLowMid;
	UIPanel* pkPressedLowMid;
	UIButton* pkEventLowMid;
	UIPanel* pkNormalLowRight;
	UIPanel* pkPressedLowRight;
	UIButton* pkEventLowRight;
	CCNode* pkLowHelpNodeL;
	CCNode* pkLowHelpNodeM;
	CCNode* pkLowHelpNodeR;
};
