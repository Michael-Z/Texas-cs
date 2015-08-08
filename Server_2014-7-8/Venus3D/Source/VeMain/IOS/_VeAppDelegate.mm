////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeAppDelegate.mm
//  Version:     v1.00
//  Created:     27/11/2012 by Napoleon
//  Compilers:   Xcode Obj-c
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#import "_VeAppDelegate.h"
#import "_VeViewController.h"
#import "_VeEAGLView.h"

@implementation VeAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    m_pkWindow = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    
    VeEAGLView* pkView = [VeEAGLView viewWithFrame:[m_pkWindow bounds]];
    
    m_pkViewController = [[VeViewController alloc] initWithNibName:nil bundle:nil];
    m_pkViewController.wantsFullScreenLayout = YES;
    m_pkViewController.view = pkView;
    
    [m_pkWindow setRootViewController:m_pkViewController];
    [m_pkWindow makeKeyAndVisible];
    [[UIApplication sharedApplication] setStatusBarHidden: YES];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    [m_pkViewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    [m_pkViewController startAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    [m_pkViewController stopAnimation];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}

- (void)dealloc
{
    [m_pkWindow release];
    [m_pkViewController release];
    [super dealloc];
}

@end
