#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>

#include "TickHandle.h"
#include "AssetLoader.h"
#include "GameState.h"
#include "TextManager.h"
#include "Defines.h"

class b2Actor2D;
class b2Actor2DContactListener;

struct FRenderWindowData
{
	int Width = 1024;
	int Height = 768;
	int BitsPerPixel = 32;
	
	std::string WindowName = "BoxWorld";

	inline sf::VideoMode	GetVideoModeFromData()	{	return sf::VideoMode(Width, Height, BitsPerPixel);	}
	inline std::string		GetWindowName()			{	return WindowName;	}
};

class Application
{
public:
	static Application* GetInstance();
	~Application();

	void Initialize();
	void BeginPlay();
	void Tick(const float DeltaTime);

	b2World* GetWorld() const { return World.get(); }
	FTickHandle& GetTickHandle() { return TickHandle;  }
	sf::RenderWindow* GetWindow() { return &AppWindow; }

private:

	Application();
	static void BallTick(b2Actor2D* Actor);
	static void SensorOverlap(b2Actor2D* OverlapActor);

	void OnWindowResize();
	void SetupText();

	FTickHandle TickHandle;
	FGameState GameState;
	FTextManager TextRenderer;

	FRenderWindowData RenderWindowData;
	sf::RenderWindow AppWindow;
	sf::View AppView;
	sf::Music* BGM;
	
	//Box2D
	b2Vec2 Gravity; 
	std::shared_ptr<b2World> World;
	std::unique_ptr<b2Actor2DContactListener> b2ActorContactListner;

	std::vector<std::unique_ptr<sf::Shape>> RenderShapes;
	std::vector<std::unique_ptr<b2Actor2D>> b2Actors;

	sf::Vertex AngleIndicators[2];

	bool bLeftPressed = false;
	bool bRightPressed = false;
	bool bSpacePressed = false;
	bool bRightMousePressed = false;
	bool bMiddleMousePressed = false;
	bool bIsPaused = false;


#if DEBUG_GAME
	std::unique_ptr<FText>							PositionDataText = std::make_unique<FText>();
	std::unique_ptr<FTextFlashEffect>				FlashPositionEffect = std::make_unique<FTextFlashEffect>();
#endif

	std::unique_ptr<FText>							StartGameText = std::make_unique<FText>();
	std::unique_ptr<FTextLerpLocationEffect>		StartGameTranslateOut = std::make_unique<FTextLerpLocationEffect>();
	std::unique_ptr<FTextLerpAlphaEffect>			StartGameAlphaFadeOut = std::make_unique<FTextLerpAlphaEffect>();

	std::unique_ptr<FText>							TimerText = std::make_unique<FText>();

	static Application* Instance;
	void OnKeyPressed();
	void OnKeyRelease();
};
