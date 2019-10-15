#pragma once
#include <SFML/System.hpp>
#include <ctime>
#include "Interface.h"

class FTickHandle : public ITickable
{
public:
	FTickHandle() {}
	~FTickHandle() {}
	virtual void Tick() override;
	void BeginTick();

private:
	float TimeElapsedSinceLastFrame = 0.0f;
	sf::Clock FixedUpdateClock;
};

