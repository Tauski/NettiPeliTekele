#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "ServerNet.h"
#include "SharedNet.h"

ServerNet::ServerNet(char *port)
{
	WSADATA wsaData; // init wsadata
	ListenSocket = INVALID_SOCKET; //validate sockets
	ClientSocket = INVALID_SOCKET; 
	struct addrinfo *result = nullptr, hints; 

	//setup winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	//set info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; // using tcp protocol
	hints.ai_flags = AI_PASSIVE;
		
	//get addr info
	iResult = getaddrinfo(nullptr, port, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	//create listening socket
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}
	
	//set socket as nonblocking, needed for multiple clients ready requests
	u_long iMode = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
	
	//Bind listening socket
	iResult = bind(ListenSocket, result->ai_addr, int(result->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	//free info
	freeaddrinfo(result);

	// start listening socket
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}
ServerNet::~ServerNet(void)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//accept connection and create clientsocket, check for client socket and add it into sessions
bool ServerNet::acceptNewClient(unsigned int &id)
{
	ClientSocket = accept(ListenSocket, nullptr, nullptr); 
	if (ClientSocket != INVALID_SOCKET)
	{
		currentClients.insert(std::pair<unsigned int, SOCKET>(id + 1, ClientSocket));
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Recieve data from socket
int ServerNet::receiveData(unsigned int client_id, char *recvbuf)
{
	if (currentClients.find(client_id) != currentClients.end())
	{
		SOCKET currentSocket = currentClients[client_id];
		iResult = NetworkServices::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);

		if (iResult == 0)
		{
			printf("Connection closed\n");
			closesocket(currentSocket);
			currentClients.erase(currentSocket);
		}
		return iResult;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Send packet to specific client
void ServerNet::sendClient(unsigned int id, char* packet, int totalSize)
{
	//get right socket from session map
	SOCKET socket = currentClients.at(id);
	int iSendResult = NetworkServices::sendMessage(socket, packet, totalSize);
	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
		currentClients.erase(socket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Send packet to all but one client
void ServerNet::sendAllClientsException(unsigned int id, char* packet, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter = currentClients.begin();
	int iSendResult;

	while (iter != currentClients.end())
	{
		if (iter->first == id)
		{
			++iter;
			continue;
		}

		currentSocket = iter->second;
		iSendResult = NetworkServices::sendMessage(currentSocket, packet, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
			iter = currentClients.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Send packet to all clients
void ServerNet::sendAllClients(char *packet, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter = currentClients.begin();
	int iSendResult;

	while (iter != currentClients.end())
	{
		currentSocket = iter->second;
		iSendResult = NetworkServices::sendMessage(currentSocket, packet, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
			iter = currentClients.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}