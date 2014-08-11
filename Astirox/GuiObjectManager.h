#ifndef _GUIOBJECTMANAGER_H
#define _GUIOBJECTMANAGER_H

#include "VisibleGameObject.h"
#include "Battle.h"
#include "Player.h"
#include "Monster.h"

class GuiObjectManager
{
public:
	GuiObjectManager();
	~GuiObjectManager();
	
	void Init();
	
	void Add(std::string name, VisibleGameObject* guiObject);
	
	const static sf::Event& GetInput();
	VisibleGameObject* Get(std::string name) const;
	
	void DrawOOCGUI(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player);
	bool DrawCombatGUI(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Battle& battle);
	void DrawHUD(sf::RenderWindow& renderWindow, sf::Event currentEvent, Player& player, sf::Vector2f HUDpos, bool selected);
	void DrawMonsterHUD(sf::RenderWindow& renderWindow, Monster& monster, sf::Vector2f HUDpos, bool selected);
	bool DrawInventory(sf::RenderWindow& renderWindow, sf::Event currentEvent, Player& player);
	void DrawStatMenu(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player);
	void DrawStatButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player, int posx, int posy, int stat);
	void DrawItemStats(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player);
	void DrawEquippedItems(sf::RenderWindow& renderWindow, Player& player);
	void UseItemButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player);
	void DropItemButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player);
	bool DrawAttackButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Battle& battle, int posx, int posy, int stat);

	void UpdateResources(sf::RenderWindow& renderWindow, Player& player);
	void UpdateMonsterResources(sf::RenderWindow& renderWindow, Monster& monster);
	
	bool isInventoryDrawn();

	void UpdateHp(int percentHP);
	void UpdateMp(int percentMP);
	void UpdateXp(int percentXP);

private:
	std::map<std::string, VisibleGameObject*> _guiObjects;

	sf::Texture _statButtonTexture;


	Equipment* mSelectedItem;

	bool _inventory_is_drawn;
	bool _movingInventory;


	bool _statmenu_is_drawn;
	bool _movingStatMenu;

	bool _moving_player_HUD;

	struct GuiObjectDeallocator
	{
		void operator() (const std::pair<std::string, VisibleGameObject*> & p) const
		{
			delete p.second;
		}
	};
};

#endif