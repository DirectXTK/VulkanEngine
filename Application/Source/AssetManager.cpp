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
			//name of the file
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size()-4);

			size_t t = std::hash<std::string>{}(FilePath);
		
			LoadAssetPerma<Texture>(texture,AssetType::TEXTURE,FilePath);
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

					

			//Get texture info 
			//texture= m_APP->m_Renderer->LoadTexture(FilePath);
			//LoadAssetPerma<TextureAtlasData>(texture,AssetType::TEXTURE,TexturePath );
			

			texture->CreateTextureAtlasData(FilePath);



			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());

			
			m_ResourceCount[TypesToLoad]++;
					
					LoadAssetPerma<Texture>(texture,AssetType::TEXTURE,TexturePath );
				

		
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
	
void AssetManager::DebugStatistics(bool GUI){
		//Core::Log(ErrorType::Info,"Font Count ",m_ResourceCount[AssetType::FONT]);
		//Core::Log(ErrorType::Info,"Texture Count ",m_ResourceCount[AssetType::TEXTUREATLAS]);

}


void AssetManager::LoadAnimation(const std::string& FolderPath)
{
	
	std::string TexturePath{};
	std::string AnimationPath{};
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
		TexturePath = TexturePath.substr(0,TexturePath.size()-5);
		TexturePath +=".png";

		Texture* baseTexture = m_APP->m_Renderer->LoadTexture(TexturePath);

		auto index =TexturePath.find("Resources");

		TexturePath = TexturePath.substr(FolderPath.size(),TexturePath.size()-FolderPath.size()-4);
		TexturePath +="Texture";
		printf("Texture path %s %lu\n",TexturePath.c_str(),Core::GetStringHash(TexturePath));

		Asset<Texture> asset=LoadAssetPerma<Texture>(baseTexture,AssetType::TEXTURE,TexturePath);

		GUUID TextureID =Core::GetStringHash(TexturePath);

		baseTexture->CreateTextureAtlasData(FilePath);


		AnimationPath = FilePath;
		//Load animation
		size_t atlasGUUID = std::hash<std::string>{}(AnimationPath.substr(FolderPath.size(),AnimationPath.size()-FolderPath.size()-5));
	
	


		Animator* animator= new Animator(AnimationPath,atlasGUUID, TextureID,&m_APP->m_AssetManager);
		AnimationPath =AnimationPath.substr(FolderPath.size(),AnimationPath.size()-FolderPath.size()-5);
		printf("Animation path %s %lu\n",AnimationPath.c_str(),atlasGUUID);
		animator->SetStage("IDLE");
		
	
		LoadAssetPerma<Animator>(animator,AssetType::ANIMATION,AnimationPath);

	}
	Core::Log(ErrorType::Error, "This function isn't implemented yet");
}

