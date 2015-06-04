#include "stdafx.h"
#include "InputHandler.hpp"

InputHandler::~InputHandler()
{
	delete moveLeft_;
	delete moveRight_;
	delete moveDown_;
	delete moveUp_;
	delete pickUpItem_;
	delete reloadScripts_;
}

Command* InputHandler::handleInput(sf::Event currentEvent)//sf::RenderWindow renderWindow)
{
	if (currentEvent.type == sf::Event::KeyReleased)
	{
		if (currentEvent.key.code == pickUpItemButton_)
			return pickUpItem_;
		else if (currentEvent.key.code == reloadScriptsButton_)
			return reloadScripts_;
		else if (currentEvent.key.code == moveLeftButton_)
			return moveLeft_;
		else if (currentEvent.key.code == moveRightButton_)
			return moveRight_;
		else if (currentEvent.key.code == moveUpButton_)
			return moveUp_;
		else if (currentEvent.key.code == moveDownButton_)
			return moveDown_;
		else
			return NULL;
	}
	else if (currentEvent.type == sf::Event::MouseButtonReleased)
	{
		return clickToMove_;
	}
	else
		return NULL;
}

void InputHandler::init()
{
	pickUpItemButton_ = sf::Keyboard::Z;
	reloadScriptsButton_ = sf::Keyboard::F5;
	moveLeftButton_ = sf::Keyboard::Left;
	moveRightButton_ = sf::Keyboard::Right;
	moveUpButton_ = sf::Keyboard::Up;
	moveDownButton_ = sf::Keyboard::Down;

	moveLeft_ = new MoveLeftCommand();
	moveRight_ = new MoveRightCommand();
	moveUp_ = new MoveUpCommand();
	moveDown_ = new MoveDownCommand();
	pickUpItem_ = new PickUpItemCommand();
	reloadScripts_ = new ReloadScriptsCommand();
	clickToMove_ = new ClickToMoveCommand();
}