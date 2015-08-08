//
//  CocoaHelp.h
//  QueenJong
//
//  Created by liu letian on 12-4-14.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#pragma once

#include "SocketIncludes.h"
#include "Venus3D.h"

class CocoaHelper
{
public:
    
    static const char* GetDeviceID();
    
    static const char* GetLanguage();
    
    static const char* GetMachineName();
    
    static void GetAppVersion(VeUInt8* pu8Version);
    
    static void ShowFeedBack();
    
    static void ShowWall();
    
    static void OnEvent(const VeChar8* pcEvent);
    
    static void OnEvent(const VeChar8* pcEvent, const VeVector< VePair<VeStringA,VeStringA> >& kValue);
    
    static void OnEventBegin(const VeChar8* pcEvent);
    
    static void OnEventEnd(const VeChar8* pcEvent);
    
    static void OnPageStart(const VeChar8* pcPage);
    
    static void OnPageEnd(const VeChar8* pcPage);
    
    static void OnPurchase(VeUInt32 u32Index);
    
    static void OnAuthenticateLocalPlayer();
    
    static void OnUpdateChips(VeUInt64 u64Chips);
    
};
