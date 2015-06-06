#ifndef _COMMAND_H
#define _COMMAND_H

#include "Player.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap) = 0;
private:
};

class MoveLeftCommand : public Command
{
public:
	virtual ~MoveLeftCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{
		entity.move(-16, 0);
	}
};

class MoveRightCommand : public Command
{
public:
	virtual ~MoveRightCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{
		entity.move(16, 0);
	}
};

class MoveUpCommand : public Command
{
public:
	virtual ~MoveUpCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{
		entity.move(0, -16);
	}
};

class MoveDownCommand : public Command
{
public:
	virtual ~MoveDownCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{
		entity.move(0, 16);
	}
};

class ReloadScriptsCommand : public Command
{
public:
	//virtual ~ReloadScriptsCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{
		Player* player = dynamic_cast<Player*>(&entity);
		if (player)
			player->ReloadScripts();
	}
};

class PickUpItemCommand : public Command
{
public:
	virtual ~PickUpItemCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{
		Player* player = dynamic_cast<Player*>(&entity);
		if (player)
		player->PickUpItem();
	}
};

class ClickToMoveCommand : public Command
{
	virtual ~ClickToMoveCommand() {}
	virtual void execute(Entity& entity, sf::RenderWindow& renderWindow, Map& currentMap)
	{

		//The position of the mouse in pixels
		sf::Vector2i pixelPos = sf::Mouse::getPosition(renderWindow);

		//The position of the mouse converted to window coordinates adjusted to the view
		sf::Vector2f worldPos = renderWindow.mapPixelToCoords(pixelPos);


		int xPos = 8 + (worldPos.x - ((int)worldPos.x % 16));
		int yPos = 8 + (worldPos.y - ((int)worldPos.y % 16));

		std::cout << "xPos: " << xPos << "\nyPos: " << yPos << "\n";

		currentMap.Astar(entity.GetPosition(), sf::Vector2f(xPos, yPos));
	}
};

#endif