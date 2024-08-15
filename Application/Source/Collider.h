#pragma once
#include "AppCore.h"

#define VULKAN_ENGINE_COLLISION_ON

class Collider;
class CollisionSystem;
struct Node {
	int NodeIndex{ -1 };
	float F{ FLT_MAX };
	float G{ FLT_MAX };
	float H{ FLT_MAX };
	int ParentNodeIndex{ -1 };
	friend bool operator<(const Node& l, const Node& r) {
		return l.F < r.F;
	}
};
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

	Float2 GetPathToObj(Float2 CurrentPos, Float2 FinalPath);

	GUUID GetID() { return m_ID; }
private:

	bool IsBlocked(int NodeIndex);

	CollisionSystem* m_System{};
	GUUID m_ID{ 0 };
};

class CollisionSystem {
public:
	CollisionSystem();
	ColliderBackEnd* GetColliderBackEnd(GUUID ID) { return &m_Colliders[ID]; }
	void CheckCollisions();
	Collider CreateCollider() { GUUID id = GUUID(); m_Colliders[id] = ColliderBackEnd(); return Collider(this, id); }

	Node* GetCellInfo() { return m_PathGrid; }
	bool IsBlocked(int NodeIndex);

private:
	std::unordered_map<GUUID, ColliderBackEnd> m_Colliders{};
	Float2 m_OnCollisionMovePercent{ 0.00023f,0.0001f };


	
	bool* m_BlockedNodeList{};
	Node* m_PathGrid{};
};



