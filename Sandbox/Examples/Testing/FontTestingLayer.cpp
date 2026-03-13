#include "FontTestingLayer.h"
#include "DefaultCameraControlls.h"
FontTestingLayer::FontTestingLayer(): Layer("FontTestingLayer")
{
}

void FontTestingLayer::OnCreate()
{
	GUIRenderer* guiRenderer= Application::GetGUIRenderer();
	guiRenderer->SetFontSize(46.0f);
	//Application::m_AssetManager.LoadAsset(fontatlas, AssetType::TEXTURE, "FONTAtlas");

	//Application::m_Renderer->SetCurrentFont(fontatlas);
}

void FontTestingLayer::OnUpdate(double DeltaTime)
{
	Renderer* renderer = Application::GetRenderer();
	DefaultCameraControlls(Application::GetCurrentCamera());

	for(uint32_t i=0;i < 1000;i++)
		renderer->DrawQuad({ 0.01f*i,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, 0);

}
void FontTestingLayer::OnRender(double deltime){

}
void FontTestingLayer::OnGUI()
{
	static char Buffer[255];
	static bool Init{ true };
	if (Init) {
		Init = false;
		Buffer[0] = 'A';
		Buffer[1] = 'A';
		Buffer[2] = 'A';
		Buffer[3] = 'A';


	}
	static Float2 Pos{ -0.99f,-0.99f };
	Renderer* renderer = Application::GetRenderer();
	GUIRenderer* guiRenderer = Application::GetGUIRenderer();

	guiRenderer->Text("FontSize", std::to_string(guiRenderer->GetFontSize()), { -0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.05f });
	//guiRenderer->Button("makt", "GALVA MAKT YRA", { 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f });
	static float number{ 0.1f };
	static float FontSize{ 46.f };

	GUI::ColorStyle ColorStyle{ sizeof(GUI::ColorStyle),{1.0f,0.0f,0.0f,1.0f} };
	GUI::BorderStyle BorderStyle{sizeof(GUI::BorderStyle)};
	GUI::SliderStyle sliderStyle{sizeof(GUI::SliderStyle),true,{0.0f,1.0f,0.0f,1.0f}};


	BorderStyle.BorderColor = { 0.0f,0.0f,1.0f,1.0f };
	BorderStyle.BackGroundColor = { 0.0f,0.0f,0.7f,1.0f };
	BorderStyle.BorderWidth = 0.007f;
	guiRenderer->SetFontSize(FontSize);

	guiRenderer->Text("FrameTime",std::to_string(Application::GetDeltaTime()),{-0.8f,-0.8f},{0.0f,0.0f,0.0f,1.0f},{0.1f,0.1f});

	guiRenderer->PushStyle(GUI::Style::BORDER, &BorderStyle);
	guiRenderer->PushStyle(GUI::Style::COLOR, &ColorStyle);
	guiRenderer->Slider("Slider", &FontSize, { 0.0f,0.0f },{1.0f,1.0f,1.0f,1.0f}, { 0.1f,0.1f }, 1.0f,{0.0f,100.f},1);
	guiRenderer->InputText("Mkat", Buffer, ARRAYSIZE(Buffer), { 0.0f, -0.5f },{0.5f,0.1f});
	guiRenderer->PopStyle();
	
	
	guiRenderer->PushStyle(GUI::Style::SLIDER,&sliderStyle);
	guiRenderer->Slider("Slider1", &FontSize, { 0.5f,0.0f },{1.0f,1.0f,1.0f,1.0f}, { 0.1f,0.1f }, 0.01f, {0.0f,100.f});
	guiRenderer->Slider("ldwad", &FontSize, { 0.5f,0.5f },{1.0f,1.0f,1.0f,1.0f}, { 0.1f,0.1f }, 1.0f, {0.0f,100.f});
	guiRenderer->PopStyle();
	
	static float TestNumber{ -1.0f };
	guiRenderer->Slider("Test123", &TestNumber, { 0.0f,0.5f },{1.0f,1.0f,1.0f,1.0f}, { 0.1f,0.1f }, 1.0f, {0.0f,100.f});
	guiRenderer->Text("Number",std::to_string(Application::GetAssetCount(AssetType::FONT)),{-0.5f,0.5f},{0.0f,0.0f,0.0f,1.0f},{0.1f,0.1f});
	guiRenderer->Text("Number",std::to_string(Application::GetAssetCount(AssetType::TEXTURE)),{-0.5f,0.8f},{0.0f,0.0f,0.1f,1.0f},{0.1f,0.1f});

	//guiRenderer->Text("Number",std::to_string(Application::m_AssetManager.GetAsset<Font>(Core::GetStringHash("Font46")).GetRefCount()),{-0.5f,0.4f},{0.0f,0.0f,0.0f,1.0f},{0.1f,0.1f});

	guiRenderer->PopStyle();

	//m_FontSystem->PopStyle();

	//Application::m_Renderer->DrawOutline({ 0.45f,0 .525f }, { 0.5f,0.05f }, { 1.0f,1.0f,1.0f,1.0f }, 0.001f);
}

void FontTestingLayer::OnDestroy()
{
}
