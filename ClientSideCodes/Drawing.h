#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "SFML/Graphics.hpp"

// window size in pixels
#define WINDOWSIZE_X 1680
#define WINDOWSIZE_Y 720
// game board size in tiles
#define MAX_X 140
#define MAX_Y 60
// tile size in pixels
#define TILESIZE 12

//Type indicators for board tiles to differiate players
enum TileTypeEnum
{
	EMPTY,
	CURRENT_PLAYER, 
	ANOTHER_PLAYER	
};

//Tiles of wich the board will be combined
struct Tile
{
	unsigned char x;
	unsigned char y;
	TileTypeEnum type;

	Tile()
	{
		this->x = 0;
		this->y = 0;
		this->type = EMPTY;
	}

	Tile(unsigned char x, unsigned char y, TileTypeEnum type)
	{
		this->x = x;
		this->y = y;
		this->type = type;
	}
};

//Handles drawing the game area
class Drawing 
{
public:
	static void drawGameArea(sf::RenderWindow *window, Tile board[MAX_X * MAX_Y],int score);
};
