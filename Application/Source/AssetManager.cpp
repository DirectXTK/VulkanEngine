#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
void AssetManager::Init(Application* app)
{
	m_APP = app;
}
void AssetManager::LoadAllResources(std::string FolderPath, ResourceType TypesToLoad)
{
	Texture* texture  = m_APP->m_Renderer->LoadTexture(FolderPath);
	size_t t = std::hash<std::string>{}(FolderPath);
	m_Resources[t] = texture;
}
