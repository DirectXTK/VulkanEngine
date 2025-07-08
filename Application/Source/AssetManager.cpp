#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
#include "Animator.h"

  


void ASSETLOADASSET(AssetHandle* data){
			data->Manager->UnloadAsset(data->ID,data->Type);
}

void AssetManager::Init(Application* app)
{
	m_APP = app;
}
void AssetManager::LoadAllAssets(std::string FolderPath, AssetType TypesToLoad)
{
	/*
	std::string FilePath{};
	switch (TypesToLoad) {
	case AssetType::TEXTURE: {
		std::string str{};
		str.resize(200);
		str = Core::GetModuleFileName();
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			FilePath = dir_entry.path().string();
			
			if (Core::GetFileExtension(FilePath) != "png")
				continue;

			Texture* texture = m_APP->m_Renderer->LoadTexture(FilePath);
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());

			size_t t = std::hash<std::string>{}(FilePath);
		
			m_ResourceCount[TypesToLoad]++;

			m_Resources[t].CreateAsset((GUUID)t,texture,TypesToLoad,this);
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
			//m_Resources[Core::GetStringHash(FilePath)].CreateAsset(Core::GetStringHash(FilePath), AtlasData,TypesToLoad,this);

			
			
			Texture** atlases = texture->GetTextureAtlas();
			AtlasCount = texture->GetTextureAtlasSize();

			//TexturePath = TexturePath.substr(0,TexturePath.size());
			

				for (uint32_t i = 0; i < AtlasCount; i++) {
					m_ResourceCount[AssetType::TEXTURE]++;
					GUUID t = Core::GetStringHash((TexturePath + std::to_string(i)));
					m_Resources[t] = atlases[i];

				}
				

		
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
	*/
	


}
	
void AssetManager::DebugStatistics(bool GUI){
	if(GUI){

	}else{
		//Core::Log(ErrorType::Info,"Font Count ",m_ResourceCount[AssetType::FONT]);
		//Core::Log(ErrorType::Info,"Texture Count ",m_ResourceCount[AssetType::TEXTUREATLAS]);

	}
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
		m_ResourceCount[AssetType::ANIMATION]++;::


	
	}*/
	Core::Log(ErrorType::Error, "This function isn't implemented yet");
}

