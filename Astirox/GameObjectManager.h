#ifndef _GAMEOBJECTMANAGER_H
#define _GAMEOBJECTMANAGER_H

#include "Monster.h"

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Add(std::string name, VisibleGameObject* gameObject);
	void Remove(std::string name);
	void RemoveMonster(Monster& monster);
	int GetObjectCount() const;
	VisibleGameObject* Get(std::string name) const;
	std::string GetName(VisibleGameObject* gameObject);

	void DrawAll(sf::RenderWindow& renderWindow);
	void UpdateAll();

private:
	std::map<std::string, VisibleGameObject*> _gameObjects;

	sf::Clock clock;

	struct GameObjectDeallocator
	{
		void operator() (const std::pair<std::string, VisibleGameObject*> & p) const
		{
			delete p.second;
		}
	};
};

#endif