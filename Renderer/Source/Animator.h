#pragma once
#include "RendCore.h"
#include "Texture.h"
#include "AssetManager.h"
class Animator
{
public:
	Animator() {};
	//pass texture if you use textureatlases.
	Animator(const std::string& animationPathAbs,GUUID AnimationID,GUUID TextureID,AssetManager* assetManager);
	//This Doesn't use textureatlases
	Animator(const std::string& Path);


	void Update(float DeltaTime);
	void SetStage(const std::string& Stage);
	//Changes the stage if it's different.
	void KeepStage(const std::string& Stage);

	Asset<Texture> GetCurrentTexture();
	uint32_t GetTextureIndex(){return m_CurrentStageInfo.TextureIndex;}
	GUUID GetAnimationID();


private:
	struct Stage {
		uint32_t From{};
		uint32_t To{};
	};
	struct StageInfo {
		uint32_t TextureIndex{};
		Asset<Texture> TextureAsset{};
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

