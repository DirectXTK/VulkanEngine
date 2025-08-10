#pragma once
#include "AppCore.h"

enum class AssetType{NONE,TEXTURE,TEXTUREATLAS,TEXTUREMETADATA,SHADER,AI,ANIMATION,FONT,NUMOFASSETTYPES};
class Texture;
class Application;
class AssetManager;

struct AssetHandle{

	AssetHandle(GUUID id,AssetManager* manager,const AssetType& type,void* Data): ID(id),Manager(manager),Type(type),Data(Data){
	}
	AssetHandle(){}

	void MakeItPermaHandle(){RefCount++;}
	GUUID ID{};
	uint32_t RefCount{};
	AssetManager* Manager{};
	AssetType Type{};
	void* Data{};
};

void ASSETLOADASSET(AssetHandle* data);

template<typename T>
class Asset {
public:
	friend AssetManager;
	Asset(){}
    Asset(const Asset& other){
		  m_Data = other.m_Data;
		if(m_Data)
        	m_Data->RefCount+=1;
	}
    Asset( Asset&& other){
        m_Data = std::move(other.m_Data);

		memset(&other,0,sizeof(Asset));
		if(m_Data)
        	m_Data->RefCount+=1;
	}
    Asset& operator=(const Asset& other){
		if(&other == this)
			return *this;
		Release();

		m_Data = other.m_Data;
		if(m_Data)
        	m_Data->RefCount+=1;
        return *this;


	}
    Asset& operator=( Asset&& other){
		
		m_Data = std::move(other.m_Data);
		
		//m_Data->RefCount++;
		memset(&other,0,sizeof(Asset));

        return *this;
}

    void* GetData(){return m_Data->Data;}
    AssetType GetType(){return m_Data->Type;}
	uint32_t GetRefCount(){return m_Data->RefCount;}
	GUUID GetID(){return m_Data->ID;}

    bool operator==(const Asset& other){return m_Data->ID == other.m_Data->ID? true:false;}
    operator bool(){return m_Data==nullptr? false:true&&m_Data->Type == AssetType::NONE? false:true;}


    ~Asset(){Release();}
private:
	Asset(AssetHandle* handle){m_Data = handle;m_Data->RefCount++;}
 
    void CreateAsset(GUUID id,void* resource,const AssetType& type,AssetManager* manager){

    
         m_Data->Data = resource;
		   m_Data->Manager = manager;
        m_Data->Type= type;
		m_Data->ID = id;
        //*m_RefCount+=1;
	}
    void Release(){
        if(m_Data){
			m_Data->RefCount-=1;
		}
        else{
			return;
		}
        
        if(m_Data->RefCount ==0)
        {	
			T* ConvertedData = (T*)m_Data->Data;
			delete ConvertedData;
			ASSETLOADASSET(m_Data);
          	 
        }
	}


	AssetHandle* m_Data{};

};

class AssetManager
{
public:
	void Init(Application* app);

	void LoadAllAssets(std::string FolderPath, AssetType TypesToLoad);
	template<typename T>
	Asset<T> LoadAsset(void* Resource, AssetType type,std::string Name){
		GUUID ID = Core::GetStringHash(Name);
	  auto Index = m_Resources.find(ID);
        if(Index != m_Resources.end())
            return Asset<T>(&m_Resources[ID]);
         m_Resources[ID] = AssetHandle(ID,this,type,Resource);
		 m_ResourceCount[type]++;
         return Asset<T>(&m_Resources[ID]);
	}
	template<typename T>
	Asset<T> LoadAssetPerma(void* Resource, AssetType type,std::string Name){
		GUUID ID = Core::GetStringHash(Name);
	  auto Index = m_Resources.find(ID);
        if(Index != m_Resources.end())
            return Asset<T>(&m_Resources[ID]);
         m_Resources[ID] = AssetHandle(ID,this,type,Resource);
		 m_Resources[ID].MakeItPermaHandle();
		 m_ResourceCount[type]++;
         return Asset<T>(&m_Resources[ID]);
	}
	


	uint64_t GetAssetCount(AssetType type) { return m_ResourceCount[type];}
	uint64_t GetAssetCount() { return m_Resources.size();}

	void UnloadAsset(GUUID handle,AssetType type){
		DebugStatistics(false);

		auto Index = m_Resources.find(handle);
		if(Index != m_Resources.end()){
			m_Resources.erase(Index);

			m_ResourceCount[type]--;
		}
	}
	template<typename T>
	Asset<T> HasAsset(GUUID Handle) {
		if (m_Resources.find(Handle) == m_Resources.end()) {
			
			return Asset<T>();
		}
		return Asset<T>(&m_Resources[Handle]);
	}

	template<typename T>
	Asset<T> GetAsset(GUUID Handle) {
		if (m_Resources.find(Handle) == m_Resources.end()) {
			Core::Log(ErrorType::Warning, "Resources wasn't found (ID", Handle.ID,")");
			
			return Asset<T>();
		}
		return Asset<T>(&m_Resources[Handle]);
	}
	AssetType GetAssetType(GUUID id){
		if (m_Resources.find(id) == m_Resources.end()) {
			Core::Log(ErrorType::Warning, "Resources wasn't found (ID", id.ID,")");
			return AssetType::NONE;
		}
		return m_Resources[id].Type;
	}
	void DebugStatistics(bool GUI);
	
	//void Save();
	void FreeAssetsPerma();
private:
	void LoadFont(const std::string& FilePath);
	void LoadAnimation(const std::string& FolderPath);

	std::unordered_map<GUUID, AssetHandle> m_Resources{};
	std::unordered_map<AssetType, uint64_t> m_ResourceCount;



	Application* m_APP{};
};

