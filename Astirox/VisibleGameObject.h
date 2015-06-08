#ifndef _VISIBLEGAMEOBJECT_H
#define _VISIBLEGAMEOBJECT_H


class VisibleGameObject
{
public:
	VisibleGameObject();
	VisibleGameObject(std::string filename);
	VisibleGameObject(std::string filename, int left, int top, int width, int height);
	VisibleGameObject(sf::Texture& texture, sf::IntRect& textureCoords);
	virtual ~VisibleGameObject();

	virtual void Load(std::string filename);
	virtual void Load(std::string filename, int left, int top, int width, int height);
	virtual void Load(sf::Texture& texture, sf::IntRect& textureCoords);
	virtual void Draw(sf::RenderWindow & window);
	virtual void Update(float elapsedTime);

	virtual void SetPosition(float x, float y);
	virtual void SetPosition(sf::Vector2f position);
	virtual sf::Vector2f GetPosition() const;
	virtual void SetScale(float x, float y);
	virtual void SetTransparency(int a);
	virtual void SetOrigin(float x, float y);

	virtual bool IsLoaded();

	virtual float GetWidth() const;
	virtual float GetHeight() const;
	virtual sf::Vector2f GetOrigin();

	virtual sf::Rect<float> GetBoundingRect() const;

	const static int MOVE_SPEED = 16;

protected:
	sf::Sprite& GetSprite();

private:
	sf::Sprite _sprite;
	sf::Texture _image;
	std::string _filename;
	bool _isLoaded;

};

#endif