#pragma once
#include "AppCore.h"
#include <Windows.h>
class DLLLoading
{
public:
	void Init(std::string DLLPath);
	template<typename T>
	T LoadFunction(std::string FunctionName);
	~DLLLoading();
private:
	HMODULE m_DLL{};
};

template<typename T>
inline T DLLLoading::LoadFunction(std::string FunctionName)
{
	T proc = (T)GetProcAddress(m_DLL, FunctionName.c_str());
	if (!proc)
		Core::Log(ErrorType::Error, "Failed to load function named:", FunctionName);

	return proc;
}
