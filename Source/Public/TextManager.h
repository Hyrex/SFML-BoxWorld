#pragma once
#include <SFML/Graphics.hpp>
#include "Text.h"
#include "TextEffect.h"
#include "Interface.h"

struct FTextEffectBundle
{
	static int ID;
	FText* TargetText;
	std::vector<FTextEffect*> Effects;

	// Generate unique IDs.
	FTextEffectBundle() { ID++; };
};

class FTextManager : public ITickable, public IDrawable
{
public:

	static FTextManager* GetInstance();
	~FTextManager();

	virtual void Tick() override;
	virtual void Draw() override;

	static void Register(FTextEffectBundle Data);
	static void Unregister(const int ID);
	static std::vector<FTextEffectBundle>& GetTextEffectBundles();

private:
	FTextManager() {};
	static FTextManager* Instance;

	std::vector<FTextEffectBundle> TextEffectBundles;
};

