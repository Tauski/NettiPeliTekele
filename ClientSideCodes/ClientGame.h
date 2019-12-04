#pragma once
#include "ClientNetwork.h"
#include "SharedNet.h"
#include "Drawing.h"

//possible directions of the snake
enum DirectionEnum
{
	UP,
	DOWN,
	RIGHT,
	LEFT
};


class ClientGame
{
	sf::RenderWindow *gWindow; //game window
	unsigned int tiles;	//number of tiles on board
	unsigned int client_id;
	ClientNetwork* network;
	char network_data[MAX_PACKET_SIZE]; // data buffer

public:
	ClientGame(char *ip, char *port, sf::RenderWindow * w); //client game created with server ip, port and sf::renderwindow
	~ClientGame();

	void sendReadyPacket() const;							
	void sendActionPacket(DirectionEnum direction) const;
	void update(Tile board[MAX_X * MAX_Y]); // update loop handles incoming data

private:
	void hInitPacket(char data[], Tile board[MAX_X * MAX_Y]);
	void hUpdatePacket(char data[], Tile board[MAX_X * MAX_Y]);
	void hRestartPacket(char data[], Tile board[MAX_X * MAX_Y]);
	void hNewPlayerPacket(char data[], Tile board[MAX_X * MAX_Y]);
};