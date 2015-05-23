#ifndef _COMMAND_H
#define _COMMAND_H

#include "Player.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(Entity& entity) = 0;
private:
};

class MoveLeftCommand : public Command
{
public:
	virtual ~MoveLeftCommand() {}
	virtual void execute(Entity& entity)
	{
		entity.move(-16, 0);
	}
};

class MoveRightCommand : public Command
{
public:
	virtual ~MoveRightCommand() {}
	virtual void execute(Entity& entity)
	{
		entity.move(16, 0);
	}
};

class MoveUpCommand : public Command
{
public:
	virtual ~MoveUpCommand() {}
	virtual void execute(Entity& entity)
	{
		entity.move(0, -16);
	}
};

class MoveDownCommand : public Command
{
public:
	virtual ~MoveDownCommand() {}
	virtual void execute(Entity& entity)
	{
		entity.move(0, 16);
	}
};

class ReloadScriptsCommand : public Command
{
public:
	//virtual ~ReloadScriptsCommand() {}
	virtual void execute(Entity& entity)
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
	virtual void execute(Entity& entity)
	{
		Player* player = dynamic_cast<Player*>(&entity);
		if (player)
		player->PickUpItem();
	}
};

#endif