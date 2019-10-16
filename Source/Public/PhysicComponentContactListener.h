#pragma once
#include <Box2D/Box2D.h>

class PhysicComponentContactListener : public b2ContactListener
{
public:
	PhysicComponentContactListener();
	~PhysicComponentContactListener();

	virtual void BeginContact(b2Contact* Contact);
	virtual void EndContact(b2Contact* Contact);
};

