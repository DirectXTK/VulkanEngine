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
		Buffer[0] = 'Z';
		Buffer[1] = 'I';
		Buffer[2] = 'N';
		Buffer[3] = 'O';

		
	}

	m_FontSystem->InputText("", Buffer, 255, { 0.0f,0.0f }, { 0.5f,0.1f }, 0);
	//m_App->m_Renderer->DrawOutline({ 0.45f,0.525f }, { 0.5f,0.05f }, { 1.0f,1.0f,1.0f,1.0f }, 0.001f);
}

void FontTestingLayer::OnDestroy()
{
}
