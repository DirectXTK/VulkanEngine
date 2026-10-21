#include "DLLLoading.h"

#ifdef LINUX
void DLLLoading::Init(std::string DLLPath)
{
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
#endif
#ifdef WINDOWS
void DLLLoading::Init(std::string DLLPath)
{
	wchar_t* WideChar = new wchar_t[DLLPath.size()+1];

	m_DLL = LoadLibraryW(WideChar);

	if (!m_DLL) {
		Core::Log(ErrorType::Error, "Failed to load dll.");
	}
}

DLLLoading::~DLLLoading()
{
	FreeLibrary(m_DLL);
}
#endif
