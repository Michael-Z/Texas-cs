//
//  TexasPokerAppController.h
//  TexasPoker
//
//  Created by Napoleon on 13-5-6.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//

#import <StoreKit/StoreKit.h>

@class RootViewController;

@interface AppController : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate, SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    UIWindow *window;
    RootViewController    *viewController;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) RootViewController *viewController;

+ (AppController *) AppDelegate;

- (void)backgroundHandler;
- (void)ShowFeedBackHandler;

- (void)OnPurchase:(unsigned int)index;

@end

