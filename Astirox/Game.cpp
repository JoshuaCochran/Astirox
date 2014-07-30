#include "stdafx.h"
#include "Game.h"
#include "SplashScreen.h"
#include "MainMenu.h"
#include "ServiceLocator.h"
#include "SFMLSoundProvider.h"
#include "Player.h"
#include "GuiObjectManager.h"
#include "Monster.h"
#include "Battle.h"

void Game::Start(void)
{
	if (_gameState != Uninitialized)
		return;

	lua_state = luaL_newstate();

	srand(time(NULL));
	rng.seed(static_cast<unsigned int>(std::time(0)));

	_mainWindow.create(sf::VideoMode(Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT, 32), "Astirox");
	_mainWindow.setFramerateLimit(60);

	_mainWindow.setKeyRepeatEnabled(false);

	_mainWindow.setMouseCursorVisible(false);
	VisibleGameObject *cursor = new VisibleGameObject("data/images/gui/cursor.png");

	battle = NULL;
	DIFFICULTY_SETTING = "NORMAL";

	font.loadFromFile("data/fonts/arial.ttf");

	SFMLSoundProvider soundProvider;
	
	currentMap = new Map("mine.lua");
	/*tmx::MapLoader m1("data/maps");
	m1.Load("mine.tmx");*/

	//tgui::Gui gui(_mainWindow);

	ServiceLocator::RegisterServiceLocator(&soundProvider);
	//ServiceLocator::RegisterServiceLocator(&m1);
	//ServiceLocator::RegisterServiceLocator(&gui);

	ServiceLocator::GetAudio()->PlaySong("audio/Soundtrack.ogg", true);

	Player *player = new Player();
	player->SetPosition(24, 24);

	Monster *monster1 = new Monster("test_monster.lua");//"test_monster.lua");
	monster1->SetPosition(136, 88);
	
	/*Monster *monster2 = new Monster();
	monster2->SetPosition(136, 168);

	Monster *monster3 = new Monster();
	monster3->SetPosition(104, 224);

	Monster *monster4 = new Monster();
	monster4->SetPosition(262, 88);

	Monster *monster5 = new Monster();
	monster5->SetPosition(280, 200);*/

	_guiObjectManager.Init();


	_gameObjectManager.Add("Player", player);
	/*_gameObjectManager.Add("Monster1", monster1);
	_gameObjectManager.Add("Monster2", monster2);
	_gameObjectManager.Add("Monster3", monster3);
	_gameObjectManager.Add("Monster4", monster4);
	_gameObjectManager.Add("Monster5", monster5);*/
	_gameObjectManager.Add("cursor", cursor);

	_animationManager.Init();

	/*_gameObjectManager.Add("ahud", playerHud);
	_gameObjectManager.Add("portrait", characterPortrait);
	_gameObjectManager.Add("hp bar", hpBar);
	_gameObjectManager.Add("mp bar", mpBar);
	_gameObjectManager.Add("exp bar", expBar);
	_gameObjectManager.Add("inventory", playerInventory);
	_gameObjectManager.Add("text box", textBox);*/

	_gameState = Game::ShowingSplash;

	while (!IsExiting())
	{
		GameLoop();
	}

	_mainWindow.close();
}

bool Game::IsExiting()
{
	if (_gameState == Game::Exiting)
		return true;
	else
		return false;
}

const sf::Event& Game::GetInput()
{
	sf::Event currentEvent;
	_mainWindow.pollEvent(currentEvent);
	return currentEvent;
}

sf::RenderWindow& Game::GetWindow()
{
	return _mainWindow;
}

GameObjectManager& Game::GetGameObjectManager()
{
	return _gameObjectManager;
}

GuiObjectManager& Game::GetGuiObjectManager()
{
	return _guiObjectManager;
}

AnimationManager& Game::GetAnimationManager()
{
	return _animationManager;
}

void Game::GameLoop()
{
	sf::Event currentEvent;
	_mainWindow.pollEvent(currentEvent);
	Player* player1 = dynamic_cast<Player*>(Game::GetGameObjectManager().Get("Player"));
	_gameObjectManager.Get("cursor")->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_mainWindow)));

	switch (_gameState)
	{
		case Game::ShowingMenu:
			ShowMenu();
			_gameObjectManager.Get("cursor")->Draw(_mainWindow);
			break;

		case Game::ShowingSplash:
			ShowSplashScreen();
			_gameObjectManager.Get("cursor")->Draw(_mainWindow);
			break;

		case Game::Playing:
			_mainWindow.clear(sf::Color(sf::Color(0, 0, 0)));
			currentMap->DrawAll(_mainWindow);
			//_mainWindow.draw(*ServiceLocator::GetMapLoader());
			if (currentEvent.type == sf::Event::KeyPressed && currentEvent.key.code == sf::Keyboard::Z)
				player1->PickUpItem();

			if (currentEvent.type == sf::Event::KeyPressed && currentEvent.key.code == sf::Keyboard::F5)
				player1->ReloadScripts();

			for (int i = 0; i < equipmentOnFloor.size(); i++)
			{
				equipmentOnFloor[i]->Draw(_mainWindow);
			}
			if (player1->moveto(*currentMap, currentEvent))
			{
				currentMap->UpdateMonsters();
				currentMap->CheckSpawn();
				//_gameObjectManager.UpdateAll();
				if (_gameState == Fight) break;
				// If monster has died during previous update, remove it from the game.
			}
			_guiObjectManager.DrawOOCGUI(_mainWindow, currentEvent, *player1);
			_gameObjectManager.DrawAll(_mainWindow);

			//wordWrap("This is a test of the word wrap function. So, here is a very long string that will hopefully demonstrate that the function is, in fact, working as intended so that I may go about working on other problems.");
			_mainWindow.display();

			if (currentEvent.type == sf::Event::Closed) _gameState = Game::Exiting;
			break;

		case Game::Fight:
			// Player turn is handled within the combat GUI implementation
			_guiObjectManager.DrawCombatGUI(_mainWindow, currentEvent, *battle);
			
			player1->Draw(_mainWindow);
			battle->GetMonster()->Draw(_mainWindow);
			_gameObjectManager.Get("cursor")->Draw(_mainWindow);

			// Enemy turn
			if (battle->GetTurn() == 1 && battle->GetTurnTimer().getElapsedTime().asSeconds() > 1)
			{
				battle->GetDamageText().setString(MonsterAttack(*player1, *battle->GetMonster()));
				battle->GetDamageText().setPosition(player1->GetPosition().x, player1->GetPosition().y - 32);
				battle->GetTextFadeClock().restart();
				battle->GetSpellFrameClock().restart();
				battle->SetTurn(0);
				battle->SetPlayerAtt(false);
				_animationManager.GetSpellSprite()->stop();
			}

			//Fade Damage Text
			if (battle->GetTextFadeClock().getElapsedTime().asSeconds() < 1)
			{
				battle->GetDamageText().setColor(sf::Color(255, 255, 255, 255 - 255 * battle->GetTextFadeClock().getElapsedTime().asSeconds()));
				battle->GetDamageText().setPosition(battle->GetDamageText().getPosition().x, battle->GetDamageText().getPosition().y - 0.25f);
			}

			// For one second after player attack play attack animation
			// and cause monster to blink indicating a hit
			if (battle->GetSpellFrameClock().getElapsedTime().asSeconds() < 1 && battle->GetPlayerAtt())
			{
				_animationManager.GetSpellSprite()->play(*_animationManager.GetSpellAnimation());
				_animationManager.GetSpellSprite()->move(sf::Vector2f(-3.0f, 0.0f));
				_animationManager.GetSpellSprite()->update(battle->GetSpellFrameClock().getElapsedTime());
				_mainWindow.draw(*_animationManager.GetSpellSprite());
				battle->GetMonster()->SetTransparency(255 * (battle->GetSpellFrameClock().getElapsedTime().asMilliseconds() % 2 ));
			}
			else battle->GetMonster()->SetTransparency(255);

			// For one second after monster attack cause player to flash indicating hit.
			if (battle->GetSpellFrameClock().getElapsedTime().asSeconds() < 1 && !battle->GetPlayerAtt())
			{
				battle->GetPlayer()->SetTransparency(255 * (battle->GetSpellFrameClock().getElapsedTime().asMilliseconds() % 2));
			}
			else battle->GetPlayer()->SetTransparency(255);

			_mainWindow.draw(battle->GetDamageText());
			_mainWindow.display();

			if (battle->GetPlayer()->GetCurrentHP() <= 0 || battle->GetMonster()->GetCurrentHP() <= 0)
			{
				if (battle->GetTurn() != 2) battle->GetTurnTimer().restart();
				battle->SetTurn(2);
				if (battle->GetTurn() == 2 && battle->GetTurnTimer().getElapsedTime().asSeconds() > 1)
				{
					if (battle->GetPlayer()->GetCurrentHP() <= 0)
					{
						battle->GetPlayer()->AddHP(battle->GetPlayer()->GetMaxHP());
						battle->GetMonster()->AddHP(battle->GetMonster()->GetMaxHP() - battle->GetMonster()->GetCurrentHP());
						battle->GetMonster()->SetPosition(battle->GetMonsterPos());
						battle->GetMonster()->SetScale(1, 1);
					}
					if (battle->GetMonster()->GetCurrentHP() <= 0)
					{
						battle->GetPlayer()->GainXP(battle->GetMonster()->GetXPReward());
						battle->GetMonster()->SetPosition(battle->GetMonsterPos());
						Equipment* itemDrop = new Equipment(*battle->GetMonster());
						Game::equipmentOnFloor.push_back(itemDrop);
						std::stringstream ss;
						for (int i = 1; i < currentMap->GetSpawnedMonsters().size(); i++)
						{
							if (currentMap->GetSpawnedMonsters()[i] != NULL && currentMap->GetSpawnedMonsters()[i]->GetCurrentHP() <= 0)
							{
								currentMap->GetSpawnedMonsters()[i] = currentMap->GetSpawnedMonsters().back();
								currentMap->GetSpawnedMonsters().pop_back();
								break;
							}
						}
					}
					player1->SetPosition(battle->GetPlayerPos());
					player1->SetScale(1, 1);
					_gameState = Game::Playing;
				}
			}
			break;
	}
}

void Game::ShowSplashScreen()
{
	SplashScreen splashScreen;
	splashScreen.Show(_mainWindow);
	_gameState = Game::ShowingMenu;
}

void Game::ShowMenu()
{
	MainMenu mainMenu;
	MainMenu::MenuResult result = mainMenu.Show(_mainWindow);
	switch (result)
	{
	case MainMenu::Exit:
		_gameState = Game::Exiting;
		break;
	case MainMenu::Play:
		_gameState = Game::Playing;
		break;
	}
}

void Game::startBattle(Player& player1, Monster& monster1)
{
	_gameState = Game::Fight;
	battle = new Battle(player1, monster1);
}

void Game::wordWrap(std::string text)
{
	sf::Font font;
	font.loadFromFile("data/fonts/arial.ttf");
	sf::Text words(text, font, 12);
	
	int pos = 0; 
	std::string line;
	//float CONSOLE_WIDTH = Game::GetGameObjectManager().Get("text box")->GetBoundingRect().width;	
	int CONSOLE_WIDTH = 130;
	while (text.length() > CONSOLE_WIDTH)//words.getLocalBounds().width > CONSOLE_WIDTH)
	{
		pos = text.rfind(" ", CONSOLE_WIDTH, 1);
		text.replace(pos, 1, "\n");
		line += text.substr(0, pos);
		text.erase(0, pos);
	}
	line += text;

	words.setString(line);
	words.setPosition(25, 617);
	//words.setStyle(sf::Text::Bold);
	Game::GetWindow().draw(words);
}

Game::GameState Game::_gameState = Uninitialized;
sf::RenderWindow Game::_mainWindow;
GameObjectManager Game::_gameObjectManager;
GuiObjectManager Game::_guiObjectManager;
AnimationManager Game::_animationManager;
sf::Font Game::font;
boost::mt19937 Game::rng;
std::vector<Equipment*> Game::equipmentOnFloor;
Battle* Game::battle;
std::string Game::DIFFICULTY_SETTING;
lua_State* Game::lua_state;
Map* Game::currentMap;