#include "PhysicComponentContactListener.h"
#include "PhysicComponent.h"
#include "Defines.h"

PhysicComponentContactListener::PhysicComponentContactListener()
{
}


PhysicComponentContactListener::~PhysicComponentContactListener()
{
}


void PhysicComponentContactListener::BeginContact(b2Contact* Contact)
{
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
		static_cast<PhysicComponent*>(BodyUserDataA)->BeginOverlap(static_cast<PhysicComponent*>(BodyUserDataA), static_cast<PhysicComponent*>(BodyUserDataB), BodyUserDataA, BodyUserDataB);

	if (BodyUserDataB)
		static_cast<PhysicComponent*>(BodyUserDataB)->BeginOverlap(static_cast<PhysicComponent*>(BodyUserDataB), static_cast<PhysicComponent*>(BodyUserDataA), BodyUserDataB, BodyUserDataA);

	LOG_CMD("BeginContact Fired");
}

void PhysicComponentContactListener::EndContact(b2Contact* Contact)
{
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
		static_cast<PhysicComponent*>(BodyUserDataA)->EndOverlap(static_cast<PhysicComponent*>(BodyUserDataA), static_cast<PhysicComponent*>(BodyUserDataB), BodyUserDataA, BodyUserDataB);

	if (BodyUserDataB)
		static_cast<PhysicComponent*>(BodyUserDataB)->EndOverlap(static_cast<PhysicComponent*>(BodyUserDataB), static_cast<PhysicComponent*>(BodyUserDataA), BodyUserDataB, BodyUserDataA);
	
	LOG_CMD("EndContact Fired");
}
