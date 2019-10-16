#pragma once
#include "Actor.h"

class StaticBlockActor : public Actor
{
public:

	StaticBlockActor(std::string Name, int ID);
	virtual ~StaticBlockActor();
	void Construct(sf::Vector2f Size = UNIT_VECTOR, sf::Vector2f Location = UNIT_ZERO_VECTOR, const float Rotation = 0.0f); //wip
private:

};

