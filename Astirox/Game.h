#ifndef _GAME_H
#define _GAME_H

#include "GameObjectManager.h"
#include "Equipment.hpp"
#include "InputHandler.hpp"
#include "Monster.h"
#include "Player.h"
#include "GuiObjectManager.h"
#include "AnimationManager.hpp"
#include "Battle.h"
#include "Map.hpp"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}
#include <LuaBridge.h>

class GuiObjectManager;
class Battle;
class InputHandler;


class Game
{
public:
	static void Start();
	const static sf::Event& Game::GetInput();
	static sf::RenderWindow& GetWindow();
	static GameObjectManager& GetGameObjectManager();
	static GuiObjectManager& GetGuiObjectManager();
	static AnimationManager& GetAnimationManager();
	static InputHandler& GetInputHandler();

	static void startBattle(Battle* fight);
	static Battle* battle;

	const static int SCREEN_WIDTH = 1024;
	const static int SCREEN_HEIGHT = 768;
	static sf::View view;
	const static int MONSTER_LIMIT = 5;

	static std::string DIFFICULTY_SETTING; 

	static lua_State* lua_state;
	static Map* currentMap;
	static std::vector<Map*> recentlyVisitedMaps;

	static boost::mt19937 rng;

	//static std::vector<Player*> playerParty;
	static Player* player;

	static sf::Font font;
	static void wordWrap(std::string text);

	static std::stack<path_element*> player_path;

private:
	static bool IsExiting();
	static void GameLoop();

	static void ShowSplashScreen();
	static void ShowMenu();

	enum GameState{
		Uninitialized, ShowingSplash, Paused,
		ShowingMenu, Playing, Fight, Exiting
	};

	static sf::Clock frameTime;

	static GameState _gameState;
	static sf::RenderWindow _mainWindow;
	static sf::Event _currentEvent;

	static Command* command;

	static GameObjectManager _gameObjectManager;
	static GuiObjectManager _guiObjectManager;
	static AnimationManager _animationManager;
	static InputHandler _inputHandler;
};

#endif