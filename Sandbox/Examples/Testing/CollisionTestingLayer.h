#pragma once
#include "Application.h"
#include "Animator.h"
#include "ChunkSystem.h"
struct CollisionObjects{
	Float2 Position{};
	Float4 Color{1.0f,1.0f,1.0f,1.0f};
	Collider collider{};
};
class CollisionTestingLayer : public Layer
{
public:
	CollisionTestingLayer();
	void OnUpdate(float Deltatime) override;
	void OnCreate()override;
	void OnDestroy()override;
	void OnGUI()override;
private:
	std::vector<CollisionObjects> m_Colliders{};
	uint32_t m_ChunkWidth{2};
	uint32_t m_ChunkHeight{ 2 };
	Float4* m_Colors{};

	Float2 m_Size{0.01f,0.01f};
};

