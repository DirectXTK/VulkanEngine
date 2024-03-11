#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
#include <Windows.h>

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
			size_t LocOfPng = FilePath.find(".png", 0);
			if (LocOfPng == (uint64_t)-1)
				continue;
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

	default: {
		Core::Log(ErrorType::Error, "Invalid ResourceType.");
		break;
	}
	}


}

