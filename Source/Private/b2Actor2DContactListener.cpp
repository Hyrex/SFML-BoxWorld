#include "b2Actor2DContactListener.h"
#include "b2Actor2D.h"
#include "Defines.h"

b2Actor2DContactListener::b2Actor2DContactListener()
{
}


b2Actor2DContactListener::~b2Actor2DContactListener()
{
}


void b2Actor2DContactListener::BeginContact(b2Contact* Contact)
{
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
		static_cast<b2Actor2D*>(BodyUserDataA)->BeginOverlap(static_cast<b2Actor2D*>(BodyUserDataA), static_cast<b2Actor2D*>(BodyUserDataB), BodyUserDataA, BodyUserDataB);

	if (BodyUserDataB)
		static_cast<b2Actor2D*>(BodyUserDataB)->BeginOverlap(static_cast<b2Actor2D*>(BodyUserDataB), static_cast<b2Actor2D*>(BodyUserDataA), BodyUserDataB, BodyUserDataA);
}

void b2Actor2DContactListener::EndContact(b2Contact* Contact)
{
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
		static_cast<b2Actor2D*>(BodyUserDataA)->EndOverlap(static_cast<b2Actor2D*>(BodyUserDataA), static_cast<b2Actor2D*>(BodyUserDataB), BodyUserDataA, BodyUserDataB);

	if (BodyUserDataB)
		static_cast<b2Actor2D*>(BodyUserDataB)->EndOverlap(static_cast<b2Actor2D*>(BodyUserDataB), static_cast<b2Actor2D*>(BodyUserDataA), BodyUserDataB, BodyUserDataA);
}
