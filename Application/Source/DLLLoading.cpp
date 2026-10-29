#include "DLLLoading.h"
namespace Core{


void DLLLazy::InitFunction(){
	if(m_Invalid)
		return;
	m_References++;
}
void DLLLazy::ReleaseFunction(){
	m_References--;
	if(m_References ==0){
		m_Invalid = true;
		ShutDown();
		return;
	}


}
void DLLLazy::ShutDown(){
	#ifdef LINUX

	#elif WINDOWS

	#endif
	if(m_DLL)
		dlclose(m_DLL);
	m_DLL = nullptr;
}
void DLLLazy::Init(std::string DLLPath){
	size_t Count = DLLPath.size();
	wchar_t* WideChar = new wchar_t[DLLPath.size()+1];

	int flag{};
	#ifdef DEBUG
		flag = RTLD_NOW;
	#else
		flag = RTLD_LAZY;
	#endif
	m_DLL =dlopen(DLLPath.c_str(),RTLD_LAZY);

	if (!m_DLL) {
		Core::Log(ErrorType::Error, "Failed to load dll.");
	}
}
DLLLazy::~DLLLazy(){
	#ifdef DEBUG
	if(m_References !=0)
		Core::Log(ErrorType::Error,"DLLLazy was destroyed using destructor it means not all functions got released.");
	#endif
	if(m_DLL)
		dlclose(m_DLL);
}



void DLL::Init(std::string DLLPath)
{
	size_t Count = DLLPath.size();
	wchar_t* WideChar = new wchar_t[DLLPath.size()+1];

	int flag{};
	#ifdef DEBUG
		flag = RTLD_NOW;
	#else
		flag = RTLD_LAZY;
	#endif
	m_DLL =dlopen(DLLPath.c_str(),RTLD_LAZY);

	if (!m_DLL) {
		Core::Log(ErrorType::Error, "Failed to load dll.");
	}
}

DLL::~DLL()
{
	dlclose(m_DLL);
}

}