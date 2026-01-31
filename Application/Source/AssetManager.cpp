#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
#include "Animator.h"

  


void ASSETLOADASSET(AssetHandle* data){
			data->Manager->UnloadAsset(data->ID,data->Type);
}

void AssetManager::Init(Application* app)
{
	m_APP = app;
}
void AssetManager::LoadAllAssets(std::string FolderPath, AssetType TypesToLoad)
{
	if(!std::filesystem::exists(FolderPath)){
		Core::Log(ErrorType::Warning,"Specified path doesn't exist.{",FolderPath,"}");
		return;
	}
	
	std::string FilePath{};
	switch (TypesToLoad) {
	case AssetType::TEXTURE: {
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			FilePath = dir_entry.path().string();
			
			if (Core::GetFileExtension(FilePath) != "png")
				continue;

			Texture* texture = m_APP->m_Renderer->LoadTexture(FilePath);
			//name of the file
			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size()-4);

			size_t t = std::hash<std::string>{}(FilePath);
			

			LoadAssetPerma<Texture>(texture,AssetType::TEXTURE,FilePath);
		}
		break;
	}
	case AssetType::TEXTUREATLAS: {
		std::string TexturePath{};
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {

			Texture* texture{};
			uint32_t AtlasCount{};

			FilePath = dir_entry.path().string();

			if (Core::GetFileExtension(FilePath) != "json")
				continue;

			size_t LocOfPng = FilePath.find("json", 0);
			if (LocOfPng == (uint64_t)-1)
				continue;

			TexturePath = FilePath;
			texture = m_APP->m_Renderer->LoadTexture(TexturePath);
			TexturePath = FilePath.substr(0, FilePath.size() - 5);

			TexturePath = TexturePath.substr(FolderPath.size(), TexturePath.find(".png")- FolderPath.size());

					

			//Get texture info 
			//texture= m_APP->m_Renderer->LoadTexture(FilePath);
			//LoadAssetPerma<TextureAtlasData>(texture,AssetType::TEXTURE,TexturePath );
			

			texture->CreateTextureAtlasData(FilePath);



			FilePath = FilePath.substr(FolderPath.size(), FilePath.size() - FolderPath.size());

			
			m_ResourceCount[TypesToLoad]++;
					
					LoadAssetPerma<Texture>(texture,AssetType::TEXTURE,TexturePath );
				

		
		}
		break;

	}
	case AssetType::ANIMATION: {

		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
				std::string currentFile = dir_entry.path().string();
				std::string fileExtension = Core::GetFileExtension(FilePath);
				std::string fileName = currentFile.substr(FolderPath.size(),currentFile.size()-FolderPath.size()-fileExtension.size());
				if (fileExtension != "json")
					break;
				LoadAnimation(currentFile,fileName);
		}
	
		break;
	}
	case AssetType::NONE:{
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			std::string currentFile = dir_entry.path().string();
			std::string fileExtension = Core::GetFileExtension(currentFile);	
			std::string fileName = currentFile.substr(FolderPath.size(),currentFile.size()-FolderPath.size()-fileExtension.size()-1);

			if(fileExtension == "png"){
				LoadTexture(currentFile,fileName);
			}else if(fileExtension == "json"){
				LoadTextureAtlas(currentFile,fileName);
			}
			else if(fileExtension == "ttf"){
				LoadFont(currentFile,fileName);
			}

		}
		break;
	}

	default: {
		Core::Log(ErrorType::Error, "Invalid AssetType.");
		break;
	}
	}
	


}
	
void AssetManager::DebugStatistics(bool GUI){
	Core::Log("Resource count",m_Resources.size());
	std::vector<GUUID> idsSorted{};
	idsSorted.reserve(m_Resources.size());
	//Order it first
	//the 0 is none asset type
	uint32_t CurrentType{1};
	while(idsSorted.size() != m_Resources.size()){
	for(auto it = m_Resources.begin();it != m_Resources.end();it++){
		if(idsSorted.size() == m_Resources.size())	
			break;
		if((uint32_t)it->second.GetType() == CurrentType)
			idsSorted.push_back(it->first);
	}
	CurrentType++;
}
	for(uint32_t i=0 ;i < idsSorted.size();i++){
		AssetHandle handle = m_Resources[idsSorted[i]];
		std::string debugStuff{};
		#ifdef DEBUG
			debugStuff = " Path:"+handle.AssetPath;
		#endif

		switch(handle.GetType()){
			case AssetType::FONT:{
				Core::Log("Font ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
				case AssetType::TEXTURE:{
				Core::Log("Texture ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
				case AssetType::TEXTUREATLAS:{
				Core::Log("Animation ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
				case AssetType::ANIMATION:{
				Core::Log("Animation ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
			default:{
				Core::Log("Not implemented or invalid type{",(uint32_t)handle.GetType(),"}"+debugStuff);
				break;
			}
		}
		
	}

}
void AssetManager::LoadFont(const std::string& FilePath,const std::string& fileName){
	//Application::GetApplication()->m_FontSystem.LoadFont();
		auto it = m_Resources.find(fileName);
			if(it != m_Resources.end())
				return;
}
void AssetManager::LoadTexture(const std::string& filePath,const std::string& fileName)
{	
			auto it = m_Resources.find(fileName);
			if(it != m_Resources.end())
				return;
			Texture* texture = m_APP->m_Renderer->LoadTexture(filePath);
			LoadAssetPerma<Texture>(texture,AssetType::TEXTURE,fileName);
}
void AssetManager::LoadTextureAtlas(const std::string& filePath,const std::string& fileName){
			auto it = m_Resources.find(fileName);
			if(it != m_Resources.end()){
				Texture* texture =(Texture*)it->second.Data;
				if(it->second.GetType() != AssetType::TEXTUREATLAS){
					it->second.Type = AssetType::TEXTUREATLAS;
					texture->CreateTextureAtlasData(filePath);
				}
				return;
			}

			std::string TexturePath{filePath.substr(0,filePath.size()-5)};
			TexturePath +=".png";

			Texture* texture{};
			uint32_t AtlasCount{};

			//load the texture of the atlas
			texture = Application::GetRenderer()->LoadTexture(TexturePath,TextureType::TextureAtlas);
			texture->CreateTextureAtlasData(filePath);


			m_ResourceCount[AssetType::TEXTUREATLAS]++;
			LoadAssetPerma<Texture>(texture,AssetType::TEXTUREATLAS,fileName );
}
void AssetManager::LoadAnimation(const std::string& filePath,const std::string& fileName)
{
	auto it = m_Resources.find(fileName);
	if(it != m_Resources.end())
		return;
	std::string TexturePath{};
	std::string AnimationPath{};
	uint32_t AtlasCount{};


		//Load texture only works if it uses atlases
		//TODO: CHange so it support seperate textures also.

		TexturePath = filePath;
		TexturePath = TexturePath.substr(0,TexturePath.size()-5);
		TexturePath +=".png";

		Texture* baseTexture = m_APP->m_Renderer->LoadTexture(TexturePath);


		Asset<Texture> asset=LoadAssetPerma<Texture>(baseTexture,AssetType::TEXTURE,fileName+"Texture");

		GUUID TextureID =asset.GetID();

		baseTexture->CreateTextureAtlasData(filePath);


		//Load animation
		size_t atlasGUUID = std::hash<std::string>{}(fileName);
	
	


		Animator* animator= new Animator(AnimationPath,atlasGUUID, TextureID,&m_APP->m_AssetManager);
		animator->SetStage("IDLE");
	
		LoadAssetPerma<Animator>(animator,AssetType::ANIMATION,fileName);
}
void AssetManager::Shutdown(){
	for(auto it = m_Resources.begin();it != m_Resources.end();it++){
		#ifdef DEBUG
			Core::Log("Deleted:",it->second.AssetPath);
		#endif
		switch(it->second.Type){
			case AssetType::TEXTURE : {delete (Texture*)it->second.Data; break;}
			case AssetType::FONT : {delete (Font*)it->second.Data; break;}
			case AssetType::TEXTUREATLAS : {delete (Texture*)it->second.Data; break;}
			case AssetType::ANIMATION : {delete (Animator*)it->second.Data; break;}
		}
	}
	m_Resources.clear();
}
AssetManager::~AssetManager(){
	Shutdown();
}

namespace Core{
     std::string  GetAssetTypeString(const AssetType& assetType){
		switch(assetType){
			case AssetType::NONE: return "NONE";
			case AssetType::ANIMATION: return "ANIMATION";
			case AssetType::ASSETTYPECOUNT: return "ASSETTYPECOUNT";
			case AssetType::FONT: return "FONT";
			case AssetType::SHADER: return "SHADER";
			case AssetType::TEXTURE: return "TEXTURE";
			case AssetType::TEXTUREATLAS: return "TEXTUREATLAS";
			case AssetType::TEXTUREMETADATA: return "TEXTUREMETADATA";
			default: return "INVALID TYPE";
		}
	 }

}


