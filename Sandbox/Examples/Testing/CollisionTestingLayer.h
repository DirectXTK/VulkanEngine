#pragma once
#include "Application.h"
#include "Animator.h"
struct CollisionObjects{
	Float2 Position{};
	Float4 Color{};
	Collider collider{};
};
class CollisionTestingLayer : public Layer
{
public:
	CollisionTestingLayer();
	void OnUpdate(double Deltatime) override;
	void OnCreate()override;
	void OnDestroy()override;
	void OnGUI()override;
private:
	std::vector<CollisionObjects> m_Colliders{};

	Float2 m_Size{0.01f,0.01f};
};

