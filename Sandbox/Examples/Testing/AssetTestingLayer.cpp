#include "AssetTestingLayer.h"
#include "DLLLoading.h"
AssetTestingLayer::AssetTestingLayer():Layer("AssetTestingLayer"){

}
void PizdaMne123(void* lafa){
    Core::Log("VSIO NX");
}
void AssetTestingLayer::OnCreate(){
    {
    Core::DLLLazy loader{};
    Application::GetAssetManager()->LoadAsset<Core::DLL>("/users/jimy/Repos/DLLProgram/DLLProject/bin/Debug/BLEC.so","DJ");

    auto asset2 = Application::GetAssetManager()->GetAsset<Core::DLL>("DJ23");
    if(asset2){
    auto GetString = asset2.GetData()->LoadFunction<std::string(*)()>("GetTestString");
    if(GetString){
        std::string output = GetString();
        Core::Log(output);
    }

    Application::GetAssetManager()->LoadAllAssets("EngineResources/",AssetType::NONE);
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

}
void AssetTestingLayer::OnEvent(Event& event){
    if(event.GetEventType() == EventType::MOUSE)
        OnMouseMoveEvent((MouseEvent&)event);
}
void AssetTestingLayer::OnMouseMoveEvent(MouseEvent& event){
}
void AssetTestingLayer::OnUpdate(double DeltaTime){

}
void AssetTestingLayer::OnDestroy(){
    
}
void AssetTestingLayer::OnGUI(){
    
}
void AssetTestingLayer::OnRender(double deltime){
    
}