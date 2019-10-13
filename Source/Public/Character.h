#pragma once
#include "b2Actor2D.h"
#include "Interface.h"

class Application;

class Character : public ITickable
{
public:

	std::unique_ptr<b2Actor2D> b2Actor;

	void Spawn(Application* App);

	virtual void Tick() override;

	bool bSpawned = false;

	Character();
	virtual ~Character();
};

