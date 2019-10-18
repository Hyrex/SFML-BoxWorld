#pragma once
#include "PhysicComponent.h"
#include "Interface.h"
#include "Actor.h"

class FText;

#define JUMP_BLOCK_INTERVAL				0.15f
#define JUMP_MAX_HOLD_THRESHOLD_TIME	0.1f

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
	void StartJump();
	void StopJump();
	void UpdateMovement();

	bool IsInitialized() const { return bInitialized; }
	bool IsJumping() const { return bJump; }

	// For bindings
	static void BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB);
	static void EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB);

#if	DEBUG_GAME
	std::unique_ptr<FText> DebugText = std::make_unique<FText>();
	std::unique_ptr<FText> OverlapDataText = std::make_unique<FText>();
#endif

private:
	bool bWantToMoveLeft = false;
	bool bWantToMoveRight = false;
	bool bWantToJump = false;
	bool bJump = false;
	bool bInitialized = false;
	
	float JumpMaxHoldTimer = 0.0f;
	float JumpInputTimedOutTimer = 10.0f;


};

