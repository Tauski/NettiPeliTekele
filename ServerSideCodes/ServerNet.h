#pragma once
#include <winsock2.h>
#include <map>
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 512 //buffer


class ServerNet
{
	int iResult; 
	//listening socket and clients socket
	SOCKET ListenSocket;
	SOCKET ClientSocket; 

public:	
	explicit ServerNet(char *port);
	~ServerNet(void);

	//array of connected clients
	std::map<unsigned int, SOCKET> currentClients; 

	void sendAllClients(char *packet, int size);
	void sendAllClientsException(unsigned int cID, char *packet, int size);
	void sendClient(unsigned int cID, char *packet, int size);
	
	int receiveData(unsigned int cID, char *buffer);
	bool acceptNewClient(unsigned int &id);

};