#include "Actor.h"
#include "b2Actor2D.h"
#include "Defines.h"

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
				si.Shape->setRotation(b2Component.Component->GetBody()->GetAngle() * 180 / b2_pi);
				const float LocationX = UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().x);
				const float LocationY = UNIT_BOX2D_TO_SFML(b2Component.Component->GetBody()->GetPosition().y);
				si.Shape->setPosition(LocationX, LocationY);
			}
		}
	}

	b2Component.Component->Tick();
}

void Actor::SetObjectName(std::string Name)
{
	ObjectName = Name;
}

void Actor::SetID(int ID)
{
	this->ID = ID;
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
