#include "Animator.h"
Animator::Animator(const std::string& Path,Texture* texture)
{
	//Texture data

	uint32_t TextureIndex{};
	uint64_t Index{};
	std::string Data{};
	std::ifstream input(Path);
	uint64_t FileSize{};

	input.seekg(0, input.end);
	FileSize = input.tellg();
	Data.resize(FileSize);
	input.seekg(0, input.beg);
	input.read(&Data[0], FileSize);
	//For atlas
	Index = Data.find("\"frames\"", Index);
	uint64_t End = Data.find("\"meta\"", Index);
	while (Index < End) {
		const uint64_t DurationOffset{ 12 };
		Index = Data.find("\"duration\"",Index) ;
		if (Index != (uint64_t)-1) {
			Index += DurationOffset;
			StageInfo stageinfo{};
			uint64_t Duration{};

			stageinfo.Texture = texture;
			stageinfo.TextureIndex = TextureIndex;
			Duration = std::stoull(Data.substr(Index, Data.find("\n",Index) -Index));
			stageinfo.Duration = (float)Duration;

			m_StageInfos.push_back(stageinfo);
			TextureIndex++;
		}
	
	}
	Index = 0;
	Index = Data.find("\"frameTags\"", Index);
    End = Data.find("]", Index);
	while (Index < End) {
		const uint64_t TagOffset{9};
		const uint64_t OffsetFrom{8};
		const uint64_t OffsetTo{6};

		std::string Tag{};
		uint32_t From{};
		uint32_t To{};

		Index = Data.find("\"name\"",Index)+ TagOffset;
		Tag = Data.substr( Index, Data.find("\",", Index)  - Index);
		Index = Data.find("\"from\"", Index) + OffsetFrom;

		From = std::stoull(Data.substr(Index, Data.find(",", Index) - Index));
		Index = Data.find("\"to\"",Index)+OffsetTo;

		To = std::stoull(Data.substr(Index, Data.find(",", Index) - Index));

		Index = Data.find("\"name\"", Index);


		m_Stages[Tag] = { From,To };
	}


}
void Animator::AddStage(std::string StageName, float* Duration, Texture** textures, uint32_t FrameCount) {

	/*StageInfo stageinfo{};
	stageinfo.m_Textures = textures;
	stageinfo.Duration = new float[FrameCount];
	stageinfo.FrameCount = FrameCount;
	for (uint32_t i = 0; i <FrameCount; i++) {
		stageinfo.Duration[i] = Duration[i];
	}
	m_Frames[StageName] = stageinfo;
	*/

}
void Animator::Update(float DeltaTime)
{
	m_CurrentDuration -= DeltaTime;
	if (m_CurrentDuration <= 0.0f) {
		m_StageInfoOffset++;
		if (m_StageInfoOffset > m_CurrentStage.To)
			m_StageInfoOffset = m_CurrentStage.From;
		
		m_CurrentStageInfo= m_StageInfos[m_StageInfoOffset];
		m_CurrentDuration = m_CurrentStageInfo.Duration;

	}
		
}

void Animator::SetStage(const std::string& Stage)
{
	m_CurrentStage = m_Stages[Stage];
	m_CurrentStageInfo = m_StageInfos[m_CurrentStage.From];
	m_StageInfoOffset = m_CurrentStage.From;
	m_CurrentDuration = m_CurrentStageInfo.Duration;

}

Texture* Animator::GetFrame()
{
	return m_CurrentStageInfo.Texture;
}
uint32_t Animator::GetTextureIndex() {
	return m_CurrentStageInfo.TextureIndex;
}

Float2* Animator::GetTextureCoords()
{
	return m_CurrentStageInfo.Texture->GetTextureCoords(m_CurrentStageInfo.TextureIndex);
}
