#include "DLLLoading.h"
void DLLLoading::Init(std::string DLLPath)
{
	size_t Count = DLLPath.size();
	wchar_t* WideChar = new wchar_t[DLLPath.size()+1];

	//mbstowcs(WideChar, DLLPath.c_str(), DLLPath.size());
	m_DLL =dlopen(DLLPath.c_str(),RTLD_LAZY);

	if (!m_DLL) {
		Core::Log(ErrorType::Error, "Failed to load dll.");
	}
}

DLLLoading::~DLLLoading()
{
	dlclose(m_DLL);
}

