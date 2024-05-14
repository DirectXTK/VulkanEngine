#pragma once
#include "RendCore.h"
#include "Texture.h"
class Animator
{
public:
	Animator() {};
	//pass texture if you use textureatlases.
	Animator(const std::string& Path, GUUID TextureID);
	//This Doesn't use textureatlases
	Animator(const std::string& Path);


	void Update(float DeltaTime);
	void SetStage(const std::string& Stage);

	GUUID GetCurrentTextureID();
	GUUID GetAnimationID();

	uint32_t GetTextureIndex();

private:
	struct Stage {
		uint32_t From{};
		uint32_t To{};
	};
	struct StageInfo {
		GUUID TextureID{};
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
	GUUID m_AnimationID{};
};

