#include "CollisionTestingLayer.h"

	 CollisionLayer::CollisionLayer():Layer("CollisionLayer"){

	}	

	void CollisionLayer::OnCreate(){

		m_Units.push_back(Unit());
		m_Units.push_back(Unit());
		
		Float2 size = {0.01f,0.01f};


		m_Units[0].collider = Application::CreateCollider({0.0f,0.0f},size);
		m_Units[1].collider = Application::CreateCollider({0.0f,0.5f},size);

		for(uint32_t i =0 ;i < 32;i++)
			{
				m_Units.push_back(Unit());
				m_Units[m_Units.size()-1].collider = Application::CreateCollider({0.0f,0.0f},size);
			}
	}

		void CollisionLayer::OnUpdate(double deltaTime){
			Float2 pos =m_Units[1].collider.GetPosition();

			if(Application::IsKeyPressed(KeyCodes::S))
				pos.y -=0.0001f;
			if(Application::IsKeyPressed(KeyCodes::W))
				pos.y +=0.0001f;
			if(Application::IsKeyPressed(KeyCodes::A))
				pos.x -=0.0001f;
			if(Application::IsKeyPressed(KeyCodes::D))
				pos.x +=0.0001f;
			m_Units[1].collider.SetPosition(pos);
		}
		void CollisionLayer::OnRender(double deltaTime){
			Renderer* renderer = Application::GetRenderer();
 
			for(uint32_t i=0 ;i < m_Units.size();i++){
				renderer->DrawQuad({m_Units[i].collider.GetPosition().x,m_Units[i].collider.GetPosition().y,0.0f},{0.0f,1.0f,0.0f,1.0f},m_Units[i].collider.GetSize(),0);
			}
		}
		void CollisionLayer::OnGUI(){

		}
		void CollisionLayer::OnDestroy(){

		}