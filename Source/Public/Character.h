#pragma once
#include "PhysicComponent.h"
#include "Interface.h"
#include "Actor.h"

class FText;

#define JUMP_BLOCK_INTERVAL				0.15f
#define JUMP_MAX_HOLD_THRESHOLD_TIME	0.5f

#define WALKING_SPEED_MAX				7.5f
#define WALKING_SPEED_DELTA				5.0f
#define DASH_SPEED						20.0f
#define DASH_COOLDOWN					0.5f
#define DASH_EXECUTE_LOCK				0.15f

class Character : public Actor
{
public:
	Character() {}
	Character(std::string Name, int ID);
	virtual ~Character();
	virtual void Tick() override;
	virtual void Draw() override;

	void Initialize();
	void MoveLeft();
	void MoveRight();
	void DashPressed();
	void DashReleased();
	void GrabPressed();
	void GrabRelease();
	void JumpPressed();
	void JumpReleased();
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
	// Request pending flags
	bool bWantToMoveLeft = false;
	bool bWantToMoveRight = false;
	bool bWantToJump = false;
	bool bWantToGrab = false;
	bool bWantToDash = false;

	// Actual states
	bool bDash = false;
	bool bGrab = false;
	bool bJump = false;
	bool bInitialized = false;
	
	// Timed out actions.
	float JumpMaxHoldTimer = 0.0f;
	float JumpInputTimedOutTimer = 10.0f;

	float DashCoolDownTimer = 0.0f;		// Time required to reuse Dash.
	float DashExecuteTimer = 0.0f; // Time required Character to fully perform a dash. During this period input are blocked.

	float CharacterScale = 0.75f;
	sf::Vector2f CharacterSize = UNIT_VECTOR * CharacterScale;

	// Foot RayCast line
	sf::Vertex FootLine[2];
};

