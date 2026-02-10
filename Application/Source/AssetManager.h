#pragma once
#include "AppCore.h"

enum class AssetType{NONE,TEXTURE,TEXTUREATLAS,TEXTUREMETADATA,SHADER,AI,ANIMATION,FONT,NUMOFASSETTYPES,ASSETTYPECOUNT};
class Texture;
class Application;
class AssetManager;

struct AssetHandle{

	#ifdef DEBUG
		AssetHandle(GUUID id,AssetManager* manager,const AssetType& type,void* Data,const std::string& assetFullPath ): ID(id),Manager(manager),Type(type),Data(Data),AssetPath(assetFullPath){}
	#else
		AssetHandle(GUUID id,AssetManager* manager,const AssetType& type,void* Data): ID(id),Manager(manager),Type(type),Data(Data){}
	#endif

	AssetHandle(){}

	void MakeItPermaHandle(){RefCount++;}
	//only use for perma handles once.
	void FreePermaHandle();
	AssetType GetType(){return Type;}
	GUUID ID{};
	uint32_t RefCount{};
	AssetManager* Manager{};
	AssetType Type{};
	void* Data{};
	#ifdef DEBUG
	std::string AssetPath{"NONE"};
	#endif

};
namespace Core{
     std::string  GetAssetTypeString(const AssetType& assetType);

}

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

		//other = nullptr;
		other.m_Data = nullptr;
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
		if(this != &other){
			Release();
			m_Data = std::move(other.m_Data);
			other.m_Data = nullptr;
		}

        return *this;
}

    T* GetData(){return (T*)m_Data->Data;}
    AssetType GetType(){return m_Data->Type;}
	uint32_t GetRefCount(){return m_Data->RefCount;}
	GUUID GetID(){return m_Data->ID;}
	#ifdef DEBUG
	std::string& GetAssetPath(){return m_Data->AssetPath;}
	#endif

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
		if(!m_Data)
			return;
		if(m_Data->RefCount <=0)
			return;
		m_Data->RefCount--;
        
		if(m_Data->RefCount ==0){
			T* ConvertedData = (T*)m_Data->Data;
			delete ConvertedData;
			ASSETLOADASSET(m_Data);
			m_Data = nullptr;
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
		#ifdef DEBUG
      	   m_Resources[ID] = AssetHandle(ID,this,type,Resource,Name);
		#else
      	   m_Resources[ID] = AssetHandle(ID,this,type,Resource);
		#endif
		 m_ResourceCount[type]++;
         return Asset<T>(&m_Resources[ID]);
	}
	template<typename T>
	Asset<T> LoadAssetPerma(void* Resource, AssetType type,std::string Name){
		GUUID ID = Core::GetStringHash(Name);
	  auto Index = m_Resources.find(ID);
        if(Index != m_Resources.end())
            return Asset<T>(&m_Resources[ID]);
        #ifdef DEBUG
      	   m_Resources[ID] = AssetHandle(ID,this,type,Resource,Name);
		#else
      	   m_Resources[ID] = AssetHandle(ID,this,type,Resource);
		#endif
		 m_Resources[ID].MakeItPermaHandle();
		 m_PermaAssets.push_back(ID);
		 m_ResourceCount[type]++;
         return Asset<T>(&m_Resources[ID]);
	}
	template<typename T>
	Asset<T> LoadAssetPerma(void* Resource, AssetType type,GUUID ID){
	 	auto Index = m_Resources.find(ID);
        if(Index != m_Resources.end())
            return Asset<T>(&m_Resources[ID]);
         m_Resources[ID] = AssetHandle(ID,this,type,Resource,"UNNAMED");
		 m_Resources[ID].MakeItPermaHandle();
		 m_PermaAssets.push_back(ID);
		 m_ResourceCount[type]++;
         return Asset<T>(&m_Resources[ID]);
	}
	


	uint64_t GetAssetCount(AssetType type) { return m_ResourceCount[type];}
	uint64_t GetAssetCount() { return m_Resources.size();}

	void UnloadAsset(GUUID handle,AssetType type){

		auto Index = m_Resources.find(handle);
		if(Index != m_Resources.end()){
			m_Resources.erase(Index);

			m_ResourceCount[type]--;
		}
	}
	bool HasAsset(GUUID Handle) {
		if (m_Resources.find(Handle) == m_Resources.end()) {
			
			return false;
		}
		return true;
	}

	template<typename T>
	Asset<T> GetAsset(GUUID Handle) {
		auto Index = m_Resources.find(Handle);
		if (Index== m_Resources.end()) {
			Core::Log(ErrorType::Warning, "Resources wasn't found (ID", Handle.ID,")");
			
			return Asset<T>();
		}
		return Asset<T>(&Index->second);
	}
		template<typename T>
	Asset<T> GetAsset(const std::string& strHandle) {
		GUUID id = Core::GetStringHash(strHandle);
		auto Index = m_Resources.find(id);
		if (Index == m_Resources.end()) {
			Core::Log(ErrorType::Warning, "Resources wasn't found (ID", id.ID,")");
			
			return Asset<T>();
		}
		return Asset<T>(&Index->second);
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

	void Shutdown();
	~AssetManager();
private:
	void LoadFont(const std::string& FilePath,const std::string& fileName);
	void LoadAnimation(const std::string& FolderPath,const std::string& fileName);
	void LoadTexture(const std::string& filePath,const std::string& fileName);
	void LoadTextureAtlas(const std::string& filePath,const std::string& fileName);

	std::unordered_map<GUUID, AssetHandle> m_Resources{};
	std::vector<GUUID> m_PermaAssets{};
	std::unordered_map<AssetType, uint64_t> m_ResourceCount;



	Application* m_APP{};
};

