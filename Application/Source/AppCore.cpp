#include "AppCore.h"
namespace Core {
	GUUID GetStringHash(std::string Path) { return (uint64_t)std::hash<std::string>{}(Path); }

	std::string GetFileExtension(const std::string& File)
	{
		uint64_t LocOfDot = File.find(".")+1;
		return File.substr(LocOfDot, File.size() - LocOfDot);
	}
}