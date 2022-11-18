#include "EventHandler.h"
#include <iostream>


void SumHandler::Handle(const Node& Author, const Node& Subscriber)
{
	std::cout << Author.GetName() << "->" << Subscriber.GetName() << ": S = " << Subscriber.NeighborsDataMap.at(Author.GetName()).Sum << std::endl;
}


void CountHandler::Handle(const Node& Author, const Node& Subscriber)
{
	std::cout << Author.GetName() << "->" << Subscriber.GetName() << ": N = " << Subscriber.NeighborsDataMap.at(Author.GetName()).EventCounter << std::endl;
}
