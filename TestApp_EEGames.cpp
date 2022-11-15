// Copyright 2022 Ivan Babanov, IB Dray. No Rights Reserved.

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <windows.h>
#include <random>

#include "Node.h"


bool IsKeyPressed(unsigned TimeoutMS = 0)
{
	return WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), TimeoutMS) == WAIT_OBJECT_0;
}

size_t GenerateRandomNumber(const size_t Min = 0, const size_t Max = 1000)
{
	std::random_device RandomDevice;
	std::mt19937 Rand(RandomDevice());
	std::uniform_int_distribution<size_t> RandomDistribution(Min, Max);

	return RandomDistribution(Rand);
}

int main()
{
    for (int i = 0; i < GenerateRandomNumber(0, 20); ++i)
    {
	    auto N = Node::Create();
		for (int i = 0; i < GenerateRandomNumber(0, 10); ++i)
		{
			auto NN = N->GenerateNewNeighbor();
			for (int i = 0; i < GenerateRandomNumber(0, 5); ++i)
			{
				auto NNN = NN->GenerateNewNeighbor();
			}
		}
    }
	


	int CycleCounter = 0;

	while (!IsKeyPressed())
	{
		int Size = 0;
		for (const auto Temp : Node::Nodes)
		{
			if (Temp)
			{
				Temp->Update();
				Size += 1;
			}
		}

		std::cout << "\n ======= Cycle " << ++CycleCounter << " =======\n"
				  << "\tNumber of nodes: " << Size << std::endl << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

    std::cin.get();
}
