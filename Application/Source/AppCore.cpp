#include "AppCore.h"
GUUID GetResourceHandle(std::string Path) { return (uint64_t)std::hash<std::string>{}(Path); }