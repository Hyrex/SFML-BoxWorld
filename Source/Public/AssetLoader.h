#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Defines.h"


class FAssetLoader
{
public:

	static FAssetLoader* GetInstance();

	~FAssetLoader();
	bool LoadResources();
	static sf::Texture*	GetTexture(const std::string Name);
	static sf::Font*		GetFont(const std::string Name);
	static sf::Music*		GetMusic(const std::string Name);
	
private:


	FAssetLoader() {};
	std::map<const std::string, std::unique_ptr<sf::Texture>> TextureLibrary;
	std::map<const std::string, std::unique_ptr<sf::Font>>	FontLibrary;
	std::map<const std::string, std::unique_ptr<sf::Music>>	MusicLibrary;

	bool LoadTexture(const std::string FileName);
	bool LoadFont(const std::string FileName);
	bool LoadMusic(const std::string FileName);

	static FAssetLoader* Instance;
};

