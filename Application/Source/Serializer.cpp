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
void Serializer::Save(void* data,uint64_t DataCount, SerializerFormat* formats,uint32_t formatCount,  SerializerClassDesc* classdesc)
{
	if (!m_OutputFile.is_open()) {
		std::cout << "You forgot to start function StartSaving().\n";
		return;
	}
	uint64_t Offset{};

	m_OutputFile << "FormatCount=" << formatCount<<"\n";
	for (uint32_t i = 0; i < formatCount; i++) {

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
	for (uint32_t d = 0; d < DataCount; d++) {
		m_OutputFile << classdesc->ClassName << "\n";
		for (uint32_t j = 0; j < formatCount; j++) {
			SerializerFormat* format = &formats[j];
			int64_t variablecount{format->count};
			Offset = format->Offset + d * classdesc->Stride;

			if (variablecount == -1) {
				if(format->format == Format::FORMAT)
					variablecount = *(int64_t*)((char*)data + Offset + formats[j + 2].Offset - format->Offset);
				else
					variablecount = *(int64_t*)((char*)data + Offset + formats[j + 1].Offset-format->Offset);
			}

				m_OutputFile << classdesc->MemberPrefix << format->keyword;
			for (int i = 0; i < variablecount; i++) {
				if (i != 0)
					m_OutputFile << format->PaddingBetweenCount;

				if (format->count == -1) {
					Format currentFormat = format->format;
					if (currentFormat == Format::FORMAT)
					{
						uint64_t address = *(uint64_t*)((char*)data + Offset+ formats[j + 1].Offset- format->Offset);
						currentFormat = ((Format*)address)[i];
						DynamicArrayTypePP(data, Offset, currentFormat, i);

					}
					else {
						DynamicArrayType(data, Offset, currentFormat, i);

					}

				}
				else {

					StackArrayType(data, Offset, format->format, i);
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
	for (uint32_t i = 0; i < FormatCount; i++) {
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

			if(formats[j].count != -1)
				for (uint32_t d = 0; d < (uint32_t)formats[j].count; d++) {

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
			else {
				//Dynamic format
				if (formats[j].format == Format::FORMAT) {
					uint32_t formatCountDynamic{};
					Format* dynamicformats{};
					std::string formatline{};
					uint64_t OldOffset{ Offset };
					//Padding prefixes sufixes
					uint64_t ExtraSize = classdesc.MemberPrefix.size() + classdesc.MemberSuffix.size();

					Offset = m_InputData.find("\n", Offset) + 1;
					formatline = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);

					Offset = m_InputData.find("\n", Offset) + 1;


					line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
					formatCountDynamic = std::stoi(line.substr(formats[j + 2].keyword.size() + ExtraSize, line.find("\n") - formats[j + 2].keyword.size() + ExtraSize));
					dynamicformats = new Format[formatCountDynamic];
					Offset = OldOffset;

					GetDynamicFormats(&formatline,formats[j+1], dynamicformats,formatCountDynamic);
					AllocateSpaceForDynamicArrayCustomFormat((Data+DataOffset), formatCountDynamic, &DataOffset, dynamicformats);

					for (uint64_t d = 0; d < formatCountDynamic; d++) {

						if (d == formatCountDynamic - 1) {
							line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
							FillContainerWithDataDynamicCustomData((Data + DataOffset), &d, formatCountDynamic, line,dynamicformats[d]);
							Offset = m_InputData.find("\n", Offset) + 1;
						}
						else {
							line = m_InputData.substr(Offset, m_InputData.find(formats[j].PaddingBetweenCount, Offset) - Offset);
							FillContainerWithDataDynamicCustomData((Data + DataOffset), &d, formatCountDynamic, line, dynamicformats[d]);
						
							Offset = m_InputData.find(formats[j].PaddingBetweenCount, Offset) + 1;
							//return Data;
						}
					}
					if (formatCountDynamic == 0)
						Offset = m_InputData.find("\n", Offset) + 1;

				}
				else {

					//Dynamic 
					uint32_t formatCountDynamic{};
					uint64_t OldOffset{ Offset };
					//Padding prefixes sufixes
					uint64_t ExtraSize = classdesc.MemberPrefix.size() + classdesc.MemberSuffix.size();
					Offset = m_InputData.find("\n", Offset) + 1;

					line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
					formatCountDynamic = std::stoi(line.substr(formats[j + 1].keyword.size()+ ExtraSize, line.find("\n") - formats[j + 1].keyword.size()+ ExtraSize));
					Offset = OldOffset;

					AllocateSpaceForDynamicArray(Data, formatCountDynamic, &DataOffset, formats[j].format);

					for (uint64_t d = 0; d < formatCountDynamic; d++) {

						if (d == formatCountDynamic - 1) {
							line = m_InputData.substr(Offset, m_InputData.find("\n", Offset) - Offset);
							FillContainerWithDataDynamic((Data + DataOffset), &d,1, line, formats[j].format);
							Offset = m_InputData.find("\n", Offset) + 1;
						}
						else {
							line = m_InputData.substr(Offset, m_InputData.find(formats[j].PaddingBetweenCount, Offset) - Offset);
							FillContainerWithDataDynamic((Data + DataOffset), &d,1, line, formats[j].format);
							Offset = m_InputData.find(formats[j].PaddingBetweenCount, Offset) + 1;
							//return Data;
						}
					}
					if (formatCountDynamic == 0)
						Offset = m_InputData.find("\n", Offset) + 1;
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
		uint16_t converteddata = (uint16_t)std::stoul(line);
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
		*DataOffset += sizeof(uint64_t);


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

void Serializer::FillContainerWithDataDynamic(char* Data, uint64_t* Index,uint64_t ArraySize, std::string line, Format format)
{
	switch (format) {
	case Format::STRING: {
		
		std::string* str{};
		memcpy(&str, Data, sizeof(void*));
		str[*Index] = std::string();
		str[*Index] = line;
		//	*str = std::string();
		//	*str = line;
			//memcpy(Data + *DataOffset, line.data(), line.size());
		break;
	}
	case Format::UINT16: {
		uint16_t converteddata = (uint16_t)std::stoul(line);
		uint16_t* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;

		break;
	}
	case Format::UINT32: {
		uint32_t converteddata = std::stoul(line);
		uint32_t* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;

		//*Index += sizeof(uint32_t);

		break;
	}
	case Format::UINT64: {
		uint64_t converteddata = std::stoull(line);
		uint64_t* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;


		break;
	}
	case Format::INT16: {
		int16_t converteddata = std::stoi(line);
		int16_t* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;

		break;
	}
	case Format::INT32: {
		int32_t converteddata = std::stoi(line);
		int32_t* dst{};
		memcpy(&dst, Data, sizeof(void*));
		//*(int32_t*)Data[*Index] = converteddata;
		dst[*Index] = converteddata;


		break;
	}
	case Format::INT64: {
		int64_t converteddata = std::stoll(line);
		int64_t* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;


		break;
	}
	case Format::FLOAT: {
		float converteddata = std::stof(line);
		float* dst{};
		memcpy(&dst, Data, sizeof(void*)* ArraySize);
		dst[*Index] = converteddata;


		break;
	}
	case Format::DOUBLE: {
		double converteddata = std::stod(line);
		double* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;


		break;
	}
	case Format::CHAR: {
		if (line.size() == 0)
			break;
		*Index += sizeof(char);
		char converteddata = line[*Index];
		char* dst{};
		memcpy(&dst, Data, sizeof(void*));
		dst[*Index] = converteddata;


		break;
	}


	default: {
		std::cout << "No such format\n";
		return;
	}
	}
}

void Serializer::FillContainerWithDataDynamicCustomData(char* Data, uint64_t* Index, uint64_t ArraySize, std::string line, Format format)
{
	void** ArrayPtr = (void**)Data;
	void** dst = (void**)*ArrayPtr;
	switch (format) {
	case Format::STRING: {

		std::string* Member = (std::string*)dst[*Index];
		*Member = line;
		break;
	}
	case Format::UINT16: {
		uint16_t converteddata = (uint16_t)std::stoul(line);
		uint16_t* Member = (uint16_t*)dst[*Index];
		*Member = converteddata;

		break;
	}
	case Format::UINT32: {
		uint32_t converteddata = std::stoul(line);
		uint32_t* Member = (uint32_t*)dst[*Index];
		*Member = converteddata;

		//*Index += sizeof(uint32_t);

		break;
	}
	case Format::UINT64: {
		uint64_t converteddata = std::stoull(line);
		uint64_t* Member = (uint64_t*)dst[*Index];
		*Member = converteddata;


		break;
	}
	case Format::INT16: {
		int16_t converteddata = std::stoi(line);
		int16_t* Member = (int16_t*)dst[*Index];
		*Member = converteddata;

		break;
	}
	case Format::INT32: {
		int32_t converteddata = std::stoi(line);
		uint32_t* Member = (uint32_t*)dst[*Index];
		*Member = converteddata;
		//memcpy(&dst, ((void**)Data)[*Index], sizeof(void*));
		//*(int32_t*)Data[*Index] = converteddata;


		break;
	}
	case Format::INT64: {
		int64_t converteddata = std::stoll(line);
		int64_t* Member = (int64_t*)dst[*Index];
		*Member = converteddata;


		break;
	}
	case Format::FLOAT: {
		float converteddata = std::stof(line);
		float* Member = (float*)dst[*Index];
		*Member = converteddata;
		//memcpy(&dst, ((void**)Data)[*Index], sizeof(void*));
		//*(int32_t*)Data[*Index] = converteddata;




		break;
	}
	case Format::DOUBLE: {
		double converteddata = std::stod(line);
		double* Member = (double*)dst[*Index];
		*Member = converteddata;


		break;
	}
	case Format::CHAR: {
		if (line.size() == 0)
			break;
		char converteddata = line[0];
		char* Member = (char*)dst[*Index];
		*Member = converteddata;


		break;
	}


	default: {
		std::cout << "No such format\n";
		return;
	}
	}
}

void Serializer::GetDynamicFormats(std::string* line, SerializerFormat format, Format* formats,uint32_t FormatCount)
{


	std::string temp = *line;
	uint64_t Offset{};
		Offset = format.keyword.size() + 1;
	for (uint32_t d = 0; d < FormatCount; d++) {
		if (d == FormatCount - 1) {
			temp = line->substr(Offset, line->find("\n", Offset) - Offset);
			uint32_t converteddata = std::stoul(temp);
			formats[d] = (Format)converteddata;
		}
		else {
			temp = line->substr(Offset, line->find(format.PaddingBetweenCount, Offset) - Offset);
			uint32_t converteddata = std::stoul(temp);
			formats[d] = (Format)converteddata;
			//return Data;
		}
		Offset += line->find(format.PaddingBetweenCount,Offset)-Offset+1;
	}
}

void Serializer::AllocateSpaceForDynamicArray(char* Data, uint32_t NumOffAlloc,uint64_t* Offset,Format format)
{
	switch (format) {
	case Format::STRING: {
		std::string* temp = new std::string[NumOffAlloc];
		for (uint32_t i = 0; i < NumOffAlloc; i++) {
			temp[i] = std::string();
		}
		memcpy((Data + *Offset), &temp, sizeof(std::string*));
		break;
	}
	case Format::UINT16: {
		uint16_t* temp = new uint16_t[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(uint16_t*));
		break;
	}
	case Format::UINT32: {
		uint32_t* temp = new uint32_t[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(uint32_t*));
		break;
	}
	case Format::UINT64: {
		uint64_t* temp = new uint64_t[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(uint64_t*));
		break;
	}
	case Format::INT16: {
		int16_t* temp = new int16_t[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(int16_t*));
		break;
	}
	case Format::INT32: {
		int32_t* temp = new int32_t[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(int32_t*));
		break;
	}
	case Format::INT64: {
		int64_t* temp = new int64_t[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(int64_t*));
		break;
	}
	case Format::FLOAT: {
		float* temp = new float[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(float*));
		break;
	}
	case Format::DOUBLE: {
		double* temp = new double[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(double*));
		break;
	}
	case Format::CHAR: {
		char* temp = new char[NumOffAlloc];
		memcpy((Data + *Offset), &temp, sizeof(char*));
		break;
	}



	default: {
		std::cout << "No such format\n";
		return;
	}
	}
}

void Serializer::AllocateSpaceForDynamicArrayCustomFormat(char* member, uint32_t NumOffAlloc, uint64_t* Offset, Format* format)
{
	void** address = (void**)member;
	*address = new void* [NumOffAlloc];
	void** arrayres = (void**)*address;


	for (uint32_t i = 0; i < NumOffAlloc; i++) {


		switch (format[i]) {
		case Format::STRING: {
			arrayres[i] = new std::string();
			break;
		}
		case Format::UINT16: {
			arrayres[i] = new uint16_t();

			break;
		}
		case Format::UINT32: {

			arrayres[i] = new uint32_t();

			break;
		}
		case Format::UINT64: {
			arrayres[i] = new uint64_t();

			break;
		}
		case Format::INT16: {
			arrayres[i] = new int16_t();

			break;
		}
		case Format::INT32: {
			arrayres[i] = new int32_t();

			break;
		}
		case Format::INT64: {
			arrayres[i] = new int64_t();

			break;
		}
		case Format::FLOAT: {
			arrayres[i] = new float();

			break;
		}
		case Format::DOUBLE: {
			arrayres[i] = new double();

			break;
		}
		case Format::CHAR: {
			arrayres[i] = new char();

			break;
		}



		default: {
			std::cout << "No such format\n";
			return;
		}
		}


	}
}

void Serializer::DynamicArrayType(void* data, uint64_t Offset, Format format, uint32_t Index)
{
	switch (format) {
	case Format::STRING: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((std::string*)address)[Index];
		break;
	}
	case Format::UINT16: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((uint16_t*)address)[Index];
		break;
	}
	case Format::UINT32: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((uint32_t*)address)[Index];
		break;
	}
	case Format::UINT64: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((uint64_t*)address)[Index];
		break;
	}
	case Format::INT16: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((int16_t*)address)[Index];
		break;
	}
	case Format::INT32: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((int32_t*)address)[Index];
		break;
	}
	case Format::INT64: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((int64_t*)address)[Index];
		break;
	}
	case Format::FLOAT: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((float*)address)[Index];
		break;
	}
	case Format::DOUBLE: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((double*)address)[Index];
		break;
	}
	case Format::CHAR: {
		uint64_t address = *(uint64_t*)((char*)data + Offset);
		m_OutputFile << ((char*)address)[Index];
		break;
	}
	



	default: {
		std::cout << "No such format\n";
		break;
	}
	}
}

void Serializer::DynamicArrayTypePP(void* data, uint64_t Offset, Format format, uint32_t Index)
{
	uint64_t addressnumber = *(uint64_t*)((char*)data + Offset);
	void* address = ((void**)addressnumber)[Index];
	switch (format) {
	case Format::STRING: {
		m_OutputFile << *((std::string*)address);
		break;
	}
	case Format::UINT16: {
		m_OutputFile << *((uint16_t*)address);
		break;
	}
	case Format::UINT32: {
		m_OutputFile << *((uint32_t*)address);
		break;
	}
	case Format::UINT64: {
		m_OutputFile << *((uint64_t*)address);
		break;
	}
	case Format::INT16: {
		m_OutputFile << *((int16_t*)address);
		break;
	}
	case Format::INT32: {
		m_OutputFile << *((int32_t*)address);
		break;
	}
	case Format::INT64: {
		m_OutputFile << *((int64_t*)address);
		break;
	}
	case Format::FLOAT: {
		m_OutputFile << *((float*)address);
		break;
	}
	case Format::DOUBLE: {
		m_OutputFile << *((double*)address);
		break;
	}
	case Format::CHAR: {
		m_OutputFile << *((char*)address);
		break;
	}




	default: {
		std::cout << "No such format\n";
		break;
	}
	}
}

void Serializer::StackArrayType(void* data, uint64_t Offset,Format format,uint32_t Index)
{
	switch (format) {
	case Format::STRING: {
		m_OutputFile << *(std::string*)((char*)data + Offset + (Index * sizeof(std::string)));
		break;
	}
	case Format::UINT16: {
		m_OutputFile << *(uint16_t*)((char*)data + Offset + (Index * sizeof(uint16_t)));
		break;
	}
	case Format::UINT32: {
		m_OutputFile << *(uint32_t*)((char*)data + Offset + (Index * sizeof(uint32_t)));
		break;
	}
	case Format::UINT64: {
		m_OutputFile << *(uint64_t*)((char*)data + Offset + (Index * sizeof(uint64_t)));
		break;
	}
	case Format::INT16: {
		m_OutputFile << *(int16_t*)((char*)data + Offset + (Index * sizeof(int16_t)));
		break;
	}
	case Format::INT32: {
		m_OutputFile << *(int32_t*)((char*)data + Offset + (Index * sizeof(int32_t)));
		break;
	}
	case Format::INT64: {
		m_OutputFile << *(int64_t*)((char*)data + Offset + (Index * sizeof(int64_t)));
		break;
	}
	case Format::FLOAT: {
		m_OutputFile << *(float*)((char*)data + Offset + (Index * sizeof(float)));
		break;
	}
	case Format::DOUBLE: {
		m_OutputFile << *(double*)((char*)data + Offset + (Index * sizeof(double)));
		break;
	}
	case Format::CHAR: {
		m_OutputFile << *(char*)((char*)data + Offset + (Index * sizeof(char)));
		break;
	}



	default: {
		std::cout << "No such format\n";
		break;
	}
	}
}
