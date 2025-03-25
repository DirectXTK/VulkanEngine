#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
#include <Windows.h>
#include "Animator.h"
void AssetManager::Init(Application* app)
{
	m_APP = app;
}
void AssetManager::LoadAllAssets(std::string FolderPath, AssetType TypesToLoad)
{
	std::string FilePath{};
	switch (TypesToLoad) {
	case AssetType::TEXTURE: {
		std::wstring str{};
		str.resize(200);
		GetModuleFileName(nullptr, str.data(), 200);
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			FilePath = dir_entry.path().string();
			
			if (Core::GetFileExtension(FilePath) != "png")
				continue;

			Texture* texture = m_APP->m_Renderer->LoadTexture(FilePath);
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());

			size_t t = std::hash<std::string>{}(FilePath);
		
			m_ResourceCount[TypesToLoad]++;

			m_Resources[t] = Asset(TypesToLoad,texture);
		}
		break;
	}
	case AssetType::TEXTUREATLAS: {
		std::string TexturePath{};
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {

			Texture* texture{};
			uint32_t AtlasCount{};

			FilePath = dir_entry.path().string();
			if (Core::GetFileExtension(FilePath) == ".json")
				continue;

			size_t LocOfPng = FilePath.find(".json", 0);
			if (LocOfPng == (uint64_t)-1)
				continue;

			TexturePath = FilePath;
			texture = m_APP->m_Renderer->LoadTexture(TexturePath);
			TexturePath = FilePath.substr(0, FilePath.size() - 5);

			TexturePath = TexturePath.substr(FolderPath.size(), TexturePath.find(".png")- FolderPath.size());
			TexturePath += ".png";

					

			//Get texture info 

			TextureAtlasData* AtlasData =Texture::CreateTextureAtlasData(FilePath);
			AtlasData->TextureID = Core::GetStringHash(TexturePath);

			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());

			
			m_ResourceCount[TypesToLoad]++;
			m_Resources[Core::GetStringHash(FilePath)]= Asset(TypesToLoad,AtlasData);

			/*
			
			Texture** atlases = texture->GetTextureAtlas();
			AtlasCount = texture->GetTextureAtlasSize();

			//TexturePath = TexturePath.substr(0,TexturePath.size());
			

				for (uint32_t i = 0; i < AtlasCount; i++) {
					m_ResourceCount[AssetType::TEXTURE]++;
					GUUID t = Core::GetStringHash((TexturePath + std::to_string(i)));
					m_Resources[t] = atlases[i];

				}
				*/

		
		}
		break;

	}
	case AssetType::ANIMATION: {
		LoadAnimation(FolderPath);
		break;
	}

	default: {
		Core::Log(ErrorType::Error, "Invalid AssetType.");
		break;
	}
	}


}

GUUID AssetManager::LoadAsset(void* Resource, AssetType type,std::string Name)
{
	GUUID ID = Core::GetStringHash(Name);
	auto Index = m_Resources.find(ID);
	if (Index == m_Resources.end()) {
		Core::Log(ErrorType::Warning, "Resource is being loaded twice.If you intended to overwrite this resources please use ReloadResource() function");
		m_ResourceCount[type]++;

	}
	m_Resources[ID] = Asset(type,Resource);
	return ID;
}

GUUID AssetManager::ReloadAsset(void* Resource, AssetType type, std::string Name)
{
	GUUID ID = Core::GetStringHash(Name);
	Asset oldAsset = m_Resources[ID];
	switch (oldAsset.GetType()) {
		case AssetType::TEXTURE:{
			Texture* texture = (Texture*)oldAsset.GetData();
			texture->~Texture();
			break;
		}
		default: {
			Core::Log(ErrorType::Error, "Implement other resource type cases at ReloardResource() function");
			break;
		}
	}

	Core::Log(ErrorType::Error, "This Function isn't implemented yet.");
	return ID;
}

void AssetManager::LoadAnimation(const std::string& FolderPath)
{
	/*
	std::string TexturePath{};
	std::string FilePath{};
	uint32_t AtlasCount{};
	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {


		FilePath = dir_entry.path().string();
		if (Core::GetFileExtension(FilePath).size() == 0)
			continue;

		size_t MetaDataPath = FilePath.find(".json", 0);
		if (MetaDataPath == (uint64_t)-1)
			continue;

		//FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());
		//Load texture only works if it uses atlases
		//TODO: CHange so it support seperate textures also.

		TexturePath = FilePath;

		GUUID TextureID =LoadTexture(TexturePath);

		Asset Basetexture = GetAsset(TextureID);
		Texture** atlases = Basetexture->GetTextureAtlas();
		AtlasCount = Basetexture->GetTextureAtlasSize();

		//TexturePath = TexturePath.substr(0, TexturePath.size() - 4);
		TexturePath = TexturePath.substr(FolderPath.size(), TexturePath.find(".json")-FolderPath.size());


		for (uint32_t i = 0; i < AtlasCount; i++) {
			m_ResourceCount[AssetType::TEXTURE]++;
			m_Resources[Core::GetStringHash((TexturePath + std::to_string(i)))] = atlases[i];

		}
		FilePath = FilePath.substr(FolderPath.size(), FilePath.find(".json") - FolderPath.size());
		//Load animation
		size_t atlasGUUID = std::hash<std::string>{}(FilePath);
	
		std::string str = dir_entry.path().string();
		//Something wrong with renderer texture id or something
		Animator* animator= new Animator(dir_entry.path().string(), atlasGUUID, FilePath);
		animator->SetStage("IDLE");
		m_Resources[atlasGUUID] = animator;
		m_ResourceCount[AssetType::ANIMATION]++;


	
	}*/
	Core::Log(ErrorType::Error, "This function isn't implemented yet");
}

GUUID AssetManager::LoadTexture(const std::string& TexturePath)
{
		Texture* texture = m_APP->m_Renderer->LoadTexture(TexturePath);

		std::string Temp = TexturePath.substr(0, TexturePath.find("."));

		GUUID ID = Core::GetStringHash(Temp);
		m_ResourceCount[AssetType::TEXTURE]++;
		m_Resources[ID] = Asset(AssetType::TEXTURE,texture);
		return ID;
}

