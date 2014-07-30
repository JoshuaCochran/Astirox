#ifndef _EQUIPMENT_H
#define _EQUIPMENT_H

#include "VisibleGameObject.h"
#include "Monster.h"

class Monster;



namespace eEquipmentStats
{
	enum eEquipmentStats { Damage, Strength, Dexterity, Intelligence, Wisdom, Stamina, Critical_Chance, Critical_Hit_Damage, Defense, Resistance };
	const int NUMBER_OF_STATS = 11;
}
namespace eArmorStats
{
	enum eArmorStats { Strength, Dexterity, Intelligence, Wisdom, Stamina, Defense, Resistance };
}

namespace eEquipmentType
{
	enum eEquipmentType {Weapon, Offhand, Armor, Helmet, Accessory};
}
enum eRarity { Common, Uncommon, Rare, Epic, Unique };

class Equipment
	: public VisibleGameObject
{
private:
	int mStats[10];
	int mIRarity;
	std::string mSRarity;
	std::string mName;
	int equipmentType;
	int mValue;

public:
	Equipment();
	Equipment(Monster& monster);

	int GetStat(int stat);
	void SetStat(int stat, int num);
	int GetType();
	std::string GetName();
	int GetValue();
	int GetRarity();
	void RollArmorStats(Monster& monster);
	void RollWeaponStats(Monster& monster);
	void RollAccessoryStats(Monster& monster);
	void DetermineRarity(Monster& monster);
	void DetermineType(Monster& monster);
	
};

#endif