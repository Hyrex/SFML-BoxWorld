#include "TextManager.h"

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
