////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeMain.mm
//  Version:     v1.00
//  Created:     27/11/2012 by Napoleon
//  Compilers:   Xcode Obj-c
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#import "_VeAppDelegate.h"

extern VeStartParams g_kParams;

VeInt32 VeStartEngine(VeStartParams& kParams)
{
    VeInt32 i32Res(0);
    @autoreleasepool
    {
        VeChar8 acBuffer[1024];
        [[[NSBundle mainBundle] resourcePath] getCString:acBuffer maxLength:1024 encoding:NSASCIIStringEncoding];
        chdir(acBuffer);
        g_kParams = kParams;
        VeInit();
        i32Res = UIApplicationMain(kParams.m_iArgc, kParams.m_ppcArgv, nil, NSStringFromClass([VeAppDelegate class]));
        VeTerm();
    }
    return i32Res;
}
