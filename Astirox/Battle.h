#ifndef _BATTLE_H
#define _BATTLE_H

#include "Player.h"
#include "Monster.h"

class Battle
{
public:
	Battle(Player& player1, Monster& monster1);
	~Battle();

	Player* GetPlayer();
	Monster* GetMonster();

	sf::Vector2f GetPlayerPos();
	sf::Vector2f GetMonsterPos();
	sf::Text& GetDamageText();
	
	int GetTurn();
	void SetTurn(int num);
	bool GetPlayerAtt();
	void SetPlayerAtt(bool yesno);

	sf::Clock& GetSpellFrameClock();
	sf::Clock& GetTurnTimer();
	sf::Clock& GetTextFadeClock();

private:
	Player* m_player;
	Monster* m_monster;
	
	int turn;
	bool playerAtt;

	sf::Vector2f playerPos;
	sf::Vector2f monsterPos;

	sf::Text damageText;
	
	sf::Clock spellFrameClock;
	sf::Clock turnTimer;
	sf::Clock textFadeClock;
};

bool fight(sf::RenderWindow& renderWindow, Player& player, Monster& monster);
std::string BasicPhysAttack(Player& player, Monster& monster);
std::string PlayerAttackResolve(Player& player, Monster& monster, int damage);
std::string MonsterAttackResolve(Player& player, Monster & mon, int damage);
std::string MonsterAttack(Player& player, Monster & mon);

#endif