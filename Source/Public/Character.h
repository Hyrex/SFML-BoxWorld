#pragma once
#include "b2Actor2D.h"
#include "Interface.h"

class Application;

class Character : public ITickable
{
public:

	Character();
	virtual ~Character();
	virtual void Tick() override;

	void Initialize(Application* App);
	void MoveLeft();
	void MoveRight();
	void Jump();

	bool IsInitialized() const { return bInitialized; }
	b2Actor2D* Getb2Actor() { return b2Actor.get(); }

private:

	std::unique_ptr<b2Actor2D> b2Actor;
	bool bInitialized = false;
};

