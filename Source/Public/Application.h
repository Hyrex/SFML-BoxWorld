#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include "TickHandle.h"
#include "AssetLoader.h"
#include "GameState.h"
#include "TextRenderer.h"
#include "Defines.h"

class Character;
class b2Actor2D;
class b2Actor2DContactListener;

struct FRenderWindowData
{
	int Width = 1000;
	int Height = 728;
	int BitsPerPixel;
	
	std::string WindowName = "BoxWorld";

	inline SFML::VideoMode	GetVideoModeFromData()	{	return SFML::VideoMode(Width, Height, BitsPerPixel);	}
	inline std::string		GetWindowName()			{	return WindowName;	}
};

class Application
{
public:
	Application();
	~Application();;

	int Initialize();
	void BeginPlay();
	void Tick(const float DeltaTime);
	void EndPlay();

	b2World* GetWorld() const { return World.get(); }
	FTickHandle& GetTickHandle() { return TickHandle;  }
	FAssetLoader& GetAssetLoader() { return AssetLoader; }
	SFML::RenderWindow* GetWindow() { return &AppWindow; }

private:

	static void BallTick(b2Actor2D* Actor);
	static void SensorOverlap(b2Actor2D* OverlapActor);

	void SetupText();

	FTickHandle TickHandle;
	FAssetLoader AssetLoader;
	FGameState GameState;
	FTextRenderer TextRenderer;

	FRenderWindowData RenderWindowData;
	SFML::RenderWindow AppWindow;

	SFML::Music* BGM;
	
	//Box2D
	b2Vec2 Gravity; 
	std::shared_ptr<b2World> World;
	std::unique_ptr<b2Actor2DContactListener> b2ActorContactListner;

	std::vector<std::unique_ptr<SFML::Shape>> RenderShapes;
	std::vector<std::unique_ptr<b2Actor2D>> b2Actors;

	SFML::Vertex AngleIndicators[2];

	bool bRightMousePressed = false;
	bool bMiddleMousePressed = false;


	//////////////////////////////////////////
	//		Cached Pointers
	//////////////////////////////////////////

	FTextData* LevelTextCache;
	FTextData* ScoreCache;
	FTextData* HiScoreCache;
	FTextData* ElapsedTimeCache;
	FTextData* CenterTextCache;

	std::unique_ptr<Character> Player;

};
