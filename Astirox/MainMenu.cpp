#include "stdafx.h"
#include "MainMenu.h"
#include "ServiceLocator.h"

MainMenu::MenuResult MainMenu::Show(sf::RenderWindow& window)
{

	//Load menu image from file
	sf::Texture image;
	image.loadFromFile("data/images/menus/mainmenu.png");
	sf::Sprite sprite(image);

	//Setup clickable regions

	//Play menu item coordinates
	MenuItem playButton;
	playButton.rect.top = 145;
	playButton.rect.height = 235;
	playButton.rect.left = 0;
	playButton.rect.width = 1023;
	playButton.action = Play;

	//Exit menu item coordinates;
	MenuItem exitButton;
	exitButton.rect.left = 0;
	exitButton.rect.width = 1023;
	exitButton.rect.top = 383;
	exitButton.rect.height = 177;
	exitButton.action = Exit;

	_menuItems.push_back(playButton);
	_menuItems.push_back(exitButton);

	window.draw(sprite);
	window.display();

	return GetMenuResponse(window);
}

MainMenu::MenuResult MainMenu::HandleClick(int x, int y)
{
	std::list <MenuItem>::iterator it;

	for (it = _menuItems.begin(); it != _menuItems.end(); it++)
	{
		sf::Rect<int> menuItemRect = (*it).rect;
		/*if (menuItemRect.contains(sf::Vector2<int>(x, y)))
		{
		return (*it).action;
		}*/
		if (x > menuItemRect.left
			&& x < menuItemRect.left + menuItemRect.width
			&& y > menuItemRect.top
			&& y < menuItemRect.height + menuItemRect.top)
		{
			return (*it).action;
		}
	}

	return Nothing;
}


MainMenu::MenuResult MainMenu::GetMenuResponse(sf::RenderWindow& window)
{
	sf::Event menuEvent;

	while (true)
	{
		while (window.pollEvent(menuEvent))
		{
			if (menuEvent.type == sf::Event::MouseButtonPressed)
			{
				if (ServiceLocator::GetAudio()->IsSongPlaying())
					ServiceLocator::GetAudio()->StopAllSounds();

				return HandleClick(menuEvent.mouseButton.x, menuEvent.mouseButton.y);
			}
			if (menuEvent.type == sf::Event::Closed)
			{
				return Exit;
			}
		}
	}
}