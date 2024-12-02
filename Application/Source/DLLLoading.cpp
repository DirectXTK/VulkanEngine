#include "DLLLoading.h"
void DLLLoading::Init(std::string DLLPath)
{
	size_t Count = DLLPath.size();
	wchar_t* WideChar = new wchar_t[DLLPath.size()+1];

	//mbstowcs(WideChar, DLLPath.c_str(), DLLPath.size());
	mbstowcs_s(&Count, WideChar,Count, DLLPath.c_str(), (size_t)1500);

	WideChar[DLLPath.size() ] = '\0';
	m_DLL = LoadLibrary(WideChar);
	if (!m_DLL) {
		Core::Log(ErrorType::Error, "Failed to load dll.");
	}
}

DLLLoading::~DLLLoading()
{
	FreeLibrary(m_DLL);
}

