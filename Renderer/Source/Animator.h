#pragma once
#include "RendCore.h"
#include "Texture.h"
class Animator
{
public:
	//pass texture if you use textureatlases.
	Animator(const std::string& Path, Texture* texture);
	//This Doesn't use textureatlases
	Animator(const std::string& Path);

	void AddStage(std::string StageName, float* Duration, Texture** textures, uint32_t FrameCount);

	void Update(float DeltaTime);
	void SetStage(const std::string& Stage);
	Texture* GetFrame();
	uint32_t GetTextureIndex();
	Float2* GetTextureCoords();
private:
	struct Stage {
		uint32_t From{};
		uint32_t To{};
	};
	struct StageInfo {
		Texture* Texture{};
		uint32_t TextureIndex{};
		float Duration{};
	};
	//key is stage name
	std::unordered_map<std::string, Stage> m_Stages{};
	std::vector<StageInfo> m_StageInfos{};

	uint32_t m_StageInfoOffset{};
	Stage m_CurrentStage{};
	StageInfo m_CurrentStageInfo{  };

	float m_CurrentDuration{};
};

