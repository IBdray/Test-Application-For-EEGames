#pragma once
#include <functional>
#include <memory>

struct EventHandler;
class Node;

struct AuthorsData
{
	int Sum = 0;
	int Counter = 0;
	std::unique_ptr<EventHandler> HandlerPtr;

	AuthorsData() = default;
};

