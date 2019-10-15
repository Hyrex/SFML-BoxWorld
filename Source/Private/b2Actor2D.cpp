#include "b2Actor2D.h"
#include "Application.h"
#include "AssetLoader.h"

b2Actor2D::b2Actor2D(const std::string Name, EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size, sf::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bIsSensor  ,const bool bGenerateOverlaps, const bool bAutoActivate)
{
	Construct(Name, ShapeType, BodyType, Size, Location, Rotation, bIsDynamicBody, bGenerateOverlaps, bAutoActivate);
}

b2Actor2D::b2Actor2D(const Fb2ActorSpawnParam SpawnParam)
{	
	Construct(SpawnParam.Name, SpawnParam.ShapeType, SpawnParam.BodyType, 
		SpawnParam.Size, SpawnParam.Location, SpawnParam.Rotation, SpawnParam.bIsDynamicBody, SpawnParam.bIsSensor, SpawnParam.bGenerateOverlaps, SpawnParam.bAutoActivate);
}

b2Actor2D::~b2Actor2D() 
{
}

void b2Actor2D::Construct(const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size, sf::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bIsSensor , const bool bGenerateOverlaps, const bool bAutoActivate)
{
	ObjectName = Name;
	ObjectShapes.ShapeType = ShapeType;
	CollisionType = BodyType;

	MakeShapeInstance(ShapeType);
	ObjectShapeCache = ObjectShapes.Get();
	SetShapeProperties(ShapeType, Size);
	if (ObjectShapes.Get())
	{
		ObjectShapes.Get()->setOrigin(Size * 0.5f);
		ObjectShapes.Get()->setFillColor(sf::Color(128, 128, 128, 255));
		ObjectShapes.Get()->setOutlineThickness(1);
		ObjectShapes.Get()->setOutlineColor(sf::Color::Black);
	}

	BodyDef = std::make_unique<b2BodyDef>();
	BodyDef->position = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	BodyDef->type = bIsDynamicBody ? b2_dynamicBody : b2_staticBody;

	InitialPosition = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	InitialRotation = Rotation;

	MakeB2ShapeInstance(BodyType);
	SetB2ShapeProperties(BodyType, Size);

	if (BodyShape)
	{
		FixtureDef = std::make_unique<b2FixtureDef>();
		FixtureDef->shape = BodyShape.get();
		FixtureDef->density = 0.5f;
		FixtureDef->friction = 0.5f;
		FixtureDef->restitution= 0.5f;
		FixtureDef->isSensor = bIsSensor; // change it to MakeSensor and get data.
	}

	Body = Application::GetInstance()->GetWorld()->CreateBody(BodyDef.get());
	Fixture = Body->CreateFixture(FixtureDef.get());
	Body->SetUserData(this);

	this->bGenerateOverlaps = bGenerateOverlaps;
	bIsDynamicObject = bIsDynamicBody;

	if (bAutoActivate)
	{
		Activate();
	}

	// Debug!
	const sf::Vector2f DebugArrowSize = DEBUG_ARROW_SIZE * 0.25f;
	DebugForward = std::make_unique<sf::RectangleShape>();
	DebugForward->setTexture(FAssetLoader::GetInstance()->GetTexture(RESOURCES_TEXTURE_DEBUG_ARROW));
	DebugForward->setOrigin(sf::Vector2f(0, +DebugArrowSize.y/2));
	DebugForward->setSize(DebugArrowSize);
	DebugForward->setFillColor(sf::Color(255, 0, 0, 255));

}

void b2Actor2D::Tick()
{
	if (bIsActive && Body)
	{
		// Box2D uses radians for rotation, SFML uses degree
		// Snap rendering according to Box2D BodyInstance.
		ObjectShapes.Get()->setRotation(Body->GetAngle() * 180 / b2_pi);
		ObjectShapes.Get()->setPosition(Body->GetPosition().x*PIXEL_PER_METER, Body->GetPosition().y*PIXEL_PER_METER);
		DebugForward->setRotation(Body->GetAngle() * 180 / b2_pi);
		DebugForward->setPosition(Body->GetPosition().x*PIXEL_PER_METER, Body->GetPosition().y*PIXEL_PER_METER);

		if (TickCallback != 0)
		{
			TickCallback(this);
		}
	}
}


void b2Actor2D::ResetToInitTransform()
{
	if (Body)
	{
		Body->SetTransform(InitialPosition, InitialRotation);
	}
	else
	{
		LOG("No BodyInstance isn't created.");
		LOG(" ");
	}
}

void b2Actor2D::BeginOverlap(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData)
{
	if (bGenerateOverlaps && OverlappedActor && Actor)
	{
		if (OnBeginOverlapCallback)
		{
			OnBeginOverlapCallback(Actor, OverlappedActor, UserData, OtherUserData);
		}
	}
}

void b2Actor2D::EndOverlap(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData)
{
	if (bGenerateOverlaps && OverlappedActor && Actor)
	{
		if (OnEndOverlapCallback)
		{
			OnEndOverlapCallback(Actor, OverlappedActor, UserData, OtherUserData);
		}
	}
}

void b2Actor2D::BindOnBeginoverlap(void (*Callback)(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData))
{
	OnBeginOverlapCallback = Callback;
}

void b2Actor2D::BindOnEndOverlap(void (*Callback)(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData))
{
	OnEndOverlapCallback = Callback;
}

void b2Actor2D::BindOnTick(void(*TickFunction)(b2Actor2D* Actor))
{
	TickCallback = TickFunction;
}

void b2Actor2D::MakeShapeInstance(const EActorShapeType ShapeType)
{
	switch (ShapeType)
	{
		case EActorShapeType::EST_Rectangle:	ObjectShapes.RectangleShape =	std::make_unique<sf::RectangleShape>(); 		break;
		case EActorShapeType::EST_Circle:		ObjectShapes.CircleShape	=	std::make_unique<sf::CircleShape>();			break;
		case EActorShapeType::EST_Convex:		ObjectShapes.ConvexShape	=	std::make_unique<sf::ConvexShape>();			break;
	}

	// Prevent spawn at 0,0,0 at being visible before the first tick update.
	ObjectShapes.Get()->setPosition(sf::Vector2f(-200, -200));
}

void b2Actor2D::SetShapeProperties(const EActorShapeType ShapeType, sf::Vector2f Size)
{
	if (!ObjectShapeCache) ObjectShapeCache = ObjectShapes.Get();
	switch (ShapeType)
	{
		case EActorShapeType::EST_Circle:
			if (sf::CircleShape* const p = dynamic_cast<sf::CircleShape*>(ObjectShapeCache))
			{
				p->setRadius(Size.x/2);
			}
			break;
		case EActorShapeType::EST_Rectangle:
			if (sf::RectangleShape* const p = dynamic_cast<sf::RectangleShape*>(ObjectShapeCache))
			{
				p->setSize(Size);
			}
			break;
	}
}

void b2Actor2D::MakeB2ShapeInstance(const Eb2ShapeType BodyType)
{
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		BodyShape = std::make_unique<b2ChainShape>();	break;
		case Eb2ShapeType::ECT_Edge:		BodyShape = std::make_unique<b2EdgeShape>();	break;
		case Eb2ShapeType::ECT_Polygon:		BodyShape = std::make_unique<b2PolygonShape>(); break;
		case Eb2ShapeType::ECT_Circle:		BodyShape = std::make_unique<b2CircleShape>();	break;
	}
}

void b2Actor2D::SetB2ShapeProperties(const Eb2ShapeType BodyType, sf::Vector2f Size)
{
	if (!BodyShape) return;
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		
		case Eb2ShapeType::ECT_Edge:		
			//Not supported yet.
			break;
		case Eb2ShapeType::ECT_Polygon:	
		{
			if(b2PolygonShape* const p = dynamic_cast<b2PolygonShape*>(BodyShape.get()))
			{
				p->SetAsBox((Size.x * 0.5f ) / PIXEL_PER_METER, (Size.y * 0.5f) / PIXEL_PER_METER);
			}
			
			break;
		}
			
		case Eb2ShapeType::ECT_Circle:		
		{
			if (b2CircleShape* const p = dynamic_cast<b2CircleShape*>(BodyShape.get()))
			{
				p->m_radius = (Size.x * 0.5f) / PIXEL_PER_METER;
			}

			break;
		}
	}
}

void b2Actor2D::Activate()
{
	bIsActive = true; 
	Body->SetActive(true);
	Body->SetAwake(true);
}

void b2Actor2D::MakeInactive()
{
	bIsActive = false;
	
	Body->SetActive(false);
	Body->SetAwake(false);
}