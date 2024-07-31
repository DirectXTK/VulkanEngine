#pragma once
#include "Application.h"
struct TextureComponent {
	GUUID TextureID{};
};
struct AnimationComponent {
	GUUID AnimationID{};
};
struct IDComponent {
	GUUID ID{};
};
struct TagComponent {
	std::string Tag{"NULL"};
};
struct MeshComponent {
	Float3 Color{};
};
struct TransformComponent {
	Float2 Size{};
	Float3 Position{};
};
struct ColliderComponent {

};