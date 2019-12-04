#include "menu.h"
#include <iostream>

namespace gmenu
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//set title of the menu
	void Menu::setTitle(std::string title)
	{
		menuTitle = title;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Set number of menu items
	void Menu::setMenuItems(std::vector<MenuItem> items)
	{
		menuItems = items;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Handle events and draw menu 
	void Menu::createMenu()
	{
		setMenu();
		bool cont = true;
		while (window.isOpen() && cont) // menu loop
		{
			sf::Event event;
			while (window.pollEvent(event)) 
			{
				if (event.type == sf::Event::Closed)
					window.close();
				else if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Up) 
					{
						currently_selected_item = (currently_selected_item + menuItems.size() - 1) % (menuItems.size());
					}
					else if (event.key.code == sf::Keyboard::Down)
					{
						currently_selected_item = (currently_selected_item + 1) % (menuItems.size());
					}
					else if (event.key.code == sf::Keyboard::Return)
					{
						cont = menuItems.at(currently_selected_item).action->start();
					}
				}
			} 
			window.clear();
			drawMenu();
			window.display();
		} 
	} 

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Handle menu texts 
	void Menu::writeText(std::string str, sf::Font font, unsigned int size, float x, float y, const sf::Color &color)
	{
		sf::Text text;
		text.setString(str);
		text.setFont(font);
		text.setFillColor(color);
		text.setCharacterSize(size);
		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.width / 2.0f, 0);
		//handle text positions and paddings
		if (x - textRect.width / 2.0f < 0) 
		{
			x = textRect.width / 2 + style.PaddingTitle.left;
		}
		if (x + textRect.width / 2.0f > window.getSize().x) 
		{
			x = window.getSize().x - textRect.width / 2 + style.PaddingTitle.left;
		}
		text.setPosition(sf::Vector2f(x, y));
		window.draw(text);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Handle menu spacings and layouts
	void Menu::setMenu()
	{
		{
			float offset_coefficient = 0.5;
			if (style.layout & Layout::TitleCentre) offset_coefficient = 0.5;
			else if (style.layout & Layout::TitleLeft) offset_coefficient = 0.25;
			else if (style.layout & Layout::TitleRight) offset_coefficient = 0.75;
			float x = (float)window.getSize().x * offset_coefficient, y = style.PaddingTitle.top;
			title_location.x = (x + style.PaddingTitle.left);
			title_location.y = y;
			std::cout << "title_location:" << title_location.x << " " << title_location.y << offset_coefficient << std::endl;
		}

		unsigned int menu_screen_height = (int)window.getSize().y - title_location.y + style.PaddingItems.top;
		std::cout << "Screen height" << menu_screen_height << std::endl;
		unsigned int block_height = (int)menu_screen_height / menuItems.size() * style.MenuItemScaleFactor;

		float offset_coefficient = 0.5;
		if (style.layout & Layout::ItemCentre) offset_coefficient = 0.5;
		else if (style.layout & Layout::ItemLeft) offset_coefficient = 0.25;
		else if (style.layout & Layout::ItemRight) offset_coefficient = 0.75;

		float x = (float)window.getSize().x * offset_coefficient + style.PaddingItems.left;
		float y = ((float)window.getSize().y) - 0.75 * menu_screen_height + block_height * 1 / 8;
		
		for (int8_t i = 0; i < menuItems.size(); ++i)
		{
			coordinates crd;
			crd.x = x;
			crd.y = y;
			item_location.push_back(crd); // each items coordinates
			y += block_height;
		}
	} 

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Draw everything
	void Menu::drawMenu() {
		writeText(menuTitle, style.ItemFont, style.TitleFontSize, title_location.x, title_location.y, style.TitleColor);
		sf::Color color = style.ItemColor;
		for (int i = 0; i < menuItems.size(); ++i)
		{
			if (i == currently_selected_item)
			{
				color = style.Selected;
			}
			writeText(menuItems[i].title, style.ItemFont, style.ItemFontSize, item_location[i].x, item_location[i].y, color);
			color = style.ItemColor;
		}
	}
}