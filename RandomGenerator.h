#pragma once
#include <random>


struct RandomGenerator
{
	template<typename T = size_t>
	static T GenerateNumber(T Min = 0, T Max = 100)
	{
		std::random_device RandomDevice;
		std::mt19937 Rand(RandomDevice());
		std::uniform_int_distribution<T> RandomDistribution(Min, Max);

		return RandomDistribution(Rand());
	}
	

};

