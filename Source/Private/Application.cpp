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

	AppView = sf::View(AppWindow.getDefaultView());

	// Border creations
	const float BorderThickness = 16.0f;
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
	const sf::Vector2f XBorder(ViewportX, BorderThickness);
	const sf::Vector2f YBorder(BorderThickness, ViewportY);
	const sf::Vector2f UBorderLocation(ViewportX * 0.5f, BorderThickness * 0.5f);
	const sf::Vector2f DBorderLocation(ViewportX * 0.5f, ViewportY - BorderThickness * 0.5f);
	const sf::Vector2f LBorderLocation(BorderThickness * 0.5f, ViewportY * 0.5f);
	const sf::Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f, ViewportY * 0.5f);

	// Collapsed function body. Transferring ownership of local unique ptr to the container
	auto b2ActorInit = [this](std::unique_ptr<b2Actor2D>& p, const sf::Color c) ->void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(sf::Color::Black);
		p->GetShape()->setFillColor(c);
		b2Actors.push_back(move(p));
	};

	std::unique_ptr<b2Actor2D> TopBorder = std::make_unique<b2Actor2D>("TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
	b2ActorInit(TopBorder, sf::Color(100, 100, 100));

	std::unique_ptr<b2Actor2D> LeftBorder = std::make_unique<b2Actor2D>("LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
	b2ActorInit(LeftBorder, sf::Color(100, 100, 100));

	std::unique_ptr<b2Actor2D> RightBorder = std::make_unique<b2Actor2D>("RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
	b2ActorInit(RightBorder, sf::Color(100, 100, 100));

	std::unique_ptr<b2Actor2D> BotBorder = std::make_unique<b2Actor2D>("BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
	b2ActorInit(BotBorder, sf::Color(100, 100, 100));

	for (int i = 0; i < 2; i++)
	{
		AngleIndicators[i].color = (i == 1) ? sf::Color::Cyan : sf::Color::Blue;
	}

	// Board
	const float offsetX = ViewportX * 0.98f;
	const float offsetY = ViewportY * 0.35f;
	const sf::Vector2f boardSize(8.0f, 200.0f);
	const sf::Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);

	const sf::Vector2f netEdgeSize(8.0f, 90.0f);
	const sf::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);

	const sf::Vector2f sensorSize(48.0f, 48.0f);
	const sf::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);

	std::unique_ptr<b2Actor2D> ScoreSensor = std::make_unique<b2Actor2D>("sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0f, false, true);
	ScoreSensor->BindOnBeginoverlap(SensorOverlap);
	b2ActorInit(ScoreSensor, sf::Color(255, 255, 0, 100));

	SetupText();
}

void Application::Tick(const float DeltaTime)
{
	sf::Event WindowEvent;

	// Delay PollEvent, avoid to use
	while (AppWindow.pollEvent(WindowEvent));
	{
		switch (WindowEvent.type)
		{
		case sf::Event::Closed:
			AppWindow.close();
			break;
		case sf::Event::Resized:
			OnWindowResize();
			AppWindow.setView(AppView);
			break;
		case sf::Event::LostFocus:
			std::cout << "Window lose focus." << std::endl;
			bIsPaused = true;
			break;
		case sf::Event::GainedFocus:
			std::cout << "Window gain focus." << std::endl;
			bIsPaused = false;
			break;
		case sf::Event::TextEntered:
			break;
		case sf::Event::KeyPressed:
			OnKeyPressed();
			//std::cout << (int)WindowEvent.key.code << " pressed." << std::endl;
			break;
		case sf::Event::KeyReleased:
			OnKeyRelease();
			//std::cout << (int)WindowEvent.key.code << " released." << std::endl;
			break;
		case sf::Event::MouseWheelScrolled:
			break;
		case sf::Event::MouseButtonPressed:
			break;
		case sf::Event::MouseButtonReleased:
			break;
		case sf::Event::MouseMoved:
			break;
		case sf::Event::MouseEntered:
			break;
		case sf::Event::MouseLeft:
			break;
		case sf::Event::JoystickButtonPressed:
			break;
		case sf::Event::JoystickButtonReleased:
			break;
		case sf::Event::JoystickMoved:
			break;
		case sf::Event::JoystickConnected:
			break;
		case sf::Event::JoystickDisconnected:
			break;
		case sf::Event::TouchBegan:
			break;
		case sf::Event::TouchMoved:
			break;
		case sf::Event::TouchEnded:
			break;
		case sf::Event::SensorChanged:

			break;
		}

		//AppWindow.setView(AppView);
	}

	// Debug draw
	AngleIndicators[0].position = sf::Vector2f(0.0f, 0.0f);
	AngleIndicators[1].position = sf::Vector2f(sf::Mouse::getPosition(AppWindow));

	PositionDataText->SetText(GameState.GetMouseLocationString());
	PositionDataText->Text.setPosition(sf::Vector2f(sf::Mouse::getPosition(AppWindow)));

	TextRenderer.Tick();

	if (!bIsPaused)
	{
		GameState.Tick();

		for (auto& i : b2Actors)
			if (i) i->Tick();

		// Dynamic Text 
		TimerText->SetText(GameState.GetFormattedElapsedTimeString());
	
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			AppWindow.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) //Joy stick plz!
		{
			GameState.GetPlayer()->MoveLeft();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{

			GameState.GetPlayer()->MoveRight();
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
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
		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
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
		//const sf::Vector2f PivotLocation = PivotCache->GetLocation();
		//const sf::Vector2f MouseLocation = sf::Vector2f(sf::Mouse::getPosition(AppWindow));
		//const sf::Vector2f OffsetMouseLocation = sf::Vector2f(sf::Mouse::getPosition(AppWindow) - sf::Vector2i(16, 16));

		//ChargeGaugeMax->setPosition(OffsetMouseLocation);
		//ChargeGaugeMax->setSize(sf::Vector2f(160.0f, 8.0f));
		//ChargeGaugeProgress->setPosition(OffsetMouseLocation);
		//ChargeGaugeProgress->setSize(sf::Vector2f(160.0f * percentage, 8.0f));;

		// Update Angle Indicator
		AngleIndicators[0].position = sf::Vector2f(0.0f, 0.0f);
		AngleIndicators[1].position = sf::Vector2f(sf::Mouse::getPosition(AppWindow));
	}

	// Rendering
	AppWindow.clear(sf::Color::Black);

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
		if (Itr.TargetText->IsVisible())
			AppWindow.draw(Itr.TargetText->Text);
	}

	AppWindow.draw(AngleIndicators, 2, sf::Lines);
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
	AppWindow.setSize(static_cast<sf::Vector2u>(WindowSize));

	// Reset  GUI view
	AppView = sf::View(sf::FloatRect(0.f, 0.f, WindowSize.x, WindowSize.y));
	AppWindow.setView(AppView);

	// The sidebar should be 180px wide
	const float width = 180.f;
}

void Application::SetupText()
{
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;

	// Static String Texts
	TimerText->Text.setPosition(sf::Vector2f(32.0f, ViewportY - 64.0f));
	TimerText->SetFont(FAssetLoader::GetInstance()->GetFont(RESOURCES_FONT_PIXEL));

	StartGameText->SetText("Press [Space] to Start");
	StartGameText->Text.setPosition(sf::Vector2f(ViewportX / 2, ViewportY / 2));

	// Effects
	FlashPositionEffect->SetAlpha(0.2f, 0.8f);
	FlashPositionEffect->SetFadeTime(2.5f);
	FlashPositionEffect->Begin();

	StartGameTranslateOut->SetDuration(3.0f);
	StartGameTranslateOut->SetStartLocation(StartGameText->Text.getPosition());
	StartGameTranslateOut->SetEndLocation(StartGameText->Text.getPosition() + sf::Vector2f(0, -500.0f));

	StartGameAlphaFadeOut->SetFadeTime(2.0f);
	StartGameAlphaFadeOut->SetAlpha(1.0f, 0.0f);

	FTextEffectBundle e1;
	e1.TargetText = PositionDataText.get();
	e1.Effects.push_back(FlashPositionEffect.get());
	TextRenderer.Add(e1);

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

void Application::OnKeyPressed()
{

}

void Application::OnKeyRelease()
{

}