#include "Text.h"
#include "AssetLoader.h"

FText::FText()
{
	Font = FAssetLoader::GetInstance()->GetFont(RESOURCES_FONT_DEFAULT);
	Text.setFont(*Font);
	Text.setString(TextData);
	Text.setCharacterSize(TEXT_SIZE_M);
}

void FText::SetFont(SFML::Font* NewFont)
{
	Font = NewFont;
	Text.setFont(*Font);
}

void FText::SetText(std::string NewText)
{
	TextData = NewText;
	Text.setString(NewText);
}