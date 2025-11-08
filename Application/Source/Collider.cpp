#include "Collider.h"
#include "Application.h"
#define TILESIZE 0.04f

		void ColliderSystem::RunCollisions(){
				for(uint32_t i=0 ;i < m_Colliders.size();i++){
					m_Colliders[i].Collided(false);
					for(uint32_t j=i+1;j < m_Colliders.size();j++){
						bool result = Core::DefaultCollisionFunction(&m_Colliders[i],&m_Colliders[j]);
						if(result){
							m_Colliders[i].Collided(result);
							m_Colliders[j].Collided(result);

						}
					}
					Core::DefaultMovementFucntion(&m_Colliders[i]);
				}
		}
	





		Collider ColliderSystem::CreateCollider(const Float2& position,const Float2& size){
			m_Colliders.emplace_back(position,size);
			Core::Log("SOze",m_Colliders.size());
			return Collider(m_Colliders.size()-1);
		}
		bool Collider::IsCollided(){
			ColliderBackEnd* collider = Application::GetCollider(m_ID);
			if(!collider)
				Core::Log(ErrorType::Error,"forgot to create collider or something went wrong.");
			return collider->IsCollided();
		}

		void Collider::SetPosition(const Float2& position){
			ColliderBackEnd* collider = Application::GetCollider(m_ID);
			collider->SetPosition(position);
			}
		void Collider::SetSize(const Float2& size){
			ColliderBackEnd* collider = Application::GetCollider(m_ID);
			collider->SetSize(size);
		}
		
		void Collider::SetAcceleration(const Float2& aceeleration){
			ColliderBackEnd* collider = Application::GetCollider(m_ID);
			collider->SetAcceleration(aceeleration);
		}


		Float2 Collider::GetPosition(){
				ColliderBackEnd* collider = Application::GetCollider(m_ID);
			return collider->GetPosition();
		}
		Float2 Collider::GetSize(){
				ColliderBackEnd* collider = Application::GetCollider(m_ID);
			return collider->GetSize();
		}
		Float2 Collider::GetAcceleration(){
				ColliderBackEnd* collider = Application::GetCollider(m_ID);
			return collider->GetAcceleration();
		}

namespace Core{
	bool DefaultCollisionFunction(ColliderBackEnd* collider1,ColliderBackEnd* collider2){
	

		if(collider1->GetPosition().x + collider1->GetSize().x*2 <collider2->GetPosition().x ||
			collider1->GetPosition().x > collider2->GetPosition().x + collider2->GetSize().x*2)
			return false;
		if(collider1->GetPosition().y + collider2->GetSize().y*2 <collider2->GetPosition().y ||
			collider1->GetPosition().y > collider1->GetPosition().y + collider2->GetSize().y*2)
			return false;
		//calculate acceleration.
		collider1->SetAcceleration({0.00001f,0.0f});

		return true;
	}
	void DefaultMovementFucntion(ColliderBackEnd* collider1){
		//how much its deaccelerates when its stops being pushed.
		const float deacceleration{0.001f};
		Float2 pos = collider1->GetPosition();
		Float2 acce = collider1->GetAcceleration();

		pos += acce;
		acce = {acce.x-(acce.x * deacceleration),acce.y-(acce.y *deacceleration)};

		collider1->SetAcceleration(acce);
		collider1->SetPosition(pos);
	}
}




