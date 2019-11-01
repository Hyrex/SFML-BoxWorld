#include "StaticBlockActor.h"
#include "Defines.h"

StaticBlockActor::StaticBlockActor(std::string Name, int ID)
{
	SetObjectName(Name);
	SetID(ID);
}

StaticBlockActor::~StaticBlockActor()
{

}

void StaticBlockActor::Construct(sf::Vector2f Size, sf::Vector2f Location, const float Rotation)
{
	WorldLocation = Location;
	WorldRotation = Rotation;

	sf::RectangleShape* Rect = new sf::RectangleShape();
	Rect->setSize(Size);
	Rect->setOrigin(Size * 0.5f);
	Rect->setFillColor(sf::Color(125,125,125,255));
	Rect->setOutlineThickness(1);
	Rect->setOutlineColor(sf::Color::Black);

	//variable make with t prefix need to make parser later. Make it less copy paste
	PhysicComponent* b2Component = new PhysicComponent(GetObjectName()+"-PhysicComponent");
	b2Component->SetOwningParent(this);

	b2BodyDef tBodyDef;
	tBodyDef.type = b2_staticBody;
	tBodyDef.position.Set(UNIT_BOX2D(Location.x), UNIT_BOX2D(Location.y));
	tBodyDef.userData = (void*)this;
	b2Component->CreateBody(&tBodyDef);

	b2PolygonShape tFixtureShape;
	tFixtureShape.SetAsBox(UNIT_BOX2D(Size.x * 0.5f), UNIT_BOX2D(Size.y * 0.5f));

	b2FixtureDef tFixtureDef;
	tFixtureDef.shape = &tFixtureShape;
	tFixtureDef.restitution = 0.0f;
	tFixtureDef.friction = 0.7f;
	tFixtureDef.density = 60.0f;
	tFixtureDef.isSensor = false;
	tFixtureDef.userData = (void*)ID;

	b2Component->CreateFixture(&tFixtureDef);
	b2Component->SetGenerateOverlap(true);

	RegisterShape(FShapeID(Rect, 1));
	Registerb2Component(Fb2ComponentID(b2Component, 1));
}

