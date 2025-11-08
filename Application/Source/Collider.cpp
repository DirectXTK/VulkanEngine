#include "Collider.h"
#include "Application.h"
#define TILESIZE 0.04f
		void Async(std::vector<ColliderBackEnd>* collid,int offset,int size){
			std::vector<ColliderBackEnd>& colliders = *collid;

			for(uint32_t i=offset ;i < size;i++){
					colliders[i].Collided(false);
					for(uint32_t j=i+1;j < size;j++){
						bool result = Core::DefaultCollisionFunction(&colliders[i],&colliders[j]);
						if(result){
							colliders[i].Collided(result);
							colliders[j].Collided(result);

						}
					}
				}
		}
		void ColliderSystem::RunCollisions(){

			Async(&m_Colliders,0,m_Colliders.size());

			for(uint32_t i =0;i < m_Colliders.size();i++)
				Core::DefaultMovementFucntion(&m_Colliders[i]);
		}
	





		Collider ColliderSystem::CreateCollider(const Float2& position,const Float2& size){
			m_Colliders.emplace_back(position,size);
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
		//if acceleration is zero then a a bit to move the troops.

		if(collider1->GetPosition().x + (collider1->GetSize().x*2) <collider2->GetPosition().x ||
			collider1->GetPosition().x > collider2->GetPosition().x + (collider2->GetSize().x*2))
			return false;
		if(collider1->GetPosition().y + (collider1->GetSize().y*2) <collider2->GetPosition().y ||
			collider1->GetPosition().y > collider2->GetPosition().y + (collider2->GetSize().y*2))
			return false;
		//calculate acceleration.The smallest obj get pushed.
		
		Float2 pos1 = collider1->GetPosition();
		Float2 pos2 = collider2->GetPosition();
		
		Float2 Dis = {pos1.x-pos2.x,pos1.y-pos2.y};

		if(Dis.x ==0.0f || Dis.y ==0.0f){
			pos1.x += 0.001f*Core::RandomInt32(-1,1);
			pos1.y += 0.001f*Core::RandomInt32(-1,1);

		}

		float Dir = (float)Core::RandomInt32(-1,1);
		float Dir2 = (float)Core::RandomInt32(-1,1);

		pos1.x +=(Dis.x*0.033f);
		pos1.y +=(Dis.y*0.033f);

		pos2.x -=(Dis.x*0.033f);
		pos2.y -=(Dis.y*0.033f);
		

		collider1->SetPosition(pos1);
		collider2->SetPosition(pos2);

		return true;
	}
	void DefaultMovementFucntion(ColliderBackEnd* collider1){
		//how much its deaccelerates when its stops being pushed.
		
	}
}




