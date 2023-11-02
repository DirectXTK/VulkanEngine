#include "RandomGenerator.h"

std::random_device rd;
std::mt19937 engine(rd());
float Core::RandomFloat(float Min, float Max)
{
	std::uniform_real_distribution<float> dist(Min, Max);
	return dist(engine);
}
