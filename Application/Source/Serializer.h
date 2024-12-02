#pragma once
#include "AppCore.h"
struct SerializerFormat {
	std::string keyword{};
	Format format{};
	//-1 means custom count.
	int count{};
	std::string PaddingBetweenCount{};
	uint64_t Offset{  };
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


	void Save(void* data,uint64_t DataCount, SerializerFormat* format, uint32_t formatCount, SerializerClassDesc* classdesc);

	 //Load functions
	void StartLoading(std::string Path);
	void StopLoading();
	void* Load(uint64_t* Out_DataCount);
	 ~Serializer();
private:
	void FillContainerWithData(char* Data, uint64_t*DataOffset, std::string line, SerializerFormat format);
	void FillContainerWithDataDynamic(char* Data, uint64_t* DataOffset,  uint64_t ArraySize ,std::string line, Format format);
	void FillContainerWithDataDynamicCustomData(char* Data, uint64_t* DataOffset, uint64_t ArraySize, std::string line, Format format);


	void GetDynamicFormats(std::string* line, SerializerFormat format, Format* formats, uint32_t FormatCount);
	void AllocateSpaceForDynamicArray(char* Data, uint32_t NumOffAlloc, uint64_t* Offset, Format format);
	void AllocateSpaceForDynamicArrayCustomFormat(char* Member, uint32_t NumOffAlloc, uint64_t* Offset, Format* format);
	

	void DynamicArrayType(void* data, uint64_t Offset, Format format, uint32_t Index);
	void DynamicArrayTypePP(void* data, uint64_t Offset, Format format, uint32_t Index);

	void StackArrayType(void* data, uint64_t Offset, Format format, uint32_t Index);
	SerializerFormat* m_Formats{};
	uint32_t m_FormatCount{};
	std::ofstream m_OutputFile{};

	//Input data
	uint64_t m_InputDataOffset{};
	std::string m_InputData{};
	std::ifstream m_InputFile{};

};
