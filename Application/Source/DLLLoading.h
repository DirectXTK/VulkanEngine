#pragma once
#include "AppCore.h"
#include <stdio.h>

#ifdef LINUX
	#include <dlfcn.h>
#endif

#ifdef WINDOWS
	//windows.h or something
#endif

namespace Core{
//Lazy means class is deleted then all refs to fuctions are deconstructed
class DLLLazy;

template<typename T>
class DLLFunc{
	public:
		DLLFunc(){};
		#ifdef DEBUG
		DLLFunc(DLLLazy* loader,T func,const std::string& funcName);
		#endif
		DLLFunc(DLLLazy* loader,T func);
		DLLFunc(const DLLFunc& other);

		DLLFunc& operator=(const DLLFunc& other);
		template<typename...ARGS>
		decltype(auto) operator()(ARGS...args){
				if(m_Func)
			return m_Func(args...);
		}
		~DLLFunc();
	private:
		#ifdef DEBUG
		std::string m_FuncName{"UNSPECIFIED"};
		#endif
		T m_Func{};
		DLLLazy* m_Loader{};
};

class DLLLazy
{
public:
	void Init(std::string DLLPath);
	template<typename T>
	DLLFunc<T> LoadFunction(std::string FunctionName);
	~DLLLazy(){}
protected:
	template<typename T>
	friend class DLLFunc;
	void ReleaseFunction();
	void InitFunction();
private:
	void ShutDown();
	void*  m_DLL{};
	std::atomic<uint32_t> m_References{};
	bool m_Invalid{false};
};
template<typename T>
DLLFunc<T>::DLLFunc(const DLLFunc& other){
	this = other;
	Core::Log("Copied const");
	if(m_Loader)
		m_Loader->InitFunction();
}
template<typename T>
DLLFunc<T>& DLLFunc<T>::operator=(const DLLFunc& other){
	if(this == &other)
		return *this;
	this->m_Func = other.m_Func;
	this->m_Loader = other.m_Loader;
	if(this->m_Loader)
		this->m_Loader->InitFunction();
	return *this;
}
template<typename T>
inline DLLFunc<T> DLLLazy::LoadFunction(std::string FunctionName)
{
	#ifdef LINUX
	T proc = (T)dlsym(m_DLL, FunctionName.c_str());
	if (!proc)
		Core::Log(ErrorType::Error, "Failed to load function named:", FunctionName);

	#ifdef DEBUG
		return DLLFunc<T>(this,proc,FunctionName);
	#else
		return DLLFunc<T>(this,proc);
	#endif

	#elif WINDOWS




	#endif
}
template<typename T>
inline DLLFunc<T>::~DLLFunc(){
	Core::Log(ErrorType::Info,"Destroyed function",m_FuncName);
	if(m_Loader)
		m_Loader->ReleaseFunction();
}
template<typename T>
DLLFunc<T>::DLLFunc(DLLLazy* loader,T func,const std::string& funcName): m_Loader(loader), m_Func(func),m_FuncName(funcName){
	m_Loader->InitFunction();
}
template<typename T>
DLLFunc<T>::DLLFunc(DLLLazy* loader,T func): m_Loader(loader),m_Func(func){
	m_Loader->InitFunction();
}

class DLL
{
public:
	void Init(std::string DLLPath);
	template<typename T>
	T LoadFunction(std::string FunctionName);
	~DLL();
private:
	void*  m_DLL{};
};

template<typename T>
inline T DLL::LoadFunction(std::string FunctionName)
{
	#ifdef LINUX
	T proc = (T)dlsym(m_DLL, FunctionName.c_str());
	if (!proc)
		Core::Log(ErrorType::Error, "Failed to load function named:", FunctionName);

	return proc;
	#elif WINDOWS




	#endif
}
}
