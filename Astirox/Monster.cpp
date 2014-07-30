#include "stdafx.h"
#include "Monster.h"
#include "ServiceLocator.h"
#include "Player.h"
#include "Game.h"


Monster::Monster() :
_velocity(0), _maxVelocity(600.0f)
{
	monster_lua_state = luaL_newstate();
	luaL_dofile(monster_lua_state, "data/scripts/monsters/test_monster.lua");
	luaL_openlibs(monster_lua_state);
	lua_pcall(monster_lua_state, 0, 0, 0);

	LuaRef spriteTable = getGlobal(monster_lua_state, "t_monster")["sprite"];
	LuaRef baseStatsTable = getGlobal(monster_lua_state, "t_monster")["baseStats"];

	if (spriteTable["spriteLeftPos"].isNil())
	{
		Load(spriteTable["spriteSheet"].cast<std::string>());
	}
	else
	{
		Load(spriteTable["spriteSheet"].cast<std::string>(), spriteTable["spriteLeftPos"].cast<int>(), spriteTable["spriteTopPos"].cast<int>(), spriteTable["spriteWidth"].cast<int>(), spriteTable["spriteHeight"].cast<int>());
	}
	assert(IsLoaded());

	GetSprite().setOrigin(GetSprite().getLocalBounds().width / 2, GetSprite().getLocalBounds().height / 2);

	int hp_modifier = 1;
	int damage_modifier = 1;

	if (Game::DIFFICULTY_SETTING == "HARD")
	{
		hp_modifier = 2;
		damage_modifier = 2;
	}

	name = baseStatsTable["name"].cast<std::string>();
	maxDmg = baseStatsTable["maximumDamage"].cast<int>() * damage_modifier;
	minDmg = maxDmg * .5 * damage_modifier;
	def = baseStatsTable["defense"].cast<int>();
	maxHP = baseStatsTable["maximumHP"].cast<int>() *hp_modifier;
	currentHP = maxHP;
	level = baseStatsTable["level"].cast<int>();
	xpReward = baseStatsTable["expReward"].cast<int>();
	goldReward = baseStatsTable["goldReward"].cast<int>();
	miniBoss = baseStatsTable["miniboss"].cast<bool>();
	mainBoss = baseStatsTable["mainboss"].cast<bool>();
}

Monster::Monster(std::string filename) :
_velocity(0), _maxVelocity(600.0f)
{
	monster_lua_state = luaL_newstate();
	if (luaL_dofile(monster_lua_state, ("data/scripts/monsters/" + filename).c_str()) != 0)
		std::cout << "Failed to load filename\n\n";
	luaL_openlibs(monster_lua_state);
	lua_pcall(monster_lua_state, 0, 0, 0);

	LuaRef spriteTable = getGlobal(monster_lua_state, "t_monster")["sprite"];
	LuaRef baseStatsTable = getGlobal(monster_lua_state, "t_monster")["baseStats"];

	if (spriteTable["spriteLeftPos"].isNil())
	{
		Load(spriteTable["spriteSheet"].cast<std::string>());
	}
	else
	{
		Load(spriteTable["spriteSheet"].cast<std::string>(), spriteTable["spriteLeftPos"].cast<int>(), spriteTable["spriteTopPos"].cast<int>(), spriteTable["spriteWidth"].cast<int>(), spriteTable["spriteHeight"].cast<int>());
	}
	assert(IsLoaded());

	//GetSprite().setOrigin(GetSprite().getLocalBounds().width / 2, GetSprite().getLocalBounds().height / 2);

	int hp_modifier = 1;
	int damage_modifier = 1;

	if (Game::DIFFICULTY_SETTING == "HARD")
	{
		hp_modifier = 2;
		damage_modifier = 2;
	}

	name = baseStatsTable["name"].cast<std::string>();
	maxDmg = baseStatsTable["maximumDamage"].cast<int>() * damage_modifier;
	minDmg = maxDmg * .5 * damage_modifier;
	def = baseStatsTable["defense"].cast<int>();
	maxHP = baseStatsTable["maximumHP"].cast<int>() *hp_modifier;
	currentHP = maxHP;
	level = baseStatsTable["level"].cast<int>();
	xpReward = baseStatsTable["expReward"].cast<int>();
	goldReward = baseStatsTable["goldReward"].cast<int>();
	miniBoss = baseStatsTable["miniboss"].cast<bool>();
	mainBoss = baseStatsTable["mainboss"].cast<bool>();
}

Monster::~Monster()
{
}

void Monster::Draw(sf::RenderWindow& rw)
{
	VisibleGameObject::Draw(rw);
}

bool Monster::checkCollision(Map& map, float x, float y)
{
	if (this == nullptr) return true;
	bool collision = false;
	sf::Vector2f point(GetPosition().x + x, GetPosition().y + y);
	for (auto layer = map.GetMapLoader()->GetLayers().begin(); layer != map.GetMapLoader()->GetLayers().end(); ++layer)
	{
		if (layer->name == "Collision")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				collision = object->Contains(point);
				if (collision) return collision;
			}
		}
	}

	Player* player1 = dynamic_cast<Player*>(Game::GetGameObjectManager().Get("Player"));
	if (player1 != NULL)
	{
		GetSprite().move(x, y);
		sf::Rect<float> p1BB = player1->GetBoundingRect();

		if (p1BB.intersects(GetBoundingRect()))
		{
			//ServiceLocator::GetAudio()->PlaySound("audio/jingles_NES00.ogg");
			collision = true;
			Game::startBattle(*player1, *this);
		}
		GetSprite().move(-x, -y);
	}

	std::stringstream ss;
	for (int i = 1; i < map.GetSpawnedMonsters().size(); i++)
	{
		if (map.GetSpawnedMonsters()[i] != NULL && map.GetSpawnedMonsters()[i] != this)
		{
			GetSprite().move(x, y);
			sf::Rect<float> m1BB = map.GetSpawnedMonsters()[i]->GetBoundingRect();

			if (m1BB.intersects(GetBoundingRect()))
			{
				collision = true;
			}
			GetSprite().move(-x, -y);
		}
	}
	return collision;
}

void Monster::Update(Map& map)
{
	Player* player1 = dynamic_cast<Player*>(Game::GetGameObjectManager().Get("Player"));
	// Distance between player and monster
	float DISTANCE = sqrt(pow((GetPosition().x - player1->GetPosition().x), 2.0) - pow(GetPosition().y - player1->GetPosition().y, 2));
	// Distance between monster and player along the x direction
	float DISTANCE_X = GetPosition().x - player1->GetPosition().x;
	// Distance between monster and player along the y direction
	float DISTANCE_Y = GetPosition().y - player1->GetPosition().y;

	/* If the distance between the monster and player
		is less than the aggro radius of the monster activate "AI"*/
	if (DISTANCE <= AGGRO_RADIUS)
	{
		if (DISTANCE_X >= DISTANCE_Y)
		{
			// If player is above monster and there is no wall above monster, move monster up
			if (DISTANCE_X > 0 && !checkCollision(map, -VisibleGameObject::MOVE_SPEED, 0))
				GetSprite().move(-VisibleGameObject::MOVE_SPEED, 0);
			// If player is below monster or there is a wall above monster, move monster down
			else if (!checkCollision(map, VisibleGameObject::MOVE_SPEED, 0))
				GetSprite().move(VisibleGameObject::MOVE_SPEED, 0);
		}
		else
		{
			// If player is to the left of monster and there is no wall
			// to the left of monster, move monster to the left
			if (DISTANCE_Y > 0 && !checkCollision(map, 0, -VisibleGameObject::MOVE_SPEED))
				GetSprite().move(0, -VisibleGameObject::MOVE_SPEED);
			// Else if the player is to the right of the monster
			// or there is a wall to the left of the monster, move the monster right
			else if (!checkCollision(map, 0, VisibleGameObject::MOVE_SPEED))
				GetSprite().move(0, VisibleGameObject::MOVE_SPEED);
		}
	}
	// If the player is not within the aggro radius move in a random direction
	else
	{
		int direction = rand() % 4;
		switch (direction)
		{
		case 0:
			if (!checkCollision(map, -VisibleGameObject::MOVE_SPEED, 0))
			{
				GetSprite().move(-VisibleGameObject::MOVE_SPEED, 0);
				break;
			}
		case 1:
			if (!checkCollision(map, VisibleGameObject::MOVE_SPEED, 0))
			{
				GetSprite().move(VisibleGameObject::MOVE_SPEED, 0);
				break;
			}
		case 2:
			if (!checkCollision(map, 0, -VisibleGameObject::MOVE_SPEED))
			{
				GetSprite().move(0, -VisibleGameObject::MOVE_SPEED);
				break;
			}
		case 3:
			if (!checkCollision(map, 0, VisibleGameObject::MOVE_SPEED))
			{
				GetSprite().move(0, VisibleGameObject::MOVE_SPEED);
				break;
			}

		}
	}
}

void Monster::Move()
{

}

int Monster::GetCurrentHP()
{
	return currentHP;
}

int Monster::GetMaxHP()
{
	return maxHP;
}

int Monster::GetMP()
{
	return MP;
}

int Monster::GetXPReward()
{
	return xpReward;
}

int Monster::GetGoldReward()
{
	return goldReward;
}

int Monster::GetLevel()
{
	return level;
}

int Monster::GetMinDmg()
{
	return minDmg;
}

int Monster::GetMaxDmg()
{
	return maxDmg;
}

int Monster::GetDefense()
{
	return def;
}

std::string Monster::GetName()
{
	return name;
}

bool Monster::IsMiniBoss()
{
	return miniBoss;
}

bool Monster::IsMainBoss()
{
	return mainBoss;
}

void Att(Player player);
void MagicAtt();

void Monster::AddHP(int hp)
{
	currentHP += hp;
}

void SpawnMonster(Player player);