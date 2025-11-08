#pragma once 
#include "Application.h"

struct Unit{
	Unit(){}
	Collider collider;
	
};
class CollisionLayer : public Layer{
	public:
		CollisionLayer();

		void OnCreate()override;

		void OnUpdate(double deltaTime)override;
		void OnRender(double deltaTime)override;
		void OnGUI()override;

		void OnDestroy()override;
	private:
		std::vector<Unit> m_Units{};
};