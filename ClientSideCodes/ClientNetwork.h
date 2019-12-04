#pragma once
#include <winsock2.h>
#include <string>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512 // buffer


class ClientNetwork
{
	int iResult;

public:
	SOCKET ConnectSocket; // socket for client to connect to server

	ClientNetwork(char *ip, char *port);
	~ClientNetwork();

	int receivePackets(char *);
	void displayError(char* message, int errorCode);
};
