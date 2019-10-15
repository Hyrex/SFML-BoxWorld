#include "TextEffect.h"
#include "Text.h"

void FTextLerpLocationEffect::Begin()
{
	bBegin = true;
	ElapsedTime = 0;
}

void FTextLerpLocationEffect::End()
{
	bBegin = false;
}

void FTextLerpLocationEffect::Reset()
{
	bBegin = false;
	bPaused = false;
	ElapsedTime = 0;
}

void FTextLerpLocationEffect::Update()
{
	if (ModifyTextTarget && ModifyTextTarget->IsVisible() && !bPaused && bBegin && Duration >= 0.0f && ElapsedTime <= Duration)
	{
		const float Delta = ElapsedTime / Duration;
		const sf::Vector2f Diff = LocalEndLocation - LocalStartLocation;
		const sf::Vector2f ResultLocation = LocalStartLocation + (Diff * Delta);
		ModifyTextTarget->Text.setPosition(ResultLocation);
	
		ElapsedTime += DELTA_TIME_STEP;
	}
}

void FTextLerpLocationEffect::SetDuration(float NewDuration)
{
	Duration = NewDuration;
}

void FTextLerpLocationEffect::SetStartLocation(sf::Vector2f NewStartLocation)
{
	LocalStartLocation = NewStartLocation;
}

void FTextLerpLocationEffect::SetEndLocation(sf::Vector2f NewEndLocation)
{
	LocalEndLocation = NewEndLocation;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FTextPingPongTranslationEffect::Update()
{
	if (ModifyTextTarget && ModifyTextTarget->IsVisible() && !bPaused && bBegin && Duration > 0.0f)
	{
		const float LerpDelta = ElapsedTime / Duration;
		const sf::Vector2f DiffVec = LocalEndLocation - LocalStartLocation;
		const sf::Vector2f ResultLocation = LocalStartLocation + (DiffVec * LerpDelta);
		ModifyTextTarget->Text.setPosition(ResultLocation);

		if (bForward)
			ElapsedTime += DELTA_TIME_STEP;
		else
			ElapsedTime -= DELTA_TIME_STEP;

		if (ElapsedTime <= 0.0f || ElapsedTime >= Duration)
		{
			bForward = !bForward;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FTextLerpAlphaEffect::Begin()
{
	bBegin = true;
	ElapsedTime = 0;
}

void FTextLerpAlphaEffect::End()
{
	bBegin = false;
}

void FTextLerpAlphaEffect::Reset()
{
	bBegin = false;
	bPaused = false;
	ElapsedTime = 0;

	if (ModifyTextTarget)
	{
		sf::Color DefaultFillColor = ModifyTextTarget->Text.getFillColor();
		DefaultFillColor.a = (sf::Uint8)(StartAlpha * 255.0f);
		ModifyTextTarget->Text.setFillColor(DefaultFillColor);

		sf::Color DefaultOutlineColor = ModifyTextTarget->Text.getOutlineColor();
		DefaultOutlineColor.a = (sf::Uint8)(StartAlpha * 255.0f);
		ModifyTextTarget->Text.setOutlineColor(DefaultOutlineColor);
	}
}

void FTextLerpAlphaEffect::Update()
{
	if (ModifyTextTarget && ModifyTextTarget->IsVisible() && !bPaused && bBegin && FadeTime > 0.0f && ElapsedTime <= FadeTime)
	{
		const float Diff = (StartAlpha - EndAlpha) * 255.0f;
		const float Delta = ElapsedTime / FadeTime;
		const float LerpAlpha = Delta * Diff;
		const float StartingApha = StartAlpha * 255.0f;

		sf::Color FillColor = ModifyTextTarget->Text.getFillColor();
		FillColor.a = (sf::Uint8)(StartingApha - LerpAlpha);
		ModifyTextTarget->Text.setFillColor(FillColor);

		sf::Color OutlineColor = ModifyTextTarget->Text.getOutlineColor();
		OutlineColor.a = (sf::Uint8)(StartingApha - LerpAlpha);
		ModifyTextTarget->Text.setOutlineColor(OutlineColor);

		ElapsedTime += DELTA_TIME_STEP;

		if (ElapsedTime >= FadeTime)
			End();
	}
}

void FTextLerpAlphaEffect::SetFadeTime(float NewFadeTime)
{
	FadeTime = NewFadeTime;
}

void FTextLerpAlphaEffect::SetAlpha(const float StartA, const float EndA)
{
	StartAlpha = StartA;
	EndAlpha = EndA;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FTextFlashEffect::Update()
{
	if (ModifyTextTarget && ModifyTextTarget->IsVisible() && !bPaused && bBegin && FadeTime > 0.005f)
	{
		const float Diff = (StartAlpha - EndAlpha) * 255.0f;
		const float Delta = ElapsedTime / FadeTime;
		const float LerpAlpha = Delta * Diff;
		const float StartingAlpha = StartAlpha * 255.0f;
		const float CurrentAlpha = StartingAlpha - LerpAlpha;

		sf::Color FillColor = ModifyTextTarget->Text.getFillColor();
		FillColor.a = (sf::Uint8)(CurrentAlpha);
		ModifyTextTarget->Text.setFillColor(FillColor);

		sf::Color OutlineColor = ModifyTextTarget->Text.getOutlineColor();
		OutlineColor.a = (sf::Uint8)(CurrentAlpha);
		ModifyTextTarget->Text.setOutlineColor(OutlineColor);

		if(bForward)
			ElapsedTime += DELTA_TIME_STEP;
		else
			ElapsedTime -= DELTA_TIME_STEP;

		if (ElapsedTime < 0.0f || ElapsedTime > FadeTime)
			bForward = !bForward;
	}
}