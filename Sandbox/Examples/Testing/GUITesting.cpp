#include "GUITesting.h"
#include "Serializer.h"
#include "DefaultCameraControlls.h"
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
	Application::LoadAllAssets("/users/jimy/Repos/VulkanEngine/EngineResources/Examples/Animation/",AssetType::ANIMATION);

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
struct FireParticleData{
	float SwivelAmount{0.0005f};
	float SwivelDirection{1.0f};
	float CurrentTime{};
};
void ParticleFire( ParticleProps& props){
	const float timeToSwivelInOneDir{SEC(2.4f)};
	FireParticleData* fireData = (FireParticleData*)props.CustomData;

	fireData->CurrentTime-=Application::GetDeltaTime();
	if(fireData->CurrentTime <=0.0f){
		fireData->CurrentTime = Core::RandomFloat(1.0f,timeToSwivelInOneDir);
		fireData->SwivelDirection*=-1.0f;
	}

	props.Pos.y +=Core::RandomFloat(0.0f,0.001f);
	props.Pos.x += Core::RandomFloat(0.0f,fireData->SwivelAmount)*fireData->SwivelDirection;

}
void ParticleVelocityAdd(ParticleProps& props){
	const float velSlow{0.997f}; 

	props.Pos.x += props.Velocity.x;
	props.Pos.y += props.Velocity.y;

	props.Velocity.x *=velSlow;
	props.Velocity.y *=velSlow;
}
void GUITestingLayer::OnUpdate(double deltatime)
{
	Renderer* renderer = Application::GetRenderer();

	//DefaultCameraControlls(Application::GetCurrentCamera());
	static Float2 pos{0.0f,0.0f};

	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
	renderer->DrawQuad({pos.x,pos.y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);

	if(Application::IsKeyPressed(KeyCodes::ARROWRIGHT))
	{
		pos.x+= 0.01f;
	}	if(Application::IsKeyPressed(KeyCodes::ARROWLEFT))
	{
		pos.x-= 0.01f;
	}

		if(Application::IsKeyPressed(KeyCodes::ARROWUP))
	{
		pos.y+= 0.01f;
	}	if(Application::IsKeyPressed(KeyCodes::ARROWDOWN))
	{
		pos.y-= 0.01f;
	}


   if(animator)
   {
   		renderer->DrawQuad({0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},animator,0);
   		animator.Update(deltatime);
   }
	if(m_SpawnParticles&&m_SpawningEnabled){
		ParticleProps prop{};
		prop.LifeTime = Core::RandomFloat(SEC(3.3f),SEC(8.0f));
		//prop.LifeTime = SEC(3.f);
		prop.Color = {1.0f,1.0f,1.0f,1.0f};
		prop.Pos = Application::GetWorldMousePos();
		prop.Size = {0.012f,0.015f};
		prop.ID = GUUID();
		prop.CustomFunction= ParticleVelocityAdd;
		prop.Animation = *Application::GetAsset<Animator>("FireAnim").GetData();
		prop.Animation.SetStage("BURN");
		prop.TextureID = Core::GetStringHash("Particles/Water");
		for(uint32_t i=0;i < 10;i++){
		prop.Velocity.x = Core::RandomFloat(-0.005f,0.005f);
		prop.Velocity.y = Core::RandomFloat(-0.005f,0.0050f);
		Application::GetParticleSystem().DrawParticle(prop);

		}

			for(uint32_t i=0;i < 10;i++){
		prop.Velocity.x = Core::RandomFloat(-0.0015f,0.0010f);
		prop.Velocity.y = Core::RandomFloat(-0.0015f,0.0010f);
		prop.Size = {0.012f,0.012f};
		prop.CustomFunction = ParticleFire;
		FireParticleData* data = (FireParticleData*)malloc(sizeof(FireParticleData));
		*data = FireParticleData();
		prop.CustomData = data;
		prop.TextureID = Core::GetStringHash("Particles/Fire");
		Application::GetParticleSystem().DrawParticle(prop);

		}
	}
 

}
void GUITestingLayer::OnRender(double deltime){
   Render* renderer = Application::GetRender();

   renderer->DrawQuad({0.0f,-0.5f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0);
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
	Render* renderer= Application::GetRender();
	if(event.State == EventState::PRESSED){
	if(event.Key == KeyCodes::P){
		auto frag = Application::GetAsset<Shader>("Shaders/ParticleF");
   	if(frag)
  		renderer->SetShader(frag);

		auto vert= Application::GetAsset<Shader>("Shaders/ParticleV");
  	 if(vert)
   		renderer->SetShader(vert);
	}
	}
	if(event.State == EventState::PRESSED){
	if(event.Key == KeyCodes::O){
		auto frag = Application::GetAsset<Shader>("Shaders/DefaultPixel");
   		if(frag)
  		renderer->SetShader(frag);

		auto vert= Application::GetAsset<Shader>("Shaders/DefaultVertex");
  	 	if(vert)
   		renderer->SetShader(vert);
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
	GUIRenderer* gui = Application::GetGUIRenderer();
	

	static Float4 panelColor{1.0f,1.0f,1.0f,1.0f};
	static Float4 borderColor{1.0f,1.0f,1.0f,1.0f};
	static char username[50]{};
	static float timeHovered{};

	gui->SetFontSize((uint32_t)12);

	Application::GetRenderer()->ChangeArrowColor({1.0f,0.0f,0.0f,1.0f});
	GUI::BorderStyle border{};
	border.BorderColor = borderColor;
	border.BorderWidth = 0.01f;
	border.DrawBorder = true;
	border.BackGroundColor = {0.5f,0.5f,0.5f,1.0f};
	
	gui->PushStyle(GUI::Style::BORDER,&border);
	gui->Panel("Panel",{0.0f,0.0f},panelColor,{0.3f,0.3f},0,true);

	gui->Slider("PanelColorr",&panelColor.r,{-0.5f,0.8f},{1.0,1.0f,0.0f,1.0f},{0.2f,0.1f},0.01f,{0.0f,1.0f});
	gui->Slider("PanelColorg",&panelColor.g,{-0.5f,0.6f},{1.0,1.0f,0.0f,1.0f},{0.2f,0.1f},0.01f,{0.0f,1.0f});
	gui->Slider("PanelColorb",&panelColor.b,{-0.5f,0.4f},{1.0,1.0f,0.0f,1.0f},{0.2f,0.1f},0.01f,{0.0f,1.0f});

	gui->Slider("Borderr",&borderColor.r,{-0.1f,0.8f},{1.0,1.0f,0.3f,1.0f},{0.2f,0.1f},0.01f,{0.0f,1.0f});
	gui->Slider("Borderg",&borderColor.g,{-0.1f,0.6f},{1.0,1.0f,0.3f,1.0f},{0.2f,0.1f},0.01f,{0.0f,1.0f});
	gui->Slider("Borderb",&borderColor.b,{-0.1f,0.4f},{1.0,1.0f,0.3f,1.0f},{0.2f,0.1f},0.01f,{0.0f,1.0f});
	//15219296786176995281 this font is missing and causes flickering !!!!
	
	gui->SetFont("EngineResources/Fonts/JetBrainsMono-Bold.ttf");
	gui->InputText("UserName",username,ARRAYSIZE(username),{0.0f,-0.5f},{0.5f,0.2f},true);
	
	gui->PopStyle();

	//Wierd placement when using multuple tooltips.
	if(gui->IsObjectHovered("Check") || timeHovered >SEC(1.0f)){
			timeHovered+=Application::GetDeltaTime();
		gui->Tooltip("SPAWNS FIREPARTICLES",{0.1f,0.1f},{0.0f,0.2f},{1.0f,0.0f,0.0f,0.5f},{0.0f,1.0f,0.0f,0.3f},Core::GetStringHash("CheckTooltip"));

	}
	if(gui->IsObjectHovered("CheckTooltip")){
		gui->Tooltip("AND WATER PARTICLES",{0.3f,0.1f},{0.0f,0.5f},{1.0f,0.0f,0.0f,0.5f},{0.0f,1.0f,0.0f,0.3f});
	}

	if(gui->CheckBox("Check",{0.5f,0.0f},{0.2f,0.2f},{1.0f,1.0f,1.0f,1.0f}))
		m_SpawningEnabled = true;
	else 
		m_SpawningEnabled = false;
	//gui->SetFont("EngineResources/Fonts/Daydream.ttf");
	gui->EndPanel();



	//Application::GetAssetManager()->DebugStatistics(true);

}

void GUITestingLayer::OnDestroy()
{
}



