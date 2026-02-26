#include "GUITesting.h"
#include "Serializer.h"
GUI::SliderStyle sliderdata{};

Animator animator{};
GUITestingLayer::GUITestingLayer() : Layer("GUITestingLayer")
{

}
void GUITestingLayer::OnCreate() {
	sliderdata.FillOn = true;
	sliderdata.StructSize = sizeof(GUI::SliderStyle);
	sliderdata.FillColor = {0.0f,0.0f,1.0f,1.0f};

	Application::LoadAllAssets("/users/jimy/Repos/VulkanEngine/Resources/",AssetType::NONE);

	  Asset<Animator> asset= Application::GetAsset<Animator>("PEASawdaawdANT");
		if(asset){
			animator = *asset.GetData();
			animator.SetStage("WALK");
		}


	auto asset2 = Application::GetAsset<Texture>("FontTexture"+std::to_string(Application::GetGUIRenderer()->GetFontSize()));
	if(asset2){
		asset2.GetData()->WriteToFile("OutputTexture.png");
	}
}
void GUITestingLayer::OnUpdate(double deltatime)
{
   Renderer* renderer = Application::GetRenderer();

   if(animator)
   {
   		renderer->DrawQuad({0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},animator,0);
   		animator.Update(deltatime);
   }
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
   Renderer* renderer = Application::GetRenderer();

   //renderer->DrawQuad({0.1f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f},Core::GetStringHash("GUI/CheckBoxTrue"),0);
  // renderer->DrawQuad({0.2f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f},Core::GetStringHash("GUI/CheckBoxFalse"),0);
   //renderer->DrawQuad({0.3f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f},Core::GetStringHash("Textures/Examples/BLOOD_KNIGHT"),0);
     Vertex vertex[4]{};
   vertex[0].Position = {0.0f,0.0f,0.0f};
   vertex[1].Position = {0.0f,1.0f,0.0f};
   vertex[2].Position = {1.0f,1.0f,0.0f};
   vertex[3].Position = {1.0f,0.0f,0.0f};

   vertex[0].Color = {1.0f,1.0f,1.0f,1.0f};
   vertex[1].Color = {1.0f,1.0f,1.0f,1.0f};
   vertex[2].Color = {1.0f,1.0f,1.0f,1.0f};
   vertex[3].Color = {1.0f,1.0f,1.0f,1.0f};

   vertex[0].TextureID = 0;
   vertex[1].TextureID = 0;
   vertex[2].TextureID = 0;
   vertex[3].TextureID = 0;

   vertex[0].TexCoords = { 0.0f,1.0f };
   vertex[1].TexCoords = { 0.0f,0.0f };
   vertex[2].TexCoords = { 1.0f,0.0f };
   vertex[3].TexCoords = { 1.0f,1.0f };

	renderer->DrawVertices(vertex,4);
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
	
	static float FontSize{16.0f};
	static char username[50];
	static char pass[50];


	guiRenderer->SetFontSize((uint32_t)16);
	guiRenderer->SetFont("EngineResources/Fonts/Daydream.ttf");

	Application::GetRenderer()->ChangeArrowColor({1.0f,0.0f,0.0f,1.0f});
	guiRenderer->Panel("Panel", {0.0f,-0.9f}, {0.0f,0.5f,0.5f,1.0f}, {1.0f,0.1f}, 0, true);
	guiRenderer->Slider("FONTSIZE",&FontSize,{-0.5f,-0.5f},{0.5f,0.2f},0.1f,{0.0f,64.f});
	guiRenderer->InputText("USERNAME",username,ARRAYSIZE(username),{0.0f,0.0f},{1.0f,1.0f});

	guiRenderer->InputText("PASS",pass,ARRAYSIZE(pass),{-0.5f,0.0f},{0.2f,1.0f});
	guiRenderer->Quad({0.0f,0.7f},{0.2f,0.9f},{0.5f,0.5f,1.0f,1.0f});
	guiRenderer->SetFontSize((uint32_t)FontSize);

	guiRenderer->Button("Start Button","", { 0.0f,0.5f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.5f,0.5f },MouseCodes::LEFT,0,false);

	//guiRenderer->PushStyle(GUI::Style::SLIDER,&sliderdata);
	guiRenderer->Text("ID","ATlafatafa!",{-0.5f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.5f});
	guiRenderer->SetFontSize((uint32_t)4);
	
	guiRenderer->CheckBox("test",{-0.8f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});
	guiRenderer->Text("S","\"TES TING.\"",{0.5f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.5f});
	guiRenderer->SetFontSize((uint32_t)16);

	//guiRenderer->PopStyle();
	guiRenderer->EndPanel();
	guiRenderer->SetFont("EngineResources/Fonts/Sacrifice.ttf");

	guiRenderer->Panel("P2s",{0.0f,0.0f},{0.0f,0.0f,1.0f,1.0f},{0.5f,0.2f},0,true);
	guiRenderer->InputText("USERNA2ME",username,ARRAYSIZE(username),{0.0f,0.0f},{1.0f,1.0f});
	guiRenderer->EndPanel();
	guiRenderer->SetFont("EngineResources/Fonts/Daydream.ttf");

	guiRenderer->Panel("TEST",{0.0f,0.0f},{0.0f,1.0f,0.0f,1.0f},{0.6f,0.2f},0,true);
	guiRenderer->Quad({0.5f,0.0f},{1.1f,0.9f},{1.0f,1.0f,1.0f,1.0f},0);
	guiRenderer->EndPanel();


}

void GUITestingLayer::OnDestroy()
{
}



