#include "AnimationTestingLayer.h"

AnimationTestingLayer::AnimationTestingLayer():Layer("AnimationTestingLayer")
{
}

void AnimationTestingLayer::OnCreate()
{
	//m_Animation = m_App->GetAssetManager()->GetResource<Animator>(Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.json"));
	//m_Animation->SetStage("BBZ");
	m_App->LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Animation\\", ResourceType::ANIMATION);
	m_App->LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Textures\\", ResourceType::TEXTURE);


	m_Units.push_back(AnimationUnit());
	m_Units.push_back(AnimationUnit());


	m_Units[0].Position = {0.0f,0.0f};
	m_Units[0].Animator = *m_App->GetResource<Animator>("PEASANT");
	m_Units[0].Animator.SetStage("WALK");

	m_Units[1].Position = { -0.5f,0.0f };
	m_Units[1].Animator = *m_App->GetResource<Animator>("TOWN_HALL");
	m_Units[1].Animator.SetStage("IDLE");

}

void AnimationTestingLayer::OnUpdate(double DeltaTime)
{
	Renderer* renderer = m_App->m_Renderer;

	MoveUnit();
	//GUUID id = Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.png");


	for (uint32_t i = 0; i < m_Units.size(); i++) {

	renderer->DrawQuad({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.2f,0.2f}, m_Units[i].Animator, m_Units[i].ID.ID);


	m_Units[i].Animator.Update(DeltaTime);
	}
	//renderer->DrawQuad({ 0.0f,-0.5f }, { 1.0f,1.0f,1.0f,1.0 }, { 0.3f,0.3f }, Core::GetStringHash("PANEL.png"),0,0);

	

	DefaultCameraControlls(&m_App->m_InputSystem, &m_App->m_Camera);
}

void AnimationTestingLayer::OnDestroy()
{
}

void AnimationTestingLayer::OnGUI()
{
	GUIRenderer* gui = m_App->m_GUIRenderer;
	
	if (m_App->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, false) ) {
		m_CurrentlySelectedUnit = m_App->GetCurrentlyHoveredPixelID();
		if (m_SpawnUnit) {
			m_Units.push_back(AnimationUnit());
			m_Units[m_Units.size() - 1].Position = { m_App->GetWorldMousePos().x, m_App->GetWorldMousePos().y };
			m_Units[m_Units.size() - 1].Animator = *m_App->GetResource<Animator>(m_SpawnedUnit);
			m_SpawnUnit = false;
		}
	}
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
	gui->Panel({0.0f,-0.8f},{1.0f,1.0f,1.0f,1.0},{1.0f,0.2f},Core::GetStringHash("PANEL"));

	if (gui->Button({ 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"),false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TOWN_HALL";
	}
	if (gui->Button({ 0.25f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TREE";

	}

	gui->EndPanel();
	
	

}

void AnimationTestingLayer::MoveUnit()
{
}
