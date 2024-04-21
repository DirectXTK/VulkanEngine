#pragma once
#include "RendCore.h"
#include "Texture.h"
class Animator
{
public:
	//pass texture if you use textureatlases.
	Animator(const std::string& Path,Texture* texture);
	//This Doesn't use textureatlases
	Animator(const std::string& Path);

	void AddStage(std::string StageName, float* Duration, Texture** textures, uint32_t FrameCount);

	void Update(float DeltaTime);
	void SetStage(const std::string& Stage);
	Texture* GetFrame();
private:
	struct StageInfo {
		uint32_t FrameCount{};
		Texture** m_Textures{};
		float* Duration{};
	};
	//key is stage name
	std::unordered_map<std::string, StageInfo> m_Frames{};
	std::string m_CurrentStage{};
	uint32_t m_CurrentFrame{};
};

