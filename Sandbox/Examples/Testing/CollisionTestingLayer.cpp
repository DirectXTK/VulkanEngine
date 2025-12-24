#include "CollisionTestingLayer.h"

CollisionLayer::CollisionLayer():Layer("CollisionLayer"){

}	

void CollisionLayer::OnCreate(){

		m_Units.push_back(Unit());
		m_Units.push_back(Unit());
		
		Float2 size = {0.01f,0.01f};

		m_Units[0].Size = size;
		m_Units[0].Pos = {0.2f,0.0f};
		m_Units[0].Path = Application::CreatePathAgent(m_Units[0].Pos, m_Units[0].Size,AgentType::LAND );

		m_Units[1].Size = size;
		m_Units[1].Pos = {0.8f,0.4f};
		m_Units[1].Path = Application::CreatePathAgent(m_Units[1].Pos, m_Units[1].Size,AgentType::LAND );

		for(uint32_t i =m_Units.size() ;i < 10;i++)
		{
				m_Units.push_back(Unit());
				m_Units[i].Size = size;
				m_Units[i].Pos = {0.7f,0.080f*i};
				m_Units[i].Path = Application::CreatePathAgent(m_Units[i].Pos, m_Units[i].Size,AgentType::LAND );

		}

}
void CollisionLayer::OnUpdate(double deltaTime){

	Unit* unit =&m_Units[0];
	Unit* goal =&m_Units[1];
	

	goal->Pos.x +=m_Move.x;
	goal->Pos.y +=m_Move.y;

	MoveUnit(*unit);
	unit->Path.Update(unit->Pos,unit->Size);
	goal->Path.Update(goal->Pos,goal->Size);
	Application::RunCollisionAsync(m_Units.data(),offsetof(Unit,Pos),offsetof(Unit,Size),m_Units.size(),sizeof(Unit),offsetof(Unit,Collided));

	if(unit->PathToGoal.empty()){
	std::vector<Float2> path =  unit->Path.GetPathToObj(goal->Pos);
	Core::Log("FoundPath");
	if(path.size() != 0){

		unit->PathToGoal = path;

	}
	}
}
void CollisionLayer::MoveUnit( Unit& unit){

	if(unit.PathToGoal.empty())
		return;

	float MoveAmount{0.0025f};
	Float2 path = unit.PathToGoal[unit.CurrentPath];
	Float2 diff{};

	if(std::fabs(unit.Pos.x - path.x) <=MoveAmount&&std::fabs(unit.Pos.y - path.y) <=MoveAmount){
		unit.CurrentPath++;
		Core::Log("Same",path,unit.Pos);
		if(unit.CurrentPath == unit.PathToGoal.size())
		{
			unit.CurrentPath =0;
			unit.PathToGoal.clear();
		}
	}
	
Core::Log("PAth",path);
	//Core::Log("Force",unit.Path.MoveObject({MoveAmount,MoveAmount},path));
	unit.Pos += unit.Path.MoveObject({MoveAmount,MoveAmount},path);
	
}

void CollisionLayer::OnEvent(Event& event){
	if(event.GetEventType()== EventType::KEYBOARD)
		OnKeyBoardEvent((KeyBoardEvent&)event);
	}
void CollisionLayer::OnKeyBoardEvent(KeyBoardEvent& event){
	float MoveAmount{0.0008f*Application::GetDeltaTime()};

	if(event.Key == KeyCodes::P && event.State == EventState::HOLD){
		m_Units.push_back(Unit());
		Float2 size = {0.01f,0.01f};
		m_Units[m_Units.size()-1].Size = size;
		m_Units[m_Units.size()-1].Pos = {0.5f,0.5f};
	}
	if(event.Key == KeyCodes::A && event.State == EventState::RELEASED){
		m_Move.x = 0.0f;
	}
	if(event.Key == KeyCodes::D && event.State == EventState::RELEASED){
		m_Move.x = 0.0f;

	}
	if(event.Key == KeyCodes::W && event.State == EventState::RELEASED){
		m_Move.y = 0.0f;

	}
	if(event.Key == KeyCodes::S && event.State == EventState::RELEASED){
		m_Move.y = 0.0f;

	} 


	if(event.Key == KeyCodes::A && event.State == EventState::PRESSED){
		m_Move.x = -MoveAmount;
	}
	if(event.Key == KeyCodes::D && event.State == EventState::PRESSED){
		m_Move.x = MoveAmount;

	}
	if(event.Key == KeyCodes::W && event.State == EventState::PRESSED){
		m_Move.y = MoveAmount;

	}
	if(event.Key == KeyCodes::S && event.State == EventState::PRESSED){
		m_Move.y = -MoveAmount;

	} 

}
void CollisionLayer::OnRender(double deltaTime){
	Renderer* renderer = Application::GetRenderer();
	for(uint32_t i=0 ;i < m_Units.size();i++){
		if(m_Units[i].Collided)
		renderer->DrawQuad({m_Units[i].Pos.x,m_Units[i].Pos.y,0.0f},{1.0f,0.0f,0.0f,1.0f},m_Units[i].Size,0);
		else
		renderer->DrawQuad({m_Units[i].Pos.x,m_Units[i].Pos.y,0.0f},{0.0f,1.0f,0.0f,1.0f},m_Units[i].Size,0);

	}
	
}
void CollisionLayer::OnGUI(){

}
void CollisionLayer::OnDestroy(){

}