#include "AnimationTestingLayer.h"

AnimationTestingLayer::AnimationTestingLayer():Layer("AnimationTestingLayer")
{
}
#define TILESIZE 0.04f

void AnimationTestingLayer::OnCreate()
{
	//m_Animation = m_App->GetAssetManager()->GetResource<Animator>(Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.json"));
	//m_Animation->SetStage("BBZ");
	m_App->LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Animation\\", ResourceType::ANIMATION);
	m_App->LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Textures\\", ResourceType::TEXTURE);

	m_Units.reserve(10);
	m_Units.push_back(AnimationUnit());
	m_Units.push_back(AnimationUnit());

	m_App->m_Camera.SetPosition({ 0.0f,0.0f });

	m_Units[0].Position = {0.0f,0.0f};
	m_Units[0].Animator = *m_App->GetResource<Animator>("WARRIOR");
	m_Units[0].Animator.SetStage("WALK");
	m_Units[0].Collid = m_System.CreateCollider();
	m_Units[0].Collid.Update(&m_Units[0].Position, &m_Size);


	m_Units[1].Position = { 0.04f*5,0.0f };
	m_Units[1].Animator = *m_App->GetResource<Animator>("TOWN_HALL");
	m_Units[1].Animator.SetStage("IDLE");
	m_Units[1].Collid = m_System.CreateCollider();
	m_Units[1].Collid.Update(&m_Units[1].Position, &m_Size);


	//temp
	/*m_PathGrid.resize(50 * 50);
	for (uint32_t x = 0; x < 50; x++) {

		for (uint32_t y = 0; y < 50; y++) {

			m_PathGrid[y + (x * 50)].Position.x = x * (1 / 50.f);
			m_PathGrid[y + (x * 50)].Position.y = y * (1 / 50.f);

		}
	}
	*/

}

void AnimationTestingLayer::OnUpdate(float DeltaTime)
{
	Renderer* renderer = m_App->m_Renderer;

	//Core::Log(ErrorType::Info,m_App->GetWorldMousePos().x," ", m_App->GetWorldMousePos().y);
	//GUUID id = Core::GetStringHash("C:\\Repos\\VulkanEngine\\Resources\\Animation\\TEST.png");
	m_PathGridTime -= DeltaTime;


	Vertex vertices[2];
	vertices[0].Position = {0.0f,0.0f};
	vertices[1].Position = { 0.0f,0.0f };
	bool Outline = true;

	for (uint32_t i = 0; i < m_Units.size(); i++) {
	if(m_Units[i].Animator.GetAnimationID() == Core::GetStringHash("WARRIOR"))
		MoveUnit(&m_Units[i]);
	m_Units[i].Collid.Update(&m_Units[i].Position,&m_Size);

	if (m_Units[i].Collid.IsCollided())
		renderer->DrawOutline({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f },  m_Size,{0.0f,1.0f,0.0f,1.0f }, 0.003f);
	else
		renderer->DrawOutline({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f }, m_Size, {1.0f,1.0f,0.0f,1.0f}, 0.003f);

	renderer->DrawQuad({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f}, {1.0f,1.0f,1.0f,1.0f}, m_Size, m_Units[i].Animator, m_Units[i].ID.ID);
	

	m_Units[i].Animator.Update(DeltaTime);
	}
	if (m_PathGridTime > 0) {
		for (uint32_t i = 0; i < m_PathGrid.size(); i++) {
			renderer->DrawQuad({ m_PathGrid[i].Position.x,m_PathGrid[i].Position.y,1.0f }, { 1.0f,0.0f,0.0f,1.0f }, m_PathGrid[i].Size, 0);
		}
	}

	//renderer->DrawQuad({ 0.0f,-0.5f }, { 1.0f,1.0f,1.0f,1.0 }, { 0.3f,0.3f }, Core::GetStringHash("PANEL.png"),0,0);
	//uint32_t PathCount{};
	//Float2* loc =  m_Units[0].Collid.GetPathToObj(m_Units[0].Position, m_Units[1].Position,&PathCount);
	//m_System.CheckCollisions();

	DefaultCameraControlls(&m_App->m_InputSystem, &m_App->m_Camera);
}

void AnimationTestingLayer::OnDestroy()
{
}
int ConvertPositionToNodeIndexa(Float2 Position) {
	uint32_t X = (uint32_t)((1 + Position.x) / TILESIZE);
	uint32_t Y = (uint32_t)((1 - Position.y) / TILESIZE);
	return (Y * 50) + X;
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
			m_Units[m_Units.size() - 1].Collid.Update(&m_Units[m_Units.size() - 1].Position, &m_Size);


			m_SpawnUnit = false;
		}
	
		Core::Log(ErrorType::Info, ConvertPositionToNodeIndexa(m_App->GetWorldMousePos()));
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
		m_SpawnedUnit = "PEASANT";

	}

	gui->EndPanel();
	
	

}

void AnimationTestingLayer::MoveUnit(AnimationUnit* unit)
{
	Float2 CurrentMoveLocation{};
	Float2 MoveAmount{0.0f,0.0f};
	if (m_CurrentlySelectedUnit.ID == unit->ID) {

		if (m_App->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, false) ){
			Float2 Dest = { m_App->GetWorldMousePos().x,m_App->GetWorldMousePos().y };
			unit->MoveLocation = unit->Collid.GetPathToObj(unit->Position, Dest, &unit->MoveCellCount);

			if (unit->MoveCellCount != 0) {
				unit->Moving = true;
				//Core::Log(ErrorType::Info, "LastLocationOfPath:", unit->MoveLocation[unit->MoveCellCount - 1].x, " ", unit->MoveLocation[unit->MoveCellCount - 1].y);
				//Core::Log(ErrorType::Info, "Dest:", Dest.x, " ", Dest.y);
				Core::Log(ErrorType::Info, "StartLoc:",unit->Position.x," ", unit->Position.y);
				Core::Log(ErrorType::Info, "EndLoc:", unit->MoveLocation[unit->MoveCellCount-1].x, " ", unit->MoveLocation[unit->MoveCellCount-1].y);
				m_PathGrid.resize(unit->MoveCellCount);
				m_PathGridTime = SEC(3);
			}

		}
		for (uint32_t i = 0; i < unit->MoveCellCount; i++) {
			std::cout << unit->MoveLocation[i].x << " " << unit->MoveLocation[i].y << "\n";
			m_PathGrid[i] = { unit->MoveLocation[i] };
		}
		std::cout << std::endl;
	}
	if ( unit->MoveCellCount-1< unit->CurrentCellIndex) {
		unit->MoveCellCount = 0;
		unit->Moving = false;
		delete[] unit->MoveLocation;
		unit->MoveLocation = nullptr;
		return;
	}
	if (unit->MoveCellCount == 0) {
		unit->Moving = false;
		unit->CurrentCellIndex = 0;
		return;
	}
	CurrentMoveLocation = unit->MoveLocation[unit->CurrentCellIndex];
	if (CurrentMoveLocation == unit->Position ) {
		unit->CurrentCellIndex++;
		CurrentMoveLocation = unit->MoveLocation[unit->CurrentCellIndex];

	}

	if (unit->Moving) {
		float Difference = std::abs(unit->Position.x - CurrentMoveLocation.x);
		if (unit->MoveSpeed>= Difference) {
			unit->Position.x = CurrentMoveLocation.x;
			MoveAmount.x = Difference;

		}
		else if (unit->Position.x < CurrentMoveLocation.x) {
			unit->Position.x += unit->MoveSpeed;
			MoveAmount.x = unit->MoveSpeed;
		}
		else {
			unit->Position.x -= unit->MoveSpeed;
			MoveAmount.x = unit->MoveSpeed * -1.0f;

		}

		if (MoveAmount.x == 0.0f) {
			Difference = std::abs(unit->Position.y - CurrentMoveLocation.y);
			if (unit->MoveSpeed >= Difference) {
				unit->Position.y = CurrentMoveLocation.y;
				MoveAmount.y = Difference;

			}
			else if (unit->Position.y < CurrentMoveLocation.y) {
				unit->Position.y += unit->MoveSpeed;
				MoveAmount.y = unit->MoveSpeed;


			}
			else {
				unit->Position.y -= unit->MoveSpeed;
				MoveAmount.y = unit->MoveSpeed * -1.0f;

			}
		}

	}
	
	unit->Collid.UpdateMoveAmount(MoveAmount);

}
