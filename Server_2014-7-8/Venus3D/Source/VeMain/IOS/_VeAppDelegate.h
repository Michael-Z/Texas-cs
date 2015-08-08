////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeAppDelegate.h
//  Version:     v1.00
//  Created:     27/11/2012 by Napoleon
//  Compilers:   Xcode Obj-c
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#import <UIKit/UIKit.h>

@class VeViewController;

@interface VeAppDelegate : UIResponder <UIApplicationDelegate>
{
    UIWindow* m_pkWindow;
    VeViewController* m_pkViewController;
}

@end
