#pragma once
namespace GUI {
	enum class Style { NONE, COLOR, BORDER };
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
		void* BackGroundTexture{};
		void* BorderTexture{ nullptr };
	};

}