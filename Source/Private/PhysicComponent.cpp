#include "PhysicComponent.h"
#include "Application.h"
#include "AssetLoader.h"


PhysicComponent::PhysicComponent(std::string ComponentName)
{
	this->ComponentName = ComponentName;
	LOG_CMD("ComponentName Set!");
	LOG_CMD(ComponentName);
#if DEBUG_GAME
	// debug properties
	const sf::Vector2f DebugArrowSize = DEBUG_ARROW_SIZE * 0.25f;
	DebugForward->setTexture(FAssetLoader::GetInstance()->GetTexture(RESOURCES_TEXTURE_DEBUG_ARROW));
	DebugForward->setOrigin(sf::Vector2f(0, +DebugArrowSize.y / 2));
	DebugForward->setSize(DebugArrowSize);
	DebugForward->setFillColor(sf::Color(255, 0, 0, 255));
#endif
}

void PhysicComponent::Tick()
{
	if (Body)
	{
#if DEBUG_GAME
		// Box2D uses radians for rotation, SFML uses degree
		DebugForward->setRotation(Body->GetAngle() * 180 / b2_pi);
		DebugForward->setPosition(UNIT_BOX2D_TO_SFML(Body->GetPosition().x), UNIT_BOX2D_TO_SFML(Body->GetPosition().y));
#endif

		if (TickCallback != 0)
		{
			TickCallback(this);
		}
	}
}

void PhysicComponent::SetOwningParent(Actor* Parent)
{
	this->Parent = Parent;
}

void PhysicComponent::SetGenerateOverlap(const bool bNewGenerate)
{
	bGenerateOverlaps = bNewGenerate;
}

void PhysicComponent::SetPhysicEnabled(bool bEnabled)
{
	Body->SetActive(bEnabled);
	Body->SetAwake(bEnabled);
}

#pragma region Callbacks
void PhysicComponent::BeginOverlap(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB)
{
	if (bGenerateOverlaps && Component && OverlappedComponent)
	{
		if (OnBeginOverlapCallback != 0)
		{
			OnBeginOverlapCallback(Component, OverlappedComponent, UserDataA, UserDataB); 
		}
	}
}

void PhysicComponent::EndOverlap(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB)
{
	if (bGenerateOverlaps && Component && OverlappedComponent)
	{
		if (OnEndOverlapCallback != 0)
		{
			OnEndOverlapCallback(Component, OverlappedComponent, UserDataA, UserDataB);
		}
	}
}

void PhysicComponent::BindOnBeginoverlap(void(*Callback)(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB))
{
	OnBeginOverlapCallback = Callback;
}

void PhysicComponent::BindOnEndOverlap(void(*Callback)(PhysicComponent* Component, PhysicComponent* OverlappedComponent, void* UserDataA, void* UserDataB))
{
	OnEndOverlapCallback = Callback;
}

void PhysicComponent::BindOnTick(void(*TickFunction)(PhysicComponent* Component))
{
	TickCallback = TickFunction;
}
#pragma endregion Callbacks

void PhysicComponent::CreateBody(b2BodyDef* BodyDef)
{
	Body = Application::GetInstance()->GetWorld()->CreateBody(BodyDef);
}

void PhysicComponent::CreateFixture(b2FixtureDef* FixtureDef)
{
	Body->CreateFixture(FixtureDef);
}

Actor* PhysicComponent::GetOwner()
{
	return Parent;
}

std::string PhysicComponent::GetComponentName() const
{
	return ComponentName;
}

sf::RectangleShape* PhysicComponent::GetDebugForward()
{
	return DebugForward.get();
}

b2Body* PhysicComponent::GetBody()
{
	return Body;
}
