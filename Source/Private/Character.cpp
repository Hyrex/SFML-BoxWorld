#include "Character.h"
#include "Application.h"

Character::Character()
{

}

Character::~Character()
{
}

void Character::Initialize()
{
	if (bInitialized)
		return;

	// Construct data to parse.
	Fb2ActorSpawnParam SpawnParam;
	SpawnParam.Name = "Box";
	SpawnParam.ShapeType = EActorShapeType::EST_Rectangle;
	SpawnParam.BodyType = Eb2ShapeType::ECT_Polygon;
	SpawnParam.Size = sf::Vector2f(32, 32);

	sf::View ViewPort;
	Application::GetInstance()->GetWindow()->getViewport(ViewPort);

	SpawnParam.Location = ViewPort.getSize() / 2.0f;
	SpawnParam.Rotation = 0.0f;
	SpawnParam.bIsDynamicBody = true;
	SpawnParam.bGenerateOverlaps = false;
	SpawnParam.bAutoActivate = true;

	b2Actor = std::move(std::make_unique<b2Actor2D>(SpawnParam));
	b2Actor->GetShape()->setOutlineColor(sf::Color::White);
	b2Actor->GetShape()->setFillColor(sf::Color::Black);

	std::cout << "X: " << b2Actor->GetBody()->GetLinearVelocity().x << " Y: " << b2Actor->GetBody()->GetLinearVelocity().y << std::endl;
	b2Actor->GetFixtureDefinition()->density = 0.83f;
	b2Actor->GetFixtureDefinition()->friction = 0.4f;
	b2Actor->GetFixtureDefinition()->restitution = 0.25f;

	b2Actor->GetBodyDef()->fixedRotation = true;

	b2MassData data;
	data.mass = 1; //kg
	data.center = b2Vec2(0, 0);// no offset.
	data.I = 0.0f;

	b2Actor->GetBody()->SetMassData(&data);
	//b2Actor->BindOnTick(BallTick);

	/// Add foot body fixture.
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(0.3 / 32.0f, 0.3 / 32.0f, b2Vec2(0, -1), 0);
	
	b2FixtureDef FootFixtureDef;
	FootFixtureDef.isSensor = true;
	FootFixtureDef.shape = &polygonShape;
	FootFixtureDef.density = 1;
	
	b2Fixture* FootSensorFixture = b2Actor->GetBody()->CreateFixture(&FootFixtureDef);
	FootSensorFixture->SetUserData((void*)GAMETAG_PLAYER_FOOT);

	bInitialized = true;
}

void Character::Tick()
{
	if (b2Actor)
		b2Actor->Tick();

	if (bJump)
	{
		b2Vec2 Velocity = b2Actor->GetBody()->GetLinearVelocity();
		if (Velocity.y <= 0)
			bJump = false;
	}

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
	if (bInitialized && !bJump)
	{
		bJump = true;
		b2Vec2 Velocity = b2Actor->GetBody()->GetLinearVelocity();
		float TargetVelocity = 10;

		float VelocityChange = TargetVelocity - Velocity.y;
		float Impulse = b2Actor->GetBody()->GetMass() * VelocityChange;
		b2Actor->GetBody()->ApplyLinearImpulse(b2Vec2(0, Impulse), b2Actor->GetBody()->GetWorldCenter(), true);
	}
}

void Character::UpdateMovement()
{
	if (bInitialized)
	{
		float TargetVelocity = 0.0f;

		if (bWantToMoveLeft && !bWantToMoveRight) TargetVelocity = -10.0f;
		if (!bWantToMoveLeft && bWantToMoveRight) TargetVelocity = 10.0f;

		bWantToMoveLeft = false;
		bWantToMoveRight = false;
		b2Vec2 Velocity = b2Actor->GetBody()->GetLinearVelocity();
		float VelocityChange = TargetVelocity - Velocity.x;
		float Impulse = b2Actor->GetBody()->GetMass() * VelocityChange;
		b2Actor->GetBody()->ApplyLinearImpulse(b2Vec2(Impulse, 0), b2Actor->GetBody()->GetWorldCenter(), true);
	}
}
