#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include <vector>
#include "Interface.h"
#include "PhysicComponent.h"

enum EFaceDirection
{
	EFD_Left,
	EFD_Right
};

struct FShapeID
{
	sf::Shape* Shape = nullptr;
	int sID = 0;

	FShapeID() {}
	explicit FShapeID(sf::Shape* Shape, int ID)
	{ 
		this->Shape = Shape; 
		sID = ID; 
	};
};

struct Fb2ComponentID
{
	PhysicComponent* Component = nullptr;
	int b2ID = 0;

	Fb2ComponentID() {}
	explicit Fb2ComponentID(PhysicComponent* Component, int ID)
	{ 
		this->Component = Component; 
		b2ID = ID;
	}
};

class Actor : public ITickable, public IDrawable
{
public:
	Actor() {};
	Actor(std::string Name, int ID);
	virtual ~Actor();

	virtual void Tick() override;
	virtual void Draw() override;

	void SetObjectName(std::string Name);
	void SetID(int ID);
	void SetWorldLocation(sf::Vector2f Location);
	void SetWorldRotation(float Rotation);
	void SetWorldTransform(sf::Vector2f Location, float Rotation);

	void RegisterShape(FShapeID ShapeID);
	void Registerb2Component(Fb2ComponentID ComponentID);

	int GetID()	const;
	int GetShapeCount() const { return (int)ObjectShapes.size(); }
	Fb2ComponentID Getb2Component() { return b2Component; }
	PhysicComponent* GetPhysicComponent() { return b2Component.Component; }
	std::string	GetObjectName()	const;
	sf::Shape*	GetShapeAtIndex(const int Index);
	sf::Vector2f GetWorldLocation() const;
	b2Vec2 Getb2WorldLocation() const;

protected:
	std::string							ObjectName;
	std::vector<FShapeID>				ObjectShapes;
	Fb2ComponentID						b2Component; 
	sf::Vector2f						WorldLocation; //represent object world center, must b2Vec2 compatible.
	float								WorldRotation;

	EFaceDirection						LastFaceDirection = EFD_Right;
	int ID = 0;
};

