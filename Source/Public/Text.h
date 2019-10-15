#pragma once
#include <SFML/Graphics.hpp>
#include "Defines.h"
#include "Interface.h"

class FText
{
public: 
	FText();
	void SetFont(sf::Font* NewFont);
	void SetText(std::string NewText);
	void SetVisible(bool bNewVisibility) { bIsVisible = bNewVisibility; }
	bool IsVisible() const { return bIsVisible; }

	sf::Text	Text;
	sf::Font* Font;
	std::string TextData = "NO TEXT";

private:
	bool bIsVisible = true;
};