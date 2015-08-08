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
#import "_VeRendererGLES2.h"

VeEAGLView* g_pkView = NULL;

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

@end
