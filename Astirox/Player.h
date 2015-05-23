#ifndef _PLAYER_H
#define _PLAYER_H

#include "Entity.h"
#include "ServiceLocator.h"
#include "Equipment.hpp"
#include "Monster.h"
#include "Battle.h"
#include "Map.hpp"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge.h>

using namespace luabridge;

class Map;
class Equipment;

namespace statModifiers
{
	enum statModifiers { str, dex, intel, wis, stam, def, res, hp, mp, wdmg, burned, drop, xp, jp };
}

namespace baseStats
{
	enum baseStats { str, dex, intel, wis, stam, wdmg };
}

class Player : public Entity
{
public:
	Player();
	~Player();

	void Update(Map& map, sf::Event currentEvent);
	void DrawStats(sf::RenderWindow& renderWindow);

	void ReloadScripts();

	void GainXP(int num);
	virtual void LevelUp();
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

	std::vector<std::function<std::string(Entity&, Entity&)>>& GetSpellInventory();

private:
	bool move(int x, int y);

	void CalculateStats();

	int baseStats[6];
	int gearStats[10];
	int bonusStats[18];
	double statModifiers[14];

	std::vector<Equipment*> playerInventory;
	std::vector<Equipment*> playerEquipment;

	std::vector<std::function<std::string(Entity&, Entity&)>> spellInventory;

	bool can_wear_mail;
	bool can_wear_plate;

	virtual void update_map_pos(sf::Vector2f previous, sf::Vector2f target);
	virtual bool collision(Map& map, sf::Vector2f point);
};

#endif