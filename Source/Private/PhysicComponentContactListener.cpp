#include "PhysicComponentContactListener.h"
#include "PhysicComponent.h"
#include "Defines.h"
#include "Actor.h"

PhysicComponentContactListener::PhysicComponentContactListener()
{
}


PhysicComponentContactListener::~PhysicComponentContactListener()
{
}


void PhysicComponentContactListener::BeginContact(b2Contact* Contact)
{
	void* FixtureDataA = Contact->GetFixtureA()->GetUserData();
	void* FixtureDataB = Contact->GetFixtureB()->GetUserData(); 
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	Actor* const pActorA = static_cast<Actor*>(BodyUserDataA);
	Actor* const pActorB = static_cast<Actor*>(BodyUserDataB);

	// Forwarding fixture body user data rather than main body user data.
	if(pActorA && pActorB && BodyUserDataA && BodyUserDataB)
	{
		pActorA->Getb2Component().Component->BeginOverlap(pActorA->Getb2Component().Component, pActorB->Getb2Component().Component, FixtureDataA, FixtureDataB);
		pActorB->Getb2Component().Component->BeginOverlap(pActorB->Getb2Component().Component, pActorA->Getb2Component().Component, FixtureDataB, FixtureDataA);
	}
}

void PhysicComponentContactListener::EndContact(b2Contact* Contact)
{
	void* FixtureDataA = Contact->GetFixtureA()->GetUserData();
	void* FixtureDataB = Contact->GetFixtureB()->GetUserData();
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	Actor* const pActorA = static_cast<Actor*>(BodyUserDataA);
	Actor* const pActorB = static_cast<Actor*>(BodyUserDataB);

	if (pActorA && pActorB && BodyUserDataA && BodyUserDataB)
	{
		pActorA->Getb2Component().Component->EndOverlap(pActorA->Getb2Component().Component, pActorB->Getb2Component().Component, FixtureDataA, FixtureDataB);
		pActorB->Getb2Component().Component->EndOverlap(pActorB->Getb2Component().Component, pActorA->Getb2Component().Component, FixtureDataB, FixtureDataA);
	}
}
