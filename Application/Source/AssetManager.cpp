#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
void AssetManager::Init(Application* app)
{
	m_APP = app;
}
void AssetManager::LoadAllResources(std::string FolderPath, ResourceType TypesToLoad)
{
	std::string FilePath{};
	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
		FilePath = dir_entry.path().string();
		size_t LocOfPng = FilePath.find(".png",0);
		if (LocOfPng == (uint64_t)-1)
			continue;
		Texture* texture = m_APP->m_Renderer->LoadTexture(FilePath);
		FilePath = FilePath.substr(FolderPath.size(), FilePath .size() - FolderPath.size());
		size_t t = std::hash<std::string>{}(FilePath);
		m_ResourceCount[TypesToLoad]++;
		m_Resources[t] = texture;
	}


}

