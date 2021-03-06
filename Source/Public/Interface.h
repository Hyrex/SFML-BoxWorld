#pragma once

class ITickable
{
public:
	virtual void Tick() = 0;
};

class IDrawable
{
public:
	virtual void Draw() = 0;
};

class IWindowsEvent
{
public:
	virtual void OnWindowResize() = 0;
};