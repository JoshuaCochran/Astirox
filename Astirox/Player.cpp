#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "ServiceLocator.h"
#include "Monster.h"
#include "Battle.h"




Player::Player() :
_velocity(0), _maxVelocity(600.0f)
{
	//	Init LUA
	player_lua_state = luaL_newstate();
	luaL_dofile(player_lua_state, "data/scripts/player/player.lua");
	luaL_openlibs(player_lua_state);
	lua_pcall(player_lua_state, 0, 0, 0);
	
	getGlobalNamespace(player_lua_state)
		.beginClass <Player>("Player")
			.addFunction("BaseStat", &Player::GetBaseStat)
			.addFunction("GearStat", &Player::GetGearStat)
			.addFunction("StatMod", &Player::GetStatMod)
		.endClass();

	push(player_lua_state, this);
	lua_setglobal(player_lua_state, "player");

	LuaRef spriteTable = getGlobal(player_lua_state, "t_player")["sprite"];
	LuaRef baseStatTable = getGlobal(player_lua_state, "t_player")["baseStats"];
	LuaRef statModTable = getGlobal(player_lua_state, "t_player")["modifiers"];

	// Load Default Player Sprite
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


	name = "Arixal";
	//Initializes all stats with 0.
	for (int i = 0; i < (sizeof(stats)/sizeof(stats[0])); i++)
		stats[i] = 0;
	for (int i = 0; i < (sizeof(baseStats)/sizeof(baseStats[0])); i++)
		baseStats[i] = 1;
	for (int i = 0; i < (sizeof(gearStats) / sizeof(gearStats[0])); i++)
		gearStats[i] = 0;
	for (int i = 0; i < (sizeof(bonusStats) / sizeof(bonusStats[0])); i++)
		bonusStats[i] = 0;
	for (int i = 0; i < (sizeof(statModifiers) / sizeof(statModifiers[0])); i++)
		statModifiers[i] = 1;

	baseStats[baseStats::str] = baseStatTable["strength"].cast<int>();
	baseStats[baseStats::dex] = baseStatTable["dexterity"].cast<int>();
	baseStats[baseStats::intel] = baseStatTable["intelligence"].cast<int>();
	baseStats[baseStats::wis] = baseStatTable["wisdom"].cast<int>();
	baseStats[baseStats::stam] = baseStatTable["stamina"].cast<int>();

	statModifiers[statModifiers::str] = statModTable["str_modifier"].cast<double>();
	statModifiers[statModifiers::dex] = statModTable["dex_modifier"].cast<double>();
	statModifiers[statModifiers::intel] = statModTable["int_modifier"].cast<double>();
	statModifiers[statModifiers::wis] = statModTable["wis_modifier"].cast<double>();
	statModifiers[statModifiers::stam] = statModTable["stam_modifier"].cast<double>();
	statModifiers[statModifiers::def] = statModTable["defense_modifier"].cast<double>();
	statModifiers[statModifiers::res] = statModTable["res_modifier"].cast<double>();
	statModifiers[statModifiers::hp] = statModTable["hp_modifier"].cast<double>();
	statModifiers[statModifiers::mp] = statModTable["mp_modifier"].cast<double>();
	statModifiers[statModifiers::wdmg] = statModTable["weapon_damage_modifier"].cast<double>();
	statModifiers[statModifiers::burned] = statModTable["burned_enemy_modifier"].cast<double>();
	statModifiers[statModifiers::drop] = statModTable["drop_rate_modifier"].cast<double>();
	statModifiers[statModifiers::xp] = statModTable["xp_modifier"].cast<double>();
	statModifiers[statModifiers::jp] = statModTable["jp_modifier"].cast<double>();

	stats[ePlayerStats::level] = 1;

	UpdateStats();
	stats[ePlayerStats::curHP] = stats[ePlayerStats::maxHP];
	stats[ePlayerStats::curMP] = stats[ePlayerStats::maxMP];
	stats[ePlayerStats::gold] = 0;


	playerEquipment.push_back(NULL);
	playerEquipment.push_back(NULL);
	playerEquipment.push_back(NULL);
	playerEquipment.push_back(NULL);
	playerEquipment.push_back(NULL);

	spellInventory.push_back(BasicPhysAttack);

	can_wear_mail = false;
	can_wear_plate = false;
}

Player::~Player()
{
}

void Player::ReloadScripts()
{
	luaL_dofile(player_lua_state, "data/scripts/player.lua");
	LuaRef baseStatTable = luabridge::getGlobal(player_lua_state, "t_player")["baseStats"];
	LuaRef statModTable = luabridge::getGlobal(player_lua_state, "t_player")["modifiers"];

	statModifiers[statModifiers::str] = statModTable["str_modifier"].cast<double>();
	statModifiers[statModifiers::dex] = statModTable["dex_modifier"].cast<double>();
	statModifiers[statModifiers::intel] = statModTable["int_modifier"].cast<double>();
	statModifiers[statModifiers::wis] = statModTable["wis_modifier"].cast<double>();
	statModifiers[statModifiers::stam] = statModTable["stam_modifier"].cast<double>();
	statModifiers[statModifiers::def] = statModTable["defense_modifier"].cast<double>();
	statModifiers[statModifiers::res] = statModTable["res_modifier"].cast<double>();
	statModifiers[statModifiers::hp] = statModTable["hp_modifier"].cast<double>();
	statModifiers[statModifiers::mp] = statModTable["mp_modifier"].cast<double>();
	statModifiers[statModifiers::wdmg] = statModTable["weapon_damage_modifier"].cast<double>();
	statModifiers[statModifiers::burned] = statModTable["burned_enemy_modifier"].cast<double>();
	statModifiers[statModifiers::drop] = statModTable["drop_rate_modifier"].cast<double>();
	statModifiers[statModifiers::xp] = statModTable["xp_modifier"].cast<double>();
	statModifiers[statModifiers::jp] = statModTable["jp_modifier"].cast<double>();

	UpdateStats();
}

std::vector<std::function<std::string(Player&, Monster&)>>& Player::GetSpellInventory()
{
	return spellInventory;
}

void Player::Draw(sf::RenderWindow & rw)
{
	std::stringstream ss;
	ss << "(" << GetPosition().x << "," << GetPosition().y << ")";
	sf::Text text(ss.str(), Game::font);
	text.setCharacterSize(10);
	text.setStyle(sf::Text::Bold);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	text.setPosition(GetPosition().x, GetPosition().y + GetHeight());
	Game::GetWindow().draw(text);
	
	VisibleGameObject::Draw(rw);
}

void Player::DrawStats(sf::RenderWindow & renderWindow)
{
	std::stringstream ss;
	ss << name << "Level: " << stats[ePlayerStats::level]
		<< "\n\nStrength:   \t\t\t\t\t\t\t\t\t" << stats[ePlayerStats::str]
		<< "\n\nDexterity:   \t\t\t\t\t\t\t\t\t" << stats[ePlayerStats::dex]
		<< "\n\nInteligence:   \t\t\t\t\t\t\t\t" << stats[ePlayerStats::intel]
		<< "\n\nWisdom:\t\t\t\t\t\t\t\t\t\t" << stats[ePlayerStats::wis]
		<< "\n\nVitality: \t\t\t\t\t\t\t\t\t\t" << stats[ePlayerStats::stam]
		<< "\n\n\n\n\nStat Points:\t\t\t\t\t\t\t\t\t" << stats[ePlayerStats::statPts];
	std::string z(ss.str());
	sf::Text text(z, Game::font);
	text.setCharacterSize(10);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	text.setPosition(792 + text.getLocalBounds().width / 2 + 15, 186);
	
	Game::GetGuiObjectManager().Get("stat menu")->Draw(renderWindow);
	renderWindow.draw(text);
}

void Player::Update(float elapsedTime)
{
}


bool Player::moveto(Map& map, sf::Event currentEvent)
{
	float x = 0.0f;
	float y = 0.0f;
	if (currentEvent.type == sf::Event::KeyReleased)
	{
		switch (currentEvent.key.code)
		{
		case sf::Keyboard::Left:
			if (!checkCollision(map, -16, 0)) GetSprite().move(-16, 0);
			map.AddMove();
			return true;
			break;
		case sf::Keyboard::Right:
			if (!checkCollision(map, 16, 0)) GetSprite().move(16, 0);
			map.AddMove();
			return true;
			break;
		case sf::Keyboard::Up:
			if (!checkCollision(map, 0, -16)) GetSprite().move(0, -16);
			map.AddMove();
			return true;
			break;
		case sf::Keyboard::Down:
			if (!checkCollision(map, 0, 16)) GetSprite().move(0, 16);
			map.AddMove();
			return true;
			break;
		}
	}
	return false;
}

bool Player::checkCollision(Map& map, float x, float y)
{
	bool collision = false;
	//Check to see if moving onto monster
	for (int i = 0; i < map.GetSpawnedMonsters().size(); i++)
	{
		Player* player1 = dynamic_cast<Player*>(Game::GetGameObjectManager().Get("Player"));
		if (map.GetSpawnedMonsters()[i] != NULL)
		{
			GetSprite().move(x, y);
			sf::Rect<float> m1BB = map.GetSpawnedMonsters()[i]->GetBoundingRect();
			if (m1BB.intersects(GetBoundingRect()))
			{
				//ServiceLocator::GetAudio()->PlaySound("audio/jingles_NES00.ogg");
				Game::startBattle(*player1, *map.GetSpawnedMonsters()[i]);
				GetSprite().move(-x, -y);
				return true;
			}
			GetSprite().move(-x, -y);
		}
	}


	//Check to see if collision or entering portal
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
		else if (layer->name == "Portal")
		{
			for (auto object = layer->objects.begin(); object != layer->objects.end(); ++object)
			{
				if (object->GetName() != "" && object->GetPropertyString("Target X") != "" && object->GetPropertyString("Target Y") != "")
				{
					if (object->Contains(point))
					{
						if (object->GetName() == map.GetTMXFile())
						{
							float teleX = (boost::lexical_cast<float>(object->GetPropertyString("Target X")) * 16.0f) + 8.0f;
							float teleY = (boost::lexical_cast<float>(object->GetPropertyString("Target Y")) * 16.0f) + 8.0f;
							std::cout << "Teleporting player to:\t\n"
								<< "Map:\t" << object->GetName() << "\n"
								<< "PosX:\t" << teleX << "\n"
								<< "PosY:\t" << teleY;
							SetPosition(teleX, teleY);
							collision = true;
							break;
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
									SetPosition(teleX, teleY);
									collision = true;
									break;
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
								SetPosition(teleX, teleY);
								collision = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	return collision;
}

std::string Player::GetName() const
{
	return name;
}

int Player::GetCurrentHP() const
{
	return stats[ePlayerStats::curHP];
}

int Player::GetMaxHP() const
{
	return stats[ePlayerStats::maxHP];
}

int Player::GetCurrentMP() const
{
	return stats[ePlayerStats::curMP];
}
int Player::GetMaxMP() const
{
	return stats[ePlayerStats::maxMP];
}
int Player::GetCurrentXP() const
{
	return stats[ePlayerStats::curXP];
}
int Player::GetXPNeeded() const
{
	return stats[ePlayerStats::xpNeeded];
}

int Player::GetPhysAttDmg() const
{
	return stats[ePlayerStats::pATT];
}

int Player::GetLevel() const
{
	return stats[ePlayerStats::level];
}

int Player::GetDefense() const
{
	return stats[ePlayerStats::defense];
}

void Player::AddHP(int hp)
{
	stats[ePlayerStats::curHP] += hp;
}

void Player::GainXP(int num)
{
	stats[ePlayerStats::curXP] += num;
	if (stats[ePlayerStats::curXP] >= stats[ePlayerStats::xpNeeded])
	{
		LevelUp();
	}
}

void Player::LevelUp()
{
	stats[ePlayerStats::level] += 1;
	stats[ePlayerStats::curXP] = 0;
	stats[ePlayerStats::statPts] += 5;
	UpdateStats();
}

void Player::UpdateStats()
{
	LuaRef calculateHP = luabridge::getGlobal(player_lua_state, "calculateHP");
	LuaRef calculateMP = luabridge::getGlobal(player_lua_state, "calculateMP");
	LuaRef calculatePATT = luabridge::getGlobal(player_lua_state, "calculatePhysicalAttack");
	LuaRef calculateMATT = luabridge::getGlobal(player_lua_state, "calculateMagicAttack");
	LuaRef calculateXPToLevel = luabridge::getGlobal(player_lua_state, "calculateXPToLevel");
	LuaRef calculateResistance = luabridge::getGlobal(player_lua_state, "calculateResistance");
	LuaRef calculateDefense = luabridge::getGlobal(player_lua_state, "calculateDefense");

	stats[ePlayerStats::str] = (baseStats[baseStats::str] + gearStats[eEquipmentStats::Strength]) * statModifiers[statModifiers::str];
	stats[ePlayerStats::dex] = (baseStats[baseStats::dex] + gearStats[eEquipmentStats::Dexterity]) * statModifiers[statModifiers::dex];
	stats[ePlayerStats::intel] = (baseStats[baseStats::intel] + gearStats[eEquipmentStats::Intelligence]) * statModifiers[statModifiers::intel];;
	stats[ePlayerStats::wis] = (baseStats[baseStats::wis] + gearStats[eEquipmentStats::Wisdom]) * statModifiers[statModifiers::wis];;
	stats[ePlayerStats::stam] = (baseStats[baseStats::stam] + gearStats[eEquipmentStats::Stamina]) * statModifiers[statModifiers::stam];;
	stats[ePlayerStats::maxHP] = calculateHP();
	stats[ePlayerStats::maxMP] = calculateMP();
	stats[ePlayerStats::wdmg] = (baseStats[baseStats::wdmg] + gearStats[eEquipmentStats::Damage]) * statModifiers[statModifiers::wdmg];;

	stats[ePlayerStats::pATT] = calculatePATT();
	stats[ePlayerStats::mATT] = calculateMATT();
	stats[ePlayerStats::xpNeeded] = calculateXPToLevel();
	stats[ePlayerStats::res] = calculateResistance();
	stats[ePlayerStats::defense] = calculateDefense();

	/*stats[ePlayerStats::str] = (baseStats[baseStats::str] + gearStats[eEquipmentStats::Strength]) * statModifiers[statModifiers::str];
	stats[ePlayerStats::dex] = (baseStats[baseStats::dex] + gearStats[eEquipmentStats::Dexterity]) * statModifiers[statModifiers::dex];
	stats[ePlayerStats::intel] = (baseStats[baseStats::intel] + gearStats[eEquipmentStats::Intelligence]) * statModifiers[statModifiers::intel];;
	stats[ePlayerStats::wis] = (baseStats[baseStats::wis] + gearStats[eEquipmentStats::Wisdom]) * statModifiers[statModifiers::wis];;
	stats[ePlayerStats::stam] = (baseStats[baseStats::stam] + gearStats[eEquipmentStats::Stamina]) * statModifiers[statModifiers::stam];;
	stats[ePlayerStats::maxHP] = ((baseStats[baseStats::stam] + gearStats[eEquipmentStats::Stamina]) * 10) * statModifiers[statModifiers::hp];
	stats[ePlayerStats::maxMP] = ((baseStats[baseStats::wis] + gearStats[eEquipmentStats::Wisdom]) * 10) * statModifiers[statModifiers::mp];
	stats[ePlayerStats::wdmg] = (baseStats[baseStats::wdmg] + gearStats[eEquipmentStats::Damage]) * statModifiers[statModifiers::wdmg];;
	stats[ePlayerStats::pATT] = ceil(stats[ePlayerStats::wdmg] * (stats[ePlayerStats::str] + (stats[ePlayerStats::dex] * 0.8)));
	stats[ePlayerStats::mATT] = ceil(stats[ePlayerStats::intel] * 0.8);
	stats[ePlayerStats::xpNeeded] = pow(stats[ePlayerStats::level], 3) + 30;
	stats[ePlayerStats::res] = ceil((stats[ePlayerStats::intel] * 0.05) + (stats[ePlayerStats::wis] * 0.1) + gearStats[eEquipmentStats::Resistance]) * statModifiers[statModifiers::def];
	stats[ePlayerStats::defense] = ceil((stats[ePlayerStats::str] * 0.05) + (stats[ePlayerStats::stam] * 0.1) + gearStats[eEquipmentStats::Defense]) * statModifiers[statModifiers::res];*/

	/*if (unlocked_jobs.size() == 1 && unlocked_jobs[vagabond].getLevel() >= 3)
	{
		std::cout << "You have unlocked the Warrior and Mage jobs!\n";
		unlockJob("Warrior");
		unlockJob("Mage");
	}*/
}

void Player::AddStat(int stat, int num)
{
	if (stats[ePlayerStats::statPts] > 0)
	{
		stats[ePlayerStats::statPts] -= num;
		stats[stat] += num;
	}
	UpdateStats();
}

int Player::GetStat(int stat)
{
	if ((sizeof(stats) / sizeof(stats[0])) >= stat)
		return stats[stat];
	else
		return 0;
}

/*int Player::GetGearStat(int stat)
{
	if ((sizeof(gearStats) / sizeof(gearStats[0])) >= stat)
		return gearStats[stat];
	else
		return 0;
}*/

int Player::GetBaseStat(std::string stat)
{
	if (boost::iequals(stat, "str") || boost::iequals(stat, "strength"))
		return stats[ePlayerStats::str];
	else if (boost::iequals(stat, "dex") || boost::iequals(stat, "dexterity"))
		return stats[ePlayerStats::dex];
	else if (boost::iequals(stat, "int") || boost::iequals(stat, "intelligence"))
		return stats[ePlayerStats::intel];
	else if (boost::iequals(stat, "wis") || boost::iequals(stat, "wisdom"))
		return stats[ePlayerStats::wis];
	else if (boost::iequals(stat, "stam") || boost::iequals(stat, "stamina"))
		return stats[ePlayerStats::stam];
	else if (boost::iequals(stat, "max HP") || boost::iequals(stat, "maxHP"))
		return stats[ePlayerStats::maxHP];
	else if (boost::iequals(stat, "max MP") || boost::iequals(stat, "maxMP"))
		return stats[ePlayerStats::maxMP];
	else if (boost::iequals(stat, "def") || boost::iequals(stat, "defense"))
		return stats[ePlayerStats::defense];
	else if (boost::iequals(stat, "res") || boost::iequals(stat, "resistance"))
		return stats[ePlayerStats::res];
	else if (boost::iequals(stat, "pATT") || boost::iequals(stat, "physical attack"))
		return stats[ePlayerStats::pATT];
	else if (boost::iequals(stat, "rATT") || boost::iequals(stat, "ranged attack"))
		return stats[ePlayerStats::rATT];
	else if (boost::iequals(stat, "mATT") || boost::iequals(stat, "magic attack"))
		return stats[ePlayerStats::mATT];
	else if (boost::iequals(stat, "acc") || boost::iequals(stat, "accuracy"))
		return stats[ePlayerStats::ACC];
	else if (boost::iequals(stat, "CHD") || boost::iequals(stat, "crit hit damage") || boost::iequals(stat, "critical hit damage"))
		return stats[ePlayerStats::CHD];
	else if (boost::iequals(stat, "CC") || boost::iequals(stat, "crit chance") || boost::iequals(stat, "critical chance"))
		return stats[ePlayerStats::CC];
	else if (boost::iequals(stat, "level") || boost::iequals(stat, "lvl"))
		return stats[ePlayerStats::level];
	else if (boost::iequals(stat, "stat points") || boost::iequals(stat, "statPts"))
		return stats[ePlayerStats::statPts];
	else if (boost::iequals(stat, "curXP") || boost::iequals(stat, "current XP"))
		return stats[ePlayerStats::curXP];
	else if (boost::iequals(stat, "xp to level") || boost::iequals(stat, "xpNeeded"))
		return stats[ePlayerStats::xpNeeded];
	else if (boost::iequals(stat, "gold"))
		return stats[ePlayerStats::gold];
	else if (boost::iequals(stat, "wdmg") || boost::iequals(stat, "weapon damage"))
		return stats[ePlayerStats::wdmg];
	else
		return 0;
}

int Player::GetGearStat(std::string stat)
{
	if (boost::iequals(stat, "dmg") || boost::iequals(stat, "damage"))
		return gearStats[eEquipmentStats::Damage];
	else if (boost::iequals(stat, "str") || boost::iequals(stat, "strength"))
		return gearStats[eEquipmentStats::Strength];
	else if (boost::iequals(stat, "dex") || boost::iequals(stat, "dexterity"))
		return gearStats[eEquipmentStats::Dexterity];
	else if (boost::iequals(stat, "int") || boost::iequals(stat, "intelligence"))
		return gearStats[eEquipmentStats::Intelligence];
	else if (boost::iequals(stat, "wis") || boost::iequals(stat, "wisdom"))
		return gearStats[eEquipmentStats::Wisdom];
	else if (boost::iequals(stat, "stam") || boost::iequals(stat, "stamina"))
		return gearStats[eEquipmentStats::Stamina];
	else if (boost::iequals(stat, "CHD") || boost::iequals(stat, "crit hit damage") || boost::iequals(stat, "critical hit damage"))
		return gearStats[eEquipmentStats::Critical_Hit_Damage];
	else if (boost::iequals(stat, "CC") || boost::iequals(stat, "crit chance") || boost::iequals(stat, "critical chance"))
		return gearStats[eEquipmentStats::Critical_Chance];
	else if (boost::iequals(stat, "def") || boost::iequals(stat, "defense"))
		return gearStats[eEquipmentStats::Defense];
	else if (boost::iequals(stat, "res") || boost::iequals(stat, "resistance"))
		return gearStats[eEquipmentStats::Resistance];
	else
		return 0;
}

double Player::GetStatMod(std::string mod)
{
	if (boost::iequals(mod, "str") || boost::iequals(mod, "strength"))
		return statModifiers[statModifiers::str];
	else if (boost::iequals(mod, "dex") || boost::iequals(mod, "dexterity"))
		return statModifiers[statModifiers::dex];
	else if (boost::iequals(mod, "int") || boost::iequals(mod, "intelligence"))
		return statModifiers[statModifiers::intel];
	else if (boost::iequals(mod, "wis") || boost::iequals(mod, "wisdom"))
		return statModifiers[statModifiers::wis];
	else if (boost::iequals(mod, "stam") || boost::iequals(mod, "stamina"))
		return statModifiers[statModifiers::stam];
	else if (boost::iequals(mod, "def") || boost::iequals(mod, "defense"))
		return statModifiers[statModifiers::def];
	else if (boost::iequals(mod, "res") || boost::iequals(mod, "resistance"))
		return statModifiers[statModifiers::res];
	else if (boost::iequals(mod, "health") || boost::iequals(mod, "hp"))
		return statModifiers[statModifiers::hp];
	else if (boost::iequals(mod, "mana") || boost::iequals(mod, "mp"))
		return statModifiers[statModifiers::mp];
	else if (boost::iequals(mod, "wdmg") || boost::iequals(mod, "weapon damage"))
		return statModifiers[statModifiers::wdmg];
	else if (boost::iequals(mod, "burned") || boost::iequals(mod, "burn"))
		return statModifiers[statModifiers::burned];
	else if (boost::iequals(mod, "drop") || boost::iequals(mod, "drop rate"))
		return statModifiers[statModifiers::res];
	else if (boost::iequals(mod, "xp") || boost::iequals(mod, "exp") || boost::iequals(mod, "experience") || boost::iequals(mod, "xp rate") || boost::iequals(mod, "exp rate") || boost::iequals(mod, "experience rate"))
		return statModifiers[statModifiers::xp];
	else if (boost::iequals(mod, "jp") || boost::iequals(mod, "jp rate"))
		return statModifiers[statModifiers::jp];
	else
		return 0;
}

std::vector<Equipment*> Player::GetInventory()
{
	return playerInventory;
}

void Player::PickUpItem()
{
	for (int i = 0; i < Game::equipmentOnFloor.size(); i++)
	{
		if (Game::equipmentOnFloor.at(i) != NULL)
		{
			sf::Rect<float> itemBB = Game::equipmentOnFloor.at(i)->GetBoundingRect();

			if (itemBB.intersects(GetBoundingRect()))
			{
				playerInventory.push_back(Game::equipmentOnFloor.at(i));
				Game::equipmentOnFloor[i] = Game::equipmentOnFloor.back();
				Game::equipmentOnFloor.pop_back();
			}
		}
	}
}

void Player::DropItem(Equipment& toBeDropped)
{
	Game::equipmentOnFloor.push_back(&toBeDropped);
	for (int i = 0; i < playerInventory.size(); i++)
	{
		if (playerInventory[i] == &toBeDropped && &toBeDropped != NULL)
		{
			toBeDropped.SetPosition(GetPosition());
			toBeDropped.SetScale(1, 1);
			playerInventory[i] = playerInventory.back();
			playerInventory.pop_back();
		}
	}
}

void Player::EquipItem(Equipment& toBeEquipped)
{
	UnequipItem(toBeEquipped.GetType());

	Equipment* equip = &toBeEquipped;
	
	for (int i = 0; i < playerInventory.size(); i++)
	{
		if (playerInventory[i] == equip)
		{
			playerInventory[i] = playerInventory.back();
			playerInventory.pop_back();
		}
	}
	
	gearStats[eEquipmentStats::Strength] += toBeEquipped.GetStat(eEquipmentStats::Strength);
	gearStats[eEquipmentStats::Dexterity] += toBeEquipped.GetStat(eEquipmentStats::Dexterity);
	gearStats[eEquipmentStats::Intelligence] += toBeEquipped.GetStat(eEquipmentStats::Intelligence);
	gearStats[eEquipmentStats::Wisdom] += toBeEquipped.GetStat(eEquipmentStats::Wisdom);
	gearStats[eEquipmentStats::Stamina] += toBeEquipped.GetStat(eEquipmentStats::Stamina);
	gearStats[eEquipmentStats::Critical_Chance] += toBeEquipped.GetStat(eEquipmentStats::Critical_Chance);
	gearStats[eEquipmentStats::Critical_Hit_Damage] += toBeEquipped.GetStat(eEquipmentStats::Critical_Hit_Damage);
	gearStats[eEquipmentStats::Defense] += toBeEquipped.GetStat(eEquipmentStats::Defense);
	gearStats[eEquipmentStats::Resistance] += toBeEquipped.GetStat(eEquipmentStats::Resistance);
	gearStats[eEquipmentStats::Damage] += toBeEquipped.GetStat(eEquipmentStats::Damage);

	UpdateStats();
	playerEquipment[toBeEquipped.GetType()] = equip;
}

void Player::UnequipItem(int pos)
{
	if (playerEquipment[pos] != NULL)
	{
		gearStats[eEquipmentStats::Strength] -= playerEquipment[pos]->GetStat(eEquipmentStats::Strength);
		gearStats[eEquipmentStats::Dexterity] -= playerEquipment[pos]->GetStat(eEquipmentStats::Dexterity);
		gearStats[eEquipmentStats::Intelligence] -= playerEquipment[pos]->GetStat(eEquipmentStats::Intelligence);
		gearStats[eEquipmentStats::Wisdom] -= playerEquipment[pos]->GetStat(eEquipmentStats::Wisdom);
		gearStats[eEquipmentStats::Stamina] -= playerEquipment[pos]->GetStat(eEquipmentStats::Stamina);
		gearStats[eEquipmentStats::Critical_Chance] -= playerEquipment[pos]->GetStat(eEquipmentStats::Critical_Chance);
		gearStats[eEquipmentStats::Critical_Hit_Damage] -= playerEquipment[pos]->GetStat(eEquipmentStats::Critical_Hit_Damage);
		gearStats[eEquipmentStats::Defense] -= playerEquipment[pos]->GetStat(eEquipmentStats::Defense);
		gearStats[eEquipmentStats::Resistance] -= playerEquipment[pos]->GetStat(eEquipmentStats::Resistance);
		gearStats[eEquipmentStats::Damage] -= playerEquipment[pos]->GetStat(eEquipmentStats::Damage);

		playerInventory.push_back(playerEquipment[pos]);
		playerEquipment[pos] = NULL;
	}
}

std::vector<Equipment*> Player::GetEquipment()
{
	return playerEquipment;
}

