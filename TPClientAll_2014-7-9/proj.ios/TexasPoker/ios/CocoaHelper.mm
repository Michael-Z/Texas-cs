#include "CocoaHelper.h"
#import "UIDevice+IdentifierAddition.h"
#import "AppController.h"
#import "TexasPoker.h"
#import "MobClick.h"
#import "UMFeedback.h"
#import "YouMiWall.h"
#import "OpenUDID.h"
#import "GameKit/GameKit.h"

const char* CocoaHelper::GetDeviceID()
{
    NSString* DeviceID = [OpenUDID value];
    const char* str = [DeviceID UTF8String];
    if (strcmp(str,"") != 0) {
        return str;
    }
    return "";
}

const char* CocoaHelper::GetLanguage()
{
    NSString* pkLan = [[[NSUserDefaults standardUserDefaults] objectForKey:@"AppleLanguages"] objectAtIndex:0];
    if(![pkLan compare:@"zh-Hans"])
    {
        return "zh_CN";
    }
    else if(![pkLan compare:@"zh-Hant"])
    {
        return "zh_TW";
    }
    else
    {
        return "en_US";
    }
}

const char* CocoaHelper::GetMachineName()
{
    NSString* deviceName = [UIDevice currentDevice].name;
    const char* str = [deviceName UTF8String];
    return str;
}

void CocoaHelper::GetAppVersion(VeUInt8* pu8Version)
{
    VeZeroMemory(pu8Version, 4);
    NSString* pkVersion = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
    VeChar8 acBuffer[64];
    VeStrcpy(acBuffer, 64, [pkVersion UTF8String]);
    VeChar8* pcContent;
    VeChar8* pcTemp = VeStrtok(acBuffer, ".", &pcContent);
    VeUInt32 i(0);
    while(pcTemp && i < 4)
    {
        VeStringA(pcTemp).To(pu8Version[i]);
        pcTemp = VeStrtok(NULL, ".", &pcContent);
        ++i;
    }    
}

void CocoaHelper::ShowFeedBack()
{
    [[AppController AppDelegate] ShowFeedBackHandler];
}

void CocoaHelper::ShowWall()
{
    [YouMiWall showOffers:YES didShowBlock:^{
        NSLog(@"有米积分墙已显示");
    } didDismissBlock:^{
        NSLog(@"有米积分墙已退出");
    }];
}

void CocoaHelper::OnEvent(const VeChar8* pcEvent)
{
    [MobClick event:[NSString stringWithUTF8String:pcEvent]];
}

void CocoaHelper::OnEvent(const VeChar8* pcEvent, const VeVector< VePair<VeStringA,VeStringA> >& kValue)
{
    NSMutableDictionary* pkDic = [NSMutableDictionary dictionary];
    for(VeUInt32 i(0); i < kValue.Size(); ++i)
    {
        [pkDic setObject:[NSString stringWithUTF8String:kValue[i].m_tSecond] forKey:[NSString stringWithUTF8String:kValue[i].m_tFirst]];
    }
    [MobClick event:[NSString stringWithUTF8String:pcEvent] attributes:pkDic];
}

void CocoaHelper::OnEventBegin(const VeChar8* pcEvent)
{
    [MobClick beginEvent:[NSString stringWithUTF8String:pcEvent]];
}

void CocoaHelper::OnEventEnd(const VeChar8* pcEvent)
{
    [MobClick endEvent:[NSString stringWithUTF8String:pcEvent]];
}

void CocoaHelper::OnPageStart(const VeChar8* pcPage)
{
    [MobClick beginLogPageView:[NSString stringWithUTF8String:pcPage]];
}

void CocoaHelper::OnPageEnd(const VeChar8* pcPage)
{
    [MobClick endLogPageView:[NSString stringWithUTF8String:pcPage]];
}

void CocoaHelper::OnPurchase(VeUInt32 u32Index)
{
    if([SKPaymentQueue canMakePayments])
    {
        [[AppController AppDelegate] OnPurchase:u32Index];
    }
    else
    {
        g_pkGame->OnPurchaseEnd(-1);
    }
}

void CocoaHelper::OnAuthenticateLocalPlayer()
{
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error)
     {
        if(error == nil)
        {
            NSLog(@"Start Game Center Succeed");
        }
        else
        {
            NSLog(@"Start Game Center Failed");
        }
    }];
}

void CocoaHelper::OnUpdateChips(VeUInt64 u64Chips)
{
    GKScore* score = [[[GKScore alloc] initWithCategory:@"cn.VenusIE.TexasPoker.RichList"] autorelease];
    score.value = u64Chips;
    [score reportScoreWithCompletionHandler:^(NSError *error) {
        if(error == nil)
        {
            NSLog(@"Game Center Seport Score Succeed");
        }
        else
        {
            NSLog(@"Game Center Seport Score Failed");
        }
    }];
}

void GetPurchaseList(TexasPoker::PurchaseInfoList& kList)
{
    TexasPoker::Language eLan = g_pkGame->GetLanguage();
    kList.Resize(7);
    
    kList[0].m_kInfo = TT("IOS_BuyPackage0");
    kList[0].m_u32Chips = 40000;
    kList[0].m_bVip = false;
    kList[0].m_u32Icon = 0;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[0].m_u32Price = 600;
        kList[0].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[0].m_u32Price = 99;
        kList[0].m_eType = PURCHASE_TYPE_DOLLAR;
    }
    
    kList[1].m_kInfo = TT("IOS_BuyPackage1");
    kList[1].m_u32Chips = 90000;
    kList[1].m_bVip = true;
    kList[1].m_u32Icon = 0;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[1].m_u32Price = 1200;
        kList[1].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[1].m_u32Price = 199;
        kList[1].m_eType = PURCHASE_TYPE_DOLLAR;
    }
    
    kList[2].m_kInfo = TT("IOS_BuyPackage2");
    kList[2].m_u32Chips = 180000;
    kList[2].m_bVip = true;
    kList[2].m_u32Icon = 1;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[2].m_u32Price = 2500;
        kList[2].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[2].m_u32Price = 399;
        kList[2].m_eType = PURCHASE_TYPE_DOLLAR;
    }
    
    kList[3].m_kInfo = TT("IOS_BuyPackage3");
    kList[3].m_u32Chips = 350000;
    kList[3].m_bVip = true;
    kList[3].m_u32Icon = 2;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[3].m_u32Price = 5000;
        kList[3].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[3].m_u32Price = 799;
        kList[3].m_eType = PURCHASE_TYPE_DOLLAR;
    }
    
    kList[4].m_kInfo = TT("IOS_BuyPackage4");
    kList[4].m_u32Chips = 900000;
    kList[4].m_bVip = true;
    kList[4].m_u32Icon = 3;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[4].m_u32Price = 12800;
        kList[4].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[4].m_u32Price = 1999;
        kList[4].m_eType = PURCHASE_TYPE_DOLLAR;
    }
    
    kList[5].m_kInfo = TT("IOS_BuyPackage5");
    kList[5].m_u32Chips = 2000000;
    kList[5].m_bVip = true;
    kList[5].m_u32Icon = 4;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[5].m_u32Price = 25800;
        kList[5].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[5].m_u32Price = 3999;
        kList[5].m_eType = PURCHASE_TYPE_DOLLAR;
    }
    
    kList[6].m_kInfo = TT("IOS_BuyPackage6");
    kList[6].m_u32Chips = 8000000;
    kList[6].m_bVip = true;
    kList[6].m_u32Icon = 5;
    if(eLan == TexasPoker::zh_CN)
    {
        kList[6].m_u32Price = 64800;
        kList[6].m_eType = PURCHASE_TYPE_RMB;
    }
    else
    {
        kList[6].m_u32Price = 9999;
        kList[6].m_eType = PURCHASE_TYPE_DOLLAR;
    }
}

const VeChar8* GetChannel()
{
    return "APP_STORE";
}

bool ShowWall()
{
    if(!VeStrcmp("0", [[MobClick getConfigParams:@"ShowWall"] UTF8String]))
    {
        return false;
    }
    else
    {
        return true;
    }
}
