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

   Renderer* renderer = Application::GetRenderer();


}
void GUITestingLayer::OnUpdate(double deltatime)
{
   Renderer* renderer = Application::GetRenderer();
   if(animator)
   {
   		renderer->DrawQuad({0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},animator,0);
   		animator.Update(deltatime);
   }
	if(m_SpawnParticles){
		ParticleProps prop{};
		prop.LifeTime = Core::RandomFloat(SEC(0.0f),SEC(4.5f));
		//prop.LifeTime = SEC(3.f);
		prop.Color = {1.0f,1.0f,1.0f,1.0f};
		prop.Pos = Application::GetWorldMousePos();
		prop.Size = {0.009f,0.009f};
		prop.TextureID = Core::GetStringHash("Particles/Water");
		for(uint32_t i=0;i < 10;i++){
		prop.Velocity.x = Core::RandomFloat(-0.0005f,0.0005f);
		prop.Velocity.y = Core::RandomFloat(0.0001f,0.0010f);
		Application::GetParticleSystem().DrawParticle(prop);

		}
			for(uint32_t i=0;i < 10;i++){
		prop.Velocity.x = Core::RandomFloat(-0.0015f,0.0010f);
		prop.Velocity.y = Core::RandomFloat(-0.0015f,0.0010f);
		prop.TextureID = Core::GetStringHash("Particles/Fire");
		Application::GetParticleSystem().DrawParticle(prop);

		}
	}
 

}
void GUITestingLayer::OnRender(double deltime){
   Renderer* renderer = Application::GetRenderer();

	//renderer->DrawParticle(Application::GetWorldMousePos(),{0.9f,0.1f,0.1f,1.0f},{0.01f,0.01f},0);
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
	Renderer* renderer= Application::GetRenderer();
	if(event.State == EventState::PRESSED){
	if(event.Key == KeyCodes::P){
		auto frag = Application::GetAsset<Shader>("Shaders/ParticleF");
   	if(frag)
  		renderer->QueueShaderChange(frag);

		auto vert= Application::GetAsset<Shader>("Shaders/ParticleV");
  	 if(vert)
   		renderer->QueueShaderChange(vert);
	}
	}
	if(event.State == EventState::PRESSED){
	if(event.Key == KeyCodes::O){
		auto frag = Application::GetAsset<Shader>("Shaders/DefaultPixel");
   		if(frag)
  		renderer->QueueShaderChange(frag);

		auto vert= Application::GetAsset<Shader>("Shaders/DefaultVertex");
  	 	if(vert)
   		renderer->QueueShaderChange(vert);
	}
	}
}
void GUITestingLayer::OnMouseEvent(MouseEvent& event){
	if(event.Code == MouseCodes::LEFT&& event.State == EventState::PRESSED){
		m_SpawnParticles = true;
	}
	if(event.Code == MouseCodes::LEFT&& event.State == EventState::RELEASED){
		m_SpawnParticles = false;

	}


	

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



