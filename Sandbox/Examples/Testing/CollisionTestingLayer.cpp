#include "CollisionTestingLayer.h"

CollisionTestingLayer::CollisionTestingLayer(): Layer("CollisionTestingLayer")
{
}

void CollisionTestingLayer::OnCreate()
{
	m_Colliders.push_back({ {0.0f,0.0f} });
	m_Colliders.push_back({ {0.7f,0.0f} });

	m_Colliders[0].collider = m_App->CreateCollider(&m_Colliders[0].Position,&m_Size);
	m_Colliders[1].collider = m_App->CreateCollider(&m_Colliders[1].Position, &m_Size);

}

void CollisionTestingLayer::OnUpdate(double Deltatime)
{
	for (uint32_t i = 0; i < m_Colliders.size(); i++) {
		m_App->m_Renderer->DrawQuad({ m_Colliders[i].Position.x,m_Colliders[i].Position.y,0.0f }, m_Colliders[i].Color, { 0.01f ,0.01f }, 0);
	}
}

void CollisionTestingLayer::OnDestroy()
{
}

void CollisionTestingLayer::OnGUI()
{
}
