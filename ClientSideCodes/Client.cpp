
#include <process.h>
#include <iostream>
#include "ClientGame.h"
#include "Drawing.h"
#include "menu.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
//inits
bool selectionNotDone = true; //boolean to check when disable menu and draw game area
char *ip = "192.168.1.30"; //local ip of the server
char *port = "48000";	//according port of server
int score = 0;
ClientGame *client; // client
Tile board[MAX_X * MAX_Y]; // game area (same as servers)
sf::RenderWindow window(sf::VideoMode(1680, 720), "Menu", sf::Style::Close); //init window
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
//Redraws the window from menu window
void RedrawWindow(sf::RenderWindow *rWindow, Tile board[MAX_X * MAX_Y])
{
	if (client == nullptr) //client checker
	{
		std::cout << "client was nullptr" << std::endl;
		return;
	}

	while (rWindow->isOpen() && !selectionNotDone) //main loop
	{
		sf::Event event;
		while (rWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				rWindow->close();
			else if (event.type == sf::Event::KeyPressed) 
			{
				if (event.key.code == sf::Keyboard::Up)
				{
					client->sendActionPacket(UP);
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					client->sendActionPacket(DOWN);
				}
				else if (event.key.code == sf::Keyboard::Left)
				{
					client->sendActionPacket(LEFT);
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					client->sendActionPacket(RIGHT);
				}
				else if (event.key.code == sf::Keyboard::Space)
				{
					score += 1;
					client->sendReadyPacket();				
				}
				else if (event.key.code == sf::Keyboard::Escape)
				{
					PostQuitMessage(0);
				}
			}
		} 
		rWindow->clear(); //clear sfml window 		
		Drawing::drawGameArea(rWindow,board,score); //draw new game area with updated positions
		rWindow->display(); //display window
	} 
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Simple menu actions
namespace test 
{
	class startAction : public gmenu::Action
	{
	public:
		bool start() 
		{
			selectionNotDone = false; //selection is done so send this so menu closes
			client = new ClientGame(ip, port, &window); // create new client
			//client->sendReadyPacket();	//sending ready to server
			RedrawWindow(&window, board); //Draw game area
			
			return true;
		}
	};
	class ExitAction : public gmenu::Action
	{
	public:
		bool start()
		{
			selectionNotDone = false;
			return false;	//returning false closes the window
		}
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//Clients connection to server
void __cdecl connectionThread(void *args)
{
	while (true)
	{
		if (client != nullptr)
		{
			client->update(board);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//main that draws menu and handles input in menu
int main()
{

	client = nullptr;
	//Create thread to listen clients updates
	auto hThread1 = HANDLE(_beginthread(connectionThread, 0, nullptr));

	//Menu stuff
	std::vector<gmenu::MenuItem> itemList;
	std::string text[4] = { "Ready", "exit"};
	gmenu::MenuItem item;
	sf::Font font;
	font.loadFromFile("sansation.ttf");
	gmenu::Style style(font, font);
	style.layout = gmenu::Layout::TitleLeft | gmenu::Layout::ItemLeft;
	style.TitleColor = sf::Color::White;
	style.ItemColor = sf::Color::Cyan;
	style.Selected = sf::Color::Yellow;
	style.PaddingTitle.top = 25;
	style.PaddingItems.top = 30;
	style.PaddingItems.left = -100;

	for (int i = 0; i < 2; ++i)
	{
		item.title = text[i];
		if (i > 0 && selectionNotDone) 
		{
			item.action = std::make_shared <test::ExitAction>();
		}
		else
		{
			item.action = std::make_shared < test::startAction>();
		}
		itemList.push_back(item);
	}
	gmenu::Menu menu(window, "Menu", itemList, style);
	menu.createMenu();	
	return 0;
}



