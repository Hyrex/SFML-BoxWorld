#include "TextManager.h"
#include "Application.h"

int FTextEffectBundle::ID = 1;
FTextManager* FTextManager::Instance = nullptr;

FTextManager* FTextManager::GetInstance()
{
	if (!Instance)
		Instance = new FTextManager();

	return Instance;
}

FTextManager::~FTextManager()
{
	delete Instance;
}

void FTextManager::Tick()
{
	for (auto& i : TextEffectBundles)
	{
		if (i.Effects.size() != 0)
		{
			for (auto& Effect : i.Effects)
			{
				Effect->ModifyTextTarget = i.TargetText;
				Effect->Update();
			}
		}
	}
}

void FTextManager::Draw()
{
	for (auto& Itr : GetInstance()->TextEffectBundles)
	{
		if (Itr.TargetText->IsVisible())
			Application::GetInstance()->GetWindow()->draw(Itr.TargetText->Text);
	}
}

void FTextManager::Register(FTextEffectBundle Data)
{
	GetInstance()->TextEffectBundles.push_back(Data);
}

void FTextManager::Unregister(const int ID)
{
	std::vector<FTextEffectBundle>& v = GetInstance()->TextEffectBundles;
	auto RemovePredicate = [ID](const FTextEffectBundle Data) -> bool { return Data.ID == ID; };
	v.erase(std::remove_if(v.begin(), v.end(), RemovePredicate), v.end());
}

std::vector<FTextEffectBundle>& FTextManager::GetTextEffectBundles()
{
	return GetInstance()->TextEffectBundles;
}

