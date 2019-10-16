#pragma once
#include "Application.h"


#include <iostream>

class A 
{
public:
	void(*OnTestFunc)(int x) = 0;
	void TestFunc(int x) 
	{
		std::cout << "A TestFunc called " << x << std::endl;
	}
};

class Container
{
public:
	std::unique_ptr<A> a = std::make_unique<A>();
};

class ChildContainer : public Container
{
public:
	static void TestFunc(int x) { std::cout << "Child TestFunc called " << x << std::endl; }
	void SetFunc() { a.get()->OnTestFunc = TestFunc; }
	void CallFunc() { a.get()->OnTestFunc(5); }
};

int main()
{
	//to simulate crash
	
	//ChildContainer* cc = new ChildContainer();
	//cc->SetFunc();
	//cc->CallFunc();
	//system("pause");
	//
	//
	//delete cc;
	
	if (Application::GetInstance())
	{
		Application::GetInstance()->Initialize();
		Application::GetInstance()->BeginPlay();
	}
	return 0;
}