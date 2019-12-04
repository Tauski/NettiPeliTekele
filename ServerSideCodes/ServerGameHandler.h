#pragma once
#include <ctime>
#include "ServerNet.h"
#include "SharedNet.h" //using the same networking class as client
#include "Snake.h"


#define MAX_PLAYERS 4
// game board size in tiles
#define MAX_X 139   // one less than client side to allow for simple collision detection
#define MAX_Y 59	// one less than client side to allow for simple collision detection


class ServerGameHandler
{
	ServerNet* network;
	bool game_started;
	
	unsigned int currentPlayers;	
	unsigned int readyPlayers;
	unsigned int alivePlayers;

	char network_data[MAX_PACKET_SIZE]; //buffer

	Snake snakes[MAX_PLAYERS]; //4 max players
	bool board[MAX_X][MAX_Y];

public:
	explicit ServerGameHandler(char *port);
	~ServerGameHandler();

	void update();
	
	//threaded
	void getNewClients();
	void receiveFromClients();

private:
	void restartGame();
	void initializePlayer(unsigned char id);
	void movePlayers();

	//senders
	void sUpdatePacket() const;
	void sRestartPacket() const;
	void sInitPacket(unsigned char id) const;
	void sNewPlayerPacket(unsigned char id) const;

	//creators
	void cRestartPacket(char packet_data[]) const;
	void cPacketWithPositions(unsigned char id, char packet_data[]) const;
	void cNewPlayerPacket(unsigned char id, char packet_data[]) const;

	//handlers of incoming packets
	void handleReadyPacket(unsigned char id);
	void handleActionPacket(unsigned char id, int direction);

};