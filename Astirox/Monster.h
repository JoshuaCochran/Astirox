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

namespace eMonsterStats
{
	enum eMonsterStats 
	{
		defense, maxHP, curHP, maxMP, curMP, pATT, mATT, ACC, 
		avoid, CC, CHD, level, wdmg, baseXP, baseGold, boss, miniboss, speed
	};
}

namespace eElementalResistances
{
	enum eElementalResistances
	{
		Physical, Fire, Ice, Lightning, Earth, Air, Holy, Dark
	};
}

namespace eAilmentResistances
{
	enum eAilmentResistances
	{
		Poison, Blind, Silence, Sleep, Paralysis, Confuse, Charm, Burn, Frozen, Stun
	};
}

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
	int GetCurrentMP();
	int GetMaxMP();
	int GetXPReward();
	int GetGoldReward();
	int GetLevel();
	int GetMinDmg();
	int GetMaxDmg();
	int GetDefense();
	int GetStat(int stat) const;
	std::string GetName();

	int GetSizeInParty() const;

	sf::Sprite& GetSelectedSprite();

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
	int stats[17];
	int elementalresistances[7];
	int ailmentresistances[9];
	int sizeInParty;

	sf::Sprite selectedSprite;

	double statModifiers[14];

	std::vector<int> loot;

	bool miniBoss;
	bool mainBoss;

	bool burn;
	int burnTime;
};


#endif