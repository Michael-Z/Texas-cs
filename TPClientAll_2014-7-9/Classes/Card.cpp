#include "Card.h"
#include "Knickknacks.h"
#include "TexasPoker.h"
#include "SoundSystem.h"

USING_NS_CC;

Card::Card() : m_u32Type(1), m_eNumber(CARD_A), m_eSuit(CARD_SPADE)
	, m_f32TrunTime(0.0f), m_bFace(true), m_bLight(false)
{
	m_pkRoot = CCNode::create();
	m_pkRoot->setScale(ui_main_scale);
	addChild(m_pkRoot, 0);

	m_pkCardFace = Perspective::create("card_face.png");
	m_pkRoot->addChild(m_pkCardFace, 0);
	m_pkCardBack = new Perspective;
	m_pkCardBack->autorelease();
	m_pkRoot->addChild(m_pkCardBack, 0);

	m_pkNumber = new Perspective;
	m_pkNumber->autorelease();
	m_pkRoot->addChild(m_pkNumber, 0);

	m_pkSuitLarge = new Perspective;
	m_pkSuitLarge->autorelease();
	m_pkRoot->addChild(m_pkSuitLarge, 0);

	m_pkSuitSmall = new Perspective;
	m_pkSuitSmall->autorelease();
	m_pkRoot->addChild(m_pkSuitSmall, 0);

	m_pkLight = CCSprite::create();
	m_pkRoot->addChild(m_pkLight, 0);
	m_pkLight->setVisible(false);

	m_pkCardFace->SetRotateCenter(24.0f * g_f32ScaleHD, 32.0f * g_f32ScaleHD);
	m_pkCardBack->SetRotateCenter(24.0f * g_f32ScaleHD, 32.0f * g_f32ScaleHD);
	m_pkNumber->SetRotateCenter(6.0f * g_f32ScaleHD, 9.0f * g_f32ScaleHD);
	m_pkSuitSmall->SetRotateCenter(7.0f * g_f32ScaleHD, 7.0f * g_f32ScaleHD);
	m_pkSuitLarge->SetRotateCenter(20.0f * g_f32ScaleHD, 23.0f * g_f32ScaleHD);

	m_kNumberOffset = ccp(-14.0f * g_f32ScaleHD, 20.0f * g_f32ScaleHD);
	m_pkNumber->setPosition(m_kNumberOffset);

	m_kSuitSmallOffset = ccp(-14.0f * g_f32ScaleHD, 5.0f * g_f32ScaleHD);
	m_pkSuitSmall->setPosition(m_kSuitSmallOffset);

	m_kSuitLargeOffset = ccp(0.0f * g_f32ScaleHD, -5.0f * g_f32ScaleHD);
	m_pkSuitLarge->setPosition(m_kSuitLargeOffset);

	UpdateType();
	UpdateContent();
	UpdateAngle();
}

Card::~Card()
{

}

void Card::SetType(VeUInt32 u32Type)
{
	if(m_u32Type != u32Type)
	{
		m_u32Type = u32Type;
		UpdateType();
	}
}

VeUInt32 Card::GetType()
{
	return m_u32Type;
}

void Card::SetContent(CardNum eNum, CardSuit eSuit)
{
	if(m_eNumber != eNum || m_eSuit != eSuit)
	{
		m_eNumber = eNum;
		m_eSuit = eSuit;
		UpdateContent();
	}
}

CardNum Card::GetNumber()
{
	return m_eNumber;
}

CardSuit Card::GetSuit()
{
	return m_eSuit;
}

void Card::UpdateType()
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "card_back_%d.png", m_u32Type);
	m_pkCardBack->initWithSpriteFrameName(acBuffer);
	m_pkLight->initWithSpriteFrameName(m_u32Type ? "vip_card_light.png" : "card_light.png");
}

void Card::UpdateContent()
{
	VeChar8 acBuffer[64];
	const VeChar8 cCol = m_eSuit & 0x1 ? 'r' : 'b';
	VeSprintf(acBuffer, 64, "%c%d.png", cCol, m_eNumber + 1);
	m_pkNumber->initWithSpriteFrameName(acBuffer);
	if(m_eNumber >=  CARD_J)
	{
		VeChar8 cSuit;
		if(m_eNumber == CARD_J)
			cSuit = 'j';
		else if(m_eNumber == CARD_Q)
			cSuit = 'q';
		else
			cSuit = 'k';
		VeSprintf(acBuffer, 64, "%c%c.png", cCol, cSuit);
		m_pkSuitLarge->initWithSpriteFrameName(acBuffer);
	}

	switch(m_eSuit)
	{
	case CARD_SPADE:
		m_pkSuitSmall->initWithSpriteFrameName("spade_s.png");
		if(m_eNumber <  CARD_J)
			m_pkSuitLarge->initWithSpriteFrameName("spade_l.png");
		break;
	case CARD_HEART:
		m_pkSuitSmall->initWithSpriteFrameName("heart_s.png");
		if(m_eNumber <  CARD_J)
			m_pkSuitLarge->initWithSpriteFrameName("heart_l.png");
		break;
	case CARD_CLUB:
		m_pkSuitSmall->initWithSpriteFrameName("club_s.png");
		if(m_eNumber <  CARD_J)
			m_pkSuitLarge->initWithSpriteFrameName("club_l.png");
		break;
	case CARD_DIAMOND:
		m_pkSuitSmall->initWithSpriteFrameName("diamond_s.png");
		if(m_eNumber <  CARD_J)
			m_pkSuitLarge->initWithSpriteFrameName("diamond_l.png");
		break;
	default:
		break;
	}
}

void Card::UpdateAngle()
{
	if(VeFloat32(m_kAngle) <= VE_MATH_PI_2_F && VeFloat32(m_kAngle) >= -VE_MATH_PI_2_F)
	{
		VeQuaternion kQuat;
		kQuat.FromAngleAxis(m_kAngle, VeVector3D::UNIT_Y);
		m_pkCardFace->SetRotate(kQuat);
		m_pkNumber->SetRotate(kQuat);
		m_pkSuitLarge->SetRotate(kQuat);
		m_pkSuitSmall->SetRotate(kQuat);
		m_pkCardFace->setVisible(true);
		m_pkNumber->setVisible(true);
		m_pkSuitSmall->setVisible(true);
		m_pkSuitLarge->setVisible(true);
		m_pkCardBack->setVisible(false);

		VeVector3D kPos = kQuat * VeVector3D(m_kNumberOffset.x, m_kNumberOffset.y, 0);
		m_pkNumber->setPosition(ccp(kPos.x, kPos.y));
		kPos = kQuat * VeVector3D(m_kSuitSmallOffset.x, m_kSuitSmallOffset.y, 0);
		m_pkSuitSmall->setPosition(ccp(kPos.x, kPos.y));
		kPos = kQuat * VeVector3D(m_kSuitLargeOffset.x, m_kSuitLargeOffset.y, 0);
		m_pkSuitLarge->setPosition(ccp(kPos.x, kPos.y));
	}
	else
	{
		VeFloat32 f32Angle = m_kAngle;
		if(f32Angle > VE_MATH_PI_2_F)
		{
			f32Angle -= VE_MATH_PI_F;
		}
		else
		{
			f32Angle += VE_MATH_PI_F;
		}

		VeQuaternion kQuat;
		kQuat.FromAngleAxis(f32Angle, VeVector3D::UNIT_Y);
		m_pkCardBack->SetRotate(kQuat);
		m_pkCardFace->setVisible(false);
		m_pkNumber->setVisible(false);
		m_pkSuitSmall->setVisible(false);
		m_pkSuitLarge->setVisible(false);
		m_pkCardBack->setVisible(true);
	}
}

void Card::SetAngle(VeFloat32 f32Angle)
{
	if(m_kAngle != f32Angle)
	{
		m_kAngle = f32Angle;
		UpdateAngle();
	}
}

VeFloat32 Card::GetAngle()
{
	return m_kAngle;
}

void Card::Turn(VeFloat32 f32Delay)
{
	scheduleOnce(schedule_selector(Card::OnTurn), f32Delay);
}

bool Card::IsFace()
{
	return m_bFace;
}

void Card::SetFace(bool bFace)
{
	if(bFace)
	{
		m_bFace = true;
		m_f32TrunTime = 0;
		if(getZOrder() == 3)
		{
			setZOrder(5);
		}
		SetAngle(0);
	}
	else
	{
		m_bFace = false;
		m_f32TrunTime = CARD_TURN_DELAY;
		if(getZOrder() == 5)
		{
			setZOrder(3);
		}
		SetAngle(VE_MATH_PI_F);
	}
	unschedule(schedule_selector(Card::Update));
}

void Card::Update(VeFloat32 f32Delta)
{
	if(m_bFace)
	{
		m_f32TrunTime -= f32Delta;
		if(m_f32TrunTime > 0.0f)
		{
			VeFloat32 f32Rate = VeCosf((m_f32TrunTime / CARD_TURN_DELAY) * VE_MATH_PI_2_F);
			SetAngle(VeLerp(f32Rate, VE_MATH_PI_F, 0));
			if(m_f32TrunTime < (CARD_TURN_DELAY * 0.5f))
			{
				if(getZOrder() == 3)
				{
					setZOrder(5);
				}
			}
		}
		else
		{
			m_f32TrunTime = 0.0f;
			SetAngle(0);
			unschedule(schedule_selector(Card::Update));
			m_kEvent.Callback(*this, TURN_FACE);
			m_pkLight->setVisible(m_bLight);
		}
	}
	else
	{
		m_f32TrunTime += f32Delta;
		if(m_f32TrunTime < CARD_TURN_DELAY)
		{
			VeFloat32 f32Rate = VeCosf((m_f32TrunTime / CARD_TURN_DELAY) * VE_MATH_PI_2_F);
			SetAngle(VeLerp(f32Rate, VE_MATH_PI_F, 0));
			if(m_f32TrunTime > (CARD_TURN_DELAY * 0.5f))
			{
				if(getZOrder() == 5)
				{
					setZOrder(3);
				}
			}
		}
		else
		{
			m_f32TrunTime = CARD_TURN_DELAY;
			SetAngle(VE_MATH_PI_F);
			unschedule(schedule_selector(Card::Update));
			m_kEvent.Callback(*this, TURN_BACK);
		}
	}
}

void Card::SetDealPos(cocos2d::CCPoint kPos)
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	m_kDealPos.x = kPos.x - kTransform.tx;
	m_kDealPos.y = kPos.y - kTransform.ty;
}

void Card::Deal(VeFloat32 f32Delay)
{
	SetVisible(false);
	m_pkLight->setVisible(false);
	scheduleOnce(schedule_selector(Card::OnDeal), f32Delay);
}

void Card::OnDeal(VeFloat32 f32Delta)
{
	SetFace(false);
	m_f32DealTime = 0.0f;
	m_pkRoot->setPosition(m_kDealPos);
	m_pkCardBack->setOpacity(0);
	m_pkRoot->setRotation(0);
	SetVisible(true);
	schedule(schedule_selector(Card::UpdateDeal), 0.0f);
	g_pSoundSystem->PlayEffect(SE_DEAL_CARD);
}

void Card::OnTurn(VeFloat32 f32Delta)
{
	m_bFace = !m_bFace;
	schedule(schedule_selector(Card::Update), 0.0f);
}

void Card::UpdateDeal(VeFloat32 f32Delta)
{
	m_f32DealTime += f32Delta * 3.0f;
	if(m_f32DealTime >= 1.0f)
	{
		m_f32DealTime = 1.0f;
		unschedule(schedule_selector(Card::UpdateDeal));
		m_pkRoot->setPosition(ccp(0, 0));
		m_pkCardBack->setOpacity(255);
		m_pkRoot->setRotation(0);
	}
	else
	{
		VeFloat32 f32Alpha = VePowf(VE_MIN(m_f32DealTime * 3.0f, 1.0f), 2.0f);
		m_pkCardBack->setOpacity(f32Alpha * 255);
		VeFloat32 f32RatePos = VeCosf(m_f32DealTime * VE_MATH_PI_2_F);
		m_pkRoot->setPosition(ccp(m_kDealPos.x * f32RatePos, m_kDealPos.y * f32RatePos));
		m_pkRoot->setRotation(-f32RatePos * 360);
	}
}

void Card::Reset()
{
	SetVisible(false);
	unscheduleAllSelectors();
	m_pkRoot->setPosition(ccp(0, 0));
	m_pkCardBack->setOpacity(255);
	m_pkRoot->setRotation(0);
	SetFace(false);
}

void Card::SetLight(bool bLight)
{
	m_bLight = bLight;
	m_pkLight->setVisible(m_bLight);
}

MiniCard::MiniCard() : m_u32Type(1), m_eNumber(CARD_A), m_eSuit(CARD_SPADE)
	, m_f32TrunTime(0.0f), m_bFace(true)
{
	m_pkRoot = CCNode::create();
	m_pkRoot->setScale(ui_main_scale);
	addChild(m_pkRoot, 0);

	m_pkCardFace = Perspective::create("card_face_s.png");
	m_pkRoot->addChild(m_pkCardFace, 0);
	m_pkCardBack = new Perspective;
	m_pkCardBack->autorelease();
	m_pkRoot->addChild(m_pkCardBack, 0);

	m_pkNumber = new Perspective;
	m_pkNumber->autorelease();
	m_pkRoot->addChild(m_pkNumber, 0);

	m_pkSuitSmall = new Perspective;
	m_pkSuitSmall->autorelease();
	m_pkRoot->addChild(m_pkSuitSmall, 0);

	m_pkCardFace->SetRotateCenter(14.0f * g_f32ScaleHD, 18.0f * g_f32ScaleHD);
	m_pkCardBack->SetRotateCenter(14.0f * g_f32ScaleHD, 18.0f * g_f32ScaleHD);
	m_pkNumber->SetRotateCenter(6.0f * g_f32ScaleHD, 9.0f * g_f32ScaleHD);
	m_pkSuitSmall->SetRotateCenter(7.0f * g_f32ScaleHD, 7.0f * g_f32ScaleHD);

	m_kNumberOffset = ccp(-5.0f * g_f32ScaleHD, 8.0f * g_f32ScaleHD);
	m_pkNumber->setPosition(m_kNumberOffset);

	m_kSuitSmallOffset = ccp(-5.0f * g_f32ScaleHD, -7.0f * g_f32ScaleHD);
	m_pkSuitSmall->setPosition(m_kSuitSmallOffset);

	UpdateType();
	UpdateContent();
	UpdateAngle();
}

MiniCard::~MiniCard()
{

}

void MiniCard::SetType(VeUInt32 u32Type)
{
	if(m_u32Type != u32Type)
	{
		m_u32Type = u32Type;
		UpdateType();
	}
}

VeUInt32 MiniCard::GetType()
{
	return m_u32Type;
}

void MiniCard::SetContent(CardNum eNum, CardSuit eSuit)
{
	if(m_eNumber != eNum || m_eSuit != eSuit)
	{
		m_eNumber = eNum;
		m_eSuit = eSuit;
		UpdateContent();
	}
}

CardNum MiniCard::GetNumber()
{
	return m_eNumber;
}

CardSuit MiniCard::GetSuit()
{
	return m_eSuit;
}

void MiniCard::UpdateType()
{
	VeChar8 acBuffer[64];
	VeSprintf(acBuffer, 64, "card_back_s_%d.png", m_u32Type);
	m_pkCardBack->initWithSpriteFrameName(acBuffer);
}

void MiniCard::UpdateContent()
{
	VeChar8 acBuffer[64];
	const VeChar8 cCol = m_eSuit & 0x1 ? 'r' : 'b';
	VeSprintf(acBuffer, 64, "%c%d.png", cCol, m_eNumber + 1);
	m_pkNumber->initWithSpriteFrameName(acBuffer);

	switch(m_eSuit)
	{
	case CARD_SPADE:
		m_pkSuitSmall->initWithSpriteFrameName("spade_s.png");
		break;
	case CARD_HEART:
		m_pkSuitSmall->initWithSpriteFrameName("heart_s.png");
		break;
	case CARD_CLUB:
		m_pkSuitSmall->initWithSpriteFrameName("club_s.png");
		break;
	case CARD_DIAMOND:
		m_pkSuitSmall->initWithSpriteFrameName("diamond_s.png");
		break;
	default:
		break;
	}
}

void MiniCard::UpdateAngle()
{
	if(VeFloat32(m_kAngle) <= VE_MATH_PI_2_F && VeFloat32(m_kAngle) >= -VE_MATH_PI_2_F)
	{
		VeQuaternion kQuat;
		kQuat.FromAngleAxis(m_kAngle, VeVector3D::UNIT_Y);
		m_pkCardFace->SetRotate(kQuat);
		m_pkNumber->SetRotate(kQuat);
		m_pkSuitSmall->SetRotate(kQuat);
		m_pkCardFace->setVisible(true);
		m_pkNumber->setVisible(true);
		m_pkSuitSmall->setVisible(true);
		m_pkCardBack->setVisible(false);

		VeVector3D kPos = kQuat * VeVector3D(m_kNumberOffset.x, m_kNumberOffset.y, 0);
		m_pkNumber->setPosition(ccp(kPos.x, kPos.y));
		kPos = kQuat * VeVector3D(m_kSuitSmallOffset.x, m_kSuitSmallOffset.y, 0);
		m_pkSuitSmall->setPosition(ccp(kPos.x, kPos.y));
	}
	else
	{
		VeFloat32 f32Angle = m_kAngle;
		if(f32Angle > VE_MATH_PI_2_F)
		{
			f32Angle -= VE_MATH_PI_F;
		}
		else
		{
			f32Angle += VE_MATH_PI_F;
		}

		VeQuaternion kQuat;
		kQuat.FromAngleAxis(f32Angle, VeVector3D::UNIT_Y);
		m_pkCardBack->SetRotate(kQuat);
		m_pkCardFace->setVisible(false);
		m_pkNumber->setVisible(false);
		m_pkSuitSmall->setVisible(false);
		m_pkCardBack->setVisible(true);
	}
}

void MiniCard::SetAngle(VeFloat32 f32Angle)
{
	if(m_kAngle != f32Angle)
	{
		m_kAngle = f32Angle;
		UpdateAngle();
	}
}

VeFloat32 MiniCard::GetAngle()
{
	return m_kAngle;
}

void MiniCard::OnTurn(VeFloat32 f32Delta)
{
	m_bFace = !m_bFace;
	schedule(schedule_selector(MiniCard::Update), 0.0f);
}

void MiniCard::Turn(VeFloat32 f32Delay)
{
	scheduleOnce(schedule_selector(MiniCard::OnTurn), f32Delay);
}

bool MiniCard::IsFace()
{
	return m_bFace;
}

void MiniCard::SetFace(bool bFace)
{
	if(bFace)
	{
		m_bFace = true;
		m_f32TrunTime = 0;
		if(getZOrder() == 3)
		{
			setZOrder(5);
		}
		SetAngle(0);
	}
	else
	{
		m_bFace = false;
		m_f32TrunTime = CARD_TURN_DELAY;
		if(getZOrder() == 5)
		{
			setZOrder(3);
		}
		SetAngle(VE_MATH_PI_F);
	}
	unschedule(schedule_selector(MiniCard::Update));
}

void MiniCard::Update(VeFloat32 f32Delta)
{
	if(m_bFace)
	{
		m_f32TrunTime -= f32Delta;
		if(m_f32TrunTime > 0.0f)
		{
			VeFloat32 f32Rate = VeCosf((m_f32TrunTime / CARD_TURN_DELAY) * VE_MATH_PI_2_F);
			SetAngle(VeLerp(f32Rate, VE_MATH_PI_F, 0));
			if(m_f32TrunTime < (CARD_TURN_DELAY * 0.5f))
			{
				if(getZOrder() == 3)
				{
					setZOrder(5);
				}
			}
		}
		else
		{
			m_f32TrunTime = 0.0f;
			SetAngle(0);
			unschedule(schedule_selector(MiniCard::Update));
		}
	}
	else
	{
		m_f32TrunTime += f32Delta;
		if(m_f32TrunTime < CARD_TURN_DELAY)
		{
			VeFloat32 f32Rate = VeCosf((m_f32TrunTime / CARD_TURN_DELAY) * VE_MATH_PI_2_F);
			SetAngle(VeLerp(f32Rate, VE_MATH_PI_F, 0));
			if(m_f32TrunTime > (CARD_TURN_DELAY * 0.5f))
			{
				if(getZOrder() == 5)
				{
					setZOrder(3);
				}
			}
		}
		else
		{
			m_f32TrunTime = CARD_TURN_DELAY;
			SetAngle(VE_MATH_PI_F);
			unschedule(schedule_selector(MiniCard::Update));
		}
	}
}

void MiniCard::SetDealPos(cocos2d::CCPoint kPos)
{
	CCAffineTransform kTransform = nodeToWorldTransform();
	m_kDealPos.x = kPos.x - kTransform.tx;
	m_kDealPos.y = kPos.y - kTransform.ty;
}

void MiniCard::Deal(VeFloat32 f32Delay)
{
	SetVisible(false);
	scheduleOnce(schedule_selector(MiniCard::OnDeal), f32Delay);
}

void MiniCard::OnDeal(VeFloat32 f32Delta)
{
	SetFace(false);
	m_f32DealTime = 0.0f;
	m_pkRoot->setPosition(m_kDealPos);
	m_pkCardBack->setOpacity(0);
	m_pkRoot->setRotation(0);
	SetVisible(true);
	schedule(schedule_selector(MiniCard::UpdateDeal), 0.0f);
	g_pSoundSystem->PlayEffect(SE_DEAL_CARD);
}

void MiniCard::UpdateDeal(VeFloat32 f32Delta)
{
	m_f32DealTime += f32Delta * 3.0f;
	if(m_f32DealTime >= 1.0f)
	{
		m_f32DealTime = 1.0f;
		unschedule(schedule_selector(MiniCard::UpdateDeal));
		m_pkRoot->setPosition(ccp(0, 0));
		m_pkCardBack->setOpacity(255);
		m_pkRoot->setRotation(0);
	}
	else
	{
		VeFloat32 f32Alpha = VePowf(VE_MIN(m_f32DealTime * 3.0f, 1.0f), 2.0f);
		m_pkCardBack->setOpacity(f32Alpha * 255);
		VeFloat32 f32RatePos = VeCosf(m_f32DealTime * VE_MATH_PI_2_F);
		m_pkRoot->setPosition(ccp(m_kDealPos.x * f32RatePos, m_kDealPos.y * f32RatePos));
		m_pkRoot->setRotation(-f32RatePos * 360);
	}
}

void MiniCard::Reset()
{
	SetVisible(false);
	unscheduleAllSelectors();
	m_pkRoot->setPosition(ccp(0, 0));
	m_pkCardBack->setOpacity(255);
	m_pkRoot->setRotation(0);
	SetFace(false);
}
