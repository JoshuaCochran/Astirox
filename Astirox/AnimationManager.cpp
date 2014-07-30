#include "stdafx.h"
#include "AnimationManager.hpp"

AnimationManager::AnimationManager()
{
}

AnimationManager::~AnimationManager()
{
	std::for_each(_animationObjects.begin(), _animationObjects.end(), AnimationObjectDeallocator());
	std::for_each(_animationTextures.begin(), _animationTextures.end(), AnimationTextureDeallocator());
	delete spellSprite;
}

void AnimationManager::AddTexture(std::string name, sf::Texture* texture)
{
	_animationTextures.insert(std::pair<std::string, sf::Texture*>(name, texture));
}
void AnimationManager::AddAnimation(std::string name, Animation* animation)
{
	_animationObjects.insert(std::pair<std::string, Animation*>(name, animation));
}

sf::Texture* AnimationManager::GetTexture(std::string name) const
{
	std::map<std::string, sf::Texture *>::const_iterator results = _animationTextures.find(name);
	if (results == _animationTextures.end())
		return NULL;
	return results->second;
}

Animation* AnimationManager::GetAnimation(std::string name) const
{
	std::map<std::string, Animation*>::const_iterator results = _animationObjects.find(name);
	if (results == _animationObjects.end())
		return NULL;
	return results->second;
}

Animation* AnimationManager::GetSpellAnimation()
{
	return spellAnimation;
}

void AnimationManager::SetSpellAnimation(Animation* animation)
{
	spellAnimation = animation;
}

AnimatedSprite* AnimationManager::GetSpellSprite()
{
	return spellSprite;
}

void AnimationManager::Init()
{
	spellAnimation = NULL;
	spellSprite = new AnimatedSprite(sf::seconds(1), true, false);

	sf::Texture *fireLionLeftTexture = new sf::Texture;
	fireLionLeftTexture->loadFromFile("data/firelion_left.png");
	AddTexture("fire lion left", fireLionLeftTexture);

	Animation* fireAnimation = new Animation;
	fireAnimation->setSpriteSheet(*GetTexture("fire lion left"));
	fireAnimation->addFrame(sf::IntRect(0, 0, 128, 128));
	fireAnimation->addFrame(sf::IntRect(128, 0, 128, 128));
	fireAnimation->addFrame(sf::IntRect(256, 0, 128, 128));
	fireAnimation->addFrame(sf::IntRect(384, 0, 128, 128));
	fireAnimation->addFrame(sf::IntRect(0, 128, 128, 128));
	fireAnimation->addFrame(sf::IntRect(128, 128, 128, 128));
	fireAnimation->addFrame(sf::IntRect(256, 128, 128, 128));
	fireAnimation->addFrame(sf::IntRect(384, 128, 128, 128));
	fireAnimation->addFrame(sf::IntRect(0, 256, 128, 128));
	fireAnimation->addFrame(sf::IntRect(128, 256, 128, 128));
	fireAnimation->addFrame(sf::IntRect(256, 256, 128, 128));
	fireAnimation->addFrame(sf::IntRect(384, 256, 128, 128));
	fireAnimation->addFrame(sf::IntRect(0, 384, 128, 128));
	fireAnimation->addFrame(sf::IntRect(128, 384, 128, 128));
	fireAnimation->addFrame(sf::IntRect(256, 384, 128, 128));
	fireAnimation->addFrame(sf::IntRect(384, 384, 128, 128));

	sf::Texture *monsterTexture = new sf::Texture;
	monsterTexture->loadFromFile("data/images/monster/archon.png");
	AddTexture("monster texture", monsterTexture);

	Animation* monsterAnimation = new Animation;
	monsterAnimation->setSpriteSheet(*GetTexture("monster texture"));
	monsterAnimation->addFrame(sf::IntRect(0, 0, 144, 144));
	monsterAnimation->addFrame(sf::IntRect(144, 0, 144, 144));
	monsterAnimation->addFrame(sf::IntRect(288, 0, 144, 144));
	monsterAnimation->addFrame(sf::IntRect(144, 0, 144, 144));
	monsterAnimation->addFrame(sf::IntRect(0, 0, 144, 144));

	AddAnimation("fire lion left", fireAnimation);
	AddAnimation("monster", monsterAnimation);
}