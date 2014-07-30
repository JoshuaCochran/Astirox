#include "stdafx.h"
#include "Map.hpp"
#include "Game.h"
#include "ServiceLocator.h"

using namespace luabridge;

Map::Map()
{

}

Map::Map(std::string filename)
{
	map_lua_state = luaL_newstate();
	luaL_openlibs(map_lua_state);
	int ret = luaL_dofile(map_lua_state, ("data/scripts/maps/" + filename).c_str());
	if (ret != 0)
	{
		printf("Error occurs when calling luaL_dofile() Hint Machine 0x%x\n", ret);
		printf("Error: %s", lua_tostring(map_lua_state, -1));
	}
	ret = luaL_dofile(map_lua_state, "data/scripts/maps/maps.lua");
	if (ret != 0)
	{
		printf("Error occurs when calling luaL_dofile() Hint Machine 0x%x\n", ret);
		printf("Error: %s", lua_tostring(map_lua_state, -1));
	}
	lua_pcall(map_lua_state, 0, 0, 0);

	tmxFile = getGlobal(map_lua_state, "mapFile").cast<std::string>();
	mapLoader = new tmx::MapLoader("data/maps");
	mapLoader->Load(tmxFile);
	
	mobMax = getGlobal(map_lua_state, "mobMax").cast<int>();

	getGlobalNamespace(map_lua_state)
		.beginClass<Map>("Map")
			.addFunction("AddSpawnCandidate", &Map::AddSpawnCandidate)
			.addFunction("AddTargetMap", &Map::AddTargetMap)
			.addFunction("AddExitPosX", &Map::AddExitPosX)
			.addFunction("AddExitPosY", &Map::AddExitPosY)
			.addFunction("AddTargetPosX", &Map::AddTargetPosX)
			.addFunction("AddTargetPosY", &Map::AddTargetPosY)
		.endClass();

	push(map_lua_state, this);
	lua_setglobal(map_lua_state, "map");

	LuaRef initSpawn = getGlobal(map_lua_state, "initSpawn");
	initSpawn();
	for (int i = 0; i < mobMax; i++)
		SpawnMonster();

	LuaRef initPortals = getGlobal(map_lua_state, "initPortals");
	initPortals();
	for (int i = 0; i < portals.size(); i++)
	{
		std::cout << "Portal " << i << ":\n\n"
			<< portals[i].exitPos.x << std::endl
			<< portals[i].exitPos.y << std::endl
			<< portals[i].targetMap << std::endl
			<< portals[i].targetPos.x << std::endl
			<< portals[i].targetPos.y << std::endl;
	}

	//movesSinceSpawn = 0;	
}

Map::~Map()
{
	for (int i = 0; i < spawnedMonsters.size(); i++)
		delete spawnedMonsters[i];
	for (int i = 0; i < equipmentOnFloor.size(); i++)
		delete equipmentOnFloor[i];
	delete map_lua_state;
	delete mapLoader;
}

tmx::MapLoader* Map::GetMapLoader()
{
	return mapLoader;
}

void Map::DrawAll(sf::RenderWindow& renderWindow)
{
	renderWindow.draw(*mapLoader);
	for (int i = 0; i < equipmentOnFloor.size(); i++)
	{
		equipmentOnFloor[i]->Draw(renderWindow);
	}
	for (int i = 0; i < spawnedMonsters.size(); i++)
	{
		spawnedMonsters[i]->Draw(renderWindow);
	}
}

void Map::UpdateMonsters()
{
	for (int i = 0; i < spawnedMonsters.size(); i++)
		spawnedMonsters[i]->Update(*this);
}

void Map::AddSpawnCandidate(std::string filename, double rarity)
{
	spawnCandidates.push_back(filename + ".lua");
	candidateRarities.push_back(rarity);
}
void Map::CheckSpawn()
{
	if (spawnedMonsters.size() < mobMax && movesSinceSpawn >= 20)
	{
		SpawnMonster();
		movesSinceSpawn = 0;
	}
}
void Map::SpawnMonster()
{
	boost::random::discrete_distribution<> dist(candidateRarities.begin(), candidateRarities.end());
	std::string monsterToSpawn = spawnCandidates[dist(Game::rng)];
	Monster* mon = new Monster(monsterToSpawn);
	spawnedMonsters.push_back(mon);
	
	boost::random::uniform_int_distribution<> spawnLocChooser(1, mobMax);
	std::stringstream ss;
	int mobLocation = spawnLocChooser(Game::rng);
	ss << "spawn" << mobLocation;
	std::cout << ss.str();
	LuaRef spawnLocTable = getGlobal(map_lua_state, "spawnLocs")[ss.str()];
	//int posx = spawnLocTable["x"];
	mon->SetPosition(sf::Vector2f(spawnLocTable["x"].cast<int>(), spawnLocTable["y"].cast<int>()));
}

std::vector<Monster*>& Map::GetSpawnedMonsters()
{
	return spawnedMonsters;
}

int Map::GetMaxMobs()
{
	return mobMax;
}

int Map::GetMovesSinceSpawn()
{
	return movesSinceSpawn;
}

void Map::AddMove()
{
	movesSinceSpawn += 1;
}

std::vector<TeleportInfo>& Map::GetPortals()
{
	return portals;
}

void Map::AddTargetMap(std::string filename)
{
	std::stringstream ss;
	ss << filename << ".lua";
	TeleportInfo portal1;
	portal1.targetMap = ss.str();
	portals.push_back(portal1);
}

void Map::AddExitPosX(int posx)
{
	portals[portals.size() - 1].exitPos.x = posx;
}

void Map::AddExitPosY(int posy)
{
	portals[portals.size() - 1].exitPos.y = posy;
}
void Map::AddTargetPosX(int posx)
{
	portals[portals.size() - 1].targetPos.x = posx;
}
void Map::AddTargetPosY(int posy)
{
	portals[portals.size() - 1].targetPos.y = posy;
}

void Map::UsePortal(Player& player, TeleportInfo portal)
{
	delete Game::currentMap;
	Game::currentMap = new Map(portal.targetMap);
	player.SetPosition(portal.targetPos);
}