// Copyright 2022 Ivan Babanov, IB Dray. No Rights Reserved.

#include <iostream>
#include <thread>
#include <chrono>

#include <Windows.h>

#include "Node.h"
#include "NodeEnums.h"
#include "ActionPreferences.h"
#include "RandomGenerator.h"



bool IsKeyPressed(unsigned TimeoutMS = 0);
void GenerateDefaultNodes();
void Cycle();
ActionPreferences SetupPreferences();
ActionPreferences SetupEqualPreferences();
ActionPreferences SetupDetailedPreferences();






bool IsKeyPressed(unsigned TimeoutMS)
{
	return WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), TimeoutMS) == WAIT_OBJECT_0;
}

void GenerateDefaultNodes()
{
	for (int i = 0; i < RandomGenerator::GenerateNumber(0, 20); ++i)
    {
	    const auto FirstLine = Node::Factory::CreateNode();
		for (int j = 0; j < RandomGenerator::GenerateNumber(0, 10); ++j)
		{
			const auto SecondLine = Node::Factory::CreateNeighborTo(FirstLine);
			for (int k = 0; k < RandomGenerator::GenerateNumber(0, 5); ++k)
			{
				const auto ThirdLine = Node::Factory::CreateNeighborTo(SecondLine);
			}
		}
    }
}

void Cycle()
{
	int CycleCounter = 0;

	while (!IsKeyPressed())
	{
		int NodesCounter = 0;
		for (const auto& NodePtr : Node::Manager::GetNodes())
		{
			if (NodePtr)
			{
				NodePtr->Update();
				NodesCounter += 1;

				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
		}

		std::cout << "\n ======= Cycle " << ++CycleCounter << " =======\n"
				  << "Number of remaining nodes: " << NodesCounter << std::endl << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

ActionPreferences SetupPreferences()
{
	std::cout << "You can make all action equally possible or enter specific chance to each." << std::endl;
	std::cout << "Make all actions equally possible? [Y/N]" << std::endl << std::endl;

	char Input;
	std::cin >> Input;

	if (Input == 'Y' || Input == 'y')
		return SetupEqualPreferences();
	else if (Input == 'N' || Input == 'n')
		return SetupDetailedPreferences();
	else
	{
		std::flush(std::cout);
		std::cout << "Incorrect input" << std::endl << std::endl;
		return SetupPreferences();
	}

}

ActionPreferences SetupEqualPreferences()
{
	std::flush(std::cout);
	std::cout << "Enter preferences for all actions:\n";

	int Val;
	std::cin >> Val;

	return ActionPreferences{static_cast<size_t>(Val)};
}

ActionPreferences SetupDetailedPreferences()
{
	ActionPreferences Preferences {0};
	int Val;

	std::flush(std::cout);
	std::cout << "Enter action preferences:\n\n";
	std::cout << "Chance to generate event: ";

	std::cin >> Val;
	Preferences.SetPreference(NodeActions::GenerateEvent, Val);

	std::flush(std::cout);
	std::cout << "Enter action preferences:\n\n";
	std::cout << "Chance to generate event: " << Preferences.GetPreference(NodeActions::GenerateEvent) << std::endl;
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: ";

	std::cin >> Val;
	Preferences.SetPreference(NodeActions::SubscribeToNeighbor, Val);

	std::flush(std::cout);
	std::cout << "Enter action preferences:\n\n";
	std::cout << "Chance to generate event: " << Preferences.GetPreference(NodeActions::GenerateEvent) << std::endl;
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: " << Preferences.GetPreference(NodeActions::SubscribeToNeighbor) << std::endl;
	std::cout << "Chance to unsubscribe from random neighbor: ";

	std::cin >> Val;
	Preferences.SetPreference(NodeActions::UnsubscribeFromNeighbor, Val);

	std::flush(std::cout);
	std::cout << "Enter action preferences:\n\n";
	std::cout << "Chance to generate event: " << Preferences.GetPreference(NodeActions::GenerateEvent) << std::endl;
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: " << Preferences.GetPreference(NodeActions::SubscribeToNeighbor) << std::endl;
	std::cout << "Chance to unsubscribe from random neighbor: " << Preferences.GetPreference(NodeActions::UnsubscribeFromNeighbor) << std::endl;
	std::cout << "Chance to create new neighbor node: ";

	std::cin >> Val;
	Preferences.SetPreference(NodeActions::GenerateNewNeighbor, Val);

	std::flush(std::cout);
	std::cout << "Enter action preferences:\n\n";
	std::cout << "Chance to generate event: " << Preferences.GetPreference(NodeActions::GenerateEvent) << std::endl;
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: " << Preferences.GetPreference(NodeActions::SubscribeToNeighbor) << std::endl;
	std::cout << "Chance to unsubscribe from random neighbor: " << Preferences.GetPreference(NodeActions::UnsubscribeFromNeighbor) << std::endl;
	std::cout << "Chance to create new neighbor node: " << Preferences.GetPreference(NodeActions::GenerateNewNeighbor) << std::endl;
	std::cout << "Chance to skip update: ";

	std::cin >> Val;
	Preferences.SetPreference(NodeActions::Sleep, Val);

	std::flush(std::cout);
	std::cout << "Enter action preferences:\n\n";
	std::cout << "Chance to generate event: " << Preferences.GetPreference(NodeActions::GenerateEvent) << std::endl;
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: " << Preferences.GetPreference(NodeActions::SubscribeToNeighbor) << std::endl;
	std::cout << "Chance to unsubscribe from random neighbor: " << Preferences.GetPreference(NodeActions::UnsubscribeFromNeighbor) << std::endl;
	std::cout << "Chance to create new neighbor node: " << Preferences.GetPreference(NodeActions::GenerateNewNeighbor) << std::endl;
	std::cout << "Chance to skip update: " << Preferences.GetPreference(NodeActions::Sleep) << std::endl;

	return Preferences;
}





int main()
{
	std::cout << "App created by Ivan Babanov, IB Dray for EEGames" << std::endl;


	Node::SetPreferences(SetupPreferences());


	GenerateDefaultNodes();



	Cycle();

	

    std::cin.get();
}
