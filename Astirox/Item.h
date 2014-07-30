#ifndef _ITEM_H
#define _ITEM_H

#include "Player.h"
#include "Monster.h"

/*enum itemSlot {Weapon, Offhand, Chest, Helmet, Accessory};
enum weaponType {Sword, Dagger, Axe, Polearm, Bow, Stave, Shield};
enum armorType {Light, Medium, Heavy, None};*/

class Player;
class Monster;

class Item
	: public VisibleGameObject
{
private:
	std::string mName;
	int mValue;

public:
	Item();
	Item(std::string filename, sf::Texture texture, sf::IntRect textureCoords);

	void SetName(std::string name);
	std::string GetName();
	void SetValue(int value);
	int GetValue();

	void GenerateLoot(Player& player, Monster& monster);

	/*virtual std::string GetName() const;
	virtual int GetSlot() const;
	virtual int GetValue() const;
	
	virtual void SetName(std::string name);
	
	virtual void Equip();
	virtual void Unequip();

	void itemEffect();*/
};

#endif