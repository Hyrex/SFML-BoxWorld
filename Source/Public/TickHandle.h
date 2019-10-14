#pragma once
#include <SFML/System.hpp>
#include <ctime>
#include "Defines.h"
#include "Interface.h"

class Application;

class FTickHandle : public ITickable
{
public:
	FTickHandle();
	~FTickHandle();
	virtual void Tick() override;
	void BeginTick();

private:
	float TimeElapsedSinceLastFrame = 0.0f;
	SFML::Clock FixedUpdateClock;
};

