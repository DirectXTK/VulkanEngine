#include "RandomGenerator.h"

std::random_device rd;
std::mt19937 engine(rd());
float Core::RandomFloat(float Min, float Max)
{
	std::uniform_real_distribution<float> dist(Min, Max);
	return dist(engine);
}

int32_t Core::RandomInt32(int32_t Min, int32_t Max)
{
	std::uniform_int_distribution<int32_t> dist(Min, Max);
	return dist(engine);
}

int64_t Core::RandomInt64(int64_t Min, int64_t Max)
{
	std::uniform_int_distribution<int64_t> dist(Min, Max);
	return dist(engine);
}

uint64_t Core::RandomUInt64(uint64_t min, uint64_t Max)
{
	std::uniform_int_distribution<uint64_t> dist(min, Max);
	return dist(engine);
}

uint32_t Core::RandomUInt32(uint32_t min, uint32_t Max)
{
	std::uniform_int_distribution<uint32_t> dist(min, Max);
	return dist(engine);
}
