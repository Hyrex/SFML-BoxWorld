#pragma once
#include <SFML\Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Defines.h"
#include "Interface.h"

class Actor;

static const float PIXEL_PER_METER = 32.0f;

enum EActorShapeType
{
	EST_Circle,
	EST_Rectangle,
	EST_Convex,
};

enum Eb2ShapeType
{
	ECT_Polygon,
	ECT_Circle,
	ECT_Edge,
	ECT_Chain,
};

struct FShapeCollection
{
	std::unique_ptr<sf::CircleShape> CircleShape;
	std::unique_ptr<sf::RectangleShape> RectangleShape;
	std::unique_ptr<sf::ConvexShape> ConvexShape;
	EActorShapeType ShapeType;

	sf::Shape* Get()
	{
		switch (ShapeType)
		{
			case EActorShapeType::EST_Rectangle:	return RectangleShape.get();
			case EActorShapeType::EST_Circle:		return CircleShape.get();
			case EActorShapeType::EST_Convex:		return ConvexShape.get();	
		}
		return nullptr;
	}
};

struct Fb2ActorSpawnParam
{
	std::string Name;
	EActorShapeType ShapeType;
	Eb2ShapeType BodyType;
	sf::Vector2f Size; 
	sf::Vector2f Location;
	float Rotation; 
	bool bIsDynamicBody; 
	bool bIsSensor;
	bool bGenerateOverlaps;
	bool bAutoActivate;
};

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

class b2Actor2D : public ITickable
{
public:

	b2Actor2D(const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size = sf::Vector2f(1,1), sf::Vector2f Location = sf::Vector2f(0,0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bIsSensor = false, const bool bGenerateOverlap = false, const bool bAutoActivate = true);
	b2Actor2D(const Fb2ActorSpawnParam SpawnParam);
	~b2Actor2D();

	virtual void Tick() override; /// DONE

	void SetInitTransform(sf::Vector2f Location, float Rotation)	{ SetInitLocation(Location); SetInitRotation(Rotation); } /// DONE
	void SetInitLocation(b2Vec2 Location)							{ InitialPosition = Location; }/// DONE
	void SetInitLocation(sf::Vector2f Location)						{ InitialPosition = b2Actor2D::Tob2Vec2Location(Location); }/// DONE
	void SetInitRotation(float Rotation)							{ InitialRotation = Rotation; }/// DONE
	void ResetToInitTransform();

	void SetGenerateOverlap(const bool bNewGenerate)				{ bGenerateOverlaps = bNewGenerate; } /// DONE
	void BeginOverlap(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData);/// DONE
	void EndOverlap(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData);/// DONE
	void BindOnBeginoverlap(void (*Callback)(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData));/// DONE
	void BindOnEndOverlap(void (*Callback)(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* UserData, void* OtherUserData));/// DONE
	void BindOnTick(void(*TickFunction)(b2Actor2D* Actor));/// DONE


	/** Utility*/
	static b2Vec2 Tob2Vec2Location(sf::Vector2f Location) { return b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER); }; /// VERIFY

	sf::Shape*		GetShape() { return ObjectShapes.Get(); } /// DONE separate on Actor Shape
	b2FixtureDef*	GetFixtureDef() { return FixtureDef.get(); } /// DONE Complete externalize
	b2Fixture*		GetFixture() { return Fixture; } /// NOT needed
	b2Body*			GetBody() { return Body; } /// DONE
	b2BodyDef*		GetBodyDef() { return BodyDef.get(); } /// NOT NEEDED
	bool			IsActive()								const { return bIsActive; }

	std::unique_ptr<sf::RectangleShape> DebugForward = std::make_unique<sf::RectangleShape>();

private:

	void Construct(const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, sf::Vector2f Size = sf::Vector2f(1, 1), sf::Vector2f Location = sf::Vector2f(0, 0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bIsSensor = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	void MakeShapeInstance(const EActorShapeType ShapeType);
	void SetShapeProperties(const EActorShapeType ShapeType, sf::Vector2f Size);
	void MakeB2ShapeInstance(const Eb2ShapeType BodyType);
	void SetB2ShapeProperties(const Eb2ShapeType BodyType, sf::Vector2f Size);

	void(*OnBeginOverlapCallback)(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* ActorUserData, void* OtherActorUserData) = 0;
	void(*OnEndOverlapCallback)(b2Actor2D* Actor, b2Actor2D* OverlappedActor, void* ActorUserData, void* OtherActorUserData) = 0;
	void(*TickCallback)(b2Actor2D* Actor) = 0;

	/** Internally contains unique ptr to assigned shape. (Try std::variant)*/
	FShapeCollection ObjectShapes;	// Act like display component

	/** Box2D Components*/
	b2Body* Body;
	std::unique_ptr<b2BodyDef>		BodyDef;	
	std::unique_ptr<b2Shape>		BodyShape;				// Act as collision component

	b2Fixture* Fixture;
	std::unique_ptr<b2FixtureDef>	FixtureDef;				
	Eb2ShapeType CollisionType;

	/////////////////////////////////
	//		Cache
	/////////////////////////////////

	/** Reference only, doesn't own object.*/
	sf::Shape* ObjectShapeCache;

	b2Vec2 InitialPosition;
	float InitialRotation;

	/////////////////////////////////
	//		Flags
	/////////////////////////////////

	bool bIsActive = false;
	bool bGenerateOverlaps = false;
	bool bIsDynamicObject = false;
};