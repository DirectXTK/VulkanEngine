#pragma once
#include "Application.h"
#include "Animator.h"
class AnimationTestingLayer:public Layer
{
public:
	AnimationTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnDestroy()override;
	void OnGUI()override;
private:
	Animator* m_Animation{};
};

