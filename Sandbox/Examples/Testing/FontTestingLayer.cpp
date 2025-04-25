#include "FontTestingLayer.h"
#include "DefaultCameraControlls.h"
FontTestingLayer::FontTestingLayer(): Layer("FontTestingLayer")
{
}

void FontTestingLayer::OnCreate()
{
	m_App->m_GUIRenderer->SetFontSize(20.0f);
	//m_App->m_AssetManager.LoadAsset(fontatlas, AssetType::TEXTURE, "FONTAtlas");

	//m_App->m_Renderer->SetCurrentFont(fontatlas);
}

void FontTestingLayer::OnUpdate(float DeltaTime)
{
	DefaultCameraControlls(&m_App->m_InputSystem, &m_App->m_Camera);

	m_App->m_Renderer->DrawQuad({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, 0);


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

	StyleBorderData props{};
	props.BorderWidth = 0.0005f;
	props.BackGroundColor = { 0.2f,0.2f,0.2f,1.0f };
	props.BorderColor = { 1.0f,0.0f,.9f,1.0f };
	props.BackGroundTexture = nullptr;
	props.BorderTexture = nullptr;

	//	m_FontSystem->Text("Play", "PLAY", { 0.0f,0.5f });
		//m_FontSystem->Text("Options", "OPTIONS", { -0.1f,0.4f });
		//m_FontSystem->Text("Exit", "EXIT", { 0.0f,0.3f });

		//m_FontSystem->PushStyle(Style::DrawBorder);
	m_App->m_GUIRenderer->Text("FontSize", std::to_string(m_App->m_GUIRenderer->GetFontSize()), { -0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.05f });
	//m_App->m_GUIRenderer->Button("makt", "GALVA MAKT YRA", { 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f });
	static float number{ 0.1f };
	static float FontSize{ 16.f };

	GUI::ColorStyle ColorStyle{ sizeof(GUI::ColorStyle),{1.0f,0.0f,0.0f,1.0f} };
	GUI::BorderStyle BorderStyle{sizeof(GUI::BorderStyle)};
	BorderStyle.BorderColor = { 0.0f,0.0f,1.0f,1.0f };
	BorderStyle.BorderWidth = 0.007f;

	m_App->m_GUIRenderer->PushStyle(GUI::Style::BORDER, &BorderStyle);
	m_App->m_GUIRenderer->PushStyle(GUI::Style::COLOR, &ColorStyle);
	m_App->m_GUIRenderer->Slider("Slider", &FontSize, { 0.0f,0.0f }, { 0.1f,0.1f }, 1.0f, 1);
	m_App->m_GUIRenderer->SetFontSize(FontSize);
	m_App->m_GUIRenderer->InputText("Mkat", Buffer, ARRAYSIZE(Buffer), { 0.0f, -0.5f },{0.5f,0.1f});
	m_App->m_GUIRenderer->PopStyle();
	m_App->m_GUIRenderer->Slider("Slider1", &FontSize, { 0.5f,0.0f }, { 0.1f,0.1f }, 1.0f, 1);
	m_App->m_GUIRenderer->Slider("ldwad", &FontSize, { 0.5f,0.5f }, { 0.1f,0.1f }, 1.0f, 1);
	m_App->m_GUIRenderer->PopStyle();

	//m_FontSystem->PopStyle();

	//m_App->m_Renderer->DrawOutline({ 0.45f,0 .525f }, { 0.5f,0.05f }, { 1.0f,1.0f,1.0f,1.0f }, 0.001f);
}

void FontTestingLayer::OnDestroy()
{
}
