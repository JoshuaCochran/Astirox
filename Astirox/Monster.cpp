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
	/*maxDmg = baseStatsTable["maximumDamage"].cast<int>() * damage_modifier;
	minDmg = maxDmg * .5 * damage_modifier;
	def = baseStatsTable["defense"].cast<int>();
	maxHP = baseStatsTable["maximumHP"].cast<int>() *hp_modifier;
	currentHP = maxHP;
	level = baseStatsTable["level"].cast<int>();
	xpReward = baseStatsTable["expReward"].cast<int>();
	goldReward = baseStatsTable["goldReward"].cast<int>();
	miniBoss = baseStatsTable["miniboss"].cast<bool>();
	mainBoss = baseStatsTable["mainboss"].cast<bool>();*/
}

Monster::Monster(std::string filename) :
_velocity(0), _maxVelocity(600.0f)
{
	pugi::xml_document doc;
	pugi::xml_parse_result monster_file = doc.load_file("data/scripts/monsters/monsters.xml");
	if (monster_file)
	{
		std::cout << "XML [" << "data/scripts/monsters/monsters.xml" << "] parsed without errors, attr value : [" << doc.child("node").attribute("attr").value() << "]\n\n";
	}
	else
	{
		std::cout << "XML [" << "data/scripts/monsters/monsters.xml" << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
		std::cout << "Error description: " << monster_file.description() << "\n";
		std::cout << "Error offset: " << monster_file.offset << " (error at [..." << ("data/scripts/monsters/monsters.xml" + monster_file.offset) << "]\n\n";
	}

	pugi::xml_node monsters = doc.child("Monsters");
	for (pugi::xml_node_iterator it = monsters.begin(); it != monsters.end(); ++it)
	{
		std::string nameNode = it->child_value("name");
		if (nameNode == filename)
		{
			std::cout << "Successfully loaded " << nameNode << "\n\n";
			if ((std::string)it->child("Sprite").child_value("spriteimage") != "")
				Load((std::string)it->child("Sprite").child_value("spriteimage"));
			else if ((std::string)it->child("Sprite").child_value("spritesheet") != "")
			{
				Load((std::string)it->child("Sprite").child_value("spritesheet"),
					atoi(it->child("Sprite").child_value("leftpos")),
					atoi(it->child("Sprite").child_value("toppos")),
					atoi(it->child("Sprite").child_value("width")),
					atoi(it->child("Sprite").child_value("height")));
			}
			else
				std::cout << "Failed to load " << filename << " sprite data\n\n";

			GetSprite().setOrigin(GetSprite().getLocalBounds().width / 2, GetSprite().getLocalBounds().height / 2);

			stats[eMonsterStats::maxHP] = atoi(it->child("Stats").child_value("HP"));
			stats[eMonsterStats::curHP] = stats[eMonsterStats::maxHP];
			stats[eMonsterStats::maxMP] = atoi(it->child("Stats").child_value("MP"));
			stats[eMonsterStats::curMP] = stats[eMonsterStats::maxMP];

			stats[eMonsterStats::pATT] = atoi(it->child("Stats").child_value("BasePhysicalDamage"));
			stats[eMonsterStats::mATT] = atoi(it->child("Stats").child_value("BaseMagicDamage"));
			stats[eMonsterStats::ACC] = atoi(it->child("Stats").child_value("Accuracy"));
			stats[eMonsterStats::avoid] = atoi(it->child("Stats").child_value("Evasion"));
			stats[eMonsterStats::CC] = atoi(it->child("Stats").child_value("CriticalHitChance"));
			stats[eMonsterStats::CHD] = atoi(it->child("Stats").child_value("CriticalHitDamage"));
			stats[eMonsterStats::defense] = atoi(it->child("Stats").child_value("Defense"));
			stats[eMonsterStats::speed] = atoi(it->child("Stats").child_value("Speed"));

			elementalresistances[eElementalResistances::Physical] = atoi(it->child("ElementalResistances").child_value("Physical"));
			elementalresistances[eElementalResistances::Fire] = atoi(it->child("ElementalResistances").child_value("Fire"));
			elementalresistances[eElementalResistances::Ice] = atoi(it->child("ElementalResistances").child_value("Ice"));
			elementalresistances[eElementalResistances::Lightning] = atoi(it->child("ElementalResistances").child_value("Lightning"));
			elementalresistances[eElementalResistances::Earth] = atoi(it->child("ElementalResistances").child_value("Earth"));
			elementalresistances[eElementalResistances::Air] = atoi(it->child("ElementalResistances").child_value("Air"));
			elementalresistances[eElementalResistances::Holy] = atoi(it->child("ElementalResistances").child_value("Holy"));
			elementalresistances[eElementalResistances::Dark] = atoi(it->child("ElementalResistances").child_value("Dark"));

			ailmentresistances[eAilmentResistances::Poison] = atoi(it->child("AilmentResistances").child_value("Poison"));
			ailmentresistances[eAilmentResistances::Blind] = atoi(it->child("AilmentResistances").child_value("Blind"));
			ailmentresistances[eAilmentResistances::Silence] = atoi(it->child("AilmentResistances").child_value("Silence"));
			ailmentresistances[eAilmentResistances::Sleep] = atoi(it->child("AilmentResistances").child_value("Sleep"));
			ailmentresistances[eAilmentResistances::Paralysis] = atoi(it->child("AilmentResistances").child_value("Paralysis"));
			ailmentresistances[eAilmentResistances::Confuse] = atoi(it->child("AilmentResistances").child_value("Confuse"));
			ailmentresistances[eAilmentResistances::Charm] = atoi(it->child("AilmentResistances").child_value("Charm"));
			ailmentresistances[eAilmentResistances::Burn] = atoi(it->child("AilmentResistances").child_value("Burn"));
			ailmentresistances[eAilmentResistances::Frozen] = atoi(it->child("AilmentResistances").child_value("Frozen"));
			ailmentresistances[eAilmentResistances::Stun] = atoi(it->child("AilmentResistances").child_value("Stun"));

			stats[eMonsterStats::baseXP] = atoi(it->child_value("BaseXP"));
			stats[eMonsterStats::baseGold] = atoi(it->child_value("BaseGold"));
			stats[eMonsterStats::boss] = atoi(it->child_value("Boss"));
			stats[eMonsterStats::miniboss] = atoi(it->child_value("Miniboss"));

			sizeInParty = atoi(it->child_value("SizeInParty"));

			break;
		}
	}


	/*monster_lua_state = luaL_newstate();
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
	mainBoss = baseStatsTable["mainboss"].cast<bool>();*/
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
	if (Game::battle != NULL) return true;

	sf::Vector2f point(GetPosition().x + x, GetPosition().y + y);
	if (map.is_wall(point)) return true;
	else if (map.is_player(point))
	{
		if (Game::playerParty[0] != NULL)
		{
			sf::Rect<float> mBB = GetBoundingRect();
			sf::Rect<float> p1BB = Game::playerParty[0]->GetBoundingRect();
			mBB.left += x;
			mBB.top += y;
			if (p1BB.intersects(mBB))
			{
				//ServiceLocator::GetAudio()->PlaySound("audio/jingles_NES00.ogg");
				for (int i = 0; i < map.GetSpawnedMonsters().size(); i++)
				{
					for (int j = 0; j < map.GetSpawnedMonsters()[i].size(); j++)
					{
						if (map.GetSpawnedMonsters()[i][j] == this)
						{
							Battle* combat = new Battle(Game::playerParty, map.GetSpawnedMonsters()[i], true);
							combat->SetTurn(1);
							Game::startBattle(combat);
							return true;
						}
					}
				}
			}
		}
	}
	else if (map.is_monster(point)) return true;
	else
	{
		map.set_map(GetPosition(), ".");
		map.set_map(sf::Vector2f(GetPosition().x + x, GetPosition().y + y), "m");
		return false;
	}
}

void Monster::Update(Map& map)
{
	// Distance between player and monster
	float DISTANCE = sqrt(pow((GetPosition().x - Game::playerParty[0]->GetPosition().x), 2.0) - pow(GetPosition().y - Game::playerParty[0]->GetPosition().y, 2));
	// Distance between monster and player along the x direction
	float DISTANCE_X = GetPosition().x - Game::playerParty[0]->GetPosition().x;
	// Distance between monster and player along the y direction
	float DISTANCE_Y = GetPosition().y - Game::playerParty[0]->GetPosition().y;

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
	return stats[eMonsterStats::curHP];
}

int Monster::GetMaxHP()
{
	return stats[eMonsterStats::maxHP];
}

int Monster::GetCurrentMP()
{
	return stats[eMonsterStats::curMP];
}

int Monster::GetMaxMP()
{
	return stats[eMonsterStats::maxMP];
}

int Monster::GetXPReward()
{
	return stats[eMonsterStats::baseXP];
}

int Monster::GetGoldReward()
{
	return stats[eMonsterStats::baseGold];
}

int Monster::GetLevel()
{
	return stats[eMonsterStats::level];
}

int Monster::GetMinDmg()
{
	return (int)stats[eMonsterStats::pATT] * 0.6;
}

int Monster::GetMaxDmg()
{
	return stats[eMonsterStats::pATT];
}

int Monster::GetDefense()
{
	return stats[eMonsterStats::defense];
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
	stats[eMonsterStats::curHP] += hp;
	if (stats[eMonsterStats::curHP] <= 0)
		GetSprite().rotate(-90.0f);
	else GetSprite().setRotation(0);
}

int Monster::GetSizeInParty() const
{
	return sizeInParty;
}

int Monster::GetStat(int stat) const
{
	if ((sizeof(stats) / sizeof(stats[0])) >= stat)
		return stats[stat];
	else
		return 0;
}

void SpawnMonster(Player player);