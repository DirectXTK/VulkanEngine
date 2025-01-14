#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
#include <Windows.h>
#include "Animator.h"
void AssetManager::Init(Application* app)
{
	m_APP = app;
}
void AssetManager::LoadAllResources(std::string FolderPath, ResourceType TypesToLoad)
{
	std::string FilePath{};
	switch (TypesToLoad) {
	case ResourceType::TEXTURE: {
		std::wstring str{};
		str.resize(200);
		GetModuleFileName(nullptr, str.data(), 200);
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			FilePath = dir_entry.path().string();
			
			if (Core::GetFileExtension(FilePath) != "png")
				continue;

			Texture* texture = m_APP->m_Renderer->LoadTexture(FilePath);
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size()-4);

			size_t t = std::hash<std::string>{}(FilePath);
		
			m_ResourceCount[TypesToLoad]++;
			m_Resources[t] = texture;
		}
		break;
	}
	case ResourceType::TEXTUREATLAS: {
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

			m_Resources[Core::GetStringHash(TexturePath)] = texture;
			m_ResourceCount[ResourceType::TEXTURE]++;

					
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());



			texture = GetResource<Texture>(Core::GetStringHash(TexturePath));
			
			Texture** atlases = texture->GetTextureAtlas();
			AtlasCount = texture->GetTextureAtlasSize();

			//TexturePath = TexturePath.substr(0,TexturePath.size());
			

				for (uint32_t i = 0; i < AtlasCount; i++) {
					m_ResourceCount[ResourceType::TEXTURE]++;
					GUUID t = Core::GetStringHash((TexturePath + std::to_string(i)));
					m_Resources[t] = atlases[i];

				}

		
		}
		break;

	}
	case ResourceType::ANIMATION: {
		LoadAnimation(FolderPath);
		break;
	}

	default: {
		Core::Log(ErrorType::Error, "Invalid ResourceType.");
		break;
	}
	}


}

GUUID AssetManager::LoadResource(void* Resource, ResourceType type,std::string Name)
{
	GUUID ID = Core::GetStringHash(Name);
	auto Index = m_Resources.find(ID);
	if (Index == m_Resources.end()) 
		Core::Log(ErrorType::Warning, "Resource is being loaded twice.If you intended to overwrite this resources please use ReloadResource() function");
	
	m_ResourceCount[type]++;
	m_Resources[ID] = Resource;
	return ID;
}

GUUID AssetManager::ReloadResource(void* Resource, ResourceType type, std::string Name)
{
	GUUID ID = Core::GetStringHash(Name);
	void* OldResource = m_Resources[ID];
	switch (type) {
		case ResourceType::TEXTURE:{
			Texture* texture = (Texture*)OldResource;
			texture->~Texture();
			break;
		}
		default: {
			Core::Log(ErrorType::Error, "Implement other resource type cases at ReloardResource() function");
			break;
		}
	}


	m_ResourceCount[type]++;
	m_Resources[ID] = Resource;
	return ID;
}

void AssetManager::LoadAnimation(const std::string& FolderPath)
{
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

		Texture* Basetexture = GetResource<Texture>(TextureID);
		Texture** atlases = Basetexture->GetTextureAtlas();
		AtlasCount = Basetexture->GetTextureAtlasSize();

		//TexturePath = TexturePath.substr(0, TexturePath.size() - 4);
		TexturePath = TexturePath.substr(FolderPath.size(), TexturePath.find(".json")-FolderPath.size());


		for (uint32_t i = 0; i < AtlasCount; i++) {
			m_ResourceCount[ResourceType::TEXTURE]++;
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
		m_ResourceCount[ResourceType::ANIMATION]++;


	
	}
}

GUUID AssetManager::LoadTexture(const std::string& TexturePath)
{
		Texture* texture = m_APP->m_Renderer->LoadTexture(TexturePath);

		std::string Temp = TexturePath.substr(0, TexturePath.find("."));

		GUUID ID = Core::GetStringHash(Temp);
		m_ResourceCount[ResourceType::TEXTURE]++;
		m_Resources[ID] = texture;
		return ID;
}

