#include "Character.h"
#include "Actor.h"
#include "Application.h"
#include "Defines.h"
#include "TextManager.h"
#include <sstream>
#include <iomanip>



Character::Character(std::string Name, int ID)
{
	SetObjectName(Name);
	SetID(ID);

	FTextManager::Register(FTextEffectBundle(DebugText.get())); 
	FTextManager::Register(FTextEffectBundle(OverlapDataText.get()));
}

Character::~Character()
{
}

void Character::Initialize()
{
	if (bInitialized)
		return;

	// Change scaling to modify character size.
	const float Scale = 0.75f;
	const sf::Vector2f Size = UNIT_VECTOR * Scale;

	// Visual - MainBody
	sf::RectangleShape* CharacterBody = new sf::RectangleShape();
	CharacterBody->setSize(Size);
	CharacterBody->setOrigin(CharacterBody->getSize() * 0.5f);
	CharacterBody->setFillColor(sf::Color::Transparent);
	CharacterBody->setOutlineThickness(1);
	CharacterBody->setOutlineColor(sf::Color::White);
	RegisterShape(FShapeID(CharacterBody, 1)); 
	
	// Box2D - MainBody
	PhysicComponent* Component = new PhysicComponent(GetObjectName() + "-PhysicComponent");
	Component->SetOwningParent(this);

	sf::View ViewPort;
	Application::GetInstance()->GetWindow()->getViewport(ViewPort);

	b2BodyDef CharacterBodyDef;
	CharacterBodyDef.fixedRotation = true;
	CharacterBodyDef.type = b2_dynamicBody;
	CharacterBodyDef.position.Set(UNIT_SFML_TO_BOX2D(ViewPort.getCenter().x), UNIT_SFML_TO_BOX2D(ViewPort.getSize().y *0.5f));
	Component->CreateBody(&CharacterBodyDef);
	Component->GetBody()->SetUserData((void*)this); // Main UserData on BodyInstance

	b2PolygonShape BodyShape;
	BodyShape.SetAsBox(UNIT_SFML_TO_BOX2D(Size.x * 0.5f), UNIT_SFML_TO_BOX2D(Size.y * 0.5f));

	b2FixtureDef CharacterFixtureDef;
	CharacterFixtureDef.shape = &BodyShape;
	CharacterFixtureDef.restitution = 0.0f;
	CharacterFixtureDef.friction = 0.5f;
	CharacterFixtureDef.density = 20.0f;
	CharacterFixtureDef.userData = ((void*)GAMETAG_PLAYER_BODY);

	Component->SetGenerateOverlap(true);
	Component->BindOnBeginoverlap(BeginOverlap);
	Component->BindOnEndOverlap(EndOverlap);
	Component->GetBody()->CreateFixture(&CharacterFixtureDef);

	b2MassData CharacterMass;
	CharacterMass.mass = 1.0f;
	CharacterMass.center = b2Vec2(0.0f, 0.0f);

	Component->GetBody()->SetMassData(&CharacterMass);

	// Visual - Foot 
	sf::RectangleShape* FootRect = new sf::RectangleShape();
	FootRect->setSize(Size * 0.5f);

	const sf::Vector2f Offset = Scale > 0 ? sf::Vector2f(0.0f, (Scale - 1.0f) * 16.0f) : sf::Vector2f(0.0f, 0.0f);
	FootRect->setOrigin(FootRect->getSize() * 0.5f - Offset);
	FootRect->setFillColor(sf::Color::Red);
	FootRect->setOutlineColor(sf::Color::White);
	RegisterShape(FShapeID(FootRect, 2));

	// Box2D - Foot
	const float BoxSize = Size.x * 0.5f;
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(UNIT_SFML_TO_BOX2D(BoxSize), UNIT_SFML_TO_BOX2D(BoxSize) , b2Vec2(0, UNIT_SFML_TO_BOX2D (BoxSize * 0.5f)), 0);

	b2FixtureDef FootFixtureDef;
	FootFixtureDef.isSensor = true;
	FootFixtureDef.shape = &polygonShape;
	FootFixtureDef.density = 1;
	FootFixtureDef.userData = (void*)GAMETAG_PLAYER_FOOT;

	Component->GetBody()->CreateFixture(&FootFixtureDef);
	Registerb2Component(Fb2ComponentID(Component, 1));

	bInitialized = true;
}

void Character::Tick()
{
	Actor::Tick();

	// Update Foot Sensor Position
	ObjectShapes[1].Shape->setPosition(UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().x), UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().y) + 16.f);

	if(JumpInputTimedOutTimer >= -1.0f)
		JumpInputTimedOutTimer -= DELTA_TIME_STEP;

	if (bJump)
	{
		JumpMaxHoldTimer += DELTA_TIME_STEP;
		if (JumpMaxHoldTimer >= JUMP_MAX_HOLD_THRESHOLD_TIME)
			JumpMaxHoldTimer = JUMP_MAX_HOLD_THRESHOLD_TIME;
	}

	if (DashCoolDownTimer <= 0.0f && bWantToDash)
	{
		bDash = true;
		DashCoolDownTimer = DASH_COOLDOWN;
		DashExecuteTimer = DASH_EXECUTE_LOCK;

		// No longer affect by gravity and then cut off original speed.
		b2Component.Component->GetBody()->SetGravityScale(0.0f);
		b2Component.Component->GetBody()->SetLinearVelocity(b2Vec2(0, 0));

		float ImpulseX = b2Component.Component->GetBody()->GetMass() * DASH_SPEED;
		if (LastFaceDirection == EFD_Right)
		{
			b2Component.Component->GetBody()->ApplyLinearImpulse(b2Vec2(ImpulseX, 0), b2Component.Component->GetBody()->GetWorldCenter(), true);
		}
		else if (LastFaceDirection == EFD_Left)
		{
			b2Component.Component->GetBody()->ApplyLinearImpulse(b2Vec2(-ImpulseX, 0), b2Component.Component->GetBody()->GetWorldCenter(), true);
		}
	}

	if (DashCoolDownTimer > 0.0f)
		DashCoolDownTimer -= DELTA_TIME_STEP;

	if (DashExecuteTimer > 0.0f)
		DashExecuteTimer -= DELTA_TIME_STEP;

	if (DashExecuteTimer < 0.0f)
	{
		//
		b2Component.Component->GetBody()->SetGravityScale(1.0f);
		bDash = false;
	}

	UpdateMovement();
}
void Character::MoveLeft()
{
	if (bInitialized)
	{
		bWantToMoveLeft = true;
		LastFaceDirection = EFD_Left;
	}
}

void Character::MoveRight()
{
	if (bInitialized)
	{
		bWantToMoveRight = true;
		LastFaceDirection = EFD_Right;
	}
}

void Character::DashPressed()
{
	if (bInitialized)
	{
		bWantToDash = true;
	}
}

void Character::DashReleased()
{
	if (bInitialized)
	{
		bWantToDash = false;
	}
}

void Character::GrabPressed()
{
	if (bInitialized)
	{
		bWantToGrab = true;
	}
}

void Character::GrabRelease()
{
	if (bInitialized)
	{
		bWantToGrab = false;
		bGrab = false;

		b2Component.Component->GetBody()->SetGravityScale(1.0f);
	}
}

void Character::JumpPressed()
{	
	if (bInitialized)
	{
		bWantToJump = (JumpMaxHoldTimer < JUMP_MAX_HOLD_THRESHOLD_TIME);

		if (!bJump && JumpInputTimedOutTimer <= 0.0f)
		{
			bJump = true;
		}
	}
}

void Character::JumpReleased()
{
	if (bInitialized)
	{
		bWantToJump = false;

		if (bJump)
		{
			JumpMaxHoldTimer = JUMP_MAX_HOLD_THRESHOLD_TIME;
		}
	}
}

void Character::UpdateMovement()
{
	if (bInitialized)
	{
		b2Vec2 Velocity = b2Component.Component->GetBody()->GetLinearVelocity();

		// Decay multiplier on individual component.
		float Vx = Velocity.x * 0.98f;
		float Vy = Velocity.y; //(Velocity.y >= 0) ? Velocity.y * 0.01f : Velocity.y * 100.0f; // If Vy is positive, reduce 1/4 per tick, or Vy is negative, increase 4/1 per tick.

		// Y speed decay isn't fast enough. Need a very strong value.

		// Gradually accelerate to respective direction.
		if (bWantToMoveLeft && !bWantToMoveRight)
			Vx = b2Max(Velocity.x - WALKING_SPEED_DELTA, -WALKING_SPEED_MAX);

		if (!bWantToMoveLeft && bWantToMoveRight) 
			Vx = b2Min(Velocity.x + WALKING_SPEED_DELTA, +WALKING_SPEED_MAX);

		if (bWantToJump)
			Vy = Velocity.y + 8.0f; //b2Min(Velocity.y + 16.0f, +16.0f);

		float DeltaVelocityX = Vx - Velocity.x;
		float DeltaVelocityY = Vy - Velocity.y;
		float ImpulseX = b2Component.Component->GetBody()->GetMass() * DeltaVelocityX;
		float ImpulseY = b2Component.Component->GetBody()->GetMass() * DeltaVelocityY;

		// Allow horizontal movement even if jumping but not dashing.
		if(!bDash)
			b2Component.Component->GetBody()->ApplyLinearImpulse(b2Vec2(ImpulseX, 0), b2Component.Component->GetBody()->GetWorldCenter(), true);

		if (bJump && bWantToJump && JumpMaxHoldTimer < JUMP_MAX_HOLD_THRESHOLD_TIME) // continue pumping it if want to jump.
		{
			b2Component.Component->GetBody()->ApplyLinearImpulse(b2Vec2(0, -ImpulseY), b2Component.Component->GetBody()->GetWorldCenter(), true);
		}

		bWantToMoveLeft = false;
		bWantToMoveRight = false;
		bWantToJump = false;
			
		// Debug information
		b2Vec2 v = b2Component.Component->GetBody()->GetLinearVelocity();

		std::ostringstream ss;
		ss << std::setprecision(3);
		ss << std::fixed;
		ss << "Vx = " << v.x << "\nVy = " << v.y;
		ss << "\nWantJump=" << (bWantToJump ? "Yes" : "No");
		ss << "\nJumpState="<< (bJump ? "Jumping" : "NotJumping");
		ss << "\nJumpHoldTime=" << JumpMaxHoldTimer;
		ss << "\nWantGrab=" << (bWantToGrab ? "Yes" : "No");
		ss << "\nGrabState=" << (bGrab ? "Grabbed" : "NotGrabbed");
		ss << "\nWantDash=" << (bWantToDash ? "Yes" : "No");
		ss << "\nDashCoolDown=" << DashCoolDownTimer;
		ss << "\nFacingDirection=" << (LastFaceDirection == EFaceDirection::EFD_Right) ? "Right" : "Left";

		DebugText->SetText(ss.str());
		DebugText->Text.setPosition(sf::Vector2f(32.0f, 32.0f*2));
	}
}

void Character::BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB)
{
	if (Component && OverlapComponent && OverlapComponent->GetOwner())
	{
		std::ostringstream ss;
		ss << std::setprecision(3);
		ss << std::fixed;
		ss << "Object: " << OverlapComponent->GetOwner()->GetObjectName();
		ss << "\nComponent= " << OverlapComponent->GetComponentName();
		ss << "\nBodyUserData= " << (int)OverlapComponent->GetBody()->GetUserData();
		ss << "\nFixtureUserData= " << GetTagName((int)UserDataB);
		ss << "\nMyBodyUserData=" << (int)Component->GetBody()->GetUserData();
		ss << "\nMyFixtureUserData=" << GetTagName((int)UserDataA);

		Character* pCharacter = static_cast<Character*>(Component->GetOwner());
		if ((int)UserDataA == GAMETAG_PLAYER_FOOT && (int)UserDataB == GAMETAG_STATIC_FLOOR)
		{
			if (!pCharacter) return;
			
			pCharacter->bJump = false;
			pCharacter->JumpInputTimedOutTimer = JUMP_BLOCK_INTERVAL;
			pCharacter->JumpMaxHoldTimer = 0.0f;
		}

		if ((int)UserDataB == GAMETAG_STATIC_WALL)
		{
			std::string v = GetTagName((int)UserDataA);

			bool bFinishCheckList = false;
			b2Fixture* Fixture = Component->GetBody()->GetFixtureList();// ->GetNext();
			while (!bFinishCheckList)
			{
				if ((int)Fixture->GetUserData() == GAMETAG_PLAYER_BODY)
				{
					if (!pCharacter) return;

					if (pCharacter->bWantToGrab)
					{
						pCharacter->bJump = false;
						pCharacter->bWantToGrab = false;
						pCharacter->bGrab = true;
						
						////b2Vec2 v = pCharacter->Getb2Component().Component->GetBody()->GetLinearVelocity();
						pCharacter->Getb2Component().Component->GetBody()->SetGravityScale(0.0f);
						
						/// TODO: 
						// Do something to the velocity to make it pin on the wall and make it stay that way until release
					}
				}

				Fixture = Fixture->GetNext();
				if (Fixture == nullptr)
					bFinishCheckList = true;
			}
		}

		pCharacter->OverlapDataText->SetText(ss.str());
		pCharacter->OverlapDataText->Text.setPosition(sf::Vector2f(32.0f, 32.0f * 8));
	}
}

void Character::EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB)
{
	
}