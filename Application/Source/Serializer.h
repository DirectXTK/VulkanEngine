#pragma once
#include "AppCore.h"
struct SerializerFormat {
	std::string keyword{};
	Format format{};
	uint32_t count{};
	std::string PaddingBetweenCount{};
	bool ignorespaces{ false };
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
	SerializerFormat* m_Formats{};
	uint32_t m_FormatCount{};
	std::ofstream m_OutputFile{};
	std::ifstream m_InputFile{};

};
