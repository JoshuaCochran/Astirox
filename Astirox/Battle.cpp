#include "stdafx.h"
#include "Battle.h"
#include "Game.h"
#include "Monster.h"
#include "Equipment.hpp"

Battle::Battle(std::vector<Player*>& playerParty, std::vector<Monster*>& monsterParty, bool ambush)
{
	monsterPos = monsterParty[0]->GetPosition();
	m_monsterParty = monsterParty;
	for (int i = 0; i < monsterParty.size(); i++)
	{
		monsterParty[i]->SetPosition(Game::SCREEN_WIDTH / 2 - 200 + (48 * i),
			Game::SCREEN_HEIGHT / 2 + 96 - (48 * i));
		monsterParty[i]->SetScale(3, 3);
	}
	m_playerParty = playerParty;
/*	playerPos = Game::playerParty[0]->GetPosition();
	for (int i = 0; i < Game::playerParty.size(); i++)
	{
		m_playerParty[i]->SetPosition(Game::SCREEN_WIDTH / 2 + 100 + (48 * i), 
			Game::SCREEN_HEIGHT / 2 + 96 - (48 * i));
		m_playerParty[i]->SetScale(3, 3);
	}*/

	damageText.setString("");
	damageText.setFont(Game::font);
	damageText.setCharacterSize(22);

	Game::startBattle(this);
}

void Battle::calculateTurnOrder()
{
}

Entity& Battle::GetTarget()
{
	return m_target;
}
Entity& Battle::GetActiveEntity()
{
	return m_activeEntity;
}

std::vector<Monster*>& Battle::GetMonsterParty()
{
	return m_monsterParty;
}

void Battle::SetTarget(Entity* target, bool friendly)
{
}

void Battle::ClearTarget()
{
}

void Battle::ClearActiveEntity()
{
}

void Battle::SetActiveEntity(Entity* target, bool friendly)
{
}

sf::Vector2f Battle::GetPlayerPos()
{
	return playerPos;
}

sf::Vector2f Battle::GetMonsterPos()
{
	return monsterPos;
}

sf::Text& Battle::GetDamageText()
{
	return damageText;
}

int Battle::GetTurn()
{
	return turn;
}

bool Battle::GetPlayerAtt()
{
	return playerAtt;
}

sf::Clock& Battle::GetSpellFrameClock()
{
	return spellFrameClock;
}

sf::Clock& Battle::GetTurnTimer()
{
	return turnTimer;
}

sf::Clock& Battle::GetTextFadeClock()
{
	return textFadeClock;
}

void Battle::SetTurn(int num)
{
	turn = num;
}

void Battle::SetPlayerAtt(bool input)
{
	playerAtt = input;
}

std::string BasicPhysAttack(Entity& actor, Entity& target)
{
	int minDmg = (int)(actor.GetStat(Stats::pATT) * .6);
	int damage = (rand() % (actor.GetStat(Stats::pATT) - minDmg)) + minDmg;

	/*
	If player has burning enchantment roll to see if monster is inflicted with burning.
	33% chance to inflict burning
	Burning does 3% of the monster's total health per turn
	*/
	/*if (player.getEnchant() == 1) {
		if ((rand() % 100) < 33 * x.getRandMod()) monster.SetBurn(true);
	}*/

	return PlayerAttackResolve(actor, target, damage);
}

std::string PlayerAttackResolve(Entity& player, Entity& monster, int damage)
{
	std::stringstream ss;
	int burnDamage = 0;
	/*if (mon.IsBurning())
	{
		burnDamage = monster.GetMaxHP() * 0.03;
		damage *= player.getBurnedMonMod();
	}*/

	int damage_mitigation = (monster.GetStat(Stats::defense) / (monster.GetStat(Stats::defense) + player.GetStat(Stats::level) * 50));
	//Hard cap of damage mitigation is 80%.
	if (damage_mitigation > .8) damage_mitigation = .8;
	damage *= 1 - damage_mitigation;
	monster.AddHP((damage + burnDamage) * -1);

	if (monster.GetStat(Stats::curHP) > 0)
	{
		ss << damage;
		return ss.str();
	}
	else
	{
		/*cout << "You have slain " << mon.getName() << "!\n"
			<< "You gain " << mon.getExpReward() << " experience points, " << mon.getExpReward() * 2 * jp_modifier << " jp, and "
			<< mon.getGoldReward() * x.getDropMod() << " gold.\n\n";
		x.gainExp(mon.getExpReward());
		x.getJob(x.getActiveJob()).gainJp(mon.getExpReward() * 2 * jp_modifier);
		x.gainGold(mon.getGoldReward() * x.getDropMod());
		cout << "You now have " << x.getGold() << " gold, " << x.getJob(x.getActiveJob()).get_current_jp() << "/" <<
			x.getJob(x.getActiveJob()).get_required_jp() << "jp, and "
			<< x.getCurrentExp() << "/" << x.getExpNeeded() << " experience points.\n";
		x.checkLevel();
		x.getJob(x.getActiveJob()).checkLevel(x);
		jp_modifier = 1;*/
		//checkDrop(mon, invent);
		return "";
	}
}

std::string MonsterAttack(Player& player, Monster & monster)
{
	int damage = 0;//(rand() % (monster.GetMaxDmg() - monster.GetMinDmg())) + monster.GetMinDmg();

	return MonsterAttackResolve(player, monster, damage);
}

std::string MonsterAttackResolve(Player& player, Monster & monster, int damage)
{
	int damage_mitigation = 0;//(player.GetDefense() / (player.GetDefense() + (monster.GetLevel() * 50)));
	//Hard cap of damage mitigation is 80%.
	if (damage_mitigation > .8) damage_mitigation = .8;
	damage *= 1 - damage_mitigation;


	player.AddHP(damage * -1);

	std::stringstream ss;
	ss << damage;
	return ss.str();
	
	/*if (x.getCurrentHp() > 0)
	{
		cout << monster.getName() << " attacks you for " << damage << ".\n"
			<< "You have " << x.getCurrentHp() << " hp remaining.\n" << endl;
	}
	else
	{
		cout << "You take " << damage << " points of damage and die to " << mon.getName() << "!\n";
		cout << "You lose " << (x.getCurrentExp() * .20) << " experience points.\n"
			<< "Sorry! Better luck next time.";
		x.gainExp((int)(-1 * (x.getCurrentExp() * .20)));
	}*/
}