#ifndef _MAP_HPP
#define _MAP_HPP

#include "Monster.h"
#include "Player.h"
#include "Equipment.hpp"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge.h>

struct TeleportInfo{
	sf::Vector2f exitPos;
	sf::Vector2f targetPos;
	std::string targetMap;
};

class Player;

class Map
{
public:
	Map();
	Map(std::string filename);
	~Map();

	void DrawAll(sf::RenderWindow& renderWindow);
	void UpdateMonsters();
	tmx::MapLoader* GetMapLoader();
	std::vector<Monster*>& GetSpawnedMonsters();

	int GetMovesSinceSpawn();
	void AddMove();

	std::vector<TeleportInfo>& GetPortals();
	void UsePortal(Player& player, TeleportInfo portal);

	int GetMaxMobs();
	void CheckSpawn();
	
private:
	std::string tmxFile; // e.g. map.tmx
	tmx::MapLoader* mapLoader;
	lua_State* map_lua_state;
	int mobMax;
	int movesSinceSpawn;

	std::vector<TeleportInfo> portals;
	
	std::vector<sf::Vector2f> spawnLocs;
	std::vector<std::string> spawnCandidates;
	std::vector<double> candidateRarities;
	std::vector<Monster*> spawnedMonsters;
	std::vector<Equipment*> equipmentOnFloor;

	void AddSpawnCandidate(std::string filename, double rarity);
	void SpawnMonster();

	void AddTargetMap(std::string filename);
	void AddExitPosX(int posx);
	void AddExitPosY(int posy);
	void AddTargetPosX(int posx);
	void AddTargetPosY(int posy);
};


#endif
