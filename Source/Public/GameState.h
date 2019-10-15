#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Character.h"
#include "Interface.h"

class Application;

class FGameState : ITickable
{
public:
	
	FGameState();
	virtual void Tick() override;

	void ResetValues();

	void StartGame();
	void ResetGame();

	int GetScore()					const	{ return Score; }
	int GetLevel()					const	{ return Level; }
	
	bool IsPlayerSpawned()			const	{ return Player.get() != nullptr;}
	bool IsLevelUp()				const	{ return bIsLevelUp; }
	bool IsGameOver()				const	{ return bIsGameOver; }
	bool IsGameStarted()			const	{ return bIsGameStarted; }

	std::string	GetMouseLocationString()		const;
	std::string GetLevelString()				const;
	std::string GetScoreString()				const;
	std::string GetElapsedTimeMinString()		const;
	std::string GetElapsedTimeSecondString()	const;
	std::string GetFormattedElapsedTimeString() const;
	
	Character* GetPlayer();
private:

	int Score;
	int Level;

	bool bIsGameOver;
	bool bIsLevelUp;
	bool bIsGameStarted;

	float TotalSurvivedTime;
	
	float RestartTimer = 3.0f;

	std::unique_ptr<Character> Player;
};

