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
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
	{
		if (Actor* const pActorA = static_cast<Actor*>(BodyUserDataA))
		{
			if (Actor* const pActorB = static_cast<Actor*>(BodyUserDataB))
			{
				pActorA->Getb2Component().Component->BeginOverlap(pActorA->Getb2Component().Component, pActorB->Getb2Component().Component, BodyUserDataA, BodyUserDataB);
			}

		}
	}

	if (BodyUserDataB)
	{
		if (Actor* const pActorA = static_cast<Actor*>(BodyUserDataA))
		{
			if (Actor* const pActorB = static_cast<Actor*>(BodyUserDataB))
			{
				pActorB->Getb2Component().Component->BeginOverlap(pActorB->Getb2Component().Component, pActorA->Getb2Component().Component, BodyUserDataB, BodyUserDataA);
			}

		}
	}
}

void PhysicComponentContactListener::EndContact(b2Contact* Contact)
{
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
	{
		if (Actor* const pActorA = static_cast<Actor*>(BodyUserDataA))
		{
			if (Actor* const pActorB = static_cast<Actor*>(BodyUserDataB))
			{
				pActorA->Getb2Component().Component->EndOverlap(pActorA->Getb2Component().Component, pActorB->Getb2Component().Component, BodyUserDataA, BodyUserDataB);
			}

		}
	}

	if (BodyUserDataB)
	{
		if (Actor* const pActorA = static_cast<Actor*>(BodyUserDataA))
		{
			if (Actor* const pActorB = static_cast<Actor*>(BodyUserDataB))
			{
				pActorB->Getb2Component().Component->EndOverlap(pActorB->Getb2Component().Component, pActorA->Getb2Component().Component, BodyUserDataB, BodyUserDataA);
			}

		}
	}
}
