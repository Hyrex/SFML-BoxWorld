#pragma once
#include <SFML\Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Defines.h"
#include "Interface.h"

class Actor;

class PhysicComponent : public ITickable
{
public:
	explicit PhysicComponent(std::string ComponentName);

	virtual void Tick() override;

	void(*OnBeginOverlapCallback)(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB) = 0;
	void(*OnEndOverlapCallback)(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB) = 0;
	void(*TickCallback)(PhysicComponent* Component) = 0;

	void SetOwningParent(Actor* Parent);
	void SetGenerateOverlap(const bool bNewGenerate);
	void SetPhysicEnabled(bool bEnabled);
	void BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB);
	void EndOverlap(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB);
	void BindOnBeginoverlap(void(*Callback)(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB));
	void BindOnEndOverlap(void(*Callback)(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB));
	void BindOnTick(void(*TickFunction)(PhysicComponent* Component));
	
	// Explanation on how Box2D component construction
	// http://www.iforce2d.net/b2dtut/bodies
	// 1. Use BodyDef (Reusable) -> Create Body
	// 2. Polygon -> Make Fixtures // externallize.
	// 3. Body -> Create Fixtures (Can add multiple)
	void CreateBody(b2BodyDef* BodyDef);
	void CreateFixture(b2FixtureDef* FixtureDef); 
	
	Actor*				GetOwner();
	std::string			GetComponentName() const;
	sf::RectangleShape* GetDebugForward();
	b2Body*				GetBody();

private:
	std::string ComponentName;
	Actor* Parent;

	b2Body* Body;

	std::unique_ptr<sf::RectangleShape> DebugForward = std::make_unique<sf::RectangleShape>();
	bool bGenerateOverlaps;
};
