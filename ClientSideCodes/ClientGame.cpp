#include <cstdio>
#include "ClientGame.h"
#include "SharedNet.h"

//construct client and connect it to network
ClientGame::ClientGame(char *ip, char *port,sf::RenderWindow *w)
{
	this->gWindow = w;
	tiles = 0;
	network = new ClientNetwork(ip, port);
}
ClientGame::~ClientGame()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Sending that we are ready 
void ClientGame::sendReadyPacket() const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = READY_PACKET;

	packet.serialize(packet_data);
	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Sending action event, which is in this case only direction change
void ClientGame::sendActionPacket(DirectionEnum direction) const
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;
	packet.data[0] = direction;

	packet.serialize(packet_data);
	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Update loop that handles incoming data from server
void ClientGame::update(Tile board[MAX_X * MAX_Y])
{
	Packet packet;
	int data_length = network->receivePackets(network_data);

	if (data_length <= 0)
	{
		return;
	}

	int i = 0;
	while (i < data_length)
	{
		packet.deserialize(&network_data[i]);
		i += sizeof(Packet);

		switch (packet.packet_type) {
		case INIT_PACKET:
			hInitPacket(packet.data, board);
			break;
		case UPDATE_PACKET:
			hUpdatePacket(packet.data, board);
			break;
		case RESTART_PACKET:
			hRestartPacket(packet.data, board);
			break;
		case NEW_PLAYER_CONNECTED:
			hNewPlayerPacket(packet.data, board);
			break;
		default:
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Recieved init package from server, so randomize boards starting positions
void ClientGame::hInitPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	int index = 0;
	client_id = data[index++];

	//generate board according to incoming data
	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];
		auto type = (id == client_id) ? CURRENT_PLAYER : ANOTHER_PLAYER;

		board[tiles++] = Tile(x, y, type); 
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Recieved movement updates so change board types
void ClientGame::hUpdatePacket(char data[], Tile board[MAX_X * MAX_Y])
{
	int index = 1;

	//generate board according to incoming data
	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];
		auto type = (id == client_id) ? CURRENT_PLAYER : ANOTHER_PLAYER;

		board[tiles++] = Tile(x, y, type);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Restart package recieved so clear board and randomize starting positions
void ClientGame::hRestartPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	int index = tiles = 0;
	memset(board, 0, sizeof(board[0]) * MAX_X * MAX_Y); // clear the board

	//generate new board according to incoming data (randomized starting positions)
	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];
		auto type = (id == client_id) ? CURRENT_PLAYER : ANOTHER_PLAYER;

		board[tiles++] = Tile(x, y, type);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//New player joined, add the new enemy to board
void ClientGame::hNewPlayerPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	char x = data[0];
	char y = data[1];

	board[tiles++] = Tile(x, y, ANOTHER_PLAYER);
}
