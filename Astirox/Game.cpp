

#include "stdafx.h"
#include "Game.h"
#include "SplashScreen.h"
#include "MainMenu.h"
#include "ServiceLocator.h"
#include "SFMLSoundProvider.h"

void Game::Start(void)
{
	if (_gameState != Uninitialized)
		return;

	lua_state = luaL_newstate();

	srand(time(NULL));
	rng.seed(static_cast<unsigned int>(std::time(0)));

	_mainWindow.create(sf::VideoMode(Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT, 32), "Astirox");
	_mainWindow.setFramerateLimit(60);
	//_mainWindow.setVerticalSyncEnabled(true);
	view.reset(sf::FloatRect(24, 24, 256, 192));
	view.setViewport(sf::FloatRect(0.f, 0.f, 0.75f, 1.f));

	frameTime.restart();

	_mainWindow.setKeyRepeatEnabled(false);

	_mainWindow.setMouseCursorVisible(false);
	VisibleGameObject *cursor = new VisibleGameObject("data/images/gui/cursor.png");

	battle = NULL;
	DIFFICULTY_SETTING = "NORMAL";

	if (!font.loadFromFile("data/fonts/arial.ttf"))
	{
		std::cout << "ERROR: Cannot load font" << std::endl;
	}
	else
		std::cout << "Successfully loaded font" << std::endl;

	SFMLSoundProvider soundProvider;
	
	currentMap = new Map("mine.tmx");


	ServiceLocator::RegisterServiceLocator(&soundProvider);


	//ServiceLocator::GetAudio()->PlaySong("audio/Soundtrack.ogg", true);

	player = new Player();
	player->SetPosition(24, 24);
	currentMap->do_fov(player->GetPosition().x, player->GetPosition().y, 96);

	_inputHandler.init();
	_guiObjectManager.Init();

	//_gameObjectManager.Add("Player", player);
	_gameObjectManager.Add("cursor", cursor);

	_animationManager.Init();


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
	Command* command = _inputHandler.handleInput(currentEvent);
	_gameObjectManager.Get("cursor")->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_mainWindow)));
	
	frameTime.restart();
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
			if (command)
			{
				command->execute(*player);
				currentMap->UpdateMonsters();
				currentMap->CheckSpawn();
			}
			command = NULL;

			Game::view.setCenter(player->GetPosition().x, player->GetPosition().y);
			_mainWindow.setView(view);
			
			//_mainWindow.draw(currentMap->GetMapLoader());
			
			currentMap->DrawAll(_mainWindow);
			player->Draw(_mainWindow);

			_mainWindow.setView(_mainWindow.getDefaultView());
			_guiObjectManager.DrawOOCGUI(_mainWindow, currentEvent, *player);
			_gameObjectManager.Get("cursor")->Draw(_mainWindow);//*/


			///wordWrap("This is a test of the word wrap function. So, here is a very long string that will hopefully demonstrate that the function is, in fact, working as intended so that I may go about working on other problems.");
			_mainWindow.display();

			if (currentEvent.type == sf::Event::Closed) _gameState = Game::Exiting;//*/
			break;

		case Game::Fight:
			_mainWindow.setView(_mainWindow.getDefaultView());
			// Player turn is handled within the combat GUI implementation
			_guiObjectManager.DrawCombatGUI(_mainWindow, currentEvent, *battle);
			
			//for (int i = 0; i < playerParty.size(); i++)
				player->Draw(_mainWindow);
			for (int i = 0; i < battle->GetMonsterParty().size(); i++)
				battle->GetMonsterParty()[i]->Draw(_mainWindow);
			_gameObjectManager.Get("cursor")->Draw(_mainWindow);


			// Enemy turn
			/*if (!battle->GetActiveEntity().friendly && battle->GetTurnTimer().getElapsedTime().asSeconds() > 1 && dynamic_cast<Monster*>(battle->GetActiveEntity().entity)->GetCurrentHP() > 0)
			{
				battle->GetDamageText().setString(MonsterAttack(*playerParty[0], *dynamic_cast<Monster*>(battle->GetActiveEntity().entity)));
				battle->GetDamageText().setPosition(playerParty[0]->GetPosition().x, playerParty[0]->GetPosition().y - 32);
				battle->GetTextFadeClock().restart();
				battle->GetSpellFrameClock().restart();
				battle->SetActiveEntity(*Game::playerParty[0], true);
				_animationManager.GetSpellSprite()->stop();
			}*/

			/*if (battle->GetTarget().entity != NULL)
			{
				// For one second after monster attack cause player to flash indicating hit.
				if (battle->GetSpellFrameClock().getElapsedTime().asSeconds() < 1 && battle->GetDamageText().getString() != "MISS" && battle->GetDamageText().getString() != "")
				{
					battle->GetTarget().entity->SetTransparency(255 * (battle->GetSpellFrameClock().getElapsedTime().asMilliseconds() % 2));
				}
				else battle->GetTarget().entity->SetTransparency(255);
			}*/


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
			}

			_mainWindow.draw(battle->GetDamageText());
			_mainWindow.display();


			int playerDeadCount = 0;
			/*for (int i = 0; i < Game::playerParty.size(); i++)
			{
				if (Game::playerParty[i]->GetStat(Stats::curHP) <= 0)
					playerDeadCount++;
			}*/
			int monsterDeadCount = 0;
			for (int i = 0; i < battle->GetMonsterParty().size(); i++)
			{
				if (battle->GetMonsterParty()[i]->GetStat(Stats::curHP) <= 0)
				{
					monsterDeadCount++;
				}
			}

			/*if (playerDeadCount == Game::playerParty.size() || battle->GetMonsterParty().size() == monsterDeadCount)
			{

				/*if (battle->GetActiveEntity().entity == NULL && battle->GetTurnTimer().getElapsedTime().asSeconds() > 1)
				{
					if (playerDeadCount == Game::playerParty.size())
					{
						for (int i = 0; i < Game::playerParty.size(); i++)
						{
							Game::playerParty[i]->AddHP(Game::playerParty[i]->GetMaxHP());
						}
						for (int i = 0; i < battle->GetMonsterParty().size(); i++)
						{
							battle->GetMonsterParty()[i]->AddHP(battle->GetMonsterParty()[i]->GetMaxHP() - battle->GetMonsterParty()[i]->GetCurrentHP());
							battle->GetMonsterParty()[i]->SetPosition(battle->GetMonsterPos());
							battle->GetMonsterParty()[i]->SetScale(1, 1);
						}
					}
					int loc = 99;
					if (monsterDeadCount == battle->GetMonsterParty().size())
					{
						for (int i = 0; i < battle->GetMonsterParty().size(); i++)
						{
							if (battle->GetMonsterParty()[i]->GetCurrentHP() <= 0)
							{
								for (int j = 0; j < Game::playerParty.size(); j++)
									Game::playerParty[j]->GainXP(battle->GetMonsterParty()[i]->GetXPReward());

								Equipment* itemDrop = new Equipment(*battle->GetMonsterParty()[i]);
								currentMap->GetEquipmentOnFloor().push_back(itemDrop);
							}
						}
						for (int i = 0; i < battle->GetMonsterParty().size(); i++)
						{
							for (int j = 0; j < currentMap->GetSpawnedMonsters().size(); j++)
							{
								for (int k = 0; k < currentMap->GetSpawnedMonsters()[j].size(); k++)
								{
									if (currentMap->GetSpawnedMonsters()[j][k] == battle->GetMonsterParty()[i])
									{
										loc = j;
										delete currentMap->GetSpawnedMonsters()[j][k];
									}
								}
							}
						}
						currentMap->GetSpawnedMonsters()[loc] == currentMap->GetSpawnedMonsters().back();
						currentMap->GetSpawnedMonsters().pop_back();
					}
					playerParty[0]->SetPosition(battle->GetPlayerPos());
					playerParty[0]->SetScale(1, 1);
					currentMap->do_fov(playerParty[0]->GetPosition().x, playerParty[0]->GetPosition().y, 96);
					battle = NULL;
					_gameState = Game::Playing;
				}
			}
			break;*/
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

void Game::startBattle(Battle* fight)
{
	battle = fight;
	_gameState = Game::Fight;
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
InputHandler Game::_inputHandler;
sf::Font Game::font;
boost::mt19937 Game::rng;
//std::vector<Player*> Game::playerParty;
Player* Game::player;
Battle* Game::battle;
std::string Game::DIFFICULTY_SETTING;
lua_State* Game::lua_state;
Map* Game::currentMap;
std::vector<Map*> Game::recentlyVisitedMaps;
sf::View Game::view;
sf::Clock Game::frameTime;