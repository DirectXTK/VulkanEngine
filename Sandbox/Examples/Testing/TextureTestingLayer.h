#pragma once
#include "Application.h"
class TextureTestingLayer: public Layer
{
public :
	TextureTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnGUI()override;
	void OnDestroy()override;
private:
	struct Unit {
		GUUID TextureHandle{};
		Float3 Position{};
		GUUID TextureAtlasHandle{};
	};
	std::vector<Unit> m_Units{};
	uint32_t m_TextureIndex{};

	float m_Move{ -1.0 };
	float m_Direct{ 0.005f };
	float m_Zoom{1.0f};
	float m_Magnification{0.2f};
};

