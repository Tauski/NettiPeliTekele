#include <process.h>
#include "ServerGameHandler.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
//inits
ServerGameHandler *server;
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//thread to get new connections
void  __cdecl getNewClientsThread(void *args)
{
	while (true)
	{
		server->getNewClients();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//thread to receive data from clients
void  __cdecl receiveFromClientsThread(void *args)
{
	while (true)
	{
		server->receiveFromClients();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//main server loop
void gameLoop(void *args)
{
	float tickLength = 120;
	while (true)
	{
		Sleep(tickLength);
		tickLength *= 0.999; // illusion of game getting faster
		server->update();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//start server and threads
int main(int argc, char *argv[])
{
	printf("I'm server ! \n");

	if (argc < 2) server = new ServerGameHandler("48000");
	else server = new ServerGameHandler(argv[1]);
	//set new threads for client handlers
	auto hThread1 = HANDLE(_beginthread(getNewClientsThread, 0, nullptr));
	auto hThread2 = HANDLE(_beginthread(receiveFromClientsThread, 0, nullptr));
	gameLoop(nullptr);
}