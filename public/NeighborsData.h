#pragma once
#include <functional>

struct EventHandler;
class Node;

struct NeighborsData
{
	int Sum = 0;
	int Counter = 0;
	std::function<void(const Node& Author, const Node& Subscriber)> Handler;

	NeighborsData() = default;
};

