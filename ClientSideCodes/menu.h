#include <SFML/Graphics.hpp>
#include <memory>
namespace gmenu 
{

	//Abstract action class that is used in client.cpp
	class Action 
	{
	public:
		virtual bool start() = 0;
	};

	//Each menu item has its own action
	struct MenuItem 
	{
		std::shared_ptr<Action> action;
		std::string title;
	};

	//Layouts
	enum Layout 
	{
		TitleCentre = 1 << 0,
		TitleRight = 1 << 1,
		TitleLeft = 1 << 2,
		ItemCentre = 1 << 3,
		ItemRight = 1 << 4,
		ItemLeft = 1 << 5,
		Default = TitleCentre 
	};
	//styles
	struct Style 
	{
		sf::Font &TitleFont;
		sf::Font &ItemFont;

		sf::Color TitleColor = sf::Color::Green;;
		sf::Color ItemColor = sf::Color::Red;
		sf::Color Selected = sf::Color::Blue;

		unsigned int TitleFontSize = 50;
		unsigned int ItemFontSize = 20;

		float MenuTitleScaleFactor = 1.25;
		float MenuItemScaleFactor = 0.25;

		struct 
		{
			signed int top, left;
		} PaddingItems, PaddingTitle;

		int layout = Layout::Default;
		Style(sf::Font &mf, sf::Font &itmf) :
			TitleFont(mf), ItemFont(itmf), PaddingTitle{ 10,0 }, PaddingItems{ 0,0 }
		{
		}
	};
	   	 
	//simple menu class
	class Menu 
	{
	public:
		Menu(sf::RenderWindow &wnd, std::string title, std::vector<MenuItem> items, Style &st) :
			style(st),
			window(wnd)
		{
			menuTitle = title;
			menuItems = items;
		}
		void createMenu();
		void setMenuItems(std::vector<MenuItem>);
		void setTitle(std::string title);

	private:

		void writeText(std::string string, sf::Font font, unsigned int size, float x, float y,
			const sf::Color &color);
		void setMenu();
		void drawMenu();

		//Menu selection items
		std::vector<MenuItem> menuItems;
		//Selection items position
		struct coordinates
		{
			float x = 0;
			float y = 0;
		}  title_location;
		std::vector<coordinates> item_location;

		//currently higlighted selection
		int currently_selected_item = 0;


		Style &style;
		sf::RenderWindow &window;
		std::string menuTitle;
	};
}
