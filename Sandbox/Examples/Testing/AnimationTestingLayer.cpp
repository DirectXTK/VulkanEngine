#include "AnimationTestingLayer.h"

AnimationTestingLayer::AnimationTestingLayer():Layer("AnimationTestingLayer")
{
}

void AnimationTestingLayer::OnCreate()
{
	m_Animation = m_App->GetAssetManager()->GetResource<Animator>(Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\WaterFire.json"));
	m_Animation->SetStage("Water");
}

void AnimationTestingLayer::OnUpdate(double DeltaTime)
{
	Renderer* renderer = m_App->m_Renderer;

	GUUID id = Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\WaterFire.png");
	renderer->DrawQuad({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, id,0, m_Animation->GetTextureIndex());
	m_Animation->Update(DeltaTime);
}

void AnimationTestingLayer::OnDestroy()
{
}

void AnimationTestingLayer::OnGUI()
{
	GUIRenderer* gui = m_App->m_GUIRenderer;
	if (gui->Button({ 0.0f,0.0f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, 0, false)) {
		m_Animation->SetStage("Water");

	}
	if (gui->Button({ 0.3f,0.0f }, { 0.0f,1.0f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, 0, false)) {
		m_Animation->SetStage("Fire");

	}
}
