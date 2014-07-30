#ifndef _ANIMATIONMANAGER_HPP
#define _ANIMATIONMANAGER_HPP

class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();

	void AddTexture(std::string name, sf::Texture* texture);
	void AddAnimation(std::string name, Animation* animation);
	sf::Texture* GetTexture(std::string name) const;
	Animation* GetAnimation(std::string name) const;
	Animation* GetSpellAnimation();
	AnimatedSprite* GetSpellSprite();

	void SetSpellAnimation(Animation* animation);


	void Init();

private:
	std::map<std::string, Animation*> _animationObjects;
	std::map<std::string, sf::Texture*> _animationTextures;
	Animation* spellAnimation;
	AnimatedSprite* spellSprite;

	struct AnimationObjectDeallocator
	{
		void operator() (const std::pair<std::string, Animation*> & p) const
		{
			delete p.second;
		}
	};

	struct AnimationTextureDeallocator
	{
		void operator() (const std::pair<std::string, sf::Texture*> & p) const
		{
			delete p.second;
		}
	};
};

#endif