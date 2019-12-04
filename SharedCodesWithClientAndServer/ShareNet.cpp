#include "SharedNet.h"

//sends data to connected socket
int NetworkServices::sendMessage(SOCKET curSocket, char * message, int messageSize)
{
	return send(curSocket, message, messageSize, 0);
}

//recieves data from connected socket
int NetworkServices::receiveMessage(SOCKET curSocket, char * buffer, int bufSize)
{
	return recv(curSocket, buffer, bufSize, 0);
}
