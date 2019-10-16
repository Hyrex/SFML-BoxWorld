#pragma once
#include "b2Actor2D.h"
#include "Interface.h"
#include "Actor.h"

class Character : public Actor
{
public:
	Character() {}
	Character(std::string Name, int ID);
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

	static void BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB);
	static void EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB);

private:
	bool bWantToMoveLeft = false;
	bool bWantToMoveRight = false;
	bool bJump = false;
	bool bInitialized = false;
	std::unique_ptr<b2Actor2D> b2Actor;
};

