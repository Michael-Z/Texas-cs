////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   _VeEAGLView.mm
//  Version:     v1.00
//  Created:     27/11/2012 by Napoleon
//  Compilers:   Xcode Obj-c
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#import "_VeEAGLView.h"
#import <QuartzCore/QuartzCore.h>
#include "../VeMainPCH.h"
#include "../_VeRendererGLES2.h"

VeEAGLView* g_pkView = NULL;
static VeVector<VeVector2D> s_kTouchCache;

void VeRenderbufferStorageLayer()
{
    VE_ASSERT(g_pkView);
    [g_pkView->m_pkContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)g_pkView.layer];
}

void VeGLESPresent()
{
    VE_ASSERT(g_pkView);
    [g_pkView->m_pkContext presentRenderbuffer:GL_RENDERBUFFER];
}

void VeViewControllerTick()
{
    if(g_pRenderer->IsInited())
    {
        VE_ASSERT(g_pkView);
        [EAGLContext setCurrentContext:g_pkView->m_pkContext];
        g_pApplication->Update();
        g_pApplication->Render();
    }
}

@implementation VeEAGLView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

+ (id) viewWithFrame:(CGRect)frame
{
    return [[[self alloc] initWithFrame:frame] autorelease];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        g_pkView = self;
        if([self respondsToSelector:@selector(setContentScaleFactor:)])
		{
			self.contentScaleFactor = [[UIScreen mainScreen] scale];
		}
        [self setMultipleTouchEnabled:TRUE];
        CAEAGLLayer* pkLayer = (CAEAGLLayer*)self.layer;
        pkLayer.opaque = YES;
        pkLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
            kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        m_pkContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if(!m_pkContext || ![EAGLContext setCurrentContext:m_pkContext])
        {
            [self release];
            return nil;
        }
    }
    return self;
}

- (void) layoutSubviews
{
    [EAGLContext setCurrentContext:m_pkContext];
    if(g_pRenderer->IsInited())
    {

        ((VeRendererGLES2*)g_pRenderer)->OnBufferRestorage();
    }
    else
    {
        g_pRenderer->Init();
        g_pApplication->Init();
    }
}

- (void)dealloc
{
    if(g_pRenderer->IsInited())
    {
        g_pApplication->Term();
        g_pRenderer->Term();
    }
    [EAGLContext setCurrentContext:nil];
    [m_pkContext release];
    [super dealloc];
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    VeUInt32 u32Time = g_pTime->GetTimeUInt();
    for(UITouch* touch in touches)
    {
        bool bNeedAppend = true;
        for(VeUInt32 i(0); i < s_kTouchCache.Size(); ++i)
        {
            VeVector2D& kVector = s_kTouchCache[i];
            if(kVector.x < 0)
            {
                VeInputManager::TouchPointParam* pkParam(NULL);
                g_pInputManager->GetSingle(pkParam);
                pkParam->x = [touch locationInView:self].x * self.contentScaleFactor;
                pkParam->y = [touch locationInView:self].y * self.contentScaleFactor;
                pkParam->id = i;
                g_pInputManager->AppendMessage(VeInputMessage::DT_TOUCH, VeInputMessage::TIT_BEGAN, u32Time, pkParam);
                kVector.x = pkParam->x;
                kVector.y = pkParam->y;
                bNeedAppend = false;
                break;
            }
        }
        if(bNeedAppend)
        {
            VeInputManager::TouchPointParam* pkParam(NULL);
            g_pInputManager->GetSingle(pkParam);
            pkParam->x = [touch locationInView:self].x * self.contentScaleFactor;
            pkParam->y = [touch locationInView:self].y * self.contentScaleFactor;
            pkParam->id = s_kTouchCache.Size();
            g_pInputManager->AppendMessage(VeInputMessage::DT_TOUCH, VeInputMessage::TIT_BEGAN, u32Time, pkParam);
            s_kTouchCache.PushBack(VeVector2D(pkParam->x, pkParam->y));
        }
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent *)event
{
    VeUInt32 u32Time = g_pTime->GetTimeUInt();
    VeVector<VeInputManager::TouchPointParam>* pkParams = g_pInputManager->GetList<VeInputManager::TouchPointParam>();
    pkParams->Clear();
    
    for(UITouch* touch in touches)
    {
        for(VeUInt32 i(0); i < s_kTouchCache.Size(); ++i)
        {
            VeVector2D& kVector = s_kTouchCache[i];
            if(VeAlmostEqual(kVector, VeVector2D([touch previousLocationInView:self].x * self.contentScaleFactor, [touch previousLocationInView:self].y * self.contentScaleFactor), 10 * self.contentScaleFactor))
            {
                VeInputManager::TouchPointParam kParam =
                {
                    [touch locationInView:self].x * self.contentScaleFactor,
                    [touch locationInView:self].y * self.contentScaleFactor,
                    i
                };
                kVector.x = kParam.x;
                kVector.y = kParam.y;
                pkParams->PushBack(kParam);
                break;
            }
        }
    }
    
    g_pInputManager->AppendMessage(VeInputMessage::DT_TOUCH, VeInputMessage::TIT_MOVED, u32Time, pkParams);
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    VeUInt32 u32Time = g_pTime->GetTimeUInt();
    for(UITouch* touch in touches)
    {
        for(VeUInt32 i(0); i < s_kTouchCache.Size(); ++i)
        {
            VeVector2D& kVector = s_kTouchCache[i];
            if(VeAlmostEqual(kVector, VeVector2D([touch previousLocationInView:self].x * self.contentScaleFactor, [touch previousLocationInView:self].y * self.contentScaleFactor), 10 * self.contentScaleFactor))
            {
                VeInputManager::TouchPointParam* pkParam(NULL);
                g_pInputManager->GetSingle(pkParam);
                pkParam->x = [touch locationInView:self].x * self.contentScaleFactor;
                pkParam->y = [touch locationInView:self].y * self.contentScaleFactor;
                pkParam->id = i;
                g_pInputManager->AppendMessage(VeInputMessage::DT_TOUCH, VeInputMessage::TIT_ENDED, u32Time, pkParam);
                kVector.x = -1;
                break;
            }
        }
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    VeUInt32 u32Time = g_pTime->GetTimeUInt();
    VeVector<VeInputManager::TouchPointParam>* pkParams = g_pInputManager->GetList<VeInputManager::TouchPointParam>();
    pkParams->Clear();
    
    for(UITouch* touch in touches)
    {
        for(VeUInt32 i(0); i < s_kTouchCache.Size(); ++i)
        {
            VeVector2D& kVector = s_kTouchCache[i];
            if(VeAlmostEqual(kVector, VeVector2D([touch previousLocationInView:self].x * self.contentScaleFactor, [touch previousLocationInView:self].y * self.contentScaleFactor), 10 * self.contentScaleFactor))
            {
                VeInputManager::TouchPointParam kParam =
                {
                    [touch locationInView:self].x * self.contentScaleFactor,
                    [touch locationInView:self].y * self.contentScaleFactor,
                    i
                };
                pkParams->PushBack(kParam);
                break;
            }
        }
    }
    
    s_kTouchCache.Clear();
    g_pInputManager->AppendMessage(VeInputMessage::DT_TOUCH, VeInputMessage::TIT_CANCELLED, u32Time, pkParams);
}

@end
