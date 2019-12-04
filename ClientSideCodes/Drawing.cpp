#include "Drawing.h"
#include <iostream>

//static std::vector<Tile> mySnake;
//static std::vector<Tile> eSnake;

//////////////////////////////////////////////////////////////////////////////////////////////////
//Redraws game window accordingly to board after update
void Drawing::drawGameArea(sf::RenderWindow *window, Tile board[MAX_X * MAX_Y], int score)
{

	int points;
	//int realscore = score-1;
	if (score == 0)
	{
		points = 0;
	}
	else
	{
		points = score - 1;
	}
	//draw score (not implemented fully, just increases when rady pressed)
	sf::Font font;
	if (!font.loadFromFile("sansation.ttf"))
	{
		printf("error when loading font.. ");
	}
	sf::Text text;		
	std::string scorestring("SCORE | " + std::to_string(points)); 
	text.setFont(font); 
	text.setString(scorestring);
	text.setFillColor(sf::Color::Red);
	text.setPosition(WINDOWSIZE_X * 0.4, 10);


	//players
	sf::RectangleShape drawpos; // simple rectangle shape 12 by 12 pixels
	drawpos.setSize(sf::Vector2f(TILESIZE, TILESIZE));
	drawpos.setOutlineColor(sf::Color::White);
	drawpos.setOutlineThickness(-1);

	sf::RectangleShape filler; // filling tile in 0,0 index (somehow always drawn as current player)
	filler.setSize(sf::Vector2f(TILESIZE, TILESIZE));
	filler.setOutlineColor(sf::Color::Black);
	filler.setFillColor(sf::Color::Black);
	filler.setPosition(0, 0);
	filler.setOutlineThickness(-1);

	for (auto index = 0; index < MAX_X * MAX_Y; index++)
	{
		auto x = board[index].x;
		auto y = board[index].y;

		//setting the draw position according to tilesize
		drawpos.setPosition(x * 12, y * 12);		
		
		//When player or enemy moves on board they change tile type accordingly so we can check 
		//not empty tiles and color them differently
	
		if (board[index].type == CURRENT_PLAYER)  
		{
			drawpos.setFillColor(sf::Color::Red);
			mySnake.push_back(board[index]);
		}
		else if (board[index].type == ANOTHER_PLAYER)
		{
			drawpos.setFillColor(sf::Color::Blue);
		}

		//endless snake draw calls
		window->draw(text);
		window->draw(drawpos);
		window->draw(filler);
		

		/////////////////////////////////////////////////////////////////////////
		////test for non endless snake
		//for (int i = 0; i < mySnake.size(); i++) 
		//{
		//	if (mySnake.size() > 5)
		//	{
		//		board[mySnake.front().x, mySnake.front().y].type == EMPTY;
		//		mySnake.erase(mySnake.begin(), mySnake.begin() + 1);				
		//	}
		//	drawpos.setPosition(mySnake.at(i).x * 12, mySnake.at(i).y * 12);
		//	window->draw(drawpos);
		//}
	}
}
