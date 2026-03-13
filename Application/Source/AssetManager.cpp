#include "AssetManager.h"
#include "Renderer.h"
#include "Application.h"
#include "Animator.h"

  


void ASSETLOADASSET(AssetHandle* data){
			data->Manager->UnloadAsset(data->ID,data->Type);
}
void AssetHandle::FreePermaHandle(){
	RefCount--;
	if(RefCount <=0)
	{
		switch(Type){
		case AssetType::FONT: {delete (Font*)Data;break;}	
		case AssetType::TEXTURE: {delete (Texture*)Data;break;}	
		case AssetType::TEXTUREATLAS: {delete (Texture*)Data;break;}	
		case AssetType::ANIMATION: {delete (Animator*)Data;break;}	
		case AssetType::SHADER: {delete (Shader*)Data;break;}	
	
		default : {Core::Log("FreePermaHandle type ",(uint32_t)Type," not implemented.");}
		}
	}

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
	
	switch (TypesToLoad) {
	case AssetType::TEXTURE: {
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			std::string currentFile = dir_entry.path().string();
			std::string fileExtension = Core::GetFileExtension(currentFile);	
			std::string fileName = currentFile.substr(FolderPath.size(),currentFile.size()-FolderPath.size()-fileExtension.size()-1);

			if(fileExtension == "png"){
				LoadTexture(currentFile,fileName);		}

		}
		break;
	}
	case AssetType::TEXTUREATLAS: {
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {

			std::string currentFile = dir_entry.path().string();
			std::string fileExtension = Core::GetFileExtension(currentFile);	
			std::string fileName = currentFile.substr(FolderPath.size(),currentFile.size()-FolderPath.size()-fileExtension.size()-1);

			 if(fileExtension == "json")
				LoadTextureAtlas(currentFile,fileName);

		
		}
		break;

	}
	case AssetType::ANIMATION: {

		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
				std::string currentFile = dir_entry.path().string();

				std::string fileExtension = Core::GetFileExtension(currentFile);
				std::string fileName = currentFile.substr(FolderPath.size(),currentFile.size()-FolderPath.size()-fileExtension.size()-1);
				
				if (fileExtension != "json")
					continue;
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
	case AssetType::SHADER:{
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ FolderPath }) {
			std::string currentFile = dir_entry.path().string();
			std::string fileExtension = Core::GetFileExtension(currentFile);	
			std::string fileName = currentFile.substr(FolderPath.size(),currentFile.size()-FolderPath.size()-fileExtension.size()-1);

			if(Core::IsShaderExtension(fileExtension)){
				LoadShader(currentFile,fileName);
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

void AssetManager::LoadShader(const std::string& filePath,const std::string& fileName){
	auto it = m_Resources.find(fileName);
	if(it != m_Resources.end())
		return;
	Shader* shader = new Shader(filePath,Application::GetRenderer()->GetContext()->Device);
	LoadAssetPerma<Shader>(shader,AssetType::SHADER,fileName);
	
}
	
void AssetManager::DebugStatistics(bool GUI){
	if(!GUI){
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
				Core::Log("TEXTUREATLAS ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
				case AssetType::ANIMATION:{
				Core::Log("Animation ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
				case AssetType::SHADER:{
				Core::Log("Shader ID{",handle.ID.ID,"}"+debugStuff);

				break;
			}
			default:{
				Core::Log("Not implemented or invalid type{",(uint32_t)handle.GetType(),"}"+debugStuff);
				break;
			}
		}
		
	}
	}else{
		GUIRenderer* gui = Application::GetGUIRenderer();
		std::string buffer{};

		for(auto it = m_Resources.begin();it != m_Resources.end();it++){
			buffer +=it->second.AssetPath+"\n";
		}
		gui->InputText("RESOURCES",buffer.data(),buffer.size(),{0.0f,0.0f},{0.5f,0.25f},true);





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
	if(it != m_Resources.end()){
			return;
	}
	std::string TexturePath{};
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
	
	

		Animator* animator= new Animator(filePath,atlasGUUID, TextureID,&m_APP->m_AssetManager);
		if(!animator)
			return;
		animator->SetStage("IDLE");
	
		LoadAssetPerma<Animator>(animator,AssetType::ANIMATION,fileName);
}
void AssetManager::Shutdown(){
	for(uint32_t i=0 ; i < m_PermaAssets.size();i++){
		auto it =m_Resources.find(m_PermaAssets[i]);
		if(it != m_Resources.end())
			m_Resources[m_PermaAssets[i]].FreePermaHandle();
	}
	for(auto it = m_Resources.begin();it != m_Resources.end();it++){
	
		#ifdef DEBUG
				if(it->second.RefCount >0)
					Core::Log("Resources isn't deleted.",it->second.RefCount ," ",(uint32_t)it->second.Type,"} ",it->second.AssetPath);
		#endif
		
	}
	#ifdef DEBUG
	 m_ShutDown = true;
	 #endif
	m_Resources.clear();
}
AssetManager::~AssetManager(){
	#ifdef DEBUG
	if(!m_ShutDown)
		Core::Log("You needed to shutdown the manager manually");
	#endif

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


