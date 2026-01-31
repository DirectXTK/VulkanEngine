#include "AppCore.h"
namespace Core {
        ScopedTimer::ScopedTimer(){
			auto now = std::chrono::system_clock::now();
			m_Start = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		}
		ScopedTimer::~ScopedTimer(){
			auto now = std::chrono::system_clock::now();
			uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
			Core::Log("This function took:", (end-m_Start)/1000.f," ms");
		}
		void Timer::Start(){
			auto now = std::chrono::system_clock::now();
			m_Start = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		}
		uint64_t Timer::End(){
			auto now = std::chrono::system_clock::now();
			uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
			end = end-m_Start;
			Core::Log("This function took:", end/1000.f," ms");
			return end;
		}
	
	GUUID GetStringHash(const std::string& Path) {return (uint64_t)std::hash<std::string>{}(Path); }
   
	std::string GetFileExtension(const std::string& File)
	{
		uint64_t LocOfDot = File.find(".");
		if (LocOfDot == (uint64_t)-1)
			return std::string();
		return File.substr(LocOfDot+1, File.size() - LocOfDot+1);
	}
	bool IsWithinRectRegion(const Float2& PointPos, const Float2& ObjPos, const Float2& ObjSize)
	{
			if (PointPos.y >= ObjPos.y - ObjSize.y && PointPos.y <= ObjPos.y + ObjSize.y) {
				if (PointPos.x >= ObjPos.x - ObjSize.x && PointPos.x <= ObjPos.x + ObjSize.x) {
					return true;
				}
			}
			return false;
	}
	bool IsWithinRegionOrg(const Float2& PointPos, Float2 Region[4])
	{
			if (PointPos.y >= Region[0].y && PointPos.y <= Region[1].y) {
				if (PointPos.x >= Region[0].x && PointPos.x <= Region[3].x) {
					return true;
				}
			}
			return false;
	}
	  uint64_t Align(uint64_t Number,uint64_t Aligment){
    return (Number +Aligment-1)& ~(Aligment - 1);
}

   float CalculateDistanceFast(const Float2& pos1,const Float2& pos2){
		return std::fabs(pos1.x-pos2.x)+std::fabs(pos1.y-pos2.y);
   }
    float CalculateDistance(const Float2& pos1,const Float2& pos2){
		return std::sqrt(std::pow(pos1.x-pos2.x,2)+std::pow(pos1.y-pos2.y,2));
	}


 std::string GetCurrentExecutablePath(){
	
	    #if defined(_WIN32)
	    char path[MAX_PATH];
	    GetModuleFileNameA(nullptr, path, MAX_PATH);
	    return wstd::string(path);
	#elif defined(__linux__)
	   char buffer[PATH_MAX];
	    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	    if (count == -1) {
	        return "";
	    }
	    buffer[count] = '\0';
	
	    std::string narrowPath(buffer);
	    return narrowPath;
	#elif defined(__APPLE__)
	    char path[PATH_MAX];
	    uint32_t size = sizeof(path);
	    if (_NSGetExecutablePath(path, &size) == 0) {
	        return std::string(path);
	    }
	    return {};
	#else
	    return {};
	#endif
}



void cpuid(int out[4], int x)
{
#ifdef _MSC_VER
    __cpuid(out, x);
#else
    __cpuid(x, out[0], out[1], out[2], out[3]);
#endif
}

bool has_xsave_xrstore()
{
    int regs[4];
    cpuid(regs, 1);
    return (regs[2] & (1 << 27)) != 0; // XSAVE/XRSTOR
}

bool os_avx_support()
{
#ifdef _MSC_VER
    return (_xgetbv(0) & 0x6) == 0x6;
#else
    unsigned int eax, edx;
    __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
    return ((eax & 0x6) == 0x6);
#endif
}

#ifdef __linux__
std::string GetCPUName(){
    std::ifstream input("/proc/cpuinfo");
    std::string line{};

    while(std::getline(input,line)){
        if(line.find("model name") != std::string::npos){
            return line;
        }
    }
    return "Unknown CPU";
}
uint32_t GetCPUCoreCount(){
	std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;

    int logical = 0;
    std::set<int> cores;

    int coreID = -1, physicalID = -1;

    while (std::getline(cpuinfo, line)) {
        if (line.find("processor") != std::string::npos)
            logical++;

        if (line.find("core id") != std::string::npos)
            coreID = std::stoi(line.substr(line.find(":") + 2));

        if (line.find("physical id") != std::string::npos)
            physicalID = std::stoi(line.substr(line.find(":") + 2));

        if (coreID != -1 && physicalID != -1) {
            cores.insert(coreID + physicalID * 1000);
            coreID = -1;
            physicalID = -1;
        }
    }
	return cores.size();
}
uint32_t GetCPUThreadCount(){
	std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;

    int logical = 0;
    std::set<int> cores;

    int coreID = -1, physicalID = -1;

    while (std::getline(cpuinfo, line)) {
        if (line.find("processor") != std::string::npos)
            logical++;

        if (line.find("core id") != std::string::npos)
            coreID = std::stoi(line.substr(line.find(":") + 2));

        if (line.find("physical id") != std::string::npos)
            physicalID = std::stoi(line.substr(line.find(":") + 2));

        if (coreID != -1 && physicalID != -1) {
            cores.insert(coreID + physicalID * 1000);
            coreID = -1;
            physicalID = -1;
        }
    }
	return logical;
}

std::string GetGPUName(){
	FILE* pipe = popen("lspci | grep -E 'VGA|3D' | head -n 1", "r");
    if (!pipe) return "Unknown GPU";

    char buffer[256];
    std::string result;

    if (fgets(buffer, sizeof(buffer), pipe))
        result = buffer;

    pclose(pipe);

    return result;
}
CPUFeatures GetCPUSupportedInscrutionSets(){
    int regs[4];
    CPUFeatures f;

    cpuid(regs, 1);

    f.SSE   = regs[3] & (1 << 25);
    f.SSE2  = regs[3] & (1 << 26);
    f.SSE3  = regs[2] & (1 << 0);
    f.SSSE3 = regs[2] & (1 << 9);
    f.SSE41 = regs[2] & (1 << 19);
    f.SSE42 = regs[2] & (1 << 20);

    bool osAVXReady = has_xsave_xrstore() && os_avx_support();
    f.AVX = osAVXReady && (regs[2] & (1 << 28));

    cpuid(regs, 7);
    f.AVX2 = osAVXReady && (regs[1] & (1 << 5));

    f.AVX512F = osAVXReady && (regs[1] & (1 << 16));
    f.AVX512DQ = osAVXReady && (regs[1] & (1 << 17));
    f.AVX512CD = osAVXReady && (regs[1] & (1 << 28));
    f.AVX512BW = osAVXReady && (regs[1] & (1 << 30));
    f.AVX512VL = osAVXReady && (regs[1] & (1 << 31));

    return f;
}
uint64_t GetTotalAmountOfRam(){
	std::ifstream meminfo("/proc/meminfo");
    std::string key;
    unsigned long long value;
    std::string unit;

    while (meminfo >> key >> value >> unit) {
        if (key == "MemTotal:") {
            return value * 1024ULL; // kB → bytes
        }
    }

    return 0;
}
#elif __win32__
std::string GetCPUName(){
    int cpuInfo[4] = {0};
    char cpuBrand[0x40] = {};

    __cpuid(cpuInfo, 0x80000000);
    unsigned int maxExt = cpuInfo[0];

    if (maxExt >= 0x80000002) {
        __cpuid((int*)(cpuBrand +  0), 0x80000002);
        __cpuid((int*)(cpuBrand + 16), 0x80000003);
        __cpuid((int*)(cpuBrand + 32), 0x80000004);
    }

    return std::string(cpuBrand);
}
uint32_t GetCPUCoreCount(){
	SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD cores = 0;
    DWORD bytes = 0;

    GetLogicalProcessorInformation(nullptr, &bytes);

    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(bytes / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
    GetLogicalProcessorInformation(buffer.data(), &bytes);

    for (auto &info : buffer) {
        if (info.Relationship == RelationProcessorCore)
            cores++;
    }
	return cores;
}
uint32_t GetCPUThreadCount(){
	SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}
std::string GetGPUName(){
	IDXGIFactory1* factory = nullptr;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
        return "Unknown GPU";

    IDXGIAdapter1* adapter = nullptr;
    if (FAILED(factory->EnumAdapters1(0, &adapter))) {
        factory->Release();
        return "Unknown GPU";
    }

    DXGI_ADAPTER_DESC1 desc;
    adapter->GetDesc1(&desc);

    // Convert wide char to UTF-8
    char name[128];
    wcstombs(name, desc.Description, 128);

    adapter->Release();
    factory->Release();

    return std::string(name);
}
uint64_t GetTotalAmountOfRam(){
	  MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);

    GlobalMemoryStatusEx(&status);

    return status.ullTotalPhys; // bytes
}
#endif

#ifdef DEBUG
std::string GetBuildConfiguration(){return "DEBUG";}
#elif RELEASE
std::string GetBuildConfiguration(){return "RELEASE";}
#elif DISTRIB
std::string GetBuildConfiguration(){return "DISTRIB";}
#endif


}