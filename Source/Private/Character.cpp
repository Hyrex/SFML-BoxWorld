#include "Character.h"
#include "Application.h"
#include "Defines.h"
#include "TextManager.h"

Character::Character(std::string Name, int ID)
{
	SetObjectName(Name);
	SetID(ID);

	FTextManager::Register(FTextEffectBundle(DebugJumpFlagText.get()));
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
	CharacterBodyDef.position.Set(UNIT_SFML_TO_BOX2D(ViewPort.getSize().x * 0.5f), UNIT_SFML_TO_BOX2D(ViewPort.getSize().y * 0.5f));
	Component->CreateBody(&CharacterBodyDef);
	Component->GetBody()->SetUserData((void*)this); // Main UserData on BodyInstance

	b2PolygonShape BodyShape;
	BodyShape.SetAsBox(UNIT_SFML_TO_BOX2D(UNIT_VECTOR.x * 0.5f), UNIT_SFML_TO_BOX2D(UNIT_VECTOR.y * 0.5f));

	b2FixtureDef CharacterFixtureDef;
	CharacterFixtureDef.shape = &BodyShape;
	CharacterFixtureDef.restitution = 0.1f;
	CharacterFixtureDef.friction = 0.3f;
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

	if(JumpTimedOutTimer >= -1.0f)
		JumpTimedOutTimer -= DELTA_TIME_STEP;

#if DEBUG_GAME
	DebugJumpFlagText->SetText(bJump ? "Jump=Yes" : "Jump=No");
	DebugJumpFlagText->Text.setPosition(GetWorldLocation() + sf::Vector2f(16.0f, -32.0f));
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

void Character::Jump()
{
	if (bInitialized && !bJump && JumpTimedOutTimer <= 0.0f)
	{
		bJump = true;
		LOG_CMD("Jump pressed");
		
		b2Vec2 Velocity = b2Component.Component->GetBody()->GetLinearVelocity();
		float TargetVelocity = 8.0f;
		
		float VelocityChange = TargetVelocity - Velocity.y;
		float Impulse = b2Component.Component->GetBody()->GetMass() * VelocityChange;
		b2Component.Component->GetBody()->ApplyLinearImpulse(b2Vec2(0, -Impulse), b2Component.Component->GetBody()->GetWorldCenter(), true);
	}
}

void Character::UpdateMovement()
{
	if (bInitialized)
	{
		b2Vec2 Velocity = b2Component.Component->GetBody()->GetLinearVelocity();
		float TargetVelocityX = Velocity.x * 0.75f; // decrease per update
		Velocity.y *= (Velocity.y >= 0) ? 0.25f : 4.0f; // decrease per update
		//float TargetVelocityY = Velocity.y * 0.85f; // decrease per update

		if (bWantToMoveLeft && !bWantToMoveRight) TargetVelocityX = b2Max(Velocity.x - 4.0f, -16.0f);
		if (!bWantToMoveLeft && bWantToMoveRight) TargetVelocityX = b2Min(Velocity.x + 4.0f, +16.0f);
		//if (bJump)
		//{
		//	TargetVelocityY = 16.0f;
		//}

		bWantToMoveLeft = false;
		bWantToMoveRight = false;
		
		//float DeltaVelocityY = TargetVelocityY - Velocity.y;
		float DeltaVelocityX = TargetVelocityX - Velocity.x;
		float ImpulseX = b2Component.Component->GetBody()->GetMass() * DeltaVelocityX;
		//float ImpulseY = b2Component.Component->GetBody()->GetMass() * DeltaVelocityY;
		b2Component.Component->GetBody()->ApplyLinearImpulse(b2Vec2(ImpulseX, 0), b2Component.Component->GetBody()->GetWorldCenter(), true);
	}
}

void Character::BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB)
{
	if (OverlapComponent && OverlapComponent->GetOwner())
	{
		if ((int)UserDataA == GAMETAG_PLAYER_FOOT && (int)UserDataB == GAMETAG_STATIC_OBJECT)
		{
			if (Character* p = static_cast<Character*>(Component->GetOwner()))
			{
				p->bJump = false;
				p->JumpTimedOutTimer = JUMP_BLOCK_INTERVAL;
				LOG_CMD("Jump unlocked");
			}
		} 
	}
}

void Character::EndOverlap(PhysicComponent* Component, PhysicComponent* OverlapComponent, void* UserDataA, void* UserDataB)
{
	
}