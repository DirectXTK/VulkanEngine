#include "FontTestingLayer.h"

FontTestingLayer::FontTestingLayer(): Layer("FontTestingLayer")
{
}

void FontTestingLayer::OnCreate()
{
	m_FontSystem = new FontSystem(m_App);
	m_FontSystem->SetCharcterSize( 96.f);
	Texture* fontatlas = m_FontSystem->GetFontAtlas();

	m_App->m_AssetManager.LoadResource(fontatlas, ResourceType::TEXTURE, "FONTAtlas");

	m_App->m_Renderer->SetCurrentFont(fontatlas);
}

void FontTestingLayer::OnUpdate(float DeltaTime)
{
	Float2 BoundingBox[4] = {
		{0.5f,0.5f},
		{0.5f,0.54f},
		{0.9f,0.54f},
		{0.9f,0.5f}
	};

	m_App->m_Renderer->RenderText("OPA!! LAFA", { 0.5f,0.5f }, BoundingBox,0.1f);
}

void FontTestingLayer::OnGUI()
{
}

void FontTestingLayer::OnDestroy()
{
}
