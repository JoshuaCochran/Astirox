#ifndef _MONSTER_H
#define _MONSTER_H

#include "Player.h"
#include "Map.hpp"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge.h>

using namespace luabridge;

class Monster :
	public VisibleGameObject
{
public:
	Monster();
	Monster(std::string filename);
	~Monster();

	void Update(Map& map);
	void Draw(sf::RenderWindow& rw);
	void Move();
	bool checkCollision(Map& map, float x, float y);

	float GetVelocity() const;

	const static int AGGRO_RADIUS = 96; // 6 Character lengths - ?Make variable later?

	int GetCurrentHP();
	int GetMaxHP();
	int GetMP();
	int GetXPReward();
	int GetGoldReward();
	int GetLevel();
	int GetMinDmg();
	int GetMaxDmg();
	int GetDefense();
	std::string GetName();

	bool IsMiniBoss();
	bool IsMainBoss();

	void AddHP(int hp);
	//void SpawnMonster(Player player);


	//Player enchant effects
	void SetBurn(bool yesno);
	void BurnTic();
	bool IsBurning();
	int GetBurnTime();

private:
	float _velocity;
	float _maxVelocity;
	
	sf::Vector2f pos;

	lua_State* monster_lua_state;

	std::string name;
	int def;
	int level;
	int minDmg;
	int maxDmg;
	int currentHP;
	int maxHP;
	int MP;
	int goldReward;
	int xpReward;

	double statModifiers[14];

	std::vector<int> loot;

	bool miniBoss;
	bool mainBoss;

	bool burn;
	int burnTime;
};


#endif