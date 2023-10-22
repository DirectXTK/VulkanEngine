#include "Serializer.h"
void Serializer::Init(SerializerFormat* formats, uint32_t formatCount) {
	m_Formats = formats;
	m_FormatCount = formatCount;
}
void Serializer::StartSaving(std::string Path)
{
	m_OutputFile = std::ofstream(Path);
}
void Serializer::Save(void* data,uint32_t DataCount, SerializerFormat* formats,uint32_t formatCount,  SerializerClassDesc* classdesc)
{
	if (!m_OutputFile.is_open()) {
		std::cout << "You forgot to provide with a location.";
		return;
	}

	uint64_t Offset{};

	m_OutputFile << "DataCount=" << DataCount << "\n";
	for(int d=0;d < DataCount;d++){
		m_OutputFile << classdesc->ClassName<<"\n";
		for (int j = 0; j < formatCount; j++) {
			SerializerFormat* format = &formats[j];
			for (int i = 0; i < format->count; i++) {
				if (i == 0)
				{
					m_OutputFile <<classdesc->MemberPrefix<< format->keyword;
				}
				else
					m_OutputFile << format->PaddingBetweenCount;

				switch (format->format) {
				case Format::STRING: {
					m_OutputFile << *(std::string*)((char*)data + Offset);
					Offset += sizeof(std::string);
					break;
				}
				case Format::UINT16: {
					m_OutputFile << *(uint16_t*)((char*)data + Offset);
					Offset += sizeof(uint16_t);
					break;
				}
				case Format::UINT32: {
					m_OutputFile << *(uint32_t*)((char*)data + Offset);
					Offset += sizeof(uint32_t);
					break;
				}
				case Format::UINT64: {
					m_OutputFile << *(uint64_t*)((char*)data + Offset);
					Offset += sizeof(uint64_t);
					break;
				}
				case Format::INT16: {
					m_OutputFile << *(int16_t*)((char*)data + Offset);
					Offset += sizeof(int16_t);
					break;
				}
				case Format::INT32: {
					m_OutputFile << *(int32_t*)((char*)data + Offset);
					Offset += sizeof(int32_t);
					break;
				}
				case Format::INT64: {
					m_OutputFile << *(int64_t*)((char*)data + Offset);
					Offset += sizeof(int64_t);
					break;
				}
				case Format::FLOAT: {
					m_OutputFile << *(float*)((char*)data + Offset);
					Offset += sizeof(float);
					break;
				}
				case Format::DOUBLE: {
					m_OutputFile << *(double*)((char*)data + Offset);
					Offset += sizeof(double);
					break;
				}
				case Format::CHAR: {
					m_OutputFile << *(char*)((char*)data + Offset);
					Offset += sizeof(char);
					break;
				}

				
				default: {
					std::cout << "No such format\n";
					break;
				}
				}
				

			}
			m_OutputFile << classdesc->MemberSuffix;
			m_OutputFile << "\n";
		}
		Offset = (d + 1) * classdesc->Stride;
	}



}
void Serializer::StartLoading(std::string Path)
{
	m_InputFile = std::ifstream(Path);
}
void Serializer::StopLoading()
{
	m_InputFile.close();
}
void* Serializer::Load(uint64_t* Out_DataCount)
{
	if(!m_InputFile.is_open())
	return nullptr;






}


void Serializer::StopSaving()
{
	m_OutputFile.close();
}

Serializer::~Serializer()
{
	StopSaving();
}
