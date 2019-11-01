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

	// Visual - MainBody
	sf::RectangleShape* CharacterBody = new sf::RectangleShape();
	CharacterBody->setSize(CharacterSize);
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
	CharacterBodyDef.position.Set(UNIT_BOX2D(ViewPort.getCenter().x), UNIT_BOX2D(ViewPort.getSize().y *0.5f));
	Component->CreateBody(&CharacterBodyDef);
	Component->GetBody()->SetUserData((void*)this); // Main UserData on BodyInstance

	b2PolygonShape BodyShape;
	BodyShape.SetAsBox(UNIT_BOX2D(CharacterSize.x * 0.5f), UNIT_BOX2D(CharacterSize.y * 0.5f));

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
	FootRect->setSize(CharacterSize * 0.5f);

	const sf::Vector2f Offset = CharacterScale > 0 ? sf::Vector2f(0.0f, (CharacterScale - 1.0f) * 16.0f) : sf::Vector2f(0.0f, 0.0f);
	FootRect->setOrigin(FootRect->getSize() * 0.5f - Offset);
	FootRect->setFillColor(sf::Color::Red);
	FootRect->setOutlineColor(sf::Color::White);
	RegisterShape(FShapeID(FootRect, 2));

	// Box2D - Foot
	const float BoxSize = CharacterSize.x * 0.5f;
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(UNIT_BOX2D(BoxSize), UNIT_BOX2D(BoxSize) , b2Vec2(0, UNIT_BOX2D (BoxSize * 0.5f)), 0);

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
	ObjectShapes[1].Shape->setPosition(UNIT_SFML(GetPhysicComponent()->GetBody()->GetPosition().x), UNIT_SFML(GetPhysicComponent()->GetBody()->GetPosition().y) + 16.f);

	if(JumpInputTimedOutTimer >= -1.0f)
		JumpInputTimedOutTimer -= DELTA_TIME_STEP;

	if (bJump)
	{
		JumpHoldTime += DELTA_TIME_STEP;
	}

	if (JumpHoldTime >= JUMP_HOLD_MAX_TIME)
	{
		bCanJump = false;
		JumpHoldTime = JUMP_HOLD_MAX_TIME;
	}

	if (DashCoolDownTimer > 0.0f)
		DashCoolDownTimer -= DELTA_TIME_STEP;

	if (DashExecuteTimer > 0.0f)
		DashExecuteTimer -= DELTA_TIME_STEP;

	if (DashExecuteTimer <= 0.0f && bDash)
	{
		GetPhysicComponent()->GetBody()->SetGravityScale(1.0f);
		const b2Vec2 V = GetPhysicComponent()->GetBody()->GetLinearVelocity();
		GetPhysicComponent()->GetBody()->SetLinearVelocity(b2Vec2(V.x * 0.1f, V.y));
		bDash = false;
	}

	UpdateMovement();

	
#if 0 // // Foot Ray cast doesnt work but keep for reference.
	const float RayLength = 0.25f;
	b2Vec2 StartLocation(Getb2WorldLocation() + b2Vec2(0, UNIT_BOX2D(CharacterSize.y  * 0.5f)));
	b2Vec2 EndLocation = StartLocation + b2Vec2(0, (RayLength));

	FootLine[0].position = UNIT_SFML_VEC(StartLocation);
	FootLine[1].position = UNIT_SFML_VEC(EndLocation);

	// Using own fixture to ray cast downward
	b2RayCastInput HitInput;
	HitInput.p1 = StartLocation;
	HitInput.p2 = EndLocation;
	HitInput.maxFraction = 1;

	for (b2Fixture* f = GetPhysicComponent()->GetBody()->GetFixtureList(); f; f = f->GetNext())
	{
		b2RayCastOutput HitResult;
		if (!f->RayCast(&HitResult, HitInput,0))
			continue;
		
		bJump = false;
	}
#endif
}

void Character::Draw()
{
	if (bInitialized)
	{
		sf::RenderWindow* const pWindow = Application::GetInstance()->GetWindow();
		
		for (int i = 0; i < GetShapeCount(); ++i)
		{
			if (sf::Shape* s = GetShapeAtIndex(i))
				pWindow->draw(*s);
		}

		pWindow->draw(*GetPhysicComponent()->GetDebugForward());
		pWindow->draw(FootLine, 2, sf::Lines);
	}
	Actor::Draw();
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

		GetPhysicComponent()->GetBody()->SetGravityScale(1.0f);
	}
}

void Character::JumpPressed()
{	
	if (bInitialized)
	{
		if (!bDash && !bWantToDash)
		{
			bWantToJump = true;

			if (!bJump && JumpInputTimedOutTimer <= 0.0f)
			{
				bJump = true;
			}
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
			JumpHoldTime = JUMP_HOLD_MAX_TIME;
		}
	}
}

void Character::UpdateMovement()
{
	if (bInitialized)
	{
		// Horizontal Movement
		if (!bDash)
		{
			// Decay multiplier on individual component.
			const b2Vec2 Velocity = GetPhysicComponent()->GetBody()->GetLinearVelocity();
			float Vx = Velocity.x * 0.98f;

			// Gradually accelerate to respective direction.
			if (bWantToMoveLeft && !bWantToMoveRight)
				Vx = b2Max(Velocity.x - WALKING_SPEED_DELTA, -WALKING_SPEED_MAX);

			if (!bWantToMoveLeft && bWantToMoveRight)
				Vx = b2Min(Velocity.x + WALKING_SPEED_DELTA, +WALKING_SPEED_MAX);

			const float ImpulseX = GetPhysicComponent()->GetBody()->GetMass() * (Vx - Velocity.x);
			GetPhysicComponent()->GetBody()->ApplyLinearImpulse(b2Vec2(ImpulseX, 0), GetPhysicComponent()->GetBody()->GetWorldCenter(), true);
		}

		const bool bExecuteDash = bWantToDash && bCanDash;
		if (bExecuteDash && DashCoolDownTimer <= 0.0f)
		{
			bDash = true;
			DashCoolDownTimer = DASH_COOLDOWN;
			DashExecuteTimer = DASH_EXECUTE_LOCK;

			// No longer affect by gravity and then cut off original speed.
			GetPhysicComponent()->GetBody()->SetGravityScale(0.0f);
			GetPhysicComponent()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));

			float ImpulseX = GetPhysicComponent()->GetBody()->GetMass() * DASH_SPEED;
			if (LastFaceDirection == EFD_Right)
			{
				GetPhysicComponent()->GetBody()->ApplyLinearImpulse(b2Vec2(ImpulseX, 0), GetPhysicComponent()->GetBody()->GetWorldCenter(), true);
			}
			else if (LastFaceDirection == EFD_Left)
			{
				GetPhysicComponent()->GetBody()->ApplyLinearImpulse(b2Vec2(-ImpulseX, 0), GetPhysicComponent()->GetBody()->GetWorldCenter(), true);
			}

			// If Dash was execute on jumping, then you can no longer dash until you landed.
			if (bJump)
			{
				bCanDash = false;
			}
		}

		// Vertical Component
		bCanJump = JumpHoldTime < JUMP_HOLD_MAX_TIME;

		if (bWantToJump && bCanJump) // continue pumping it if want to jump.
		{
			if (!bDash)
			{
				bJump = true;
				const b2Vec2 CurrentVelocity = GetPhysicComponent()->GetBody()->GetLinearVelocity();
				const float JumpVelocity = JUMP_VELOCITY; 

				const b2Vec2 FinalVelocity = b2Vec2(CurrentVelocity.x, b2Max(CurrentVelocity.y - JumpVelocity, -JUMP_VELOCITY));
				GetPhysicComponent()->GetBody()->SetLinearVelocity(FinalVelocity);

				if (GetPhysicComponent()->GetBody()->GetLinearVelocity().y < 0)
				{
					const float Gravity = Application::GetInstance()->GetWorld()->GetGravity().y;
					const b2Vec2 FallingVelocity = GetPhysicComponent()->GetBody()->GetLinearVelocity() + b2Vec2(0, Gravity * 1.25f * DELTA_TIME_STEP);
					GetPhysicComponent()->GetBody()->SetLinearVelocity(FallingVelocity);
				}
			}
		}
		else if (!bCanJump)
		{
			if (!bDash && GetPhysicComponent()->GetBody()->GetLinearVelocity().y > 0)
			{
				const float Gravity = Application::GetInstance()->GetWorld()->GetGravity().y;
				const b2Vec2 FallingVelocity = GetPhysicComponent()->GetBody()->GetLinearVelocity() + b2Vec2(0, Gravity * 2 * DELTA_TIME_STEP);
				GetPhysicComponent()->GetBody()->SetLinearVelocity(FallingVelocity);
			}
		}

#if DEBUG_GAME
		// Debug information
		b2Vec2 v = GetPhysicComponent()->GetBody()->GetLinearVelocity();

		std::ostringstream ss;
		ss << std::setprecision(3);
		ss << std::fixed;
		ss << "Vx = " << v.x << "\nVy = " << v.y;
		ss << "\nWantJump=" << (bWantToJump ? "Yes" : "No");
		ss << "\nCanJump=" << (bCanJump ? "Yes" : "No");
		ss << "\nJumpState="<< (bJump ? "Jumping" : "NotJumping");
		ss << "\nJumpHoldTime=" << JumpHoldTime;
		ss << "\nWantGrab=" << (bWantToGrab ? "Yes" : "No");
		ss << "\nGrabState=" << (bGrab ? "Grabbed" : "NotGrabbed");
		ss << "\nWantDash=" << (bWantToDash ? "Yes" : "No");
		ss << "\nCanDash=" << (bCanDash ? "Yes" : "No");
		ss << "\nDashState=" << (bDash ? "Dashing" : "NotDashing");
		ss << "\nDashCoolDown=" << DashCoolDownTimer;
		ss << "\nFacingDirection=" << ((LastFaceDirection == EFaceDirection::EFD_Right) ? "Right" : "Left");

		DebugText->SetText(ss.str());
		DebugText->Text.setPosition(sf::Vector2f(32.0f, 32.0f*2));
#endif

		bWantToMoveLeft = false;
		bWantToMoveRight = false;

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
			pCharacter->bCanDash = true;
			pCharacter->bWantToJump = false;
			pCharacter->JumpInputTimedOutTimer = JUMP_BLOCK_INTERVAL;
			pCharacter->JumpHoldTime = 0.0f;
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
		pCharacter->OverlapDataText->Text.setPosition(sf::Vector2f(32.0f, 32.0f * 15));
	}
}

void Character::EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB)
{
	
}
