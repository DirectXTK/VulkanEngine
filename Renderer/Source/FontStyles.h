#pragma once
#include "AppCore.h"
enum class Style { DrawBorder };

struct StyleBorderData {
	float BorderWidth{0.01f};
	Float4 BorderColor{1.0f,1.0f,1.0f,1.0f};
	Float4 BackGroundColor{ 0.2f,0.2f,0.2f,1.0f };
	//placeholder for now
	void* BackGroundTexture{};
	void* BorderTexture{ nullptr };
};