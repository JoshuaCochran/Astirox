#ifndef _PLAYER_H
#define _PLAYER_H

#include "VisibleGameObject.h"
#include "Monster.h"
#include "Equipment.hpp"
#include "Map.hpp"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge.h>

using namespace luabridge;

class Map;


namespace ePlayerStats
{
	enum ePlayerStats
	{
		str, dex, intel, wis, stam, maxHP, curHP, maxMP, curMP, defense, res, pATT, rATT, mATT,
		ACC, avoid, CC, CHD, level, statPts, curXP, xpNeeded, gold, wdmg, enchant, speed
	};
}

namespace statModifiers
{
	enum statModifiers { str, dex, intel, wis, stam, def, res, hp, mp, wdmg, burned, drop, xp, jp };
}

namespace baseStats
{
	enum baseStats { str, dex, intel, wis, stam, wdmg };
}

class Equipment;

class Player :
	public VisibleGameObject
{
public:
	Player();
	~Player();

	void Update(float elapsedTime);
	void Draw(sf::RenderWindow& rw);
	void DrawStats(sf::RenderWindow& renderWindow);
	bool moveto(Map& map, sf::Event currentEvent);
	bool checkCollision(Map& map, float x, float y);

	void ReloadScripts();

	float GetVelocity() const;

	std::string GetName() const;

	int GetCurrentHP() const;
	int GetMaxHP() const;

	int GetCurrentMP() const;
	int GetMaxMP() const;

	int GetDefense() const;

	int GetCurrentXP() const;
	int GetXPNeeded() const;
	int GetLevel() const;

	int GetPhysAttDmg() const;

	void AddHP(int hp);

	void GainXP(int num);
	void LevelUp();
	void UpdateStats();
	void AddStat(int stat, int num);
	int GetStat(int stat) const;
	//int GetGearStat(int stat);
	
	int GetBaseStat(std::string stat);
	int GetGearStat(std::string stat);
	double GetStatMod(std::string stat);
	int test();

	void PickUpItem();
	void DropItem(Equipment& toBeDropped);
	std::vector<Equipment*> GetInventory();
	std::vector<Equipment*> GetEquipment();
	void EquipItem(Equipment& toBeEquipped);
	void UnequipItem(int pos);

	std::vector<std::function<std::string(Player&, Monster&)>>& GetSpellInventory();

private:
	float _velocity; //-- left ++ right
	float _maxVelocity;
	bool move(int x, int y);

	lua_State* player_lua_state;
	void CalculateStats();

	int stats[25];
	int baseStats[6];
	int gearStats[10];
	int bonusStats[18];
	double statModifiers[14];

	std::string name;

	std::vector<Equipment*> playerInventory;
	std::vector<Equipment*> playerEquipment;
	Equipment* playerWeapon;
	Equipment* playerShield;
	Equipment* playerArmor;
	Equipment* playerHelmet;
	Equipment* playerAccessory;

	std::vector<std::function<std::string(Player&, Monster&)>> spellInventory;

	bool can_wear_mail;
	bool can_wear_plate;
};

#endif