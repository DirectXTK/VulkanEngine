#pragma once
#include "AppCore.h"
enum class ResourceType{UNDIFINED,TEXTURE,TEXTUREATLAS,SHADER};

class Application;
class AssetManager
{
public:
	void Init(Application* app);
	void LoadAllResources(std::string FolderPath,ResourceType TypesToLoad);
	template<typename T>
	T* GetResource(GUUID Handle) {
		return (T*)m_Resources[Handle];
	}
private:
	std::unordered_map<GUUID, void*> m_Resources{};
	Application* m_APP{};
};

