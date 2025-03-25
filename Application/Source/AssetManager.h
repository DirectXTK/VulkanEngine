#pragma once
#include "AppCore.h"

enum class AssetType{UNDIFINED,TEXTURE,TEXTUREATLAS,TEXTUREMETADATA,SHADER,AI,ANIMATION,FONT};
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
//template<typename T>
class Asset {
public:
	Asset(const AssetType& Type, void* Data) {
		m_Data = Data;
		m_Type = Type;
	}
	Asset(){}

	AssetType GetType() { return m_Type; }
	void* GetData() { return m_Data; }

	~Asset() {
		
	}
private:
	//AssetController<T> m_AssetController{};

	AssetType m_Type{AssetType::UNDIFINED};
	void* m_Data{};

};
class AssetManager
{
public:
	void Init(Application* app);
	void LoadAllAssets(std::string FolderPath, AssetType TypesToLoad);
	GUUID LoadAsset(void* Resource, AssetType type,std::string Name);
	GUUID ReloadAsset(void* Resource, AssetType type, std::string Name);
	uint64_t GetAssetCount(AssetType type) { return m_ResourceCount[type];}

	Asset GetAsset(GUUID Handle) {
		if (m_Resources.find(Handle) == m_Resources.end()) {
			Core::Log(ErrorType::Warning, "Resources wasn't found (ID", Handle.ID,")");
				return Asset(AssetType::UNDIFINED,nullptr);
		}
		return m_Resources[Handle];
	}
	
	//void Save();
private:

	void LoadAnimation(const std::string& FolderPath);
	GUUID LoadTexture(const std::string& TexturePath);

	std::unordered_map<GUUID, Asset> m_Resources{};
	std::unordered_map<AssetType, uint64_t> m_ResourceCount;
	Application* m_APP{};
};

