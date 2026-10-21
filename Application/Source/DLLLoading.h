#pragma once

#ifdef LINUX
	#include <dlfcn.h>
#endif
#ifdef WINDOWS
	#include <Windows.h>
#endif
#include <windows.h>
#include "AppCore.h"
#include <stdio.h>
class DLLLoading
{
public:
	void Init(std::string DLLPath);
	template<typename T>
	T LoadFunction(std::string FunctionName);
	~DLLLoading();
private:
#ifdef LINUX
	void*  m_DLL{};
#endif
#ifdef WINDOWS
	HMODULE m_DLL = nullptr;
#endif
};
#ifdef LINUX
template<typename T>
inline T DLLLoading::LoadFunction(std::string FunctionName)
{
	T proc = (T)dlsym(m_DLL, FunctionName.c_str());
	if (!proc)
		Core::Log(ErrorType::Error, "Failed to load function named:", FunctionName);

	return proc;
}
#endif
#ifdef WINDOWS
template<typename T>
inline T DLLLoading::LoadFunction(std::string FunctionName)
{

	T proc = (T)GetProcAddress(m_DLL,FunctionName.c_str());
	if (!proc)
		Core::Log(ErrorType::Error, "Failed to load function named:", FunctionName);

	return proc;
}
#endif
