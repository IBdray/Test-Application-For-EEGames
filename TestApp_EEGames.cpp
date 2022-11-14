// Copyright 2022 Ivan Babanov, IB Dray. No Rights Reserved.

#include <iostream>
#include <memory>

#include "Node.h"


int main()
{
    auto n1 = Node::Create();
    auto n2 = n1->GenerateNewNeighbor();

	for (int i = 0; i < 500; ++i)
	{
	    for (const auto& NodeTemp : n1->GetNeighbors())
		{
		    if (!NodeTemp.expired())
		        NodeTemp.lock()->Update();
		}
		for (const auto& NodeTemp : n2->GetNeighbors())
		{
		    if (!NodeTemp.expired())
		        NodeTemp.lock()->Update();
		}
	}


    std::cin.get();
}
