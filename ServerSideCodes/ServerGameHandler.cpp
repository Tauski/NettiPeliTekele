#include "ServerGameHandler.h"


ServerGameHandler::ServerGameHandler(char *port)
{
	game_started = false;
	currentPlayers = 0;
	readyPlayers = 0;
	alivePlayers = 0;
	network = new ServerNet(port);
}
ServerGameHandler::~ServerGameHandler()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Update loop, first moves players serverside then sends new positions to all clients
//Also restarts game if only 1 snake is left
void ServerGameHandler::update()
{
	if (game_started && alivePlayers < 2) restartGame();

	if (!game_started) return;

	movePlayers();
	sUpdatePacket();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//New client has joined (thread handled)
void ServerGameHandler::getNewClients()
{
	if (game_started)
	{
		return;
	}
	if (currentPlayers < MAX_PLAYERS && network->acceptNewClient(currentPlayers))
	{
		initializePlayer(currentPlayers++);
		sInitPacket(currentPlayers);
		sNewPlayerPacket(currentPlayers);
		printf("client %d has connected to the server\n", currentPlayers);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Packet came from client (thread handled)
void ServerGameHandler::receiveFromClients()
{
	Packet packet;
	std::map<unsigned int, SOCKET>::iterator iter;
	auto localSessionsCopy = network->currentClients;

	for (iter = localSessionsCopy.begin(); iter != localSessionsCopy.end(); ++iter) //looping through all clients
	{
		int data_length = network->receiveData(iter->first, network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < data_length)
		{
			packet.deserialize(&network_data[i]);
			i += sizeof(Packet);

			switch (packet.packet_type) {
			case ACTION_EVENT:
				handleActionPacket(iter->first - 1, network_data[4]);
				break;
			case READY_PACKET:
				handleReadyPacket(iter->first - 1);
				break;
			default:
				printf("error in packet types\n");
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Resets player positions and clears the board, then sends the packet to clients
void ServerGameHandler::restartGame()
{
	game_started = false;
	readyPlayers = alivePlayers = 0;
	memset(board, 0, sizeof(board[0][0]) * MAX_X * MAX_Y); // clear the board

	for (int id = 0; id < currentPlayers; id++)
	{
		initializePlayer(id);
	}

	sRestartPacket();
	printf("game has been restarted\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Handles player position movements and collision checks
void ServerGameHandler::movePlayers()
{
	for (int id = 0; id < currentPlayers; id++)
	{
		if (!snakes[id].is_alive) continue;

		auto pos = snakes[id].position;

		switch (snakes[id].direction)
		{
		case UP:
			if (pos.y != 0 && !board[pos.x][pos.y - 1])
			{
				snakes[id].move(0, -1);
				board[pos.x][pos.y - 1] = true;
				continue;
			}
			break;
		case RIGHT:
			if (pos.x != MAX_X && !board[pos.x + 1][pos.y])
			{
				snakes[id].move(1, 0);
				board[pos.x + 1][pos.y] = true;
				continue;
			}
			break;
		case DOWN:
			if (pos.y != MAX_Y - 1 && !board[pos.x][pos.y + 1])
			{
				snakes[id].move(0, 1);
				board[pos.x][pos.y + 1] = true;
				continue;
			}
			break;
		case LEFT:
			if (pos.x != 0 && !board[pos.x - 1][pos.y])
			{
				snakes[id].move(-1, 0);
				board[pos.x - 1][pos.y] = true;
				continue;
			}
			break;
		default:
			continue;
		}

		//player not moving = player dead
		alivePlayers--;
		snakes[id].is_alive = false;
		printf("client %d was killed at %d, %d\n", id + 1, snakes[id].position.x, snakes[id].position.y);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Ready recieved from client, if that client was already ready unready it, else increase ready count
//if all players ready start game
void ServerGameHandler::handleReadyPacket(unsigned char id)
{
	if (game_started) return; //no rdy packets if game is running

	
	if (snakes[id].is_ready) // if already ready, set false
	{
		readyPlayers--;
		snakes[id].is_ready = false;
	}
	else
	{
		readyPlayers++;
		snakes[id].is_ready = true;
	}

	printf("client %d changed state to %s\n", id + 1, snakes[id].is_ready ? "ready" : "not ready");

	if (currentPlayers > 1 && readyPlayers == currentPlayers ) // all ready start game
	{
		game_started = true;
		printf("All ready, game start \n");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Action packet recieved from client, (up,right,down,left) change player direction accordingly
void ServerGameHandler::handleActionPacket(unsigned char id, int direction)
{
	auto prevDirection = snakes[id].direction;
	auto curDirection = DirectionEnum(direction);

	if (prevDirection == curDirection) return; 

	switch (curDirection)
	{
	case UP:
		if (prevDirection == DOWN) return;
		break;
	case RIGHT:
		if (prevDirection == LEFT) return;
		break;
	case DOWN:
		if (prevDirection == UP) return;
		break;
	case LEFT:
		if (prevDirection == RIGHT) return;
		break;
	default:
		break;
	}
	snakes[id].direction = curDirection;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Create player 
void ServerGameHandler::initializePlayer(unsigned char id)
{
	//Randomize start position and start direction
	auto dir = DirectionEnum(rand() % LEFT);
	unsigned char x = MAX_X / 5 + rand() % MAX_X / 5 + 2 * (id % 2) * MAX_X / 5;  //position is within split game area to devoid chance of spawning 2 players on 1 tile
	unsigned char y = MAX_Y / 5 + rand() % MAX_Y / 5;
	if (id >= 2) y += 2 * MAX_Y / 5; // if more than 2 players split screen also horizontally

	//add new player to players
	Snake s = Snake(id + 1, dir, Position(x, y));
	snakes[id] = s;
	board[x][y] = true;
	alivePlayers++;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Sends serverside update packet to all clients, comes with position data
void ServerGameHandler::sUpdatePacket() const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = UPDATE_PACKET;
	cPacketWithPositions(0, packet.data);

	packet.serialize(packet_data);
	network->sendAllClients(packet_data, packet_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Sends server side restart call to all clients, comes with reseted player positions
void ServerGameHandler::sRestartPacket() const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = RESTART_PACKET;
	cRestartPacket(packet.data);

	packet.serialize(packet_data);
	network->sendAllClients(packet_data, packet_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Sends new players initial packet to the recently joined, comes with all players position
void ServerGameHandler::sInitPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_PACKET;
	cPacketWithPositions(id, packet.data);

	packet.serialize(packet_data);
	network->sendClient(id, packet_data, packet_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Sends new players initial packet to all alrady joined clients, comes with new players position
void ServerGameHandler::sNewPlayerPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = NEW_PLAYER_CONNECTED;
	cNewPlayerPacket(id, packet.data);

	packet.serialize(packet_data);
	network->sendAllClientsException(id, packet_data, packet_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Making restart packet, data filled with new player positions
void ServerGameHandler::cRestartPacket(char packet_data[]) const
{
	int index = 0;

	// store data about each player's position
	for (unsigned int i = 0; i < currentPlayers; i++)
	{
		packet_data[index++] = snakes[i].id;
		packet_data[index++] = snakes[i].position.x;
		packet_data[index++] = snakes[i].position.y;
	}

	// add two -1s at the end for easier parsing on client side
	packet_data[index + 1] = packet_data[index] = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Making packet with all player positions. 
void ServerGameHandler::cPacketWithPositions(unsigned char id, char packet_data[]) const
{
	int index = 0;

	packet_data[index++] = id; // identifier to wich client has this position

	//insert position to packet_data array
	for (unsigned int i = 0; i < currentPlayers; i++)
	{
		packet_data[index++] = snakes[i].id;
		packet_data[index++] = snakes[i].position.x;
		packet_data[index++] = snakes[i].position.y;
	}
	packet_data[index + 1] = packet_data[index] = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Making packet with a new player.
void ServerGameHandler::cNewPlayerPacket(unsigned char id, char packet_data[]) const
{
	packet_data[0] = snakes[id - 1].position.x;
	packet_data[1] = snakes[id - 1].position.y;
}