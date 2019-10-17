#pragma once
#include "PhysicComponent.h"
#include "Interface.h"
#include "Actor.h"

class FText;

#define JUMP_BLOCK_INTERVAL 0.15f

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

	// For bindings
	static void BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB);
	static void EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB);

private:
	bool bWantToMoveLeft = false;
	bool bWantToMoveRight = false;
	bool bJump = false;
	bool bInitialized = false;

	float JumpTimedOutTimer = JUMP_BLOCK_INTERVAL;
#if	DEBUG_GAME
	std::unique_ptr<FText> DebugJumpFlagText = std::make_unique<FText>();
#endif
};

