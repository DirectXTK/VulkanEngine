#pragma once
#include "Application.h"
class AssetTestingLayer : public Layer{
public :
    AssetTestingLayer();
    void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnDestroy()override;
	void OnEvent(Event& event)override;
	void OnGUI()override;
	void OnRender(double deltime)override;

private:

	void OnMouseMoveEvent(MouseEvent& event);
};