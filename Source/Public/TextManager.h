#pragma once
#include <SFML/Graphics.hpp>
#include "Text.h"
#include "TextEffect.h"
#include "Interface.h"

struct FTextEffectBundle
{
	FText* TargetText;
	std::vector<FTextEffect*> Effects;
};

class FTextManager : public ITickable
{
public:
	virtual void Tick() override;

	void Add(FTextEffectBundle Data) { TextEffectBundles.push_back(Data); }
	std::vector<FTextEffectBundle>& GetTextEffectBundles() { return TextEffectBundles; }
private:
	std::vector<FTextEffectBundle> TextEffectBundles;
};

