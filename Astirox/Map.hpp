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
struct s_FogOfWar
{
	sf::Sprite sprite;
	bool IsVisible;
	bool HasSeen;
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
	std::vector<Equipment*>& GetEquipmentOnFloor();

	int GetMovesSinceSpawn();
	void AddMove();

	std::vector<TeleportInfo>& GetPortals();
	void UsePortal(Player& player, TeleportInfo portal);

	int GetMaxMobs();
	std::string GetTMXFile();
	void CheckSpawn();

	int get_fog_width() const;
	int get_fog_height() const;

	void set_visible(unsigned int x, unsigned int y, bool visible);
	bool is_opaque(unsigned int x, unsigned int y);
	std::vector<s_FogOfWar>& GetFogOfWar();
	void do_fov(Map& map, uint x, uint y, uint radius);
	void cast_light(Map& map, uint x, uint y, uint radius, uint row,
		float start_slope, float end_slope, uint xx, uint xy, uint yx,
		uint yy);
	
private:
	std::string tmxFile; // e.g. map.tmx
	tmx::MapLoader* mapLoader;
	lua_State* map_lua_state;
	int mobMax;
	int movesSinceSpawn;

	sf::RectangleShape* test;

	std::vector<TeleportInfo> portals;
	
	std::vector<sf::Vector2f> spawnLocs;
	std::vector<std::string> spawnCandidates;
	std::vector<double> candidateRarities;
	std::vector<Monster*> spawnedMonsters;
	std::vector<Equipment*> equipmentOnFloor;
	
	sf::Texture fogTexture;
	std::vector<s_FogOfWar> fogOfWar;
	int opaque_map[64][48];

	int FOG_OF_WAR_WIDTH;
	int FOG_OF_WAR_HEIGHT;

	void AddSpawnCandidate(std::string filename, double rarity);
	void SpawnMonster();

	bool CheckSpawnIntersection(std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject>>::_Val_types>::_Myt>& mo, double& totalRarity, std::vector<std::_Vector_iterator<std::_Vector_val<std::_Vec_base_types<tmx::MapObject, std::allocator<tmx::MapObject >> ::_Val_types>::_Myt >>& alreadyChecked);

	void AddTargetMap(std::string filename);
	void AddExitPosX(int posx);
	void AddExitPosY(int posy);
	void AddTargetPosX(int posx);
	void AddTargetPosY(int posy);
};


#endif
