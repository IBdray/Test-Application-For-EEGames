#include "../public/EventHandler.h"
#include <iostream>


void SumHandler::Handle(const Node& Author, const Node& Subscriber)
{
	std::cout << Author.GetName() << "->" << Subscriber.GetName()
			  << ": S = " << Subscriber.GetAuthorsData().at(Author.GetName()).Sum << std::endl;
}


void CountHandler::Handle(const Node& Author, const Node& Subscriber)
{
	std::cout << Author.GetName() << "->" << Subscriber.GetName()
			  << ": N = " << Subscriber.GetAuthorsData().at(Author.GetName()).Counter << std::endl;
}
