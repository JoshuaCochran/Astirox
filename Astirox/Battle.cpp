#include "stdafx.h"
#include "Battle.h"
#include "Game.h"
#include "Monster.h"
#include "Equipment.hpp"

Battle::Battle(Player& player1, Monster& monster1)
{
	m_player = &player1;
	m_monster = &monster1;

	playerPos = m_player->GetPosition();
	monsterPos = m_monster->GetPosition();

	m_player->SetPosition(Game::SCREEN_WIDTH / 2 + 100, Game::SCREEN_HEIGHT / 2);
	m_player->SetScale(3, 3);

	m_monster->SetPosition(Game::SCREEN_WIDTH / 2 - 200, Game::SCREEN_HEIGHT / 2);
	m_monster->SetScale(3, 3);
	
	damageText.setString("");
	damageText.setFont(Game::font);
	damageText.setCharacterSize(22);

	turn = 0;
	playerAtt = false;
}

Player* Battle::GetPlayer()
{
	return m_player;
}

Monster* Battle::GetMonster()
{
	return m_monster;
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

/*bool fight(sf::RenderWindow& renderWindow, Player& player, Monster& monster /*,Inventory& invent)
{
	/*sf::Texture icon;
	icon.loadFromFile("data/rogue.png");
	sf::Sprite iconTest(icon);

	sf::Texture borderImage;
	borderImage.loadFromFile("data/border.png");
	sf::Sprite border1(borderImage);
	sf::Sprite border2(borderImage);

	iconTest.setScale(0.25, 0.25);
	iconTest.setPosition(2, Game::SCREEN_HEIGHT - 130);
	border1.setPosition(0, Game::SCREEN_HEIGHT - 136);
	border2.setPosition(136, Game::SCREEN_HEIGHT - 136);

	float playerPosX = player.GetPosition().x;
	float playerPosY = player.GetPosition().y;

	float monsterPosX = monster.GetPosition().x;
	float monsterPosY = monster.GetPosition().y;

	///Monster monster1("data/images/monster/archon0.png");
	player.SetPosition(Game::SCREEN_WIDTH / 2 + 100, Game::SCREEN_HEIGHT / 2);
	//monster1.SetPosition(Game::SCREEN_WIDTH / 2 - 100, Game::SCREEN_HEIGHT / 2);
	player.SetScale(3, 3);

	/*sf::Texture image;
	image.loadFromFile("data/images/map backgrounds/gold_shifter_dunes.png");
	sf::Sprite background(image);
	background.setScale(1.5f, 1.5f);*/

	/*sf::Texture spellAnimation;
	if (!spellAnimation.loadFromFile("data/firelion_left.png"))
	{
		std::cout << "Failed to load spell spritesheet!" << std::endl;
	}*/

	/*Animation fireAnimation;
	fireAnimation.setSpriteSheet(spellAnimation);
	fireAnimation.addFrame(sf::IntRect(0, 0, 128, 128));
	fireAnimation.addFrame(sf::IntRect(128, 0, 128, 128));
	fireAnimation.addFrame(sf::IntRect(256, 0, 128, 128));
	fireAnimation.addFrame(sf::IntRect(384, 0, 128, 128));
	fireAnimation.addFrame(sf::IntRect(0, 128, 128, 128));
	fireAnimation.addFrame(sf::IntRect(128, 128, 128, 128));
	fireAnimation.addFrame(sf::IntRect(256, 128, 128, 128));
	fireAnimation.addFrame(sf::IntRect(384, 128, 128, 128));
	fireAnimation.addFrame(sf::IntRect(0, 256, 128, 128));
	fireAnimation.addFrame(sf::IntRect(128, 256, 128, 128));
	fireAnimation.addFrame(sf::IntRect(256, 256, 128, 128));
	fireAnimation.addFrame(sf::IntRect(384, 256, 128, 128));
	fireAnimation.addFrame(sf::IntRect(0, 384, 128, 128));
	fireAnimation.addFrame(sf::IntRect(128, 384, 128, 128));
	fireAnimation.addFrame(sf::IntRect(256, 384, 128, 128));
	fireAnimation.addFrame(sf::IntRect(384, 384, 128, 128));

	Animation* currentAnimation = Game::GetAnimationManager().GetAnimation("fire lion left");

	AnimatedSprite animatedSprite(sf::seconds(1), true, false);
	animatedSprite.setPosition(Game::SCREEN_WIDTH / 2 + 84, Game::SCREEN_HEIGHT / 2 - 64);

	/*sf::Texture monsterTexture;
	monsterTexture.loadFromFile("data/images/monster/archon.png");
	
	sf::Sprite monsterCooler(monsterTexture);
	sf::Color monsterColor = monsterCooler.getColor();

	Animation monsterAnimation;
	monsterAnimation.setSpriteSheet(monsterTexture);
	monsterAnimation.addFrame(sf::IntRect(0, 0, 144, 144));
	monsterAnimation.addFrame(sf::IntRect(144, 0, 144, 144));
	monsterAnimation.addFrame(sf::IntRect(288, 0, 144, 144));
	monsterAnimation.addFrame(sf::IntRect(144, 0, 144, 144));
	monsterAnimation.addFrame(sf::IntRect(0, 0, 144, 144));

	AnimatedSprite monsterSprite(sf::seconds(0.03), false, false);
	monsterSprite.setPosition(Game::SCREEN_WIDTH / 2 - 216, Game::SCREEN_HEIGHT / 2 - 72);

	monsterSprite.play(*Game::GetAnimationManager().GetAnimation("monster"));
	//renderWindow.draw(background);
	player.Draw(renderWindow);
	renderWindow.draw(monsterSprite);
	//renderWindow.draw(iconTest);

	sf::Clock textFadeClock;
	sf::Clock turnTimer;
	sf::Clock frameClock;
	sf::Clock monsterFrameClock;

	sf::Event currentEvent;
	renderWindow.pollEvent(currentEvent);
	int turn = 0;
	bool playerAtt = false;
	/*sf::Text damageText("", Game::font);
	damageText.setCharacterSize(22);
	while (monster.GetCurrentHP() > 0 && player.GetCurrentHP() > 0)
	{
		monsterFrameClock.restart();
		/*  if (!mon_stunned)
			{
				monAtt(x, mon);
			}
			else if (mon_stunned)
			{
				cout << mon.getName() << " was stunned and it couldn't move!\n";
				mon_stunned = false;
			}
		

		// Player turn
		if (border1.getGlobalBounds().contains(sf::Mouse::getPosition(renderWindow).x, sf::Mouse::getPosition(renderWindow).y))
		{
			if (renderWindow.pollEvent(currentEvent) && currentEvent.type == sf::Event::MouseButtonReleased && turn == 0)
			{
					damageText.setString(BasicPhysAttack(player, monster));
					damageText.setPosition(monsterSprite.getPosition().x + 64, monsterSprite.getPosition().y - 64);
					animatedSprite.setPosition(player.GetPosition().x - 128, player.GetPosition().y - 64);
					textFadeClock.restart();
					turnTimer.restart();
					frameClock.restart();
					turn = 1;
					playerAtt = true;
			}
		}
		
		// Enemy turn
		if (turn == 1 && turnTimer.getElapsedTime().asSeconds() > 1)
		{
			damageText.setString(MonsterAttack(player, monster));
			damageText.setPosition(player.GetPosition().x, player.GetPosition().y - 32);
			textFadeClock.restart();
			turn = 0;
			playerAtt = false;
			animatedSprite.stop();
		}
		
		// Fade Damage Text
		if (textFadeClock.getElapsedTime().asSeconds() < 1)
		{
			damageText.setColor(sf::Color(255, 255, 255, 255 - 255 * textFadeClock.getElapsedTime().asSeconds()));
			damageText.setPosition(damageText.getPosition().x, damageText.getPosition().y - 0.25f);
		}


		//renderWindow.draw(background);
		player.Draw(renderWindow);
		//monster1.Draw(renderWindow);
		Game::GetGuiObjectManager().DrawHUD(renderWindow, currentEvent, player);
		Game::GetGuiObjectManager().DrawMonsterHUD(renderWindow, monster);
		Game::GetGameObjectManager().Get("cursor")->SetPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(renderWindow)));

		if (frameClock.getElapsedTime().asSeconds() < 1 && playerAtt)
		{
			animatedSprite.play(*currentAnimation);
			animatedSprite.move(sf::Vector2f(-3.0f, 0.0f));
			animatedSprite.update(frameClock.getElapsedTime());
			renderWindow.draw(animatedSprite);
			monsterSprite.setColor(sf::Color(monsterColor.r, monsterColor.g, monsterColor.b, 255 * (frameClock.getElapsedTime().asMilliseconds() % 2)));
		}
		else monsterSprite.setColor(sf::Color(monsterColor.r, monsterColor.g, monsterColor.b, 255));

		//renderWindow.draw(iconTest);
		//renderWindow.draw(border1);
		//renderWindow.draw(border2);
		monsterSprite.play(monsterAnimation);
		monsterSprite.update(monsterFrameClock.getElapsedTime());
		renderWindow.draw(monsterSprite);
		renderWindow.draw(damageText);
		Game::GetGameObjectManager().Get("cursor")->Draw(renderWindow);
		renderWindow.display();
	}

	if (player.GetCurrentHP() <= 0)
	{
		player.AddHP(player.GetMaxHP());
		monster.AddHP(monster.GetMaxHP());
		monster.SetPosition(monsterPosX, monsterPosY);
	}
	if (monster.GetCurrentHP() <= 0)
	{
		player.GainXP(monster.GetXPReward());

		Equipment* itemDrop = new Equipment(monster);
		Game::equipmentOnFloor.push_back(itemDrop);
		//checkDrop();
	}
	player.SetPosition(playerPosX, playerPosY);
	player.SetScale(1, 1);

	return true;
}*/

std::string BasicPhysAttack(Player& player, Monster& monster)
{
	int minDmg = (int)(player.GetPhysAttDmg() * .6);
	int damage = (rand() % (player.GetPhysAttDmg() - minDmg)) + minDmg;

	/*
	If player has burning enchantment roll to see if monster is inflicted with burning.
	33% chance to inflict burning
	Burning does 3% of the monster's total health per turn
	*/
	/*if (player.getEnchant() == 1) {
		if ((rand() % 100) < 33 * x.getRandMod()) monster.SetBurn(true);
	}*/

	return PlayerAttackResolve(player, monster, damage);
}

std::string PlayerAttackResolve(Player& player, Monster& monster, int damage)
{
	std::stringstream ss;
	int burnDamage = 0;
	/*if (mon.IsBurning())
	{
		burnDamage = monster.GetMaxHP() * 0.03;
		damage *= player.getBurnedMonMod();
	}*/

	int damage_mitigation = (monster.GetDefense() / (monster.GetDefense() + player.GetLevel() * 50));
	//Hard cap of damage mitigation is 80%.
	if (damage_mitigation > .8) damage_mitigation = .8;
	damage *= 1 - damage_mitigation;
	monster.AddHP((damage + burnDamage) * -1);

	if (monster.GetCurrentHP() > 0)
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
	int damage = (rand() % (monster.GetMaxDmg() - monster.GetMinDmg())) + monster.GetMinDmg();

	return MonsterAttackResolve(player, monster, damage);
}

std::string MonsterAttackResolve(Player& player, Monster & monster, int damage)
{
	int damage_mitigation = (player.GetDefense() / (player.GetDefense() + (monster.GetLevel() * 50)));
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