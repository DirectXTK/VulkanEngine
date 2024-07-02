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

	m_App->m_Camera.SetPosition({ 0.0f,0.0f });

	m_Units[0].Position = {0.0f,0.0f};
	m_Units[0].Animator = *m_App->GetResource<Animator>("PEASANT");
	m_Units[0].Animator.SetStage("WALK");
	m_Units[0].Collid = m_System.CreateCollider();
	m_Units[0].Collid.Update(&m_Units[0].Position, &m_Size);


	m_Units[1].Position = { -0.5f,0.0f };
	m_Units[1].Animator = *m_App->GetResource<Animator>("TOWN_HALL");
	m_Units[1].Animator.SetStage("IDLE");
	m_Units[1].Collid = m_System.CreateCollider();
	m_Units[1].Collid.Update(&m_Units[1].Position, &m_Size);


}

void AnimationTestingLayer::OnUpdate(double DeltaTime)
{
	Renderer* renderer = m_App->m_Renderer;

	//GUUID id = Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.png");
	MoveUnit();

	Vertex vertices[2];
	vertices[0].Position = {0.0f,0.0f};
	vertices[1].Position = { 0.5f,0.0f };
	bool Outline = true;

	for (uint32_t i = 0; i < m_Units.size(); i++) {
	m_Units[i].Collid.Update(&m_Units[i].Position,&m_Size);

	if (m_CurrentlySelectedUnit == m_Units[i].ID)
	renderer->DrawOutline({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f }, m_Size, 0.01f);
	renderer->DrawQuad({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f}, {1.0f,1.0f,1.0f,1.0f}, m_Size, m_Units[i].Animator, m_Units[i].ID.ID);
	


	m_Units[i].Animator.Update(DeltaTime);
	}

	//renderer->DrawQuad({ 0.0f,-0.5f }, { 1.0f,1.0f,1.0f,1.0 }, { 0.3f,0.3f }, Core::GetStringHash("PANEL.png"),0,0);

	m_System.CheckCollisions();


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
			m_Units[m_Units.size() - 1].Collid = m_System.CreateCollider();

			m_SpawnUnit = false;
		}
	}
	if (m_CurrentlySelectedUnit != 0&& false) {
		//make a map istead of vector
	
		
				if (gui->Button("IDLE", {0.0f,0.0f}, {1.0f,0.0f,0.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, 0, false)) {
						m_Units[0].Animator.SetStage("IDLE");
						m_CurrentlySelectedUnit = 0;

				}
				if (gui->Button("WALK", {0.3f,0.0f}, {0.0f,1.0f,0.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, 0, false)) {
					m_Units[0].Animator.SetStage("WALK");
					m_CurrentlySelectedUnit = 0;


				}

		
	}
	gui->Panel("UI Bar", {0.0f,-0.8f}, {1.0f,1.0f,1.0f,1.0}, {1.0f,0.2f}, Core::GetStringHash("PANEL"));

	if (gui->Button("TOWN_HALL", {0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TOWN_HALL";
	}
	if (gui->Button("TREE", { 0.25f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TREE";

	}
	if (gui->Button("SUN", {0.50f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "SUN";

	}
	if (gui->Button("TEST", {0.75f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TEST";

	}

	gui->EndPanel();
	
	

}

void AnimationTestingLayer::MoveUnit()
{
	Float2 MoveAmount{};
	if (m_CurrentlySelectedUnit.ID == m_Units[0].ID.ID) {

		m_Units[0].MoveLocation = { m_App->GetWorldMousePos().x,m_App->GetWorldMousePos().y };

	}
	if (m_Units[0].Position.x < m_Units[0].MoveLocation.x) {
		m_Units[0].Position.x += m_Units[0].MoveSpeed;
		MoveAmount.x = m_Units[0].MoveSpeed;
	}
	else {
		m_Units[0].Position.x -= m_Units[0].MoveSpeed;
		MoveAmount.x = m_Units[0].MoveSpeed*-1.0f;

	}
	if (m_Units[0].Position.y < m_Units[0].MoveLocation.y) {
		m_Units[0].Position.y += m_Units[0].MoveSpeed;
		MoveAmount.y = m_Units[0].MoveSpeed;


	}
	else {
		m_Units[0].Position.y -= m_Units[0].MoveSpeed;
		MoveAmount.y = m_Units[0].MoveSpeed*-1.0f;

	}
	m_Units[0].Collid.UpdateMoveAmount(MoveAmount);
}
