#include "DLLLoadingLayer.h"
DLLLoadingLayer::DLLLoadingLayer() : Layer("DLLLoadingLayer")
{

}
void DLLLoadingLayer::OnCreate() {
	m_Loader.Init("/users/jimy/Repos/DLLProgram/DLLProject/bin/Debug/libDLLProject.so");
	auto func = m_Loader.LoadFunction<void(*)(void)>("SimpleLafa");
	func();

	m_DllAdder= m_Loader.LoadFunction<int(*)(int,int)>("Added");
	int a =m_DllAdder(15,15);
	Core::Log("Adder result",a);
	a =m_DllAdder(15,30);
	Core::Log("Adder result",a);

	std::string MyName = m_Loader.GetVariable<std::string>("g_Name");
	Core::Log("MYNAME IS ",MyName);
	/*

	
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
	*/
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
