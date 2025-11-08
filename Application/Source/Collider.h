#pragma once
#include "AppCore.h"

#define VULKAN_ENGINE_COLLISION_ON

class Collider;
struct Int2
{
	int x{};
	int y{};
	bool operator==(const Int2& rhs) {
		return rhs.x == x && rhs.y == y;
	}
	bool operator!=(const Int2& rhs) {
		return !(*this == rhs);
	}
};
struct Node {
	Float2 Position{};
	float F{ FLT_MAX };
	float G{ FLT_MAX };
	float H{ FLT_MAX };
	int64_t Index{};
	int64_t ParentNodeIndex{ -1 };
	friend bool operator<(const Node& l, const Node& r) {
		return l.F < r.F;
	}
};

class ColliderBackEnd{
	public:
		ColliderBackEnd(const Float2& position={0.0f,0.0f},const Float2& size={0.0f,0.0f}){m_Position = position;m_Size = size;}
		void SetPosition(const Float2& position){m_Position = position;}
		void SetSize(const Float2& size){m_Size =size;}
		void SetAcceleration(const Float2& acceleration){m_Acceleration = acceleration;}

		Float2 GetPosition(){return m_Position;}
		Float2 GetSize(){return m_Size;}
		Float2 GetAcceleration(){return m_Acceleration;}


		void Collided(bool isCollided){m_IsCollided = isCollided;}
		void SetMoving(bool moving){m_IsMoving = moving;}

		bool IsCollided(){return m_IsCollided;}
		bool IsMoving(){return m_IsMoving;};

	private:
		Float2 m_Position{};
		Float2 m_Size{};
		Float2 m_Acceleration{};
		bool m_IsMoving{false};
		bool m_IsCollided{false};
};
class Collider{
	public:
		Collider(int32_t id){m_ID = id;}
		Collider(){}

		void SetPosition(const Float2& position);
		void SetSize(const Float2& size);
		void SetAcceleration(const Float2& aceeleration);


		Float2 GetPosition();
		Float2 GetSize();
		Float2 GetAcceleration();

		//returns true if collided.
		bool IsCollided();
	private:
		int32_t m_ID{};
};




class ColliderSystem{
	public:
		void RunCollisions();
		Collider CreateCollider(const Float2& position={0.0f,0.0f},const Float2& size={0.0f,0.0f});
		ColliderBackEnd* GetCollider(int32_t id){return &m_Colliders[id];}
	private:
		std::vector<ColliderBackEnd> m_Colliders{};
};
namespace Core{
	//returns true if collided.
	bool DefaultCollisionFunction(ColliderBackEnd* collider1,ColliderBackEnd* collider2);
	void DefaultMovementFucntion(ColliderBackEnd* collider1);
}


/*
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

	Float2* GetPathToObj(Float2 CurrentPos, Float2 FinalPath,uint32_t* outDestCount);

	GUUID GetID() { return m_ID; }
private:

	bool IsBlocked(ColliderBackEnd* movingCollider, Float2 Position);

	CollisionSystem* m_System{};
	GUUID m_ID{ 0 };
};

class CollisionSystem {
public:
	CollisionSystem();
	ColliderBackEnd* GetColliderBackEnd(GUUID ID) { return &m_Colliders[ID]; }

	void CheckCollisions();
	void AddObjectPathMap(Float2 Pos);
	Collider CreateCollider() { GUUID id = GUUID(); m_Colliders[id] = ColliderBackEnd(); return Collider(this, id); }

	Node* GetCellInfo() { return m_PathGrid; }
	bool IsBlocked(ColliderBackEnd* movingCollider, Float2 Position);

private:
	std::unordered_map<GUUID, ColliderBackEnd> m_Colliders{};
	Float2 m_OnCollisionMovePercent{ 0.00023f,0.0001f };


	

	bool* m_BlockedNodeList{};
	Node* m_PathGrid{};
};



*/