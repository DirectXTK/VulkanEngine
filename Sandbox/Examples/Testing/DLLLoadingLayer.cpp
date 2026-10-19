#include "DLLLoadingLayer.h"
#include "DLLLoading.h"
DLLLoadingLayer::DLLLoadingLayer() : Layer("DLLLoadingLayer")
{

}
void DLLLoadingLayer::OnCreate() {


	Application::GetAssetManager()->LoadAllAssets("Resources/DLL/",AssetType::DLL);

	auto asset = Application::GetAsset<DLLLoading>("libDLLProject");
	if(asset){
		auto fn =asset.GetData()->LoadFunction<void(*)(void)>("SimpleLafa");

		if(fn)
			fn();
		else	
			Core::Log(ErrorType::Error,"Failed to run function");
	}


	Application::GetAssetManager()->UnloadAsset((uint64_t)14021025719309637068);
	Application::GetAssetManager()->UnloadAsset((uint64_t)966090126753073597);
}	
void DLLLoadingLayer::OnRender(double deltime){


	Application::GetRender()->DrawQuad({0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f},0);
}
void DLLLoadingLayer::OnUpdate(double deltatime)
{


}

void DLLLoadingLayer::OnGUI()
{
	
}

void DLLLoadingLayer::OnDestroy()
{
}
