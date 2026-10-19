#pragma once
#include "AppCore.h"
#include <stdio.h>

#ifdef LINUX
	#include <dlfcn.h>
#endif

#ifdef WINDOWS
	//windows.h or something
#endif


class DLLLoading
{
public:
	void Init(std::string DLLPath);
	template<typename T>
	T LoadFunction(std::string FunctionName);
	~DLLLoading();
private:
	void*  m_DLL{};
};

template<typename T>
inline T DLLLoading::LoadFunction(std::string FunctionName)
{
	#ifdef LINUX
	T proc = (T)dlsym(m_DLL, FunctionName.c_str());
	if (!proc)
		Core::Log(ErrorType::Error, "Failed to load function named:", FunctionName);

	return proc;
	#elif WINDOWS




	#endif
}
