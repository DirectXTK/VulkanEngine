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
void Save(std::string Path, void* data, uint64_t Size, SerializerFormat* formats,uint32_t FormatCount)
{
	

	std::ofstream output(Path);
	SerializerFormat format = formats[0];
	uint64_t Offset{};

	for (uint32_t i = 0; i < Size; i++) {
		for (uint32_t j = 0; j < FormatCount; j++) {
				output << formats[j].keyword;
				for (uint32_t d = 0; d < formats[j].count; d++) {
					if(d !=0)
						output << formats[j].PaddingBetweenCount;
					switch (formats[j].format) {
					case Format::STRING: {
						output << *(std::string*)((char*)data + Offset);
						Offset += sizeof(std::string);
						break;
					}
					case Format::UINT32: {
						output << *(uint32_t*)((char*)data + Offset);
						Offset += sizeof(uint32_t);
						break;
					}
					default: {
						std::cout << "No such format\n";
						break;
					}
					}
				}
				output << "\n";
		}
		//Offset=i*

			
		
	}



	output.close();
}

void Serializer::StopSaving()
{
	m_OutputFile.close();
}

Serializer::~Serializer()
{
	StopSaving();
}
