#include "TexasPokerHallServer.h"

//--------------------------------------------------------------------------
VeInt32 main()
{
	printf("================================================================================");
	printf("=                        Venus Interactive Entertainment                       =");
	printf("=                                                                              =");
	printf("=                      Texas Poker Hall Server Ver 0.0.0.1                     =");
	printf("=                                  LoginServer                                 =");
	printf("=                                                                              =");
	printf("=                             Archived With Venus_Neo                          =");
	printf("=                                                                              =");
	printf("=                       CopyRight (C)  VenusIE  2012-2018                      =");
	printf("================================================================================");
	VeServerInit();
	TexasPokerHallServer* pkServer = VE_NEW TexasPokerHallServer();
	g_pServerManager->Start();
	VeServerTerm();
	return 0;
}
//--------------------------------------------------------------------------
