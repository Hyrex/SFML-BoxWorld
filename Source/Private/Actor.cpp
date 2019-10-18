#include "Actor.h"
#include "PhysicComponent.h"
#include "Defines.h"
#include "Application.h"

Actor::Actor(std::string Name, int ID)
{
	SetObjectName(Name);
	SetID(ID);
}

Actor::~Actor()
{
	for (auto i : ObjectShapes)
	{
		if (i.Shape != nullptr)
			delete i.Shape;
	}
}

void Actor::Tick()
{
	if (ObjectShapes.size() > 0 )
	{
		for (auto si : ObjectShapes)
		{
			//for each of the b2Component, update if component match visual id
			if (b2Component.b2ID == si.sID)
			{
				WorldRotation = b2Component.Component->GetBody()->GetAngle() * 180 / b2_pi;
				si.Shape->setRotation(WorldRotation);
				WorldLocation.x = UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().x);
				WorldLocation.y = UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().y);
				si.Shape->setPosition(WorldLocation);
			}
		}
	}

	b2Component.Component->Tick();
}

void Actor::Draw()
{
	for (int i = 0; i < GetShapeCount(); ++i)
	{
		if (sf::Shape* s = GetShapeAtIndex(i))
			Application::GetInstance()->GetWindow()->draw(*s);
	}
}

void Actor::SetObjectName(std::string Name)
{
	ObjectName = Name;
}

void Actor::SetID(int ID)
{
	this->ID = ID;
}

void Actor::SetWorldLocation(sf::Vector2f Location) 
{ 
	WorldLocation = Location; 
}

void Actor::SetWorldRotation(float Rotation) 
{ 
	WorldRotation = Rotation;
}

void Actor::SetWorldTransform(sf::Vector2f Location, float Rotation) 
{ 
	SetWorldLocation(Location); 
	SetWorldRotation(Rotation); 
}

void Actor::RegisterShape(FShapeID ShapeID)
{
	ObjectShapes.push_back(ShapeID);
}

void Actor::Registerb2Component(Fb2ComponentID ComponentID)
{
	b2Component = ComponentID;
}

int Actor::GetID() const
{
	return ID;
}

std::string Actor::GetObjectName() const
{
	return ObjectName;
}

sf::Shape* Actor::GetShapeAtIndex(const int Index)
{
	if (Index < ObjectShapes.size())
		return ObjectShapes[Index].Shape;

	std::cout << "GetShapeAtIndex() out of range!" << std::endl;
	return nullptr;
}

sf::Vector2f Actor::GetWorldLocation() const
{
	return WorldLocation;
}
