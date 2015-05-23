#ifndef _BATTLE_H
#define _BATTLE_H

#include "Player.h"
#include "Monster.h"

/*struct Entity
{
	VisibleGameObject* entity;
	bool friendly;
};*/

class Player;
class Monster;

class Battle
{
public:
	Battle(std::vector<Player*>& playerParty, std::vector<Monster*>& monsterParty, bool ambush);
	~Battle();

	sf::Vector2f GetPlayerPos();
	sf::Vector2f GetMonsterPos();
	sf::Text& GetDamageText();

	Entity& GetTarget();
	Entity& GetActiveEntity();
	std::vector<Entity>& GetTurnOrder();
	void SetTarget(Entity* target, bool friendly);
	void SetActiveEntity(Entity* target, bool friendly);
	void ClearTarget();
	void ClearActiveEntity();

	std::vector<Monster*>& GetMonsterParty();
	
	int GetTurn();
	void SetTurn(int num);
	bool GetPlayerAtt();
	void SetPlayerAtt(bool yesno);

	sf::Clock& GetSpellFrameClock();
	sf::Clock& GetTurnTimer();
	sf::Clock& GetTextFadeClock();

private:
	std::vector<Player*> m_playerParty;
	std::vector<Monster*> m_monsterParty;
	std::vector<Entity> m_turnOrder;
	std::vector<Entity> m_entityList;
	
	void calculateTurnOrder();

	Entity m_target;
	Entity m_activeEntity;

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
std::string BasicPhysAttack(Entity& player, Entity& monster);
std::string PlayerAttackResolve(Entity& player, Entity& monster, int damage);
std::string MonsterAttackResolve(Player& player, Monster& mon, int damage);
std::string MonsterAttack(Player& player, Monster & mon);

#endif