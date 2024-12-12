#include "FontTestingLayer.h"

FontTestingLayer::FontTestingLayer(): Layer("FontTestingLayer")
{
}

void FontTestingLayer::OnCreate()
{
}

void FontTestingLayer::OnUpdate(float DeltaTime)
{
	m_FontSystem.Run(m_App,m_App->m_Renderer);
}

void FontTestingLayer::OnGUI()
{
}

void FontTestingLayer::OnDestroy()
{
}
