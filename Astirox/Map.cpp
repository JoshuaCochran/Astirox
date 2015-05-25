#include "stdafx.h"
#include "Map.hpp"
#include "Game.h"
#include "ServiceLocator.h"

using namespace luabridge;

Map::Map(std::string filename)
	: mapLoader("data/maps")
{
	//Pure tmx map method
	tmxFile = filename;
	if (!mapLoader.Load(tmxFile))
		std::cout << "Failed to load map " << tmxFile << std::endl;

	FOG_OF_WAR_HEIGHT = mapLoader.GetMapSize().y / 16;
	FOG_OF_WAR_WIDTH = mapLoader.GetMapSize().x / 16;


	fogTexture.loadFromFile("data/maps/fogTexture.png");
	for (int i = 0; i < (mapLoader.GetMapSize().x / 16 * mapLoader.GetMapSize().y / 16); i++)
	{
		s_FogOfWar temp;
		temp.sprite.setTexture(fogTexture);
		temp.IsVisible = false;
		temp.HasSeen = false;
		fogOfWar.push_back(temp);
	}
	int count = 0;
	for (int j = 0; j < (mapLoader.GetMapSize().x / 16); j++)
	{
		for (int k = 0; k < (mapLoader.GetMapSize().y / 16); k++)
		{
			fogOfWar[count].sprite.setPosition(j * 16, k * 16);
			count++;
		}
	}

	tileSelectTexture.loadFromFile("data/images/gui/selected.png");
	for (int i = 0; i < (mapLoader.GetMapSize().x / 16 * mapLoader.GetMapSize().y / 16); i++)
	{
		s_TileSelect temp;
		temp.sprite.setTexture(tileSelectTexture);
		tileSelect.push_back(temp);
	}

	count = 0;
	for (int j = 0; j < (mapLoader.GetMapSize().x / 16); j++)
	{
		for (int k = 0; k < (mapLoader.GetMapSize().y / 16); k++)
		{
			tileSelect[count].sprite.setPosition(j * 16, k * 16);
			count++;
		}
	}


	for (int j = 0; j < 48; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			tile_map[i][j].object_type = ".";
		}
	}
	for (auto layer = GetMapLoader().GetLayers().begin(); layer != GetMapLoader().GetLayers().end(); ++layer)
	{
		if (layer->name == "Collision")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				for (int i = 0; i < 64; i++)
				{
					for (int j = 0; j < 48; j++)
					{
						if (object->Contains(sf::Vector2f(i * 16, j * 16)))
						{
							tile_map[i][j].object_type = "#";
						}
					}
				}
			}
		}
		else if (layer->name == "Secret")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				for (int i = 0; i < 64; i++)
				{
					for (int j = 0; j < 48; j++)
					{
						if (object->Contains(sf::Vector2f(i * 16, j * 16)))
						{
							tile_map[i][j].object_type = "S";
						}
					}
				}
			}
		}
		else if (layer->name == "Portal")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				for (int i = 0; i < 64; i++)
				{
					for (int j = 0; j < 48; j++)
					{
						if (object->Contains(sf::Vector2f(i * 16, j * 16)))
						{
							tile_map[i][j].object_type = "P";
						}
					}
				}
			}
		}
		else if (layer->name == "Unpassable")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				for (int i = 0; i < 64; i++)
				{
					for (int j = 0; j < 48; j++)
					{
						if (object->Contains(sf::Vector2f(i * 16, j * 16)))
						{
							tile_map[i][j].object_type = ",";
						}
					}
				}
			}
		}
	}
	for (int j = 0; j < 48; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			std::cout << tile_map[i][j].object_type;
		}
		std::cout << "\n";
	}

	//Spawn Monsters
	sf::Vector2f spawnZone(16 * 4.0f, 16 * 4.0f);
	for (auto layer = GetMapLoader().GetLayers().begin(); layer != GetMapLoader().GetLayers().end(); ++layer)
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
					if (object->GetPropertyString("rarity") != "" && object->GetName() != "")
					{
						++mobMax;
						double rarity = boost::lexical_cast<int>(object->GetPropertyString("rarity"));

						CheckSpawnIntersection(object, rarity, checked);
						spawnCandidates.push_back(object->GetName());
						candidateRarities.push_back(boost::lexical_cast<double>(object->GetPropertyString("rarity")));
							
						for (int i = 0; i < candidateRarities.size(); i++)
						{
							candidateRarities[i] = candidateRarities[i] / rarity;
						}
							
						boost::random::discrete_distribution<> dist(candidateRarities.begin(), candidateRarities.end());
							
						std::string monsterToSpawn = spawnCandidates[dist(Game::rng)];
						Monster* mon = new Monster(monsterToSpawn);
						sf::Vector2f objectPos(object->GetPosition().x + 8.0f, object->GetPosition().y + 8.0f);
						mon->SetPosition(objectPos);
							
						for (int i = 0; i < spawnCandidates.size(); i++)
							spawnCandidates.pop_back();
						for (int i = 0; i < candidateRarities.size(); i++)
							candidateRarities.pop_back();

						tile_map[(int)mon->GetPosition().x / 16][(int)mon->GetPosition().y / 16].object_type = "m";
						tile_map[(int)mon->GetPosition().x / 16][(int)mon->GetPosition().y / 16].monster = mon;
						spawnedMonsters.push_back(mon);
					}
				}
			}
		}
	}


	count = 0;
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
	mapLoader.Load(tmxFile);
	
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
}

tmx::MapLoader& Map::GetMapLoader()
{
	return mapLoader;
}

bool Map::CheckSpawnIntersection(std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject>>::_Val_types>::_Myt>& mo, double& totalRarity, std::vector<std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject >> ::_Val_types>::_Myt >>& alreadyChecked)
{
	bool newIntersection = false;
	
	for (auto layer = GetMapLoader().GetLayers().begin(); layer != GetMapLoader().GetLayers().end(); ++layer)
	{
		if (layer->name == "MonsterSpawn")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				if (object->GetName() != "" && object->GetPropertyString("rarity") != "")
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
									newIntersection = false;
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
	}
	return true;
}

void Map::DrawAll(sf::RenderWindow& renderWindow)
{
	renderWindow.draw(mapLoader);
	sf::Rect<float> viewBox(sf::Vector2f(Game::view.getCenter().x - 128, Game::view.getCenter().y - 96), sf::Vector2f(256, 192));
	for (int i = 0; i < fogOfWar.size(); i++)
	{
		if (viewBox.intersects(fogOfWar[i].sprite.getGlobalBounds()))
		{
			if (!fogOfWar[i].IsVisible)
			{
				if (fogOfWar[i].HasSeen)
					fogOfWar[i].sprite.setColor(sf::Color(255, 255, 255, 255 / 2));
				else
					fogOfWar[i].sprite.setColor(sf::Color(255, 255, 255, 255));

				renderWindow.draw(fogOfWar[i].sprite);
			}
			else
			{
				for (int j = 0; j < spawnedMonsters.size(); j++)
				{

					if (fogOfWar[i].sprite.getGlobalBounds().intersects(spawnedMonsters[j]->GetBoundingRect()))
					{
						spawnedMonsters[j]->Draw(renderWindow);
					}
				}
				for (int j = 0; j < equipmentOnFloor.size(); j++)
				{
					if (fogOfWar[i].sprite.getGlobalBounds().intersects(equipmentOnFloor[j]->GetBoundingRect()))
					{
						equipmentOnFloor[j]->Draw(renderWindow);
					}
				}

				
				//The position of the mouse in pixels
				sf::Vector2i pixelPos = sf::Mouse::getPosition(renderWindow);

				//The position of the mouse converted to window coordinates adjusted to the view
				sf::Vector2f worldPos = renderWindow.mapPixelToCoords(pixelPos);

				worldPos.x = worldPos.x - ((int)worldPos.x % 16);
				worldPos.y = worldPos.y - ((int)worldPos.y % 16);

				draw_select_tile(worldPos.x, worldPos.y, renderWindow);
			}
		}
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
		//SpawnMonster();
		movesSinceSpawn = 0;
	}
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
	if (spawnedMonsters.size() < mobMax)
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

bool Map::UsePortal(Player& player, sf::Vector2f point)
{
	for (auto layer = GetMapLoader().GetLayers().begin(); layer != GetMapLoader().GetLayers().end(); ++layer)
	{
		if (layer->name == "Portal")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				if (object->GetName() != "" && object->GetPropertyString("Target X") != "" && object->GetPropertyString("Target Y") != "")
				{
					if (object->Contains(point))
					{
						if (object->GetName() == GetTMXFile())
						{
							float teleX = (boost::lexical_cast<float>(object->GetPropertyString("Target X")) * 16.0f) + 8.0f;
							float teleY = (boost::lexical_cast<float>(object->GetPropertyString("Target Y")) * 16.0f) + 8.0f;
							
							set_map(player.GetPosition(), ".");
							player.SetPosition(teleX, teleY);
							set_map(sf::Vector2f(teleX, teleY), "@");

							return true;
						}
						else
						{
							bool recentlyVisited = false;
							for (int i = 0; i < Game::recentlyVisitedMaps.size(); i++)
							{
								if (Game::recentlyVisitedMaps[i]->GetTMXFile() == object->GetName())
								{
									recentlyVisited = true;
									Game::currentMap = Game::recentlyVisitedMaps[i];
									float teleX = (boost::lexical_cast<float>(object->GetPropertyString("Target X")) * 16.0f) + 8.0f;
									float teleY = (boost::lexical_cast<float>(object->GetPropertyString("Target Y")) * 16.0f) + 8.0f;
									
									set_map(player.GetPosition(), ".");
									player.SetPosition(teleX, teleY);
									Game::currentMap->set_map(player.GetPosition(), "@");

									return true;
								}
							}
							if (!recentlyVisited)
							{
								Game::recentlyVisitedMaps.push_back(Game::currentMap);
								if (Game::recentlyVisitedMaps.size() > 4)
								{
									delete Game::recentlyVisitedMaps[0];
									Game::recentlyVisitedMaps.erase(Game::recentlyVisitedMaps.begin());
								}
								Game::currentMap = new Map(object->GetName());
								float teleX = (boost::lexical_cast<float>(object->GetPropertyString("Target X")) * 16.0f) + 8.0f;
								float teleY = (boost::lexical_cast<float>(object->GetPropertyString("Target Y")) * 16.0f) + 8.0f;
								
								set_map(player.GetPosition(), ".");
								player.SetPosition(teleX, teleY);
								Game::currentMap->set_map(player.GetPosition(), "@");

								return true;
							}
						}
					}
				}
			}
		}
	}
}

std::vector<Equipment*>& Map::GetEquipmentOnFloor()
{
	return equipmentOnFloor;
}

void Map::set_visible(unsigned int x, unsigned int y, bool visible)
{
	int xPos = x/16 * FOG_OF_WAR_HEIGHT;
	int yPos = (y/16) % FOG_OF_WAR_HEIGHT;
	fogOfWar[(xPos + yPos)].IsVisible = visible;
}

void Map::draw_select_tile(unsigned int x, unsigned int y, sf::RenderWindow& renderWindow)
{
	int xPos = x / 16 * FOG_OF_WAR_HEIGHT;
	int yPos = (y / 16) % FOG_OF_WAR_HEIGHT;
	if (xPos + yPos < tileSelect.size())
		renderWindow.draw(tileSelect[(xPos + yPos)].sprite);
}

void Map::set_map(sf::Vector2f point, std::string str)
{
	tile_map[(int)point.x / 16][(int)point.y / 16].object_type = str;
}

void Map::draw_map()
{
	for (int j = 0; j < 48; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			std::cout << tile_map[i][j].object_type;
		}
		std::cout << "\n";
	}
}

/*
	is_opaque checks if light can pass through the tile and shine on tiles behind it
	Input:
	x, type unsigned int, the x coordinate of the tile to check
	
	y, type unsigned int, the y coordinate of the tile to check
	Output:
	bool
*/
bool Map::is_opaque(unsigned int x, unsigned int y)
{
	//If the tile at x,y is a wall, denoted #, or a secret entrance, denoted S, return true
	if (tile_map[x / 16][y / 16].object_type == "#" || tile_map[x / 16][y / 16].object_type == "S")
		return true;
	else return false;
}

/*
	is_wall checks to see if entities, players and monsters, can move through the tile
	Input:
	point, type sf::Vector2f, the position of the tile to check
	Output:
	bool
*/
bool Map::is_wall(sf::Vector2f point)
{
	//If the tile at the x coord and y coord of point is an opaque wall, denoted #, or a non-opaque wall, denoted ',' , return true
	if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "#" 
		|| tile_map[(int)point.x / 16][(int)point.y / 16].object_type == ",")
		return true;
	else return false;
}

/*
	is_portal checks to see if the tile at the given position is a teleportor
	Input:
	point, type sf::Vector2f, the position of the tile to check
	Output:
	bool
*/
bool Map::is_portal(sf::Vector2f point)
{
	//If the tile at point.x,point.y is a portal, denoted P, return true
	if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "P")
		return true;
	else return false;
}

bool Map::is_monster(sf::Vector2f point)
{
	if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "m")
		return true;
	else return false;
}

bool Map::is_player(sf::Vector2f point)
{
	if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "@")
		return true;
	else return false;
}

Monster* Map::get_monster_at(sf::Vector2f point)
{
	return tile_map[(int)point.x / 16][(int)point.y / 16].monster;
}

int Map::get_fog_width() const
{
	return FOG_OF_WAR_WIDTH;
}

int Map::get_fog_height() const
{
	return FOG_OF_WAR_HEIGHT;
}

std::vector<s_FogOfWar>& Map::GetFogOfWar()
{
	return fogOfWar;
}

std::vector<s_TileSelect>& Map::GetTileSelect()
{
	return tileSelect;
}

void Map::cast_light(uint x, uint y, uint radius, uint row,
	float start_slope, float end_slope, uint xx, uint xy, uint yx,
	uint yy) {
	if (start_slope < end_slope) {
		return;
	}
	float next_start_slope = start_slope;
	for (uint i = row; i <= radius; i++) {
		bool blocked = false;
		for (int dx = -(int)(i), dy = -(int)(i); dx <= 0; dx++) {
			dx = (uint)dx;
			dy = (uint)dy;
			float l_slope = (dx - 0.5) / (dy + 0.5);
			float r_slope = (dx + 0.5) / (dy - 0.5);
			if (start_slope < r_slope) {
				continue;
			}
			else if (end_slope > l_slope) {
				break;
			}

			int sax = dx * xx + dy * xy;
			int say = dx * yx + dy * yy;
			if ((sax < 0 && (uint)std::abs(sax) > x) ||
				(say < 0 && (uint)std::abs(say) > y)) {
				continue;
			}
			uint ax = x + sax;
			uint ay = y + say;
			if (ax >= GetMapLoader().GetMapSize().x || ay >= GetMapLoader().GetMapSize().y
				|| ((ax % 16 != 0) && (ay % 16 != 0)))  {
				continue;
			}

			uint radius2 = radius * radius;
			if ((uint)(dx * dx + dy * dy) < radius2) {
				set_visible(ax, ay, true);
			}

			if (blocked) {
				if (is_opaque(ax, ay)) {
					next_start_slope = r_slope;
					continue;
				}
				else {
					blocked = false;
					start_slope = next_start_slope;
				}
			}
			else if (is_opaque(ax, ay)) {
				blocked = true;
				next_start_slope = r_slope;
				cast_light(x, y, radius, i + 1, start_slope, l_slope, xx,
					xy, yx, yy);
			}
		}
		if (blocked) {
			break;
		}
	}
}

void Map::do_fov(uint x, uint y, uint radius) {
	for (int i = 0; i < fogOfWar.size(); i++)
	{
		if (fogOfWar[i].IsVisible)
		{
			fogOfWar[i].HasSeen = true;
			fogOfWar[i].IsVisible = false;
		}
	}
	for (uint i = 0; i < 8; i++) {
		cast_light(x, y, radius, 1, 1.0, 0.0, multipliers[0][i],
			multipliers[1][i], multipliers[2][i], multipliers[3][i]);
	}
}