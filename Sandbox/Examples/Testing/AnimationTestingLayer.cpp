#include "AnimationTestingLayer.h"

AnimationTestingLayer::AnimationTestingLayer():Layer("AnimationTestingLayer")
{
}

void AnimationTestingLayer::OnCreate()
{
	//m_Animation = m_App->GetAssetManager()->GetResource<Animator>(Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.json"));
	//m_Animation->SetStage("BBZ");
	m_Units.push_back(Unit());
	m_Units.push_back(Unit());


	m_Units[0].Position = {0.0f,0.0f};
	m_Units[0].Animator = *m_App->GetResource<Animator>("C:\\Repos\\VulkanEngine\\Resources\\Animation\\PEASANT.json");
	m_Units[0].Animator.SetStage("IDLE");

	m_Units[1].Position = { -0.5f,0.0f };
	m_Units[1].Animator = *m_App->GetResource<Animator>("C:\\Repos\\VulkanEngine\\Resources\\Animation\\SUN.json");
	m_Units[1].Animator.SetStage("MOVE");


}

void AnimationTestingLayer::OnUpdate(double DeltaTime)
{
	Renderer* renderer = m_App->m_Renderer;

	MoveUnit();
	//GUUID id = Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.png");


	for (uint32_t i = 0; i < m_Units.size(); i++) {

	renderer->DrawQuad({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.2f,0.2f}, m_Units[i].Animator.GetCurrentTextureID(), m_Units[i].ID.ID, m_Units[i].Animator.GetTextureIndex());
	m_Units[i].Animator.Update(DeltaTime);
	}
	
}

void AnimationTestingLayer::OnDestroy()
{
}

void AnimationTestingLayer::OnGUI()
{
	GUIRenderer* gui = m_App->m_GUIRenderer;

	
	if (m_CurrentlySelectedUnit != 0) {
		//make a map istead of vector
		for (uint32_t i = 0; i < m_Units.size(); i++)
		{
			if (m_Units[i].ID == m_CurrentlySelectedUnit) {
				if (gui->Button({ 0.0f,0.0f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, 0, false)) {
						m_Units[i].Animator.SetStage("IDLE");

				}
				if (gui->Button({ 0.3f,0.0f }, { 0.0f,1.0f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, 0, false)) {
					m_Units[i].Animator.SetStage("WALK");


				}
				break;
			}

		}
	}
	if (m_App->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, false)) {
		m_CurrentlySelectedUnit = m_App->GetCurrentlyHoveredPixelID();
		Core::Log(ErrorType::Info, m_CurrentlySelectedUnit.ID);
		Core::Log(ErrorType::Info, m_App->GetMousePos().x, m_App->GetMousePos().y);
	}
	

}

void AnimationTestingLayer::MoveUnit()
{
}
