#pragma once
#include "AppCore.h"

enum class ResourceType{UNDIFINED,TEXTURE,TEXTUREATLAS,TEXTUREMETADATA,SHADER,AI,ANIMATION};
class Texture;
class Application;

template<typename T>
class AssetController {
public:
	
	void IncreaseRefCount() {
		m_RefCount++;
	}
	void DecreaseRefCount() {
		m_RefCount--;
		if (m_RefCount == 0)
			~AssetController();
	}

	~AssetController() {

	}
private:
	uint64_t m_RefCount{};
	T m_Asset{};
};

template<typename T>
class Asset {
public:
	~Asset() {
		m_AssetController.DecreaseRefCount();
	}
private:
	AssetController<T> m_AssetController{};
};
class AssetManager
{
public:
	void Init(Application* app);
	void LoadAllResources(std::string FolderPath,ResourceType TypesToLoad);
	uint64_t GetResourceCount(ResourceType type) { return m_ResourceCount[type];}
	template<typename T>
	T* GetResource(GUUID Handle) {
		if (m_Resources.find(Handle) == m_Resources.end()) {
			Core::Log(ErrorType::Error, "Resources wasn't found (ID", Handle.ID,")");
				return nullptr;
		}
		return (T*)m_Resources[Handle];
	}
	
	//void Save();
private:

	void LoadAnimation(const std::string& FolderPath);
	GUUID LoadTexture(const std::string& TexturePath);

	std::unordered_map<GUUID, void*> m_Resources{};
	std::unordered_map<ResourceType, uint64_t> m_ResourceCount;
	Application* m_APP{};
};

