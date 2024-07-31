#pragma once
#include "AppCore.h"
class Collider;
class CollisionSystem;
class ColliderBackEnd
{
public:
	ColliderBackEnd() {}
	void Update(Float2* Position, Float2* Size);
	void UpdateMoveAmount(Float2 MoveAmount);
	void SetStatus(bool Colided) { m_IsColided = Colided; }
	bool IsColided() { return m_IsColided; }

	Float2 GetMoveAmount() { return m_MoveAmount; }
	Float2 GetPosition() { return *m_Position; }
	Float2 GetSize() { return *m_Size; }

	void SetPosition(const Float2& Pos) { *m_Position = Pos; }
private:
	Float2* m_Position{};
	Float2* m_Size{};
	Float2 m_MoveAmount{ 0.0f };
	bool m_ColliderOn{true};
	bool m_IsColided{};
};
class Collider {
public:
	Collider() {}
	Collider(CollisionSystem* system, GUUID ID);

	void Update(Float2* Position, Float2* Size);
	void UpdateMoveAmount(Float2 MoveAmount);
	bool IsCollided();

	GUUID GetID() { return m_ID; }
private:
	CollisionSystem* m_System{};
	GUUID m_ID{ 0 };
};

class CollisionSystem {
public:
	ColliderBackEnd* GetColliderBackEnd(GUUID ID) { return &m_Colliders[ID]; }
	void CheckCollisions();
	Collider CreateCollider() { GUUID id = GUUID(); m_Colliders[id] = ColliderBackEnd(); return Collider(this, id); }
private:
	std::unordered_map<GUUID, ColliderBackEnd> m_Colliders{};
	Float2 m_OnCollisionMovePercent{ 0.00023f,0.0001f };
};



