#include "../public/RandomGenerator.h"
#include <random>

int RandomGenerator::GenerateNumber(const int Min, const int Max)
{
	std::random_device RandomDevice;
	std::mt19937 Rand(RandomDevice());
	std::uniform_int_distribution<int> RandomDistribution(Min, Max);
	
	return RandomDistribution(Rand);
}
