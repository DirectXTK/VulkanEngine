#pragma once
#include "Application.h"
#include "Animator.h"
struct Unit {
	Float2 Position{};
	Animator Animator{};
	GUUID ID{};
};
class AnimationTestingLayer:public Layer
{
public:
	AnimationTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnDestroy()override;
	void OnGUI()override;
private:
	void MoveUnit();

	std::vector<Unit> m_Units{};
	Float2 m_MoveLocation{};
	GUUID m_CurrentlySelectedUnit{};
};

