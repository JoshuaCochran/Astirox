#include "stdafx.h"
#include "VisibleGameObject.h"

VisibleGameObject::VisibleGameObject()
: _isLoaded(false)
{
}
VisibleGameObject::VisibleGameObject(std::string filename)
: _isLoaded(false)
{
	Load(filename);
}

VisibleGameObject::VisibleGameObject(std::string filename, int left, int top, int width, int height)
: _isLoaded(false)
{
	Load(filename, left, top, width, height);
}

VisibleGameObject::VisibleGameObject(sf::Texture& texture, sf::IntRect& textureCoords)
: _isLoaded(true)
{
	_image = texture;
	_sprite.setTexture(_image);
	_sprite.setTextureRect(textureCoords);
}

VisibleGameObject::~VisibleGameObject()
{
}

void VisibleGameObject::Load(std::string filename)
{
	if (_image.loadFromFile(filename) == false)
	{
		_filename = "";
		_isLoaded = false;
	}
	else
	{
		_filename = filename;
		_sprite.setTexture(_image);
		_isLoaded = true;
	}
}

void VisibleGameObject::Load(sf::Texture& texture, sf::IntRect& textureCoords)
{
	_image = texture;
	_sprite.setTexture(_image);
	_sprite.setTextureRect(textureCoords);
	_isLoaded = true;
}

void VisibleGameObject::Load(std::string filename, int left, int top, int width, int height)
{
	if (_image.loadFromFile(filename, sf::IntRect(left, top, width, height)) == false)
	{
		_filename = "";
		_isLoaded = false;
	}
	else
	{
		_filename = filename;
		_sprite.setTexture(_image);
		_isLoaded = true;
	}
}


void VisibleGameObject::Draw(sf::RenderWindow & renderWindow)
{
	if (_isLoaded)
	{
		renderWindow.draw(_sprite);
	}
}

void VisibleGameObject::Update(float elapsedTime)
{
}

bool VisibleGameObject::moveto(sf::Event currentEvent)
{
	return true;
}

void VisibleGameObject::SetPosition(float x, float y)
{
	if (_isLoaded)
	{
		_sprite.setPosition(x, y);
	}
}

void VisibleGameObject::SetPosition(sf::Vector2f position)
{
	if (_isLoaded)
	{
		_sprite.setPosition(position);
	}
}

sf::Vector2f VisibleGameObject::GetPosition() const
{
	if (_isLoaded)
	{
		return _sprite.getPosition();
	}
	return sf::Vector2f();
}

int VisibleGameObject::GetStat(int stat) const
{
	return 0;
}


sf::Sprite& VisibleGameObject::GetSprite()
{
	return _sprite;
}

void VisibleGameObject::SetScale(float x, float y)
{
	if (_isLoaded)
	{
		_sprite.setScale(x, y);
	}
}

void VisibleGameObject::SetTransparency(int a)
{
	sf::Color color = _sprite.getColor();
	if (_isLoaded)
	{
		_sprite.setColor(sf::Color(color.r, color.g, color.b, a));
	}
}

void VisibleGameObject::SetOrigin(float x, float y)
{
	_sprite.setOrigin(x, y);
}

sf::Vector2f VisibleGameObject::GetOrigin()
{
	return _sprite.getOrigin();
}

bool VisibleGameObject::IsLoaded()
{
	return _isLoaded;
}

float VisibleGameObject::GetHeight() const
{
	return _sprite.getLocalBounds().height;
}

float VisibleGameObject::GetWidth() const
{
	return _sprite.getLocalBounds().width;
}

sf::Rect<float> VisibleGameObject::GetBoundingRect() const
{
	return _sprite.getGlobalBounds();
}

