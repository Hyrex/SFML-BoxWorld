#pragma once
#include <SFML/Graphics.hpp>
#include "Defines.h"

class FText;

class FTextEffect
{
public:
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Reset() = 0;
	virtual void Update() = 0;
	virtual void Pause() { bPaused = true; }
	virtual void Resume() { bPaused = false; }

	void SetPaused(bool bIsPaused) { bPaused = bIsPaused; }
	bool IsPaused() const { return bPaused; }
	bool HasBegun() const { return bBegin; }

	FText* ModifyTextTarget;
	bool bPaused = false;
	bool bBegin = false;
};

class FTextLerpLocationEffect : public FTextEffect
{
public:
	virtual void Begin() override;
	virtual void End() override;
	virtual void Reset() override;
	virtual void Update() override;

	void SetDuration(float NewDuration);
	void SetStartLocation(SFML::Vector2f NewStartLocation);
	void SetEndLocation(SFML::Vector2f NewEndLocation);

protected:
	float ElapsedTime = 0.0f;
	float Duration = 0.0f;
	SFML::Vector2f LocalStartLocation;
	SFML::Vector2f LocalEndLocation;
};

class FTextPingPongTranslationEffect : public FTextLerpLocationEffect
{
public:
	virtual void Update() override;
protected:
	bool bForward = true;
};

class FTextLerpAlphaEffect : public FTextEffect
{
public: 
	virtual void Begin() override;
	virtual void End() override;
	virtual void Reset() override;
	virtual void Update() override;

	void SetFadeTime(float NewFadeTime);
	void SetAlpha(const float StartA, const float EndA);

protected: 
	float FadeTime = 0.0f;
	float ElapsedTime = 0.0f;
	float StartAlpha = 1.0f;
	float EndAlpha = 0.0f;
};

class FTextFlashEffect : public FTextLerpAlphaEffect
{
public :
	virtual void Update() override;
protected:
	bool bForward = true;
};

