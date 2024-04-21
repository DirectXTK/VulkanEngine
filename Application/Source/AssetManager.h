#pragma once
#include "AppCore.h"

enum class ResourceType{UNDIFINED,TEXTURE,TEXTUREATLAS,TEXTUREMETADATA,SHADER,AI,ANIMATION};

class Texture;
class Application;
class AssetManager
{
public:
	void Init(Application* app);
	void LoadAllResources(std::string FolderPath,ResourceType TypesToLoad);
	uint64_t GetResourceCount(ResourceType type) { return m_ResourceCount[type];}
	template<typename T>
	T* GetResource(GUUID Handle) {
		return (T*)m_Resources[Handle];
	}
private:

	void LoadAnimation(const std::string& FolderPath);
	Texture* LoadTexture(const std::string& TexturePath);

	std::unordered_map<GUUID, void*> m_Resources{};
	std::unordered_map<ResourceType, uint64_t> m_ResourceCount;
	Application* m_APP{};
};

