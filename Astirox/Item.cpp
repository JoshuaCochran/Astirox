#include "stdafx.h"
#include "Item.h"
#include "Game.h"

Item::Item()
{

}

Item::Item(std::string filename, sf::Texture texture, sf::IntRect textureCoords)
{
	Load(texture, textureCoords);
}

/*CheckDrop(Player& player, Monster& monster)
{
	int randNumberOfDrops = rand() % 100;
	int numberOfDrops = 0;
	enum {Weapon, Offhand, Armor, Helmet, Accessory, Misc};
	int randDroppedItemSlot = rand() % 100;
	std::string droppedItemSlot = 0;
	int randEquipmentType = rand() % 100;
	
	if (randNumberOfDrops < 50)
		numberOfDrops = 1;
	else if (randNumberOfDrops < 60 && randNumberOfDrops > 50)
		numberOfDrops = 2;
	else numberOfDrops = 0;

	if (numberOfDrops > 0)
	{
		if (randDroppedItemSlot < 60)
			droppedItemSlot = "Misc";
		else if (randEquipmentType <= 25)
			droppedItemSlot = "Helmet";
		else if (randEquipmentType > 25 && randEquipmentType <= 50)
			droppedItemSlot = "Armor";
		else if (randEquipmentType > 50 && randEquipmentType <= 75)
			droppedItemSlot = "Offhand";
		else if (randEquipmentType <= 89)
			droppedItemSlot = "Weapon";
		else
			droppedItemSlot = "Accessory";
	}
	std::stringstream ss;
	ss << "data/items/" << droppedItemSlot << "/" << monster.GetLevel() << "/" << 

}*/
void Item::SetName(std::string name)
{
	mName = name;
}

std::string Item::GetName()
{
	return mName;
}

void Item::SetValue(int value)
{
	mValue = value;
}

int Item::GetValue()
{
	return mValue;
}

/*void GenerateLoot(Player& player, Monster& monster)
{
	boost::random::discrete_distribution<> dist({ 0.5, 0.25, 0.25, 0.1, 0.1, 0.05 });
	
	int _itemType = dist(rng);

	if (_itemType == 0)
		return;
	else if (_itemType == 1)
		GenerateHelmet();
	else if (_itemType == 2)
		GenerateArmor();
	else if (_itemType == 3)
		GenerateOffhand();
	else if (_itemType == 4)
		GenerateWeapon();
	else if (_itemType == 5)
		GenerateAccessory();
}*/
