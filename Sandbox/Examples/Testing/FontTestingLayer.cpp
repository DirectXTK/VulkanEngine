#include "FontTestingLayer.h"
#include "DefaultCameraControlls.h"
FontTestingLayer::FontTestingLayer(): Layer("FontTestingLayer")
{
}

void FontTestingLayer::OnCreate()
{
	m_FontSystem = new FontSystem(m_App);
	m_FontSystem->SetCharcterSize( 36.f);
	Texture* fontatlas = m_FontSystem->GetFontAtlas();

	//m_App->m_AssetManager.LoadAsset(fontatlas, AssetType::TEXTURE, "FONTAtlas");

	//m_App->m_Renderer->SetCurrentFont(fontatlas);
}

void FontTestingLayer::OnUpdate(float DeltaTime)
{
	DefaultCameraControlls(&m_App->m_InputSystem, &m_App->m_Camera);

	m_App->m_Renderer->DrawQuad({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, 0);

	StyleBorderData data{};

	m_FontSystem->PushStyle(Style::DrawBorder, &data);
	m_FontSystem->Text("cub", "KUBAS", { 0.0f,-0.2 });
	m_FontSystem->PopStyle();
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
	static Float2 Pos{  -0.99f,-0.99f  };

	StyleBorderData props{};
	props.BorderWidth=0.01f;
	props.BackGroundColor = { 0.2f,0.2f,0.2f,1.0f };
	props.BorderColor = { 0.0f,0.0f,.9f,1.0f };
	props.BackGroundTexture = nullptr;
	props.BorderTexture = nullptr;

	m_FontSystem->Text("Play", "PLAY", { 0.0f,0.5f });
	m_FontSystem->Text("Options", "OPTIONS", { -0.1f,0.4f });
	m_FontSystem->Text("Exit", "EXIT", { 0.0f,0.3f });

	m_FontSystem->PushStyle(Style::DrawBorder);



	m_FontSystem->InputText("", Buffer, 255, Pos, { 1.0f,0.1f });
	m_FontSystem->PopStyle();

	//m_App->m_Renderer->DrawOutline({ 0.45f,0.525f }, { 0.5f,0.05f }, { 1.0f,1.0f,1.0f,1.0f }, 0.001f);
}

void FontTestingLayer::OnDestroy()
{
}
