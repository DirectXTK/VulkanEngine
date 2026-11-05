#include "AssetTestingLayer.h"
#include "DLLLoading.h"
AssetTestingLayer::AssetTestingLayer():Layer("AssetTestingLayer"){

}
void AssetTestingLayer::OnCreate(){
    {
    Application::GetAssetManager()->LoadAllAssets("EngineResources",AssetType::NONE);
    auto asset=Application::GetAsset<Core::DLL>("DLL/libDLLProject");
    auto str = asset.GetData()->GetVariable<std::string>("g_Name");
    Core::Log("g_Name from DLL is ",str);
    //Work here on unloading
    asset.UnloadAsset();

   
    }
     if(Application::HasAsset("DLL/libDLLProject")){
        Core::Log("Still here");
    }
}
void AssetTestingLayer::OnUpdate(double DeltaTime){
    
}
void AssetTestingLayer::OnDestroy(){
    
}
void AssetTestingLayer::OnGUI(){
    
}
void AssetTestingLayer::OnRender(double deltime){
    
}