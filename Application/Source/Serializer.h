#pragma once
#include "AppCore.h"
struct SerializerFormat {
	std::string keyword{};
	Format format{};
	uint32_t count{};
	std::string PaddingBetweenCount{};
	uint32_t Offset{  };
};
struct SerializerClassDesc {
	std::string ClassName{};
	uint32_t Stride{};
	std::string MemberPrefix{};
	std::string MemberSuffix{};

};
class Serializer
{
public:
	void Init(SerializerFormat* formats, uint32_t formatCount);
	//Save functions
	void StartSaving(std::string Path);
	void StopSaving();


	void Save(void* data, uint32_t DataCount, SerializerFormat* format, uint32_t formatCount, SerializerClassDesc* classdesc);

	 //Load functions
	void StartLoading(std::string Path);
	void StopLoading();
	void* Load(uint64_t* Out_DataCount);
	 ~Serializer();
private:
	void FillContainerWithData(char* Data, uint64_t*DataOffset, std::string line, SerializerFormat format);
	SerializerFormat* m_Formats{};
	uint32_t m_FormatCount{};
	std::ofstream m_OutputFile{};

	//Input data
	uint64_t m_InputDataOffset{};
	std::string m_InputData{};
	std::ifstream m_InputFile{};

};