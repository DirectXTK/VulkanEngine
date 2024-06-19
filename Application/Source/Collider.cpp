#include "Collider.h"



Collider::Collider(Float2* Position, Float2* Size):m_Position(Position),m_Size(Size)
{
}

void Collider::Update(Float2 MoveAmount)
{
	m_MoveAmount = MoveAmount;
}
void CollisionSystem::CheckCollisions()
{
	for (uint32_t i = 0; i < m_Colliders.size(); i++) {
		//if (m_Colliders[0].GetPosition().x + (m_Colliders[0].GetSize().x * 2) < m_Colliders[1].GetPosition().x) {
		//	m_Colliders[0].SetPosition({ m_Colliders[1].GetPosition().x + (m_Colliders[0].GetSize().x * 2),m_Colliders[0].GetPosition().y });
		//}
		if (m_Colliders[0].GetPosition().y - (m_Colliders[0].GetSize().y * 2) < m_Colliders[1].GetPosition().y&& m_Colliders[0].GetPosition().x - (m_Colliders[0].GetSize().x * 2) < m_Colliders[1].GetPosition().x) {
			m_Colliders[0].SetPosition({ m_Colliders[0].GetPosition().x,  m_Colliders[1].GetPosition().y + (m_Colliders[0].GetSize().y * 2) });
			
		}
	}
}
