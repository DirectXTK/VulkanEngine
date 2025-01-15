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
	
	m_FontSystem->RenderText("ZYNO", { 0.5f,0.5f }, { 0.2f,0.1f });
}

void FontTestingLayer::OnGUI()
{
}

void FontTestingLayer::OnDestroy()
{
}
