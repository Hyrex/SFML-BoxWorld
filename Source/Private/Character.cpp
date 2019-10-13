#include "Character.h"
#include "Application.h"

Character::Character()
{
	
}

Character::~Character()
{
}

void Character::Spawn(Application* App)
{
	if (bSpawned)
		return;

	// Construct data to parse.
	Fb2ActorSpawnParam SpawnParam;
	SpawnParam.Package = App;
	SpawnParam.WorldContext = App->GetWorld();
	SpawnParam.Name = "Box";
	SpawnParam.ShapeType = EActorShapeType::EST_Rectangle;
	SpawnParam.BodyType = Eb2ShapeType::ECT_Polygon;
	SpawnParam.Size = SFML::Vector2f(32, 32);
	
	SFML::View ViewPort;
	App->GetWindow()->getViewport(ViewPort);
	
	SpawnParam.Location = ViewPort.getSize() / 2.0f;
	SpawnParam.Rotation = 0.0f;
	SpawnParam.bIsDynamicBody = true;
	SpawnParam.bGenerateOverlaps = false;
	SpawnParam.bAutoActivate = true;

	b2Actor = std::move(std::make_unique<b2Actor2D>(SpawnParam));
	b2Actor->GetShape()->setOutlineColor(SFML::Color::White);
	b2Actor->GetShape()->setFillColor(SFML::Color::Black);

	std::cout << "X: " << b2Actor->GetBodyInstance()->GetLinearVelocity().x << " Y: " << b2Actor->GetBodyInstance()->GetLinearVelocity().y << std::endl;
	b2Actor->GetFixtureDefinition()->density = 0.83f;
	b2Actor->GetFixtureDefinition()->friction = 0.4f;
	b2Actor->GetFixtureDefinition()->restitution = 0.25f;

	b2MassData data;
	data.mass = 1; //kg
	data.center = b2Vec2(0, 0);// no offset.
	data.I = 0.0f;


	b2Actor->GetBodyInstance()->SetMassData(&data);
	//b2Actor->BindOnTick(BallTick);
	bSpawned = true;
}

void Character::Tick()
{
	if(b2Actor)
		b2Actor->Tick();
}

