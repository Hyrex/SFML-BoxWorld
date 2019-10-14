#include "AssetLoader.h"

FAssetLoader*  FAssetLoader::Instance = nullptr;

FAssetLoader* FAssetLoader::GetInstance()
{
	if (!Instance)
	{
		Instance = new FAssetLoader();
		Instance->LoadResources();
	}

	return Instance;
}

FAssetLoader::~FAssetLoader() 
{
	for (auto& i : MusicLibrary)
	{
		if (!i.second.get()) continue;
			i.second.get()->stop();
	}

	delete Instance;
}

bool FAssetLoader::LoadResources()
{
	bool bResult = true;
	bResult &= LoadFont(RESOURCES_FONT_PIXEL);
	bResult &= LoadTexture(RESOURCES_TEXTURE_DEBUG_ARROW);
	return bResult;
}

SFML::Texture* FAssetLoader::GetTexture(const std::string Name)
{
	auto Iterator = GetInstance()->TextureLibrary.find(Name);
	return (Iterator != GetInstance()->TextureLibrary.end()) ? Iterator->second.get() : nullptr ;
}

SFML::Font* FAssetLoader::GetFont(const std::string Name)
{
	auto Iterator = GetInstance()->FontLibrary.find(Name);
	return (Iterator != GetInstance()->FontLibrary.end()) ? Iterator->second.get() : nullptr;
}

SFML::Music* FAssetLoader::GetMusic(const std::string Name)
{
	auto Iterator = GetInstance()->MusicLibrary.find(Name);
	return (Iterator != GetInstance()->MusicLibrary.end()) ? Iterator->second.get() : nullptr;
}

bool FAssetLoader::LoadTexture(const std::string FileName)
{
	std::unique_ptr<SFML::Texture> pNewTexture  = std::make_unique<SFML::Texture>();
	bool bResult = pNewTexture->loadFromFile(FileName);
	LOAD_CHECK(bResult, FileName);

	if (bResult)
	{
		TextureLibrary.insert(std::pair<const std::string, std::unique_ptr<SFML::Texture>>(FileName, std::move(pNewTexture)));
	}
	return bResult;
}

bool FAssetLoader::LoadFont(const std::string FileName)
{
	std::unique_ptr<SFML::Font> pNewFont = std::make_unique<SFML::Font>();
	bool bResult = pNewFont->loadFromFile(FileName);
	LOAD_CHECK(bResult, FileName);
	
	if (bResult)
	{
		FontLibrary.insert(std::pair<const std::string, std::unique_ptr<SFML::Font>>(FileName, std::move(pNewFont)));
	}
	return bResult;
}

bool FAssetLoader::LoadMusic(const std::string FileName)
{
	std::unique_ptr<SFML::Music> pNewMusic = std::make_unique<SFML::Music>();
	bool bResult = pNewMusic->openFromFile(FileName);
	LOAD_CHECK(bResult, FileName);

	if (bResult)
	{
		MusicLibrary.insert(std::pair<const std::string, std::unique_ptr<SFML::Music>>(FileName, std::move(pNewMusic)));
	}
	return bResult;
}
