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


 std::string GetModuleFileName(){
	
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

}