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

	inline SFML::VideoMode	GetVideoModeFromData()	{	return SFML::VideoMode(Width, Height, BitsPerPixel);	}
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
	SFML::RenderWindow* GetWindow() { return &AppWindow; }

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
	SFML::RenderWindow AppWindow;
	SFML::View AppView;
	SFML::Music* BGM;
	
	//Box2D
	b2Vec2 Gravity; 
	std::shared_ptr<b2World> World;
	std::unique_ptr<b2Actor2DContactListener> b2ActorContactListner;

	std::vector<std::unique_ptr<SFML::Shape>> RenderShapes;
	std::vector<std::unique_ptr<b2Actor2D>> b2Actors;

	SFML::Vertex AngleIndicators[2];

	bool bLeftPressed = false;
	bool bRightPressed = false;
	bool bSpacePressed = false;
	bool bRightMousePressed = false;
	bool bMiddleMousePressed = false;


#if DEBUG_GAME
	std::unique_ptr<FText>							PositionDataText = std::make_unique<FText>();
	std::unique_ptr<FTextFlashEffect>				FlashPositionEffect = std::make_unique<FTextFlashEffect>();

	std::unique_ptr<FText>							TranslateTestText = std::make_unique<FText>();
	std::unique_ptr<FTextPingPongTranslationEffect>	PingPongEffect = std::make_unique<FTextPingPongTranslationEffect>();
#endif

	std::unique_ptr<FText>							StartGameText = std::make_unique<FText>();
	std::unique_ptr<FTextLerpLocationEffect>		StartGameTranslateOut = std::make_unique<FTextLerpLocationEffect>();
	std::unique_ptr<FTextLerpAlphaEffect>			StartGameAlphaFadeOut = std::make_unique<FTextLerpAlphaEffect>();

	std::unique_ptr<FText>							TimerText = std::make_unique<FText>();

	static Application* Instance;
};
