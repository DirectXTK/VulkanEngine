#include "Animator.h"
Animator::Animator(const std::string& animationPathAbs,GUUID animationID,GUUID textureID,AssetManager* assetManager)
{
	m_AnimationID = animationID;
	//Texture data


	uint32_t TextureIndex{};
	uint64_t Index{};
	std::string Data{};
	std::ifstream input(animationPathAbs);
	uint64_t FileSize{};

	if(!std::filesystem::exists(animationPathAbs))
	{
		Core::Log(ErrorType::Error,"File path doesn't exist ",animationPathAbs);
		return;
	}

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
			stageinfo.TextureAsset = assetManager->GetAsset<Texture>(textureID);
			stageinfo.TextureIndex = TextureIndex;
			if(!stageinfo.TextureAsset)
			{
				Core::Log(ErrorType::Error,"Texture doeos't exists Animator::Animator ",textureID.ID);
				return;
			}
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

		From = std::stoul(Data.substr(Index, Data.find(",", Index) - Index));
		Index = Data.find("\"to\"",Index)+OffsetTo;

		To = std::stoul(Data.substr(Index, Data.find(",", Index) - Index));

		Index = Data.find("\"name\"", Index);


		m_Stages[Tag] = { From,To };
	}


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

void Animator::SetStage(const std::string& StageTag)
{	
	auto Index =m_Stages.find(StageTag); 
	if(Index != m_Stages.end()){
	m_CurrentStage = m_Stages[StageTag];
	m_CurrentStageInfo = m_StageInfos[m_CurrentStage.From];
	m_StageInfoOffset = m_CurrentStage.From;
	m_CurrentDuration = m_CurrentStageInfo.Duration;
	}else{
		m_CurrentStage = m_Stages.begin()->second;
		Core::Log(ErrorType::Warning,"Invalid animator stageTag ",StageTag,".");
	}

}

void Animator::KeepStage(const std::string& StageTag)
{
	Stage NextStage = m_Stages[StageTag];
	if (NextStage.From != m_CurrentStage.From)
	{
		m_CurrentStage = m_Stages[StageTag];
		m_CurrentStageInfo = m_StageInfos[m_CurrentStage.From];
		m_StageInfoOffset = m_CurrentStage.From;
		m_CurrentDuration = m_CurrentStageInfo.Duration;
	}
}


Asset<Texture> Animator::GetCurrentTexture() {
	return m_CurrentStageInfo.TextureAsset;
}

GUUID Animator::GetAnimationID()
{
	return m_AnimationID;
}