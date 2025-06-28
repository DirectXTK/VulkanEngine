#pragma once
#include "AppCore.h"

enum class AssetType{UNDIFINED,TEXTURE,TEXTUREATLAS,TEXTUREMETADATA,SHADER,AI,ANIMATION,FONT};
class Texture;
class Application;
class AssetManager;
//template<typename T>
class Asset {
public:
	friend AssetManager;
	
	Asset(){CreateAsset(AssetType::UNDIFINED,nullptr);}


	Asset(const Asset&& other){
		m_Data = std::move(other.m_Data);
		m_RefCount = std::move(other.m_RefCount);
		m_Type = std::move(other.m_Type);
			if(m_RefCount){
			Core::Log(ErrorType::Error,"Ref count is null");
		}
	}
	Asset(const Asset& other){
		m_Data = other.m_Data;
		m_RefCount = other.m_RefCount;
		if(m_RefCount)
			*m_RefCount+=1;

		m_Type = other.m_Type;
		
	}
	Asset& operator=(const Asset& other){
	
		m_Data = other.m_Data;
		m_Type = other.m_Type;
		m_RefCount = other.m_RefCount;
		if(m_RefCount){
			*m_RefCount+=1;
		}else{
			Core::Log(ErrorType::Error,"Ref count is null");
		}
		return *this;
	}

	AssetType GetType() { return m_Type; }
	void* GetData() { return m_Data; }
	operator bool(){
		return m_Type== AssetType::UNDIFINED? false:true;
	}
	~Asset() {
		DecRefAndDel();
	}
private:

	void DecRefAndDel(){
		//DEBUG
		if(!m_RefCount){
		Core::Log(ErrorType::Error,"DecRefAndDel m_RefCount is nullptr");
		return;
		}
		
		*m_RefCount-=1;
		if(*m_RefCount <=0)
		{
		delete m_RefCount;
		delete m_Data;

		
		}
	
	}
	Asset(const AssetType& type,void* resource){
		m_Data = resource;
		m_Type = type;
	}
	void CreateAsset(const AssetType& Type, void* Data) {
		m_Data = Data;
		m_Type = Type;
		m_RefCount= new int();
	}
	int* m_RefCount{};
	AssetType m_Type{AssetType::UNDIFINED};
	void* m_Data{};

};
class AssetManager
{
public:
	void Init(Application* app);
	void LoadAllAssets(std::string FolderPath, AssetType TypesToLoad);
	Asset LoadAsset(void* Resource, AssetType type,std::string Name);
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
	Asset CreateAsset(const AssetType& Type,void* Data);
	void LoadFont(const std::string& FilePath);
	void LoadAnimation(const std::string& FolderPath);
	GUUID LoadTexture(const std::string& TexturePath);

	std::unordered_map<GUUID, Asset> m_Resources{};
	std::unordered_map<AssetType, uint64_t> m_ResourceCount;
	Application* m_APP{};
};

