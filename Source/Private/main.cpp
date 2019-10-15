#pragma once
#include "Application.h"

int main()
{
	if (Application::GetInstance())
	{
		Application::GetInstance()->Initialize();
		Application::GetInstance()->BeginPlay();
	}
	return 0;
}