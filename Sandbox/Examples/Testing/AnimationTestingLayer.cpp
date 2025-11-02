#include "AnimationTestingLayer.h"

AnimationTestingLayer::AnimationTestingLayer():Layer("AnimationTestingLayer")
{
}
#define TILESIZE 0.04f

void AnimationTestingLayer::OnCreate()
{
	Application::LoadAssets("Resources/Animation/", AssetType::ANIMATION);
	Application::LoadAssets("Resources/Textures/", AssetType::TEXTURE);

	m_Units.reserve(1000);



	Application::GetCurrentCamera()->SetPosition({ 0.0f,-0.3f });


for(uint32_t i=0;i < 100;i++){
	Asset<Animator> animator = Application::GetAsset<Animator>("TOWN_HALL");
	if(!animator)
		continue;
	m_Units.push_back(AnimationUnit());

		m_Units[i].Position = { 0.04f*2.0f*i,0.0f };
	m_Units[i].animator = *animator.GetData();
	m_Units[i].animator.SetStage("IDLE");
	m_Units[i].Collid = m_System.CreateCollider();
	m_Units[i].Collid.Update(&m_Units[i].Position, &m_Size);

}
	
}
void AnimationTestingLayer::OnRender(double deltime){

}
void AnimationTestingLayer::OnUpdate(float DeltaTime)
{
	Renderer* renderer = Application::GetRenderer();
	m_PathGridTime -= DeltaTime;



	for (uint32_t i = 0; i < m_Units.size(); i++) {
	renderer->DrawQuad({ m_Units[i].Position.x,m_Units[i].Position.y,0.0f}, {1.0f,1.0f,1.0f,1.0f}, m_Size, m_Units[i].animator, m_Units[i].ID.ID);
	

	m_Units[i].animator.Update(DeltaTime);
	}



	DefaultCameraControlls(Application::GetCurrentCamera());
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
	GUIRenderer* gui = Application::GetGUIRenderer();
	
		RendererDesc desc{};
		desc.Rendermode = RenderMode::SOLID;
		Application::GetRenderer()->SetRenderDesc(desc);

	if (Application::IsMouseClicked(MouseCodes::LEFT, true) ) {
		m_CurrentlySelectedUnit = Application::GetCurrentlyHoveredPixelID();
		if (m_SpawnUnit) {
			m_Units.push_back(AnimationUnit());
			m_Units[m_Units.size() - 1].Position = { Application::GetWorldMousePos().x, Application::GetWorldMousePos().y };
			if(!Application::GetAssetManager()->HasAsset(Core::GetStringHash(m_SpawnedUnit)))
				printf("Doesn't have %s",m_SpawnedUnit.c_str());
			m_Units[m_Units.size()-1].animator = *Application::GetAsset<Animator>(m_SpawnedUnit).GetData();
			m_Units[m_Units.size() - 1].Collid = m_System.CreateCollider();
			m_Units[m_Units.size() - 1].Collid.Update(&m_Units[m_Units.size() - 1].Position, &m_Size);


			m_SpawnUnit = false;
		}
	
	}

	if (m_CurrentlySelectedUnit != 0&& false) {
		//make a map istead of vector
	
		
				if (gui->Button("IDLE","", {0.0f,0.0f}, {1.0f,0.0f,0.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT)) {
						m_Units[0].animator.SetStage("IDLE");
						m_CurrentlySelectedUnit = 0;

				}
				if (gui->Button("WALK","", {0.3f,0.0f}, {0.0f,1.0f,0.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT)) {
					m_Units[0].animator.SetStage("WALK");
					m_CurrentlySelectedUnit = 0;


				}

		
	}

	



	gui->Panel("UI Bar", {0.0f,-0.8f}, {1.0f,1.0f,1.0f,1.0}, {1.0f,0.2f}, Core::GetStringHash("PANEL"));
	if (gui->Button("TOWN_HALL", "",{0.0f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT,0,true)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TOWN_HALL";
	}
	if (gui->Button("TREE", "",{ 0.25f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, Core::GetStringHash("GUI/SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "TREE";

	}
	GUI::OutlineStyle outline{sizeof(GUI::OutlineStyle)};
	outline.Color = {0.0f,0.0f,1.0f,1.0f};
	
	gui->PushStyle(GUI::Style::OUTLINE,&outline);
	if (gui->Button("SUN", "",{0.50f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI/SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "SUN";

	}
	gui->PopStyle();

	if (gui->Button("TEST", "",{0.75f,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI/SpawnButton"), false)) {
		m_SpawnUnit = true;
		m_SpawnedUnit = "PEASANT";

	}

	gui->EndPanel();
	
		if(gui->Button("Wireframe","",{0.8f,0.9f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},MouseCodes::LEFT,0,false)){
			desc.Rendermode = RenderMode::WIREFRAME;
			Application::GetRenderer()->SetRenderDesc(desc);
	}
	if(gui->Button("Solid","",{0.5f,0.9f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},MouseCodes::LEFT,0,false)){
			desc.Rendermode = RenderMode::SOLID;
			Application::GetRenderer()->SetRenderDesc(desc);
	}

}

void AnimationTestingLayer::MoveUnit(AnimationUnit* unit)
{
	Float2 CurrentMoveLocation{};
	Float2 MoveAmount{0.0f,0.0f};
	if (m_CurrentlySelectedUnit.ID == unit->ID) {

		if (Application::IsMouseClicked(MouseCodes::LEFT, false) ){
			Float2 Dest = { Application::GetWorldMousePos().x,Application::GetWorldMousePos().y };
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
