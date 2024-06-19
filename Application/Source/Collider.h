#pragma once
#include "AppCore.h"
class Collider
{
public:
	Collider(Float2* Position, Float2* Size);
	void Update(Float2 MoveAmount);
	void SetStatus(bool Colided) { m_IsColided = Colided; }
	bool IsColided() { return m_IsColided; }

	Float2 GetMoveAmount() { return m_MoveAmount; }
	Float2 GetPosition() { return *m_Position; }
	Float2 GetSize() { return *m_Size; }

	void SetPosition(const Float2& Pos) { *m_Position = Pos; }
private:
	Float2* m_Position{};
	Float2* m_Size{};
	Float2 m_MoveAmount{};
	bool m_IsColided{};
};
class CollisionSystem {
public:

	void CheckCollisions();
	Collider* CreateCollider(Float2* Position,Float2* Size) { m_Colliders.push_back(Collider(Position,Size)); return &m_Colliders[m_Colliders.size()-1]; }
private:
	std::vector<Collider> m_Colliders{};
};


