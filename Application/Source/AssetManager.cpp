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
		
			Texture* texture = m_APP->m_Renderer->LoadTexture(FilePath);
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());
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

			FilePath = dir_entry.path().string();
			size_t LocOfPng = FilePath.find(".json", 0);
			if (LocOfPng == (uint64_t)-1)
				continue;
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());

			TexturePath = FilePath;
			TexturePath = TexturePath.substr(0, TexturePath.find(".json"));
			TexturePath += ".png";

			size_t texturehash = std::hash<std::string>{}(TexturePath);
			m_ResourceCount[TypesToLoad]++;
			if (m_Resources.find(texturehash) == m_Resources.end()) {
				TexturePath = dir_entry.path().string();
				TexturePath = TexturePath.substr(0, TexturePath.find(".json"));
				TexturePath += ".png";

				texture = new Texture(m_APP->m_Renderer->GetContext(),TexturePath);
				m_ResourceCount[ResourceType::TEXTURE]++;

				m_Resources[texturehash] = texture;

			}
			else {
				TextureAtlas* atlas=new TextureAtlas(m_APP->m_Renderer->GetContext(), dir_entry.path().string(), (Texture*)m_Resources[texturehash]);
				m_ResourceCount[TypesToLoad]++;

				size_t atlasGUUID = std::hash<std::string>{}(FilePath);
				m_Resources[atlasGUUID] = atlas;


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

void AssetManager::LoadAnimation(const std::string& FolderPath)
{
	std::string TexturePath{};
	std::string FilePath{};
	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {


		FilePath = dir_entry.path().string();
		size_t MetaDataPath = FilePath.find(".json", 0);
		if (MetaDataPath == (uint64_t)-1)
			continue;

		//FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());
		//Load texture only works if it uses atlases
		//TODO: CHange so it support seperate textures also.
		Texture* texture =LoadTexture(FilePath);

		//Load animation
		size_t atlasGUUID = std::hash<std::string>{}(FilePath);
		m_Resources[atlasGUUID] = new Animator(FilePath, texture);
		m_ResourceCount[ResourceType::ANIMATION]++;


	
	}
}

Texture* AssetManager::LoadTexture(const std::string& TexturePath)
{
		Texture* texture = m_APP->m_Renderer->LoadTexture(TexturePath);

		std::string Temp = TexturePath.substr(0, TexturePath.find("."));
		Temp += ".png";

		size_t t = std::hash<std::string>{}(Temp);
		m_ResourceCount[ResourceType::TEXTURE]++;
		m_Resources[t] = texture;
		return texture;
}

