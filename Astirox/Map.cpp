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

	MAP_SIZE_IN_TILES = (mapLoader.GetMapSize().x / 16) * (mapLoader.GetMapSize().y / 16);
	MAP_HEIGHT_IN_TILES = mapLoader.GetMapSize().y / 16;
	MAP_WIDTH_IN_TILES = mapLoader.GetMapSize().x / 16;

	fogTexture.loadFromFile("data/maps/fogTexture.png");
	for (int i = 0; i < (MAP_WIDTH_IN_TILES * MAP_HEIGHT_IN_TILES); i++)
	{
		s_FogOfWar temp;
		temp.sprite.setTexture(fogTexture);
		temp.IsVisible = false;
		temp.HasSeen = false;
		fogOfWar.push_back(temp);
	}
	int count = 0;
	for (int j = 0; j < (MAP_WIDTH_IN_TILES); j++)
	{
		for (int k = 0; k < (MAP_HEIGHT_IN_TILES); k++)
		{
			fogOfWar[count].sprite.setPosition(j * 16, k * 16);
			count++;
		}
	}

	tileSelectTexture.loadFromFile("data/images/gui/selected.png");
	for (int i = 0; i < (MAP_WIDTH_IN_TILES * MAP_HEIGHT_IN_TILES); i++)
	{
		s_TileSelect temp;
		temp.sprite.setTexture(tileSelectTexture);
		tileSelect.push_back(temp);
	}

	count = 0;
	for (int j = 0; j < (MAP_WIDTH_IN_TILES); j++)
	{
		for (int k = 0; k < (MAP_HEIGHT_IN_TILES); k++)
		{
			tileSelect[count].sprite.setPosition(j * 16, k * 16);
			count++;
		}
	}


	/*for (int j = 0; j < 48; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			tile_map[i][j].object_type = ".";
			
			tile_map[i][j].on_closed = false;
			tile_map[i][j].on_open = false;
			tile_map[i][j].on_path = false;
		}
	}//*/
	tile_map = new TilemapObject*[MAP_WIDTH_IN_TILES];
	for (int i = 0; i < MAP_WIDTH_IN_TILES; i++)
		tile_map[i] = new TilemapObject[MAP_HEIGHT_IN_TILES];

	for (int j = 0; j < MAP_HEIGHT_IN_TILES; j++)
	{
		for (int i = 0; i < MAP_WIDTH_IN_TILES; i++)
		{
			tile_map[i][j].object_type = ".";

			tile_map[i][j].on_closed = false;
			tile_map[i][j].on_open = false;
			tile_map[i][j].on_path = false;
		}
	}//*/
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
	for (int j = 0; j < MAP_HEIGHT_IN_TILES; j++)
	{
		for (int i = 0; i < MAP_WIDTH_IN_TILES; i++)
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

				
				/*//The position of the mouse in pixels
				sf::Vector2i pixelPos = sf::Mouse::getPosition(renderWindow);

				//The position of the mouse converted to window coordinates adjusted to the view
				sf::Vector2f worldPos = renderWindow.mapPixelToCoords(pixelPos);

				worldPos.x = worldPos.x - ((int)worldPos.x % 16);
				worldPos.y = worldPos.y - ((int)worldPos.y % 16);

				draw_select_tile(worldPos.x, worldPos.y, renderWindow);//*/
			}
		}
	}
	for (int j = 0; j < 48; j++)
	{
		for (int i = 0; i < 64; i++)
		{


			if (tile_map[i][j].on_path)
			{
				draw_select_tile(i * 16, j * 16, renderWindow);
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
	if ((((int)point.x / 16) < MAP_WIDTH_IN_TILES) && ((int)point.y / 16) < MAP_HEIGHT_IN_TILES
		&& point.x > 0 && point.y > 0)
	{
		//if (((int)point.x / 16)*((int)point.y / 16) 
		//If the tile at the x coord and y coord of point is an opaque wall, denoted #, or a non-opaque wall, denoted ',' , return true
		if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "#"
			|| tile_map[(int)point.x / 16][(int)point.y / 16].object_type == ",")
			return true;
	}
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
	if ((((int)point.x / 16) < MAP_WIDTH_IN_TILES) && ((int)point.y / 16) < MAP_HEIGHT_IN_TILES
		&& point.x > 0 && point.y > 0)
	{
		//If the tile at point.x,point.y is a portal, denoted P, return true
		if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "P")
			return true;
	}
	else return false;
}

bool Map::is_monster(sf::Vector2f point)
{
	if ((((int)point.x / 16) < MAP_WIDTH_IN_TILES) && ((int)point.y / 16) < MAP_HEIGHT_IN_TILES
		&& point.x > 0 && point.y > 0)
	{
		if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "m")
			return true;
	}
	else return false;
}

bool Map::is_player(sf::Vector2f point)
{
	if ((((int)point.x / 16) < MAP_WIDTH_IN_TILES) && ((int)point.y / 16) < MAP_HEIGHT_IN_TILES
		&& point.x > 0 && point.y > 0)
	{
		if (tile_map[(int)point.x / 16][(int)point.y / 16].object_type == "@")
			return true;
	}
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

/*
	Astar uses the A* algorithm to find the shortest path between two tiles
	Input:
	start, sf::Vector2f, the position of the starting tile
	
	goal, sf::Vector2f, the position of the goal, or target, tile
	Output:
	None
*/
void Map::Astar(sf::Vector2f start, sf::Vector2f goal)
{
	resetPlayerPath();

	if (is_wall(goal))
		return;

	//The set of checked tiles
	std::vector <path_element*> closed_set;
	//The set of tiles that have been encountered but not yet thoroughly checked
	std::vector<path_element*> open_set;

	//Initializing the first path_element at the start locatation
	path_element* beginning = new path_element;
	
	//The path_element's g_cost 
	//is the number of tiles you would have to move through from the starting tile to reach this tile times 10
	beginning->g_cost = 0;

	//To get a path_element's h_cost 
	//use the manhattan method to estimate the g_cost of moving from the current tile to the goal
	beginning->h_cost = heuristic_cost_estimate(start, goal);
	
	//The path_element's f_cost is the sum of it's g_cost and h_cost
	beginning->f_cost = beginning->g_cost + beginning->h_cost;

	//A path_element's parent is the tile adjacent to it 
	//with the lowest f_score
	beginning->parent = NULL;

	//A path_element's pos is it's position
	beginning->pos = start;

	//Add the starting path_element to the closed set
	set_open(beginning->pos, true);
	open_set.push_back(beginning);
	
	//Set the current path_element to the starting tile's path_element
	path_element* current = beginning;

	do
	{
		//Current lowest is smallest f_cost value of path_elements in the open set
		//Current lowest is initialized to the first element in the open set
		int current_lowest = open_set[0]->f_cost;

		//Navigate through the open set comparing the f_cost of each element
		//and storing the smallest value
		//The element with the smallest f_cost becomes the new current
		for (int i = 0; i < open_set.size(); i++)
		{
			//If the ith element of open set has a f_cost smaller than the current smallest
			if (open_set[i]->f_cost <= current_lowest)
			{
				//Change current lowest to the f_cost of the ith element
				current_lowest = open_set[i]->f_cost;
				//Make current point to the ith element of the open set
				current = open_set[i];
			}
		}

		//If the position of current path_element is equal to the goal
		//The goal has been reached, so exit the while loop
		if (current->pos == goal)
			break;

		//Remove the current path element from the open set and add it to the closed set
		for (int i = 0; i < open_set.size(); i++)
		{
			//If two path_elements have the same position they are the same path_element
			//So to find the current path_element in the open set compare their pos value
			if (open_set[i]->pos == current->pos)
			{
				//Once current is found in the open set add it to the closed set
				closed_set.push_back(open_set[i]);
				set_closed(open_set[i]->pos, true);

				//And remove it from the open set
				set_open(open_set[i]->pos, false);
				open_set.erase(open_set.begin() + i);
			}
		}

		//The meat of the algorithm
		//Constructs path_elements for the tiles to the top, bottom, left, and right of current
		for (int i = 0; i < 4; i++)
		{
			//Initializes a new path_element
			path_element* temp = new path_element;
			
			//Sets the new path_element's position to current's position
			temp->pos = current->pos;

			//Makes the new path_element's parent current
			temp->parent = current;

			//If i is 0, make the position of the path_element to the right of current
			if (i == 0)
				temp->pos.x += 16;
			//If i is 1, make the position of the path_element to the left of current
			else if (i == 1)
				temp->pos.x -= 16;
			//If i is 2, make the position of the path_element equal to the tile below current
			else if (i == 2)
				temp->pos.y += 16;
			//If is 3, make the position of the path_element equal to the tile above current
			else if (i == 3)
				temp->pos.y -= 16;
			//In this way you create a path element for each tile adjacent to the current tile

			//The g_cost of a new tile is equal to the g_cost of it's parent + 10
			//Essentially counts the number of tiles it takes to walk from the start to this tile
			temp->g_cost = current->g_cost + 10;
			//Use the manhattan method to estimate the cost to get from this new path_element to the goal
			temp->h_cost = heuristic_cost_estimate(temp->pos, goal);
			//Calculate the f_cost of this new path_element
			temp->f_cost = temp->g_cost + temp->h_cost;

			//If the temp path_element is walkable and not already in the closed set
			if (!is_wall(temp->pos) && !on_closed(temp->pos))
			{
				//and the tile is in the open set
				if (on_open(temp->pos))
				{
					//Navigate through the open set
					for (int z = 0; z < open_set.size(); z++)
					{
						//Until you find the path_element equal to the temp path_element
						if (open_set[z]->pos == temp->pos)
						{
							//Delete the temp path_element because there is no need for duplicates
							delete temp;
							temp = NULL;
							//Then set temp to equal the path_element in the open set that had the same position
							temp = open_set[z];

							//Exit the loop
							break;
						}
					}

					//If the g_cost of the temp path_element is greater than the g_cost of current + 10
					//In other words if it takes fewer tiles to get to the temp path_element from the current path element
					//Than from the previous path to the temp tile
					if (temp->g_cost > current->g_cost + 10)
					{
						//Set temp's parent to current
						temp->parent = current;

						//Recalculate the g_cost, h_cost, and f_cost of temp from current
						temp->g_cost = current->g_cost + 10;
						temp->h_cost = heuristic_cost_estimate(temp->pos, goal);
						temp->f_cost = temp->g_cost + temp->h_cost;
					}
				}
				//If the temp path_element is not in the open set
				else
				{
					//Add temp to the open set
					set_open(temp->pos,true);
					open_set.push_back(temp);
				}
			}
			//If the tile at temp's position is not walkable or temp is already on the closed list
			else
			{
				//Delete the temp tile and move on
				delete temp;
				temp = NULL;
			}
		}
	} while (open_set.size() != 0);//Run the loop while the open set is not empty
									//If the open_set is ever empty then there exists no path to the target tile

	//If the open set is not empty than a path was found
	if (!open_set.empty())
	{
		//So reconstruct the shortest path found
		//By navigating from current back through it's parents until you reach the starting tile
		path_element* nav = current;
		//Exits when the starting tile is reached
		while (nav != NULL)
		{
			//Mark the nav tile as being on the path from start to goal
			set_on_path(nav->pos, true);
			//Add the nav path_element to the Game::player_path stack
			Game::player_path.push(nav);
			//Make nav point to its parent
			nav = nav->parent;
		}
	}

	//Empties the open_set vector and deletes its elements
	while (!open_set.empty())
	{
		//If the last element in the open_set vector is not NULL and is not on the found path
		if (open_set.back() != NULL && !on_path(open_set.back()->pos))
		{
			//Delete the path_element
			set_open(open_set.back()->pos, false);
			delete open_set.back();
		}
		//Then remove it from the open set
		open_set.pop_back();
	}

	//Empties the closed_set vector and deletes its elements
	while (!closed_set.empty())
	{
		//If the last element in the closed_set vector is not NULL and is not on the found path
		if (closed_set.back() != NULL && !on_path(closed_set.back()->pos))
		{
			//Delete the path_element
			set_closed(closed_set.back()->pos, false);
			delete closed_set.back();
		}
		//Then remove it from the closed set
		closed_set.pop_back();
	}
}

/*
	heuristic_cost_estimate runs a heuristic to estimate the cost of moving
	from a given tile to another tile
	Input:
	start, sf::Vector2f, the locatation of the starting tile

	goal, sf::Vector2f, the location of the target tile
	Output:
	(integer) The result of the heuristic.

*/
int Map::heuristic_cost_estimate(sf::Vector2f start, sf::Vector2f goal)
{
	//Manhattan method
	//std::cout << 10 * (abs(start.x - goal.x) + abs(start.y - goal.y)) << std::endl;
	return 10 * (abs(start.x - goal.x) + abs(start.y - goal.y));
}

bool Map::on_closed(sf::Vector2f point)
{
	return tile_map[(int)point.x / 16][(int)point.y / 16].on_closed;
}

void Map::set_closed(sf::Vector2f point, bool closed)
{
	tile_map[(int)point.x / 16][(int)point.y / 16].on_closed = closed;
}

bool Map::on_open(sf::Vector2f point)
{
	return tile_map[(int)point.x / 16][(int)point.y / 16].on_open;
}

void Map::set_open(sf::Vector2f point, bool closed)
{
	tile_map[(int)point.x / 16][(int)point.y / 16].on_open = closed;
}

void Map::set_on_path(sf::Vector2f point, bool trulse)
{
	tile_map[(int)point.x / 16][(int)point.y / 16].on_path = trulse;
}

bool Map::on_path(sf::Vector2f point)
{
	return tile_map[(int)point.x / 16][(int)point.y / 16].on_path;
}

/*
	resetPlayerPath empties and deletes the Game::player_path stack
	as well as setting all tile_map path variables to false (on_path, on_closed, on_open)
	Input:
	none
	Output:
	none
*/
void Map::resetPlayerPath()
{
	while (!Game::player_path.empty())
	{
		delete Game::player_path.top();
		Game::player_path.pop();
	}

	for (int j = 0; j < 48; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			tile_map[i][j].on_path = false;
			tile_map[i][j].on_closed = false;
			tile_map[i][j].on_open = false;
		}
	}
}