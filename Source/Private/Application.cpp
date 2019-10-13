#include <SFML/Audio.hpp>

#include "Application.h"
#include "Character.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"

Application::Application()
{
	b2ActorContactListner = std::make_unique<b2Actor2DContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	World = std::make_shared<b2World>(Gravity);
	World->SetContactListener(b2ActorContactListner.get());

	Player = std::make_unique<Character>();
}

Application::~Application() {}

void Application::BeginPlay()
{
	while (AppWindow.isOpen())
	{
		TickHandle.BeginTick();
	}

	EndPlay();
}

int Application::Initialize()
{
	// Reduce the code length, scope in this function only.
	using namespace sf;
	using namespace std;

	bool bInitChecks = true;
	bInitChecks &= TickHandle.BindApplication(this);
	bInitChecks &= GameState.BindApplication(this);
	bInitChecks &= TextRenderer.BindApplication(this);
	bInitChecks &= AssetLoader.LoadResources();

	if (bInitChecks)
	{
		//if (BGM = FAssetLoader::FindMusic(&AssetLoader, RESOURCES_AUDIO_TROLOLO))
		//{
		//	BGM->setVolume(0);
		//	BGM->setLoop(true);
		//	BGM->play();
		//}

		// Window creation
		AppWindow.create(RenderWindowData.GetVideoModeFromData(), RenderWindowData.GetWindowName());
		AppWindow.setVerticalSyncEnabled(true);
		AppWindow.setActive();
		AppWindow.setMouseCursorVisible(false);

		

		// Border creations
		const float BorderThickness = 16.0f;
		const float ViewportX = (float)RenderWindowData.Width;
		const float ViewportY = (float)RenderWindowData.Height;
		const Vector2f XBorder(ViewportX, BorderThickness);
		const Vector2f YBorder(BorderThickness, ViewportY);
		const Vector2f UBorderLocation(ViewportX * 0.5f						, BorderThickness * 0.5f);
		const Vector2f DBorderLocation(ViewportX * 0.5f						, ViewportY - BorderThickness * 0.5f);
		const Vector2f LBorderLocation(BorderThickness * 0.5f				, ViewportY * 0.5f ); 
		const Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f	, ViewportY * 0.5f ); 

		// Collapsed function body. Transfering ownership of local unique ptr to the container
		auto b2ActorInit = [this](unique_ptr<b2Actor2D>& p, const Color c) ->void 
		{
			p->GetShape()->setOutlineThickness(-1);
			p->GetShape()->setOutlineColor(Color::Black);
			p->GetShape()->setFillColor(c);
			b2Actors.push_back(move(p));
		};

		unique_ptr<b2Actor2D> TopBorder = make_unique<b2Actor2D>(this, World.get(), "TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
		b2ActorInit(TopBorder, Color(100, 100, 100));

		unique_ptr<b2Actor2D> LeftBorder = make_unique<b2Actor2D>(this, World.get(), "LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
		b2ActorInit(LeftBorder , Color(100, 100, 100) );

		unique_ptr<b2Actor2D> RightBorder = make_unique<b2Actor2D>(this, World.get(), "RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
		b2ActorInit(RightBorder,Color(100, 100, 100));

#if 1 // debug floor!
		unique_ptr<b2Actor2D> BotBorder = make_unique<b2Actor2D>(this, World.get(), "BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
		b2ActorInit(BotBorder, Color(100, 100, 100));
#endif 

		for (int i = 0; i < 2; i++)
		{
			AngleIndicators[i].color = (i == 1) ? Color::Cyan : Color::Blue;
		}
		
		// Board
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		const Vector2f boardSize(8.0f, 200.0f);
		const Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);

		const Vector2f netEdgeSize(8.0f, 90.0f);
		const Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
	
		const Vector2f sensorSize(48.0f, 48.0f);
		const Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		unique_ptr<b2Actor2D> ScoreSensor = make_unique<b2Actor2D>(this, World.get(), "sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0f, false, true);
		ScoreSensor->BindOnBeginoverlap(SensorOverlap);
		b2ActorInit(ScoreSensor, Color(255, 255, 0, 100));

		SetupText();
		Player->Spawn(this);
	}

	return bInitChecks;
}

void Application::Tick(const float DeltaTime)
{
	SFML::Event ExitEvent;

	// Delay PollEvent, avoid to use
	while (AppWindow.pollEvent(ExitEvent));
	{
		if (ExitEvent.type == SFML::Event::Closed)
		{
			AppWindow.close();		
		}
	}

	GameState.Tick();
	TextRenderer.Tick();

	for (auto& i : b2Actors)
		if (i) i->Tick();

	Player->Tick();
	
	// Need to update on tick.
	LevelTextCache->Text.setString("LEVEL\n" + GameState.GetLevelString());
	ScoreCache->Text.setString("SCORE\n" + GameState.GetScoreString());
	HiScoreCache->Text.setString("HISCORE\n" + GameState.GetHiScoreString());
	ElapsedTimeCache->Text.setString("ELAPSED MIN\n" + GameState.GetElapsedTimeMinString() + " M" + GameState.GetElapsedTimeSecondString() + " S");
	
	if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::Space))
	{
		if (!GameState.GetIsGameStarted())
		{
			GameState.StartGame();
			CenterTextCache->bIsPaused = false;
		}
		else
		{
			
			Player->b2Actor->GetBodyInstance()->ApplyLinearImpulse(b2Vec2(-0, -1), Player->b2Actor->GetBodyInstance()->GetPosition(), true);

		}
	}
	else if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::Escape))
	{
		AppWindow.close();
	}
	else if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::A)) //Joy stick plz!
	{
		Player->b2Actor->GetBodyInstance()->ApplyLinearImpulse(b2Vec2(-1,0), Player->b2Actor->GetBodyInstance()->GetPosition(), true);

	}
	else if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::D))
	{
		Player->b2Actor->GetBodyInstance()->ApplyLinearImpulse(b2Vec2(+1, 0), Player->b2Actor->GetBodyInstance()->GetPosition(), true);
	}

	// Right Click to Spawn Ball.
	if (SFML::Mouse::isButtonPressed(SFML::Mouse::Right))
	{
		if (!bRightMousePressed)
		{
			if (!GameState.GetIsGameOver() && GameState.GetIsGameStarted())
			{
				
			}
			bRightMousePressed = true;
		}
	}
	else
	{
		bRightMousePressed = false;
	}

	// Middle Button ： Reset
	if (SFML::Mouse::isButtonPressed(SFML::Mouse::Middle))
	{
		if (!bMiddleMousePressed)
		{
			bMiddleMousePressed = true;

			GameState.ResetGame();
			TickHandle.ClearTimer();
			CenterTextCache->Init();
			CenterTextCache->bIsActive = true;
			CenterTextCache->bIsPaused = true;
		}
	}
	else
	{
		bMiddleMousePressed = false;
	}
	

	/// Reserve for remake a charge gauge.
	//const SFML::Vector2f PivotLocation = PivotCache->GetLocation();
	//const SFML::Vector2f MouseLocation = SFML::Vector2f(SFML::Mouse::getPosition(AppWindow));
	//const SFML::Vector2f OffsetMouseLocation = SFML::Vector2f(SFML::Mouse::getPosition(AppWindow) - SFML::Vector2i(16, 16));

	//ChargeGaugeMax->setPosition(OffsetMouseLocation);
	//ChargeGaugeMax->setSize(SFML::Vector2f(160.0f, 8.0f));
	//ChargeGaugeProgress->setPosition(OffsetMouseLocation);
	//ChargeGaugeProgress->setSize(SFML::Vector2f(160.0f * percentage, 8.0f));;

	// Update Angle Indicator
	AngleIndicators[0].position = SFML::Vector2f(0.0f,0.0f);
	AngleIndicators[1].position = SFML::Vector2f(SFML::Mouse::getPosition(AppWindow));


	// Rendering
	AppWindow.clear(SFML::Color::Black);



	for (auto& Itr : RenderShapes)
		AppWindow.draw(*Itr);


	for (auto& Itr : b2Actors)
		AppWindow.draw(*Itr->GetShape());

	AppWindow.draw(*Player->b2Actor->GetShape());
	AppWindow.draw(*Player->b2Actor->DebugForward);


	for (auto& Itr : TextRenderer.GetTextData())
	{
		if(Itr->bIsActive)
			AppWindow.draw(Itr->Text);
	}

	AppWindow.draw(AngleIndicators, 2, SFML::Lines);
	AppWindow.display();
}

void Application::EndPlay()
{
	TickHandle.EndTick();
}

void Application::SetupText()
{
	const float Unit = 32.0f;
	const float LineY1 = 530;
	const float LineY2 = 620;
	std::unique_ptr<FTextData> t1 = std::make_unique<FTextData>();
	t1->StartLocation = SFML::Vector2f(80, LineY1);
	t1->Text.setCharacterSize(TEXT_SIZE_M);
	t1->Font = FAssetLoader::FindFont(&AssetLoader, RESOURCES_FONT_PIXEL);
	t1->Init();
	LevelTextCache = t1.get();
	TextRenderer.Add(t1);

	std::unique_ptr<FTextData> t2 = std::make_unique<FTextData>();
	t2->StartLocation = SFML::Vector2f(80, LineY2);
	t2->Text.setCharacterSize(TEXT_SIZE_M);
	t2->Font = FAssetLoader::FindFont(&AssetLoader, RESOURCES_FONT_PIXEL);
	t2->Init();
	ScoreCache = t2.get();
	TextRenderer.Add(t2);

	std::unique_ptr<FTextData> t3 = std::make_unique<FTextData>();
	t3->StartLocation = SFML::Vector2f(768, LineY1);
	t3->Text.setCharacterSize(TEXT_SIZE_M);
	t3->Font = FAssetLoader::FindFont(&AssetLoader, RESOURCES_FONT_PIXEL);
	t3->Init();
	HiScoreCache = t3.get();
	TextRenderer.Add(t3);


	std::unique_ptr<FTextData> t6 = std::make_unique<FTextData>();
	t6->StartLocation = SFML::Vector2f(368, LineY2);
	t6->Text.setCharacterSize(TEXT_SIZE_M);
	t6->Font = FAssetLoader::FindFont(&AssetLoader, RESOURCES_FONT_PIXEL);
	t6->Init();
	ElapsedTimeCache = t6.get();
	TextRenderer.Add(t6);

	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
	std::unique_ptr<FTextData> t7 = std::make_unique<FTextData>(); // middle
	t7->StartLocation = SFML::Vector2f(ViewportX/2, ViewportY/2);
	t7->EndLocation = SFML::Vector2f(ViewportX / 2, -16);
	t7->FadeTime = 1.0f;
	t7->Text.setCharacterSize(TEXT_SIZE_M);
	t7->TextData = "SPACE TO START!";
	t7->Font = FAssetLoader::FindFont(&AssetLoader, RESOURCES_FONT_PIXEL);
	t7->bIsPaused = true;
	t7->Init();
	CenterTextCache = t7.get();
	TextRenderer.Add(t7);
}

void Application::BallTick(b2Actor2D* Actor)
{
	/// this code was to mark inactive, no longer in need to pool anything.
	if (!Actor) return;
	if (!Actor->GetPackage()->GameState.GetIsGameStarted()) return;

	const bool Ax = Actor->GetLocation().x >= Actor->GetPackage()->RenderWindowData.Width + 64.0f;
	const bool Bx = Actor->GetLocation().x <= -64.0f;
	const bool Ay = Actor->GetLocation().y >= Actor->GetPackage()->RenderWindowData.Height + 64.0f;
	const bool By = Actor->GetLocation().y <= -64.0f;
}

void Application::SensorOverlap(b2Actor2D* OverlapActor)
{
	if (!OverlapActor->GetPackage()->GameState.GetIsGameStarted()) return;

	if (OverlapActor->GetObjectName() == "Ball")
	{
		OverlapActor->GetPackage()->GameState.ScoreBall();
	}
}
