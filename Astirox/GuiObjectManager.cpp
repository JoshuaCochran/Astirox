#include "stdafx.h"
#include "GuiObjectManager.h"
#include "Game.h"
#include "Battle.h"

GuiObjectManager::GuiObjectManager()
{
}

GuiObjectManager::~GuiObjectManager()
{
	std::for_each(_guiObjects.begin(), _guiObjects.end(), GuiObjectDeallocator());
}

void GuiObjectManager::Init()
{
	_inventory_is_drawn = false;
	_movingInventory = false;

	_statmenu_is_drawn = false;
	_movingStatMenu = false;
	_moving_player_HUD = false;

	mSelectedItem = NULL;

	_statButtonTexture.loadFromFile("data/green_left_button.png");
	
	// Init HUD
	int hud_pos_x = 792;
	int hp_pos_x = hud_pos_x + 76;
	int mp_pos_x = hp_pos_x;
	int xp_pos_x = mp_pos_x;
	int portrait_pos_x = hud_pos_x + 20;
	
	int hud_pos_y = 0;
	int hp_pos_y = hud_pos_y + 11;
	int mp_pos_y = hp_pos_y + 20;
	int xp_pos_y = mp_pos_y + 20;
	int portrait_pos_y = hud_pos_y + 18;
	VisibleGameObject *playerHud = new VisibleGameObject("data/images/gui/player hud rbg.png");
	playerHud->SetPosition(hud_pos_x, hud_pos_y);

	VisibleGameObject *hudSelected = new VisibleGameObject("data/images/gui/player hud-selected.png");

	VisibleGameObject *hpBar = new VisibleGameObject("data/images/gui/hp bar.png");
	hpBar->SetPosition(hp_pos_x, hp_pos_y);

	VisibleGameObject *mpBar = new VisibleGameObject("data/images/gui/mana bar.png");
	mpBar->SetPosition(mp_pos_x, mp_pos_y);

	VisibleGameObject *expBar = new VisibleGameObject("data/images/gui/exp bar.png");
	expBar->SetPosition(xp_pos_x, xp_pos_y);

	VisibleGameObject *characterPortrait = new VisibleGameObject("data/images/player/humanoid1.png", 0, 16, 16, 16);
	characterPortrait->SetPosition(portrait_pos_x, portrait_pos_y);
	characterPortrait->SetScale(2, 2);

	VisibleGameObject *playerInventory = new VisibleGameObject("data/images/gui/inventory.png");
	playerInventory->SetPosition(792, 426);
	playerInventory->SetScale(2, 2);

	VisibleGameObject *playerInventorySlots = new VisibleGameObject("data/images/gui/inventory slots.png");
	playerInventorySlots->SetPosition(804, 506);
	playerInventorySlots->SetScale(2, 2);

	VisibleGameObject *textBox = new VisibleGameObject("data/textbox_no_center.png");
	textBox->SetPosition(0, 602);
	textBox->SetScale(2, 2);

	VisibleGameObject *statMenu = new VisibleGameObject("data/statmenu_no_title.png");
	statMenu->SetPosition(792, 76);
	statMenu->SetScale(2, 2);

	VisibleGameObject *monsterHud = new VisibleGameObject("data/images/gui/player hud rbg.png");
	monsterHud->SetPosition(0, 0);

	VisibleGameObject *monsterHPBar = new VisibleGameObject("data/images/gui/hp bar.png");
	monsterHPBar->SetPosition(76, 11);

	VisibleGameObject *monsterMPBar = new VisibleGameObject("data/images/gui/mana bar.png");
	monsterMPBar->SetPosition(76, 31);

	VisibleGameObject *monsterXPBar = new VisibleGameObject("data/images/gui/exp bar.png");
	monsterXPBar->SetPosition(76, 18);

	VisibleGameObject *str_button_unactive = new VisibleGameObject(_statButtonTexture, sf::IntRect(0, 0, 14, 14));
	str_button_unactive->SetPosition(942, 116);

	VisibleGameObject *str_button_active = new VisibleGameObject(_statButtonTexture, sf::IntRect(14, 0, 14, 14));
	str_button_active->SetPosition(942, 116);

	VisibleGameObject *str_button_pressed = new VisibleGameObject(_statButtonTexture, sf::IntRect(28, 0, 14, 14));
	str_button_pressed->SetPosition(942, 116);

	VisibleGameObject *textBoxBorder = new VisibleGameObject("data/images/gui/text box border.png");
	textBoxBorder->SetScale(2, 2);

	VisibleGameObject *itemSelect = new VisibleGameObject("data/images/gui/item select.png");
	itemSelect->SetOrigin(itemSelect->GetWidth() / 2, itemSelect->GetHeight() / 2);

	VisibleGameObject *itemUseButton = new VisibleGameObject("data/images/gui/use button.png");
	itemUseButton->SetPosition(936, 634);
	itemUseButton->SetScale(2, 2);

	VisibleGameObject *itemDropButton = new VisibleGameObject("data/images/gui/drop button.png");
	itemDropButton->SetPosition(936, 638);
	itemDropButton->SetScale(2, 2);

	this->Add("ahud", playerHud);
	this->Add("portrait", characterPortrait);
	this->Add("hp bar", hpBar);
	this->Add("mp bar", mpBar);
	this->Add("xp bar", expBar);
	this->Add("inventory", playerInventory);
	this->Add("inventory slots", playerInventorySlots);
	//this->Add("text box", textBox);
	this->Add("stat menu", statMenu);
	this->Add("aMonsterHUD", monsterHud);
	this->Add("monster HP bar", monsterHPBar);
	this->Add("monster MP bar", monsterMPBar);
	this->Add("monster XP bar", monsterXPBar);
	this->Add("left green button unactive", str_button_unactive);
	this->Add("left green button active", str_button_active);
	this->Add("left green button pressed", str_button_pressed);
	this->Add("text box border", textBoxBorder);
	this->Add("item select box", itemSelect);
	this->Add("use button", itemUseButton);
	this->Add("drop button", itemDropButton);
	this->Add("ahud selected", hudSelected);

	// Battle Specific GUI
	VisibleGameObject *spellIconBorder = new VisibleGameObject("data/border.png");
	VisibleGameObject *iconTest = new VisibleGameObject("data/rogue.png");
	iconTest->SetScale(0.25f, 0.25f);
	VisibleGameObject *combatBackground = new VisibleGameObject("data/images/map backgrounds/gold_shifter_dunes.png");
	combatBackground->SetScale(1.5f, 1.5f);

	this->Add("spell icon border", spellIconBorder);
	this->Add("icon test", iconTest);
	this->Add("combat background", combatBackground);
}

void GuiObjectManager::Add(std::string name, VisibleGameObject* guiObject)
{
	_guiObjects.insert(std::pair<std::string, VisibleGameObject*>(name, guiObject));
}

VisibleGameObject* GuiObjectManager::Get(std::string name) const
{
	std::map<std::string, VisibleGameObject*>::const_iterator results = _guiObjects.find(name);
	if (results == _guiObjects.end())
		return NULL;
	return results->second;
}


void GuiObjectManager::DrawOOCGUI(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player)
{
	DrawInventory(renderWindow, currentEvent, player);
	DrawStatMenu(renderWindow, currentEvent, player);
	DrawHUD(renderWindow, currentEvent, player, sf::Vector2f(792, 70), false);

	std::stringstream ss;
	ss << "(" << player.GetPosition().x << "," << player.GetPosition().y << ")";
	sf::Text text(ss.str(), Game::font);
	text.setCharacterSize(20);
	text.setStyle(sf::Text::Bold);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	text.setPosition(player.GetPosition().x, player.GetPosition().y + player.GetHeight());
	renderWindow.draw(text);
}

//trash
bool GuiObjectManager::DrawCombatGUI(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Battle& battle)
{
	Get("combat background")->Draw(renderWindow);
	for (int i = 0; i < battle.GetMonsterParty().size(); i++)
	{
		if (battle.GetActiveEntity().isfriendly()
			&& battle.GetMonsterParty()[i]->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
		{
			if (currentEvent.type == sf::Event::MouseButtonReleased)
				battle.SetTarget(battle.GetMonsterParty()[i], false);
		}
		if (!battle.GetTarget().isfriendly() 
			&& &battle.GetTarget() == battle.GetMonsterParty()[i])
			DrawMonsterHUD(renderWindow, *battle.GetMonsterParty()[i], sf::Vector2f(0, i * 72), true);
		else DrawMonsterHUD(renderWindow, *battle.GetMonsterParty()[i], sf::Vector2f(0, i * 72), false);
	}
	DrawAttackButton(renderWindow, currentEvent, battle, 0, Game::SCREEN_HEIGHT - 136, 0);
	return true;
}

bool GuiObjectManager::DrawAttackButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Battle& battle, int posx, int posy, int stat)
{
	Get("spell icon border")->SetPosition(posx, posy);
	Get("icon test")->SetPosition(posx + 2, posy);
	if (Get("spell icon border")->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
	{
		Player* player1 = dynamic_cast<Player*>(&battle.GetActiveEntity());
		if (currentEvent.type == sf::Event::MouseButtonReleased && battle.GetActiveEntity().isfriendly() && player1 != NULL)
		{
			//battle.GetDamageText().setString(BasicPhysAttack(*battle.GetPlayer(), *battle.GetMonster()));
			battle.GetDamageText().setString(player1->GetSpellInventory()[0](*player1, battle.GetTarget()));
			battle.GetDamageText().setPosition(battle.GetTarget().GetPosition().x, battle.GetTarget().GetPosition().y - 32);
			Game::GetAnimationManager().SetSpellAnimation(Game::GetAnimationManager().GetAnimation("fire lion left"));
			Game::GetAnimationManager().GetSpellSprite()->setPosition(player1->GetPosition().x - 128, player1->GetPosition().y - 64);
			battle.GetTextFadeClock().restart();
			battle.GetTurnTimer().restart();
			battle.GetSpellFrameClock().restart();
			battle.ClearTarget();
			battle.SetActiveEntity(battle.GetMonsterParty()[0], false);
			if (battle.GetMonsterParty()[0]->GetStat(Stats::curHP) <= 0)
				battle.SetActiveEntity(battle.GetMonsterParty()[1], false);

			int monsterDeadCount = 0;
			for (int i = 0; i < battle.GetMonsterParty().size(); i++)
			{
				if (battle.GetMonsterParty()[i]->GetStat(Stats::curHP) <= 0)
					monsterDeadCount++;
			}
			if (monsterDeadCount == battle.GetMonsterParty().size())
				battle.ClearActiveEntity();

			return true;
		}
	}
	Get("icon test")->Draw(renderWindow);
	Get("spell icon border")->Draw(renderWindow);
	return false;
}

void GuiObjectManager::UpdateResources(sf::RenderWindow& renderWindow, Player& player)
{
	_guiObjects.find("hp bar")->second->SetScale(1.0f * player.GetStat(Stats::curHP) / player.GetStat(Stats::maxHP), 1);
	_guiObjects.find("mp bar")->second->SetScale(1.0f * player.GetStat(Stats::curMP) / player.GetStat(Stats::maxMP), 1);
	_guiObjects.find("xp bar")->second->SetScale(1.0f * player.GetStat(Stats::curXP) / player.GetStat(Stats::xpNeeded), 1);
}

void GuiObjectManager::UpdateMonsterResources(sf::RenderWindow& renderWindow, Monster& monster)
{
	_guiObjects.find("monster HP bar")->second->SetScale(1.0f * monster.GetStat(Stats::curHP) / monster.GetStat(Stats::maxHP), 1);
	//_guiObjects.find("mp bar")->second->SetScale(1.0f * player.GetStat(Stats::curMP) / player.GetStat(Stats::maxMP), 1);
}

void GuiObjectManager::DrawHUD(sf::RenderWindow& renderWindow, sf::Event currentEvent, Player& player, sf::Vector2f HUDpos, bool selected)
{

	sf::Rect<float> BoundingRect(Get("ahud")->GetPosition(), sf::Vector2f(Get("ahud")->GetWidth() * 2.0f, Get("ahud")->GetHeight() * 2.0f));

	if (BoundingRect.contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
	{
		if (currentEvent.type == sf::Event::MouseButtonPressed)
		{
			if (!_movingInventory && !_movingStatMenu)
			_moving_player_HUD = true;
		}
	}
	if (currentEvent.type == sf::Event::MouseButtonReleased)
		_moving_player_HUD = false;

	if (_moving_player_HUD)
	{
		_guiObjects.find("ahud")->second->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(renderWindow)));
		
		if (Get("ahud")->GetPosition().x > Game::SCREEN_WIDTH - Get("ahud")->GetWidth())
			Get("ahud")->SetPosition(Game::SCREEN_WIDTH - Get("ahud")->GetWidth(), Get("ahud")->GetPosition().y);
		else if (Get("ahud")->GetPosition().x < 0)
			Get("ahud")->SetPosition(0, Get("ahud")->GetPosition().y);
		
		if (Get("ahud")->GetPosition().y > Game::SCREEN_HEIGHT - Get("ahud")->GetHeight())
			Get("ahud")->SetPosition(Get("ahud")->GetPosition().x, Game::SCREEN_HEIGHT - Get("ahud")->GetHeight());
		else if (Get("ahud")->GetPosition().y < 0)
			Get("ahud")->SetPosition(Get("ahud")->GetPosition().x, 0);

	}
	Get("ahud")->SetPosition(HUDpos);
	Get("ahud selected")->SetPosition(Get("ahud")->GetPosition());
	int hp_pos_x = Get("ahud")->GetPosition().x + 76;
	int mp_pos_x = hp_pos_x;
	int xp_pos_x = mp_pos_x;
	int portrait_pos_x = Get("ahud")->GetPosition().x + 20;

	int hp_pos_y = Get("ahud")->GetPosition().y + 11;
	int mp_pos_y = hp_pos_y + 20;
	int xp_pos_y = mp_pos_y + 20;
	int portrait_pos_y = Get("ahud")->GetPosition().y + 18;

	Get("hp bar")->SetPosition(hp_pos_x, hp_pos_y);
	Get("mp bar")->SetPosition(mp_pos_x, mp_pos_y);
	Get("xp bar")->SetPosition(xp_pos_x, xp_pos_y);
	Get("portrait")->SetPosition(portrait_pos_x, portrait_pos_y);

	UpdateResources(renderWindow, player);

	_guiObjects.find("ahud")->second->Draw(renderWindow);
	_guiObjects.find("portrait")->second->Draw(renderWindow);
	_guiObjects.find("hp bar")->second->Draw(renderWindow);
	_guiObjects.find("mp bar")->second->Draw(renderWindow);
	_guiObjects.find("xp bar")->second->Draw(renderWindow);//*/
	//_guiObjects.find("text box")->second->Draw(renderWindow);

	std::stringstream ss;

	ss << player.GetStat(Stats::curHP) << "/" << player.GetStat(Stats::maxHP);
	sf::Text text(ss.str(), Game::font);
	text.setCharacterSize(10);
	text.setColor(sf::Color::Black);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	text.setPosition(_guiObjects.find("hp bar")->second->GetPosition().x + _guiObjects.find("hp bar")->second->GetWidth() /2, _guiObjects.find("hp bar")->second->GetPosition().y + 2);
	Game::GetWindow().draw(text);

	ss.str(std::string());
	ss << player.GetStat(Stats::curMP) << "/" << player.GetStat(Stats::maxMP);
	text.setString(ss.str());
	text.setPosition(_guiObjects.find("mp bar")->second->GetPosition().x + _guiObjects.find("mp bar")->second->GetWidth() / 2, _guiObjects.find("mp bar")->second->GetPosition().y + 2);
	Game::GetWindow().draw(text);

	ss.str(std::string());
	ss << player.GetStat(Stats::curXP) << "/" << player.GetStat(Stats::xpNeeded);
	text.setString(ss.str());
	text.setPosition(_guiObjects.find("xp bar")->second->GetPosition().x + _guiObjects.find("xp bar")->second->GetWidth() / 2, _guiObjects.find("xp bar")->second->GetPosition().y + 2);
	Game::GetWindow().draw(text);
}

void GuiObjectManager::DrawStatMenu(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player)
{

	if (currentEvent.type == sf::Event::KeyReleased
		&& currentEvent.key.code == sf::Keyboard::S)
	{
		if (_statmenu_is_drawn)
			_statmenu_is_drawn = false;
		else
			_statmenu_is_drawn = true;
	}
	if (_statmenu_is_drawn)
	{
		//sf::RectangleShape testShape(sf::Vector2f(Get("stat menu")->GetWidth() * 2.0f, Get("stat menu")->GetHeight() * 2.0f));
		//testShape.setPosition(Get("stat menu")->GetPosition());
		sf::Rect<float> BoundingRect(Get("stat menu")->GetPosition(), sf::Vector2f(Get("stat menu")->GetWidth() * 1.5f, Get("stat menu")->GetHeight() * 2.0f));

		if (BoundingRect.contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
		{
			if (currentEvent.type == sf::Event::MouseButtonPressed)
			{
				if (!_movingInventory && !_moving_player_HUD)
				_movingStatMenu = true;
			}
		}
		if (currentEvent.type == sf::Event::MouseButtonReleased)
			_movingStatMenu = false;

		if (_movingStatMenu)
		{
			_guiObjects.find("stat menu")->second->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(renderWindow)));

			if (Get("stat menu")->GetPosition().x > Game::SCREEN_WIDTH - Get("stat menu")->GetWidth())
				Get("stat menu")->SetPosition(Game::SCREEN_WIDTH - Get("stat menu")->GetWidth(), Get("stat menu")->GetPosition().y);
			else if (Get("stat menu")->GetPosition().x < 0)
				Get("stat menu")->SetPosition(0, Get("stat menu")->GetPosition().y);

			if (Get("stat menu")->GetPosition().y > Game::SCREEN_HEIGHT - Get("stat menu")->GetHeight())
				Get("stat menu")->SetPosition(Get("stat menu")->GetPosition().x, Game::SCREEN_HEIGHT - Get("stat menu")->GetHeight());
			else if (Get("stat menu")->GetPosition().y < 0)
				Get("stat menu")->SetPosition(Get("stat menu")->GetPosition().x, 0);
		}

		std::stringstream ss;
		ss << player.GetName() << "\tLevel: " << player.GetStat(Stats::level)
			<< "\n\nStrength:   \t\t\t\t\t\t\t\t\t" << player.GetStat(Stats::str)
			<< "\n\nDexterity:   \t\t\t\t\t\t\t\t\t" << player.GetStat(Stats::dex)
			<< "\n\nInteligence:   \t\t\t\t\t\t\t\t" << player.GetStat(Stats::intel)
			<< "\n\nWisdom:\t\t\t\t\t\t\t\t\t\t" << player.GetStat(Stats::wis)
			<< "\n\nVitality: \t\t\t\t\t\t\t\t\t\t" << player.GetStat(Stats::stam)
			<< "\n\n\n\n\nStat Points:\t\t\t\t\t\t\t\t\t" << player.GetStat(Stats::statPts);
		sf::Text text(ss.str(), Game::font);
		text.setCharacterSize(10);
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
		text.setPosition(this->Get("stat menu")->GetPosition().x + text.getLocalBounds().width / 2 + 15, this->Get("stat menu")->GetPosition().y + 110);

		Get("stat menu")->Draw(renderWindow);
		renderWindow.draw(text);

		if (player.GetStat(Stats::statPts) > 0)
		{
			int pos_x = this->Get("stat menu")->GetPosition().x + 150;
			int pos_y = this->Get("stat menu")->GetPosition().y + 40;
			DrawStatButton(renderWindow, currentEvent, player, pos_x, pos_y, Stats::str);
			DrawStatButton(renderWindow, currentEvent, player, pos_x, pos_y + 25, Stats::dex);
			DrawStatButton(renderWindow, currentEvent, player, pos_x, pos_y + 50, Stats::intel);
			DrawStatButton(renderWindow, currentEvent, player, pos_x, pos_y + 75, Stats::wis);
			DrawStatButton(renderWindow, currentEvent, player, pos_x, pos_y + 100, Stats::stam);
		}
		//renderWindow.draw(testShape);
	}
}

void GuiObjectManager::DrawStatButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player, int posx, int posy, int stat)
{
	this->Get("left green button unactive")->SetPosition(posx, posy);
	this->Get("left green button active")->SetPosition(posx, posy);
	this->Get("left green button pressed")->SetPosition(posx, posy);
	if (this->Get("left green button unactive")->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
	{
		if (currentEvent.type == sf::Event::MouseButtonReleased)
		{
			this->Get("left green button pressed")->Draw(renderWindow);
			player.AddStat(stat, 1);
		}
		else this->Get("left green button active")->Draw(renderWindow);
	}
	else this->Get("left green button unactive")->Draw(renderWindow);
}

void GuiObjectManager::UseItemButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player)
{
	Get("use button")->SetPosition(Get("inventory")->GetPosition().x + 144, Get("inventory")->GetPosition().y + 208);
	Get("use button")->Draw(renderWindow);
	if (Get("use button")->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
	{
		if (currentEvent.type == sf::Event::MouseButtonReleased)
		{
			if (mSelectedItem != NULL)
			{
				player.EquipItem(*mSelectedItem);
			}
		}
	}
}

void GuiObjectManager::DropItemButton(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player)
{
	Get("drop button")->SetPosition(Get("inventory")->GetPosition().x + 144, Get("inventory")->GetPosition().y + 230);
	Get("drop button")->Draw(renderWindow);
	if (Get("drop button")->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
	{
		if (currentEvent.type == sf::Event::MouseButtonReleased)
		{
			if (mSelectedItem != NULL)
			{
				player.DropItem(*mSelectedItem);
			}
		}
	}
}

void GuiObjectManager::DrawMonsterHUD(sf::RenderWindow& renderWindow, Monster& monster, sf::Vector2f HUDpos, bool selected)
{
	UpdateMonsterResources(renderWindow, monster);
	_guiObjects.find("ahud selected")->second->SetPosition(HUDpos);
	_guiObjects.find("aMonsterHUD")->second->SetPosition(HUDpos);
	int hp_pos_x = Get("aMonsterHUD")->GetPosition().x + 76;
	int mp_pos_x = hp_pos_x;
	int xp_pos_x = mp_pos_x;

	int hp_pos_y = Get("aMonsterHUD")->GetPosition().y + 11;
	int mp_pos_y = hp_pos_y + 20;
	int xp_pos_y = mp_pos_y + 20;
	int portrait_pos_y = Get("ahud")->GetPosition().y + 18;

	_guiObjects.find("monster HP bar")->second->SetPosition(hp_pos_x, hp_pos_y);
	//Get("mp bar")->SetPosition(mp_pos_x, mp_pos_y);
	//Get("xp bar")->SetPosition(xp_pos_x, xp_pos_y);

	if (selected)
		_guiObjects.find("ahud selected")->second->Draw(renderWindow);
	else
		_guiObjects.find("aMonsterHUD")->second->Draw(renderWindow);
	_guiObjects.find("monster HP bar")->second->Draw(renderWindow);
	//_guiObjects.find("monster MP bar")->second->Draw(renderWindow);
	//_guiObjects.find("monster XP bar")->second->Draw(renderWindow);

	std::stringstream ss;
	ss << monster.GetStat(Stats::curHP) << "/" << monster.GetStat(Stats::maxHP);
	sf::Text text(ss.str(), Game::font);
	text.setCharacterSize(10);
	text.setColor(sf::Color::Black);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	text.setPosition(_guiObjects.find("monster HP bar")->second->GetPosition().x + _guiObjects.find("monster HP bar")->second->GetWidth() / 2, _guiObjects.find("monster HP bar")->second->GetPosition().y + 2);
	Game::GetWindow().draw(text);
}

bool GuiObjectManager::DrawInventory(sf::RenderWindow& renderWindow, sf::Event currentEvent, Player& player)
{
	if (currentEvent.type == sf::Event::KeyReleased
		&& currentEvent.key.code == sf::Keyboard::I)
	{
		if (_inventory_is_drawn)
			_inventory_is_drawn = false;
		else
			_inventory_is_drawn = true;
	}
	if (_inventory_is_drawn)
	{
		sf::Rect<float> BoundingRect(Get("inventory")->GetPosition(), sf::Vector2f(Get("inventory")->GetWidth() * 1.8f, Get("inventory")->GetHeight() * 0.33f));

		if (BoundingRect.contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
		{
			if (currentEvent.type == sf::Event::MouseButtonPressed)
			{
				if (!_moving_player_HUD)
				_movingInventory = true;
			}
		}
		if (currentEvent.type == sf::Event::MouseButtonReleased)
			_movingInventory = false;

		if (_movingInventory)
		{
			_guiObjects.find("inventory")->second->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(renderWindow)));

			if (Get("inventory")->GetPosition().x > Game::SCREEN_WIDTH - Get("inventory")->GetWidth())
				Get("inventory")->SetPosition(Game::SCREEN_WIDTH - Get("inventory")->GetWidth(), Get("inventory")->GetPosition().y);
			else if (Get("inventory")->GetPosition().x < 0)
				Get("inventory")->SetPosition(0, Get("inventory")->GetPosition().y);

			if (Get("inventory")->GetPosition().y > Game::SCREEN_HEIGHT - Get("inventory")->GetHeight())
				Get("inventory")->SetPosition(Get("inventory")->GetPosition().x, Game::SCREEN_HEIGHT - Get("inventory")->GetHeight());
			else if (Get("inventory")->GetPosition().y < 0)
				Get("inventory")->SetPosition(Get("inventory")->GetPosition().x, 0);

			_guiObjects.find("inventory slots")->second->SetPosition(_guiObjects.find("inventory")->second->GetPosition().x + 12, _guiObjects.find("inventory")->second->GetPosition().y + 80);
		}

		_guiObjects.find("inventory")->second->Draw(renderWindow);
		_guiObjects.find("inventory slots")->second->Draw(renderWindow);
		int item_pos_x = 22;
		int item_pos_y = 22;
		int item_count = 0;
		VisibleGameObject* inventorySlots = _guiObjects.find("inventory slots")->second;
		for (int i = 0; i < player.GetInventory().size(); i++)
		{
			player.GetInventory()[i]->SetPosition(inventorySlots->GetPosition().x + (item_pos_x + (34 * item_count)), inventorySlots->GetPosition().y + item_pos_y);
			player.GetInventory()[i]->SetScale(1.5, 1.5);
			player.GetInventory()[i]->Draw(renderWindow);
			if (item_count == 4)
			{
				item_pos_y += 2;
				item_count = 0;
			}
			item_count++;
		}

		UseItemButton(renderWindow, currentEvent, player);
		DropItemButton(renderWindow, currentEvent, player);
		DrawEquippedItems(renderWindow, player);
		DrawItemStats(renderWindow, currentEvent, player);
	}
	return _inventory_is_drawn;
}

void GuiObjectManager::DrawEquippedItems(sf::RenderWindow& renderWindow, Player& player)
{
	VisibleGameObject* inventorySlots = _guiObjects.find("inventory slots")->second;
	for (int i = 0; i < player.GetEquipment().size(); i++)
	{
		if (player.GetEquipment()[i] != NULL)
		{
			player.GetEquipment()[i]->Draw(renderWindow);
			player.GetEquipment()[i]->SetPosition(inventorySlots->GetPosition().x + 22 + (34 * i), inventorySlots->GetPosition().y + 232);
		}
	}
}

bool GuiObjectManager::isInventoryDrawn()
{
	return _inventory_is_drawn;
}

void GuiObjectManager::DrawItemStats(sf::RenderWindow& renderWindow, sf::Event& currentEvent, Player& player)
{
	sf::Text itemName("", Game::font);
	sf::Text itemStats("", Game::font);
	itemName.setCharacterSize(10);
	itemStats.setCharacterSize(10);
	std::stringstream ss;
	
	bool mouseOverItem = false; 

	// Manage Inventory
	for (int i = 0; i < player.GetInventory().size(); i++)
	{
		if (player.GetInventory()[i]->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
		{	
			mouseOverItem = true;

			// If an item is clicked draw a yellow box around it, indicating it is selected.
			if (currentEvent.type == sf::Event::MouseButtonReleased)
			{
				if (mSelectedItem == player.GetInventory()[i])
				{
					player.EquipItem(*player.GetInventory()[i]);
					mSelectedItem = NULL;
					break;
				}
				else mSelectedItem = player.GetInventory()[i];
			}

			_guiObjects.find("text box border")->second->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(renderWindow)));
			_guiObjects.find("text box border")->second->Draw(renderWindow);
			if (player.GetInventory()[i]->GetRarity() == eRarity::Common)
			{
				itemName.setColor(sf::Color::White);
			}
			else if (player.GetInventory()[i]->GetRarity() == eRarity::Uncommon)
			{
				itemName.setColor(sf::Color::Green);
			}
			itemName.setString(player.GetInventory()[i]->GetName());
			itemName.setPosition(_guiObjects.find("text box border")->second->GetPosition().x + 12, _guiObjects.find("text box border")->second->GetPosition().y + 10);
			renderWindow.draw(itemName);
			for (int j = 0; j < 10; j++)
			{
				if (player.GetInventory()[i]->GetStat(j) > 0)
				{
					if (j == 0)
					{
						ss << "\nDmg: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 1)
					{
						ss << "\nStr: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 2)
					{
						ss << "\nDex: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 3)
					{
						ss << "\nInt: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 4)
					{
						ss << "\nWis: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 5)
					{
						ss << "\nStam: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 6)
					{
						ss << "\nCC: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 7)
					{
						ss << "\nCHD: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 8)
					{
						ss << "\nDef: " << player.GetInventory()[i]->GetStat(j);
					}
					else if (j == 9)
					{
						ss << "\nRes: " << player.GetInventory()[i]->GetStat(j);
					}
				}
			}
			itemStats.setString(ss.str());
			itemStats.setPosition(_guiObjects.find("text box border")->second->GetPosition().x + 12, _guiObjects.find("text box border")->second->GetPosition().y + 20);
			renderWindow.draw(itemStats);
		}
	}

	//Manage Equipment
	for (int k = 0; k < player.GetEquipment().size(); k++)
	{
		if (player.GetEquipment()[k] != NULL)
		{
			if (player.GetEquipment()[k]->GetBoundingRect().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
			{
				mouseOverItem = true;

				// If an item is clicked draw a yellow box around it, indicating it is selected.
				if (currentEvent.type == sf::Event::MouseButtonReleased)
				{
					if (mSelectedItem == player.GetEquipment()[k])
					{
						player.UnequipItem(k);
						mSelectedItem = NULL;
						break;
					}
					else mSelectedItem = player.GetEquipment()[k];
				}
				_guiObjects.find("text box border")->second->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(renderWindow)));
				_guiObjects.find("text box border")->second->Draw(renderWindow);
				if (player.GetEquipment()[k]->GetRarity() == eRarity::Common)
				{
					itemName.setColor(sf::Color::White);
				}
				else if (player.GetEquipment()[k]->GetRarity() == eRarity::Uncommon)
				{
					itemName.setColor(sf::Color::Green);
				}
				itemName.setString(player.GetEquipment()[k]->GetName());
				itemName.setPosition(_guiObjects.find("text box border")->second->GetPosition().x + 12, _guiObjects.find("text box border")->second->GetPosition().y + 10);
				renderWindow.draw(itemName);
				for (int j = 0; j < 10; j++)
				{
					if (player.GetEquipment()[k]->GetStat(j) > 0)
					{
						if (j == 0)
						{
							ss << "\nDmg: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 1)
						{
							ss << "\nStr: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 2)
						{
							ss << "\nDex: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 3)
						{
							ss << "\nInt: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 4)
						{
							ss << "\nWis: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 5)
						{
							ss << "\nStam: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 6)
						{
							ss << "\nCC: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 7)
						{
							ss << "\nCHD: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 8)
						{
							ss << "\nDef: " << player.GetEquipment()[k]->GetStat(j);
						}
						else if (j == 9)
						{
							ss << "\nRes: " << player.GetEquipment()[k]->GetStat(j);
						}
					}
				}
				itemStats.setString(ss.str());
				itemStats.setPosition(_guiObjects.find("text box border")->second->GetPosition().x + 12, _guiObjects.find("text box border")->second->GetPosition().y + 20);
				renderWindow.draw(itemStats);
			}
		}
	}
	if (!mouseOverItem)
	{
		if (currentEvent.type == sf::Event::MouseButtonReleased)
			mSelectedItem = NULL;
	}
	if (mSelectedItem != NULL)
	{
		Get("item select box")->SetPosition(mSelectedItem->GetPosition());
		Get("item select box")->Draw(renderWindow);
	}
}
