#include "Character.h"
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
	CharacterBody->setSize(UNIT_VECTOR);
	CharacterBody->setOrigin(UNIT_VECTOR * 0.5f);
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
	BodyShape.SetAsBox(UNIT_SFML_TO_BOX2D(UNIT_VECTOR.x * 0.5f), UNIT_SFML_TO_BOX2D(UNIT_VECTOR.y * 0.5f));

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
	FootRect->setSize(UNIT_VECTOR * 0.5f);
	FootRect->setOrigin(UNIT_VECTOR * 0.25f);
	FootRect->setFillColor(sf::Color::Red);
	FootRect->setOutlineColor(sf::Color::White);
	RegisterShape(FShapeID(FootRect, 2));

	// Box2D - Foot
	const float BoxSize = 16.0f;
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(UNIT_SFML_TO_BOX2D(BoxSize), UNIT_SFML_TO_BOX2D(BoxSize) , b2Vec2(0, UNIT_SFML_TO_BOX2D (BoxSize)), 0);

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

	// Update Foot Sensor
	ObjectShapes[1].Shape->setPosition(UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().x), UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().y) + 16.f);

	if(JumpInputTimedOutTimer >= -1.0f)
		JumpInputTimedOutTimer -= DELTA_TIME_STEP;

	if (bJump)
	{
		JumpMaxHoldTimer += DELTA_TIME_STEP;
		if (JumpMaxHoldTimer >= JUMP_MAX_HOLD_THRESHOLD_TIME)
			JumpMaxHoldTimer = JUMP_MAX_HOLD_THRESHOLD_TIME;
	}
		


#if DEBUG_GAME


#endif

	UpdateMovement();
}
void Character::MoveLeft()
{
	if (bInitialized)
	{
		bWantToMoveLeft = true;
	}
}

void Character::MoveRight()
{
	if (bInitialized)
	{
		bWantToMoveRight = true;
	}
}

void Character::GrabPressed()
{
	if (bInitialized)
	{
		bWantGrab = true;
	}
}

void Character::GrabRelease()
{
	if (bInitialized)
	{
		bWantGrab = false;
		bGrab = false;
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
			Vx = b2Max(Velocity.x - 5.0f, -10.0f);

		if (!bWantToMoveLeft && bWantToMoveRight) 
			Vx = b2Min(Velocity.x + 5.0f, +10.0f);

		if (bWantToJump)
			Vy = Velocity.y + 8.0f; //b2Min(Velocity.y + 16.0f, +16.0f);

		float DeltaVelocityX = Vx - Velocity.x;
		float DeltaVelocityY = Vy - Velocity.y;
		float ImpulseX = b2Component.Component->GetBody()->GetMass() * DeltaVelocityX;
		float ImpulseY = b2Component.Component->GetBody()->GetMass() * DeltaVelocityY;
		
		if(!bJump) // if not in jumping state, continue accelerate X-axis movement.
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
		ss << "\nWantGrab=" << (bWantGrab ? "Yes" : "No");
		ss << "\nGrabState=" << (bGrab ? "Grabbed" : "NotGrabbed");

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

		Character* p = static_cast<Character*>(Component->GetOwner());
		if ((int)UserDataA == GAMETAG_PLAYER_FOOT && (int)UserDataB == GAMETAG_STATIC_FLOOR)
		{
			if (!p) return;
			
			p->bJump = false;
			p->JumpInputTimedOutTimer = JUMP_BLOCK_INTERVAL;
			p->JumpMaxHoldTimer = 0.0f;
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
					if (!p) return;

					if (p->bWantGrab)
					{
						p->bJump = false;
						p->bWantGrab = false;
						
						/// TODO: 
						// Do something to the velocity to make it pin on the wall and make it stay that way until release
					}
				}

				Fixture = Fixture->GetNext();
				if (Fixture == nullptr)
					bFinishCheckList = true;
			}
		}

		p->OverlapDataText->SetText(ss.str());
		p->OverlapDataText->Text.setPosition(sf::Vector2f(32.0f, 32.0f * 8));
	}
}

void Character::EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB)
{
	
}