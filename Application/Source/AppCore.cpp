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
}