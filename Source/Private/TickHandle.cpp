#include <Box2D/Box2D.h>
#include "TickHandle.h"
#include "Application.h"


FTickHandle::FTickHandle()
{

}

FTickHandle::~FTickHandle()
{
	LOG("Destructing TickHandle.\n");
}

void FTickHandle::BeginTick()
{
	Tick();
}

void FTickHandle::Tick()
{
	if (!Application::GetInstance()->GetWorld()) 
		return;

	TimeElapsedSinceLastFrame += FixedUpdateClock.restart().asSeconds();
	if (TimeElapsedSinceLastFrame >= DELTA_TIME_STEP)
	{
		// Step is used to update physics position/rotation
		Application::GetInstance()->GetWorld()->Step(DELTA_TIME_STEP,	//update frequency
			8,												//velocityIterations
			3												//positionIterations  
		);

		Application::GetInstance()->Tick(DELTA_TIME_STEP);
		TimeElapsedSinceLastFrame -= DELTA_TIME_STEP;
	}
}
