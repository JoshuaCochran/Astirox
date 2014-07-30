#include "stdafx.h"
#include "Game.h"
#include "Equipment.hpp"

Equipment::Equipment()
{
	for (int i = 0; i < 10; i++)
	{
		mStats[i] = 0;
	}
}

// Randomly generates a piece of armor
Equipment::Equipment(Monster& monster)
{
	for (int i = 0; i < 10; i++)
	{
		mStats[i] = 0;
	}

	mValue = monster.GetLevel() * 10 * mIRarity;
	DetermineRarity(monster);
	DetermineType(monster);
	if (equipmentType == eEquipmentType::Helmet || equipmentType == eEquipmentType::Armor
		|| equipmentType == eEquipmentType::Offhand)
		RollArmorStats(monster);

	else if (equipmentType == eEquipmentType::Weapon)
		RollWeaponStats(monster);

	else if (equipmentType == eEquipmentType::Accessory)
		RollAccessoryStats(monster);

	GetSprite().setOrigin(GetSprite().getLocalBounds().width / 2, GetSprite().getLocalBounds().height / 2);
	SetPosition(monster.GetPosition().x, monster.GetPosition().y);
}

int Equipment::GetStat(int stat)
{
	return mStats[stat];
}

void Equipment::SetStat(int stat, int num)
{
	mStats[stat] += num;
}

void Equipment::DetermineRarity(Monster& monster)
{
	double COMMON_DROP_CHANCE = 0;
	double UNCOMMON_DROP_CHANCE = 0;
	double RARE_DROP_CHANCE = 0;
	double EPIC_DROP_CHANCE = 0;
	double UNIQUE_DROP_CHANCE = 0;
	int RARITY = 0;

	if (monster.IsMiniBoss())
	{
		// Do stuff
	}
	else if (monster.IsMainBoss())
	{
		// Do stuff
	}
	else
	{
		UNCOMMON_DROP_CHANCE = 0.30;

		if (monster.GetLevel() > 10)
			RARE_DROP_CHANCE = 0.15;
		if (monster.GetLevel() > 20)
			EPIC_DROP_CHANCE = 0.05;
		if (monster.GetLevel() > 30)
			UNIQUE_DROP_CHANCE = 0.0001;

		COMMON_DROP_CHANCE = 1 - (UNCOMMON_DROP_CHANCE
			+ RARE_DROP_CHANCE + EPIC_DROP_CHANCE + UNIQUE_DROP_CHANCE);

		boost::random::discrete_distribution<> dist({ COMMON_DROP_CHANCE, UNCOMMON_DROP_CHANCE, RARE_DROP_CHANCE, EPIC_DROP_CHANCE, UNIQUE_DROP_CHANCE });
		RARITY = dist(Game::rng);
	}
	if (RARITY == eRarity::Common) mSRarity = "Common";
	else if (RARITY == eRarity::Uncommon) mSRarity = "Uncommon";
	else if (RARITY == eRarity::Rare) mSRarity = "Rare";
	else if (RARITY == eRarity::Epic) mSRarity = "Epic";
	else if (RARITY == eRarity::Unique) mSRarity = "Unique";
	mIRarity = RARITY;
}

void Equipment::DetermineType(Monster& monster)
{
	double WEAPON_CHANCE = 0.15;
	double OFFHAND_CHANCE = 0.25;
	double ARMOR_CHANCE = 0.25;
	double HELMET_CHANCE = 0.25;
	double ACCESSORY_CHANCE = 0.10;

	if (monster.GetLevel() < 10)
	{
		ARMOR_CHANCE += ACCESSORY_CHANCE;
		ACCESSORY_CHANCE = 0;
	}
	
	boost::random::discrete_distribution<> dist({ WEAPON_CHANCE, OFFHAND_CHANCE, ARMOR_CHANCE, HELMET_CHANCE, ACCESSORY_CHANCE });
	equipmentType = dist(Game::rng);
}

void Equipment::RollArmorStats(Monster& monster)
{
	int None = 0;

	boost::random::uniform_real_distribution<> uniformRealDist(0.75, 1.25);
	boost::random::uniform_int_distribution<> uniformIntDist(0, 6);
	std::string suffix = "";

	int stat1 = uniformIntDist(Game::rng);
	int stat2 = uniformIntDist(Game::rng); 
	
	int statValue1 = 0;
	int statValue2 = 0;
	int defenseValue = 0;
	int bonusDefenseValue = 0;

	if (equipmentType == eEquipmentType::Armor)
	{
		statValue1 = monster.GetLevel() / 5 * uniformRealDist(Game::rng);
		statValue2 = monster.GetLevel() / 5 * uniformRealDist(Game::rng);
		defenseValue = (int)(pow(monster.GetLevel(), 2) * uniformRealDist(Game::rng));
		bonusDefenseValue = monster.GetLevel() * 3 * uniformRealDist(Game::rng);
	}
	else if (equipmentType == eEquipmentType::Helmet)
	{
		statValue1 = monster.GetLevel() / 8 * uniformRealDist(Game::rng);
		statValue2 = monster.GetLevel() / 8 * uniformRealDist(Game::rng);
		defenseValue = (int)(pow(monster.GetLevel(), 1.5) * uniformRealDist(Game::rng));
		bonusDefenseValue = monster.GetLevel() * 2 * uniformRealDist(Game::rng);
	}


	if (mIRarity == eRarity::Common)
	{
		mStats[eEquipmentStats::Defense] += defenseValue;
	}

	else if (mIRarity == eRarity::Uncommon)
	{
		if (stat1 == None || stat2 == None || stat1 == stat2)
		{
			if (stat1 == None)
				stat1 = stat2;
			else
				stat2 = stat1;

			if (stat1 == eArmorStats::Strength)
			{
				suffix = " of Strength";
				mStats[eEquipmentStats::Strength] = statValue1;
			}
			else if (stat1 == eArmorStats::Dexterity)
			{
				suffix = " of Dexterity";
				mStats[eEquipmentStats::Dexterity] = statValue1;
			}
			else if (stat1 == eArmorStats::Intelligence)
			{
				suffix = " of Intelligence";
				mStats[eEquipmentStats::Intelligence] = statValue1;
			}
			else if (stat1 == eArmorStats::Wisdom)
			{
				suffix = " of Wisdom";
				mStats[eEquipmentStats::Wisdom] = statValue1;
			}
			else if (stat1 == eArmorStats::Stamina)
			{
				suffix = " of Stamina";
				mStats[eEquipmentStats::Stamina] = statValue1;
			}
			else if (stat1 == eArmorStats::Defense)
			{
				suffix = " of Defense";
				mStats[eEquipmentStats::Defense] += bonusDefenseValue;
			}
			else if (stat1 == eArmorStats::Resistance)
			{
				suffix = " of Reistance";
				mStats[eEquipmentStats::Resistance] = bonusDefenseValue;
			}
		}
		else if ((stat1 == eArmorStats::Strength && stat2 == eArmorStats::Dexterity)
			|| (stat1 == eArmorStats::Dexterity && stat2 == eArmorStats::Strength))
		{
			suffix = " of the Tiger";
			mStats[eEquipmentStats::Strength] = statValue1;
			mStats[eEquipmentStats::Dexterity] = statValue2;
		}
		else if ((stat1 == eArmorStats::Strength && stat2 == eArmorStats::Stamina)
			|| (stat1 == eArmorStats::Stamina && stat2 == eArmorStats::Strength))
		{
			suffix = " of the Bear";
			mStats[eEquipmentStats::Strength] = statValue1;
			mStats[eEquipmentStats::Stamina] = statValue2;
		}
		else if ((stat1 == eArmorStats::Strength && stat2 == eArmorStats::Intelligence)
			|| (stat1 == eArmorStats::Intelligence && stat2 == eArmorStats::Strength))
		{
			suffix = " of the Gorilla";
			mStats[eEquipmentStats::Strength] = statValue1;
			mStats[eEquipmentStats::Intelligence] = statValue2;
		}
		else if ((stat1 == eArmorStats::Strength && stat2 == eArmorStats::Wisdom)
			|| (stat1 == eArmorStats::Wisdom && stat2 == eArmorStats::Strength))
		{
			suffix = " of the Boar";
			mStats[eEquipmentStats::Strength] = statValue1;
			mStats[eEquipmentStats::Wisdom] = statValue2;
		}
		else if ((stat1 == eArmorStats::Dexterity && stat2 == eArmorStats::Stamina)
			|| (stat1 == eArmorStats::Stamina && stat2 == eArmorStats::Dexterity))
		{
			suffix = " of the Monkey";
			mStats[eEquipmentStats::Dexterity] = statValue1;
			mStats[eEquipmentStats::Stamina] = statValue2;
		}
		else if ((stat1 == eArmorStats::Dexterity && stat2 == eArmorStats::Wisdom)
			|| (stat1 == eArmorStats::Wisdom && stat2 == eArmorStats::Dexterity))
		{
			suffix = " of the Wolf";
			mStats[eEquipmentStats::Dexterity] = statValue1;
			mStats[eEquipmentStats::Wisdom] = statValue2;
		}
		else if ((stat1 == eArmorStats::Intelligence && stat2 == eArmorStats::Stamina)
			|| (stat1 == eArmorStats::Stamina && stat2 == eArmorStats::Intelligence))
		{
			suffix = " of the Eagle";
			mStats[eEquipmentStats::Intelligence] = statValue1;
			mStats[eEquipmentStats::Stamina] = statValue2;
		}
		else if ((stat1 == eArmorStats::Wisdom && stat2 == eArmorStats::Stamina)
			|| (stat1 == eArmorStats::Stamina && stat2 == eArmorStats::Wisdom))
		{
			suffix = " of the Whale";
			mStats[eEquipmentStats::Wisdom] = statValue1;
			mStats[eEquipmentStats::Stamina] = statValue2;
		}
		else if ((stat1 == eArmorStats::Intelligence && stat2 == eArmorStats::Wisdom)
			|| (stat1 == eArmorStats::Wisdom && stat2 == eArmorStats::Intelligence))
		{
			suffix = " of the Owl";
			mStats[eEquipmentStats::Intelligence] = statValue1;
			mStats[eEquipmentStats::Wisdom] = statValue2;
		}
		mStats[eEquipmentStats::Defense] += defenseValue;
	}

	int imageSize = 16;
	boost::random::uniform_int_distribution<> StrArmor(0, 11);
	boost::random::uniform_int_distribution<> DexArmor(0, 5);
	boost::random::uniform_int_distribution<> IntArmor(0, 7);
	boost::random::uniform_int_distribution<> StamArmor(0, 7);
	boost::random::uniform_int_distribution<> WisArmor(0, 6);
	
	boost::random::uniform_int_distribution<> Helm1(0, 5);
	boost::random::uniform_int_distribution<> Helm2(0, 2);
	boost::random::uniform_int_distribution<> Helm3(0, 2);
	boost::random::uniform_int_distribution<> Helm4(0, 2);
	if (equipmentType == eEquipmentType::Armor)
	{
		mName = ("Armor" + suffix);
		if (mStats[eEquipmentStats::Strength] > 0)
		{
			Load("data/images/items/Armor.png", imageSize * StrArmor(Game::rng), 0, 16, 16);
			assert(IsLoaded());
		}
		else if (mStats[eEquipmentStats::Dexterity] > 0)
		{
			Load("data/images/items/Armor.png", imageSize * DexArmor(Game::rng), 8, 16, 16);
			assert(IsLoaded());
		}
		else if (mStats[eEquipmentStats::Intelligence] > 0)
		{
			Load("data/images/items/Armor.png", imageSize * IntArmor(Game::rng), 4, 16, 16);
			assert(IsLoaded());
		}
		else if (mStats[eEquipmentStats::Stamina] > 0)
		{
			Load("data/images/items/Armor.png", imageSize * StamArmor(Game::rng), 6, 16, 16);
			assert(IsLoaded());
		}
		else if (mStats[eEquipmentStats::Wisdom] > 0)
		{
			Load("data/images/items/Armor.png", imageSize * WisArmor(Game::rng), 7, 16, 16);
			assert(IsLoaded());
		}
		else
		{
			Load("data/images/items/Armor.png", 0, 2, 16, 16);
			assert(IsLoaded());
		}
	}
	else if (equipmentType == eEquipmentType::Offhand) 
	{
		mName = ("Offhand" + suffix);
		Load("data/images/items/Glove.png", 0, 0, 16, 16);
		assert(IsLoaded());
	}
	else if (equipmentType == eEquipmentType::Helmet)
	{
		mName = ("Helmet" + suffix);
		if (mStats[eEquipmentStats::Strength] > 0 || (mStats[eEquipmentStats::Strength] > 0 && mStats[eEquipmentStats::Stamina] > 0))
		{
			Load("data/images/items/Hat.png", imageSize * Helm1(Game::rng), 0, 16, 16);
			assert(IsLoaded());
		}
		else if (mStats[eEquipmentStats::Dexterity])
		{
			Load("data/images/items/Hat.png", imageSize * Helm2(Game::rng), 1, 16, 16);
			assert(IsLoaded());
		}
		else if (mStats[eEquipmentStats::Intelligence] > 0 || mStats[eEquipmentStats::Wisdom] > 0)
		{
			Load("data/images/items/Hat.png", imageSize * Helm3(Game::rng), 2, 16, 16);
			assert(IsLoaded());
		}
		else
		{
			Load("data/images/items/Hat.png", imageSize * Helm4(Game::rng), 3, 16, 16);
			assert(IsLoaded());
		}
	}
}

void Equipment::RollWeaponStats(Monster& monster)
{
	int None = 0;

	boost::random::uniform_real_distribution<> uniformRealDist(0.75, 1.25);
	boost::random::uniform_int_distribution<> uniformIntDist(2, 8);

	int imageSize = 16;
	boost::random::uniform_int_distribution<> weaponImage(0, 5);

	mStats[eEquipmentStats::Damage] = (int)(pow(2, monster.GetLevel()) * uniformRealDist(Game::rng));

	mName = ("Weapon");
	Load("data/images/items/LongWep.png", imageSize * weaponImage(Game::rng), 1, 16, 16);
	assert(IsLoaded());
}

void Equipment::RollAccessoryStats(Monster& monster)
{
	boost::random::uniform_real_distribution<> uniformRealDist(0.75, 1.25);
	boost::random::uniform_int_distribution<> uniformIntDist(2, 8);

	int stat1 = uniformIntDist(Game::rng);
	int stat2 = uniformIntDist(Game::rng);

	int	statValue1 = monster.GetLevel() / 5 * uniformRealDist(Game::rng);
	int statValue2 = monster.GetLevel() / 5 * uniformRealDist(Game::rng);

	mStats[stat1] += statValue1;
	mStats[stat2] += statValue2;

	int imageSize = 16;
	boost::random::uniform_int_distribution<> accessoryImage(0, 7);

	mName = ("Accessory");
	Load("data/images/items/Amulet.png", imageSize * accessoryImage(Game::rng), 0, 16, 16);
	assert(IsLoaded());


}

std::string Equipment::GetName()
{
	return mName;
}
int Equipment::GetValue()
{
	return mValue;
}

int Equipment::GetRarity()
{
	return mIRarity;
}

int Equipment::GetType()
{
	return equipmentType;
}