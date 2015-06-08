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

typedef unsigned int uint;
static int multipliers[4][8] = {
		{ 1, 0, 0, -1, -1, 0, 0, 1 },
		{ 0, 1, -1, 0, 0, -1, 1, 0 },
		{ 0, 1, 1, 0, 0, -1, -1, 0 },
		{ 1, 0, 0, 1, -1, 0, 0, -1 }
};

struct TeleportInfo{
	sf::Vector2f exitPos;
	sf::Vector2f targetPos;
	std::string targetMap;
};

struct TilemapObject{
	TilemapObject() : object_type(""), monster(NULL), on_closed(false), on_open(false), on_path(false) {}
	std::string object_type;
	Monster* monster;
	
	bool on_closed;
	bool on_open;
	bool on_path;
};

struct s_FogOfWar
{
	sf::Sprite sprite;
	bool IsVisible;
	bool HasSeen;
};

struct s_TileSelect
{
	sf::Sprite sprite;
};

struct path_element
{
	sf::Vector2f pos;
	path_element* parent;
	int f_cost;
	int g_cost;
	int h_cost;
};

class Player;

class Map
{
public:
	Map(std::string filename = "");
	~Map();

	void DrawAll(sf::RenderWindow& renderWindow);
	void UpdateMonsters();
	tmx::MapLoader& GetMapLoader();
	std::vector<Monster*>& GetSpawnedMonsters();
	std::vector<Equipment*>& GetEquipmentOnFloor();

	int GetMovesSinceSpawn();
	void AddMove();

	std::vector<TeleportInfo>& GetPortals();
	bool UsePortal(Player& player, sf::Vector2f point);

	int GetMaxMobs();
	std::string GetTMXFile();
	void CheckSpawn();

	int get_fog_width() const;
	int get_fog_height() const;

	void set_visible(unsigned int x, unsigned int y, bool visible);
	bool is_wall(sf::Vector2f point);
	bool is_portal(sf::Vector2f point);
	bool is_monster(sf::Vector2f point);
	bool is_player(sf::Vector2f point);
	Monster* get_monster_at(sf::Vector2f point);
	void monster_moved(sf::Vector2f from, sf::Vector2f to);


	void draw_select_tile(unsigned int x, unsigned int y, sf::RenderWindow& renderWindow);
	std::vector<s_TileSelect>& GetTileSelect();

	void set_map(sf::Vector2f point, std::string str);
	void draw_map();

	std::vector<s_FogOfWar>& GetFogOfWar();
	void do_fov(uint x, uint y, uint radius);

	void Astar(sf::Vector2f start, sf::Vector2f goal, Entity& entity);

private:
	std::string tmxFile; // e.g. map.tmx
	tmx::MapLoader mapLoader;
	lua_State* map_lua_state;
	int mobMax;
	int movesSinceSpawn;

	int MAP_SIZE_IN_TILES;
	int MAP_HEIGHT_IN_TILES;
	int MAP_WIDTH_IN_TILES;

	sf::RectangleShape* test;

	std::vector<TeleportInfo> portals;
	
	std::vector<sf::Vector2f> spawnLocs;
	std::vector<std::string> spawnCandidates;
	std::vector<double> candidateRarities;
	//std::vector<Monster*> spawningMonsterParty;
	//std::vector<std::vector<Monster*>> spawnedMonsters;
	std::vector<Monster*> spawnedMonsters;
	std::vector<Equipment*> equipmentOnFloor;

	const int monsterPartySize = 4;

	//TilemapObject tile_map[64][48];
	TilemapObject** tile_map;
	
	sf::Texture fogTexture;
	std::vector<s_FogOfWar> fogOfWar;
//	std::string opaque_map[64][48];
	int FOG_OF_WAR_WIDTH;
	int FOG_OF_WAR_HEIGHT;

	sf::Texture tileSelectTexture;
	std::vector<s_TileSelect> tileSelect;


	bool is_opaque(unsigned int x, unsigned int y);
	void cast_light(uint x, uint y, uint radius, uint row,
		float start_slope, float end_slope, uint xx, uint xy, uint yx,
		uint yy);

	void AddSpawnCandidate(std::string filename, double rarity);
	//void SpawnMonster();

	double CheckSpawnIntersection(std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject>>::_Val_types>::_Myt>& mo, double& totalRarity, std::vector<std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject >> ::_Val_types>::_Myt >>& alreadyChecked);

	void AddTargetMap(std::string filename);
	void AddExitPosX(int posx);
	void AddExitPosY(int posy);
	void AddTargetPosX(int posx);
	void AddTargetPosY(int posy);

	/*
		A* pathfinding functions
	*/


	const int default_g_cost = 10;

	//std::vector<path_element*> Astar(sf::Vector2f start, sf::Vector2f goal);
	void reconstruct_path(sf::Vector2f came_from, sf::Vector2f current);
	int heuristic_cost_estimate(sf::Vector2f start, sf::Vector2f goal);
	bool on_closed(sf::Vector2f point);
	void set_closed(sf::Vector2f point, bool closed);
	bool on_open(sf::Vector2f point);
	void set_open(sf::Vector2f point, bool closed);

	void set_on_path(sf::Vector2f point, bool trulse);
	bool on_path(sf::Vector2f point);

	void resetPath(Entity& entity);
};


#endif
