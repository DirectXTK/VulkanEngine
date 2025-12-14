#include "GUITesting.h"
#include "Serializer.h"
GUI::SliderStyle sliderdata{};

GUITestingLayer::GUITestingLayer() : Layer("GUITestingLayer")
{

}
void GUITestingLayer::OnCreate() {
	sliderdata.FillOn = true;
	sliderdata.StructSize = sizeof(GUI::SliderStyle);
	sliderdata.FillColor = {0.0f,0.0f,1.0f,1.0f};

	auto asset = Application::GetAsset<Texture>("FontTexture46");
	if(asset){
		asset.GetData()->WriteToFile("OutputTexture.png");
	}
}
void GUITestingLayer::OnUpdate(double deltatime)
{
   Renderer* renderer = Application::GetRenderer();

	namespace fs = std::filesystem;

	static double lastmod1{};
	static double lastmod2{};
	std::string file1="EngineResources/Shaders/Pixel.fragS";
	std::string file2="EngineResources/Shaders/Vertex.vertS";


	auto ftime = fs::last_write_time(file1);

	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now()+std::chrono::system_clock::now());

	std::time_t lastmodified= std::chrono::system_clock::to_time_t(sctp);



}
void GUITestingLayer::OnRender(double deltime){

}
void GUITestingLayer::OnEvent(Event& event){
	if(event.GetEventType() == EventType::MOUSE){
		OnMouseEvent((MouseEvent&)event);
	}
	if(event.GetEventType() == EventType::KEYBOARD){
		OnKeyBoardEvent((KeyBoardEvent&)event);
	}
}
void GUITestingLayer::OnKeyBoardEvent(KeyBoardEvent& event){

}
void GUITestingLayer::OnMouseEvent(MouseEvent& event){


}


void GUITestingLayer::OnGUI()
{
	GUIRenderer* guiRenderer = Application::GetGUIRenderer();
	
	static float lafa{46.0f};
	guiRenderer->Panel("Panel", {0.0f,-0.9f}, {1.0f,0.5f,0.5f,1.0f}, {1.0f,0.1f}, 0, false);

	//guiRenderer->Button("Start Button","", { 0.0f,0.5f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.5f,0.5f },MouseCodes::LEFT,0,false);
//	guiRenderer->CheckBox("test",{-0.8f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});
	//guiRenderer->CheckBox("test1",{-0.6f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});
	//guiRenderer->CheckBox("test2",{-0.4f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});
	//guiRenderer->Quad({0.0f,0.0f},{0.2f,0.5f},{0.0f,0.0f,1.0f,1.0f});
	guiRenderer->PushStyle(GUI::Style::SLIDER,&sliderdata);
	guiRenderer->SetFontSize((uint32_t)lafa);
	guiRenderer->Slider("SLi",&lafa,{0.0f,0.0f},{0.2f,0.5f},1.0f,{-500.0f,500.0f});
	guiRenderer->Text("ID","TEXT",{0.9f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.5f});
	guiRenderer->SetFontSize(12);
	guiRenderer->Text("S","SMALL",{0.9f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.5f});

	guiRenderer->PopStyle();
	guiRenderer->EndPanel();

}

void GUITestingLayer::OnDestroy()
{
}



