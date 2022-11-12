// Copyright 2022 Ivan Babanov, IB Dray. No Rights Reserved.

#include <iostream>
#include <memory>

#include "Node.h"

int main()
{
    auto n1 = Node::Create();
    auto n2 = n1->GenerateNewNeighbor();
    n2->CreateEvent();

    std::cin.get();
}
