#include "Client.h"

VeInt32 main()
{
	VeClientInit();
	Connection* pkClient = VE_NEW Connection("TestLink");

	g_pClient->Connect("TestLink", "127.0.0.1", 6688, "password");

	while(1)
	{
		g_pClient->Update();
	}

	VeClientTerm();
	return 0;
}
