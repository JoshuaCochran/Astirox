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
	//Pure tmx map method

	tmxFile = filename;
	mapLoader = new tmx::MapLoader("data/maps");
	mapLoader->Load(tmxFile);

	sf::Vector2f spawnZone(16 * 4.0f, 16 * 4.0f);
	for (auto layer = GetMapLoader()->GetLayers().begin(); layer != GetMapLoader()->GetLayers().end(); ++layer)
	{
		if (layer->name == "MonsterSpawn")
		{
			std::vector<std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject>>::_Val_types>::_Myt>> checked;
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{ 
				bool monsterInSpawnZone = false;
				sf::Rect<float> spawnZone(sf::Vector2f(object->GetPosition().x - 32, object->GetPosition().y - 32), sf::Vector2f(80, 80));
				for (int i = 0; i < spawnedMonsters.size(); i++)
				{
					sf::Rect<float> mBB = spawnedMonsters[i]->GetBoundingRect();
					if (mBB.intersects(spawnZone))
					{
						monsterInSpawnZone = true;
					}
				}
				if (!monsterInSpawnZone)
				{
					int rarity = boost::lexical_cast<int>(object->GetPropertyString("rarity"));
					spawnCandidates.push_back(object->GetName());
					CheckSpawnIntersection(object, rarity, checked);
					candidateRarities.push_back(boost::lexical_cast<int>(object->GetPropertyString("rarity")) / rarity);
					boost::random::discrete_distribution<> dist(candidateRarities.begin(), candidateRarities.end());
					std::string monsterToSpawn = spawnCandidates[dist(Game::rng)];
					Monster* mon = new Monster(monsterToSpawn);
					sf::Vector2f objectPos(object->GetPosition().x + 8.0f, object->GetPosition().y + 8.0f);
					mon->SetPosition(objectPos);
					spawnedMonsters.push_back(mon);

					for (int i = 0; i < spawnCandidates.size(); i++)
						spawnCandidates.pop_back();
					for (int i = 0; i < candidateRarities.size(); i++)
						candidateRarities.pop_back();
				}
			}
		}
	}

	//Lua map method

	/*map_lua_state = luaL_newstate();
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

	//movesSinceSpawn = 0;*/
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

bool Map::CheckSpawnIntersection(std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject>>::_Val_types>::_Myt> mo, int& totalRarity, std::vector<std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject >> ::_Val_types>::_Myt >> &alreadyChecked)
{
	bool newIntersection = false;
	for (auto layer = GetMapLoader()->GetLayers().begin(); layer != GetMapLoader()->GetLayers().end(); ++layer)
	{
		if (layer->name == "MonsterSpawn")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				if (object->Contains(mo->GetPosition()) && mo != object)
				{
					if (alreadyChecked.size() == 0) newIntersection = true;
					else
					{
						for (int i = 0; i < alreadyChecked.size(); i++)
						{
							if (object == alreadyChecked[i])
							{
								newIntersection = true;
							}
						}
					}
				}
				if (newIntersection)
				{
					spawnCandidates.push_back(object->GetName());
					candidateRarities.push_back(boost::lexical_cast<int>(object->GetPropertyString("rarity")));
					alreadyChecked.push_back(object);
					totalRarity += boost::lexical_cast<int>(object->GetPropertyString("rarity"));
					if (alreadyChecked.size() < layer->objects.size()) CheckSpawnIntersection(mo, totalRarity, alreadyChecked);
				}
			}
		}
	}
	return true;
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
	/*if (spawnedMonsters.size() < mobMax && movesSinceSpawn >= 20)
	{
		SpawnMonster();
		movesSinceSpawn = 0;
	}*/
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
std::string Map::GetTMXFile()
{
	return tmxFile;
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

std::vector<Equipment*>& Map::GetEquipmentOnFloor()
{
	return equipmentOnFloor;
}