#include "Serializer.h"
void Serializer::Init(SerializerFormat* formats, uint32_t formatCount) {
	m_Formats = formats;
	m_FormatCount = formatCount;
}
void Serializer::StartSaving(std::string Path)
{
	m_OutputFile = std::ofstream(Path);
	if(!m_OutputFile.is_open())
		std::cout << "Invalid path.\n";

}
void Serializer::Save(void* data,uint32_t DataCount, SerializerFormat* formats,uint32_t formatCount,  SerializerClassDesc* classdesc)
{
	if (!m_OutputFile.is_open()) {
		std::cout << "You forgot to start function StartSaving().\n";
		return;
	}
	uint64_t Offset{};

	m_OutputFile << "FormatCount=" << formatCount<<"\n";
	for (int i = 0; i < formatCount; i++) {

		m_OutputFile << formats[i].keyword << "\n";
		m_OutputFile << (uint32_t)formats[i].format << "\n";
		m_OutputFile << formats[i].count << "\n";
		m_OutputFile << formats[i].PaddingBetweenCount << "\n";
		m_OutputFile << formats[i].Offset << "\n";

	}
	m_OutputFile << classdesc->ClassName << "\n";
	m_OutputFile << classdesc->MemberPrefix << "\n";
	m_OutputFile << classdesc->MemberSuffix << "\n";
	m_OutputFile << classdesc->Stride << "\n";



	
	m_OutputFile << "DataCount=" << DataCount << "\n";
	for(int d=0;d < DataCount;d++){
		m_OutputFile << classdesc->ClassName<<"\n";
		for (int j = 0; j < formatCount; j++) {
			SerializerFormat* format = &formats[j];
			Offset = format->Offset+ d * classdesc->Stride;
			for (int i = 0; i < format->count; i++) {
				if (i == 0)
				{
					m_OutputFile <<classdesc->MemberPrefix<< format->keyword;
				}
				else
					m_OutputFile << format->PaddingBetweenCount;

				switch (format->format) {
				case Format::STRING: {
					m_OutputFile << *(std::string*)((char*)data + Offset+(i*sizeof(std::string))  );
					break;
				}
				case Format::UINT16: {
					m_OutputFile << *(uint16_t*)((char*)data + Offset + (i * sizeof(uint16_t)));
					break;
				}
				case Format::UINT32: {
					m_OutputFile << *(uint32_t*)((char*)data + Offset + (i * sizeof(uint32_t)));
					break;
				}
				case Format::UINT64: {
					m_OutputFile << *(uint64_t*)((char*)data + Offset + (i * sizeof(uint64_t)));
					break;
				}
				case Format::INT16: {
					m_OutputFile << *(int16_t*)((char*)data + Offset + (i * sizeof(int16_t)));
					break;
				}
				case Format::INT32: {
					m_OutputFile << *(int32_t*)((char*)data + Offset + (i * sizeof(int32_t)));
					break;
				}
				case Format::INT64: {
					m_OutputFile << *(int64_t*)((char*)data + Offset + (i * sizeof(int64_t)));
					break;
				}
				case Format::FLOAT: {
					m_OutputFile << *(float*)((char*)data + Offset + (i * sizeof(float)));
					break;
				}
				case Format::DOUBLE: {
					m_OutputFile << *(double*)((char*)data + Offset + (i * sizeof(double)));
					break;
				}
				case Format::CHAR: {
					m_OutputFile << *(char*)((char*)data + Offset + (i * sizeof(char)));
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
	}



}
void Serializer::StartLoading(std::string Path)
{
	m_InputFile = std::ifstream(Path);
	if(!m_InputFile.is_open())
		std::cout << "Invalid path.\n";

}
void Serializer::StopLoading()
{
	m_InputFile.close();
}
void* Serializer::Load(uint64_t* Out_DataCount)
{
	if (!m_InputFile.is_open()) {
	std::cout << "You forgot to start function StartLoading().\n";
	return nullptr;
	}
	uint32_t FormatCount{};

	uint32_t DataCount{};
	char* Data{};

	uint64_t Offset{};

	SerializerClassDesc classdesc{};
	SerializerFormat* formats{};

	uint64_t InputDataSize{ };

	m_InputFile.seekg(0, m_InputFile.end);
	InputDataSize = m_InputFile.tellg();
	m_InputFile.seekg(0, m_InputFile.beg);
	m_InputData.resize(InputDataSize);

	m_InputFile.read(m_InputData.data(), InputDataSize);

	if (InputDataSize == 0)
		std::cout << "Input file is empty/not loaded or doesn't exist.\n";

	Offset = m_InputData.find("\n", 0);

	FormatCount = std::stoi(m_InputData.substr(12, Offset - 12));
	formats = new SerializerFormat[FormatCount];
	Offset++;

	std::string line{};
	for (int i = 0; i < FormatCount; i++) {
		//Get the keyword
		line = m_InputData.substr(Offset, m_InputData.find("\n", Offset)-Offset);
		Offset = m_InputData.find("\n", Offset)+1;
		formats[i].keyword = line;
		//Get the format
		line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
		Offset = m_InputData.find("\n", Offset) + 1;
		formats[i].format = (Format)std::stoi(line);
		//Get the count of variables
		line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
		Offset = m_InputData.find("\n", Offset) + 1;
		formats[i].count = std::stoi(line);
		//Get the padding between variables
		line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
		Offset = m_InputData.find("\n", Offset) + 1;
		formats[i].PaddingBetweenCount = line;
		//Get the ignore spaces bool
		line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
		Offset = m_InputData.find("\n", Offset) + 1;
		formats[i].Offset = std::stoi(line);
	}
	//Class description
	//Class keyword
	line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
	Offset = m_InputData.find("\n", Offset) + 1;
	classdesc.ClassName = line;
	//Get the member prefix
	line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
	Offset = m_InputData.find("\n", Offset) + 1;
	classdesc.MemberPrefix = line;
	//Get the member suffix
	line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
	Offset = m_InputData.find("\n", Offset) + 1;
	classdesc.MemberSuffix = line;
	//Get the stride
	line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
	Offset = m_InputData.find("\n", Offset) + 1;
	classdesc.Stride = std::stoi(line);

	//Get the data count
	Offset += 10;
	line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
	Offset = m_InputData.find("\n", Offset) + 1;
	DataCount = std::stoi(line);
	Data = new char[DataCount * classdesc.Stride];

	uint64_t DataOffset{};
	for (uint32_t i = 0; i < DataCount; i++) {
		Offset = m_InputData.find("\n", Offset) + 1;
		DataOffset = classdesc.Stride*i;

		for (uint32_t j = 0; j < FormatCount; j++) {
			DataOffset = classdesc.Stride * i + formats[j].Offset;
			if (formats[j].count == 0)
				std::cout << "format count is equal to 0 check your SerializerFormats.\n";
			Offset += classdesc.MemberPrefix.size() + formats[j].keyword.size();

				for (uint32_t d = 0; d < formats[j].count; d++) {

					if ((d + 1) == formats[j].count) {
						line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
						FillContainerWithData(Data, &DataOffset, line, formats[j]);
						Offset = m_InputData.find("\n", Offset)+1;
					}
					else {
						line = m_InputData.substr(Offset, m_InputData.find(formats[j].PaddingBetweenCount, Offset) - Offset);
						FillContainerWithData(Data, &DataOffset, line, formats[j]);
						Offset = m_InputData.find(formats[j].PaddingBetweenCount, Offset) + 1;
					}
				}
		}

	}



	*Out_DataCount = DataCount;

	return Data;
}


void Serializer::StopSaving()
{
	m_OutputFile.close();
}

Serializer::~Serializer()
{
	StopSaving();
}

void Serializer::FillContainerWithData(char* Data, uint64_t*DataOffset, std::string line, SerializerFormat format)
{
	switch (format.format) {
	case Format::STRING: {
		std::string* str =(std::string*)(void*)(Data + *DataOffset);
		memset(str, 0, sizeof(std::string));
		*str = std::string();
		*str = line;
	//	*str = std::string();
	//	*str = line;
		//memcpy(Data + *DataOffset, line.data(), line.size());
		*DataOffset += sizeof(std::string);
		break;
	}
	case Format::UINT16: {
		uint16_t converteddata = std::stoul(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(uint16_t));
		*DataOffset += sizeof(uint16_t);

		break;
	}
	case Format::UINT32: {
		uint32_t converteddata = std::stoul(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(uint32_t));
		*DataOffset += sizeof(uint32_t);

		break;
	}
	case Format::UINT64: {
		uint64_t converteddata = std::stoull(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(uint64_t));
		*DataOffset += sizeof(uint64_t);

		break;
	}
	case Format::INT16: {
		int16_t converteddata = std::stoi(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(int16_t));
		*DataOffset += sizeof(int16_t);

		break;
	}
	case Format::INT32: {
		int32_t converteddata = std::stoi(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(int32_t));
		*DataOffset += sizeof(int32_t);

		break;
	}
	case Format::INT64: {
		int64_t converteddata = std::stoll(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(uint64_t));
		*DataOffset += sizeof(int64_t);

		break;
	}
	case Format::FLOAT: {
		float converteddata = std::stof(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(float));
		*DataOffset += sizeof(float);

		break;
	}
	case Format::DOUBLE: {
		double converteddata = std::stod(line);
		memcpy(Data + *DataOffset, &converteddata, sizeof(double));
		*DataOffset += sizeof(double);

		break;
	}
	case Format::CHAR: {
		if (line.size() == 0)
			break;
		char converteddata = line[0];
		memcpy(Data + *DataOffset, &converteddata, sizeof(char));
		*DataOffset += sizeof(char);

		break;
	}


	default: {
		std::cout << "No such format\n";
		return;
	}
	}
}
