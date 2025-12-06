#include "CollisionTestingLayer.h"

CollisionLayer::CollisionLayer():Layer("CollisionLayer"){

}	

void CollisionLayer::OnCreate(){

		m_Units.push_back(Unit());
		m_Units.push_back(Unit());
		
		Float2 size = {0.01f,0.01f};


		for(uint32_t i =0 ;i < 20;i++)
		{
				m_Units.push_back(Unit());
				m_Units[i].Size = size;
				m_Units[i].Pos = {0.5f,0.5f};

		}

}
void CollisionLayer::OnUpdate(double deltaTime){

	Unit* unit =&m_Units[0];
	unit->Pos.x +=m_Move.x;
	unit->Pos.y +=m_Move.y;

	Application::RunCollisionAsync(m_Units.data(),offsetof(Unit,Pos),offsetof(Unit,Size),m_Units.size(),sizeof(Unit),offsetof(Unit,Collided));
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