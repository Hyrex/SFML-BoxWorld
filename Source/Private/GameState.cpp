#include "GameState.h"
#include "Application.h"

FGameState::FGameState()
{
	Player = std::make_unique<Character>();
	ResetValues();
}

std::string FGameState::GetMouseLocationString() const
{
	SFML::Vector2f v = SFML::Vector2f(SFML::Mouse::getPosition(*Application::GetInstance()->GetWindow()));
	float x = v.x; float y = v.y;
	std::ostringstream ss; ss << x << "," << y; return ss.str();
}

std::string FGameState::GetLevelString() const
{
	std::ostringstream ss; 
	(ss << Level);  
	return ss.str();
}

std::string FGameState::GetScoreString() const
{
	std::ostringstream ss; 
	(ss << Score);  
	return ss.str();
}

std::string FGameState::GetElapsedTimeMinString() const
{
	std::ostringstream ss; 
	(ss << (int)(TotalSurvivedTime / 60.0f));  
	return ss.str();
}

std::string FGameState::GetElapsedTimeSecondString() const
{
	std::ostringstream ss; 
	(ss << ((int)TotalSurvivedTime) % 60);  
	return ss.str();
}

std::string FGameState::GetFormattedElapsedTimeString() const
{
	return "ELAPSED MIN\n" + GetElapsedTimeMinString() + " M" + GetElapsedTimeSecondString() + " S";
}

Character* FGameState::GetPlayer()
{
	return Player.get();
}

void FGameState::ResetValues()
{
	Score = 0;
	Level = 1;

	bIsGameStarted = false;
	bIsGameOver = false;
	bIsLevelUp = false;

	RestartTimer = 3.0f;
}

void FGameState::Tick()
{
	if (bIsGameStarted)
	{
		// Check Level up
		if (false)
		{
			bIsLevelUp = true;
		}
		else
		{
			bIsLevelUp = false;
		}

		// Check GameOver
		bIsGameOver = false;
		if (bIsGameOver)
			RestartTimer -= DELTA_TIME_STEP;

		if (RestartTimer < 0.0f)
			ResetValues();

		TotalSurvivedTime += DELTA_TIME_STEP;
		Player->Tick();
	}
}

void FGameState::StartGame()
{
	if (!bIsGameStarted)
		bIsGameStarted = true;

	Player->Initialize();
}

void FGameState::ResetGame()
{
	bIsGameStarted = false;
	ResetValues();
}