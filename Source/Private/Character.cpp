#include "Character.h"
#include "Application.h"
#include "Defines.h"

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
	SpawnParam.bIsSensor = false;
	SpawnParam.bGenerateOverlaps = true; /// FIXME ISSUE # 1 true to generate overlap as sensor but fall thru the world, true stay in the world but no event called.
	SpawnParam.bAutoActivate = true;

	b2Actor = std::move(std::make_unique<b2Actor2D>(SpawnParam));
	b2Actor->GetShape()->setOutlineColor(sf::Color::White);
	b2Actor->GetShape()->setFillColor(sf::Color::Transparent);

	b2Actor->GetFixtureDef()->density = 0.83f;
	b2Actor->GetFixtureDef()->friction = 0.4f;
	b2Actor->GetFixtureDef()->restitution = 0.25f;
	b2Actor->GetFixtureDef()->isSensor = false;

	b2Actor->GetBodyDef()->fixedRotation = true;

	b2MassData data;
	data.mass = 1; //kg
	data.center = b2Vec2(0, 0);// no offset.
	data.I = 0.0f;

	b2Actor->GetBody()->SetMassData(&data);
	b2Actor->BindOnBeginoverlap(BeginOverlap);
	b2Actor->BindOnEndOverlap(EndOverlap);

	// Foot Fixture
	const float BoxSize = 16.0f;
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(BoxSize / 32.0f, BoxSize / 32.0f, b2Vec2(0, + 16.f / 32.0f), 0);
	
	b2FixtureDef FootFixtureDef;
	FootFixtureDef.isSensor = true;
	FootFixtureDef.shape = &polygonShape;
	FootFixtureDef.density = 1;
	
	b2Fixture* FootSensorFixture = b2Actor->GetBody()->CreateFixture(&FootFixtureDef);
	FootSensorFixture->SetUserData((void*)GAMETAG_PLAYER_FOOT);

	// visualize this fixture.
	FootRect.setSize(sf::Vector2f(BoxSize, BoxSize));
	FootRect.setFillColor(sf::Color::Red);
	FootRect.setOutlineColor(sf::Color::White);
	FootRect.setOrigin(BoxSize / 2, BoxSize / 2);

	bInitialized = true;
}

void Character::Tick()
{
	if (b2Actor)
		b2Actor->Tick();

	FootRect.setPosition(b2Actor->GetBody()->GetPosition().x * 32.0f, (b2Actor->GetBody()->GetPosition().y * 32.0f) + 16.f);

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

void Character::BeginOverlap(b2Actor2D* Actor, b2Actor2D* OtherActor, void* UserData, void* OtherUserData)
{
	if (OtherActor && OtherActor->GetFixture())
	{
		/// FIXME ISSUE #2 CRASH. UserData isn't properly set?
		//if ((int)OtherActor->GetFixture()->GetUserData() == GAMETAG_STATIC_OBJECT)
		{
		//	if (Character* p = reinterpret_cast<Character*>(Actor))
		//	{
			//	p->bJump = false;
		//	}
		}
	}
	

	LOG_CMD("Character Begin Overlap");
}


void Character::EndOverlap(b2Actor2D* Actor, b2Actor2D* OtherActor, void* UserData, void* OtherUserData)
{
	
	LOG_CMD("Character End Overlap");
}