#pragma once
#include "b2Actor2D.h"
#include "Interface.h"

class Character : public ITickable
{
public:

	Character();
	virtual ~Character();
	virtual void Tick() override;

	void Initialize();
	void MoveLeft();
	void MoveRight();
	void Jump();
	void UpdateMovement();

	bool IsInitialized() const { return bInitialized; }
	bool IsJumping() const { return bJump; }
	b2Actor2D* Getb2Actor() { return b2Actor.get(); }

private:
	bool bWantToMoveLeft = false;
	bool bWantToMoveRight = false;
	bool bJump = false;
	bool bInitialized = false;
	std::unique_ptr<b2Actor2D> b2Actor;

};

