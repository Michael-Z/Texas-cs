#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "Venus3D.h"
#include "TexasPoker.h"

USING_NS_CC;

VeChar8 g_acLanguage[128] = ZH_CN;

// uncomment below line, open debug console
// #define USE_WIN32_CONSOLE

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef USE_WIN32_CONSOLE
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

	VeChar8 acBuffer[VE_MAX_PATH_LEN];
	VeStrcpy(acBuffer, VE_MAX_PATH_LEN, lpCmdLine);
	VeChar8* pcContext;
	VeChar8* pcTemp = VeStrtok(acBuffer, " ", &pcContext);
	if(VeStrcmp(pcContext, "") != 0)
	{
		VeStrcpy(g_acLanguage, 128, pcContext);
	}
	VeUInt32 u32Width(640), u32Height(960);
	if(pcTemp)
	{
		VeStrcpy(acBuffer, VE_MAX_PATH_LEN, pcTemp);
		pcTemp = VeStrtok(acBuffer, "x", &pcContext);
		if(pcTemp)
		{
			u32Width = VeAtoi(pcTemp);
			pcTemp = VeStrtok(NULL, "x", &pcContext);
			if(pcTemp)
				u32Height = VeAtoi(pcTemp);
		}
	}

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setFrameSize(u32Width, u32Height);

    int ret = CCApplication::sharedApplication()->run();

#ifdef USE_WIN32_CONSOLE
    FreeConsole();
#endif

    return ret;
}

void GetPurchaseList(TexasPoker::PurchaseInfoList& kList)
{
	kList.Resize(5);

	kList[0].m_kInfo = TT("MM_BuyPackage0");
	kList[0].m_u32Price = 200;
	kList[0].m_eType = PURCHASE_TYPE_RMB;
	kList[0].m_u32Chips = 10000;
	kList[0].m_u32Icon = 0;
	kList[0].m_bVip = false;

	kList[1].m_kInfo = TT("MM_BuyPackage1");
	kList[1].m_u32Price = 500;
	kList[1].m_eType = PURCHASE_TYPE_RMB;
	kList[1].m_u32Chips = 30000;
	kList[1].m_u32Icon = 1;
	kList[1].m_bVip = false;

	kList[2].m_kInfo = TT("MM_BuyPackage2");
	kList[2].m_u32Price = 1000;
	kList[2].m_eType = PURCHASE_TYPE_RMB;
	kList[2].m_u32Chips = 70000;
	kList[2].m_u32Icon = 2;
	kList[2].m_bVip = true;

	kList[3].m_kInfo = TT("MM_BuyPackage3");
	kList[3].m_u32Price = 2000;
	kList[3].m_eType = PURCHASE_TYPE_RMB;
	kList[3].m_u32Chips = 160000;
	kList[3].m_u32Icon = 3;
	kList[3].m_bVip = true;

	kList[4].m_kInfo = TT("MM_BuyPackage4");
	kList[4].m_u32Price = 3000;
	kList[4].m_eType = PURCHASE_TYPE_RMB;
	kList[4].m_u32Chips = 270000;
	kList[4].m_u32Icon = 4;
	kList[4].m_bVip = true;
}

const VeChar8* GetChannel()
{
	return "PC_TEST";
}

bool ShowWall()
{
	return false;
}
