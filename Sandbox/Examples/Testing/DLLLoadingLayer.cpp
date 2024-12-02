#include "DLLLoadingLayer.h"
#include "DLLLoading.h"
DLLLoadingLayer::DLLLoadingLayer() : Layer("DLLLoadingLayer")
{

}
void DLLLoadingLayer::OnCreate() {

	DLLLoading loader;
	loader.Init("C:\\Users\\jasiu\\source\\repos\\Testing\\x64\\Debug\\adwa\\DWADAWD\\Testing.dll");
	void(*Lafa)(int)  =loader.LoadFunction<void(*)(int)>("CreateLafa");
	Lafa(69);

}
void DLLLoadingLayer::OnUpdate(float deltatime)
{


}

void DLLLoadingLayer::OnGUI()
{
	
}

void DLLLoadingLayer::OnDestroy()
{
}
