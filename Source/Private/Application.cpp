#include <SFML/Audio.hpp>

#include "Application.h"
#include "Character.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"
#include "TextManager.h"

Application*  Application::Instance = nullptr;

Application::Application()
{
	b2ActorContactListner = std::make_unique<b2Actor2DContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	World = std::make_shared<b2World>(Gravity);
	World->SetContactListener(b2ActorContactListner.get());
}

Application::~Application()
{
	delete Instance;
}

Application* Application::GetInstance()
{
	if (!Instance)
	{
		Instance = new Application();
	}

	return Instance;
}

void Application::BeginPlay()
{
	while (AppWindow.isOpen())
	{
		TickHandle.BeginTick();
	}
}

void Application::Initialize()
{
	//if (BGM = FAssetLoader::FindMusic(RESOURCES_AUDIO_TROLOLO))
	//{
	//	BGM->setVolume(0);
	//	BGM->setLoop(true);
	//	BGM->play();
	//}

	// Window creation
	AppWindow.create(RenderWindowData.GetVideoModeFromData(), RenderWindowData.GetWindowName(), sf::Style::Close | sf::Style::Resize | sf::Style::Titlebar);
	AppWindow.setVerticalSyncEnabled(true);
	AppWindow.setActive();
	AppWindow.setMouseCursorVisible(false);
	AppWindow.resetGLStates();
	AppWindow.setFramerateLimit(60);

	AppView = SFML::View(AppWindow.getDefaultView());

	// Border creations
	const float BorderThickness = 16.0f;
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
	const SFML::Vector2f XBorder(ViewportX, BorderThickness);
	const SFML::Vector2f YBorder(BorderThickness, ViewportY);
	const SFML::Vector2f UBorderLocation(ViewportX * 0.5f, BorderThickness * 0.5f);
	const SFML::Vector2f DBorderLocation(ViewportX * 0.5f, ViewportY - BorderThickness * 0.5f);
	const SFML::Vector2f LBorderLocation(BorderThickness * 0.5f, ViewportY * 0.5f);
	const SFML::Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f, ViewportY * 0.5f);

	// Collapsed function body. Transferring ownership of local unique ptr to the container
	auto b2ActorInit = [this](std::unique_ptr<b2Actor2D>& p, const SFML::Color c) ->void 
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(SFML::Color::Black);
		p->GetShape()->setFillColor(c);
		b2Actors.push_back(move(p));
	};

	std::unique_ptr<b2Actor2D> TopBorder = std::make_unique<b2Actor2D>("TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
	b2ActorInit(TopBorder, SFML::Color(100, 100, 100));

	std::unique_ptr<b2Actor2D> LeftBorder = std::make_unique<b2Actor2D>("LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
	b2ActorInit(LeftBorder , SFML::Color(100, 100, 100) );

	std::unique_ptr<b2Actor2D> RightBorder = std::make_unique<b2Actor2D>("RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
	b2ActorInit(RightBorder, SFML::Color(100, 100, 100));

	std::unique_ptr<b2Actor2D> BotBorder = std::make_unique<b2Actor2D>("BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
	b2ActorInit(BotBorder, SFML::Color(100, 100, 100));

	for (int i = 0; i < 2; i++)
	{
		AngleIndicators[i].color = (i == 1) ? SFML::Color::Cyan : SFML::Color::Blue;
	}
		
	// Board
	const float offsetX = ViewportX * 0.98f;
	const float offsetY = ViewportY * 0.35f;
	const SFML::Vector2f boardSize(8.0f, 200.0f);
	const SFML::Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);
	
	const SFML::Vector2f netEdgeSize(8.0f, 90.0f);
	const SFML::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);

	const SFML::Vector2f sensorSize(48.0f, 48.0f);
	const SFML::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
	std::unique_ptr<b2Actor2D> ScoreSensor = std::make_unique<b2Actor2D>("sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0f, false, true);
	ScoreSensor->BindOnBeginoverlap(SensorOverlap);
	b2ActorInit(ScoreSensor, SFML::Color(255, 255, 0, 100));

	SetupText();
}

void Application::Tick(const float DeltaTime)
{
	SFML::Event WindowEvent;

	// Delay PollEvent, avoid to use
	while (AppWindow.pollEvent(WindowEvent));
	{
		if (WindowEvent.type == SFML::Event::Closed)
		{
			AppWindow.close();		
		}
		else if (WindowEvent.type == SFML::Event::Resized)
		{
			OnWindowResize();
		}

		AppWindow.setView(AppView);
	}

	GameState.Tick();
	TextRenderer.Tick();

	for (auto& i : b2Actors)
		if (i) i->Tick();

	// Dynamic Text 
	TimerText->SetText(GameState.GetFormattedElapsedTimeString());
	PositionDataText->SetText(GameState.GetMouseLocationString());
	PositionDataText->Text.setPosition(SFML::Vector2f(SFML::Mouse::getPosition(AppWindow)));
		
	if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::Space))
	{
		if (!GameState.IsGameStarted())
		{
			GameState.StartGame();
			StartGameTranslateOut->Begin();
			StartGameAlphaFadeOut->Begin();
		}
		else
		{
			GameState.GetPlayer()->Jump();
		}
	}
	
	if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::Escape))
	{
		AppWindow.close();
	}
	
	if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::A)) //Joy stick plz!
	{

		GameState.GetPlayer()->MoveLeft();
	}
	
	if (SFML::Keyboard::isKeyPressed(SFML::Keyboard::D))
	{

		GameState.GetPlayer()->MoveRight();
	}
	
	if (SFML::Mouse::isButtonPressed(SFML::Mouse::Right))
	{
		if (!bRightMousePressed)
		{
			if (!GameState.IsGameOver() && GameState.IsGameStarted())
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
			StartGameTranslateOut->Reset();
			StartGameAlphaFadeOut->Reset();
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

	if (GameState.GetPlayer()->IsInitialized())
	{
		AppWindow.draw(*GameState.GetPlayer()->Getb2Actor()->GetShape());
		AppWindow.draw(*GameState.GetPlayer()->Getb2Actor()->DebugForward);
	}

	for (auto& Itr : TextRenderer.GetTextEffectBundles())
	{
		if(Itr.TargetText->IsVisible())
			AppWindow.draw(Itr.TargetText->Text);
	}

	AppWindow.draw(AngleIndicators, 2, SFML::Lines);
	AppWindow.display();
}

void Application::OnWindowResize()
{
	sf::Vector2f WindowSize = static_cast<sf::Vector2f>(AppWindow.getSize());

	// Minimum size
	if (WindowSize.x < RenderWindowData.Width)
		WindowSize.x = (float)RenderWindowData.Width;

	if (WindowSize.y < RenderWindowData.Height)
		WindowSize.y = (float)RenderWindowData.Height;

	// Apply possible size changes
	AppWindow.setSize(static_cast<SFML::Vector2u>(WindowSize));


	// Reset  GUI view
	AppView = SFML::View(SFML::FloatRect(0.f, 0.f, WindowSize.x, WindowSize.y));
	AppWindow.setView(AppView);


	// The sidebar should be 180px wide
	const float width = 180.f;
}

void Application::SetupText()
{
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;

	// Static String Texts
	TimerText->Text.setPosition(SFML::Vector2f(32.0f, ViewportY - 64.0f));
	TimerText->SetFont(FAssetLoader::GetInstance()->GetFont(RESOURCES_FONT_PIXEL));

	TranslateTestText->SetText("TestTranslate");
	TranslateTestText->Text.setPosition(SFML::Vector2f(ViewportX / 2, ViewportY / 2));

	StartGameText->SetText("Press [Space] to Start");
	StartGameText->Text.setPosition(SFML::Vector2f(ViewportX / 2, ViewportY / 2));

	// Effects
	FlashPositionEffect->SetAlpha(0.2f, 0.8f);
	FlashPositionEffect->SetFadeTime(2.5f);
	FlashPositionEffect->Begin();

	PingPongEffect->SetDuration(1.0f);
	PingPongEffect->SetStartLocation(SFML::Vector2f(0.0f, 0.0f));
	PingPongEffect->SetEndLocation(SFML::Vector2f(ViewportX / 2, ViewportY / 2));
	PingPongEffect->Begin();

	StartGameTranslateOut->SetDuration(3.0f);
	StartGameTranslateOut->SetStartLocation(StartGameText->Text.getPosition());
	StartGameTranslateOut->SetEndLocation(StartGameText->Text.getPosition() + SFML::Vector2f(0, -500.0f));

	StartGameAlphaFadeOut->SetFadeTime(2.0f);
	StartGameAlphaFadeOut->SetAlpha(1.0f, 0.0f);

	FTextEffectBundle e1;
	e1.TargetText = PositionDataText.get();
	e1.Effects.push_back(FlashPositionEffect.get());
	TextRenderer.Add(e1);

	FTextEffectBundle e2;
	e2.TargetText = TranslateTestText.get();
	e2.Effects.push_back(PingPongEffect.get());
	e2.Effects.push_back(FlashPositionEffect.get());
	TextRenderer.Add(e2);
	
	FTextEffectBundle e3;
	e3.TargetText = StartGameText.get();
	e3.Effects.push_back(StartGameTranslateOut.get());
	e3.Effects.push_back(StartGameAlphaFadeOut.get());
	TextRenderer.Add(e3);

	FTextEffectBundle e4;
	e4.TargetText = TimerText.get();
	TextRenderer.Add(e4);
}

void Application::BallTick(b2Actor2D* Actor)
{
	/// this code was to mark inactive, no longer in need to pool anything.
	if (!Actor) return;
	if (!Application::GetInstance()->GameState.IsGameStarted()) return;

	const bool Ax = Actor->GetLocation().x >= Application::GetInstance()->RenderWindowData.Width + 64.0f;
	const bool Bx = Actor->GetLocation().x <= -64.0f;
	const bool Ay = Actor->GetLocation().y >= Application::GetInstance()->RenderWindowData.Height + 64.0f;
	const bool By = Actor->GetLocation().y <= -64.0f;
}

void Application::SensorOverlap(b2Actor2D* OverlapActor)
{
	if (!Application::GetInstance()->GameState.IsGameStarted()) return;

	if (OverlapActor->GetObjectName() == "Ball")
	{
	
	}
}
