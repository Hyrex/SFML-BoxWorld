#pragma once
#include <SFML/Graphics.hpp>
#include "Defines.h"
#include "Interface.h"

class FText
{
public: 
	FText();
	void SetFont(SFML::Font* NewFont);
	void SetText(std::string NewText);
	void SetVisible(bool bNewVisibility) { bIsVisible = bNewVisibility; }
	bool IsVisible() const { return bIsVisible; }

	SFML::Text	Text;
	SFML::Font* Font;
	std::string TextData = "NO TEXT";

private:
	bool bIsVisible = true;
};