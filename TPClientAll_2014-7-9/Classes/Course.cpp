#include "Course.h"
#include "TexasPoker.h"
#include "UIButton.h"
#include "UIAlert.h"
#include "SoundSystem.h"
#include "Hall.h"
#include "Knickknacks.h"
#include "Keno.h"

USING_NS_CC;

Course::Course() : UILayerExt<Course>("course")
{
	m_kOnFadeOut.Set(this, &Course::OnFadeOut);
	UIInitDelegate(Course, OnUnderstand);
}

Course::~Course()
{
	term();
}

bool Course::init()
{
	CCSpriteBatchNode* pkRoot = CCSpriteBatchNode::create(g_pkGame->GetImagePath("ui_1", false) + ".png");
	addChild(pkRoot);
	for(VeUInt32 j(0); j < 4; ++j)
	{
		CCSprite* pkBackground = CCSprite::createWithSpriteFrameName("bg_1.png");
		pkBackground->setAnchorPoint(ccp(1.0f, 0.0f));
		pkBackground->setPosition(ui_ccp(0.5f, 0.5f));
		pkBackground->setBlendFunc(kCCBlendFuncDisable);
		switch(j)
		{
		case 1:
			pkBackground->setScaleX(-ui_main_scale);
			pkBackground->setScaleY(ui_main_scale);
			break;
		case 2:
			pkBackground->setScaleX(ui_main_scale);
			pkBackground->setScaleY(-ui_main_scale);
			break;
		case 3:
			pkBackground->setScaleX(-ui_main_scale);
			pkBackground->setScaleY(-ui_main_scale);
			break;
		default:
			pkBackground->setScale(ui_main_scale);
			break;
		}		
		pkRoot->addChild(pkBackground, 0);
	}

	CCNode* pkCourseNode = CCNode::create();
	pkCourseNode->setPosition(ui_ccp(0.5f,0.5f));
	addChild(pkCourseNode);

	CCSprite* pkPlayCourse = CCSprite::createWithSpriteFrameName("help_game.png");
	pkPlayCourse->setScale(ui_main_scale);
	pkCourseNode->addChild(pkPlayCourse);

	pkUnderstand = VE_NEW UIButton("confirm_n.png", "confirm_p.png");
	pkUnderstand->setScale(ui_main_scale);
	pkUnderstand->setAnchorPoint(ccp(0.0f, 1.0f));
	pkUnderstand->setVisible(false);
	pkUnderstand->SetEnable(false);
	pkUnderstand->setPosition(ccp(ui_w(0.5f), VeFloorf(UILayer::Height() * 0.5f - 156.0f * g_f32ScaleHD * ui_main_scale)));
	CCSize m_size;
	m_size = pkUnderstand->getContentSize();
	m_size.width *= 1.2f;
	m_size.height *= 1.2f;
	pkUnderstand->setContentSize(m_size);
	AddWidget("understand", pkUnderstand);
	UILinkDelegate(pkUnderstand, OnUnderstand);

	if(g_pkGame->GetLanguage() == TexasPoker::zh_CN || g_pkGame->GetLanguage() == TexasPoker::zh_TW)
	{
		CCSize kSize;
		pkTest1 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest1->getContentSize();
		pkTest1->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkTest1->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest1->setPositionY(-110.0f * g_f32ScaleHD * ui_main_scale);
		pkTest1->setVisible(false);
		pkCourseNode->addChild(pkTest1);
		pkPrompt1 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt1->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt1->setScaleX(ui_main_scale);
		pkPrompt1->setScaleY(-ui_main_scale);
		pkPrompt1->setPositionY(-117.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt1->setVisible(false);
		pkCourseNode->addChild(pkPrompt1);
		m_pkCourseText1 = CCLabelTTF::create(TT("CourseText1"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText1->setPositionY(-126.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText1->setPositionX(-16.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText1->setColor(ccc3(0,0,0));
		m_pkCourseText1->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText1);
		kSize = m_pkCourseText1->getContentSize();
		m_pkCourseText1->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest2 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest2->getContentSize();
		pkTest2->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkTest2->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest2->setPositionY(28.0f * g_f32ScaleHD * ui_main_scale);
		pkTest2->setVisible(false);
		pkCourseNode->addChild(pkTest2);
		pkPrompt2 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt2->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt2->setScaleX(-ui_main_scale);
		pkPrompt2->setScaleY(-ui_main_scale);
		pkPrompt2->setPositionX(3.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt2->setPositionY(22.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt2->setVisible(false);
		pkCourseNode->addChild(pkPrompt2);
		m_pkCourseText2 = CCLabelTTF::create(TT("CourseText2"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText2->setPositionY(12.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText2->setPositionX(-3.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText2->setColor(ccc3(0,0,0));
		m_pkCourseText2->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText2);
		kSize = m_pkCourseText2->getContentSize();
		m_pkCourseText2->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest3 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest3->getContentSize();
		pkTest3->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * (-0.5f)) / kSize.height));
		pkTest3->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest3->setPositionY(91.0f * g_f32ScaleHD * ui_main_scale);
		pkTest3->setVisible(false);
		pkCourseNode->addChild(pkTest3);
		pkPrompt3 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt3->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt3->setScale(ui_main_scale);
		pkPrompt3->setPositionX(-4.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt3->setPositionY(97.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt3->setVisible(false);
		pkCourseNode->addChild(pkPrompt3);
		m_pkCourseText3 = CCLabelTTF::create(TT("CourseText3"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText3->setPositionY(136.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText3->setPositionX(4.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText3->setColor(ccc3(0,0,0));
		m_pkCourseText3->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText3);
		kSize = m_pkCourseText3->getContentSize();
		m_pkCourseText3->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest4 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest4->getContentSize();
		pkTest4->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkTest4->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest4->setPositionY(-115.0f * g_f32ScaleHD * ui_main_scale);
		pkTest4->setVisible(false);
		pkCourseNode->addChild(pkTest4);
		pkPrompt4 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt4->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt4->setScaleX(ui_main_scale);
		pkPrompt4->setScaleY(-ui_main_scale);
		pkPrompt4->setPositionX(-70.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt4->setPositionY(-122.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt4->setVisible(false);
		pkCourseNode->addChild(pkPrompt4);
		m_pkCourseText4 = CCLabelTTF::create(TT("CourseText4"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText4->setPositionY(-131.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText4->setPositionX(1.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText4->setColor(ccc3(0,0,0));
		m_pkCourseText4->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText4);
		kSize = m_pkCourseText4->getContentSize();
		m_pkCourseText4->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest5 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest5->getContentSize();
		pkTest5->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * (-0.5f)) / kSize.height));
		pkTest5->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest5->setPositionY(-9.0f * g_f32ScaleHD * ui_main_scale);
		pkTest5->setVisible(false);
		pkCourseNode->addChild(pkTest5);
		pkPrompt5 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt5->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt5->setScale(ui_main_scale);
		pkPrompt5->setPositionX(-60.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt5->setPositionY(-2.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt5->setVisible(false);
		pkCourseNode->addChild(pkPrompt5);
		m_pkCourseText5 = CCLabelTTF::create(TT("CourseText5"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText5->setPositionY(37.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText5->setPositionX(-1.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText5->setColor(ccc3(0,0,0));
		m_pkCourseText5->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText5);
		kSize = m_pkCourseText5->getContentSize();
		m_pkCourseText5->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));
		pkDrinkLogo = CCSprite::createWithSpriteFrameName("drink_s_p.png");
		pkDrinkLogo->setScale(ui_main_scale);
		if (g_pkGame->GetLanguage() == TexasPoker::zh_CN)
		{
			pkDrinkLogo->setPositionX(-41.0f * g_f32ScaleHD * ui_main_scale);
		}
		else if (g_pkGame->GetLanguage() == TexasPoker::zh_TW)
		{
			pkDrinkLogo->setPositionX(-44.0f * g_f32ScaleHD * ui_main_scale);
		}
		pkDrinkLogo->setPositionY(12.0f * g_f32ScaleHD * ui_main_scale);
		pkDrinkLogo->setVisible(false);
		pkCourseNode->addChild(pkDrinkLogo);

		pkTest6 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest6->getContentSize();
		pkTest6->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * (0.5f)) / kSize.height));
		pkTest6->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest6->setPositionY(116.0f * g_f32ScaleHD * ui_main_scale);
		pkTest6->setVisible(false);
		pkCourseNode->addChild(pkTest6);
		pkPrompt6 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt6->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt6->setScaleX(ui_main_scale);
		pkPrompt6->setScaleY(-ui_main_scale);
		pkPrompt6->setPositionX(-68.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt6->setPositionY(108.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt6->setVisible(false);
		pkCourseNode->addChild(pkPrompt6);
		m_pkCourseText6 = CCLabelTTF::create(TT("CourseText6"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText6->setPositionY(102.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText6->setPositionX(-31.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText6->setColor(ccc3(0,0,0));
		m_pkCourseText6->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText6);
		kSize = m_pkCourseText6->getContentSize();
		m_pkCourseText6->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));
	}
	else if(g_pkGame->GetLanguage() == TexasPoker::en_US)
	{
		CCSize kSize;
		pkTest1 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest1->getContentSize();
		pkTest1->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkTest1->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest1->setPositionY(-110.0f * g_f32ScaleHD * ui_main_scale);
		pkTest1->setVisible(false);
		pkCourseNode->addChild(pkTest1);
		pkPrompt1 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt1->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt1->setScaleX(ui_main_scale);
		pkPrompt1->setScaleY(-ui_main_scale);
		pkPrompt1->setPositionY(-117.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt1->setVisible(false);
		pkCourseNode->addChild(pkPrompt1);
		m_pkCourseText1 = CCLabelTTF::create(TT("CourseText1"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText1->setPositionY(-118.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText1->setPositionX(-16.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText1->setColor(ccc3(0,0,0));
		m_pkCourseText1->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText1);
		kSize = m_pkCourseText1->getContentSize();
		m_pkCourseText1->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest2 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest2->getContentSize();
		pkTest2->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkTest2->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest2->setPositionY(28.0f * g_f32ScaleHD * ui_main_scale);
		pkTest2->setVisible(false);
		pkCourseNode->addChild(pkTest2);
		pkPrompt2 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt2->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt2->setScaleX(-ui_main_scale);
		pkPrompt2->setScaleY(-ui_main_scale);
		pkPrompt2->setPositionX(3.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt2->setPositionY(22.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt2->setVisible(false);
		pkCourseNode->addChild(pkPrompt2);
		m_pkCourseText2 = CCLabelTTF::create(TT("CourseText2"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText2->setPositionY(20.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText2->setPositionX(-3.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText2->setColor(ccc3(0,0,0));
		m_pkCourseText2->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText2);
		kSize = m_pkCourseText2->getContentSize();
		m_pkCourseText2->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest3 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest3->getContentSize();
		pkTest3->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * (-0.5f)) / kSize.height));
		pkTest3->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest3->setPositionY(91.0f * g_f32ScaleHD * ui_main_scale);
		pkTest3->setVisible(false);
		pkCourseNode->addChild(pkTest3);
		pkPrompt3 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt3->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt3->setScale(ui_main_scale);
		pkPrompt3->setPositionX(-4.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt3->setPositionY(97.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt3->setVisible(false);
		pkCourseNode->addChild(pkPrompt3);
		m_pkCourseText3 = CCLabelTTF::create(TT("CourseText3"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText3->setPositionY(126.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText3->setPositionX(4.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText3->setColor(ccc3(0,0,0));
		m_pkCourseText3->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText3);
		kSize = m_pkCourseText3->getContentSize();
		m_pkCourseText3->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest4 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 50.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest4->getContentSize();
		pkTest4->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * 0.5f) / kSize.height));
		pkTest4->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest4->setPositionY(-115.0f * g_f32ScaleHD * ui_main_scale);
		pkTest4->setVisible(false);
		pkCourseNode->addChild(pkTest4);
		pkPrompt4 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt4->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt4->setScaleX(ui_main_scale);
		pkPrompt4->setScaleY(-ui_main_scale);
		pkPrompt4->setPositionX(-70.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt4->setPositionY(-122.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt4->setVisible(false);
		pkCourseNode->addChild(pkPrompt4);
		m_pkCourseText4 = CCLabelTTF::create(TT("CourseText4"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText4->setPositionY(-131.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText4->setPositionX(1.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText4->setColor(ccc3(0,0,0));
		m_pkCourseText4->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText4);
		kSize = m_pkCourseText4->getContentSize();
		m_pkCourseText4->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest5 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest5->getContentSize();
		pkTest5->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * (-0.5f)) / kSize.height));
		pkTest5->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest5->setPositionY(-9.0f * g_f32ScaleHD * ui_main_scale);
		pkTest5->setVisible(false);
		pkCourseNode->addChild(pkTest5);
		pkPrompt5 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt5->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt5->setScale(ui_main_scale);
		pkPrompt5->setPositionX(-60.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt5->setPositionY(-2.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt5->setVisible(false);
		pkCourseNode->addChild(pkPrompt5);
		m_pkCourseText5 = CCLabelTTF::create(TT("CourseText5"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText5->setPositionY(39.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText5->setPositionX(-7.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText5->setColor(ccc3(0,0,0));
		m_pkCourseText5->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText5);
		kSize = m_pkCourseText5->getContentSize();
		m_pkCourseText5->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));

		pkTest6 = UIPanel::createWithCltClbBtBbBl("alert_prompt", 225.0f * ui_main_scale * g_f32ScaleHD, 60.0f * ui_main_scale * g_f32ScaleHD);
		kSize = pkTest6->getContentSize();
		pkTest6->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.1f) / kSize.width, VeFloorf(kSize.height * (0.5f)) / kSize.height));
		pkTest6->setPositionX(23.0f * g_f32ScaleHD * ui_main_scale);
		pkTest6->setPositionY(116.0f * g_f32ScaleHD * ui_main_scale);
		pkTest6->setVisible(false);
		pkCourseNode->addChild(pkTest6);
		pkPrompt6 = CCSprite::createWithSpriteFrameName("alert_prompt_arrow.png");
		pkPrompt6->setAnchorPoint(ccp(0.0f, 1.0f));
		pkPrompt6->setScaleX(ui_main_scale);
		pkPrompt6->setScaleY(-ui_main_scale);
		pkPrompt6->setPositionX(-68.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt6->setPositionY(108.0f * g_f32ScaleHD * ui_main_scale);
		pkPrompt6->setVisible(false);
		pkCourseNode->addChild(pkPrompt6);
		m_pkCourseText6 = CCLabelTTF::create(TT("CourseText6"), TTF_NAME, VeFloorf(11.0f * g_f32ScaleHD * ui_main_scale));
		m_pkCourseText6->setPositionY(103.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText6->setPositionX(1.0f * g_f32ScaleHD * ui_main_scale);
		m_pkCourseText6->setColor(ccc3(0,0,0));
		m_pkCourseText6->setVisible(false);
		pkCourseNode->addChild(m_pkCourseText6);
		kSize = m_pkCourseText6->getContentSize();
		m_pkCourseText6->setAnchorPoint(ccp(VeFloorf(kSize.width * 0.5f) / kSize.width, VeFloorf(kSize.height * 1.0f) / kSize.height));
	}

	return UILayer::init();
}

void Course::term()
{
	UILayer::term();
}

void Course::OnActive(const UIUserDataPtr& spData)
{
	schedule(schedule_selector(Course::CourseUpDate), 0.6f);
}

void Course::OnDeactive(const UIUserDataPtr& spData)
{

}

bool Course::OnBack()
{
	return false;
}

bool Course::ProcessTouch(TouchType eType, cocos2d::CCTouch* pkTouch)
{
	if(!UILayer::ProcessTouch(eType, pkTouch))
	{
		if(eType == UILayer::TOUCH_BEGAN)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Course::HelpOKMusicEffect()
{
	g_pSoundSystem->PlayEffect(SE_HELPOK);
}

void Course::SetButtonEnable(CCNode* pTarget, void* data)
{
	bool m_data = data ? true : false;
	pkUnderstand->setVisible(m_data);
	pkUnderstand->SetEnable(m_data);
}

void Course::CourseUpDate(VeFloat32 f32Delta)
{
	if (m_pkCourseText1->isVisible() == false && m_pkCourseText2->isVisible() == false && m_pkCourseText3->isVisible() == false && m_pkCourseText4->isVisible() == false)
	{
		pkTest1->stopAllActions();
		pkTest1->setOpacity(0);
		pkTest1->setVisible(true);
		pkTest1->PanelFadeIn(1.0f,0.1f);
		pkPrompt1->stopAllActions();
		pkPrompt1->setOpacity(0);
		pkPrompt1->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		m_pkCourseText1->stopAllActions();
		m_pkCourseText1->setOpacity(0);
		m_pkCourseText1->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
	}
	else if (m_pkCourseText1->isVisible() == true && m_pkCourseText2->isVisible() == false && m_pkCourseText3->isVisible() == false && m_pkCourseText4->isVisible() == false)
	{
		pkTest2->stopAllActions();
		pkTest2->setOpacity(0);
		pkTest2->setVisible(true);
		pkTest2->PanelFadeIn(1.0f,0.1f);
		pkPrompt2->stopAllActions();
		pkPrompt2->setOpacity(0);
		pkPrompt2->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		m_pkCourseText2->stopAllActions();
		m_pkCourseText2->setOpacity(0);
		m_pkCourseText2->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
	}
	else if (m_pkCourseText1->isVisible() == true && m_pkCourseText2->isVisible() == true && m_pkCourseText3->isVisible() == false && m_pkCourseText4->isVisible() == false)
	{
		pkTest3->stopAllActions();
		pkTest3->setOpacity(0);
		pkTest3->setVisible(true);
		pkTest3->PanelFadeIn(1.0f,0.1f);
		pkPrompt3->stopAllActions();
		pkPrompt3->setOpacity(0);
		pkPrompt3->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		pkPrompt3->runAction(CCSequence::create(CCDelayTime::create(0.7f),CCCallFuncND::create(this,callfuncND_selector(Course::SetButtonEnable),(void*)(true)),NULL));
		m_pkCourseText3->stopAllActions();
		m_pkCourseText3->setOpacity(0);
		m_pkCourseText3->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		m_pkCourseText3->runAction(CCSequence::create(CCDelayTime::create(0.7f),CCCallFunc::create(this,callfunc_selector(Course::HelpOKMusicEffect)),NULL));
	}
	else if (m_pkCourseText4->isVisible() == true && m_pkCourseText5->isVisible() == false && m_pkCourseText6->isVisible() == false)
	{
		pkTest5->stopAllActions();
		pkTest5->setOpacity(0);
		pkTest5->setVisible(true);
		pkTest5->PanelFadeIn(1.0f,0.1f);
		pkPrompt5->stopAllActions();
		pkPrompt5->setOpacity(0);
		pkPrompt5->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		m_pkCourseText5->stopAllActions();
		m_pkCourseText5->setOpacity(0);
		m_pkCourseText5->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		if (g_pkGame->GetLanguage() == TexasPoker::zh_CN || g_pkGame->GetLanguage() == TexasPoker::zh_TW)
		{
			pkDrinkLogo->stopAllActions();
			pkDrinkLogo->setOpacity(0);
			pkDrinkLogo->runAction(CCSequence::create(CCDelayTime::create(0.3f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		}
	}
	else if (m_pkCourseText4->isVisible() == true && m_pkCourseText5->isVisible() == true && m_pkCourseText6->isVisible() == false)
	{
		pkTest6->stopAllActions();
		pkTest6->setOpacity(0);
		pkTest6->setVisible(true);
		pkTest6->PanelFadeIn(1.0f,0.1f);
		pkPrompt6->stopAllActions();
		pkPrompt6->setOpacity(0);
		pkPrompt6->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		pkPrompt6->runAction(CCSequence::create(CCDelayTime::create(0.7f),CCCallFuncND::create(this,callfuncND_selector(Course::SetButtonEnable),(void*)(true)),NULL));
		m_pkCourseText6->stopAllActions();
		m_pkCourseText6->setOpacity(0);
		m_pkCourseText6->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
		m_pkCourseText6->runAction(CCSequence::create(CCDelayTime::create(0.7f),CCCallFunc::create(this,callfunc_selector(Course::HelpOKMusicEffect)),NULL));
	}
}

void Course::EnterGame()
{
	g_pkGame->FadeOut(FADE_OUT_TIME, &m_kOnFadeOut);
}

void Course::OnFadeOut()
{
	g_pLayerManager->SetMainLayer("game", NULL, false);
	g_pkGame->FadeIn(FADE_IN_TIME);
}

UIImplementDelegate(Course, OnUnderstand, kWidget, u32State)
{
	if(u32State == UIWidget::RELEASED)
	{
		if (m_pkCourseText1->isVisible() == true && m_pkCourseText2->isVisible() == true && m_pkCourseText3->isVisible() == true)
		{
			pkTest4->setVisible(true);
			pkTest4->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCCallFunc::create(this,callfunc_selector(Course::HideNotice123)),NULL));
			pkTest4->PanelFadeIn(1.0f,0.1f);
			pkPrompt4->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
			m_pkCourseText4->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCShow::create(),CCFadeIn::create(1.0f),NULL));
			schedule(schedule_selector(Course::CourseUpDate), 0.9f);

		}
		else if (m_pkCourseText4->isVisible() == true && m_pkCourseText5->isVisible() == true && m_pkCourseText6->isVisible() == true)
		{
			EnterGame();
			HideNotice123();
			HideNotice456();
			if (g_pkGame->GetLanguage() == TexasPoker::zh_CN || g_pkGame->GetLanguage() == TexasPoker::zh_TW)
			{
				pkDrinkLogo->setVisible(false);
			}
		}
	}
}

void Course::HideNotice123()
{
	pkTest1->setVisible(false);
	pkPrompt1->setVisible(false);
	m_pkCourseText1->setVisible(false);
	pkTest2->setVisible(false);
	pkPrompt2->setVisible(false);
	m_pkCourseText2->setVisible(false);
	pkTest3->setVisible(false);
	pkPrompt3->setVisible(false);
	m_pkCourseText3->setVisible(false);
	pkUnderstand->setVisible(false);
	pkUnderstand->SetEnable(false);
}

void Course::HideNotice456()
{
	pkTest4->setVisible(false);
	pkPrompt4->setVisible(false);
	m_pkCourseText4->setVisible(false);
	pkTest5->setVisible(false);
	pkPrompt5->setVisible(false);
	m_pkCourseText5->setVisible(false);
	pkTest6->setVisible(false);
	pkPrompt6->setVisible(false);
	m_pkCourseText6->setVisible(false);
	pkUnderstand->setVisible(false);
	pkUnderstand->SetEnable(false);
}
