#include "Animator.h"
Animator::Animator(const std::string& Path,Texture* textured)
{
	std::string Data{};
	std::ifstream input(Path);
	uint64_t FileSize{};

	input.seekg(0, input.end);
	FileSize = input.tellg();
	input.seekg(0, input.beg);
	input.read(&Data[0], FileSize);

	


}
void Animator::AddStage(std::string StageName, float* Duration, Texture** textures, uint32_t FrameCount) {

	StageInfo stageinfo{};
	stageinfo.m_Textures = textures;
	stageinfo.Duration = new float[FrameCount];
	stageinfo.FrameCount = FrameCount;
	for (uint32_t i = 0; i <FrameCount; i++) {
		stageinfo.Duration[i] = Duration[i];
	}
	m_Frames[StageName] = stageinfo;

}
void Animator::Update(float DeltaTime)
{
}

void Animator::SetStage(const std::string& Stage)
{
}

Texture* Animator::GetFrame()
{
	return m_Frames[m_CurrentStage].m_Textures[m_CurrentFrame];
}
