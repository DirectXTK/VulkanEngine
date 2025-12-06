#pragma once 
#include "Application.h"

struct Unit{
	Unit(){}
	Float2 Pos{};
	Float2 Size{};
	bool Collided{false};
	
};
class CollisionLayer : public Layer{
	public:
		CollisionLayer();

		void OnCreate()override;

		void OnUpdate(double deltaTime)override;
		void OnRender(double deltaTime)override;
		void OnGUI()override;
		void OnEvent(Event& event)override;

		void OnDestroy()override;
	private:
		void OnKeyBoardEvent(KeyBoardEvent& event);

		Float2 m_Move{};
		std::vector<Unit> m_Units{};
};