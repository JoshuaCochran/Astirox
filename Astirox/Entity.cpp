#include "stdafx.h"
#include "Entity.h"
#include "Map.hpp"
#include "Game.h"

Entity::Entity()
{

}

Entity::~Entity()
{

}

int Entity::GetStat(int stat) const
{
	if ((sizeof(stats) / sizeof(stats[0])) >= stat)
		return stats[stat];
	else
		return 0;
}

std::string Entity::GetName() const
{
	return name;
}

int Entity::GetElementalReistance(int res) const
{
	return elementalresistances[res];
}

int Entity::GetAilmentResistance(int res) const
{
	return ailmentresistances[res];
}

void Entity::AddHP(int hp)
{
	stats[Stats::curHP] += hp;
	if (stats[Stats::curHP] <= 0)
		GetSprite().rotate(-90.0f);
	else GetSprite().setRotation(0);
}

void Entity::move(float x, float y)
{
	sf::Vector2f point(GetPosition().x + x, GetPosition().y + y);
	if (!this->collision(*Game::currentMap, point))
	{
		this->update_map_pos(GetPosition(), point);
		GetSprite().setPosition(point);
	}
}

bool Entity::collision(Map& map, sf::Vector2f point)
{
	if (map.is_wall(point) || map.is_player(point) || map.is_monster(point))
		return true;
	else
		return false;
}

void Entity::update_map_pos(sf::Vector2f previous, sf::Vector2f target)
{
	Game::currentMap->set_map(previous, ".");
	Game::currentMap->set_map(target, "?");
}

void Entity::update(Map& map)
{

}

void Entity::update(Map& map, sf::Event currentEvent)
{

}

bool Entity::isfriendly()
{
	return friendly;
}