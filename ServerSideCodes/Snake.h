#pragma once
//possible directions of the snake
enum DirectionEnum
{
	UP,
	DOWN,
	RIGHT,
	LEFT
};


//player position
struct Position
{
	unsigned char x;
	unsigned char y;

	Position()
	{
		this->x = 0;
		this->y = 0;
	}

	Position(unsigned char x, unsigned char y)
	{
		this->x = x;
		this->y = y;
	}
};



//simple player struct that hodls all data that the server needs of the player
struct Snake
{
	unsigned char id;
	bool is_ready;
	bool is_alive;
	Position position;
	DirectionEnum direction;

	Snake()
	{
		this->id = 0;
		this->is_ready = false;
		this->is_alive = false;
		this->direction = RIGHT;
	}

	Snake(unsigned char id, DirectionEnum direction, Position starting_position)
	{
		this->id = id;
		this->is_ready = false;
		this->is_alive = true;
		this->direction = direction;
		this->position = starting_position;
	}

	void move(unsigned char x, unsigned char y)
	{
		position.x += x;
		position.y += y;
	}
};