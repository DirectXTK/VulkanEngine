#include "AppCore.h"
namespace Core {
	GUUID GetStringHash(const std::string& Path) { return (uint64_t)std::hash<std::string>{}(Path); }

	std::string GetFileExtension(const std::string& File)
	{
		uint64_t LocOfDot = File.find(".");
		if (LocOfDot == (uint64_t)-1)
			return std::string();
		return File.substr(LocOfDot+1, File.size() - LocOfDot+1);
	}
}