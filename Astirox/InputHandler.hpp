#ifndef _INPUTHANDLER_H
#define _INPUTHANDLER_H

#include "Command.hpp"

class InputHandler
{
public:
	~InputHandler();
	Command* handleInput(sf::Event currentEvent);//sf::RenderWindow renderWindow);
	void init();

private:
	int pickUpItemButton_;
	int reloadScriptsButton_;
	int moveLeftButton_;
	int moveRightButton_;
	int moveUpButton_;
	int moveDownButton_;

	Command* pickUpItem_;
	Command* reloadScripts_;
	Command* moveLeft_;
	Command* moveRight_;
	Command* moveUp_;
	Command* moveDown_;
	Command* clickToMove_;
};

#endif