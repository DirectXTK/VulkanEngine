#pragma once
#include "DefaultCameraControlls.h"
#include "Animator.h"
struct AnimationUnit {
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

	bool m_SpawnUnit{false};
	std::string m_SpawnedUnit{};

	std::vector<AnimationUnit> m_Units{};
	Float2 m_MoveLocation{};
	GUUID m_CurrentlySelectedUnit{};
};

