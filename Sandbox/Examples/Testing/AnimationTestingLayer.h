#pragma once
#include "DefaultCameraControlls.h"
#include "Animator.h"
#define DEMO_FEATURES
#include "Utils.h"
struct AnimationUnit {
	Float2 Position{};
	Float2* MoveLocation{};
	float MoveSpeed{0.0003f};
	Animator Animator{};
	Collider Collid;
	GUUID ID{};
	bool Moving{false};
	uint32_t MoveCellCount{};
	uint32_t CurrentCellIndex{};
};
struct PathGrid {
	Float2 Position{};
	Float2 Size{0.04f,0.04f };
};
class AnimationTestingLayer :public Layer
{
public:
	AnimationTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnDestroy()override;
	void OnGUI()override;
private:
	void MoveUnit(AnimationUnit* unit);

	Float2 m_Size{ 0.04f,0.04f};
	bool m_SpawnUnit{false};
	std::string m_SpawnedUnit{};

	CollisionSystem m_System{};

	std::vector<AnimationUnit> m_Units{};
	std::vector<PathGrid> m_PathGrid{};

	Float2 m_MoveLocation{};
	GUUID m_CurrentlySelectedUnit{};
};

