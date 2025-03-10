#include "FontTestingLayer.h"

FontTestingLayer::FontTestingLayer(): Layer("FontTestingLayer")
{
}

void FontTestingLayer::OnCreate()
{
	m_FontSystem = new FontSystem(m_App);
	m_FontSystem->SetCharcterSize( 96.f*16);
	Texture* fontatlas = m_FontSystem->GetFontAtlas();

	m_App->m_AssetManager.LoadResource(fontatlas, ResourceType::TEXTURE, "FONTAtlas");

	m_App->m_Renderer->SetCurrentFont(fontatlas);
}

void FontTestingLayer::OnUpdate(float DeltaTime)
{

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

	BorderProperties props{};
	props.BorderWidth=0.01f;
	props.BackGroundColor = { 0.2f,0.2f,0.2f,1.0f };
	props.BackGroundTexture = nullptr;
	props.BorderTexture = nullptr;

	m_FontSystem->PushStyle(Style::DrawBorder, &props);
	m_FontSystem->InputText("", Buffer, 255, { -0.5f,0.0f }, { 1.1f,0.8f });
	m_FontSystem->PopStyle();
	//m_App->m_Renderer->DrawOutline({ 0.45f,0.525f }, { 0.5f,0.05f }, { 1.0f,1.0f,1.0f,1.0f }, 0.001f);
}

void FontTestingLayer::OnDestroy()
{
}
