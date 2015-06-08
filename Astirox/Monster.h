#ifndef _MONSTER_H
#define _MONSTER_H

#include "Entity.h"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge.h>

using namespace luabridge;

class Entity;

class Monster :
	public Entity
{
public:
	Monster();
	Monster(std::string filename);
	~Monster();

	void Update(Map& map);
	void Move();
	bool checkCollision(Map& map, float x, float y);

	const static int AGGRO_RADIUS = 96; // 6 Character lengths - ?Make variable later?

	int GetSizeInParty() const;

	sf::Sprite& GetSelectedSprite();


	//Player enchant effects
	void SetBurn(bool yesno);
	void BurnTic();
	bool IsBurning();
	int GetBurnTime();

private:
	int sizeInParty;

	void update_map_pos(sf::Vector2f previous, sf::Vector2f target);

	sf::Sprite selectedSprite;
};


#endif