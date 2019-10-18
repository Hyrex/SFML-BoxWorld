#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Application.h"
#include "Actor.h"
#include "Character.h"
#include "StaticBlockActor.h"
#include "PhysicComponent.h"
#include "PhysicComponentContactListener.h"
#include "TextManager.h"

Application*  Application::Instance = nullptr;


Application::Application()
{
	ContactListener = std::make_unique<PhysicComponentContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	World = std::make_shared<b2World>(Gravity);
	World->SetContactListener(ContactListener.get());
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

	std::unique_ptr<StaticBlockActor> TopBorder = std::make_unique<StaticBlockActor>("TopBorderActor", GAMETAG_STATIC_OBJECT);
	TopBorder->Construct(XBorder, UBorderLocation);
	Actors.push_back(std::move(TopBorder));

	std::unique_ptr<StaticBlockActor> LeftBorder = std::make_unique<StaticBlockActor>("LeftBorder", GAMETAG_STATIC_OBJECT);
	LeftBorder->Construct(YBorder, LBorderLocation);
	Actors.push_back(std::move(LeftBorder));

	std::unique_ptr<StaticBlockActor> RightBorder = std::make_unique<StaticBlockActor>("RightBorder", GAMETAG_STATIC_OBJECT);
	RightBorder->Construct(YBorder, RBorderLocation);
	Actors.push_back(std::move(RightBorder));

	std::unique_ptr<StaticBlockActor> BotBorder = std::make_unique<StaticBlockActor>("BotBorder", GAMETAG_STATIC_OBJECT);
	BotBorder->Construct(XBorder, DBorderLocation);
	Actors.push_back(std::move(BotBorder));

	for (int i = 0; i < 2; i++)
	{
		AngleIndicators[i].color = (i == 1) ? sf::Color::Cyan : sf::Color::Blue;
	}

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

	PositionDataText->SetText(GameState.GetMouseLocationString() + (bIsPaused ? " [Lost Focus]" : " [Has Focus]"));

	FTextManager::GetInstance()->Tick();

	if (!bIsPaused)
	{
		GameState.Tick();

		for (auto& Itr : Actors)
			if (Itr) Itr->Tick();

		// Dynamic Text 
		TimerText->SetText(GameState.GetFormattedElapsedTimeString());

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (GameState.IsGameStarted())
			{
				GameState.GetPlayer()->StartJump();
			}
			else
			{
				GameState.StartGame();
				StartGameTranslateOut->Begin();
				StartGameAlphaFadeOut->Begin();
			}
		}
		else
		{
			GameState.GetPlayer()->StopJump();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			AppWindow.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			if (GameState.IsGameStarted())
				GameState.GetPlayer()->MoveLeft();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			if(GameState.IsGameStarted())
				GameState.GetPlayer()->MoveRight();
		}

		if (sf::Joystick::isConnected(0) && GameState.IsGameStarted())
		{
			const float fDPadXValue = sf::Joystick::getAxisPosition(0, PS4_DPadX);
			if (fDPadXValue > 10.00f) GameState.GetPlayer()->MoveRight();
			if (fDPadXValue < -10.00f) GameState.GetPlayer()->MoveLeft();

			const float fLeftAxisX = sf::Joystick::getAxisPosition(0, PS4_LeftXAxis);
			if (fLeftAxisX > 10.00f) GameState.GetPlayer()->MoveRight();
			if (fLeftAxisX < -10.00f) GameState.GetPlayer()->MoveLeft();

			if (sf::Joystick::isButtonPressed(0, PS4_Cross))
			{
				if (GameState.IsGameStarted())
				{
					GameState.GetPlayer()->StartJump();
				}
				else
				{
					GameState.StartGame();
					StartGameTranslateOut->Begin();
					StartGameAlphaFadeOut->Begin();
				}
			}

			const float fLT = sf::Joystick::getAxisPosition(0, PS4_LeftTriggerAxis);
			const float fRT = sf::Joystick::getAxisPosition(0, PS4_RightTriggerAxis);

			if (fLT > 95.0f && fRT > 95.0f)
				AppWindow.close();
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

		// Update Angle Indicator
		AngleIndicators[0].position = sf::Vector2f(0.0f, 0.0f);
		AngleIndicators[1].position = sf::Vector2f(sf::Mouse::getPosition(AppWindow));
		
	
	}

	// Rendering
	AppWindow.clear(sf::Color::Black);

	for (auto& Itr : Actors)
		Itr->Draw();
	
	if (GameState.GetPlayer()->IsInitialized())
	{
		AppWindow.draw(*GameState.GetPlayer()->Getb2Component().Component->GetDebugForward());
		for (int i = 0; i < GameState.GetPlayer()->GetShapeCount(); ++i)
		{
			if(sf::Shape* s = GameState.GetPlayer()->GetShapeAtIndex(i))
				AppWindow.draw(*s);
		}
	}

	FTextManager::GetInstance()->Draw();
	
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

	PositionDataText->Text.setPosition(sf::Vector2f(64.0f, 64.0f));

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
	FTextManager::Register(e1);

	FTextEffectBundle e3;
	e3.TargetText = StartGameText.get();
	e3.Effects.push_back(StartGameTranslateOut.get());
	e3.Effects.push_back(StartGameAlphaFadeOut.get());
	FTextManager::Register(e3);

	FTextEffectBundle e4;
	e4.TargetText = TimerText.get();
	FTextManager::Register(e4);
}

void Application::OnKeyPressed()
{

}

void Application::OnKeyRelease()
{

}