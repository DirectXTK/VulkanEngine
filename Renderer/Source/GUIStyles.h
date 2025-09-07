#pragma once
#include "AppCore.h"
namespace GUI {
	enum class Style { NONE, COLOR, BORDER ,SLIDER};
	struct ColorStyle {
		//used for debbuging
		uint8_t StructSize{};
		Float4 Color{};
	};
	struct BorderStyle {
		uint8_t StructSize{};
		bool DrawBorder{ true };
		float BorderWidth{ 0.01f };
		Float4 BorderColor{ 1.0f,1.0f,1.0f,1.0f };
		Float4 BackGroundColor{ 1.0f,1.0f,1.0f,0.0f };

		//placeholder for now
		void* BackGroundTexture{nullptr};
		void* BorderTexture{ nullptr };
	};
	struct SliderStyle{
		//Fills the appopriate amount of background accoring to min max
		uint8_t StructSize{};
		bool FillOn{false};
		Float4 FillColor{1.0f,1.0f,1.0f,1.0f};
	};

}