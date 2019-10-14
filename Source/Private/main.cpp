#pragma once
#include "Application.h"

int main()
{
	//std::unique_ptr<Application> Instance = std::make_unique<Application>(); 
	
	if (Application::GetInstance())
	{
		Application::GetInstance()->Initialize();
		Application::GetInstance()->BeginPlay();
	}
	return 0;
}