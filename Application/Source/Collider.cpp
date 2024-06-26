#include "Collider.h"

float CalculateDistance(float First, float Second) {
	return std::fabs(First - Second);
}


Collider::Collider(CollisionSystem* system,GUUID ID):m_System(system),m_ID(ID)
{
}

void Collider::Update(Float2* Position, Float2* Size)
{
	m_System->GetColliderBackEnd(m_ID)->Update(Position, Size);
}

void Collider::UpdateMoveAmount(Float2 MoveAmount)
{
	m_System->GetColliderBackEnd(m_ID)->UpdateMoveAmount(MoveAmount);
}




void CollisionSystem::CheckCollisions()
{	
	uint32_t Offset{};
	for (auto i = m_Colliders.begin(); i != std::next(m_Colliders.end(),-1) ; i++) {
			ColliderBackEnd* FirstCollider = &i->second;
			Offset++;
		for (auto j = std::next(m_Colliders.begin(),Offset); j != m_Colliders.end(); j++) {

			ColliderBackEnd* SecondCollider = &j->second;
			
			float DistanceX = CalculateDistance(FirstCollider->GetPosition().x - FirstCollider->GetSize().x, SecondCollider->GetPosition().x - SecondCollider->GetSize().x);
			float MinSizeX = std::min(FirstCollider->GetSize().x, SecondCollider->GetSize().x) * 2;
			float DistanceY = CalculateDistance(FirstCollider->GetPosition().y - FirstCollider->GetSize().y, SecondCollider->GetPosition().y - SecondCollider->GetSize().y);
			float MinSizeY = std::min(FirstCollider->GetSize().y, SecondCollider->GetSize().y) * 2;
			if (DistanceX <= MinSizeX && DistanceY <= MinSizeY) {
				Float2 Position{ FirstCollider->GetPosition() };
				
					Position.x -= FirstCollider->GetMoveAmount().x;
					Position.y -= FirstCollider->GetMoveAmount().y;
				FirstCollider->SetPosition(Position);
			}
		}

	}
}






void ColliderBackEnd::Update(Float2* Position, Float2* Size)
{
	m_Position = Position;
	m_Size = Size;
}

void ColliderBackEnd::UpdateMoveAmount(Float2 MoveAmount)
{
	m_MoveAmount = MoveAmount;
}

