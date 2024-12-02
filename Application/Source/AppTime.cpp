#include "AppTime.h"
#include <chrono>
double Time::GetTimeNs()
{
	auto now = std::chrono::high_resolution_clock::now();
	return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
}
double Time::GetTimeS()
{
	auto now = std::chrono::high_resolution_clock::now();
	return (double)std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}
double Time::GetTimeMs()
{
	auto now =std::chrono::high_resolution_clock::now();
	return (double)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}
