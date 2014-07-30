#ifndef _HUD_H
#define _HUD_H

#include "VisibleGameObject.h"

class HUD :
	public VisibleGameObject
{
public:
	HUD();
	~HUD();

	void Update();
	void Draw(sf::RenderWindow& rw);

private:

};


#endif