#include "EventHandler.h"
#include <iostream>


void SumHandler::Handle(const Node& Author, const Node& Subscriber)
{
	std::cout << Author.GetName() << "->" << Subscriber.GetName()
			  << ": S = " << Subscriber.GetNeighborsData().at(Author.GetName()).Sum << std::endl;
}


void CountHandler::Handle(const Node& Author, const Node& Subscriber)
{
	std::cout << Author.GetName() << "->" << Subscriber.GetName()
			  << ": N = " << Subscriber.GetNeighborsData().at(Author.GetName()).EventCounter << std::endl;
}
