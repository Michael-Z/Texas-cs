//
//  TexasPokerAppController.mm
//  TexasPoker
//
//  Created by Napoleon on 13-5-6.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#import <UIKit/UIKit.h>
#import "AppController.h"
#import "cocos2d.h"
#import "EAGLView.h"
#import "AppDelegate.h"
#import "CocoaHelper.h"
#import "TexasPoker.h"
#import "RootViewController.h"
#import "Login.h"
#import "MobClick.h"
#import "UMFeedback.h"
#import "YouMiWall.h"
#import "GTMBase64.h"
#import <CommonCrypto/CommonDigest.h>

@implementation AppController

AppController* g_pkAppDelegate = nil;

@synthesize window;
@synthesize viewController;

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    EAGLView *__glView = [EAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGBA8
                                     depthFormat: GL_DEPTH_COMPONENT16
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples:0 ];

    // Use RootViewController manage EAGLView
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = __glView;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:viewController];
    }
    
    [window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden: YES];
    
    g_pkAppDelegate = self;

    cocos2d::CCApplication::sharedApplication()->run();
    
    [MobClick startWithAppkey:@"5228841456240b094f00a983"];
    [MobClick checkUpdate];
    [MobClick updateOnlineConfig];
    [YouMiConfig setUserID:[NSString stringWithUTF8String:CocoaHelper::GetDeviceID()]];
    [YouMiConfig setUseInAppStore:YES];
    [YouMiConfig launchWithAppID:@"60b938aa1f44cd7e" appSecret:@"27368799d40d5d5d"];
    [YouMiConfig setFullScreenWindow:self.window];
    [YouMiWall enable];
    
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    CocoaHelper::OnAuthenticateLocalPlayer();
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    cocos2d::CCDirector::sharedDirector()->pause();
    ;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    cocos2d::CCDirector::sharedDirector()->resume();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::CCApplication::sharedApplication()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::CCApplication::sharedApplication()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
     cocos2d::CCDirector::sharedDirector()->purgeCachedData();
}


- (void)dealloc {
    [super dealloc];
}

+ (AppController *) AppDelegate
{
    assert(g_pkAppDelegate != nil);
    return g_pkAppDelegate;
}

- (void)backgroundHandler
{

}

- (void)ShowFeedBackHandler
{
    [UMFeedback showFeedback:viewController withAppkey:@"5228841456240b094f00a983"];
}

- (void)OnPurchase:(unsigned int)index
{
    VeChar8 acBuffer[64];
    VeSprintf(acBuffer, 64, "cn.VenusIE.TexasPoker.Purch%d", index + 1);
    SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObjects:[NSString stringWithUTF8String:acBuffer], nil]];
    request.delegate = self;
    [request start];
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray* myProduct = response.products;
    if([myProduct count] == 1)
    {
        SKProduct* product = [myProduct objectAtIndex:0];
        [[SKPaymentQueue defaultQueue] addPayment:[SKPayment paymentWithProductIdentifier:product.productIdentifier]];
    }
    else
    {
        g_pkGame->OnPurchaseEnd(-1);
    }
    [request autorelease];
}

- (void) completeTransaction: (SKPaymentTransaction *)transaction
{
    VeString kReceipt = [[GTMBase64 stringByEncodingData:transaction.transactionReceipt] UTF8String];
    unsigned char acResult[16];
    CC_MD5(kReceipt.GetString(), kReceipt.Length(), (unsigned char*)acResult);
    VeChar8 acBuffer[64];
    VeChar8* pcTemp = acBuffer;
    for(VeUInt32 i(0); i < 16; ++i)
    {
        VeUInt8 cIter = acResult[i];
        if(cIter < 0x10)
        {
            pcTemp += VeSprintf(pcTemp, 32, "0%x", cIter);   
        }
        else
        {
            pcTemp += VeSprintf(pcTemp, 32, "%x", cIter);
        }
    }
    VeString kValue("?qudao=APPSTORE&uuid=");
    kValue += g_pkGame->GetDeviceID();
    kValue += "&orderid=";
    kValue += acBuffer;
    VeString kPostData = "orderinfo=";
    kPostData += kReceipt;
    g_pkGame->VerifyIOSPurchase(kValue, kPostData);
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
	{
		switch (transaction.transactionState)
		{
			case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                //g_pLayerManager->PopAllMiddleLayers();
                break;
            case SKPaymentTransactionStateFailed:
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                g_pLayerManager->PopAllMiddleLayers();
                break;
            case SKPaymentTransactionStateRestored:
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                g_pLayerManager->PopAllMiddleLayers();
            default:
                break;
		}
	}
}

@end
