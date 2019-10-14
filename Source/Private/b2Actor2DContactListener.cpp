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
	{
		static_cast<b2Actor2D*>(BodyUserDataA)->BeginOverlap(static_cast<b2Actor2D*>(BodyUserDataB));
		if ((int)BodyUserDataA == GAMETAG_PLAYER_FOOT)
		{
			LOG_CMD("Touch ground");
		}
	}
		

	if (BodyUserDataB)
	{
		static_cast<b2Actor2D*>(BodyUserDataB)->BeginOverlap(static_cast<b2Actor2D*>(BodyUserDataA));
		if ((int)BodyUserDataB == GAMETAG_PLAYER_FOOT)
		{
			LOG_CMD("Touch ground");
		}
	}
	

}

void b2Actor2DContactListener::EndContact(b2Contact* Contact)
{
	void* BodyUserDataA = Contact->GetFixtureA()->GetBody()->GetUserData();
	void* BodyUserDataB = Contact->GetFixtureB()->GetBody()->GetUserData();

	if (BodyUserDataA)
	{
		static_cast<b2Actor2D*>(BodyUserDataA)->EndOverlap(static_cast<b2Actor2D*>(BodyUserDataB));
		if ((int)BodyUserDataA == GAMETAG_PLAYER_FOOT)
		{
			LOG_CMD("No ground");
		}
	}


	if (BodyUserDataB)
	{
		static_cast<b2Actor2D*>(BodyUserDataB)->EndOverlap(static_cast<b2Actor2D*>(BodyUserDataA));
		if ((int)BodyUserDataB == GAMETAG_PLAYER_FOOT)
		{
			LOG_CMD("No ground");
		}
	}
		

}
